#include "pico/stdlib.h"
#include "tusb_config.h"

#include "board_config.h"
#include "matrix.h"
#include "encoder.h"
#include "i2c_modules.h"
#include "usb_descriptors.h"

// --- Held-key state for the keyboard report (up to 6 simultaneous keys, per HID spec) ---
#define MAX_HELD_KEYS 6
static uint8_t held_keys[MAX_HELD_KEYS];

static void add_held_key(uint8_t keycode) {
    if (keycode == 0) return;
    for (int i = 0; i < MAX_HELD_KEYS; i++) {
        if (held_keys[i] == keycode) return; // already held
        if (held_keys[i] == 0) { held_keys[i] = keycode; return; }
    }
    // report full — silently drop, matches typical keyboard rollover behavior
}

static void remove_held_key(uint8_t keycode) {
    for (int i = 0; i < MAX_HELD_KEYS; i++) {
        if (held_keys[i] == keycode) {
            for (int j = i; j < MAX_HELD_KEYS - 1; j++) held_keys[j] = held_keys[j + 1];
            held_keys[MAX_HELD_KEYS - 1] = 0;
            return;
        }
    }
}

static void send_keyboard_report(void) {
    if (!tud_hid_ready()) return;
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0 /* no modifiers yet */, held_keys);
}

static void send_consumer_report(uint16_t usage) {
    if (!tud_hid_ready()) return;
    tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &usage, sizeof(usage));
    // Follow with a zero report so the host sees a discrete press, not a held key.
    sleep_ms(1);
    uint16_t zero = 0;
    tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &zero, sizeof(zero));
}

// --- Callbacks from each input subsystem ---

static void on_matrix_event(uint8_t row, uint8_t col, bool pressed) {
    uint8_t kc = matrix_keycode(row, col);
    if (kc == 0) return;
    if (pressed) add_held_key(kc);
    else         remove_held_key(kc);
    send_keyboard_report();
}

// HID consumer-control usages (see HID Usage Tables, Consumer page 0x0C).
#define HID_USAGE_CONSUMER_VOLUME_INC 0x00E9
#define HID_USAGE_CONSUMER_VOLUME_DEC 0x00EA

static void on_encoder_rotate(uint8_t idx, encoder_dir_t dir) {
    // Encoder 0 -> volume, encoder 1 -> left as a placeholder for a second
    // action (scrub / parameter) once the host-side mapping is decided.
    if (idx == 0) {
        send_consumer_report(dir == ENCODER_CW ? HID_USAGE_CONSUMER_VOLUME_INC
                                                : HID_USAGE_CONSUMER_VOLUME_DEC);
    }
}

static void on_encoder_button(uint8_t idx, bool pressed) {
    // Example: encoder push = mute toggle. Swap for a real keycode/usage as needed.
    (void) idx;
    if (pressed) send_consumer_report(0x00E2 /* HID_USAGE_CONSUMER_MUTE */);
}

static void on_module_change(module_type_t type, uint8_t input_bits) {
    // Each set bit in input_bits is one button on the attached sub-module.
    // Map bit -> keycode; extend this table as more module types are added.
    static const uint8_t BUTTON_MODULE_KEYS[6] = { 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F }; // F1-F6

    if (type == MOD_TYPE_BUTTON) {
        for (int bit = 0; bit < 6; bit++) {
            bool pressed = (input_bits >> bit) & 0x01;
            if (pressed) add_held_key(BUTTON_MODULE_KEYS[bit]);
            else         remove_held_key(BUTTON_MODULE_KEYS[bit]);
        }
        send_keyboard_report();
    }
    // MOD_TYPE_ROTARY: decode input_bits into its own encoder/button events here
    // once the sub-module's bit layout is finalized.
}

static void on_module_hotplug(module_type_t type, bool connected) {
    // Hook point for user feedback (e.g. an LED blink) when a module is
    // plugged in or removed. Left as a no-op in this skeleton.
    (void) type; (void) connected;
}

int main(void) {
    stdio_init_all();

    matrix_init();
    encoder_init();
    i2c_modules_init();
    tusb_init();

    while (true) {
        tud_task(); // TinyUSB device task — must be called frequently

        matrix_task(on_matrix_event);
        encoder_task(on_encoder_rotate, on_encoder_button);
        i2c_modules_task(on_module_change, on_module_hotplug);
    }

    return 0;
}

// TinyUSB device callbacks (connection lifecycle — no action needed here)
void tud_mount_cb(void) {}
void tud_umount_cb(void) {}
void tud_suspend_cb(bool remote_wakeup_en) { (void) remote_wakeup_en; }
void tud_resume_cb(void) {}
