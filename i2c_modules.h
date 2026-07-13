// i2c_modules.h
// Handles the I2C bus to Mod Sub-Modules (Button module, Rotary module, and
// future ecosystem modules). On boot and periodically thereafter, the Main
// Module scans known addresses to see what's physically attached, then polls
// each present module's input-state register.

#ifndef I2C_MODULES_H
#define I2C_MODULES_H

#include <stdint.h>
#include <stdbool.h>
#include "board_config.h"

typedef enum {
    MOD_TYPE_NONE = 0,
    MOD_TYPE_BUTTON,
    MOD_TYPE_ROTARY,
} module_type_t;

// Fired when a module's input register changes (bit N = 1 -> that input active).
typedef void (*module_event_cb_t)(module_type_t type, uint8_t input_bits);

// Fired once when a module is plugged in / unplugged (detected via ACK on its address).
typedef void (*module_hotplug_cb_t)(module_type_t type, bool connected);

void i2c_modules_init(void);

// Call periodically (every loop). Internally rate-limited to MODBUS_POLL_INTERVAL_MS.
// Re-scans for hotplug on a slower cadence and polls connected modules for input state.
void i2c_modules_task(module_event_cb_t on_change, module_hotplug_cb_t on_hotplug);

#endif // I2C_MODULES_H
