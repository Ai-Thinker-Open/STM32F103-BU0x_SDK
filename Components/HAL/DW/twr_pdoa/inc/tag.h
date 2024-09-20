/*! ---------------------------------------------------------------------------
 * @file    node.h
 * @brief   DecaWave
 *             bare implementation layer
 *
 * @author Decawave Software
 *
 * @attention Copyright 2018 (c) DecaWave Ltd, Dublin, Ireland.
 *            All rights reserved.
 *
 */

#ifndef __NODE__H__
#define __NODE__H__ 

#ifdef __cplusplus
 extern "C" {
#endif

#include "uwb_frames.h"
#include "msg_time.h"

#include "default_config.h"	 
#include "error.h"

#define DEEP_SLEEP                      (0)				//休眠标志位




#define EVENT_BUF_SIZE                  (0x02)			//事件buf大小
#define FILTER_SIZE                     (10)			//运动滤波值


#define MASK_40BIT                      (0x00FFFFFFFFFFULL)  // DW1000 counter is 40 bits
#define MASK_TXDTS                      (0x00FFFFFFFE00ULL)  //The TX timestamp will snap to 8 ns resolution - mask lower 9 bits.

#define GATEWAY_ADDR                    (0x0000)		//网关地址(时分多址主机)

#define TAG_BLINK_TX_RC_RX_US           (500)			//标签发送Blink->接受RngCfg延迟时间
#define RC_RX_TIMEOUT_US                (300)			//标签接受rngcfg接受超时
#define RX_RELAX_TIMEOUT_SY             (20)			//接受超时时间设置
#define RX_FIXED_TIMEOUT_US             (300)			//固定接受超时时间


#define SPEED_OF_LIGHT                  (299702547.0)     // in m/s in the air

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------


/* 数据上报结构体 */
typedef struct
{
	uint16_t    addr16;         //标签地址
	uint8_t     rangeNum;       //标签测距序列号 number from Tag Poll and Final messages, which indicates the current range number
	uint32_t    resTime_us;     //标签测距时间 reception time of the end of the Final from the Tag wrt node's SuperFrame start, microseconds
	uint16_t    mask;
	uint16_t    dist_cm[MAX_ANCHOR_LIST_SIZE];	//标签上报结果

	float       pre_dist_m;		//测距结果
	float       pre_fdist_m;	//测距结果(滤波)
}result_t;



/* 发送结构体 */
typedef struct
{
	struct{
		int16_t     txDataLen;						//发送长度
		uint8_t     arr[STANDARD_FRAME_SIZE];		//发送数据包
	};

	enum {
		Twr_Tx_Poll_Sent,							//标签发送Poll状态
		Twr_Tx_Resp_Sent,							//节点发送Resp状态
		Twr_Tx_Final_Sent,							//标签发送Final状态
	}txState;
	uint8_t         is_send;						//发送成功判断位
	uint8_t         seqNum;							//发送数据seq
	uint8_t         txFlag;							//发送标志位 DWT_START_TX_IMMEDIATE DWT_START_TX_DELAYED & DWT_RESPONSE_EXPECTED

	uint8_t         t_timeStamp[TS_40B_SIZE];		//发送时间戳

	uint32_t        delayedTxTimeH_sy;  			//发送延迟(高32位) Delayed transmit time (high32)
	uint32_t        delayedRxTime_sy;				//接受延迟
	uint16_t        delayedRxTimeout_sy;			//接受超时
}tx_pckt_t;


/* 接受结构体 */
typedef struct 
{
	struct{
		int16_t     rxDataLen;						//接受长度
		uint8_t     arr[STANDARD_FRAME_SIZE];		//接收数据包
	};
	
	uint8_t         r_timeStamp[TS_40B_SIZE];		//接受时间戳
	
	uint32_t        TickTimeStamp;					//滴答时钟
}rx_pckt_t;


/* TWR结构体 */
typedef struct
{
	int      testAppState ;							//状态
	int      done ;									//事件

	// 接受UWB队列
	struct{
		rx_pckt_t   buf[EVENT_BUF_SIZE];
		int         head;
		int         tail;
	}rxPcktBuf;

	// 发送UWB队列
	struct{
		tx_pckt_t  buf;
	}txPcktBuf;

	// twr标签参数
	struct{
		struct{
			uint16_t     tagAddr;					//标签短地址
			uint16_t     panID;						//标签panID
			sfConfig_t   sfConfig;					//超级帧
		}general;

		struct{
			uint8_t  twr_sn;						//测距序列号
			int8_t   remainingRespToRx;
			int8_t   rxResponseMask;
			int      slotCorr_ms;					//时间矫正
			uint16_t a2t_usercmd;					//基站发送的自定义指令

			struct{
				uint64 tagPollTxTime;   					//标签Poll数据包发送时间
				uint64 tagRespRxTime[MAX_ANCHOR_LIST_SIZE];	//标签Resp数据包接受时间
				uint64 tagFinalTxTime;  					//标签Final数据包发送时间
			}twr_timestamp;
		}measure;
		
		struct{
			bool DwCanSleep;						//设备休眠状态
			uint8_t event;							//休眠超时事件
			
			bool timeron;							//开启定时器标志位
			uint32_t interval_in_ms;				//设备休眠时长,根据blinkfast blinkslow决定
			uint32_t nextWakeUpTime_ms;				//开启定时器器唤醒时间分析
			uint32_t preWakeUpTime_ms;				//上一次唤醒发送时间
		}machine_period;

		struct{
			msg_time_t    poll;
			msg_time_t    response;
			msg_time_t    final;
		}msg_time;	
	}twr_tag;
}twr_tag_info_t;

enum inst_done
{
	INST_NOT_DONE_YET = 0,				//当前事件仍然需要处理，正在处理中
	INST_DONE_WAIT_FOR_NEXT_EVENT,		//当前事件已经被处理，处理完成
	
	INST_DONE_WAIT_FOR_NEXT_EVENT_TO	//当前时间已经被处理，等待事件超时(标签唤醒使用)
};



enum inst_states
{
	TA_INIT,

	/*TOF*/
	TA_TXPOLL_WAIT_SEND,
	TA_TXRESP_WAIT_SEND,
	TA_TXFINAL_WAIT_SEND,
	TA_TXSYSY_WAIT_SEND,
	TA_TXE_WAIT,

	TA_TX_WAIT_CONF,		// 等待数据发送完成确认
	TA_RXE_WAIT,			// 立即开启接受
	TA_RX_WAIT_DATA,		// 接受数据阶段
	TA_SLEEP_DONE,			// 设备睡眠
	
	DWT_SIG_SLEEP_TIMEOUT	// 休眠超时类型
};






//-----------------------------------------------------------------------------
// exported functions prototypes
//
extern twr_tag_info_t * getTwrInfoPtr_tag(void);
static void tag_received_timeout_set(twr_tag_info_t *pTwrInfo, bool error);


//接受任务
static void prepare_twr_poll_msg(tx_pckt_t *pTxPckt, twr_tag_info_t *pTwrInfo);				//节点接受poll
static void prepare_twr_resp_msg(rx_pckt_t *pRxPckt, twr_tag_info_t *pTwrInfo);				//节点发送resp
static void prepare_twr_final_msg(tx_pckt_t *pTxPckt, twr_tag_info_t *pTwrInfo);			//节点接受final
static void twr_uwb_process(rx_pckt_t *pRxPckt, twr_tag_info_t *pTwrInfo);


int tagtestapprun(twr_tag_info_t *pTwrInfo);
void tag_instance_run(void);
void tag_pdoa_task(void);


extern void tag_start(void);
//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif /* __NODE__H__ */
