/*
 * @file cmd_fn.h
 *
 * @brief  header file for cmd_fn.c
 *
 * @author Decawave Software
 *
 * @attention Copyright 2018 (c) DecaWave Ltd, Dublin, Ireland.
 *            All rights reserved.
 *
 */
#ifndef INC_CMD_FN_H_
#define INC_CMD_FN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"



//-----------------------------------------------------------------------------
/* module DEFINITIONS */
#define QUERY_CMD       0x01    /* 查询命令 */
#define EXECUTE_CMD     0x02    /* 执行命令 */
#define SET_CMD         0x03    /* 设置命令*/
#define MAX_STR_SIZE       (160)
#define ARGC_LIMIT         (32)/*最大参数数量*/

/* Prefer to use dynamic strings allocation.
 * however this leads every task, which uses string output,
 * to allocate MAX_STR_SIZE from its own stack.
 * FreeRTOS does not look by default to overflow of the stack.
 * Set the configCHECK_FOR_STACK_OVERFLOW to 2 if in doubt.
 * */
#define DYNAMIC_MSG_STR_ALLOW

#if defined(DYNAMIC_MSG_STR_ALLOW)
#define CMD_MALLOC              malloc
#define CMD_FREE                free
#else
extern char staticstr[];

#define CMD_MALLOC(MAX_STR_SIZE)   staticstr
#define CMD_FREE(x)    UNUSED(x)
#endif


//-----------------------------------------------------------------------------
/* All cmd_fn functions have unified input: (char *text, param_block_t *pbss, int val)
 * Will use REG_FN macro to declare unified functions.
 * */

typedef struct
{
    char* name;
    int (*deal_func) (int opt, int argc, char* argv[]);
} command_t;

extern const command_t known_commands[];

void command_stop_received (void);



#ifdef __cplusplus
}

#endif


#endif /* INC_CMD_FN_H_ */
