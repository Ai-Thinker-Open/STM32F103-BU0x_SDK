#ifndef __HAL_TIMER_H
#define __HAL_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *                                                                              INCLUDES
 **************************************************************************************************/
#include "OSAL_Comdef.h"
#include "stm32f10x.h"


/**************************************************************************************************
 *                                                                              CONSTANTS
 **************************************************************************************************/
#define HAL_SysTick

#define HAL_TIMER2
#define HAL_IWDG


#define CLOCKS_PER_SEC      1000

#define TIM2_ARR            999     //100ms
#define TIM2_PSC            7199
/***************************************************************************************************
 *                                                                              TYPEDEF
 ***************************************************************************************************/

/**************************************************************************************************
 *                                        FUNCTIONS - API
 **************************************************************************************************/


static void HalTimeSysTick_Init (void);
static void HalTimer2_Init (void);
static void HalTimer4_Init (void);
static void HalIWDG_Init (unsigned char prer, unsigned int rlr);

extern void HalTimerInit (void);
extern void HalDelay_nMs (uint32_t nms);
extern void HalIWDG_Feed (void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif//__HAL_TIMER_H

