/*
 * @file cmd.h
 *
 * @brief  header file for command.c
 *
 * @author Decawave Software
 *
 * @attention Copyright 2018 (c) DecaWave Ltd, Dublin, Ireland.
 *            All rights reserved.
 *
 */

#ifndef __CMD__H__
#define __CMD__H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include <stdint.h>

/* Command parser datas */
typedef struct
{
    int            indx;
    int            equal;
    char         cmd[20];
    int            val;
    const char* ret;
} control_t;


/* command driver states */
enum
{
    _NO_COMMAND = 0,
    _COMMAND_FOUND,
    _COMMAND_ALLOWED
};

/* @fn         command_driver
 * @brief    check if input text in known "COMMAND" or "PARAMETER=VALUE" format
 *             and executes COMMAND or set the PARAMETER to the VALUE
 * */
int command_parser (uint8_t* pdata, uint16_t size);


#ifdef __cplusplus
}

#endif

#endif /* __CMD__H__ */
