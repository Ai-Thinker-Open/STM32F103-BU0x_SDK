[![English](https://img.shields.io/badge/English-README-green)](README.md)

# STM32F103 BU03/BU04 UWB SDK

本仓库提供安信可 BU03-Kit/BU04 系列固件。开发板由 STM32F103T8 主控与 BU03 或 BU04 超宽带模组组成，支持双向测距（TWR）、PDoA 相关模式、带 STS-SDC 的 DS-TWR 以及独立 Decawave 示例。

> 目标型号更正：仓库中的 Keil 工程配置为 **STM32F103T8**，内存为 64 KiB Flash 和 20 KiB RAM。旧 README 中的 STM32F103CB 与工程、RTE 设备目录和烧录说明不一致，已按工程事实更正。

## 技术导航

- [代码入口与运行模式](docs/CODE_ENTRY.zh.md)
- [架构与组件边界](docs/ARCHITECTURE.zh.md)
- [构建与产物证据](docs/VALIDATION.zh.md)

## 目录结构

| 路径 | 作用 |
| --- | --- |
| `Components/Main` | 从复位到应用的入口与中断处理 |
| `Components/APP` | AT 命令、配置持久化、USB/UART 命令处理 |
| `Components/HAL` | 板级驱动、DW3000 系列驱动、OLED、LIS2DH12 和 USB |
| `Components/Examples` | 11 个可选择的 UWB 示例 |
| `Projects/USER` | Keil µVision 工程、RTE 设备文件和历史构建产物 |
| `doc/img` | Keil 与 J-Flash 操作截图 |

## 使用 Keil MDK 构建

1. 安装 Keil MDK，以及本工程要求的旧版 ARM Compiler 5 工具链。
2. 安装 ARM CMSIS 5.6.0 和 Keil STM32F1xx DFP 2.4.1；使用兼容新版本时应重新验证固件。
3. 打开 `Projects/USER/Project.uvprojx`。
4. 确认目标设备为 `STM32F103T8`，然后重建 `Project` 目标。
5. 检查完整构建日志，不能只以仓库中已有 HEX 文件作为当前构建成功依据。

工程历史记录使用 µVision 5.25.2 和 ARMCC 5.06 update 6。随仓库提供的 `aitcmd.lib`、`os.lib`、`pdoa.lib`、`twr.lib` 含预编译 ARM 对象，因此本仓库不声称 GNU Arm 可以直接替代原工具链。

## 正常固件模式

`Components/Examples/examples/examples_info/examples_defines.h` 中的 `EXAMPLE_DEMO` 默认为 `0`。板级和应用初始化完成后，固件读取持久化配置并选择：

- TWR/PDoA 模式：Node（`role=1`）进入 `node_start()`，Tag（`role=0`）进入 `tag_start()`。
- DS-TWR STS-SDC 模式：按角色进入 responder（`role=1`）或 initiator（`role=0`）。
- 命令工作模式：持续处理 USB 和 USART AT 命令。

配置命令实现在 `Components/APP/cmd_fn.c`。实际部署必须使用唯一设备地址，并针对真实硬件校准 UWB 天线延时；示例中的默认值不是量产校准数据。

## 可选择示例

将 `examples_defines.h` 中的 `EXAMPLE_DEMO` 设为 `1`，再在 `Components/HAL/DW/twr_pdoa/inc/example_selection.h` 中只启用一个示例宏。示例覆盖设备 ID 读取、简单 TX/RX、PDoA TX/RX、发送后等待响应、中断响应、接收后响应、DS-TWR STS-SDC initiator/responder 和定时休眠。

## 烧录

工程输出 `Projects/USER/Output/Project.hex`，可使用正确配置的 Keil/ST-Link 或 J-Flash 流程烧录。烧录前应选择真实的 STM32F103T8，核对供电、SWD、模组版本和 Flash 地址范围。

## 验证与限制

运行仓库与产物检查：

```bash
python3 tools/validate_repository.py
```

仓库中的 ARM ELF 与 Intel HEX 内容一致，对应的历史 ARMCC 日志报告 0 错误、0 警告。但这属于**历史产物证据**，不是本次重新构建结果；当前维护环境没有已授权的 ARMCC 5 和所需 Keil Pack。本次未执行烧录、UWB 测距、PDoA、天线校准、射频、USB/UART、功耗或法规验证。

## 许可证边界

仓库根目录没有统一许可证。部分 Decawave/Qorvo 来源文件与预编译库带有各自声明或限制性条款。仓库公开并不自动授予无限制再分发或产品使用权，复用前需逐项确认适用条款。

## 资源

- [安信可官网](https://www.ai-thinker.com/)
- [UWB 使用指南](https://fcniufr8ibx1.feishu.cn/wiki/space/7454451041846034460?ccm_open_type=lark_wiki_spaceLink&open_tab_from=wiki_home)
- [Keil MDK](https://www.keil.com/download/product/)
- [Keil STM32F1 设备包](https://www.keil.arm.com/packs/stm32f1xx_dfp-keil/boards/)
- [SEGGER J-Link/J-Flash](https://www.segger.com/downloads/jlink/)
