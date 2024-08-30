#include <Audio.h>
#include "synth_dexed.h"
#include "banks.h"

AudioSynthDexed          dexed(4, SAMPLE_RATE); // 4 voices max
AudioOutputI2S           i2s1;
AudioControlSGTL5000     sgtl5000_1;
AudioConnection          patchCord1(dexed, 0, i2s1, 0);
AudioConnection          patchCord2(dexed, 0, i2s1, 1);

void setup()
{
  Serial.begin(230400);

  AudioMemory(32);

  sgtl5000_1.enable();
  sgtl5000_1.lineOutLevel(29);
  sgtl5000_1.dacVolumeRamp();
  sgtl5000_1.dacVolume(1.0);
  sgtl5000_1.unmuteHeadphone();
  sgtl5000_1.unmuteLineout();
  sgtl5000_1.volume(0.8, 0.8); // Headphone volume

  randomSeed(analogRead(0));
}

void loop()
{
  uint8_t b = random(0, 10);
  uint8_t v = random(0, 32);
  uint8_t t = random(0, 36);
  char voice_name[11];
  uint8_t decoded_voice[156];

  memset(voice_name, 0, 11);
  memcpy(voice_name, &progmem_bank[b][v][117], 10);

  Serial.print("Voice: ");
  Serial.print(voice_name);
  Serial.print(" ");
  Serial.print("Transpose: ");
  Serial.println(t);

  dexed.decodeVoice(progmem_bank[b][v],decoded_voice); 
  dexed.loadVoiceParameters(decoded_voice);
  dexed.setTranspose(t);

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
  delay(1000);
}
