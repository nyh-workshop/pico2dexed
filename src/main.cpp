#include <cstdio>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "arm_math.h"
#include "picodexed.h"
#include "pico_perf.h"

#include "miditones.h"
#include "miditonesFile.h"
#include "miditonesDriver.h"

CPicoDexed picoDexed;

TunePlayer pico2TunePlayer;

int main(void) {
  // Overclock the Pico to 250MHz instead of the default 133MHz.
  // I have it on good authority (read, my Mastodon follows)
  // that this is an ok thing to do.  So there.
#if DEXED_OVERCLOCK==1
  set_sys_clock_khz(250000, false);
#endif

  stdio_init_all();
  fprintf(stderr,"Pico2Dexed %s ...\n", PICODEXED_VERSION);
  fprintf(stderr,"\nConfiguration:\n");
  fprintf(stderr,"  Sample Rate:  %d\n", DEXED_SAMPLE_RATE);
  fprintf(stderr,"  Polyphony:    %d\n", DEXED_POLYPHONY);
#if DEXED_OVERCLOCK==1
  fprintf(stderr,"  Overclocking: On\n");
#else
  fprintf(stderr,"  Overclocking: Off\n");
#endif
  //gpio_debug_pins_init(); // For Pico Library timing pins
  ledInit();
  // Use GPIO 2-3 for timing signals for testing
  //  timingInit(2);
  //  timingInit(3);
  
  //  ledFlash(2);
  sleep_ms(50);

  picoDexed.Init();

  picoDexed.ProgramChange(11-1);

//  ledFlash(3);

  pico2TunePlayer.playScore(mdtFile1);

  pico2TunePlayer.startPlaying();

  while (1)
  {
    if (pico2TunePlayer.isPlaying())
      pico2TunePlayer.stepScore();
    else
    {
      printf("Sleep at Core0!\n");
      sleep_ms(1000);
    }
  }

  return 0;
}