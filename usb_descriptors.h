// usb_descriptors.h
#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

// Two logical HID reports share one interface: standard keyboard (matrix +
// module button keys) and consumer control (encoder volume/scrub, media keys).
typedef enum {
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_CONSUMER_CONTROL,
} report_id_t;

#endif // USB_DESCRIPTORS_H
