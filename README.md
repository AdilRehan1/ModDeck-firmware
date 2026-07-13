# ModDeck Firmware

**ModDeck-firmware** is the core software for the **ModDeck**, a highly modular and customizable input device designed for power users, creators, and enthusiasts. Built on the **Raspberry Pi Pico (RP2040)** platform using the **Pico SDK** and **TinyUSB**, this firmware transforms hardware inputs into standard USB HID commands.

## Key Features

The ModDeck firmware is designed with flexibility and expandability in mind, offering a robust set of features:

- **4x4 Matrix Keypad Support**: Efficiently scans a 16-button matrix for rapid user input.

- **Dual Rotary Encoders**: Supports two quadrature encoders with integrated push-buttons for precise control over parameters like volume or scrolling.

- **Modular Expansion via I2C**: A dedicated I2C "ModBus" allows for hot-pluggable sub-modules, such as additional button arrays or rotary controllers.

- **USB HID Compliance**: Functions as a standard plug-and-play keyboard and consumer control device (volume, mute, etc.) without requiring custom drivers.

- **Centralized Configuration**: All pin assignments and system constants are managed in a single `board_config.h` file for easy hardware adaptation.

## Hardware Configuration

The firmware is pre-configured for the following default pinout on the Raspberry Pi Pico:

### Core Inputs

| Component | Type | GPIO Pins |
| --- | --- | --- |
| **Matrix Rows** | Output (Active High) | 2, 3, 4, 5 |
| **Matrix Columns** | Input (Pull-down) | 6, 7, 8, 9 |
| **Encoder 0** | A, B, Switch | 10, 11, 12 |
| **Encoder 1** | A, B, Switch | 13, 14, 15 |

### Expansion Bus (ModBus)

| Signal | GPIO Pin | Description |
| --- | --- | --- |
| **I2C SDA** | 16 | Data line for sub-modules |
| **I2C SCL** | 17 | Clock line for sub-modules |
| **I2C Port** | `i2c0` | 100kHz standard baud rate |

## Software Architecture

The firmware follows a modular task-based architecture within the main loop:

1. **`matrix_task`**: Scans the button matrix and debounces inputs.

1. **`encoder_task`**: Monitors rotary encoder rotation and button presses.

1. **`i2c_modules_task`**: Periodically polls the I2C bus to detect and communicate with connected sub-modules.

1. **`tud_task`**: Handles the TinyUSB stack for HID report generation and USB communication.

## Getting Started

### Prerequisites

- [Pico SDK](https://github.com/raspberrypi/pico-sdk) installed and configured.

- `arm-none-eabi-gcc` toolchain.

- CMake (version 3.13 or later).

### Build Instructions

1. Clone the repository:

   ```bash
   git clone https://github.com/AdilRehan1/ModDeck-firmware.git
   cd ModDeck-firmware
   ```

1. Initialize the build directory:

   ```bash
   mkdir build
   cd build
   cmake ..
   ```

1. Compile the firmware:

   ```bash
   make
   ```

1. Flash the resulting `ModDeck.uf2` file to your Raspberry Pi Pico by holding the BOOTSEL button while connecting it to your computer.

## Customization

To adapt the firmware to your specific hardware layout, modify `board_config.h`. You can update pin assignments, adjust debounce timings, or change I2C addresses for new sub-module types.

## License

This project is open-source. Please refer to the repository for specific licensing details.

---

*Developed by *[*AdilRehan1*](https://github.com/AdilRehan1)


Developed by AdilRehan1

