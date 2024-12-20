/*! ----------------------------------------------------------------------------
 *  @file    simple_rx_pdoa.c
 *  @brief   This examples prints the PDOA value to the virtual COM.
 *           The transmitter should be simple_tx_pdoa.c
 *           See note 3 regarding calibration and offset
 *
 * @attention
 *
 * Copyright 2019 - 2020 (c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author Decawave
 */

#include <deca_device_api.h>
#include <deca_regs.h>
#include <deca_vals.h>
#include <shared_defines.h>
#include <shared_functions.h>
#include <example_selection.h>
#include "uwb.h"

#if defined(TEST_SIMPLE_RX_PDOA)

static void rx_ok_cb(const dwt_cb_data_t *cb_data);
static void rx_err_cb(const dwt_cb_data_t *cb_data);

/* Example application name and version to display on LCD screen. */
#define APP_NAME "PDOA example"

/* Default communication configuration. We use default non-STS DW mode. see note 2*/
static dwt_config_t config = {
        5,               /* 信道号. Channel number. */
        DWT_PLEN_128,     /* Preamble length. Used in TX only. */
        DWT_PAC8,        /* Preamble acquisition chunk size. Used in RX only. */
        9,               /* Tx前导码. TX preamble code. Used in TX only. */
        9,               /* Rx前导码. RX preamble code. Used in RX only. */
        1,               /* 帧分隔符模式. 0 to use standard 8 symbol SFD, 1 to use non-standard 8 symbol, 2 for non-standard 16 symbol SFD and 3 for 4z 8 symbol SDF type */
        DWT_BR_6M8,      /* 数据速率. Data rate. */
        DWT_PHRMODE_STD, /* 物理层头模式. PHY header mode. */
        DWT_PHRRATE_STD, /* 物理层头速率. PHY header rate. */
        (129 + 8 - 8),    /* 帧分隔符超时. SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
        (DWT_STS_MODE_1 | DWT_STS_MODE_SDC), /* STS模式. STS enabled */
        DWT_STS_LEN_256, /* STS长度. Cipher length see allowed values in Enum dwt_sts_lengths_e */
        DWT_PDOA_M3      /* PDOA mode 3 */
};


int16_t   pdoa_val=0;
uint8_t   pdoa_message_data[40];//Will hold the data to send to the virtual COM

/**
 * Application entry point.
 */
int simple_rx_pdoa(void)
{

    int16_t   last_pdoa_val=0;

    /* 串口输出应用名称. Sends application name to test_run_info function. */
    _dbg_printf((unsigned char *)APP_NAME);

    /* 配置SPI快速率. Configure SPI rate, DW IC supports up to 38 MHz */
    port_set_dw_ic_spi_fastrate();

    /* 硬复位DW3000模块. Reset DW IC */
    reset_DWIC(); /* Target specific drive of RSTn line into DW IC low for a period. */

    Sleep(2); // Time needed for DW3000 to start up (transition from INIT_RC to IDLE_RC

    /* 检查DW3000模块是否处于IDLE_RC */
    while (!dwt_checkidlerc()) /* Need to make sure DW IC is in IDLE_RC before proceeding */
    { };

    /* 初始化DW3000模块 */
    if (dwt_initialise(DWT_DW_IDLE) == DWT_ERROR)
    {
        _dbg_printf((unsigned char *)"INIT FAILED");
        while (1)
        { };
    }

    /* 配置DW3000信道参数. Configure DW3000. */
    if(dwt_configure(&config)) /* if the dwt_configure returns DWT_ERROR either the PLL or RX calibration has failed the host should reset the device */
    {
        _dbg_printf((unsigned char *)"CONFIG FAILED     ");
        while (1)
        { };
    }

    /* 注册rx回调函数. Register RX call-back. */
    dwt_setcallbacks(NULL, rx_ok_cb, rx_err_cb, rx_err_cb, NULL, NULL);

    /* 使能中断. Enable wanted interrupts (RX good frames and RX errors). */
    dwt_setinterrupt(SYS_ENABLE_LO_RXFCG_ENABLE_BIT_MASK | SYS_STATUS_ALL_RX_ERR, 0, DWT_ENABLE_INT);

    /* 清除中断. Clearing the SPI ready interrupt*/
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RCINIT_BIT_MASK | SYS_STATUS_SPIRDY_BIT_MASK);

    /* Install DW IC IRQ handler. */
//    port_set_dwic_isr(dwt_isr);

    /* 立即开启接受. Activate reception immediately. See NOTE 1 below. */
    dwt_rxenable(DWT_START_RX_IMMEDIATE);

    port_EnableEXT_IRQ();

    _dbg_printf("PDoA初始化成功\n");

    /*loop forever receiving frames*/
    while (1)
    {
        if (last_pdoa_val!=pdoa_val)
        {
            last_pdoa_val=pdoa_val;
            sprintf((char *)&pdoa_message_data,"PDOA val = %d",last_pdoa_val);
            _dbg_printf((unsigned char *)&pdoa_message_data);
        }

    }
    return DWT_SUCCESS;
}


/*! ------------------------------------------------------------------------------------------------------------------
 * @fn rx_ok_cb()
 *
 * @brief Callback to process RX good frame events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
static void rx_ok_cb(const dwt_cb_data_t *cb_data)
{
    int16_t cpqual;
    // 检查STS质量. checking STS quality see note 4
    if(dwt_readstsquality(&cpqual))
    {
        /* 读取相位差*/
        pdoa_val=dwt_readpdoa();
    }
    dwt_rxenable(DWT_START_RX_IMMEDIATE);
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn rx_err_cb()
 *
 * @brief Callback to process RX error and timeout events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
static void rx_err_cb(const dwt_cb_data_t *cb_data)
{
    dwt_rxenable(DWT_START_RX_IMMEDIATE);
}

#endif

/*****************************************************************************************************************************************************
 * NOTES:
 *
 * 1. Manual reception activation is performed here but DW IC offers several features that can be used to handle more complex scenarios or to
 *    optimise system's overall performance (e.g. timeout after a given time, automatic re-enabling of reception in case of errors, etc.).
 * 2. This is the default configuration recommended for optimum performance. An offset between the clocks of the transmitter and receiver will
 *    occur. The DW3000 can tolerate a difference of +/- 20ppm. For optimum performance an offset of +/- 5ppm is recommended.
 * 3. A natural offset will always occur between any two boards. To combat this offset the transmitter and receiver should be placed
 *    with a real PDOA of 0 degrees. When the PDOA is calculated this will return a non-zero value. This value should be subtracted from all
 *    PDOA values obtained by the receiver in order to obtain a calibrated PDOA.
 * 4. If the STS quality is poor the returned PDoA value will not be accurate and as such will not be recorded
 ****************************************************************************************************************************************************/
