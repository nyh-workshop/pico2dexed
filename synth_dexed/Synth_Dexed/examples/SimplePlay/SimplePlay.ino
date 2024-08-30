#include <Audio.h>
#include "synth_dexed.h"

uint8_t fmpiano_sysex[156] = {
  95, 29, 20, 50, 99, 95, 00, 00, 41, 00, 19, 00, 00, 03, 00, 06, 79, 00, 01, 00, 14, // OP6 eg_rate_1-4, level_1-4, kbd_lev_scl_brk_pt, kbd_lev_scl_lft_depth, kbd_lev_scl_rht_depth, kbd_lev_scl_lft_curve, kbd_lev_scl_rht_curve, kbd_rate_scaling, amp_mod_sensitivity, key_vel_sensitivity, operator_output_level, osc_mode, osc_freq_coarse, osc_freq_fine, osc_detune
  95, 20, 20, 50, 99, 95, 00, 00, 00, 00, 00, 00, 00, 03, 00, 00, 99, 00, 01, 00, 00, // OP5
  95, 29, 20, 50, 99, 95, 00, 00, 00, 00, 00, 00, 00, 03, 00, 06, 89, 00, 01, 00, 07, // OP4
  95, 20, 20, 50, 99, 95, 00, 00, 00, 00, 00, 00, 00, 03, 00, 02, 99, 00, 01, 00, 07, // OP3
  95, 50, 35, 78, 99, 75, 00, 00, 00, 00, 00, 00, 00, 03, 00, 07, 58, 00, 14, 00, 07, // OP2
  96, 25, 25, 67, 99, 75, 00, 00, 00, 00, 00, 00, 00, 03, 00, 02, 99, 00, 01, 00, 10, // OP1
  94, 67, 95, 60, 50, 50, 50, 50,                                                     // 4 * pitch EG rates, 4 * pitch EG level
  04, 06, 00,                                                                         // algorithm, feedback, osc sync
  34, 33, 00, 00, 00, 04,                                                             // lfo speed, lfo delay, lfo pitch_mod_depth, lfo_amp_mod_depth, lfo_sync, lfo_waveform
  03, 24,                                                                             // pitch_mod_sensitivity, transpose
  70, 77, 45, 80, 73, 65, 78, 79, 00, 00                                              // 10 * char for name ("DEFAULT   ")
}; // FM-Piano

AudioSynthDexed          dexed(4,SAMPLE_RATE); // 4 voices max
AudioOutputI2S           i2s1;
AudioControlSGTL5000     sgtl5000_1;
AudioConnection          patchCord1(dexed, 0, i2s1, 0);
AudioConnection          patchCord2(dexed, 0, i2s1, 1);
#ifdef AUDIO_INTERFACE 
AudioOutputUSB           usb1;
AudioConnection          patchCord3(dexed, 0, usb1, 0);
AudioConnection          patchCord4(dexed, 0, usb1, 1);
#endif

void setup()
{
  AudioMemory(32);

  sgtl5000_1.enable();
  sgtl5000_1.lineOutLevel(29);
  sgtl5000_1.dacVolumeRamp();
  sgtl5000_1.dacVolume(1.0);
  sgtl5000_1.unmuteHeadphone();
  sgtl5000_1.unmuteLineout();
  sgtl5000_1.volume(0.8, 0.8); // Headphone volume
}

void loop()
{
  static uint8_t count;

  if (count % 2 == 0)
  {
    dexed.loadInitVoice();
  }
  else
  {
    dexed.loadVoiceParameters(fmpiano_sysex);
    dexed.setTranspose(36);
  }

  Serial.println("Key-Down");
  dexed.keydown(48, 100);
  delay(100);
  dexed.keydown(52, 100);
  delay(100);
  dexed.keydown(55, 100);
  delay(100);
  dexed.keydown(60, 100);
  delay(2000);

  Serial.println("Key-Up");
  dexed.keyup(48);
  dexed.keyup(52);
  dexed.keyup(55);
  dexed.keyup(60);
  delay(2000);

  count++;
}
