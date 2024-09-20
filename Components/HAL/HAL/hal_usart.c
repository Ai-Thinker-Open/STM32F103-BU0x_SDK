#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include "hal_usart.h"

#define SRC_USART1_DR   (&(USART1->DR))     //串口接收寄存器


#define DMA1_MEM_LEN 256//保存DMA每次数据传送的长度
char _dbg_TXBuff[DMA1_MEM_LEN];

char USART1_DMA_RX_BYTE;

/*
 *******************************************************************************
 *   串口1发送函数
 *   由于使用的是普通模式,在每次发送完成后,重装载通道
 *   装载好通道后等待发送完成。
 *******************************************************************************
 */
uint16_t USART1_SendBuffer (const char* buffer, uint16_t length, int flag)
{
#ifdef HAL_USART1_DMA
    if (flag == true)
    {
        for (int i = 0; i < length; i++)
        {
            USART1->SR;

            /* e.g. 给USART写一个字符 */
            USART_SendData (USART1, (uint8_t) buffer[i]);

            /* 循环直到发送完成 */
            while (USART_GetFlagStatus (USART1, USART_FLAG_TC) == RESET);
        }
    }
    else
    {
        DMA_Cmd (DMA1_Channel4, DISABLE); //数据传输完成，关闭DMA4通道
        DMA1_Channel4->CNDTR = length;                      //数据传输数目
        DMA1_Channel4->CMAR = (u32) buffer;             //内存地址
        DMA_Cmd (DMA1_Channel4, ENABLE);                    //使能DMA通道4
    }
#else
    for (int i = 0; i < length; i++)
    {
        USART1->SR;

        /* e.g. 给USART写一个字符 */
        USART_SendData (USART1, (uint8_t) buffer[i]);

        /* 循环直到发送完成 */
        while (USART_GetFlagStatus (USART1, USART_FLAG_TC) == RESET);
    }

#endif
    return length;
}

/*
 *******************************************************************************
 *      DMA方式的_dbg_printf
 *******************************************************************************
 */
void _dbg_printf (const char* format, ...)
{
#ifdef HW_RELEASE
#else
    uint32_t length;
    va_list args;

    va_start (args, format);
    length = vsnprintf ( (char*) _dbg_TXBuff, sizeof (_dbg_TXBuff), (char*) format, args);
    va_end (args);

    USART1_SendBuffer ( (const char*) _dbg_TXBuff, length, true);
#endif
}
#define printf _dbg_printf
/*
 *******************************************************************************
 *      串口1DMA初始化
 *      关闭RXNE和TC中断,开启IDLE
 *      配置RX,TX传输的目的地址和源地址
 *******************************************************************************
 */
void HalUASRT1_DMA_Config (void)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE);

#ifdef HAL_USART1_DMA
    //TX发送DMA
    DMA_DeInit (DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32) SRC_USART1_DR;             //DMA外设基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          //外设作为数据传输目的地
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据宽度8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;         //内存数据宽度8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           //正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //优先级：高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                //非内存到内存
    DMA_Init (DMA1_Channel4, &DMA_InitStructure);

    DMA_ITConfig (DMA1_Channel4, DMA_IT_TC, ENABLE);    //配置DMA发送完成后产生中断
    USART_DMACmd (USART1, USART_DMAReq_Tx, ENABLE); //使能USART的DMA发送请求
#endif

    //RX接受DMA
    DMA_DeInit (DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32) SRC_USART1_DR;                  //DMA外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32) &USART1_DMA_RX_BYTE;                    //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                       //外设作为数据传输的来源
    DMA_InitStructure.DMA_BufferSize = 1;                                    //DMA缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                  //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据宽度8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          //内存数据宽度8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                          //循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                          //优先级：高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                 //非内存到内存
    DMA_Init (DMA1_Channel5, &DMA_InitStructure);                            //初始化DMA

    DMA_ITConfig (DMA1_Channel5, DMA_IT_TC, ENABLE); //使能DMA通道6传输完成中断
    DMA_Cmd (DMA1_Channel5, ENABLE);                                //使能DMA通道6
    USART_DMACmd (USART1, USART_DMAReq_Rx, ENABLE); //使能USART2接收DMA请求
}

void HalUSART1_DMA_TX_NVIC_Config (void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
}
void HalUSART1_DMA_RX_NVIC_Config (void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
}


void HalUASRT1_NVIC_Config (void)
{

}


void HalUSART1_Init (u32 bound)
{
    USART_InitTypeDef USART_InitStructure;

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init (USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig (USART1, USART_IT_RXNE, ENABLE); //开启串口接受中断
    USART_Cmd (USART1, ENABLE);                   //使能串口
}

void HalUSART1_IO_Init (void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); //使能USART1，GPIOA时钟
    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init (GPIOA, &GPIO_InitStructure); //初始化GPIOA.9

    //USART1_RX   GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init (GPIOA, &GPIO_InitStructure); //初始化GPIOA.10
}

void HalUSART2_Init (void)
{
    GPIO_InitTypeDef            GPIO_InitStructure;
    USART_InitTypeDef           USART_InitStructure;  //定义串口初始化结构体
    NVIC_InitTypeDef            NVIC_InitStructure;

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;          //选中串口默认输出管脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //定义输出最大速率
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义管脚9的模式
    GPIO_Init (GPIOA, &GPIO_InitStructure);          //调用函数，把结构体参数输入进行初始化

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200; //波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;     //校验位 无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;     //使能接收和发送引脚

    USART_Init (USART2, &USART_InitStructure); //将以上赋完值的结构体带入库函数USART_Init进行初始化
    USART_Cmd (USART2, ENABLE); //开启USART2

    USART_ITConfig (USART2, USART_IT_RXNE, ENABLE);

    USART_ClearFlag (USART2, USART_FLAG_TC);
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

}

void USART2SendByteData (unsigned char c)
{
    USART_SendData (USART2, c);
    while (USART_GetFlagStatus (USART2, USART_FLAG_TXE) == RESET);
}


void USART2SendDatas (unsigned char* str, char len)
{
    for (char n = 0; n < len; n++)
    {
        USART_SendData (USART2, str[n]);
        while (USART_GetFlagStatus (USART2, USART_FLAG_TXE) == RESET);

    }
}
#if 1
void USART2_IRQHandler (void)
{

    char buff = 0;

    if (USART_GetITStatus (USART2, USART_IT_RXNE) != RESET)
    {

        buff = USART_ReceiveData (USART2);
        USART2SendByteData (buff);
//      //UartRevOneByte(buff);
        USART_ClearITPendingBit (USART2, USART_IT_RXNE);
    }

}
#endif

void HalUARTInit (void)
{
#ifdef HAL_USART1
    HalUSART1_IO_Init();
    HalUSART1_Init (115200);
#ifdef HAL_USART1_DMA
    HalUASRT1_DMA_Config();
    HalUSART1_DMA_TX_NVIC_Config();
    HalUSART1_DMA_RX_NVIC_Config();
#endif
#endif
#ifdef HAL_USART2
    HalUSART2_Init();
#endif

}


