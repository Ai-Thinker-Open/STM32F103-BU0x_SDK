/*! ----------------------------------------------------------------------------
 * @file    example_info.h
 * @brief
 *
 * @attention
 *
 * Copyright 2013-2018(c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 */

#include <example_selection.h>
#include "examples_defines.h"

#if(EXAMPLE_DEMO)

void build_examples(void)
{
#ifdef TEST_READING_DEV_ID
    read_dev_id();

#elif defined(TEST_SIMPLE_TX)
    simple_tx();

#elif defined(TEST_SIMPLE_RX)
    simple_rx();
	
#elif defined(TEST_SIMPLE_RX_PDOA)
    simple_rx_pdoa();
	
#elif defined(TEST_SIMPLE_TX_PDOA)
    simple_tx_pdoa();
	
#elif defined(TEST_TX_WAIT_RESP)
    tx_wait_resp();
	
#elif defined(TEST_TX_WAIT_RESP_INT)
    tx_wait_resp_int();
	
#elif defined(TEST_RX_SEND_RESP)
    rx_send_resp();
		
#elif defined(TEST_DS_TWR_INI_STS)
    ds_twr_sts_sdc_init();
		
#elif defined(TEST_DS_TWR_RESP_STS)
    ds_twr_sts_sdc_resp();
		
#elif defined(TEST_TX_SLEEP_TIMED)
    tx_timed_sleep();
#endif

}

#endif