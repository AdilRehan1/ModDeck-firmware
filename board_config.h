// board_config.h
// Central place for pin assignments and system constants.
// Update these to match the actual KiCad net names once the schematic is final —
// everything else in the firmware reads from here, nothing else hardcodes a GPIO.

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "pico/stdlib.h" // provides the `uint` typedef used below

// ---------- Matrix keypad (4x4 tactile switches) ----------
#define MATRIX_ROWS 4
#define MATRIX_COLS 4

// Row pins are driven OUTPUT, scanned one at a time (active HIGH).
static const uint MATRIX_ROW_PINS[MATRIX_ROWS] = {2, 3, 4, 5};
// Col pins are INPUT with internal pull-down; read HIGH when a switch closes the row->col path.
static const uint MATRIX_COL_PINS[MATRIX_COLS] = {6, 7, 8, 9};

#define MATRIX_DEBOUNCE_MS 8
#define MATRIX_SCAN_INTERVAL_MS 2

// ---------- Rotary encoders (2x, quadrature + push button) ----------
#define NUM_ENCODERS 2

typedef struct {
    uint pin_a;
    uint pin_b;
    uint pin_sw;
} encoder_pins_t;

static const encoder_pins_t ENCODER_PINS[NUM_ENCODERS] = {
    { .pin_a = 10, .pin_b = 11, .pin_sw = 12 },
    { .pin_a = 13, .pin_b = 14, .pin_sw = 15 },
};

#define ENCODER_DEBOUNCE_MS 5

// ---------- I2C bus to Mod Sub-Modules ----------
#define MODBUS_I2C_PORT   i2c0
#define MODBUS_SDA_PIN    16
#define MODBUS_SCL_PIN    17
#define MODBUS_BAUD_HZ    100000

// Known Sub-Module addresses. Each module type ACKs at a fixed 7-bit address
// so the Main Module can identify what's attached without a separate ID pin.
#define MOD_ADDR_BUTTON   0x21   // 6-key button module
#define MOD_ADDR_ROTARY   0x22   // 2-encoder + 4-button module

#define MODBUS_POLL_INTERVAL_MS 10

#endif // BOARD_CONFIG_H
