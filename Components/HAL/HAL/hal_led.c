#include "hal_led.h"
#include "hal_timer.h"
#include "deca_device_api.h"
#include "deca_regs.h"
#include "Generic.h"

hal_led_t hal_led[HAL_LED_ALL] =
{
    {
        .mode = HAL_LED_MODE_OFF,
        .period = 0
    },
    {
        .mode = HAL_LED_MODE_OFF,
        .period = 0
    },
    {
        .mode = HAL_LED_MODE_OFF,
        .period = 0
    },
    {
        .mode = HAL_LED_MODE_OFF,
        .period = 0
    }

};
/***************************************************************************************************
 * @fn      HalLedInit
 *
 * @brief   Initialize LED Service
 *
 * @param   init - pointer to void that contains the initialized value
 *
 * @return  None
 ***************************************************************************************************/
void HalLedInit (void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;                 //定义GPIO结构体
    RCC_APB2PeriphClockCmd (HAL_LED_RCC, ENABLE);         //使能GPIOB的外设时钟

    // Enable GPIO used for LEDs
    GPIO_InitStructure.GPIO_Pin = HAL_LED1_PIN | HAL_LED2_PIN | HAL_LED3_PIN | HAL_LED4_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (HAL_LED_PORT, &GPIO_InitStructure);

    HalLedSet (HAL_LED_ALL, HAL_LED_MODE_OFF);
}

/***************************************************************************************************
 * @fn      HalLedSet
 *
 * @brief   Tun ON/OFF/TOGGLE given LEDs
 *
 * @param   led - bit mask value of leds to be turned ON/OFF/TOGGLE
 *          mode - BLINK, FLASH, TOGGLE, ON, OFF
 * @return  None
 ***************************************************************************************************/
void HalLedSet (HAL_LED_SN leds, HAL_LED_MODE mode)
{

    switch (leds)
    {
    case HAL_LED1:
        if (mode == HAL_LED_MODE_ON)
            GPIO_SetBits (HAL_LED_PORT, HAL_LED1_PIN);
        else if (mode == HAL_LED_MODE_OFF)
            GPIO_ResetBits (HAL_LED_PORT, HAL_LED1_PIN);

        break;

    case HAL_LED2:
        if (mode == HAL_LED_MODE_ON)
            GPIO_SetBits (HAL_LED_PORT, HAL_LED2_PIN);
        else if (mode == HAL_LED_MODE_OFF)
            GPIO_ResetBits (HAL_LED_PORT, HAL_LED2_PIN);

        break;

    case HAL_LED3:
        if (mode == HAL_LED_MODE_ON)
            GPIO_SetBits (HAL_LED_PORT, HAL_LED3_PIN);
        else if (mode == HAL_LED_MODE_OFF)
            GPIO_ResetBits (HAL_LED_PORT, HAL_LED3_PIN);

        break;

    case HAL_LED4:
        if (mode == HAL_LED_MODE_ON)
            GPIO_SetBits (HAL_LED_PORT, HAL_LED4_PIN);
        else if (mode == HAL_LED_MODE_OFF)
            GPIO_ResetBits (HAL_LED_PORT, HAL_LED4_PIN);

        break;

    case HAL_LED_ALL:
        if (mode == HAL_LED_MODE_ON)
            GPIO_SetBits (HAL_LED_PORT, HAL_LED1_PIN | HAL_LED2_PIN | HAL_LED3_PIN | HAL_LED4_PIN);
        else if (mode == HAL_LED_MODE_OFF)
            GPIO_ResetBits (HAL_LED_PORT, HAL_LED1_PIN | HAL_LED2_PIN | HAL_LED3_PIN | HAL_LED4_PIN);

        break;

    default:
        break;
    }
}


int HalLed_Mode_Set (HAL_LED_SN led, HAL_LED_MODE mode, uint16_t period)
{
    if (mode != HAL_LED_MODE_TOGGLE)
    {
        if (led == HAL_LED_ALL)
        {
            for (int i = 0; i < HAL_LED_ALL; i++)
                hal_led[led].mode = mode;
        }
        else
            hal_led[led].mode = mode;

        HalLedSet (led, mode);
    }
    else              //HAL_LED_MODE_TOGGLE
    {
        switch (led)
        {
        case HAL_LED1:
        case HAL_LED2:
        case HAL_LED3:
        case HAL_LED4:
            hal_led[led].mode = HAL_LED_MODE_TOGGLE;
            hal_led[led].period = period;
            break;

        default:
            break;
        }
    }
}

uint8_t i = 0;
/*
 *  应用层 流水灯
 *
 */
void flow_light (bool status)
{
    if (status)
    {
        if (i < 4)
        {
            dwt_write32bitreg (GPIO_MODE_ID, 0);

            dwt_write32bitreg (GPIO_DIR_ID, 0);

            dwt_write32bitreg (GPIO_OUT_ID, (1 << i));
        }
        else
        {
            HalLedSet (HAL_LED1, HAL_LED_MODE_ON);
        }

        HalDelay_nMs (200);

        if (i < 5)
        {
            i++;
        }
        else
        {
            HalLedSet (HAL_LED1, HAL_LED_MODE_OFF);
            i = 0;
        }
    }
    else
    {
        dwt_write32bitreg (GPIO_MODE_ID, 0x0000000F);

        dwt_write32bitreg (GPIO_DIR_ID, 0x0000000F);

        dwt_write32bitreg (GPIO_OUT_ID, 0x00000000);

        HalLed_Mode_Set (HAL_LED1, HAL_LED_MODE_TOGGLE, Sys_mode_led_blink_slow_time);
    }

}

/*
 *  应用层 LED事件
 *
 */
void HalLed_Blink (uint16_t count)
{
    for (int i = 0; i < HAL_LED_ALL; i++)
    {
        if (hal_led[i].mode != HAL_LED_MODE_TOGGLE)
            continue;

        if (count % hal_led[i].period  == 0)
        {
            if ( (count / hal_led[i].period) % 2)
                HalLedSet (i, HAL_LED_MODE_ON);
            else
                HalLedSet (i, HAL_LED_MODE_OFF);
        }
    }
}


/***************************************************************************************************
***************************************************************************************************/
