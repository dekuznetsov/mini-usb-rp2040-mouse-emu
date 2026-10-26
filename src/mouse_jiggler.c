// Every JIGGLE_PERIOD_MS the jiggler makes one short "jiggle":
//   - the on-board LED lights up,
//   - the cursor moves JIGGLE_STEPS steps diagonally down-right,
//   - then the same number of steps back, so it ends where it started,
//   - the LED goes off.
//
// tud_task() must keep running while a jiggle is in progress, so this is a
// state machine driven from the main loop instead of a blocking sequence.

#include "mouse_jiggler.h"

#include <stdbool.h>
#include <stdint.h>
#include "pico/time.h"
#include "bsp/board.h"
#include "tusb.h"

// --- Tuning ---
#define JIGGLE_PERIOD_MS   30000U  // pause between jiggles
#define JIGGLE_STEPS       12U     // steps in each direction
#define JIGGLE_STEP_PX     3       // pixels per step on each axis
#define REPORT_INTERVAL_MS 10U     // matches the HID endpoint bInterval

// --- USB HID Mouse Report (3 bytes: buttons, x, y) ---
// Layout must match desc_hid_report in usb_descriptors.c.
typedef struct {
    uint8_t buttons;  // bit0 = left, bit1 = right, bit2 = middle
    int8_t  x;        // relative movement, + = right
    int8_t  y;        // relative movement, + = down
} mouse_report_t;

static bool           jiggling;
static uint32_t       step;          // 0 .. 2*JIGGLE_STEPS-1
static absolute_time_t next_event;   // next jiggle start, or next step while jiggling
static bool           was_mounted;

static void jiggle_stop(void) {
    jiggling = false;
    board_led_write(false);
    next_event = make_timeout_time_ms(JIGGLE_PERIOD_MS);
}

void mouse_jiggler_task(void) {
    if (!tud_mounted()) {
        if (was_mounted) {
            jiggling = false;
            board_led_write(false);
        }
        was_mounted = false;
        return;
    }

    // First jiggle one full period after (re)enumeration.
    if (!was_mounted) {
        was_mounted = true;
        next_event = make_timeout_time_ms(JIGGLE_PERIOD_MS);
    }

    if (!time_reached(next_event)) {
        return;
    }

    if (!jiggling) {
        jiggling = true;
        step = 0;
        board_led_write(true);
    }

    // Send only once the previous report has been picked up by the host:
    // tud_hid_report() fails while the endpoint is busy, which would make
    // the "forward" and "back" moves unequal and the cursor drift.
    if (!tud_hid_ready()) {
        return;
    }

    int8_t d = (step < JIGGLE_STEPS) ? JIGGLE_STEP_PX : -JIGGLE_STEP_PX;
    mouse_report_t report = { .buttons = 0, .x = d, .y = d };
    if (!tud_hid_report(0, &report, sizeof(report))) {
        return;  // retry on the next loop iteration
    }

    if (++step >= 2 * JIGGLE_STEPS) {
        jiggle_stop();
    } else {
        next_event = make_timeout_time_ms(REPORT_INTERVAL_MS);
    }
}
