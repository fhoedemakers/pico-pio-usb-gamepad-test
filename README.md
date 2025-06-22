# USB Gamepad test program for RP2040/RP2350 microcontrollers

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

