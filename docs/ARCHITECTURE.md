[![中文](https://img.shields.io/badge/中文-文档-blue)](ARCHITECTURE.zh.md)

# Architecture and Component Boundaries

## Layer map

| Layer | Main paths | Responsibility |
| --- | --- | --- |
| Startup and MCU support | `Projects/USER/RTE/Device/STM32F103T8` | Cortex-M3 vectors, system clock base and STM32 configuration |
| Application control | `Components/Main`, `Components/APP` | Boot policy, persistent configuration, AT commands, mode dispatch |
| Board HAL | `Components/HAL/HAL` | Flash, SPI, I2C, timer, LED, USART and USB adapters |
| Peripheral support | `Components/HAL/USB`, `OLED`, `LIS2DH12` | Host command transport and local peripherals |
| UWB driver | `Components/HAL/DW/decadriver` | DW device registers and driver API |
| UWB algorithms | `Components/HAL/DW/twr_pdoa` | Node/Tag, TWR/PDoA, DS-TWR and shared configuration |
| Examples | `Components/Examples` | Eleven selectable radio demonstrations |
| Build | `Projects/USER/Project.uvprojx` | ARMCC 5 source list, packs, flags and output settings |

## Open-source boundary

The repository includes substantial C source for board, application, USB, device-driver and examples, but the following behavior is only available as ARM object libraries:

- `aitcmd.lib`: command-related object
- `os.lib`: OSAL object
- `pdoa.lib`: DS-TWR/PDoA and shared objects
- `twr.lib`: Node, Tag, UWB and configuration objects

These archives are Cortex-M3 EABI objects produced for ARMCC-era settings (`wchar_t=2`, small enums and microlib in the linker command). Their presence is not equivalent to source availability or a root repository license.

## Target and memory

The Keil target and RTE directory identify `STM32F103T8`. The project memory map declares 64 KiB Flash at `0x08000000` and 20 KiB RAM at `0x20000000`. The tracked AXF entry is `0x080000ed`.

## Build dependency boundary

The project references 50 local files and four local libraries, all present. STM32 StdPeriph sources and CMSIS headers are supplied through ARM CMSIS 5.6.0 and Keil STM32F1xx DFP 2.4.1 packs rather than vendored in this repository. Two stale local include directories (`../LIB/inc`, `../CMSIS`) were removed because they do not exist and the required content is pack-managed.

## Data and control flow

USB/USART input enters the command queues, is parsed by `Components/APP`, and updates Flash-backed user configuration. The normal task uses that configuration to select Node/Tag or DS-TWR roles. Board HAL translates application and UWB-driver requests into STM32 SPI/GPIO/timer operations; radio results are formatted and sent to the configured output paths.

## Integration risks

- Addresses and antenna delays require per-device configuration/calibration.
- Polling loops and intentional fatal loops can block indefinitely.
- The historical project depends on a legacy commercial compiler and pack versions.
- Precompiled libraries limit auditability and GNU-toolchain portability.
- RF behavior, ranging accuracy and regulatory compliance cannot be inferred from repository structure.
