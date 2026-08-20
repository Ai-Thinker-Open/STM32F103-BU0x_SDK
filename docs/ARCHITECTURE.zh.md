[![English](https://img.shields.io/badge/English-Docs-green)](ARCHITECTURE.md)

# 架构与组件边界

## 分层结构

| 层级 | 主要路径 | 职责 |
| --- | --- | --- |
| 启动与 MCU 支持 | `Projects/USER/RTE/Device/STM32F103T8` | Cortex-M3 向量、系统时钟基础与 STM32 配置 |
| 应用控制 | `Components/Main`、`Components/APP` | 启动策略、持久化配置、AT 命令与模式分派 |
| 板级 HAL | `Components/HAL/HAL` | Flash、SPI、I2C、定时器、LED、USART 和 USB 适配 |
| 外设支持 | `Components/HAL/USB`、`OLED`、`LIS2DH12` | 主机命令传输与本地外设 |
| UWB 驱动 | `Components/HAL/DW/decadriver` | DW 设备寄存器与驱动 API |
| UWB 算法 | `Components/HAL/DW/twr_pdoa` | Node/Tag、TWR/PDoA、DS-TWR 与共享配置 |
| 示例 | `Components/Examples` | 11 个可选择的射频示例 |
| 构建 | `Projects/USER/Project.uvprojx` | ARMCC 5 源码列表、Pack、参数与输出设置 |

## 源码开放边界

仓库提供大量板级、应用、USB、设备驱动和示例 C 源码，但以下行为只以 ARM 对象库提供：

- `aitcmd.lib`：命令相关对象
- `os.lib`：OSAL 对象
- `pdoa.lib`：DS-TWR/PDoA 与共享对象
- `twr.lib`：Node、Tag、UWB 和配置对象

这些归档是面向 Cortex-M3 的 EABI 对象，沿用 ARMCC 时代设置（`wchar_t=2`、small enums，链接使用 microlib）。提供二进制库不等于提供源码，也不等于仓库具有统一许可证。

## 目标与内存

Keil 目标和 RTE 目录均为 `STM32F103T8`。工程内存映射声明 `0x08000000` 起始的 64 KiB Flash 与 `0x20000000` 起始的 20 KiB RAM。仓库 AXF 的入口为 `0x080000ed`。

## 构建依赖边界

工程引用 50 个本地文件和 4 个本地库，均存在。STM32 标准外设驱动与 CMSIS 头文件来自 ARM CMSIS 5.6.0、Keil STM32F1xx DFP 2.4.1，而不是随仓库完整提供。已移除不存在的 `../LIB/inc` 和 `../CMSIS` 两个旧本地包含目录；所需内容由 Pack 管理。

## 数据与控制流

USB/USART 输入进入命令队列，由 `Components/APP` 解析并更新 Flash 中的用户配置。正常任务按配置选择 Node/Tag 或 DS-TWR 角色。板级 HAL 将应用和 UWB 驱动请求转换为 STM32 SPI/GPIO/定时器操作，射频结果再格式化并发送到配置的输出通道。

## 集成风险

- 地址和天线延时必须逐设备配置/校准。
- 轮询与故障无限循环可能长期阻塞。
- 历史工程依赖旧版商业编译器和 Pack。
- 预编译库限制可审计性和 GNU 工具链可移植性。
- 不能仅从仓库结构推断射频性能、测距精度或法规符合性。
