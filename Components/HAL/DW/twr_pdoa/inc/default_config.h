/*
 * @file default_config.h
 *
 * @brief  default config fie for NVM initialization
 *
 * @author Decawave Software
 *
 * @attention Copyright 2018 (c) DecaWave Ltd, Dublin, Ireland.
 *            All rights reserved.
 *
 */

#ifndef __DEFAULT_CONFIG_H__H__
#define __DEFAULT_CONFIG_H__H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <uwb_frames.h>
#include <deca_device_api.h>
#include "OSAL_Comdef.h"
#include "tag_list.h"

/**************TWR******************/

/* 信道配置 */
#define DEFAULT_CHANNEL                     (5)										//  9 | 5
#define DEFAULT_TXPREAMBLENGTH              (DWT_PLEN_64)							//
#define DEFAULT_RXPAC                       (DWT_PAC8)								//
#define DEFAULT_PCODE                       (9)										//
#define DEFAULT_NSSFD                       (1)										//
#define DEFAULT_DATARATE                    (DWT_BR_6M8)							//速率  DWT_BR_850K  |  DWT_BR_6M8
#define DEFAULT_PHRMODE                     (DWT_PHRMODE_STD)						//
#define DEFAULT_PHRRATE                     (DWT_PHRRATE_STD)						//
#define DEFAULT_SFDTO                       (65 + 8 - 8)							//
#define DEFAULT_STSMODE                     (DWT_STS_MODE_OFF)		//  (DWT_STS_MODE_1 | DWT_STS_MODE_SDC)	
#define DEFAULT_STSLENGTH                   (DWT_STS_LEN_64)						//
#define DEFAULT_PDOAMODE                    (DWT_PDOA_M0)							//


/* 基础配置*/
#define DEFAULT_ANTD                        (16336)		//天线延迟

/* sf配置*/
#define DEFAULT_SLOT_PERIOD_MS              (10)									//单个槽周期(10ms)
#define DEFAULT_NUM_SLOTS                   (MAX_TAG_LIST_SIZE)						//槽数量(DEFAULT_NODE_MAXTAGLIST+1)
#define DEFAULT_SF_PERIOD_MS                (DEFAULT_NUM_SLOTS*DEFAULT_SLOT_PERIOD_MS)//超级帧周期 = 单个槽周期 * 槽数量
#define DEFAULT_TAG_POLL_TX_RESPONSE_RX_US  (400)									//标签发送Poll->接受Resp延迟时间
#define DEFAULT_TAG_POLL_TX_FINAL_TX_US     (4800)									//标签发送Poll->发送Final延迟时间


/* 用户配置 */
#define DEFAULT_NODE_ADDR                   (0xFFFF)							//节点地址
#define DEFAULT_NODE_PANID                  (0x1111)								//节点网络
#define DEFUALT_NODE_MOTIONFILTER           (1)										//节点滤波
#define DEFAULT_NODE_USERCMD                (0)										//节点命令
#define DEFAULT_TAG_NUMSLOTS                (10)									//默认标签槽数量10个,最大32个
#define KALMAN_Q                            (0.018)									//卡尔曼滤波参数
#define KALMAN_R                            (0.642)									//卡尔曼滤波参数
#define PARA_A                              (1)										//校准参数
#define PARA_B                              (0)										//校准参数
#define PA_SET_MOD                          (0)										//PA功放

/*pdoa*/
/* holds a run-time parameters */
/****************PDOA*****************/

#define DEFAULT_GATEWAY_ADDR        0x0001

/* run-time config */
#define PDOA_DEFAULT_NODE_ADDR           0xFFFF  /**< Addr16	*/
#define DEFAULT_PANID               0x1111  /**< PanID */

#define DEFUALT_MOTIONFILTER        1
#define DEFAULT_USER_CMD            0

#define PDOA_DEFAULT_ANTD                (515.736f * 1e-9 / DWT_TIME_UNITS)
#define DEFAULT_PDOAOFF             0       /**< Phase Differences offset */
#define DEFAULT_RNGOFF              0       /**< Ranging offset */
#define DEFAULT_PDOA_TEMP           0       /**< Linear temperature coefficient for PDOA, mrad. Diff of PDOA when temperature changed +20C deg centigrade */
#define DEFAULT_ACCUM_READING       0       /**< */
#define DEFAULT_DIAG_READING        0       /**< */
#define DEFAULT_RBC_EN              1       /**< enable range bias correction */
#define DEFAULT_PHASECORR_EN        0       /**< enable antenna related phase correction polynomial */
#define DEFAULT_REPORT_LEVEL        1       /**< what to use as output of TWR: 1:JSON, 2:Reduced, 3:Minimal */
#define DEFAULT_SMART_TX            0       /**< do not use smart tx power */
#define DEFAULT_ACCEL_STATIONARY_MS 5000    /**< Time of consecutive readings under the threshold needed to consider that the device is stationary */
#define DEFAULT_ACCEL_MOVING_MS     1000    /**< Time of consecutive readings above the threshold needed to consider that the device is moving */
#define ACCEL_NORMALIZED_THRESHOLD  50      /**< Acceleration values difference threshold to consider stationary or moving state.
                                                 This normalized to a resolution in "g" values, we will use a 5% threshold as 1g == 1000 */
/* Slot configuration */
//100Hz
#define PDOA_DEFAULT_SLOT_PERIOD_MS  10      /**< Slot period (ms), the best of what the implementation can achieve */
#define PDOA_DEFAULT_NUM_SLOTS       2       /**< Number of slots: cannot be shorter than (MAX_KNOWN_TAG_LIST_SIZE + 1) */
#define PDOA_DEFAULT_SF_PERIOD_MS    20      /**< SuperFrame period: cannot be shorter than NUM_SLOTS*SLOT_PERIOD */

#define DEFAULT_TAG_MULT_FAST       1
#define DEFAULT_TAG_MULT_SLOW       100
#define DEFAULT_TAG_MODE            0

#if(PDOA_DEFAULT_NUM_SLOTS < (MAX_KNOWN_TAG_LIST_SIZE + 1) )
    #error "error config: PDOA_DEFAULT_NUM_SLOTS < (MAX_KNOWN_TAG_LIST_SIZE + 1)"
#endif

#if(DEFAULT_SF_PERIOD_MS < (PDOA_DEFAULT_NUM_SLOTS * PDOA_DEFAULT_SLOT_PERIOD_MS))
    #error "error config: PDOA_DEFAULT_SF_PERIOD_MS < (PDOA_DEFAULT_NUM_SLOTS * PDOA_DEFAULT_SLOT_PERIOD_MS)"
#endif


/* Below are 2 parameters to be sent to the Tag's in the Ranging Config message.
 * Node uses these parameters to setup its receivers during TWR exchange to a Tag.
 * Tag hardware shall be capable to accept the timings below:
 * */

#define PDOA_DEFAULT_TAG_POLL_TX_RESPONSE_RX_US  (450)  /**< rough time when Tag shall be ready to receive the node's Response after end of transmission of the Poll.
                                                     *    This parameter is Node's HW/SW dependent, as the Node's application is not designed to meet shorter timings.
                                                     *    this works in optimizations -O2 and -O3 */

#define PDOA_DEFAULT_TAG_POLL_TX_FINAL_TX_US     (2500) /**< rough time when Tag shall transmit Final's RMARKER, calculated from Tag's Poll's RMARKER. */

/* This configures the delay between end of Tag's Blink_TX and Tag start Rx of Ranging Config message.
 * From Node's view this is a delay between end of reception of Blink's data and start of transmission of preamble of Ranging Config.
 * Should be the same for Node and Tag.
 * */
#define DEFAULT_TAG_BLINK_TX_RC_RX_US       (1000)

/* Default configuration initialization */
#define DEFAULT_CONFIG \
{\
	.s.baseConfig.antRx             = (uint16_t)(DEFAULT_ANTD),\
	.s.baseConfig.antTx             = (uint16_t)(DEFAULT_ANTD),\
	\
	.s.sfConfig.slotPeriod          = DEFAULT_SLOT_PERIOD_MS, \
	.s.sfConfig.numSlots            = DEFAULT_NUM_SLOTS, \
	.s.sfConfig.sfPeriod_ms         = DEFAULT_SF_PERIOD_MS, \
	.s.sfConfig.tag_replyDly_us     = DEFAULT_TAG_POLL_TX_RESPONSE_RX_US, \
	.s.sfConfig.tag_pollTxFinalTx_us= DEFAULT_TAG_POLL_TX_FINAL_TX_US, \
	\
	.s.userConfig.nodeAddr          = DEFAULT_NODE_ADDR, \
	.s.userConfig.nodePanid         = DEFAULT_NODE_PANID, \
	.s.userConfig.nodeFilter        = DEFUALT_NODE_MOTIONFILTER, \
	.s.userConfig.tagnumSlots       = DEFAULT_TAG_NUMSLOTS, \
	.s.userConfig.kalman_Q          = KALMAN_Q,\
	.s.userConfig.kalman_R          = KALMAN_R,\
	.s.userConfig.para_a            = PARA_A,\
	.s.userConfig.para_b            = PARA_B,\
	.s.userConfig.Pa_mod            = PA_SET_MOD,\
	.s.userConfig.workmode           = 0,\
	\
	.s.s_pdoa.addr                 = PDOA_DEFAULT_NODE_ADDR, \
	.s.s_pdoa.panID                = DEFAULT_PANID, \
	.s.s_pdoa.pdoaOffset_deg       = DEFAULT_PDOAOFF, \
	.s.s_pdoa.rngOffset_mm         = DEFAULT_RNGOFF, \
	.s.s_pdoa.pdoa_temp_coeff_mrad = DEFAULT_PDOA_TEMP, \
	.s.s_pdoa.rbcEn                = DEFAULT_RBC_EN, \
	.s.s_pdoa.phaseCorrEn          = DEFAULT_PHASECORR_EN, \
	.s.s_pdoa.smartTxEn            = DEFAULT_SMART_TX, \
	.s.s_pdoa.reportLevel          = DEFAULT_REPORT_LEVEL, \
	.s.s_pdoa.acc_stationary_ms    = DEFAULT_ACCEL_STATIONARY_MS, \
	.s.s_pdoa.acc_moving_ms        = DEFAULT_ACCEL_MOVING_MS, \
	.s.s_pdoa.acc_threshold        = ACCEL_NORMALIZED_THRESHOLD, \
	.s.s_pdoa.rcDelay_us           = DEFAULT_TAG_BLINK_TX_RC_RX_US, \
	.s.s_pdoa.motionfilter         = DEFUALT_MOTIONFILTER,	\
	.s.s_pdoa.user_cmd             = DEFAULT_USER_CMD,\
	.s.s_pdoa.Dlist                = MAX_DISCOVERED_TAG_LIST_SIZE,\
	.s.s_pdoa.Klist                = MAX_KNOWN_TAG_LIST_SIZE,\
	.s.s_pdoa.UartReFreshRate      = 100,\
	.s.s_pdoa.reserver             = 0,\
	\
	.s.s_pdoa.antRx_a              = (uint16_t)(0.45* PDOA_DEFAULT_ANTD), \
	.s.s_pdoa.antTx_a              = (uint16_t)(0.55* PDOA_DEFAULT_ANTD), \
	.s.s_pdoa.antRx_b              = (uint16_t)(0.45* PDOA_DEFAULT_ANTD), \
	.s.s_pdoa.antTx_b              = (uint16_t)(0.55* PDOA_DEFAULT_ANTD), \
	\
	.s.s_pdoa.sfConfig_pdoa.slotPeriod             = PDOA_DEFAULT_SLOT_PERIOD_MS, \
	.s.s_pdoa.sfConfig_pdoa.numSlots               = DEFAULT_NUM_SLOTS, \
	.s.s_pdoa.sfConfig_pdoa.sfPeriod_ms            = PDOA_DEFAULT_SF_PERIOD_MS, \
	.s.s_pdoa.sfConfig_pdoa.tag_replyDly_us        = PDOA_DEFAULT_TAG_POLL_TX_RESPONSE_RX_US, \
	.s.s_pdoa.sfConfig_pdoa.tag_pollTxFinalTx_us   = PDOA_DEFAULT_TAG_POLL_TX_FINAL_TX_US, \
	\
	.s.s_pdoa.sfConfig_pdoa.tag_mFast              = DEFAULT_TAG_MULT_FAST, \
	.s.s_pdoa.sfConfig_pdoa.tag_mSlow              = DEFAULT_TAG_MULT_SLOW, \
	.s.s_pdoa.sfConfig_pdoa.tag_Mode               = DEFAULT_TAG_MODE, \
	\
	.dwt_config.chan                = DEFAULT_CHANNEL, \
	.dwt_config.txPreambLength      = DEFAULT_TXPREAMBLENGTH, \
	.dwt_config.rxPAC               = DEFAULT_RXPAC, \
	.dwt_config.txCode              = DEFAULT_PCODE, \
	.dwt_config.rxCode              = DEFAULT_PCODE, \
	.dwt_config.sfdType             = DEFAULT_NSSFD, \
	.dwt_config.dataRate            = DEFAULT_DATARATE, \
	.dwt_config.phrMode             = DEFAULT_PHRMODE, \
	.dwt_config.phrRate             = DEFAULT_PHRRATE, \
	.dwt_config.sfdTO               = DEFAULT_SFDTO, \
	.dwt_config.stsMode             = DEFAULT_STSMODE, \
	.dwt_config.stsLength           = DEFAULT_STSLENGTH, \
	.dwt_config.pdoaMode            = DEFAULT_PDOAMODE \
	\
}

__packed typedef struct
{
	uint16_t    antRx;						//天线延迟(接受)
	uint16_t    antTx;						//天线延迟(发送)
}baseConfig_t;


__packed typedef struct
{
	uint16_t    slotPeriod;					//单个槽周期
	uint16_t    numSlots;					//槽数量
	uint16_t    sfPeriod_ms;				//超级帧周期 = 单个槽周期 * 槽数量
	uint16_t    tag_replyDly_us;			//标签发送Poll->接受Resp延迟时间
	uint16_t    tag_pollTxFinalTx_us;		//标签发送Poll->发送Final延迟时间
}sfConfig_t;


__packed typedef struct
{
	uint16_t    nodeAddr;					//节点地址
	uint8_t     role;             //设备角色
	uint8_t     workmode;             //工作模式
	uint8_t     twr_pdoa_mode;      //算法切换
	uint16_t    nodePanid;					//节点网络
	uint16_t    nodeFilter;					//节点滤波
	uint16_t    tagnumSlots;				//默认标签槽数量10个,最大32个
	float       kalman_Q;					//滤波参数Q
	float       kalman_R;					//滤波参数R
	float       para_a;						//标定参数a
	float       para_b;						//标定参数b
	uint16_t    Pa_mod;						//功率设置
}userConfig_t;

__packed typedef struct
{
    uint16_t    slotPeriod;              /**< Slot period (time for one tag to range) */
    uint16_t    numSlots;                /**< Number of slots used in the SuperFrame */
    uint16_t    sfPeriod_ms;             /**< SuperFrame period in ms */
    uint16_t    tag_replyDly_us;         /**< wait4response delay after end of Poll for the tag, us */
    uint16_t    tag_pollTxFinalTx_us;    /**< PollTxToFinalTx delay, us */

    /* below can be removed if the d2k command, i.e. "add all discovered tags to known list automatically", is not used */
    uint16_t    tag_mFast;              /**< Used to pass a "moving" refresh rate to all tags in the "d2k" command */
    uint16_t    tag_mSlow;              /**< Used to pass a "stationary" refresh rate to all tags in the "d2k" command */
    uint16_t    tag_Mode;               /**< Used to pass a common "mode" parameter to all tags in the "d2k" command */
}sfConfig_t_pdoa;

typedef struct {
    uint16_t    addr;           /**< 基站地址 Node's address */
    uint16_t    panID;          /**< 系统网络 System PanID */
    int16_t     pdoaOffset_deg;/**< Calibration: the Phase Differences offset */
    int16_t     rngOffset_mm;   /**< Calibration: the Ranging offset */
    int16_t     pdoa_temp_coeff_mrad; /**< reserved for future */
    uint8_t     rbcEn;          /**< Enable Range bias correction */
    uint8_t     phaseCorrEn;    /**< Use correction curve to use in calculation PDOA->X-Y */
    uint8_t     smartTxEn;      /**< Enable SmartTx power */
    uint8_t     reportLevel;    /**< 0 - not output, 1-JSON, 2-limited */
    uint16_t    acc_stationary_ms;  /**< Number of milliseconds to decide the device is not moving */
    uint16_t    acc_moving_ms;      /**< Number of milliseconds to decide the device is moving */
    uint16_t    acc_threshold;      /**< The threshold for X+Y+Z axis', in milli-G, to separate instantaneous movement to stationary */
    uint16_t    rcDelay_us;     /**< Node's delay between end reception of UWB blink and start transmission of UWB Ranging Config message */
    uint16_t    motionfilter;	//是否开启滤波
    uint16_t    user_cmd;		//格式选择

    uint8_t     Dlist;          //发现列表数量
    uint8_t     Klist;          //绑定列表数量
    uint8_t     UartReFreshRate;//串口刷新速率
    uint8_t     reserver;       //保留
    
    uint16_t    antRx_a;        /**< antenna delay values for current MASTER */
    uint16_t    antTx_a;        /**< antenna delay values for current MASTER */
    uint16_t    antRx_b;        /**< antenna delay values for current SLAVE */
    uint16_t    antTx_b;        /**< antenna delay values for current SLAVE */
    sfConfig_t_pdoa  sfConfig_pdoa;       /**< System configuration: SuperFrame description */
}__attribute__((packed)) run_pdoa_t;

typedef struct 
{       
	baseConfig_t baseConfig;				//twr 基础配置
	sfConfig_t   sfConfig;					//twr sf配置
	userConfig_t userConfig;				//用户配置
	run_pdoa_t    s_pdoa;						//pdoa基本配置
}__attribute__((packed)) run_t;



typedef struct 
{
	run_t           s;						//基本配置
	dwt_config_t    dwt_config;				//信道配置
	tag_addr_slot_t knownTagList[MAX_KNOWN_TAG_LIST_SIZE];	/*171字节  19 = 171 / 9*/
}__attribute__((packed)) param_block_t;


#ifdef __cplusplus
}
#endif

#endif /* __DEFAULT_CONFIG__H__ */

