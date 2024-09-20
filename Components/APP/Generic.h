#ifndef __APP_GENERIC_H
#define __APP_GENERIC_H

#ifdef __cplusplus
extern "C"
{
#endif


/*
 *  V1.0.0:
 *          一、Dw3000
 *
 */

/**************************************************************************************************
 *                                                                              INCLUDES
 **************************************************************************************************/
#include <stdbool.h>

#include "Generic_CMD.h"
#include "OSAL_Comdef.h"

#include "default_config.h"
#include "hal_led.h"



/**************************************************************************************************
 *                                                                              CONSTANTS
 **************************************************************************************************/
#define IS_FISRT_POEWRON(x) ((x) != 0xAAAA)

//软件版本
#define uwb_software_ver "V1.0.0"

//UART TO WIFI
#define WIFI_DATE_LEN                   (37)
#define WIFI_DATE_HEADER                (0Xaa)
#define WIFI_DATE_END                   (0X55)
#define WIFI_DATE_VERSION               (0X01)

/***************************************************************************************************
 *                                                                              TYPEDEF
 ***************************************************************************************************/
typedef struct
{
    uint32_t flag;
    uint32_t start_count;

    uint32_t HardFault_error_bit;
    uint32_t MemManage_error_bit;
    uint32_t BusFault_error_bit;
    uint32_t UsageFault_error_bit;

    param_block_t param_Config;

    uint32_t flash_error_count;
    uint32_t check_sum;                           //flash check
} System_Para_t;

typedef struct
{
    uint8_t uart_dma_tx[1400];
    uint8_t uart_dma_tx_tmp[1400];
    int uart_dma_report;
    int uart_dma_index;
} System_uart_dma_t;

enum Sys_power_on
{
    Sys_power_on_restore_factory = 0xAAAA,
    Sys_power_on_running
};

enum Sys_mode_led_blink
{
    Sys_mode_led_blink_invalid   = 0,

    /*闪烁一次周期 */
    Sys_mode_led_blink_fast_time = 2, //0.2s
    Sys_mode_led_blink_slow_time = 10,    //1s
};

typedef enum
{
    /*工作模式 主*/
    Sys_Operate_Mode_INVAILD     = 0x0000,        //无效
    Sys_Operate_Mode_CFG_ING     = 0x0001,        //配置中
    Sys_Operate_Mode_WORK_DONE   = 0x0002,        //工作中
} Sys_Work_Mode;

/***************************************************************************************************
 *                                                                              GLOBAL VARIABLES
 ***************************************************************************************************/
extern System_Para_t sys_para;
extern System_uart_dma_t sys_uart_dma_buf;




/**************************************************************************************************
 *                                        FUNCTIONS - API
 **************************************************************************************************/
extern void App_Module_Init (void);

/********非易失性数据(读/写)操作***********/
extern void App_Module_Sys_Write_NVM (void);
extern void App_Module_Sys_Read_NVM (void);

extern void App_Module_Sys_Write_NVM_NodeTagRole (void);
extern void App_Module_Sys_Read_NVM_NodeTagRole (void);

/********串口/LED/按键/充电/电量事件处理***********/
extern void App_Module_Sys_Work_Mode_Event (void);
static void App_Module_Sys_Deal_UART_CMD_Event (Sys_Work_Mode Mode);
static void App_Modelu_Sys_Deal_IO_LED_Event (Sys_Work_Mode Mode);
static void App_Modelu_Sys_Deal_IO_KEY_Event (void);



extern uint16_t App_Module_Get_SysState_Usercmd (void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}

#endif
#endif//__APP_GENERIC_H

