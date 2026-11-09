cmake_minimum_required(VERSION 3.13)

# Target board: the "MINI USB RP2350" stick by default (boards/mini_usb_rp2350.h:
# WS2812 RGB LED on GPIO22). Override with -DPICO_BOARD=<board>, e.g. pico2, or pico
# for the RP2040.
list(APPEND PICO_BOARD_HEADER_DIRS ${CMAKE_CURRENT_LIST_DIR}/boards)
if (NOT DEFINED PICO_BOARD)
    set(PICO_BOARD mini_usb_rp2350 CACHE STRING "Board type")
endif()

# Use the prebuilt pioasm from the Pico SDK tools install (~/.pico-sdk)
# instead of building it from source, which fails with the host compiler here.
if (NOT DEFINED pioasm_DIR)
    if (DEFINED ENV{USERPROFILE})
        file(TO_CMAKE_PATH "$ENV{USERPROFILE}" _home)
    else()
        set(_home "$ENV{HOME}")
    endif()
    if (EXISTS "${_home}/.pico-sdk/tools/2.1.1/pioasm/pioasmConfig.cmake")
        set(pioasm_DIR "${_home}/.pico-sdk/tools/2.1.1/pioasm")
    endif()
endif()

include(pico_sdk_import.cmake)

project(mouse_emu C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(mouse_emu
    src/main.c
    src/mouse_jiggler.c
    src/usb_descriptors.c
    src/status_led.c
)

pico_generate_pio_header(mouse_emu ${CMAKE_CURRENT_SOURCE_DIR}/src/ws2812.pio)

target_include_directories(mouse_emu PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(mouse_emu PRIVATE
    CFG_TUD_HID=1
)

# RP2040-E5 USB enumeration errata workaround. The bug is fixed in RP2350
# silicon, and the SDK only provides the workaround for RP2040.
if (PICO_PLATFORM STREQUAL "rp2040")
    target_compile_definitions(mouse_emu PRIVATE
        PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1
    )
endif()

target_link_libraries(mouse_emu
    pico_stdlib
    pico_time
    hardware_pio
    tinyusb_device
    tinyusb_board
)

pico_add_extra_outputs(mouse_emu)
pico_enable_stdio_usb(mouse_emu 0)
pico_enable_stdio_uart(mouse_emu 0)
