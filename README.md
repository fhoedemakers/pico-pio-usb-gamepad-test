# USB Gamepad Test Program for Adafruit Metro RP2350 Using PIO USB

This project demonstrates USB gamepad support on the [usb host breakout](https://learn.adafruit.com/adafruit-metro-rp2350/pinouts#usb-host-pins-3193156) of the [Adafruit Metro RP2350](https://www.adafruit.com/product/6003) and other RP2250-based boards using the PIO USB stack.

---

## Prerequisites

- **Hardware:**  
  - Adafruit Metro RP2350 or compatible RP2350 boards (e.g., Raspberry Pico 2)
- **Software:**  
  - [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
  - [TinyUSB (latest master)](https://github.com/hathach/tinyusb/tree/master)
  - [Pico PIO USB (latest main)](https://github.com/sekigon-gonnoc/Pico-PIO-USB/tree/main)  
    Set the environment variable `PICO_PIO_USB_PATH` to the location of this repo.

**Note:**  
Program output is printed to the serial console (GPIO-0 and GPIO-1).

---

## Building the Project

### For Adafruit Metro RP2350

```bash
git clone https://github.com/fhoedemakers/pico-pio-usb-gamepad-test.git
cd pico-pio-usb-gamepad-test
git submodule update --init
mkdir build
cd build
cmake ..
make
```

---

### For Other RP2040 Boards

Example: Building for a Raspberry Pi Pico 2 with PIO-USB enabled on an external USB connector (DP+ on GPIO 4, DP- on GPIO 5):

```bash
git clone https://github.com/fhoedemakers/pico-pio-usb-gamepad-test.git
cd pico-pio-usb-gamepad-test
git submodule update --init
mkdir build
cd build
cmake -DBOARD=pico_sdk -DPICO_BOARD=pico2 -DPIO_DP_PLUS_PIN=4 ..
make
```

---

### Using the Internal USB Connector (Disabling PIO USB)

For boards where you want to use the internal USB controller instead of PIO USB:

```bash
git clone https://github.com/fhoedemakers/pico-pio-usb-gamepad-test.git
cd pico-pio-usb-gamepad-test
git submodule update --init
mkdir build
cd build
# For Adafruit Metro RP2350:
cmake -DENABLE_PIO_USB=0 ..
# For Raspberry Pi Pico 2:
cmake -DBOARD=pico_sdk -DPICO_BOARD=pico2 -DENABLE_PIO_USB=0 ..
make
```

---

## Tested Controllers

See `hid_app.cpp` for the full list of tested controllers. Confirmed working:

- Xbox-style controllers (XINPUT)
- Sony DualShock 4
- Sony DualSense
- (More in source)

---

## Additional Notes

- For wiring details and pin assignments, refer to the board documentation and the `CMakeLists.txt` options.
- Contributions and pull requests are welcome!

---

## Resources

- [TinyUSB Documentation](https://docs.tinyusb.org/)
- [Pico SDK Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/index.html)
- [Pico PIO USB](https://github.com/sekigon-gonnoc/Pico-PIO-USB)
