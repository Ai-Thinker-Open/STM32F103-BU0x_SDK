/*! ----------------------------------------------------------------------------
 * @file    example_selection.h
 * @brief   Example selection is configured here
 *
 * @attention
 *
 * Copyright 2013 - 2020(c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 */

#ifndef TEST_SELECTION_
#define TEST_SELECTION_

#ifdef __cplusplus
extern "C" {
#endif

#include "examples_defines.h"

#define TEST_DS_TWR_STS_SDC_INITIATOR		   //跑安信可默认demo勿关
#define TEST_DS_TWR_STS_SDC_RESPONDER		   //跑安信可默认demo勿关

#if(EXAMPLE_DEMO)
//Enable the needed example/test. Please enable only one example/test!

//#define TEST_READING_DEV_ID							 //(读取DevID例程)

//#define TEST_SIMPLE_TX								   //(简单Tx发送例程)
//#define TEST_SIMPLE_RX								   //(简单Rx接受例程)

//#define TEST_SIMPLE_RX_PDOA							//(简单PDoA Rx接受例程)
//#define TEST_SIMPLE_TX_PDOA							//(简单PDoA Tx发送例程)

//#define TEST_TX_WAIT_RESP								//(Tx发送等待Response例程)
//#define TEST_TX_WAIT_RESP_INT							//(Tx发送等待Response例程)
//#define TEST_RX_SEND_RESP								//(Rx接受发送Response例程)

//#define TEST_DS_TWR_INI_STS						//(双边测距(DS TWR STS)发起者例程)
//#define TEST_DS_TWR_RESP_STS						//(双边测距(DS TWR STS)回复者例程)

//#define TEST_TX_SLEEP_TIMED							 //(发送进入休眠内部唤醒例程)

#endif
#ifdef __cplusplus
}
#endif


#endif
