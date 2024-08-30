# Pico2Dexed

## Port of PicoDexed to the Raspberry Pi Pico 2 (RP2350, ARM Cortex-M33 cores)

Demo in Youtube:

[![demo](https://img.youtube.com/vi/Hay3UIvuQaQ/0.jpg)](https://www.youtube.com/watch?v=Hay3UIvuQaQ)

Many acknolegements and thanks goes to Kevin from *diyelectromusic* and his [webpage]( https://diyelectromusic.wordpress.com/2024/01/09/raspberry-pi-pico-synth_dexed/) has many great and interesting details on the development of the port.

This port contains [Len Shustek's miditones player](https://github.com/LenShustek/miditones) and the app plays a short piece of song in miditones format. (miditonesFile.cpp)

## Basic Build Instructions

1. Install the "Raspberry Pi Pico" [extension](https://github.com/raspberrypi/pico-vscode) through VSCode.
2. Clone this repository!
3. At VSCode, Ctrl-Shift-P, then `Raspberry Pico: Configure CMake`. ***Do not use `CMake: Clean` as it can break the compilation!*** 
3. With the same Ctrl-Shift-P too, build the project with `Raspberry Pico: Compile Pico Project`.

## Additional info on repositories used for this build:

Microdexed git hash: **c30955409a**

Picodexed git hash: **ef7928f12d**

## Hardware requirements:
* [Raspberry Pi Pico 2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)
* I2S DAC.  Has been tested with a PCM5102 and the [Pimoroni Pico Audio Pack](https://shop.pimoroni.com/products/pico-audio-pack)

## Limitations:
* This is still under testing. Currently it is running at 250MHz with 10-note polyphony and 44100Hz sampling rate and it does not stutter. I'd say it is ***way much better*** than the [original port I did for the ESP32](https://github.com/nyh-workshop/esp32-microdexed/tree/master) too.

* When it is in the original 150MHz, some of the instruments (especially with a longer sustaining ones) are outputting bubbling and stuttering noises when it hits a few notes together at 44100Hz. Still, it's *much better* than my [older ESP32 port](https://github.com/nyh-workshop/esp32-microdexed/tree/master)!

* It is compiled for ARM Cortex-M33! Currently there are no plans on building this on the Hazard3 RISC-V cores.

* For testing purposes, the USB and Serial MIDI modules are temporarily disabled.

* There are clicking noises between note switches - this is mitigated by using an older version of the Miditones (v1.12) where there are note stops before the note change happens. This note stops allow the brief release of the note in the envelope generator and significantly minimizes the unpleasant noise.

## More Information on the PicoDexed:

* Details of the hardware build can be found here: https://diyelectromusic.wordpress.com/2024/02/18/raspberry-pi-pico-synth_dexed-part-5/

* Details of the MIDI implementation can be found here: https://diyelectromusic.wordpress.com/2024/02/16/raspberry-pi-pico-synth_dexed-part-4/

## References

* [Picodexed](https://github.com/diyelectromusic/picodexed) by Kevin from *diyelectromusic*.
* Thanks to paulober for assisting me in the [build issue](https://github.com/raspberrypi/pico-vscode/issues/66).