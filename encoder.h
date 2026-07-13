#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <stdbool.h>
#include "board_config.h"

typedef enum {
    ENCODER_NONE = 0,
    ENCODER_CW,
    ENCODER_CCW,
} encoder_dir_t;

typedef void (*encoder_event_cb_t)(uint8_t encoder_idx, encoder_dir_t dir);
typedef void (*encoder_button_cb_t)(uint8_t encoder_idx, bool pressed);

void encoder_init(void);

// Call every loop iteration. Fires rotate_cb on each detent step and
// button_cb on debounced press/release of the encoder's push button.
void encoder_task(encoder_event_cb_t rotate_cb, encoder_button_cb_t button_cb);

#endif
