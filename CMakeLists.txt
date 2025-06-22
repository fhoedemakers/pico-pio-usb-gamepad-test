cmake_minimum_required(VERSION 3.20)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# include from environment var PICO_SDK_PATH
set(BOARD adafruit_metro_rp2350)
set(PICO_BOARD ${BOARD})
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
# set PICO_PIO_USB_PATH if not set
if(NOT DEFINED PICO_PIO_USB_PATH)
  set(PICO_PIO_USB_PATH $ENV{PICO_PIO_USB_PATH})
endif()
set(PICO_BOARD_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/boards)
message(STATUS "Using PICO_SDK_PATH: ${PICO_SDK_PATH}")
message(STATUS "Using PICO_PIO_USB_PATH: ${PICO_PIO_USB_PATH}")
include(pico_sdk_import.cmake)
include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/family_support.cmake)

#family_get_project_name(PROJECT ${CMAKE_CURRENT_LIST_DIR})
set(PROJECT "usbcontrollertest")
project(${PROJECT} C CXX ASM)

# Checks this example is valid for the family and initializes the project
family_initialize_project(${PROJECT} ${CMAKE_CURRENT_LIST_DIR})
add_executable(${PROJECT})

# Example source
target_sources(${PROJECT} PUBLIC
 usbcontrollertest.cpp gamepad.cpp hid_app.cpp
  )
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tusb_xinput xinput_host)
# Example include
target_include_directories(${PROJECT} PUBLIC
  .
  )

target_compile_definitions(${PROJECT} PRIVATE
    CFG_TUH_RPI_PIO_USB=1
)
# Configure compilation flags and libraries for the example without RTOS.
# See the corresponding function in hw/bsp/FAMILY/family.cmake for details.
family_configure_target(${PROJECT} "")
# require tinyusb_pico_pio_usb
target_link_libraries(${PROJECT} PUBLIC pico_stdlib tinyusb_device tinyusb_host tinyusb_pico_pio_usb xinput_host)
rp2040_family_configure_example_warnings(${PROJECT})
target_compile_options(${PROJECT} PUBLIC 
    -Wno-error 
    -Wno-shadow
    -Wno-cast-align
    -Wno-cast-qual
    -Wno-redundant-decls
    -Wno-sign-conversion
    -Wno-conversion
    -Wno-sign-compare
    -Wno-unused-function
    -Wno-format
    -Wno-suggest-attribute=format
    -Wno-unused-parameter
    -Wno-conversion
    -Wno-sequence-point
    -Wno-unused-variable
    -Wno-implicit-fallthrough
    -Wno-array-bounds
    -Wno-type-limits
    -Wno-parentheses
)

pico_enable_stdio_uart(${PROJECT} 1)
pico_enable_stdio_usb(${PROJECT} 0)
# display the all the target_link_libraries
get_target_property(LINK_LIBS ${PROJECT} LINK_LIBRARIES)
message(STATUS "Target ${PROJECT} link libraries: ${LINK_LIBS}")
