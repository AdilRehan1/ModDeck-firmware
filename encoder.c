#include "encoder.h"
#include "pico/stdlib.h"
static const int8_t QUAD_TABLE[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0,
};

typedef struct {
    uint8_t  prev_state;
    int8_t   accum;       // accumulates quarter-steps until a full detent (4 steps)
    bool     btn_state;
    bool     btn_raw_last;
    uint32_t btn_change_time;
} encoder_runtime_t;

static encoder_runtime_t enc_rt[NUM_ENCODERS];

void encoder_init(void) {
    for (int i = 0; i < NUM_ENCODERS; i++) {
        const encoder_pins_t *p = &ENCODER_PINS[i];

        gpio_init(p->pin_a);
        gpio_set_dir(p->pin_a, GPIO_IN);
        gpio_pull_up(p->pin_a);

        gpio_init(p->pin_b);
        gpio_set_dir(p->pin_b, GPIO_IN);
        gpio_pull_up(p->pin_b);

        gpio_init(p->pin_sw);
        gpio_set_dir(p->pin_sw, GPIO_IN);
        gpio_pull_up(p->pin_sw); // button typically shorts to GND when pressed

        uint8_t a = gpio_get(p->pin_a);
        uint8_t b = gpio_get(p->pin_b);
        enc_rt[i].prev_state = (a << 1) | b;
        enc_rt[i].accum = 0;
        enc_rt[i].btn_state = true; // pulled-up = released
        enc_rt[i].btn_raw_last = true;
    }
}

void encoder_task(encoder_event_cb_t rotate_cb, encoder_button_cb_t button_cb) {
    uint32_t now = to_ms_since_boot(get_absolute_time());

    for (int i = 0; i < NUM_ENCODERS; i++) {
        const encoder_pins_t *p = &ENCODER_PINS[i];
        encoder_runtime_t *rt = &enc_rt[i];

        // --- Quadrature rotation ---
        uint8_t a = gpio_get(p->pin_a);
        uint8_t b = gpio_get(p->pin_b);
        uint8_t new_state = (a << 1) | b;
        uint8_t idx = (rt->prev_state << 2) | new_state;
        int8_t step = QUAD_TABLE[idx];
        rt->prev_state = new_state;

        if (step != 0) {
            rt->accum += step;
            if (rt->accum >= 4) {
                rt->accum = 0;
                if (rotate_cb) rotate_cb((uint8_t)i, ENCODER_CW);
            } else if (rt->accum <= -4) {
                rt->accum = 0;
                if (rotate_cb) rotate_cb((uint8_t)i, ENCODER_CCW);
            }
        }

        // --- Push button (debounced, active LOW) ---
        bool raw = gpio_get(p->pin_sw); // true = released (pulled up)
        if (raw != rt->btn_raw_last) {
            rt->btn_raw_last = raw;
            rt->btn_change_time = now;
        } else if (raw != rt->btn_state &&
                   (now - rt->btn_change_time) >= ENCODER_DEBOUNCE_MS) {
            rt->btn_state = raw;
            if (button_cb) button_cb((uint8_t)i, !raw); // pressed = active low
        }
    }
}
