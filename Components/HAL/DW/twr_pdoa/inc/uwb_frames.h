/* @file    uwb_frames.h
 * @brief
 *          UWB message frames definitions and typedefs
 *
 * @author Decawave Software
 *
 * @attention Copyright 2018 (c) DecaWave Ltd, Dublin, Ireland.
 *            All rights reserved.
 *
 */

#ifndef __UWB_FRAMES__H__
#define __UWB_FRAMES__H__

#ifdef __cplusplus
extern "C" {
#endif


#pragma anon_unions
		
	
#include <stdint.h>
#include <stdbool.h>
#include "OSAL_Comdef.h"


#define MAX_ANCHOR_LIST_SIZE        (8)		//NODE MAX
#define MAX_TAG_LIST_SIZE           (32)	//TAG MAX



#define TS_40B_SIZE                 (5)
#define TS_UWB_SIZE                 (5)


//UWB空中数据包-帧控制&MAC地址长度
#define FRAME_CTRL                  (0x4188)
#define EUI64_ADDR_SIZE             (8)
#define STANDARD_FRAME_SIZE         (127)


//UWB空中数据包-功能码下标&功能码
#define MSG_FN_IDX                  (2+1+2+2+2)
#define TWR_POLL_MSG_FN             (0x1A)
#define TWR_RESP_MSG_FN             (0x1B)
#define TWR_FINAL_MSG_FN            (0x1C)


#pragma pack(push,1)
typedef struct
{
	uint16_t frameCtrl;					//控制帧(2)
	uint8_t  seqNum;					//通信序列号(1)
	uint16_t panID;						//个人网络(2)
	uint16_t destAddr;					//目标地址(2)
	uint16_t sourceAddr;				//源地址(2)
}mac_header_ss_t;

typedef struct
{
	uint8_t  fCode;						//功能码
	uint8_t  rNum;						//序列号
}poll_t;

typedef struct
{
	uint8_t  fCode;						//功能码
	int32_t  slotCorr_ms;				//标签校正参数
	uint8_t  rNum;						//序列号
	uint16_t a2t_usercmd;				//基站自定义命令
	uint16_t pre_dist_cm;				//基站到该标签距离值(上一回合)
}resp_t;


typedef struct
{
	uint8_t  fCode;						//功能码
	uint8_t  rNum;						//序列号
	uint64_t pollTx_ts;					//标签Poll发送时间戳
	uint64_t responseRx_ts[MAX_ANCHOR_LIST_SIZE];//标签Resp接受时间戳
	uint64_t finalTx_ts;				//标签Final发送时间戳
	uint8_t  rxResponseMask;				//标签接受Resp有效位

	uint16_t dist_cm[MAX_ANCHOR_LIST_SIZE];//标签到基站的距离值
}final_t;

typedef struct
{
	mac_header_ss_t mac;
	poll_t          poll;
	uint8_t         fcs[2];
}poll_msg_t;							//poll数据包

typedef struct
{
	mac_header_ss_t mac;
	resp_t          resp;
	uint8_t         fcs[2];
}resp_msg_t;							//resp数据包

typedef struct
{
	mac_header_ss_t mac;
	final_t         final;
	uint8_t         fcs[2];
}final_msg_t;							//final数据包
#pragma pack(pop)


#ifdef __cplusplus
}
#endif

#endif /* __UWB_FRAMES__H__ */
