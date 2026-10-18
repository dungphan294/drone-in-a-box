// The BOOT button as the pairing trigger.
//
// Pairing must be gated on something an attacker in radio range cannot do.
// Joining the dock's AP is not that: its password is a compile-time constant
// in this repository, shared by every unit built from it. Pressing a button on
// the board is.
//
// GPIO0 is the BOOT button on the ESP32-S3-DevKitC. It is a strapping pin, so
// it is only read after boot has finished -- holding it at reset puts the chip
// in download mode, which is unrelated and harmless here.
#pragma once

#define PAIR_BUTTON_GPIO      0
#define PAIR_BUTTON_DEBOUNCE_MS  250

void pair_button_start(void);
