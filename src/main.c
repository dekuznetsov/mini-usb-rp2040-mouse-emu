#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/adc.h"
#include "bsp/board.h"
#include "tusb.h"

// --- USB HID Mouse Report (3 bytes: buttons, x, y) ---
static struct {
    uint8_t buttons;
    int8_t  x;
    int8_t  y;
} mouse_report = {0, 0, 0};

// --- Seed RNG from ADC noise on GPIO26 (ADC0) ---
static void seed_random(void) {
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    uint32_t seed = 0;
    for (int i = 0; i < 32; i++) {
        seed = (seed << 1) | (adc_read() & 1);
        sleep_us(50);
    }
    srand(seed);
}

// --- Random delay [min_ms .. max_ms] ---
static void random_sleep(uint32_t min_ms, uint32_t max_ms) {
    uint32_t delay = min_ms + (rand() % (max_ms - min_ms + 1));
    sleep_ms(delay);
}

// --- Send mouse move then release ---
static void mouse_move(int8_t dx, int8_t dy) {
    mouse_report.buttons = 0;
    mouse_report.x = dx;
    mouse_report.y = dy;
    tud_hid_report(0, &mouse_report, sizeof(mouse_report));
    sleep_ms(10);
    // Release
    mouse_report.x = 0;
    mouse_report.y = 0;
    tud_hid_report(0, &mouse_report, sizeof(mouse_report));
}

// --- TinyUSB Callbacks ---

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t *buffer, uint16_t reqlen) {
    (void)instance; (void)report_id; (void)report_type;
    (void)buffer;   (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                            hid_report_type_t report_type,
                            uint8_t const *buffer, uint16_t bufsize) {
    (void)instance; (void)report_id; (void)report_type;
    (void)buffer;   (void)bufsize;
}

// --- Required by TinyUSB for USB enumeration ---
void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol) {
    (void)instance; (void)protocol;
}

bool tud_hid_set_idle_cb(uint8_t instance, uint8_t idle_rate) {
    (void)instance; (void)idle_rate;
    return true; // accept idle rate
}

// --- Main ---
int main(void) {
    board_init();
    tusb_init();
    seed_random();

    absolute_time_t next_move = get_absolute_time();

    while (true) {
        tud_task();

        if (tud_mounted()) {
            board_led_write(true);

            if (absolute_time_diff_us(next_move, get_absolute_time()) > 0) {
                // Випадковий рух: ±2–6 пікселів по X і Y
                int8_t dx = (rand() % 5 + 2) * ((rand() % 2) ? 1 : -1);
                int8_t dy = (rand() % 5 + 2) * ((rand() % 2) ? 1 : -1);
                mouse_move(dx, dy);

                // Наступний рух через 30–50 секунд
                uint32_t delay_ms = 30000 + (rand() % 20001);
                next_move = make_timeout_time_ms(delay_ms);
            }
        } else {
            board_led_write(false);
        }
    }
}
