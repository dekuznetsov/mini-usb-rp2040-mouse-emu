#include "status_led.h"

#include "pico/stdlib.h"

#ifdef PICO_DEFAULT_WS2812_PIN

#include "hardware/pio.h"
#include "ws2812.pio.h"

// GRB colours: dim green while idle, brighter lime green during a jiggle.
#define STATUS_LED_IDLE_GRB   0x080000u  // G=0x08
#define STATUS_LED_ACTIVE_GRB 0x402000u  // G=0x40, R=0x20

static PIO  led_pio;
static uint led_sm;
static bool led_ok;

void status_led_init(void) {
    uint offset;
    led_ok = pio_claim_free_sm_and_add_program_for_gpio_range(
        &ws2812_program, &led_pio, &led_sm, &offset,
        PICO_DEFAULT_WS2812_PIN, 1, true);
    if (!led_ok) {
        return;
    }
    ws2812_program_init(led_pio, led_sm, offset, PICO_DEFAULT_WS2812_PIN, 800000.0f);
    status_led_set(false);
}

void status_led_set(bool on) {
    if (!led_ok) {
        return;
    }
    // Data is shifted out MSB first from the top 24 bits of the word.
    uint32_t grb = on ? STATUS_LED_ACTIVE_GRB : STATUS_LED_IDLE_GRB;
    pio_sm_put_blocking(led_pio, led_sm, grb << 8);
}

#else

#include "bsp/board.h"

void status_led_init(void) {
    board_led_write(false);
}

void status_led_set(bool on) {
    board_led_write(on);
}

#endif
