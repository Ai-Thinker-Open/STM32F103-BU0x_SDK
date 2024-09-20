#ifndef __HAL_SPI_H
#define __HAL_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *                                                                              INCLUDES
 **************************************************************************************************/
#include "stm32f10x.h"
#include "string.h"
#include "OSAL_Comdef.h"

/**************************************************************************************************
 *                                                                              CONSTANTS
 **************************************************************************************************/

#define HAL_SPI_DECA


//DECAWAVE SPI端口定义
#define SPI1_PRESCALER                 SPI_BaudRatePrescaler_2

#define DW1000_RCC                     RCC_APB2Periph_GPIOB

//Decawave SPI端口A
#define DW1000_RST_PORT                GPIOA
#define DW1000_RST_PIN                 GPIO_Pin_0   //PA0
#define DW1000_CS_PORT                 GPIOA
#define DW1000_CS_PIN                  GPIO_Pin_4   //PA4
#define DW1000_WUP_PORT                GPIOB
#define DW1000_WUP_PIN                 GPIO_Pin_0   //PB0
#define DW1000_IRQ_PORT                GPIOB
#define DW1000_IRQ_PIN                 GPIO_Pin_5   //PB5

#define DW1000_RSTIRQ_PIN              GPIO_Pin_0
#define DW1000_RSTIRQ_PORT             GPIOA
#define DW1000_RSTIRQ_EXTI             EXTI_Line0
#define DW1000_RSTIRQ_EXTI_PORT        GPIO_PortSourceGPIOA
#define DW1000_RSTIRQ_EXTI_PIN         GPIO_PinSource0
#define DW1000_RSTIRQ_EXTI_IRQn        EXTI0_IRQn

#define DW1000_IRQ_EXTI                EXTI_Line5
#define DW1000_IRQ_EXTI_PORT           GPIO_PortSourceGPIOB
#define DW1000_IRQ_EXTI_PIN            GPIO_PinSource5
#define DW1000_IRQ_EXTI_IRQn           EXTI9_5_IRQn
#define DW1000_IRQ_EXTI_USEIRQ         ENABLE
#define DW1000_IRQ_EXTI_NOIRQ          DISABLE


//Decawave 公共端口
#define SPI1_PORT                      GPIOA
#define SPI1_SCK_PIN                   GPIO_Pin_5
#define SPI1_MISO_PIN                  GPIO_Pin_6
#define SPI1_MOSI_PIN                  GPIO_Pin_7

#define SPIx                           SPI1

/***************************************************************************************************
 *                                                                              TYPEDEF
 ***************************************************************************************************/


/***************************************************************************************************
 *                                                                              GLOBAL VARIABLES
 ***************************************************************************************************/



/**************************************************************************************************
 *                                        FUNCTIONS - API
 **************************************************************************************************/

/*
 * Initialize SPI Service.
 */
extern void HalSpiInit (void);

#ifdef HAL_SPI_LIS3DH
extern uint8_t SPI1_ReadWriteByte (uint8_t TxData);
#endif
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif
#endif
