#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "mouse_jiggler.h"

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

    while (true) {
        tud_task();
        mouse_jiggler_task();
    }
}
