cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(mouse_emu C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(mouse_emu
    src/main.c
    src/mouse_jiggler.c
    src/usb_descriptors.c
)

target_include_directories(mouse_emu PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(mouse_emu PRIVATE
    CFG_TUD_HID=1
    PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1
)

target_link_libraries(mouse_emu
    pico_stdlib
    pico_time
    tinyusb_device
    tinyusb_board
)

pico_add_extra_outputs(mouse_emu)
pico_enable_stdio_usb(mouse_emu 0)
pico_enable_stdio_uart(mouse_emu 0)
