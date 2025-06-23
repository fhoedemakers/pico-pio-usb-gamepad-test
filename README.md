# USB Gamepad test program for Adafruit Metro RP2350 using PIO USB

# pre-requesites

- Adafruit Metro RP2350. Other RP2350 type boards should also work, but you need to modify CMakeLists.txt for this to work.
- Raspberry PI Pico SDK
- Latest master branch of Tiny-USB.  https://github.com/hathach/tinyusb/tree/master
- Latest main branch of Pico PIO USB. https://github.com/sekigon-gonnoc/Pico-PIO-USB/tree/main Set the environment var PICO_PIO_USB_PATH to the location of that repo.

## PIO USB usage

Can be used with PIO-USB by setting 

```cmake
target_compile_definitions(${PROJECT} PRIVATE
    CFG_TUH_RPI_PIO_USB=1
)
```
In CMakeLists.txt

Used with Adafruit Metro RP2350 board as config

You need the latest release of tiunyusb for this to work

You also need to get the https://github.com/sekigon-gonnoc/Pico-PIO-USB repo. Set the environment var PICO_PIO_USB_PATH to the location of that repo.

