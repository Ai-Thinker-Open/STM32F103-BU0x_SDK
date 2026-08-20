[![中文](https://img.shields.io/badge/中文-README-blue)](README.zh.md)

# STM32F103 BU03/BU04 UWB SDK

This repository contains firmware for the Ai-Thinker BU03-Kit/BU04 family. The board combines an STM32F103T8 host MCU with a BU03 or BU04 ultra-wideband module and supports two-way ranging (TWR), PDoA-related operation, DS-TWR with STS-SDC, and standalone Decawave examples.

> Target correction: the checked-in Keil project is configured for **STM32F103T8**, with 64 KiB Flash and 20 KiB RAM. The former README reference to STM32F103CB contradicted the project, RTE device directory, and flashing instructions.

## Technical navigation

- [Code entry and operating modes](docs/CODE_ENTRY.md)
- [Architecture and component boundaries](docs/ARCHITECTURE.md)
- [Build and artifact evidence](docs/VALIDATION.md)

## Repository layout

| Path | Purpose |
| --- | --- |
| `Components/Main` | Reset-to-application entry and interrupt handlers |
| `Components/APP` | AT commands, configuration persistence, USB/UART command handling |
| `Components/HAL` | Board drivers, DW3000-family driver, OLED, LIS2DH12 and USB support |
| `Components/Examples` | Eleven selectable UWB examples |
| `Projects/USER` | Keil µVision project, RTE device files, historical build outputs |
| `doc/img` | Keil and J-Flash screenshots |

## Build with Keil MDK

1. Install Keil MDK with the legacy ARM Compiler 5 toolchain required by this project.
2. Install ARM CMSIS 5.6.0 and Keil STM32F1xx DFP 2.4.1, or compatible packs after validating the generated firmware.
3. Open `Projects/USER/Project.uvprojx`.
4. Confirm the selected device is `STM32F103T8` and rebuild target `Project`.
5. Inspect the complete build log; do not rely only on the presence of the checked-in HEX file.

The project was last recorded as built with µVision 5.25.2 and ARMCC 5.06 update 6. Four supplied libraries (`aitcmd.lib`, `os.lib`, `pdoa.lib`, and `twr.lib`) contain precompiled ARM objects, so GNU Arm is not claimed as a supported replacement toolchain.

## Normal firmware mode

`EXAMPLE_DEMO` defaults to `0`. After board and application initialization, the firmware loads persistent configuration and selects:

- TWR/PDoA mode: Node (`role=1`) calls `node_start()`; Tag (`role=0`) calls `tag_start()`.
- DS-TWR STS-SDC mode: responder (`role=1`) or initiator (`role=0`).
- Command work mode: continuously services USB and USART AT commands.

Configuration is controlled through the AT command implementation in `Components/APP/cmd_fn.c`. Use unique addresses and calibrate UWB antenna delays for the actual hardware; the examples contain development defaults that are not production calibration data.

## Selectable examples

Set `EXAMPLE_DEMO` to `1` in `Components/Examples/examples/examples_info/examples_defines.h`, then enable exactly one example macro in `Components/HAL/DW/twr_pdoa/inc/example_selection.h`. Available examples cover device-ID read, simple TX/RX, PDoA TX/RX, TX/wait-response, interrupt response, RX/send-response, DS-TWR STS-SDC initiator/responder, and timed sleep.

## Flashing

The project generates `Projects/USER/Output/Project.hex`. It can be programmed with a correctly configured Keil/ST-Link or J-Flash workflow. Select the actual STM32F103T8 target and verify supply voltage, SWD connection, module revision, and flash range before programming.

## Validation and limitations

Run the repository/artifact checks with:

```bash
python3 tools/validate_repository.py
```

The checked-in ARM ELF and Intel HEX are mutually consistent, and their associated historical ARMCC build log reports zero errors and zero warnings. This is **historical artifact evidence**, not a fresh rebuild. The current maintenance environment does not contain licensed ARMCC 5 or the required Keil packs. No flashing, UWB ranging, PDoA, antenna calibration, RF, USB/UART, power, or regulatory test was performed.

## Licensing boundary

The repository has no root-level license. Several Decawave/Qorvo-derived files and precompiled libraries carry their own notices or restricted terms. “Open repository” does not automatically grant unrestricted redistribution or product-use rights; review every applicable component notice before reuse.

## Resources

- [Ai-Thinker website](https://www.ai-thinker.com/)
- [UWB usage guide](https://fcniufr8ibx1.feishu.cn/wiki/space/7454451041846034460?ccm_open_type=lark_wiki_spaceLink&open_tab_from=wiki_home)
- [Keil MDK](https://www.keil.com/download/product/)
- [Keil STM32F1 device pack](https://www.keil.arm.com/packs/stm32f1xx_dfp-keil/boards/)
- [SEGGER J-Link/J-Flash](https://www.segger.com/downloads/jlink/)
