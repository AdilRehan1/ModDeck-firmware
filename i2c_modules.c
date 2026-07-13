#include "i2c_modules.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

typedef struct {
    uint8_t address;
    module_type_t type;
    bool connected;
    uint8_t last_bits;
} module_slot_t;

static module_slot_t slots[] = {
    { .address = MOD_ADDR_BUTTON, .type = MOD_TYPE_BUTTON, .connected = false, .last_bits = 0 },
    { .address = MOD_ADDR_ROTARY, .type = MOD_TYPE_ROTARY, .connected = false, .last_bits = 0 },
};
#define NUM_SLOTS (sizeof(slots) / sizeof(slots[0]))

static uint32_t last_poll_ms;

void i2c_modules_init(void) {
    i2c_init(MODBUS_I2C_PORT, MODBUS_BAUD_HZ);
    gpio_set_function(MODBUS_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(MODBUS_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(MODBUS_SDA_PIN);
    gpio_pull_up(MODBUS_SCL_PIN);

    last_poll_ms = to_ms_since_boot(get_absolute_time());
}
static bool probe_and_read(uint8_t addr, uint8_t *out_byte) {
    uint8_t buf;
    int ret = i2c_read_blocking(MODBUS_I2C_PORT, addr, &buf, 1, false);
    if (ret < 0) return false; // PICO_ERROR_GENERIC on NACK/timeout
    if (out_byte) *out_byte = buf;
    return true;
}

void i2c_modules_task(module_event_cb_t on_change, module_hotplug_cb_t on_hotplug) {
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_poll_ms < MODBUS_POLL_INTERVAL_MS) return;
    last_poll_ms = now;
    for (size_t i = 0; i < NUM_SLOTS; i++) {
        module_slot_t *slot = &slots[i];
        uint8_t bits = 0;
        bool ok = probe_and_read(slot->address, &bits);

        if (ok != slot->connected) {
            slot->connected = ok;
            slot->last_bits = 0;
            if (on_hotplug) on_hotplug(slot->type, ok);
            continue; // let the state settle before reporting input changes
        }

        if (ok && bits != slot->last_bits) {
            slot->last_bits = bits;
            if (on_change) on_change(slot->type, bits);
        }
    }
}
