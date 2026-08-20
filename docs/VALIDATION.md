[![中文](https://img.shields.io/badge/中文-文档-blue)](VALIDATION.zh.md)

# Validation Evidence

## Current maintenance checks (2026-08-20)

`python3 tools/validate_repository.py` verifies:

- the Keil target, memory map, ARMCC version and required pack declarations;
- all 50 local project file references and all configured local include directories;
- the four expected precompiled libraries and eleven example entry sources;
- normal Node/Tag and DS-TWR execution symbols;
- English/Chinese document pairs, badges and local links;
- the tracked ARM ELF header, load segment, entry point and Intel HEX checksums/content;
- the historical build-log result and fixed SHA-256 identities below.

The validator passes on Windows Python 3 and Linux Python 3 without third-party packages.

## Historical build evidence

The build log and artifacts were committed together in `e68b65b1aac410b39c954c40aa02b9c5a6294937` on 2024-09-29. The log records:

- µVision 5.25.2
- ARMCC 5.06 update 6 build 750
- ARM CMSIS 5.6.0 and Keil STM32F1xx DFP 2.4.1
- `Code=64868`, `RO-data=4304`, `RW-data=5084`, `ZI-data=10644`
- 0 errors and 0 warnings

Independent artifact inspection finds:

- AXF: ELF32 little-endian ARM EABI5, entry `0x080000ed`
- Load image: `0x08000000`, file size 70,164 bytes, memory size 84,900 bytes
- GNU `size` interpretation: text 69,172; data 992; BSS 10,644; total 80,808 bytes
- AXF SHA-256: `3176233a9523ad799439f1ca97fe51c36d115f17d7a2c33ebf9c2525660156a4`
- HEX SHA-256: `575a2c3f59cd040a71a709e8f73977978eb10f6b126b297cfb5a779b6211a1b1`

The validator decodes Intel HEX records, checks every record checksum, and confirms every HEX payload byte equals the corresponding byte in the AXF load segment. This proves the two tracked artifacts are internally consistent; it does not prove they were freshly produced from the current source.

## Why no fresh build is claimed

The maintenance host does not have licensed ARMCC 5 or the required Keil packs. The project also links four supplied ARMCC-era precompiled libraries and relies on microlib ABI settings, so substituting GNU Arm would not be an equivalent validation. No current-build or compiler-warning points should be awarded from the historical log alone.

## Not verified

- A fresh Keil rebuild from the final repository tree
- Flashing or booting STM32F103T8 hardware
- BU03/BU04 SPI/IRQ communication
- Node/Tag, TWR, PDoA or DS-TWR operation
- UWB antenna-delay calibration and ranging accuracy
- USB/USART AT commands on hardware
- RF performance, power consumption, stress or regulatory compliance
