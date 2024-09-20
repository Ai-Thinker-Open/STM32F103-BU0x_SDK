/************************************************************************************
* File name: lis2dh12.c
* Project  : HelTec.uvprij
* Processor: STM32F103C8T6
* Compiler : MDK fo ARM
*
* Author : Ai-Thinker
* Version: 1.00
* Date   : 2024.5.27
*************************************************************************************/
#include "lis2dh12.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "hal_timer.h"

#define IIC_TIMEOUT 100
int IIc_flag = 1 ;//存在1， 不存在0

/*******************************************************************************
* Function Name  : I2C_WriteOneByte
* Description    : 通过指定I2C接口写入一个字节数据
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
*                  - addr:预写入字节地址
*                  - value:写入数据
* Output         : None
* Return         : 成功返回0
* Attention      : None
*******************************************************************************/
uint8_t I2C_WriteOneByte (I2C_TypeDef *I2Cx, uint8_t I2C_Addr, uint8_t addr, uint8_t value)
{
    uint16_t timeout = 0;
    /* 起始位 */
    I2C_GenerateSTART (I2Cx, ENABLE);
    timeout = IIC_TIMEOUT;
    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_MODE_SELECT) && (timeout--));
    I2C_Send7bitAddress (I2Cx, I2C_Addr, I2C_Direction_Transmitter);

    if (timeout == 0)
        IIc_flag = 0;

    timeout = IIC_TIMEOUT;
    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && (timeout--));
    /*发送地址*/
    I2C_SendData (I2Cx, addr);
    timeout = IIC_TIMEOUT;
    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && (timeout--));
    /* 写一个字节*/
    I2C_SendData (I2Cx, value);
    timeout = IIC_TIMEOUT;
    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && (timeout--));
    /* 停止位*/
    I2C_GenerateSTOP (I2Cx, ENABLE);
    /*stop bit flag*/
    timeout = IIC_TIMEOUT;
    while (I2C_GetFlagStatus (I2Cx, I2C_FLAG_STOPF) && (timeout--));

    //I2C_AcknowledgePolling(I2Cx,I2C_Addr);
    I2C_AcknowledgeConfig (I2Cx, DISABLE); // disable acknowledge
    HalDelay_nMs (80);
    return 0;
}

/* iic 通讯相关操作 */
int32_t lis2dh12_iic_write_byte (uint8_t addr, uint8_t data)
{
    I2C_WriteOneByte (I2C2, 0x32, addr, data);
    return 0;

}

int32_t HalIis2dh12Init()
{
    /* Initialization of sensor */
    lis2dh12_iic_write_byte (0x20, 0x37);   /* CTRL_REG1(20h): 关闭sensor，设置进入掉电模式 ODR 25HZ */
    //  lis2dh12_iic_write_byte(0x20, 0x57);    /* CTRL_REG1(20h): 关闭sensor，设置进入低功耗模式 ODR 100HZ */
    lis2dh12_iic_write_byte (0x21, 0x03);   /* CTRL_REG2(21h): IA1、IA2 开启高通滤波 bc */
    lis2dh12_iic_write_byte (0x22, 0xc0);   /* CTRL_REG3(22h): 0x80 使能单击中断到INT_1 INT_2 */
    lis2dh12_iic_write_byte (0x23, 0x88); /* CTRL_REG4(23h): 使能快，数据更新，全量程+/-2G，非常精度模式 */
    //  lis2dh12_iic_write_byte(0x25, 0x00);  /* CTRL_REG6(25h): 高电平(上升沿)触发中断 */

    /* INT1 翻转检测，中断*/                            //0x6a
    lis2dh12_iic_write_byte (0x30, 0x7f); /* INT1_CFG(30h): 使能，6D X/Y/Z任一超过阈值中断 */
    //  lis2dh12_iic_write_byte(0x30, 0x4f);  /* INT1_CFG(30h): 使能，6D X/Y任一超过阈值中断 */
    //  lis2dh12_iic_write_byte(0x31, 0x20);  /* INT1_SRC(31h): 设置中断源 */

    lis2dh12_iic_write_byte (0x32, 0x02);   /* INT1_THS(32h): 设置中断阀值 0x10: 16*2(FS)  0x20: 32*16(FS) */

    //  lis2dh12_iic_write_byte(0x33, 0x02);    /* INT1_DURATION(33h): 1LSB=1/ODR  如果ODR=25HZ  那么1LSB=40ms 设置延时 1s,对应25->0x19 */
    //  lis2dh12_iic_write_byte(0x33, 0x03);  /* INT1_DURATION(33h): 1LSB=1/ODR  如果ODR=50HZ   那么1LSB=20ms 设置延时 1s,对应50->0x32 */
    lis2dh12_iic_write_byte (0x33, 0x03);   /* INT1_DURATION(33h): 1LSB=1/ODR  如果ODR=100HZ  那么1LSB=10ms 设置延时 1s,对应100->0x64 */


    //  /* INT2 单击中断 */
    ////    lis2dh12_iic_write_byte(0x24, 0x01);    /* CTRL_REG5(24h):  */
    //  lis2dh12_iic_write_byte(0x25, 0xa0);  /* CTRL_REG6(25h): Click interrupt on INT2 pin */
    //
    //  lis2dh12_iic_write_byte(0x38, 0x15);    /* CLICK_CFG (38h): 单击识别中断使能 */
    ////    lis2dh12_iic_write_byte(0x39, 0x10);
    //  lis2dh12_iic_write_byte(0x3a, 0x7f);  /* CLICK_THS (3Ah): 单击阀值 */
    //  lis2dh12_iic_write_byte(0x3b, 0xff);  /* TIME_LIMIT (3Bh): 时间限制窗口6 ODR 1LSB=1/ODR 1LSB=1/100HZ,10ms,设置延时1s,对应100—>0x64*/
    //  lis2dh12_iic_write_byte(0x3c, 0xff);  /* TIME_LATENCY (3Ch): 中断电平持续时间1 ODR=10ms */
    //  lis2dh12_iic_write_byte(0x3d, 0x01);  /* TIME_WINDOW (3Dh):  单击时间窗口 */

    /* Start sensor */
    //  lis2dh12_iic_write_byte(0x20, 0x37);
    lis2dh12_iic_write_byte (0x20, 0x5f); /* CTRL_REG1(20h): Start sensor at ODR 100Hz Low-power mode */
}

int32_t drv_lis2dh12_iic_read_byte (uint8_t AddressByte, uint8_t* data)
{
    uint8_t ReceiveData = 0;
    I2C_TypeDef *I2Cx = I2C2;
    uint8_t AddressDevice = 0x32;
    uint32_t timeout = IIC_TIMEOUT;;

    while (I2C_GetFlagStatus (I2Cx, I2C_FLAG_BUSY) && timeout > 0)
    {
        timeout--;
    }
    if (timeout == 0)
    {
        return -1;
    }
    I2C_GenerateSTART (I2Cx, ENABLE);
    timeout = IIC_TIMEOUT;

    while (!I2C_GetFlagStatus (I2Cx, I2C_FLAG_SB) && timeout > 0) // wait Generate Start
    {
        timeout--;
    }
    if (timeout == 0)
    {
        return -1;
    }
    I2C_Send7bitAddress (I2Cx, AddressDevice, I2C_Direction_Transmitter);
    timeout = IIC_TIMEOUT;

    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && timeout > 0) // wait send Address Device{
    {
        timeout--;
    }
    if (timeout == 0)
    {
        return -1;
    }
    I2C_SendData (I2Cx, AddressByte);
    timeout = IIC_TIMEOUT;

    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && timeout > 0) // wait send Address Byte
    {
        timeout--;
    }
    if (timeout == 0)
    {
        return -1;
    }
    I2C_GenerateSTART (I2Cx, ENABLE);
    timeout = IIC_TIMEOUT;

    while (!I2C_GetFlagStatus (I2Cx, I2C_FLAG_SB) && timeout > 0) // wait Generate Start
    {
        timeout--;
    }
    if (timeout == 0)
    {
        return -1;
    }
    I2C_Send7bitAddress (I2Cx, AddressDevice, I2C_Direction_Receiver);
    timeout = IIC_TIMEOUT;

    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && timeout > 0) // wait Send Address Device As Receiver
    {
        timeout--;
    }
    if (timeout == 0)
    {
        return -1;
    }
    timeout = IIC_TIMEOUT;

    while (!I2C_CheckEvent (I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) && timeout > 0) // wait Receive a Byte
    {
        timeout--;
    }
    if (timeout == 0)
    {
        return -1;
    }
    ReceiveData = I2C_ReceiveData (I2Cx);

    I2C_NACKPositionConfig (I2Cx, I2C_NACKPosition_Current); // send not acknowledge
    I2C_AcknowledgeConfig (I2Cx, DISABLE); // disable acknowledge

    I2C_GenerateSTOP (I2Cx, ENABLE);
    timeout = IIC_TIMEOUT;
    while (I2C_GetFlagStatus (I2Cx, I2C_FLAG_STOPF) && timeout > 0) // wait Generate Stop Condition
    {
        timeout--;
    }
    if (timeout == 0)
    {
        return -1;
    }

    I2C_AcknowledgeConfig (I2Cx, DISABLE); // disable acknowledge

    *data = ReceiveData;
    //return ReceiveData;

    //*data=I2C_ReadOneByte(I2C2,0x32,addr);
    return 0;
}

/* 获取当前倾斜角度 */
int drv_lis2dh12_get_angle (float* get)
{
    float acc_x, acc_y, acc_z, acc_g;
    float angle_x, angle_y, angle_z, angle_xyz;
    int8_t data[6] = {0};
    uint8_t i;
    int ret = 0;

    for (i = 0; i < 6; i++)
    {
        ret = drv_lis2dh12_iic_read_byte (0x28 + i, data + i);
        if (ret != 0)
        {
            return -1;
        }
        //_dbg_printf("i:%d data:%d\r\n",i,data[i]);
    }


    /* x, y, z 轴加速度 */
    acc_x = (LIS2DH12_FROM_FS_2g_HR_TO_mg (* (int16_t*) data));
    acc_y = (LIS2DH12_FROM_FS_2g_HR_TO_mg (* (int16_t*) (data + 2)));
    acc_z = (LIS2DH12_FROM_FS_2g_HR_TO_mg (* (int16_t*) (data + 4)));

    _dbg_printf ("acc_x:%f\r\n", acc_x);
    _dbg_printf ("acc_y:%f\r\n", acc_y);
    _dbg_printf ("acc_z:%f\r\n", acc_z);

    acc_x = abs (acc_x);
    acc_y = abs (acc_y);
    acc_z = abs (acc_z);



    /* 重力加速度 */
    acc_g = sqrt (pow (acc_x, 2) + pow (acc_y, 2) + pow (acc_z, 2));
    //printf("acc_g:%f\r\n",acc_g);

    //return acc_y;
    if (acc_z > acc_g)
        acc_z = acc_g;

    /* angle_z/90 = asin(acc_z/acc_g)/π/2 */
    angle_z = asin (acc_z / acc_g) * 2 / 3.14 * 90;
    angle_z = 90 - angle_z;
    if (angle_z < 0)
        angle_z = 0;
    *get = angle_z;
    return 0;
}
