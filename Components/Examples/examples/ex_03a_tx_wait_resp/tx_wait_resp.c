/*! ----------------------------------------------------------------------------
 *  @file    tx_wait_resp.c
 *  @brief   TX then wait for response example code
 *
 *           This is a simple code example that sends a frame and then turns on the DW IC receiver to receive a response. The response could be
 *           anything as no check is performed on it and it is only displayed in a local buffer but the sent frame is the one expected by the
 *           companion simple example "RX then send a response example code". After the response is received or if the reception timeouts, this code
 *           just go back to the sending of a new frame.
 *
 * @attention
 *
 * Copyright 2015-2020 (c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author Decawave
 */

#include <deca_device_api.h>
#include <deca_regs.h>
#include <shared_defines.h>
#include <example_selection.h>
#include "uwb.h"

#if defined(TEST_TX_WAIT_RESP)

/* Example application name */
#define APP_NAME "TX WAITRESP v1.0"

/* Default communication configuration. We use default non-STS DW mode. */
static dwt_config_t config = {
    5,               /* 信道号. Channel number. */
    DWT_PLEN_128,    /* Preamble length. Used in TX only. */
    DWT_PAC8,        /* Preamble acquisition chunk size. Used in RX only. */
    9,               /* Tx前导码. TX preamble code. Used in TX only. */
    9,               /* Rx前导码. RX preamble code. Used in RX only. */
    1,               /* 帧分隔符模式. 0 to use standard 8 symbol SFD, 1 to use non-standard 8 symbol, 2 for non-standard 16 symbol SFD and 3 for 4z 8 symbol SDF type */
    DWT_BR_6M8,      /* 数据速率. Data rate. */
    DWT_PHRMODE_STD, /* 物理层头模式. PHY header mode. */
    DWT_PHRRATE_STD, /* 物理层头速率. PHY header rate. */
    (129 + 8 - 8),   /* 帧分隔符超时. SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
    DWT_STS_MODE_OFF, /* STS模式. STS disabled */
    DWT_STS_LEN_64,   /* STS长度. STS length see allowed values in Enum dwt_sts_lengths_e */
    DWT_PDOA_M0      /* PDOA mode off */
};


/* The frame sent in this example is a blink encoded as per the ISO/IEC 24730-62:2013 standard. It is a 14-byte frame composed of the following fields:
 *     - byte 0: frame control (0xC5 to indicate a multipurpose frame using 64-bit addressing).
 *     - byte 1: sequence number, incremented for each new frame.
 *     - byte 2 -> 9: device ID, see NOTE 1 below.
 *     - byte 10: encoding header (0x43 to indicate no extended ID, temperature, or battery status is carried in the message).
 *     - byte 11: EXT header (0x02 to indicate tag is listening for a response immediately after this message).
 *     - byte 12/13: frame check-sum, automatically set by DW IC. */
static uint8_t tx_msg[] = {0xC5, 0, 'D', 'E', 'C', 'A', 'W', 'A', 'V', 'E', 0x43, 0x02, 0, 0};
/* Index to access to sequence number of the blink frame in the tx_msg array. */
#define BLINK_FRAME_SN_IDX 1

/* Inter-frame delay period, in milliseconds. */
#define TX_DELAY_MS 1000

/* Delay from end of transmission to activation of reception, expressed in UWB microseconds (1 uus is 512/499.2 microseconds). See NOTE 2 below. */
#define TX_TO_RX_DELAY_UUS 60

/* Receive response timeout, expressed in UWB microseconds. See NOTE 4 below. */
#define RX_RESP_TO_UUS 5000

/* Buffer to store received frame. See NOTE 5 below. */
static uint8_t rx_buffer[FRAME_LEN_MAX];

/* Values for the PG_DELAY and TX_POWER registers reflect the bandwidth and power of the spectrum at the current
 * temperature. These values can be calibrated prior to taking reference measurements. See NOTE 2 below. */
extern dwt_txconfig_t txconfig_options;

/**
 * Application entry point.
 */
int tx_wait_resp(void)
{
    /* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
    uint32_t status_reg = 0;
    /* Hold copy of frame length of frame received (if good) so that it can be examined at a debug breakpoint. */
    uint16_t frame_len = 0;

    /* 串口输出应用名称. Display application name on LCD. */
    _dbg_printf((unsigned char *)APP_NAME);

    /* 配置SPI快速率. Configure SPI rate, DW3000 supports up to 38 MHz */
    port_set_dw_ic_spi_fastrate();

    /* 硬复位DW3000模块. Reset DW IC */
    reset_DWIC(); /* Target specific drive of RSTn line into DW IC low for a period. */

    Sleep(2); // Time needed for DW3000 to start up (transition from INIT_RC to IDLE_RC, or could wait for SPIRDY event)

    /* 检查DW3000模块是否处于IDLE_RC */
    while (!dwt_checkidlerc()) /* Need to make sure DW IC is in IDLE_RC before proceeding */
    { };

    /* 初始化DW3000模块 */
    if (dwt_initialise(DWT_DW_INIT) == DWT_ERROR)
    {
        _dbg_printf((unsigned char *)"INIT FAILED     ");
        while (1)
        { };
    }

    /* Optionally Configure GPIOs to show TX/RX activity. See NOTE 10 below. */
    //dwt_setlnapamode(DWT_LNA_ENABLE | DWT_PA_ENABLE);
    /* Optionally enabling LEDs here for debug so that for each TX the D1 LED will flash on DW3000 red eval-shield boards. See Note 10 below.*/
    //dwt_setleds(DWT_LEDS_ENABLE | DWT_LEDS_INIT_BLINK) ;

    /* 配置DW3000信道参数. Configure DW IC. See NOTE 2 below. */
    if(dwt_configure(&config)) /* if the dwt_configure returns DWT_ERROR either the PLL or RX calibration has failed the host should reset the device */
    {
        _dbg_printf((unsigned char *)"CONFIG FAILED     ");
        while (1)
        { };
    }

    /* 配置DW3000发送频谱参数. Configure the TX spectrum parameters (power, PG delay and PG count) */
    dwt_configuretxrf(&txconfig_options);

    /* 配置DW3000发送完成后TX_TO_RX_DELAY_UUS开启接受.       Set delay to turn reception on after transmission of the frame. See NOTE 3 below. */
    dwt_setrxaftertxdelay(TX_TO_RX_DELAY_UUS);

    /* 配置DW3000接受超时RX_RESP_TO_UUS. Set response frame timeout. */
    dwt_setrxtimeout(RX_RESP_TO_UUS);

    /* Loop forever sending and receiving frames periodically. */
    while (1)
    {
        /* 写入待发送数据到DW3000准备发送,并设置发送长度. Write frame data to DW3000 and prepare transmission. See NOTE 7 below. */
        dwt_writetxdata(sizeof(tx_msg), tx_msg, 0); /* Zero offset in TX buffer. */
        dwt_writetxfctrl(sizeof(tx_msg), 0, 0); /* Zero offset in TX buffer, no ranging. */

        /* 立即发送发送，并有相应 Start transmission, indicating that a response is expected so that reception is enabled immediately after the frame is sent. */
        dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);

        /* 查询DW3000是否接受成功|接受超时|接受错误. We assume that the transmission is achieved normally, now poll for reception of a frame or error/timeout. See NOTE 8 below. */
        while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG_BIT_MASK | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
        { };

        if (status_reg & SYS_STATUS_RXFCG_BIT_MASK)
        {
            int i;

            /* Clear local RX buffer to avoid having leftovers from previous receptions. This is not necessary but is included here to aid reading
             * the RX buffer. */
            for (i = 0 ; i < FRAME_LEN_MAX; i++ )
            {
                rx_buffer[i] = 0;
            }

            /* 读取DW3000接收到的数据长度. A frame has been received, copy it to our local buffer. */
            frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_BIT_MASK;
            if (frame_len <= FRAME_LEN_MAX)
            {
                /* 读取DW3000接收到的数据.*/
                dwt_readrxdata(rx_buffer, frame_len, 0);
                _dbg_printf("接收到Resp\n");
            }

            /* TESTING BREAKPOINT LOCATION #1 */

            /* At this point, received frame can be examined in global "rx_buffer". An actual application would, for example, start by checking that
             * the format and/or data of the response are the expected ones. A developer might put a breakpoint here to examine this frame. */

            /* 清除接收事件. Clear good RX frame event in the DW IC status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG_BIT_MASK);
        }
        else
        {
            /* Clear RX error/timeout events in the DW3000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
        }

        /* 休眠TX_DELAY_MS. Execute a delay between transmissions. */
        Sleep(TX_DELAY_MS);

        /* Increment the blink frame sequence number (modulo 256). */
        tx_msg[BLINK_FRAME_SN_IDX]++;
    }
}
#endif
/*****************************************************************************************************************************************************
 * NOTES:
 *
 * 1. The device ID is a hard coded constant in the blink to keep the example simple but for a real product every device should have a unique ID.
 *    For development purposes it is possible to generate a DW IC unique ID by combining the Lot ID & Part Number values programmed into the
 *    DW IC during its manufacture. However there is no guarantee this will not conflict with someone else抯 implementation. We recommended that
 *    customers buy a block of addresses from the IEEE Registration Authority for their production items. See "EUI" in the DW IC User Manual.
 * 2. In this example, the DW IC is put into IDLE state after calling dwt_initialise(). This means that a fast SPI rate of up to 20 MHz can be used
 *    thereafter.
 * 3. TX to RX delay can be set to 0 to activate reception immediately after transmission. But, on the responder side, it takes time to process the
 *    received frame and generate the response (this has been measured experimentally to be around 70 祍). Using an RX to TX delay slightly less than
 *    this minimum turn-around time allows the application to make the communication efficient while reducing power consumption by adjusting the time
 *    spent with the receiver activated.
 * 4. This timeout is for complete reception of a frame, i.e. timeout duration must take into account the length of the expected frame. Here the value
 *    is arbitrary but chosen large enough to make sure that there is enough time to receive a complete frame sent by the "RX then send a response"
 *    example at the 110k data rate used (around 3 ms).
 * 5. In this example, maximum frame length is set to 127 bytes which is 802.15.4 UWB standard maximum frame length. DW IC supports an extended frame
 *    length (up to 1023 bytes long) mode which is not used in this example.
 * 6. In a real application, for optimum performance within regulatory limits, it may be necessary to set TX pulse bandwidth and TX power, (using
 *    the dwt_configuretxrf API call) to per device calibrated values saved in the target system or the DW IC OTP memory.
 * 7. dwt_writetxdata() takes the full size of tx_msg as a parameter but only copies (size - 2) bytes as the check-sum at the end of the frame is
 *    automatically appended by the DW IC. This means that our tx_msg could be two bytes shorter without losing any data (but the sizeof would not
 *    work anymore then as we would still have to indicate the full length of the frame to dwt_writetxdata()).
 * 8. We use polled mode of operation here to keep the example as simple as possible but all status events can be used to generate interrupts. Please
 *    refer to DW IC User Manual for more details on "interrupts".
 * 9. The user is referred to DecaRanging ARM application (distributed with EVK1000 product) for additional practical example of usage, and to the
 *    DW IC API Guide for more details on the DW IC driver functions.
 *10. TX and RX activity can be monitored using the following GPIOs (e.g. connected to an oscilloscope):
 *        - GPIO 2: RXLED signal. Driven high from the activation of reception for the LED blink time (~ 225 ms). If the reception remains activated
 *          for longer than that, this signal turns into a periodic signal where both ON and OFF phases last for LED blink time.
 *        - GPIO 3: TXLED signal. Driven high from the start of a transmission for the LED blink time.
 *        - GPIO 5: EXTTXE signal. Follows the transmitter enabled state.
 *        - GPIO 6: EXTRXE signal. Follows the receiver enabled state.
 *11. Desired configuration by user may be different to the current programmed configuration. dwt_configure is called to set desired
 *    configuration.
 ****************************************************************************************************************************************************/
