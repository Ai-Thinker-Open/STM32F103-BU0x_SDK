#include "hal_drivers.h"
#include "Generic.h"
#include "lis2dh12.h"

/**************************************************************************************************
 * @fn      Hal_DriverInit
 *
 * @brief   Initialize HW - These need to be initialized before anyone.
 *
 * @return  None
 **************************************************************************************************/
void Hal_Driver_Init (void)
{
    Hal_NVIC_Init (HAL_NVIC_GROUP_SN);

    /* TIMER */
#if (defined HAL_TIMER) && (HAL_TIMER == TRUE)
    HalTimerInit();
#endif

    /* FLASH */
#if (defined HAL_FLASH) && (HAL_FLASH == TRUE)
    HalFlashInit();
    App_Module_Sys_Read_NVM();

    if (sys_para.param_Config.s.userConfig.workmode == 255)
    {
        sys_para.param_Config.s.userConfig.workmode = 0;
    }

#endif

    /* LED */
#if (defined HAL_LED) && (HAL_LED == TRUE)

    if (sys_para.param_Config.s.userConfig.workmode == 0)
    {
        HalLedInit();
    }

#endif

    /* UART */
#if (defined HAL_USART) && (HAL_USART == TRUE)
    HalUARTInit();
#endif

    /* SPI */
#if (defined HAL_SPI) && (HAL_SPI == TRUE)
    HalSpiInit();
#endif

    /* USB */
#if (defined HAL_USB) && (HAL_USB == TRUE)

    if (sys_para.param_Config.s.userConfig.workmode == 0)
    {
        HalUsbInit();
    }

#endif

    /* OLED */
#if (defined HAL_IIC) && (HAL_IIC == TRUE)

    if (sys_para.param_Config.s.userConfig.workmode == 0)
    {
        Hal_I2C_LCD_Init();
    }

#endif

    /* SENSOR */
#if (defined HAL_SENSOR) && (HAL_SENSOR == TRUE)

    if (sys_para.param_Config.s.userConfig.workmode == 0)
    {
        Hal_I2C_Sensor_Init();//I2C2初始化
        HalIis2dh12Init();//加速度传感器初始化
    }

#endif


}

