#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <stdbool.h>

// On-board status LED. Uses the board's WS2812 RGB LED when it has one
// (PICO_DEFAULT_WS2812_PIN, e.g. Waveshare RP2350 boards), otherwise the
// plain LED driven by board_led_write() (PICO_DEFAULT_LED_PIN, e.g. Pico).

// Call once after board_init().
void status_led_init(void);

// on = jiggle in progress. WS2812: lime green when on, dim green otherwise
// (always lit). Plain LED: lit when on, off otherwise.
void status_led_set(bool on);

#endif // STATUS_LED_H
