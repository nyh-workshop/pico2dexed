#ifndef _config_h
#define _config_h

#define PICODEXED_VERSION "v0.01xx"

#define MIDI_CHANNEL 1
#define MIDI_SYSEX_DEVICE_ID 0  // Yamaha SysEx Device Number maps onto 0..F..

// Sensible configurations for I2S:
//    24000, 16, 1 16-note polyphony at 24kHz if overclocked to 250MHz
//    24000,  8, 0  8-note polyphony at 24kHz
//    48000,  8, 1  8-note polyphony at 48kHz if overclocked to 250MHz
//    48000,  4, 0  4-note polyphony at 48kHz
//
// Sensible configuration for PWM:
//    24000,  8, 0  8-note polyphony at 24kHz
//
// At present, overclocking messes with PWM.
//
#define DEXED_SAMPLE_RATE    44100
#define DEXED_POLYPHONY      10
#define DEXED_OVERCLOCK      1

#define AUDIO_PWM       1
#define AUDIO_I2S       2

#define AUDIO_OUTPUT    AUDIO_I2S

// PWM requires an output circuit, e.g. as described here:
// https://embeddedcomputing.com/technology/open-source/development-kits/raspberry-pi-pico-pwm-audio-output-circuit
//
#define PWM_PIN         20

#define I2S_DATA_PIN    28
#define I2S_BCLK_PIN    26
#define I2S_LRCLK_PIN   27  // Implied by BCLK=26

#define MIDI_TX_PIN     4   // Not used at present
#define MIDI_RX_PIN     5

#define DEBUG_UART_TX_PIN  0
#define DEBUG_UART_RX_PIN  1

#endif
