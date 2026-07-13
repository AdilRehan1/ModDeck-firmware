// matrix.c
#include "matrix.h"
#include "pico/stdlib.h"

// Debounce state per key: last stable state + timestamp of last raw change.
static bool     key_state[MATRIX_ROWS][MATRIX_COLS];
static bool     key_raw_last[MATRIX_ROWS][MATRIX_COLS];
static uint32_t key_change_time[MATRIX_ROWS][MATRIX_COLS];
static uint32_t last_scan_ms;

// Example keymap — replace with real HID usage-ID keycodes (see hid.h in TinyUSB,
// HID_KEY_* constants) once the legend is finalized.
static const uint8_t KEYMAP[MATRIX_ROWS][MATRIX_COLS] = {
    { 0x1E, 0x1F, 0x20, 0x21 }, // row0: '1' '2' '3' '4'
    { 0x22, 0x23, 0x24, 0x25 }, // row1: '5' '6' '7' '8'
    { 0x26, 0x27, 0x28, 0x2A }, // row2: '9' '0' Enter Backspace
    { 0x29, 0x2B, 0x2C, 0x00 }, // row3: Esc Tab Space (unused)
};

uint8_t matrix_keycode(uint8_t row, uint8_t col) {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return 0;
    return KEYMAP[row][col];
}

 
}

void matrix_task(matrix_event_cb_t cb) {
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_scan_ms < MATRIX_SCAN_INTERVAL_MS) return;
    last_scan_ms = now;

    for (int r = 0; r < MATRIX_ROWS; r++) {
        gpio_put(MATRIX_ROW_PINS[r], 1);
        sleep_us(5); // let the row line settle before sampling columns

        for (int c = 0; c < MATRIX_COLS; c++) {
            bool raw = gpio_get(MATRIX_COL_PINS[c]);

            if (raw != key_raw_last[r][c]) {
                key_raw_last[r][c] = raw;
                key_change_time[r][c] = now;
            } else if (raw != key_state[r][c] &&
                       (now - key_change_time[r][c]) >= MATRIX_DEBOUNCE_MS) {
                key_state[r][c] = raw;
                if (cb) cb((uint8_t)r, (uint8_t)c, raw);
            }
        }

        gpio_put(MATRIX_ROW_PINS[r], 0);
    }
}
