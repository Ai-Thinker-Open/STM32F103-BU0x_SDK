[![English](https://img.shields.io/badge/English-Docs-green)](CODE_ENTRY.md)

# 代码入口与运行模式

## 启动路径

1. `Projects/USER/RTE/Device/STM32F103T8/startup_stm32f10x_md.s` 提供 Cortex-M3 向量表和 `Reset_Handler`。
2. ARM C 运行时初始化内存，并调用 RTE 设备源码中的 `SystemInit()`。
3. 运行时进入 `Components/Main/main.c:main()`，随后调用 `init()`。
4. `init()` 调用 `SystemInit()`，由 `RCC_Configuration_part()` 配置 HSE ×9 PLL，再通过 `Hal_Driver_Init()` 初始化板级 HAL，通过 `App_Module_Init()` 初始化配置与命令处理。
5. `main()` 根据 `EXAMPLE_DEMO` 选择正常固件 `nt_task()` 或独立示例 `build_examples()`。

源码启用 HSE 并进行 9 倍频。工程元数据包含 `CLOCK(12000000)`，但 IDE 字段不能证明板上实际晶振频率。涉及精确时序的部署应核对 BU03/BU04 原理图并实测时钟。

## 正常固件（`EXAMPLE_DEMO=0`）

`nt_task()` 读取 Flash 配置并选择路径：

| 配置 | 路径 |
| --- | --- |
| `workmode=0` 且地址未配置 | 持续处理 AT 命令，直到保存有效 Node 地址 |
| `twr_pdoa_mode=0`、`role=1` | `node_start()` |
| `twr_pdoa_mode=0`、`role=0` | `tag_start()` |
| `twr_pdoa_mode!=0`、`role=1` | `ds_twr_sts_sdc_responder()` |
| `twr_pdoa_mode!=0`、`role=0` | `ds_twr_sts_sdc_initiator()` |
| `workmode!=0` | 重复执行 `App_Module_Sys_Work_Mode_Event()` 处理 USB/USART 命令 |

Node、Tag、DS-TWR 与 OSAL 的部分实现位于 `Components/HAL/DW/twr_pdoa/lib` 下的 4 个预编译库中。

## 独立示例（`EXAMPLE_DEMO=1`）

`build_examples()` 按 `example_selection.h` 中启用的一个宏分派。11 个有源码依据的入口为：

- `read_dev_id()`
- `simple_tx()` / `simple_rx()`
- `simple_tx_pdoa()` / `simple_rx_pdoa()`
- `tx_wait_resp()` / `tx_wait_resp_int()` / `rx_send_resp()`
- `ds_twr_sts_sdc_init()` / `ds_twr_sts_sdc_resp()`
- `tx_timed_sleep()`

一次只能启用一个示例选择宏。

## 失败与阻塞行为

多个射频示例在 DW 初始化或配置失败后有意进入无限循环，许多 UWB 操作会轮询状态寄存器。模组、IRQ、时钟或响应缺失时，示例可能按设计阻塞。板级联调阶段应配合调试器或看门狗策略。
