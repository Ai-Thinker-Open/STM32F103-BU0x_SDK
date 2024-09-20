#include "hal_spi.h"


#ifdef HAL_SPI_DECA
void Hal_DECA_KEY_NVIC_Config()
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DW1000_IRQ_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DW1000_IRQ_EXTI_NOIRQ;

    NVIC_Init (&NVIC_InitStructure);
}

void HalDecaExitInit (void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    GPIO_EXTILineConfig (DW1000_IRQ_EXTI_PORT, DW1000_IRQ_EXTI_PIN);

    EXTI_InitStructure.EXTI_Line = DW1000_IRQ_EXTI;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //MPW3 IRQ polarity is high by default
    EXTI_InitStructure.EXTI_LineCmd = DW1000_IRQ_EXTI_USEIRQ;
    EXTI_Init (&EXTI_InitStructure);

    Hal_DECA_KEY_NVIC_Config();
}


int Hal_SPI1_Deca_Init (void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    SPI_I2S_DeInit (SPI1);

    /* 使能APB2上的 SPI */
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);
    /* 使能APB2上的GPIOA和GPIOB */
    RCC_APB2PeriphClockCmd (
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    // 设置SPI1模式
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI1_PRESCALER;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init (SPI1, &SPI_InitStructure);

    // SPI1 SCK与MOSI设置
    GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN | SPI1_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init (SPI1_PORT, &GPIO_InitStructure);

    // SPI1 MISO设置
    GPIO_InitStructure.GPIO_Pin = SPI1_MISO_PIN;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;

    GPIO_Init (SPI1_PORT, &GPIO_InitStructure);

    // SPI1 CS设置
    GPIO_InitStructure.GPIO_Pin = DW1000_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (DW1000_CS_PORT, &GPIO_InitStructure);

    SPI_SSOutputCmd (SPI1, DISABLE);

    // 使能SPI1
    SPI_Cmd (SPI1, ENABLE);

    // 设置CS片选拉高
    GPIO_SetBits (DW1000_CS_PORT, DW1000_CS_PIN);

    //dw1000 外部中断配置
    RCC_APB2PeriphClockCmd (DW1000_RCC, ENABLE); //使能PA端口时钟
    GPIO_InitStructure.GPIO_Pin = DW1000_IRQ_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init (DW1000_IRQ_PORT, &GPIO_InitStructure);

    HalDecaExitInit();

    return 0;
}
#endif

void HalSpiInit (void)
{
#ifdef  HAL_SPI_DECA
    //dw1000 引脚初始化
    Hal_SPI1_Deca_Init();
#endif
}

