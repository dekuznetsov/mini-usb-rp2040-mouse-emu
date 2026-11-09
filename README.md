# mini-usb-rp2040-mouse-emu

USB mouse jiggler firmware for the **MINI USB RP2350** USB-A stick. The device enumerates as a HID mouse and every 30 seconds nudges the cursor a short way and back again to keep a PC from going idle/locking, while spoofing the USB identity (VID/PID/manufacturer/product strings) of a real mouse already connected to the host so it doesn't show up as an obviously custom device.

It still builds for the Raspberry Pi Pico 2 (RP2350) and the original Pico (RP2040), which is where the project started (hence the name).

Built with the [Pico SDK](https://github.com/raspberrypi/pico-sdk) (2.1.1) and [TinyUSB](https://github.com/hathach/tinyusb), using CMake + Ninja.

## Hardware

The target board is the "MINI USB RP2350 Development Board" by AITEWIN ROBOT ([AliExpress](https://www.aliexpress.com/item/1005009656371359.html), variant "RP2350 USB 16MB"):

| | |
|---|---|
| MCU | RP2350A, dual Cortex-M33 |
| Flash | 16 MB (the firmware uses ~50 KB; the build assumes 4 MB, which is fine) |
| USB | USB-A plug, plugs straight into the PC |
| Buttons | BOOT (BOOTSEL), RESET |
| LED | WS2812 RGB LED, data on **GPIO22**, GRB colour order |
| Other | JST 1.0 mm 4-pin connector, castellated GPIO pads (unused) |

The board isn't in the Pico SDK, so `boards/mini_usb_rp2350.h` describes it: the same layout as `waveshare_rp2350_one`, but with the LED on GPIO22 instead of GPIO16. The vendor doesn't document the LED pin, so it was found on the hardware by sending WS2812 frames to GPIOs one at a time.

### Status LED

| State | MINI USB RP2350 (WS2812) | Pico / Pico 2 (plain LED) |
|---|---|---|
| Idle | dim green, always on | off |
| Jiggling (~0.25 s every 30 s) | brighter lime green | on |

## How it works

- `src/main.c`: TinyUSB callbacks and the main loop (`tud_task()` + `mouse_jiggler_task()`).
- `src/mouse_jiggler.c`: the movement algorithm, ported from the STM32 [mouseEmulator](https://github.com/dekuznetsov/mouseEmulator) project. The first jiggle comes 30 s after mount, then one every 30 s. During a jiggle the status LED switches to its "active" state and the cursor moves 12 steps of +3/+3 px (one report per 10 ms). It then moves 12 steps back, so it ends where it started, and the LED returns to idle.
  - A report is only sent after the host has picked up the previous one. That way the forward and back moves always match, and the cursor doesn't drift.
  - It's a non-blocking state machine, so USB keeps being serviced during a jiggle.
- `src/status_led.c`, `src/ws2812.pio`: the status LED.
  - If the board defines `PICO_DEFAULT_WS2812_PIN`, the WS2812 is driven over PIO.
  - Otherwise the plain LED is driven via `board_led_write()`.
- `src/usb_descriptors.c`: USB device, configuration, HID report and string descriptors. The VID/PID and manufacturer/product strings are set here to impersonate a specific real mouse (currently a Xiaomi mouse, VID `0x2717` / PID `0x5013`).
- `boards/mini_usb_rp2350.h`: the board config for the MINI USB RP2350 (the default board).
- `tusb_config.h`: TinyUSB configuration (device mode, HID class enabled).
- `CMakeLists.txt`: the build target `mouse_emu`. It links `pico_stdlib`, `pico_time`, `hardware_pio`, `tinyusb_device` and `tinyusb_board`, and selects the board.

## Spoofed identity

The VID/PID and descriptor strings are deliberately set to match a real mouse seen on the target PC, rather than left as the TinyUSB placeholder (`0xCAFE`/`0x4002`). If you adapt this project, change the constants and `string_desc_arr` at the top of `src/usb_descriptors.c` to the device you want to impersonate.

## Building

You need the Pico SDK toolchain (CMake, Ninja, arm-none-eabi-gcc). It's typically installed under `~/.pico-sdk` and isn't on `PATH` by default:

```bash
PICO_SDK_TOOLS="$HOME/.pico-sdk"
export PATH="$PICO_SDK_TOOLS/cmake/v3.31.5/bin:$PICO_SDK_TOOLS/ninja/v1.12.1:$PATH"

# MINI USB RP2350 (the default board)
cmake -S . -B build-rp2350 -G "Ninja Multi-Config" -DPICO_SDK_PATH="$PICO_SDK_TOOLS/sdk/2.1.1"
cmake --build build-rp2350 --config Debug
```

This produces `build-rp2350/mouse_emu.uf2`.

For other boards, pass `PICO_BOARD` and use a separate build directory:

```bash
# Raspberry Pi Pico 2 (RP2350)
cmake -S . -B build-pico2 -G "Ninja Multi-Config" -DPICO_BOARD=pico2 -DPICO_SDK_PATH="$PICO_SDK_TOOLS/sdk/2.1.1"

# Raspberry Pi Pico (RP2040)
cmake -S . -B build -G "Ninja Multi-Config" -DPICO_BOARD=pico -DPICO_SDK_PATH="$PICO_SDK_TOOLS/sdk/2.1.1"
```

The prebuilt `pioasm` from `~/.pico-sdk/tools/2.1.1` is picked up automatically, because building it from source fails with some host compilers.

## Flashing

1. Hold **BOOT** while plugging the stick into a USB port, then release it. A mass-storage drive named `RP2350` appears (`RPI-RP2` on an RP2040 Pico).
2. Copy `build-rp2350/mouse_emu.uf2` onto that drive.
3. The board reboots by itself and enumerates as the spoofed mouse. The LED goes dim green.

## Known issues

- **Loose USB-A plug.** The stick's USB-A plug is a bit too thin and can seat crookedly in a port. While it settles, Windows may briefly show `Unknown USB Device (Device Descriptor Request Failed)` before the board enumerates normally. This is a mechanical fit issue, not a firmware or hub problem. If it persists, reseat the stick or use an extension cable or a snugger port. (This glitch was once blamed on the RP2040-E5 USB errata. The SDK workaround for that, `PICO_RP2040_USB_DEVICE_ENUMERATION_FIX`, made the RP2040 hang, so it's no longer enabled.)
- **WS2812 keeps its last colour** until it loses power. After flashing firmware that doesn't drive GPIO22 (for example a build for another board), the LED can stay lit in whatever colour it last received. Unplug and replug to clear it.

## License

[MIT](LICENSE)

## Disclaimer

This project is for personal use on hardware and PCs you own or are authorized to use. Spoofing another device's USB identity to disguise a custom device from a host you don't control or don't have permission to modify may violate device policies or terms of use. Use responsibly.
