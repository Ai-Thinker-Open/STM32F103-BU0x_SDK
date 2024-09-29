/**********************************************************************************
 * @file    main.c
 * @author Ai-Thinker
 * @version V1.0.0
 * @date    2024.5.16
**********************************************************************************/

#include "stm32f10x.h"
#include "Generic.h"
#include "node.h"
#include "tag.h"
#include "OSAL.h"
#include "hal_drivers.h"

#include "config.h"
#include "hal_iic.h"
#include "cmd_fn.h"

#include "shared_functions.h"
#include "user_out.h"
#include "examples_defines.h"

/*******************************************************************************
*******************************************************************************/

void RCC_Configuration_part (void)
{
    ErrorStatus HSEStartUpStatus;
    RCC_ClocksTypeDef RCC_ClockFreq;

    /* 将RCC寄存器重新设置为默认值 */
    RCC_DeInit();

    /* 打开外部高速时钟晶振HSE */
    RCC_HSEConfig (RCC_HSE_ON);

    /* 等待外部高速时钟晶振工作 */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus != ERROR)
    {
        /* 开启Flash预读缓冲功能,时钟起振后使用 */
        FLASH_PrefetchBufferCmd (FLASH_PrefetchBuffer_Enable);

        /* 48~72Mhz推荐Latency为2 */
        FLASH_SetLatency (FLASH_Latency_2);

        /* 设置AHB时钟，72MHz HCLK = SYSCLK */
        RCC_HCLKConfig (RCC_SYSCLK_Div1);
        /* 设置告诉APB2时钟，1分频72MHz PCLK2 = HCLK */
        RCC_PCLK2Config (RCC_HCLK_Div1);
        /* 设置低速APB1时钟，2分频36MHz PCLK1 = HCLK/2 */
        RCC_PCLK1Config (RCC_HCLK_Div2);
        /*  设置ADC时钟 ADCCLK = PCLK2/4 */
        RCC_ADCCLKConfig (RCC_PCLK2_Div6);

        //设置PLL时钟源及倍频系数 不分频：RCC_PLLSource_HSE_Div1 9倍频：RCC_PLLMul_9
        RCC_PLLConfig (RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        /* 打开PLL */
        RCC_PLLCmd (ENABLE);
        /* 等待PLL稳定工作 */
        while (RCC_GetFlagStatus (RCC_FLAG_PLLRDY) == RESET) {}

        /* 选择PLL时钟作为时钟源 */
        RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);

        /* 等待时钟源切换，进入稳定状态 */
        while (RCC_GetSYSCLKSource() != 0x08) {}
    }

    RCC_GetClocksFreq (&RCC_ClockFreq);
}

/*******************************************************************************
* 函数名  : init
* 描述    : 初始化函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
*******************************************************************************/
void init (void)
{
    SystemInit();

    RCC_Configuration_part();

    Hal_Driver_Init();

    App_Module_Init();
}
/*******************************************************************************
* 函数名  : nt_task
* 描述    : main task
* 输入    : 无
* 输出    : 无
* 返回值  : 无
*******************************************************************************/
void nt_task (void)
{
    //读取flash
    load_bssConfig();

    if (sys_para.param_Config.s.userConfig.workmode == 0)
    {
        OLED_ShowStr (19, 2, "Please Send", 2);
        OLED_ShowStr (19, 4, "AT Command", 2);

        while (app.pConfig->s.userConfig.nodeAddr == 0xFFFF) //AOA是否有设置
        {
            App_Module_Sys_Work_Mode_Event();
        }
        if (app.pConfig->s.userConfig.twr_pdoa_mode == 0)
        {
            if (sys_para.flag == 0xAAAA)
            {
                if (sys_para.param_Config.s.userConfig.role == 1) //node
                {
                    node_start();
                }
                else if (sys_para.param_Config.s.userConfig.role == 0) //tag
                {
                    tag_start();
                }
            }
        }
        else
        {
            if (sys_para.param_Config.s.userConfig.role == 1)
            {
                ds_twr_sts_sdc_responder();
            }
            else if (sys_para.param_Config.s.userConfig.role == 0)
            {
                ds_twr_sts_sdc_initiator();
            }
        }
    }
    else
    {
        while (1)
        {
            App_Module_Sys_Work_Mode_Event();
        }
    }

    for (;;);
}

/*******************************************************************************
* 函数名  : main
* 描述    : 主函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
********************************************************************************/
int main (void)
{
    init();
#if(EXAMPLE_DEMO)
    build_examples();
#else
		nt_task();
#endif
    for (;;)
    {
    }
}

