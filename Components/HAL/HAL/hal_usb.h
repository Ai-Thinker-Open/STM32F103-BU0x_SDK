#ifndef __HAL_USB_H
#define __HAL_USB_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *                                                                              INCLUDES
 **************************************************************************************************/
#include "stm32f10x.h"
#include "OSAL_Comdef.h"
#include "hal_timer.h"


/**************************************************************************************************
 *                                                                              CONSTANTS
 **************************************************************************************************/
#define HAL_USB_DELAY 3000

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
 * Initialize adc Service.
 */
extern void HalUsbInit (void);

extern int HalUsbRead (uint8_t* buffter, uint32_t len);

extern void HalUsbWrite (uint8_t* buffter, uint32_t len);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif
#endif//__HAL_USB_H
