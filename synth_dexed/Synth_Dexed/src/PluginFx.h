/**

   Copyright (c) 2013 Pascal Gauthier.

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

#include "stdint.h"

class PluginFx {
    float s1, s2, s3, s4;
    float sampleRate;
    float sampleRateInv;
    float d, c;
    float R24;
    float rcor24, rcor24Inv;
    float bright;

    // 24 db multimode
    float mm;
    float mmt;
    int32_t mmch;
    inline float NR24(float sample, float g, float lpc);

    // preprocess values taken the UI
    float rCutoff;
    float rReso;
    float rGain;

    // thread values; if these are different from the UI,
    // it needs to be recalculated.
    float pReso;
    float pCutoff;
    float pGain;

    // I am still keeping the 2pole w/multimode filter
    inline float NR(float sample, float g);
    bool bandPassSw;
    float rcor, rcorInv;
    int32_t R;

    float dc_id;
    float dc_od;
    float dc_r;

  public:
    PluginFx();

    // this is set directly by the ui / parameter
    float Cutoff;
    float Reso;
    float Gain;

    void init(uint16_t sampleRate);
    void process(float *work, uint16_t sampleSize);
    float getGain(void);
};
