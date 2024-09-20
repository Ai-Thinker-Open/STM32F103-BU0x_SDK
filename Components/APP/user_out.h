/**********************************************************************************
 * @file    user_out.h
 * @author Ai-Thinker
 * @brief  header file for user_out.c
 * @date    2024.9.18
**********************************************************************************/
#ifndef __USER_OUT_H_
#define __USER_OUT_H_

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <node.h>
#include "shared_functions.h"

extern uint16_t  user_tag_dist_cm[MAX_ANCHOR_LIST_SIZE];


/**
 * 设置安信可默认输出的开关状态。
 *
 * @param output 如果为true，则开启输出；如果为false，则关闭输出。
 */
void set_aiio_output_state (bool output);

/**
 * 获取安信可默认输出的开关状态。
 *
 * @return 如果输出已开启，则返回true；否则返回false
 */
bool get_aiio_output_state (void);

/**
 * 输出 twr算法测量结果，用户可自主处理
 *
 */
void node_twr_output_user (result_t* pRes);

/**
 * 输出 twr算法(标签)测量结果，用户可自主处理
 *
 */
void tag_twr_output_user (uint16_t* dis);

/**
 *输出 pdoa算法（基站）测量结果，用户可自主处理
 *
 */
void node_pdoa_output_user (result_pdoa_t* Res);

/**
 *输出 pdoa算法（标签）测量结果，用户可自主处理
 *
 */
void tag_pdoa_output_user (result_pdoa_t* Res);

#endif
