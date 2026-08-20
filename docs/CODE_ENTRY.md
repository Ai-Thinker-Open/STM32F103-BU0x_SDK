[![中文](https://img.shields.io/badge/中文-文档-blue)](CODE_ENTRY.zh.md)

# Code Entry and Operating Modes

## Startup path

1. `Projects/USER/RTE/Device/STM32F103T8/startup_stm32f10x_md.s` supplies the Cortex-M3 vector table and `Reset_Handler`.
2. The ARM C runtime initializes memory and calls `SystemInit()` from the RTE device source.
3. `Components/Main/main.c:main()` calls `init()`.
4. `init()` calls `SystemInit()`, configures HSE with a ×9 PLL in `RCC_Configuration_part()`, initializes board HAL through `Hal_Driver_Init()`, then initializes configuration and command processing through `App_Module_Init()`.
5. `main()` selects normal firmware (`nt_task()`) or standalone examples (`build_examples()`) using `EXAMPLE_DEMO`.

The source requests HSE and multiplies it by nine. The project metadata contains `CLOCK(12000000)`, but that IDE field does not prove the fitted crystal frequency. Confirm the BU03/BU04 board schematic and measured clock before timing-sensitive deployment.

## Normal firmware (`EXAMPLE_DEMO=0`)

`nt_task()` loads Flash-backed configuration and routes execution:

| Configuration | Route |
| --- | --- |
| `workmode=0`, address not configured | Process AT command events until a valid node address is stored |
| `twr_pdoa_mode=0`, `role=1` | `node_start()` |
| `twr_pdoa_mode=0`, `role=0` | `tag_start()` |
| `twr_pdoa_mode!=0`, `role=1` | `ds_twr_sts_sdc_responder()` |
| `twr_pdoa_mode!=0`, `role=0` | `ds_twr_sts_sdc_initiator()` |
| `workmode!=0` | Repeated `App_Module_Sys_Work_Mode_Event()` for USB/USART commands |

The Node, Tag, DS-TWR and OSAL implementations are partly delivered in the four precompiled libraries under `Components/HAL/DW/twr_pdoa/lib`.

## Standalone examples (`EXAMPLE_DEMO=1`)

`build_examples()` dispatches exactly one macro selected in `example_selection.h`. The eleven source-backed entries are:

- `read_dev_id()`
- `simple_tx()` / `simple_rx()`
- `simple_tx_pdoa()` / `simple_rx_pdoa()`
- `tx_wait_resp()` / `tx_wait_resp_int()` / `rx_send_resp()`
- `ds_twr_sts_sdc_init()` / `ds_twr_sts_sdc_resp()`
- `tx_timed_sleep()`

Only one example selection macro should be enabled at a time.

## Failure and blocking behavior

Several radio examples intentionally stop in infinite loops after DW initialization/configuration errors, and many UWB operations poll status registers. A missing module, IRQ, clock, or response can therefore block the example by design. Use a debugger or watchdog strategy during board integration.
