#ifdef AUDIO_INTERFACE 

#include "usb_names.h"
#define MIDI_NAME   {'S','y','n','t','h','D','e','x','e','d'}
#define MIDI_NAME_LEN 10

// Do not change this part.  This exact format is required by USB.

struct usb_string_descriptor_struct usb_string_product_name = {
  2 + MIDI_NAME_LEN * 2,
  3,
  MIDI_NAME
};

#endif
