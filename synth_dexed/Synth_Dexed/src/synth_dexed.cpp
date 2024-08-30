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

#include "synth_dexed.h"

#if defined(TEENSYDUINO)
void AudioSynthDexed::update(void)
{
  if (in_update == true)
  {
    xrun++;
    return;
  }
  else
    in_update = true;

  elapsedMicros render_time;
  audio_block_t *block;

  block = allocate();

  if (!block)
  {
    in_update = false;
    return;
  }

  getSamples(block->data, AUDIO_BLOCK_SAMPLES);

  if (render_time > audio_block_time_us) // everything greater audio_block_time_us (2.9ms for buffer size of 128) is a buffer underrun!
    xrun++;

  if (render_time > render_time_max)
    render_time_max = render_time;

  transmit(block, 0);
  release(block);

  in_update = false;
};
#endif
