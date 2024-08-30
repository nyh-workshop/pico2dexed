#ifndef MIDITONESDRIVER_H
#define MIDITONESDRIVER_H

#include "picodexed.h"
#include "miditones.h"

extern CPicoDexed picoDexed;

class TunePlayer : public PlayTune<TunePlayer>
{
    public:
    inline void noteOff(uint8_t chan) { picoDexed.keyup(chan); }
    inline void noteOn(uint8_t chan, uint8_t num) { picoDexed.keydown(num, 0xff); }
    inline void mutexTryEnter();
    inline void mutexExit();
    inline void sleepMs(uint32_t ms) { sleep_ms(ms); }
    
    private:
    // Microcontroller SDK specific variables:
    mutex_t stepScoreMutex;
    uint32_t owner;
};

inline void TunePlayer::mutexTryEnter()
{
    if (!mutex_try_enter(&stepScoreMutex, &owner))
    {
        // printf("at core 0 - locked, owner: %d, core_num: %d\n", owner, get_core_num());
        mutex_enter_blocking(&stepScoreMutex);
    }
}

inline void TunePlayer::mutexExit()
{
    mutex_exit(&stepScoreMutex);
}

#endif