# USB Gamepad Test Program for RP2040/RP2350 microcontrollers using PIO USB

This C++ project demonstrates USB gamepad support on RP2040 and RP2350-based boards using PIO USB.  
It prints the pressed gamepad buttons to the serial console for easy debugging and testing.

![image](https://github.com/user-attachments/assets/fa70e238-576d-45be-8396-d0fd65f524bb)

---

## Prerequisites

- **Hardware:**  
  - Adafruit Metro RP2350. You also need this cable to connect to the usb host breakout: [USB Type A Jack Breakout Cable with Premium Female Jumpers](https://www.adafruit.com/product/4449)
  - Other compatible RP2040/RP2350 boards (e.g., Raspberry Pico/Pico 2) with an external usb connector attached. Like the [WaveShare RP2040 PiZero](https://www.waveshare.com/rp2040-pizero.htm) or similar boards.
  - USB gamepad or controller (e.g., Xbox-style, DualShock 4, DualSense, etc.)

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
This is the default config.
Connect the gamepad to the [usb host breakout pins](https://learn.adafruit.com/adafruit-metro-rp2350/pinouts#usb-host-pins-3193156) You need to solder the headers yopurself.

```bash
git clone https://github.com/fhoedemakers/pico-pio-usb-gamepad-test.git
cd pico-pio-usb-gamepad-test
git submodule update --init
mkdir build
cd build
cmake ..
make
```
![IMG_8837](https://github.com/user-attachments/assets/c1390c02-6b30-4c21-aceb-b55fb0549fed)

---

### For Other RP2040/RP2350 Boards

### Building for Raspberry Pi Pico 2 with PIO USB
Example: Building for a Raspberry Pi Pico 2 with PIO-USB enabled on an external USB connector (DP+ on GPIO 16, DP- on GPIO 17):

```bash
git clone https://github.com/fhoedemakers/pico-pio-usb-gamepad-test.git
cd pico-pio-usb-gamepad-test
git submodule update --init
mkdir build
cd build
cmake -DBOARD=pico_sdk -DPICO_BOARD=pico2 -DPIO_DP_PLUS_PIN=16 ..
make
```

![IMG_8838](https://github.com/user-attachments/assets/ab0793d7-bcb8-426e-abe7-ac72265c4864)



### Building for the WaveShare RP2040 PiZero

Example: Building for the [WaveShare RP2040 PiZero](https://www.waveshare.com/rp2040-pizero.htm) with PIO-USB enabled on USB port labeled PIO USB (DP+ on GPIO 6, DP- on GPIO 7):

```bash
git clone https://github.com/fhoedemakers/pico-pio-usb-gamepad-test.git
cd pico-pio-usb-gamepad-test
git submodule update --init
mkdir build
cd build
cmake -DBOARD=pico_sdk -DPICO_BOARD=pico -DPIO_DP_PLUS_PIN=6 ..
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
- Some low speed devices like keyboards might not work with PIO usb.
- Button labels are displayed in the serial console using a NES controller style:  
  **A, B, Select, Start, Left, Right, Up, Down**.  
  Other buttons are currently not shown.


---

## Resources

- [TinyUSB Documentation](https://docs.tinyusb.org/)
- [Pico SDK Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/index.html)
- [Pico PIO USB](https://github.com/sekigon-gonnoc/Pico-PIO-USB)
