/*
 * Stm32_F103C8_USART_driver.h
 *
 *  Created on: Nov 3, 2023
 *      Author: galal
 */

#ifndef INC_STM32_F103C8_USART_DRIVER_H_
#define INC_STM32_F103C8_USART_DRIVER_H_

#include "stm32f103x8.h"
#include "Stm32_F103C8_gpio_driver.h"
#include "Stm32_F103C8_RCC_driver.h"

//Configuration structure
typedef struct
{
	uint8_t 		USART_Mode;		//Specifies Tx/Rx Enable/Disable
									// this parameter must be set based on @ref USART_Mode_define
	uint32_t		BaudRate;		//Specifies USART Communication Baud Rate
									// this parameter must be set based on @ref USART_BaudRate_define
	uint16_t		PayloadLen;		//Specifies number of data bits transmitted or received in a frame
									// this parameter must be set based on @ref USART_PayloadLen_define
	uint16_t		Parity;			//Specifies the parity mode
									// this parameter must be set based on @ref USART_Parity_define
	uint16_t		StopBits;		//Specifies number of stop bits in a frame
									// this parameter must be set based on @ref USART_StopBits_define
	uint16_t		HwFlowCtl;		//Specifies whether the hardware flow control mode is enabled or disabled
									// this parameter must be set based on @ref USART_HwFlowCtl_define
	uint16_t		IRQ_Enable;		//Enable or Disable IRQ Tx/Rx
									// this parameter must be set based on @ref USART_IRQ_define
	void(* P_IRQ_CallBack)(void);	//set the C Function() which will be called once the IRQ Happen

}USART_Config_t;

//==================================================
//Macros Configurations References
//==================================================

//@ref USART_Mode_define
#define USART_Mode_Tx					(1<<3)	//Bit 3 TE: Transmitter enable
#define USART_Mode_Rx					(1<<2)	//Bit 2 RE: Receiver enable
#define USART_Mode_Tx_Rx				(1<<3 | 1<<2)	//Bit 3 TE: Transmitter enable -- Bit 2 RE: Receiver enable

//@ref USART_BaudRate_define
#define USART_BaudRate_2400				((uint32_t)(2400))
#define USART_BaudRate_9600				((uint32_t)(9600))
#define USART_BaudRate_19200			((uint32_t)(19200))
#define USART_BaudRate_57600			((uint32_t)(57600))
#define USART_BaudRate_115200			((uint32_t)(115200))
#define USART_BaudRate_230400			((uint32_t)(230400))
#define USART_BaudRate_460800			((uint32_t)(460800))
#define USART_BaudRate_921600			((uint32_t)(921600))
#define USART_BaudRate_2250000			((uint32_t)(2250000))
#define USART_BaudRate_4500000			((uint32_t)(4500000))

//@ref USART_PayloadLen_define
#define USART_PayloadLen_8B				((uint16_t)(0<<12))
#define USART_PayloadLen_9B				((uint16_t)(1<<12))

//@ref USART_Parity_define
#define USART_Parity_NONE				((uint16_t)(0<<10))
#define USART_Parity_EVEN				((uint16_t)(1<<10))
#define USART_Parity_ODD				((uint16_t)(1<<10 | 1<<9))

//@ref USART_StopBits_define
#define USART_StopBits__1				((uint16_t)(0<<12))
#define USART_StopBits__Half			((uint16_t)(1<<12))
#define USART_StopBits__2				((uint16_t)(2<<12))
#define USART_StopBits__1_half			((uint16_t)(3<<12))

//@ref USART_HwFlowCtl_define
#define USART_HwFlowCtl_NONE			((uint16_t)(0))
#define USART_HwFlowCtl_RTS				((uint16_t)(1<<8))
#define USART_HwFlowCtl_CTS				((uint16_t)(1<<9))
#define USART_HwFlowCtl_RTS_CTS			((uint16_t)(1<<9 | 1<<8))

//@ref USART_IRQ_define
#define USART_IRQ_NONE					((uint16_t)(0))
#define USART_IRQ_TXE					((uint16_t)(1<<7))
#define USART_IRQ_TC					((uint16_t)(1<<6))
#define USART_IRQ_RXNE					((uint16_t)(1<<5))
#define USART_IRQ_PE					((uint16_t)(1<<8))

enum Polling_mechanism
{
	enable,
	disable
};

//BaudRate Calculation
//USARTDIV = fclk / (16 * Baudrate)
//USARTDIV_MUL100 =
// (uint32)(100 * fclk ) / (16 * Baudrate) == (25 * fclk) / (4 * Baudrate)
//DIV_Mantissa_MUL100 = Integer Part (USARTDIV ) * 100
//DIV_Mantissa = Integer Part (USARTDIV )
//DIV_Fraction = (( USARTDIV_MUL100 - DIV_Mantissa_MUL100)* 16 ) / 100

#define USARTDIV(_PCLK_, _BAUD_)			((uint32_t)(_PCLK_/(16 * _BAUD_)))
#define USARTDIV_MUL100(_PCLK_, _BAUD_)		((uint32_t)((25 * _PCLK_) / (4 * _BAUD_)))
#define Mantissa_MUL100(_PCLK_, _BAUD_)		((uint32_t)(USARTDIV(_PCLK_, _BAUD_) * 100))
#define Mantissa(_PCLK_, _BAUD_)			((uint32_t)(USARTDIV(_PCLK_, _BAUD_)))
#define DIV_Fraction(_PCLK_, _BAUD_)		((uint32_t)((((USARTDIV_MUL100(_PCLK_, _BAUD_)) - (Mantissa_MUL100(_PCLK_, _BAUD_))) * 16) / 100) )
#define USART_BRR_REGISTER(_PCLK_, _BAUD_)	( (Mantissa(_PCLK_, _BAUD_) << 4 ) | (DIV_Fraction(_PCLK_, _BAUD_) & 0xF) )

/*
 * =====================================================================================
 * 							APIS Supported by "MCAL USART driver"
 * =====================================================================================
 */
void MCAL_USART_Init(USART_TypeDef* USARTx, USART_Config_t* USART_Config);
void MCAL_USART_DeInit(USART_TypeDef* USARTx);

void MCAL_USART_GPIO_Set_Pins(USART_TypeDef* USARTx);

void MCAL_USART_SendData(USART_TypeDef* USARTx, uint16_t* pTxBuffer, enum Polling_mechanism PollingEn);
void MCAL_USART_ReceiveData(USART_TypeDef* USARTx, uint16_t* pRxBuffer, enum Polling_mechanism PollingEn);

void MCAL_USART_Wait_Tc(USART_TypeDef* USARTx);

#endif /* INC_STM32_F103C8_USART_DRIVER_H_ */
