// Board config for the "MINI USB RP2350" USB-A stick (AITEWIN ROBOT,
// AliExpress). RP2350A, USB-A plug, JST 1.0 mm 4-pin connector, WS2812 RGB LED.
//
// Same layout as the Waveshare RP2350 boards except that the WS2812's data
// line is on GPIO22 instead of GPIO16 (found empirically; colour order GRB).

// pico_cmake_set PICO_PLATFORM=rp2350

#ifndef _BOARDS_MINI_USB_RP2350_H
#define _BOARDS_MINI_USB_RP2350_H

#define MINI_USB_RP2350

#define PICO_DEFAULT_WS2812_PIN 22

#include "boards/waveshare_rp2350_one.h"

#endif
