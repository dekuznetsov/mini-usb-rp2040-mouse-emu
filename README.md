# mini-usb-rp2040-mouse-emu

USB mouse jiggler firmware for the Raspberry Pi Pico (RP2040). The device enumerates as a HID mouse and sends small random relative moves every 30–50 seconds to keep a PC from going idle/locking, while spoofing the USB identity (VID/PID/manufacturer/product strings) of a real mouse already connected to the host so it doesn't show up as an obviously custom device.

Built with the [Pico SDK](https://github.com/raspberrypi/pico-sdk) (2.1.1) and [TinyUSB](https://github.com/hathach/tinyusb), using CMake + Ninja.

## How it works

- `src/main.c` — main loop: waits for USB mount, then every 30–50s sends a randomized relative mouse move (±2–6 px on X/Y) followed by a zero-move release. The RNG is seeded once at boot from ADC noise sampled on GPIO26.
- `src/usb_descriptors.c` — USB device, configuration, HID report, and string descriptors. VID/PID and manufacturer/product strings are set here to impersonate a specific real mouse (currently a Xiaomi mouse, VID `0x2717` / PID `0x5013`).
- `tusb_config.h` — TinyUSB configuration (device mode, HID class enabled).
- `CMakeLists.txt` — build target `mouse_emu`, links `pico_stdlib`, `pico_time`, `hardware_adc`, `tinyusb_device`, `tinyusb_board`.

## Spoofed identity

The VID/PID and descriptor strings are deliberately changed to match a real mouse device seen on the target PC, rather than left as the TinyUSB placeholder (`0xCAFE`/`0x4002`). If you adapt this project, update the constants and `string_desc_arr` at the top of `src/usb_descriptors.c` to match whatever device you intend to impersonate.

## Building

Requires the Pico SDK toolchain (CMake, Ninja, arm-none-eabi-gcc). On this machine it's installed under `~/.pico-sdk` and not on `PATH` by default:

```bash
export PATH="$HOME/.pico-sdk/cmake/v3.31.5/bin:$HOME/.pico-sdk/ninja/v1.12.1:$PATH"
cd build
cmake --build . --config Debug
```

This produces `build/Debug/mouse_emu.uf2`.

## Flashing

Put the Pico in BOOTSEL mode (hold BOOTSEL while plugging in, or via the reset circuit if wired), then copy the `.uf2` file to the mass-storage device that appears.

## Known issues

`PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1` is set in `CMakeLists.txt` to work around the RP2040-E5 USB errata (an issue that can prevent re-enumeration after certain host resets). This define is currently causing the board to hang instead of recovering — under investigation.

## License

[MIT](LICENSE)

## Disclaimer

This project is for personal use on hardware and PCs you own or are authorized to use. Spoofing another device's USB identity to disguise a custom device from a host you don't control or don't have permission to modify may violate device policies or terms of use — use responsibly.
