#include "hal_iic.h"
#include "OLED_I2C.h"
#include "Generic.h"
#include "cmd_fn.h"
#include "config.h"

int Hal_I2C_LCD_Init (void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB1PeriphClockCmd (RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

    I2C_DeInit (I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;//400K

    I2C_Cmd (I2C1, ENABLE);
    I2C_Init (I2C1, &I2C_InitStructure);

    OLED_Init();

    OLED_Fill (0xff); //È«ÁÁ
    OLED_Fill (0x00);
    return 0;
}

int Hal_I2C_Sensor_Init (void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB1PeriphClockCmd (RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

    I2C_DeInit (I2C2); //Ê¹ÓÃI2C2
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;//400K

    I2C_Cmd (I2C2, ENABLE);
    I2C_Init (I2C2, &I2C_InitStructure);

    return 0;
}

void HalI2cInit (void)
{
    OLED_Fill (0x00);

    if (app.pConfig->s.userConfig.twr_pdoa_mode == 0)
    {
        uint8_t buf[32];
        sprintf (buf, "A%d", sys_para.param_Config.s.userConfig.nodeAddr);

        if (sys_para.param_Config.s.userConfig.Pa_mod == 1)
        {
            OLED_ShowStr (39, 2, "PA TWR", 2);
        }
        else
        {
            OLED_ShowStr (35, 2, "xPA TWR", 2);
        }

        if (sys_para.param_Config.s.userConfig.role == 1)
        {
            OLED_ShowStr (22, 6, "ID:", 1);
            OLED_ShowStr (35, 6, buf, 1);
            OLED_ShowStr (59, 6, "T0:---- m", 1);
        }
        else
        {
            OLED_ShowStr (40, 6, "ID:", 2);
            OLED_ShowStr (64, 6, buf, 2);
        }
    }
    else
    {
        OLED_ShowStr (48, 1, "PDoA", 2);
    }
}

void Screen_Cleaning (void)
{
    OLED_Fill (0x00);
    uint8_t buf[32];
    sprintf (buf, "%d", sys_para.param_Config.s.userConfig.nodeAddr);
    OLED_ShowCN (16, 0, 0);
    OLED_ShowCN (32, 0, 1);
    OLED_ShowCN (48, 0, 2);
    OLED_ShowCN (64, 0, 3);
    OLED_ShowStr (81, 0, " TWR", 2);
    OLED_ShowCN (32, 2, 8);
    OLED_ShowCN (48, 2, 9);
    OLED_ShowStr (64, 2, "ID:", 2);
    OLED_ShowStr (88, 2, buf, 2);
}

void OLED_Display (float New_distance)
{
    uint8_t buf[32];

    if (New_distance >= 0)
    {
        memset (buf, 0, sizeof (buf));

        if (New_distance <= 9)
        {
            sprintf (buf, "%0.3f", New_distance / 100);
        }
        else if (New_distance <= 999)
        {
            sprintf (buf, "%0.3f", New_distance / 100);
        }
        else if (New_distance <= 9999)
        {
            sprintf (buf, "%0.2f", New_distance / 100);
        }
        else
        {
            sprintf (buf, "%0.3f", New_distance / 100);
        }

        OLED_ShowStr (75, 6, buf, 1);
    }
}







