#include <string.h>
#include <stdio.h>
#include <math.h>

#include "Generic.h"
#include "cmd.h"

#include "hal_timer.h"
#include "hal_flash.h"

#include "default_config.h"
#include "cmd_fn.h"

param_block_t defaultFConfig  = DEFAULT_CONFIG;

System_uart_dma_t sys_uart_dma_buf =
{
    .uart_dma_tx = 0,
    .uart_dma_report = true,
    .uart_dma_index = 0
};


//全局结构体
System_Para_t sys_para =
{
    .start_count = 0x0000,
    .param_Config = DEFAULT_CONFIG
};

//备份全局结构体
System_Para_t sys_para_bak =
{
    .start_count = 0x0000,
    .param_Config = DEFAULT_CONFIG
};

bool led_test = false;


void App_Module_sys_para_debug (void)
{
    _dbg_printf ("*************************************\n");

    _dbg_printf ("   Time: %s %s\n", __TIME__, __DATE__);

    _dbg_printf ("   Version：Ai-Thinker-%s\r\n", uwb_software_ver);

    _dbg_printf ("*************************************\n");
}



uint32_t Check_Sum (uint32_t* Buf, uint8_t len)
{
    uint8_t i = 0;
    uint32_t sum = 0;
    uint32_t checksum = 0;

    for (i = 0; i < len; i++)
    {
        sum += *Buf++;
    }

    checksum = sum & 0xffffffff;

    return checksum;
}

void App_Module_sys_para_Init (void)
{
    sys_para.flag = 0xAAAA;
    sys_para.start_count = 0;

    sys_para.HardFault_error_bit  = 0;
    sys_para.MemManage_error_bit = 0 ;
    sys_para.BusFault_error_bit = 0;
    sys_para.UsageFault_error_bit = 0;
    sys_para.flash_error_count = 0;

    sys_para.param_Config = defaultFConfig;
}


void App_Module_Sys_Write_NVM (void)
{
    while (HalWrite_Flash (PAGE62_ADDR, &sys_para.flag, sizeof (sys_para) / 4) == 0)
    {
        HalDelay_nMs (100);
    }
}


void App_Module_Sys_Read_NVM (void)
{
    HalRead_Flash (PAGE62_ADDR, &sys_para.flag, sizeof (sys_para) / 4);
}

/*
 *  从flash读取配置信息
 *
 */
void App_Module_sys_para_read()
{
    bool is_back2factory = false;

    App_Module_Sys_Read_NVM();

    if (sys_para.flag != 0xAAAA)
    {
        is_back2factory = true;
    }
    else
    {
        sys_para.start_count += 1;
        //App_Module_Sys_Write_NVM();

    }

    if (is_back2factory == true)
    {
        App_Module_sys_para_Init();
        App_Module_Sys_Write_NVM();
        NVIC_SystemReset();
    }

#ifndef JOHHN
    GPIO_PinRemapConfig (GPIO_Remap_SWJ_Disable, DISABLE);

    if (sys_para.param_Config.s.userConfig.workmode == 1)
    {
        memcpy (&sys_para_bak, &sys_para, sizeof (sys_para));
        sys_para_bak.param_Config.s.userConfig.workmode = 0;

        //写入主存储区
        while (HalWrite_Flash (PAGE62_ADDR, &sys_para_bak.flag, sizeof (sys_para_bak) / 4) == 0)
        {
            HalDelay_nMs (100);
        }
    }

#endif
}

void App_Module_Init (void)
{
    App_Module_CMD_Queue_Init();

    HalDelay_nMs (500);

    App_Module_sys_para_read();//Read configuration

    App_Module_sys_para_debug();//Serial print configuration
}



/*
 *  Event pattern
 *
 */
void App_Module_Sys_Work_Mode_Event()
{
    Sys_Work_Mode mode = (sys_para.param_Config.s.userConfig.nodeAddr != 0xffff) ? (Sys_Operate_Mode_WORK_DONE) : (Sys_Operate_Mode_CFG_ING);
    App_Module_Sys_Deal_UART_CMD_Event (mode);
    App_Modelu_Sys_Deal_IO_LED_Event (mode);

    if (led_test == 1)
    {
        flow_light (1);
    }
}


/*
 *  Serial port event
 *
 */
void App_Module_Sys_Deal_UART_CMD_Event (Sys_Work_Mode Mode)
{
    App_Module_Process_USB_CMD();
    App_Module_Process_USART_CMD();
//  if(Mode == Sys_Operate_Mode_CFG_ING)
//      port_tx_msg("Please Send AT Command...\r\n", strlen("Please Send AT Command...\r\n"));

    if (sys_uart_dma_buf.uart_dma_index != 0)
    {
        port_tx_msg (NULL, 0);
    }
}

/*
 *  LED mode configuration
 *
 */
void App_Modelu_Sys_Deal_IO_LED_Event (Sys_Work_Mode Mode)
{
    static int val_mode = Sys_Operate_Mode_INVAILD;

    if (Mode == val_mode)
    {
        return;
    }
    else
    {
        val_mode = Mode;
    }

    if (sys_para.param_Config.s.userConfig.workmode == 0)
    {
        switch (Mode)
        {
        case Sys_Operate_Mode_CFG_ING:
            HalLed_Mode_Set (HAL_LED1, HAL_LED_MODE_TOGGLE, Sys_mode_led_blink_slow_time);
            break;

        case Sys_Operate_Mode_WORK_DONE:
            HalLed_Mode_Set (HAL_LED1, HAL_LED_MODE_TOGGLE, Sys_mode_led_blink_fast_time);
            break;

        default:
            break;
        }
    }
}




uint16_t App_Module_Get_SysState_Usercmd (void)
{
    uint16_t a2t_usercmd = 0;
    return a2t_usercmd;
}

