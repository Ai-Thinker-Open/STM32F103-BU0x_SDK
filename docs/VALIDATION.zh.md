[![English](https://img.shields.io/badge/English-Docs-green)](VALIDATION.md)

# 验证证据

## 本次维护检查（2026-08-20）

`python3 tools/validate_repository.py` 检查：

- Keil 目标、内存映射、ARMCC 版本和所需 Pack 声明；
- 工程 50 个本地文件引用以及全部已配置本地包含目录；
- 4 个预期预编译库和 11 个示例入口源码；
- 正常 Node/Tag 与 DS-TWR 运行符号；
- 中英文文档配对、徽章和本地链接；
- 仓库 ARM ELF 的文件头、加载段、入口，以及 Intel HEX 校验和与内容；
- 历史构建日志结论和下列固定 SHA-256 标识。

校验器在 Windows Python 3 和 Linux Python 3 上均可运行，不依赖第三方包。

## 历史构建证据

构建日志和产物于 2024-09-29 一同提交在 `e68b65b1aac410b39c954c40aa02b9c5a6294937`。日志记录：

- µVision 5.25.2
- ARMCC 5.06 update 6 build 750
- ARM CMSIS 5.6.0、Keil STM32F1xx DFP 2.4.1
- `Code=64868`、`RO-data=4304`、`RW-data=5084`、`ZI-data=10644`
- 0 错误、0 警告

独立产物检查得到：

- AXF：ELF32 小端 ARM EABI5，入口 `0x080000ed`
- 加载镜像：地址 `0x08000000`，文件大小 70,164 字节，内存大小 84,900 字节
- GNU `size` 解释：text 69,172、data 992、BSS 10,644、合计 80,808 字节
- AXF SHA-256：`3176233a9523ad799439f1ca97fe51c36d115f17d7a2c33ebf9c2525660156a4`
- HEX SHA-256：`575a2c3f59cd040a71a709e8f73977978eb10f6b126b297cfb5a779b6211a1b1`

校验器会解析 Intel HEX、检查每条记录校验和，并确认所有 HEX 载荷字节与 AXF 加载段对应字节一致。这能证明两个仓库产物内部一致，但不能证明它们由当前源码即时重新生成。

## 为何不声称本次重新构建

当前维护主机没有已授权的 ARMCC 5 和所需 Keil Pack。工程还链接 4 个 ARMCC 时代预编译库并依赖 microlib ABI 设置，使用 GNU Arm 替代不能构成等价验证。因此不能仅凭历史日志获得当前构建或当前编译告警分数。

## 尚未验证

- 从最终仓库树执行新的 Keil 重建
- STM32F103T8 实板烧录或启动
- BU03/BU04 SPI/IRQ 通信
- Node/Tag、TWR、PDoA 或 DS-TWR 运行
- UWB 天线延时校准与测距精度
- 实物 USB/USART AT 命令
- 射频性能、功耗、压力或法规符合性
