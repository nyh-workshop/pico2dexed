/*
   synth_dexed

   synth_dexed is a port of the Dexed sound engine (https://github.com/asb2m10/dexed)
   as library for the Teensy-3.5/3.6/4.x with an audio shield.
   Dexed ist heavily based on https://github.com/google/music-synthesizer-for-android

   (c)2018-2021 H. Wirtz <wirtz@parasitstudio.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#pragma once
#include "dexed.h"
#if defined(TEENSYDUINO)
#include <AudioStream.h>
#endif
#include <stdint.h>

#define SYNTH_DEXED_VERSION "1.0.1"
//#define DEBUG 1
#define SAMPLE_RATE 44100

#define TRANSPOSE_FIX 24
#define VOICE_SILENCE_LEVEL 1100

#define PB_RANGE_DEFAULT 1
#define PB_STEP_DEFAULT 0
#define MW_RANGE_DEFAULT 50
#define MW_ASSIGN_DEFAULT 0 // Bitmapped: 0: Pitch, 1: Amp, 2: Bias
#define MW_MODE_DEFAULT 0
#define FC_RANGE_DEFAULT 50
#define FC_ASSIGN_DEFAULT 0 // Bitmapped: 0: Pitch, 1: Amp, 2: Bias
#define FC_MODE_DEFAULT 0
#define BC_RANGE_DEFAULT 50
#define BC_ASSIGN_DEFAULT 0 // Bitmapped: 0: Pitch, 1: Amp, 2: Bias
#define BC_MODE_DEFAULT 0
#define AT_RANGE_DEFAULT 50
#define AT_ASSIGN_DEFAULT 0 // Bitmapped: 0: Pitch, 1: Amp, 2: Bias
#define AT_MODE_DEFAULT 0
#define PORTAMENTO_MODE_DEFAULT 0 // 0: Retain, 1: Follow
#define PORTAMENTO_GLISSANDO_DEFAULT 0
#define PORTAMENTO_TIME_DEFAULT 0

//#define USE_SIMPLE_COMPRESSOR 1

#if defined(TEENSYDUINO)
class AudioSynthDexed : public AudioStream, public Dexed
{
  public:

    AudioSynthDexed(uint8_t max_notes, uint16_t sample_rate) : AudioStream(0, NULL), Dexed(max_notes,sample_rate) { };

  protected:
    const uint16_t audio_block_time_us = 1000000 / (SAMPLE_RATE / AUDIO_BLOCK_SAMPLES);
    volatile bool in_update = false;
    void update(void);
};
#endif
