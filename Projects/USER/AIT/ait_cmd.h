
#ifndef __AIT_CMD_H
#define __AIT_CMD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <ctype.h>
#include <stdint.h>
#include "stm32f10x.h"
#include "deca_types.h"
#include "deca_regs.h"
#include "deca_device_api.h"





typedef struct {
	char *cmd;	
	int (*deal_func)(int opt, int argc, char *argv[]);	
}at_cmd_t;

int at_cmd_recv(uint8_t *data,uint16_t len);

#ifdef __cplusplus
}
#endif

#endif