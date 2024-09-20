/*
 * @file error.h
 *
 * @brief
 *
 * @author Decawave Software
 *
 * @attention Copyright 2018 (c) DecaWave Ltd, Dublin, Ireland.
 *            All rights reserved.
 *
 */

#ifndef INC_ERROR_H_
#define INC_ERROR_H_


typedef enum{
    _NO_ERR       = 0,
    _Err          = 1,
    _Err_Busy     = 2,
    _Err_Timeout  = 3,
	/*TWR*/
    _Err_Twr_Bad_State,
    _Err_Not_Twr_Frame,
    _Err_Unknown_Tag,
    _Err_DelayedTX_Late,
    _No_Err_New_Tag,
    _No_Err_Tx_Sent,
    _No_Err_Start_Rx,
    _No_Err_Final,
    _Err_Range_Calculation,
}error_e;


#endif /* INC_ERROR_H_ */
