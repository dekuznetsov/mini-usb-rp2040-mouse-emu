// Board config for the "Mini USB RP2350" USB-A stick: a Nologo design
// (nologo.tech; Zephyr/Bridle board "mini_usb_rp2350") that several AliExpress
// sellers sell under their own names, e.g. AITEWIN ROBOT "MINI USB RP2350" and
// TENSTAR ROBOT "RP2350-USB 16MB". Despite the TENSTAR name it is NOT the
// Waveshare RP2350-USB-A: the pinout differs.
//
// Verified on hardware: BOOTSEL VID_2E8A&PID_000F, WS2812 on GPIO22 (GRB).
// Everything else comes from the Nologo schematic and Bridle docs.
//
// Hardware:
// - RP2350A (A2 stepping), 12 MHz crystal, W25Q128JV 16 MB QSPI flash (a 4 MB
//   W25Q32JV variant also exists; build it with -DPICO_FLASH_SIZE_BYTES=4194304),
//   ME6231C33 3.3 V LDO (500 mA) fed from USB 5 V.
// - USB-A plug on the native USB controller. BOOT and RESET buttons.
// - WS2812 RGB LED on GPIO22, powered from 3V3.
// - Qwiic/STEMMA QT connector (SH1.0 4-pin): GND, 3V3, GPIO16 SDA, GPIO17 SCL,
//   with 10k pull-ups to 3V3 on the board (not verified on hardware).
// - Headers: GPIO0-8 | 5V(VBUS, no diode), GND, 3V3, GPIO29-26, GPIO15, GPIO14.
//   Bottom pads: GPIO9-13, GPIO18-25. SWD is not broken out.
// - No SMPS, so there is no PICO_SMPS_MODE_PIN, unlike the Pico 2 and the Waveshare boards.
// - RP2350-E9: internal pull-downs are unreliable on A2. Use external
//   resistors or pull-ups for buttons.

// pico_cmake_set PICO_PLATFORM=rp2350

#ifndef _BOARDS_MINI_USB_RP2350_H
#define _BOARDS_MINI_USB_RP2350_H

// For board detection
#define MINI_USB_RP2350

// --- RP2350 VARIANT ---
#define PICO_RP2350A 1

// --- UART ---
#ifndef PICO_DEFAULT_UART
#define PICO_DEFAULT_UART 0
#endif
#ifndef PICO_DEFAULT_UART_TX_PIN
#define PICO_DEFAULT_UART_TX_PIN 0
#endif
#ifndef PICO_DEFAULT_UART_RX_PIN
#define PICO_DEFAULT_UART_RX_PIN 1
#endif

// --- LED ---
#ifndef PICO_DEFAULT_WS2812_PIN
#define PICO_DEFAULT_WS2812_PIN 22
#endif

// --- I2C (Qwiic connector) ---
#ifndef PICO_DEFAULT_I2C
#define PICO_DEFAULT_I2C 0
#endif
#ifndef PICO_DEFAULT_I2C_SDA_PIN
#define PICO_DEFAULT_I2C_SDA_PIN 16
#endif
#ifndef PICO_DEFAULT_I2C_SCL_PIN
#define PICO_DEFAULT_I2C_SCL_PIN 17
#endif

// --- SPI (GPIO4-7 on the left header, as in Bridle) ---
#ifndef PICO_DEFAULT_SPI
#define PICO_DEFAULT_SPI 0
#endif
#ifndef PICO_DEFAULT_SPI_RX_PIN
#define PICO_DEFAULT_SPI_RX_PIN 4
#endif
#ifndef PICO_DEFAULT_SPI_CSN_PIN
#define PICO_DEFAULT_SPI_CSN_PIN 5
#endif
#ifndef PICO_DEFAULT_SPI_SCK_PIN
#define PICO_DEFAULT_SPI_SCK_PIN 6
#endif
#ifndef PICO_DEFAULT_SPI_TX_PIN
#define PICO_DEFAULT_SPI_TX_PIN 7
#endif

// --- FLASH ---
#define PICO_BOOT_STAGE2_CHOOSE_W25Q080 1

#ifndef PICO_FLASH_SPI_CLKDIV
#define PICO_FLASH_SPI_CLKDIV 2
#endif

// pico_cmake_set_default PICO_FLASH_SIZE_BYTES = (16 * 1024 * 1024)
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (16 * 1024 * 1024)
#endif

// pico_cmake_set_default PICO_RP2350_A2_SUPPORTED = 1
#ifndef PICO_RP2350_A2_SUPPORTED
#define PICO_RP2350_A2_SUPPORTED 1
#endif

#endif
