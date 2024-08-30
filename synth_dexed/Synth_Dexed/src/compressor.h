/* From https://github.com/chipaudette/OpenAudio_ArduinoLibrary */

/*
   AudioEffectCompressor

   Created: Chip Audette, Dec 2016 - Jan 2017
   Purpose; Apply dynamic range compression to the audio stream.
            Assumes floating-point data.

   This processes a single stream fo audio data (ie, it is mono)

   MIT License.  use at your own risk.
*/

#ifndef _COMPRESSOR_H
#define _COMPRESSOR_H

#ifndef TEENSYDUINO
#ifndef RASPBERRYPI_PICO
#include <arm_math.h> //ARM DSP extensions.  https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html
#include "synth.h"
#include "./dexed_if_common.h"

/*
static const float zeroblock_f32[] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
#if AUDIO_BLOCK_SAMPLES > 16
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
#endif
#if AUDIO_BLOCK_SAMPLES > 32
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
#endif
#if AUDIO_BLOCK_SAMPLES > 48
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
#endif
#if AUDIO_BLOCK_SAMPLES > 64
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
#endif
#if AUDIO_BLOCK_SAMPLES > 80
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
#endif
#if AUDIO_BLOCK_SAMPLES > 96
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
#endif
#if AUDIO_BLOCK_SAMPLES > 112
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
#endif
#if AUDIO_BLOCK_SAMPLES > 128
#error AUDIO_BLOCK_SAMPLES > 128 is a problem for this class
#endif
};
*/

class Compressor
{
  //GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
  public:
    //constructor
    Compressor(const float sample_rate_Hz) {
	  //setDefaultValues(AUDIO_SAMPLE_RATE);   resetStates();
	  setDefaultValues(sample_rate_Hz);
          resetStates();
    };
	
    void setDefaultValues(const float sample_rate_Hz) {
      setThresh_dBFS(-20.0f);     //set the default value for the threshold for compression
      setCompressionRatio(5.0f);  //set the default copression ratio
      setAttack_sec(0.005f, sample_rate_Hz);  //default to this value
      setRelease_sec(0.200f, sample_rate_Hz); //default to this value
      setHPFilterCoeff();  enableHPFilter(true);  //enable the HP filter to remove any DC offset from the audio
    }


    //here's the method that does all the work
    void doCompression(float *audio_block, uint16_t len) {
      //Serial.println("AudioEffectGain_F32: updating.");  //for debugging.
      if (!audio_block) {
        printf("No audio_block available for Compressor!\n");
        return;
      }

      //apply a high-pass filter to get rid of the DC offset
      if (use_HP_prefilter)
        arm_biquad_cascade_df1_f32(&hp_filt_struct, audio_block, audio_block, len);
      
      //apply the pre-gain...a negative gain value will disable
      if (pre_gain > 0.0f)
        arm_scale_f32(audio_block, pre_gain, audio_block, len); //use ARM DSP for speed!

      //calculate the level of the audio (ie, calculate a smoothed version of the signal power)
      float* audio_level_dB_block = new float[len]; 
      if(!audio_level_dB_block)
      {
        printf("Cannot allocate memory for \"audio_level_dB_block\" - stopping\n");
        while(1);
      }

      //arm_copy_f32(zeroblock_f32,audio_level_dB_block,len);

      if(audio_level_dB_block)
      	calcAudioLevel_dB(audio_block, audio_level_dB_block, len); //returns through audio_level_dB_block

      //compute the desired gain based on the observed audio level
      float* gain_block=new float[len]; 
      if(!gain_block)
      {
        printf("Cannot allocate memory for \"gain_block\" - stopping\n");
        while(1);
      }

      //arm_copy_f32(zeroblock_f32,gain_block,len);

      if(gain_block)
      {
      	calcGain(audio_level_dB_block, gain_block, len);  //returns through gain_block

        //apply the desired gain...store the processed audio back into audio_block
        arm_mult_f32(audio_block, gain_block, audio_block, len);
      }

      //release memory
      if(audio_level_dB_block)
	delete[] audio_level_dB_block;
      if(gain_block)
        delete[] gain_block;
    }

    // Here's the method that estimates the level of the audio (in dB)
    // It squares the signal and low-pass filters to get a time-averaged
    // signal power.  It then 
    void calcAudioLevel_dB(float *wav_block, float *level_dB_block, uint16_t len) { 
    	
      // calculate the instantaneous signal power (square the signal)
      float* wav_pow_block=new float[len]; 
      if(!wav_pow_block)
      {
        printf("Cannot allocate memory for \"wav_pow_block\" - stopping\n");
        while(1);
      }

      //arm_copy_f32(zeroblock_f32,wav_pow_block,len);

      arm_mult_f32(wav_block, wav_block, wav_pow_block, len);

      // low-pass filter and convert to dB
      float c1 = level_lp_const, c2 = 1.0f - c1; //prepare constants
      for (uint16_t i = 0; i < len; i++) {
        // first-order low-pass filter to get a running estimate of the average power
        wav_pow_block[i] = c1*prev_level_lp_pow + c2*wav_pow_block[i];
        
        // save the state of the first-order low-pass filter
        prev_level_lp_pow = wav_pow_block[i]; 

        //now convert the signal power to dB (but not yet multiplied by 10.0)
        level_dB_block[i] = log10f_approx(wav_pow_block[i]);
      }

      //limit the amount that the state of the smoothing filter can go toward negative infinity
      if (prev_level_lp_pow < (1.0E-13)) prev_level_lp_pow = 1.0E-13;  //never go less than -130 dBFS 

      //scale the wav_pow_block by 10.0 to complete the conversion to dB
      arm_scale_f32(level_dB_block, 10.0f, level_dB_block, len); //use ARM DSP for speed!

      //release memory and return
      if(wav_pow_block)
        delete[] wav_pow_block;

      return; //output is passed through level_dB_block
    }

    //This method computes the desired gain from the compressor, given an estimate
    //of the signal level (in dB)
    void calcGain(float *audio_level_dB_block, float *gain_block,uint16_t len) { 
    
      //first, calculate the instantaneous target gain based on the compression ratio
      float* inst_targ_gain_dB_block=new float[len];
      if(!inst_targ_gain_dB_block)
      {
	printf("Cannot allocate memory for \"inst_targ_gain_dB_block\" - stopping\n");
	while(1);
      }
      //arm_copy_f32(zeroblock_f32,inst_targ_gain_dB_block,len);

      calcInstantaneousTargetGain(audio_level_dB_block, inst_targ_gain_dB_block,len);
    
      //second, smooth in time (attack and release) by stepping through each sample
      float *gain_dB_block = new float[len]; 
      if(!gain_dB_block)
      {
	printf("Cannot allocate memory for \"gain_dB_block\" - stopping\n");
	while(1);
      }
      //arm_copy_f32(zeroblock_f32,gain_dB_block,len);

      calcSmoothedGain_dB(inst_targ_gain_dB_block,gain_dB_block, len);

      //finally, convert from dB to linear gain: gain = 10^(gain_dB/20);  (ie this takes care of the sqrt, too!)
      arm_scale_f32(gain_dB_block, 1.0f/20.0f, gain_dB_block, len);  //divide by 20 
      for (uint16_t i = 0; i < len; i++) gain_block[i] = pow10f(gain_dB_block[i]); //do the 10^(x)
      

      //release memory and return
      if(inst_targ_gain_dB_block)
        delete[] inst_targ_gain_dB_block;
      if(gain_dB_block)
        delete[] gain_dB_block;

      return;  //output is passed through gain_block
    }
      
    //Compute the instantaneous desired gain, including the compression ratio and
    //threshold for where the comrpession kicks in
    void calcInstantaneousTargetGain(float *audio_level_dB_block, float *inst_targ_gain_dB_block, uint16_t len) {
      
      // how much are we above the compression threshold?
      float* above_thresh_dB_block=new float[len];
      if(!above_thresh_dB_block)
      {
        printf("Cannot allocate memory for \"above_thresh_dB_block\" - stopping\n");
        while(1);
      }

      //arm_copy_f32(zeroblock_f32,above_thresh_dB_block,len);

      arm_offset_f32(audio_level_dB_block,  //CMSIS DSP for "add a constant value to all elements"
        -thresh_dBFS,                         //this is the value to be added
        above_thresh_dB_block,          //this is the output
        len);  

      // scale by the compression ratio...this is what the output level should be (this is our target level)
      arm_scale_f32(above_thresh_dB_block,    //CMSIS DSP for "multiply all elements by a constant value"
           1.0f / comp_ratio,                       //this is the value to be multiplied 
           inst_targ_gain_dB_block,           //this is the output
           len); 

      // compute the instantaneous gain...which is the difference between the target level and the original level
      arm_sub_f32(inst_targ_gain_dB_block,  //CMSIS DSP for "subtract two vectors element-by-element"
           above_thresh_dB_block,           //this is the vector to be subtracted
           inst_targ_gain_dB_block,         //this is the output
           len);

      // limit the target gain to attenuation only (this part of the compressor should not make things louder!)
      for (uint16_t i=0; i < len; i++) {
        if (inst_targ_gain_dB_block[i] > 0.0f) inst_targ_gain_dB_block[i] = 0.0f;
      }

      // release memory before returning
      if(above_thresh_dB_block)
        delete[] above_thresh_dB_block;

      return;  //output is passed through inst_targ_gain_dB_block
    }

    //this method applies the "attack" and "release" constants to smooth the
    //target gain level through time.
    void calcSmoothedGain_dB(float *inst_targ_gain_dB_block, float *gain_dB_block, uint16_t len) {
      float gain_dB;
      float one_minus_attack_const = 1.0f - attack_const;
      float one_minus_release_const = 1.0f - release_const;
      for (uint16_t i = 0; i < len; i++) {
        gain_dB = inst_targ_gain_dB_block[i];

        //smooth the gain using the attack or release constants
        if (gain_dB < prev_gain_dB) {  //are we in the attack phase?
          gain_dB_block[i] = attack_const*prev_gain_dB + one_minus_attack_const*gain_dB;
        } else {   //or, we're in the release phase
          gain_dB_block[i] = release_const*prev_gain_dB + one_minus_release_const*gain_dB;
        }

        //save value for the next time through this loop
        prev_gain_dB = gain_dB_block[i];
      }

      return;  //the output here is gain_block
    }


    //methods to set parameters of this module
    void resetStates(void) {
      prev_level_lp_pow = 1.0f;
      prev_gain_dB = 0.0f;
      
      //initialize the HP filter.  (This also resets the filter states,)
      arm_biquad_cascade_df1_init_f32(&hp_filt_struct, hp_nstages, hp_coeff, hp_state);
    }
    void setPreGain(float g) {  pre_gain = g;  }
    void setPreGain_dB(float gain_dB) { setPreGain(pow(10.0, gain_dB / 20.0));  }
    void setCompressionRatio(float cr) {
      comp_ratio = std::max(0.001f, cr); //limit to positive values
      updateThresholdAndCompRatioConstants();
    }
    void setAttack_sec(float a, float fs_Hz) {
      attack_sec = a;
      attack_const = expf(-1.0f / (attack_sec * fs_Hz)); //expf() is much faster than exp()

      //also update the time constant for the envelope extraction
      setLevelTimeConst_sec(std::min(attack_sec,release_sec) / 5.0, fs_Hz);  //make the level time-constant one-fifth the gain time constants
    } 
    void setRelease_sec(float r, float fs_Hz) {
      release_sec = r;
      release_const = expf(-1.0f / (release_sec * fs_Hz)); //expf() is much faster than exp()

      //also update the time constant for the envelope extraction
      setLevelTimeConst_sec(std::min(attack_sec,release_sec) / 5.0, fs_Hz);  //make the level time-constant one-fifth the gain time constants
    }
    void setLevelTimeConst_sec(float t_sec, float fs_Hz) {
      const float min_t_sec = 0.002f;  //this is the minimum allowed value
      level_lp_sec = std::max(min_t_sec,t_sec);
      level_lp_const = expf(-1.0f / (level_lp_sec * fs_Hz)); //expf() is much faster than exp()
    }
    void setThresh_dBFS(float val) { 
      thresh_dBFS = val;
      setThreshPow(pow(10.0, thresh_dBFS / 10.0));
    }
    void enableHPFilter(boolean flag) { use_HP_prefilter = flag; };

    //methods to return information about this module
    float getPreGain_dB(void) { return 20.0 * log10f_approx(pre_gain);  }
    float getAttack_sec(void) {  return attack_sec; }
    float getRelease_sec(void) {  return release_sec; }
    float getLevelTimeConst_sec(void) { return level_lp_sec; }
    float getThresh_dBFS(void) { return thresh_dBFS; }
    float getCompressionRatio(void) { return comp_ratio; }
    float getCurrentLevel_dBFS(void) { return 10.0* log10f_approx(prev_level_lp_pow); }
    float getCurrentGain_dB(void) { return prev_gain_dB; }

    void setHPFilterCoeff_N2IIR_Matlab(float b[], float a[]){
      //https://www.keil.com/pack/doc/CMSIS/DSP/html/group__BiquadCascadeDF1.html#ga8e73b69a788e681a61bccc8959d823c5
      //Use matlab to compute the coeff for HP at 20Hz: [b,a]=butter(2,20/(44100/2),'high'); %assumes fs_Hz = 44100
      hp_coeff[0] = b[0];   hp_coeff[1] = b[1];  hp_coeff[2] = b[2]; //here are the matlab "b" coefficients
      hp_coeff[3] = -a[1];  hp_coeff[4] = -a[2];  //the DSP needs the "a" terms to have opposite sign vs Matlab    	
    }
    
  private:
    //state-related variables
    float *inputQueueArray_f32[1]; //memory pointer for the input to this module
    float prev_level_lp_pow = 1.0;
    float prev_gain_dB = 0.0; //last gain^2 used

    //HP filter state-related variables
    arm_biquad_casd_df1_inst_f32 hp_filt_struct;
    static const uint8_t hp_nstages = 1;
    float hp_coeff[5 * hp_nstages] = {1.0, 0.0, 0.0, 0.0, 0.0}; //no filtering. actual filter coeff set later
    float hp_state[4 * hp_nstages];
    void setHPFilterCoeff(void) {
      //https://www.keil.com/pack/doc/CMSIS/DSP/html/group__BiquadCascadeDF1.html#ga8e73b69a788e681a61bccc8959d823c5
      //Use matlab to compute the coeff for HP at 20Hz: [b,a]=butter(2,20/(44100/2),'high'); %assumes fs_Hz = 44100
      float b[] = {9.979871156751189e-01,    -1.995974231350238e+00, 9.979871156751189e-01};  //from Matlab
      float a[] = { 1.000000000000000e+00,    -1.995970179642828e+00,    9.959782830576472e-01};  //from Matlab
      setHPFilterCoeff_N2IIR_Matlab(b, a);
      //hp_coeff[0] = b[0];   hp_coeff[1] = b[1];  hp_coeff[2] = b[2]; //here are the matlab "b" coefficients
      //hp_coeff[3] = -a[1];  hp_coeff[4] = -a[2];  //the DSP needs the "a" terms to have opposite sign vs Matlab
    }


    //private parameters related to gain calculation
    float attack_const, release_const, level_lp_const; //used in calcGain().  set by setAttack_sec() and setRelease_sec();
    float comp_ratio_const, thresh_pow_FS_wCR;  //used in calcGain();  set in updateThresholdAndCompRatioConstants()
    void updateThresholdAndCompRatioConstants(void) {
      comp_ratio_const = 1.0f-(1.0f / comp_ratio);
      thresh_pow_FS_wCR = powf(thresh_pow_FS, comp_ratio_const);    
    }

    //settings
    float attack_sec, release_sec, level_lp_sec; 
    float thresh_dBFS = 0.0;  //threshold for compression, relative to digital full scale
    float thresh_pow_FS = 1.0f;  //same as above, but not in dB
    void setThreshPow(float t_pow) { 
      thresh_pow_FS = t_pow;
      updateThresholdAndCompRatioConstants();
    }
    float comp_ratio = 1.0;  //compression ratio
    float pre_gain = -1.0;  //gain to apply before the compression.  negative value disables
    boolean use_HP_prefilter;
    
    
    // Accelerate the powf(10.0,x) function
    static float pow10f(float x) {
      //return powf(10.0f,x)   //standard, but slower
      return expf(2.302585092994f*x);  //faster:  exp(log(10.0f)*x)
    }

    // Accelerate the log10f(x)  function?
    static float log10f_approx(float x) {
      //return log10f(x);   //standard, but slower
      return log2f_approx(x)*0.3010299956639812f; //faster:  log2(x)/log2(10)
    }
    
    /* ----------------------------------------------------------------------
    ** Fast approximation to the log2() function.  It uses a two step
    ** process.  First, it decomposes the floating-point number into
    ** a fractional component F and an exponent E.  The fraction component
    ** is used in a polynomial approximation and then the exponent added
    ** to the result.  A 3rd order polynomial is used and the result
    ** when computing db20() is accurate to 7.984884e-003 dB.
    ** ------------------------------------------------------------------- */
    //https://community.arm.com/tools/f/discussions/4292/cmsis-dsp-new-functionality-proposal/22621#22621
    //float log2f_approx_coeff[4] = {1.23149591368684f, -4.11852516267426f, 6.02197014179219f, -3.13396450166353f};
    static float log2f_approx(float X) {
      //float *C = &log2f_approx_coeff[0];
      float Y;
      float F;
      int32_t E;
    
      // This is the approximation to log2()
      F = frexpf(fabsf(X), (int*)&E);
      //  Y = C[0]*F*F*F + C[1]*F*F + C[2]*F + C[3] + E;
      //Y = *C++;
      Y = 1.23149591368684f;
      Y *= F;
      //Y += (*C++);
      Y += -4.11852516267426f;
      Y *= F;
      //Y += (*C++);
      Y += 6.02197014179219f;
      Y *= F;
      //Y += (*C++);
      Y += -3.13396450166353f;
      Y += E;
    
      return(Y);
    }
    
    
};

#endif // RASPBERRYPI_PICO
#else
#warning USING TEENSYDUINO SO INTERNAL COMPRESSOR IS DISABLED!
#endif // TEENSYDUINO
#endif
