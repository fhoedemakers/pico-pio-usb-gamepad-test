# USB Gamepad test program for Adafruit Metro RP2350 using PIO USB

# pre-requesites

- Adafruit Metro RP2350. Other RP2350 type boards should also work, but you need to modify CMakeLists.txt for this to work.
- Raspberry PI Pico SDK
- Latest master branch of Tiny-USB.  https://github.com/hathach/tinyusb/tree/master
- Latest main branch of Pico PIO USB. https://github.com/sekigon-gonnoc/Pico-PIO-USB/tree/main Set the environment var PICO_PIO_USB_PATH to the location of that repo.

When running the program output is printed to the serial console. (GPIO-0 and GPIO-1)

## How to build for the Adafruit Metro RP2350

```bash
git clone https://github.com/fhoedemakers/pico-pio-usb-gamepad-test.git
cd pico-pio-usb-gamepad-test
git submodule update --init
mkdir build
cd build
cmake ..
make
```
## How to build for other RP2350 boards

Below an example for building for a Raspberry Pi Pico 2 with PIO-USB enabled on an external USB connector. DP+ is connected to GPIO 4 and DP- is connected to GPIO 5. 

```bash
git clone https://github.com/fhoedemakers/pico-pio-usb-gamepad-test.git
cd pico-pio-usb-gamepad-test
git submodule update --init
mkdir build
cd build
cmake -DBOARD=pico_sdk -DPICO_BOARD=pico2 -DPIO_DP_PLUS_PIN=4 ..
make
```

## How to build for use with the internal USB connector of the Raspberry Pi Pico, disabling PIO USB
```bash
git clone https://github.com/fhoedemakers/pico-pio-usb-gamepad-test.git
cd pico-pio-usb-gamepad-test
git submodule update --init
mkdir build
cd build
# For Adafruit Metro RP2350, use the following command:
cmake -DENABLE_PIO_USB=0 ..
# For Raspberry Pi Pico 2, use the following command:
# Note that the DP+ pin is set to GPIO 4 and DP- pin is set
cmake -DBOARD=pico_sdk -DPICO_BOARD=pico2 -DENABLE_PIO_USB=0 ..
make
```

## Tested controllers
See hid_app.cpp for the list of controllers that have been tested with this program.

- Xbox style controllers (XINPUT)
- Sony Dual Shock 4
- Sony Dual Sense
- ...
