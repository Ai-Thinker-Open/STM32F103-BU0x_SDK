#ifndef _UWB_H
#define _UWB_H

#include "deca_types.h"
#include "hal_spi.h"

#define DEV_ID_ID                   	(0x00)
#define DWT_DEVICE_ID               	(0xDECA0130)		//!< DW1000 MP device ID

#define DWT_SUCCESS                 	(0)
#define DWT_ERROR                   	(-1)

#define writetospi                  	writetospi_serial
#define readfromspi                		readfromspi_serial
#define port_GetEXT_IRQStatus()         EXTI_GetITEnStatus(DW1000_IRQ_EXTI_IRQn)
#define port_DisableEXT_IRQ()           NVIC_DisableIRQ(DW1000_IRQ_EXTI_IRQn)
#define port_EnableEXT_IRQ()            NVIC_EnableIRQ(DW1000_IRQ_EXTI_IRQn)
#define port_CheckEXT_IRQ()             GPIO_ReadInputDataBit(DW1000_IRQ_PORT, DW1000_IRQ_PIN)

#define port_SPIx_set_chip_select()		GPIO_SetBits(DW1000_CS_PORT,DW1000_CS_PIN)
#define port_SPIx_clear_chip_select()	GPIO_ResetBits(DW1000_CS_PORT,DW1000_CS_PIN)


#define portGetTickCount() 				portGetTickCnt()

extern __IO unsigned long time32_incr;

extern void Sleep(unsigned int time_ms);

extern void reset_DWIC(void);         

extern void SPI_ConfigFastRate(uint16_t scalingfactor);

extern void port_set_dw_ic_spi_slowrate(void);

extern void port_set_dw_ic_spi_fastrate(void);

extern void deca_sleep(unsigned int time_ms);

extern void setup_DW1000RSTnIRQ(int enable);

extern void disable_deca_irq(void);

extern void enable_deca_irq(void);


//extern int writetospi_serial( uint16_t headerLength,
//			   	    const uint8_t *headerBuffer,
//					uint32_t bodylength,
//					const uint8_t *bodyBuffer
//				  );
//extern int readfromspi_serial( uint16_t	headerLength,
//			    	 const uint8_t *headerBuffer,
//					 uint32_t readlength,
//					 uint8_t *readBuffer );


#endif


