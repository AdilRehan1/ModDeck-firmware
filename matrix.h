// Rows x columns matrix scanning for the main tactile keypad.
// Instead of one GPIO per key (16 pins for 16 keys), this uses
// MATRIX_ROWS + MATRIX_COLS pins (4 + 4 = 8) and scans row-by-row.

#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stdbool.h>
#include "board_config.h"

typedef void (*matrix_event_cb_t)(uint8_t row, uint8_t col, bool pressed);

// Configure row pins as outputs, column pins as inputs with pull-downs.
void matrix_init(void);

// Call frequently from the main loop (every MATRIX_SCAN_INTERVAL_MS is enough).
void matrix_task(matrix_event_cb_t cb);
uint8_t matrix_keycode(uint8_t row, uint8_t col);

#endif
