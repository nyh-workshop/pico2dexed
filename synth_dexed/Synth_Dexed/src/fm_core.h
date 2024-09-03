/*
   Copyright 2012 Google Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __FM_CORE_H
#define __FM_CORE_H

#include <stdint.h>
#include "aligned_buf.h"
#include "fm_op_kernel.h"
#include "synth.h"
#include "controllers.h"

class FmOperatorInfo {
  public:
    int32_t in;
    int32_t out;
};

enum FmOperatorFlags {
  OUT_BUS_ONE = 1 << 0,
  OUT_BUS_TWO = 1 << 1,
  OUT_BUS_ADD = 1 << 2,
  IN_BUS_ONE = 1 << 4,
  IN_BUS_TWO = 1 << 5,
  FB_IN = 1 << 6,
  FB_OUT = 1 << 7
};

class FmAlgorithm {
  public:
    int32_t ops[6];
};

class FmCore {
  public:
    FmCore() {};
    virtual ~FmCore() {};
    static void dump();
    static uint8_t get_carrier_operators(uint8_t algorithm);
    virtual void render(int32_t *output, FmOpParams *params, int32_t algorithm, int32_t *fb_buf, int32_t feedback_gain) = 0;
  protected:
    AlignedBuf<int32_t, _N_>buf_[2];
    const static FmAlgorithm algorithms[32];
};

#endif
