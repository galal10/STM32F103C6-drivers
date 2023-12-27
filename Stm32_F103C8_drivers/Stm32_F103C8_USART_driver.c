/*
 * Stm32_F103C8_USART_driver.c
 *
 *  Created on: Nov 3, 2023
 *      Author: galal
 */

#include "Stm32_F103C8_USART_driver.h"

/*
 * =====================================================================================
 * 							Generic Variables
 * =====================================================================================
 */
static USART_Config_t Global_USART_Config[3];


/*
 * =====================================================================================
 * 							APIS Supported by "MCAL USART driver"
 * =====================================================================================
 */


/*=====================================================================
 * @Fn				- MCAL_USART_Init
 * @brief 			- initializes USART registers (Supported Feature ASYNCH only)
 * @param [in] 		- USARTx: where x can be (1..3 depending on device used) to select the USART peripheral
 * @retval			- none
 * Note				- Support for now ASYNCH mode & 8 MHZ
 */
void MCAL_USART_Init(USART_TypeDef* USARTx, USART_Config_t* USART_Config)
{
	uint32_t pclk, BRR;

	//Enable Clock for USART peripheral
	if(USARTx == USART1)
	{
		RCC_USART1_CLK_EN();
		Global_USART_Config[0] = *USART_Config;
	}

	else if(USARTx == USART2)
	{
		RCC_USART2_CLK_EN();
		Global_USART_Config[1] = *USART_Config;
	}

	else if(USARTx == USART3)
	{
		RCC_USART3_CLK_EN();
		Global_USART_Config[2] = *USART_Config;
	}

	//Enable the USART by writing the UE bit in USART_CR1 register to 1
	USARTx->CR1 |= 1 << 13;

	//Enable USART Tx/Rx engines according to USART_Mode Configurations
	// USART_CR1 bit3: TE & bit2: RE
	USARTx->CR1 |= USART_Config->USART_Mode;

	//define the word length by M bit12 in USART_CR1
	USARTx->CR1 |= USART_Config->PayloadLen;

	//Select the parity mode by Bit 10 PCE, Bit 9 PS in USART_CR1
	USARTx->CR1 |= USART_Config->Parity;

	//Select number of stop bits12,13 in USART_CR2.
	USARTx->CR2 |= USART_Config->StopBits;

	//USART Hardware FlowControl
	//USART_CR3 Bit 9 CTSE: CTS enable, Bit 8 : RRTS enable
	USARTx->CR3 |= USART_Config->HwFlowCtl;

	//Configurations of BRR(Baud Rate Register)
	//PCLK1 for USART2, 3
	//PCLK2 for USART1
	if(USARTx == USART1)
	{
		pclk = MCAL_RCC_GetPCLK2Freq();
	}

	else
	{
		pclk = MCAL_RCC_GetPCLK1Freq();
	}

	BRR = USART_BRR_REGISTER(pclk, USART_Config->BaudRate);
	//Bits 15:4 DIV_Mantissa[11:0]: mantissa of USARTDIV
	//Bits 3:0 DIV_Fraction[3:0]: fraction of USARTDIV
	USARTx->BRR = BRR;

	//Enable or Disable Interrupt
	//CR1
	if(USART_Config->IRQ_Enable != USART_IRQ_NONE)
	{
		USARTx->CR1 |= USART_Config->IRQ_Enable;

		//Enable NVIC Interrupt
		if(USARTx == USART1)
		{
			NVIC_IRQ37_USART1_Enable();
		}
		else if(USARTx == USART2)
		{
			NVIC_IRQ38_USART2_Enable();
		}
		else if(USARTx == USART3)
		{
			NVIC_IRQ39_USART3_Enable();
		}
	}

}

/*=====================================================================
 * @Fn				- MCAL_USART_DeInit
 * @brief 			- DEinitializes USART registers (Supported Feature ASYNCH only)
 * @param [in] 		- USARTx: where x can be (1..3 depending on device used) to select the USART peripheral
 * @retval			- none
 * Note				- Reset the model by RCC
 */
void MCAL_USART_DeInit(USART_TypeDef* USARTx)
{
	if(USARTx == USART1)
	{
		//RESET CLK Set and cleared by software.
		RCC_USART1_CLK_RESET();
		RCC_USART1_CLK_DERESET();
		//Disable NVIC Interrupt
		NVIC_IRQ37_USART1_Disable();
	}
	else if(USARTx == USART2)
	{
		//RESET CLK Set and cleared by software.
		RCC_USART2_CLK_RESET();
		RCC_USART2_CLK_DERESET();
		//Disable NVIC Interrupt
		NVIC_IRQ38_USART2_Disable();
	}
	else if(USARTx == USART3)
	{
		//RESET CLK Set and cleared by software.
		RCC_USART3_CLK_RESET();
		RCC_USART3_CLK_DERESET();
		//Disable NVIC Interrupt
		NVIC_IRQ39_USART3_Disable();
	}

}

/*=====================================================================
 * @Fn				- MCAL_USART_GPIO_Set_Pins
 * @brief 			- Send Buffer on USART
 * @param [in] 		- USARTx: where x can be (1..3 depending on device used) to select the USART peripheral
 * @retval			- none
 * Note				- Should enable the corresponding ALT & GPIO in RCC CLK after MCAL_USART_Init()
 */
void MCAL_USART_GPIO_Set_Pins(USART_TypeDef* USARTx)
{
	//USARTx_TX : Full duplex : Alternate function push-pull
	//USARTx_RX : Full duplex : Input floating / Input pull-up
	//USARTx_RTS: HwFlowCtl   : Alternate function push-pull
	//USARTx_CTS: HwFlowCtl   : Input floating/ Input pull-up

	GPIO_PinConfig_t PinCfg;
	if(USARTx == USART1)
	{
		//USART1_TX : PA9
		PinCfg.GPIO_PinNumber = GPIO_PIN_9;
		PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
		PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
		MCAL_GPIO_init(GPIOA, &PinCfg);
		//USART1_Rx : PA10
		PinCfg.GPIO_PinNumber = GPIO_PIN_10;
		PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
		MCAL_GPIO_init(GPIOA, &PinCfg);

		//USART1_RTS: PA12
		if( (Global_USART_Config[0].HwFlowCtl == USART_HwFlowCtl_RTS) || (Global_USART_Config[0].HwFlowCtl == USART_HwFlowCtl_RTS_CTS) )
		{
			PinCfg.GPIO_PinNumber = GPIO_PIN_12;
			PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
			MCAL_GPIO_init(GPIOA, &PinCfg);
		}

		//USART1_CTS: PA11
		if( (Global_USART_Config[0].HwFlowCtl == USART_HwFlowCtl_CTS) || (Global_USART_Config[0].HwFlowCtl == USART_HwFlowCtl_RTS_CTS) )
		{
			PinCfg.GPIO_PinNumber = GPIO_PIN_11;
			PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
			MCAL_GPIO_init(GPIOA, &PinCfg);
		}
	}

	else if(USARTx == USART2)
	{
		//USART2_TX : PA2
		PinCfg.GPIO_PinNumber = GPIO_PIN_2;
		PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
		PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
		MCAL_GPIO_init(GPIOA, &PinCfg);
		//USART2_Rx : PA3
		PinCfg.GPIO_PinNumber = GPIO_PIN_3;
		PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
		MCAL_GPIO_init(GPIOA, &PinCfg);

		//USART2_RTS: PA1
		if( (Global_USART_Config[1].HwFlowCtl == USART_HwFlowCtl_RTS) || (Global_USART_Config[1].HwFlowCtl == USART_HwFlowCtl_RTS_CTS) )
		{
			PinCfg.GPIO_PinNumber = GPIO_PIN_1;
			PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
			MCAL_GPIO_init(GPIOA, &PinCfg);
		}

		//USART2_CTS: PA0
		if( (Global_USART_Config[1].HwFlowCtl == USART_HwFlowCtl_CTS) || (Global_USART_Config[1].HwFlowCtl == USART_HwFlowCtl_RTS_CTS) )
		{
			PinCfg.GPIO_PinNumber = GPIO_PIN_0;
			PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
			MCAL_GPIO_init(GPIOA, &PinCfg);
		}
	}

	else if(USARTx == USART3)
	{
		//USART3_TX : PB10
		PinCfg.GPIO_PinNumber = GPIO_PIN_10;
		PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
		PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
		MCAL_GPIO_init(GPIOB, &PinCfg);
		//USART3_Rx : PB11
		PinCfg.GPIO_PinNumber = GPIO_PIN_11;
		PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
		MCAL_GPIO_init(GPIOB, &PinCfg);

		//USART3_RTS: PB14
		if( (Global_USART_Config[2].HwFlowCtl == USART_HwFlowCtl_RTS) || (Global_USART_Config[2].HwFlowCtl == USART_HwFlowCtl_RTS_CTS) )
		{
			PinCfg.GPIO_PinNumber = GPIO_PIN_14;
			PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
			MCAL_GPIO_init(GPIOB, &PinCfg);
		}

		//USART3_CTS: PB13
		if( (Global_USART_Config[2].HwFlowCtl == USART_HwFlowCtl_CTS) || (Global_USART_Config[2].HwFlowCtl == USART_HwFlowCtl_RTS_CTS) )
		{
			PinCfg.GPIO_PinNumber = GPIO_PIN_13;
			PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
			MCAL_GPIO_init(GPIOB, &PinCfg);
		}
	}
}

/*=====================================================================
 * @Fn				- MCAL_USART_SendData
 * @brief 			- Send Buffer on USART
 * @param [in] 		- USARTx: where x can be (1..3 depending on device used) to select the USART peripheral
 * @param [in] 		- pTxBuffer: Buffer that contains that data will be transmitted
 * @param [in] 		- Polling_mechanism: Enable Polling or Disable it
 * @retval			- none
 * Note				- Should init USART firstly
 */
void MCAL_USART_SendData(USART_TypeDef* USARTx, uint16_t* pTxBuffer, enum Polling_mechanism PollingEn)
{
	if(PollingEn == enable)
	{
		//Wait until TXE flag is set
		//Bit 7 TXE: Transmit data register empty
		//This bit is set by hardware when the content of the TDR register has been transferred into the shift register
		//It is cleared by a write to the USART_DR register.
		while(!(USARTx->SR & 1<<7));
	}


	//Check the contents of USART Payload length is 8B or 9B in a frame
	//When transmitting with the parity enabled (PCE bit set to 1 in the USART_CR1 register),
	//the value written in the MSB (bit 7 or bit 8 depending on the data length) has no effect
	//because it is replaced by the parity.

	// check if it USART1, USART2 or USART3
	if(USARTx == USART1)
	{
		if(Global_USART_Config[0].PayloadLen == USART_PayloadLen_9B)
		{
			USARTx->DR = (*pTxBuffer & 0x1FF);
		}
		else
		{
			USARTx->DR = (*pTxBuffer & (uint8_t)0xFF);
		}
	}

	else if(USARTx == USART2)
	{
		if(Global_USART_Config[1].PayloadLen == USART_PayloadLen_9B)
		{
			USARTx->DR = (*pTxBuffer & 0x1FF);
		}
		else
		{
			USARTx->DR = (*pTxBuffer & (uint8_t)0xFF);
		}
	}

	else if(USARTx == USART3)
	{
		if(Global_USART_Config[2].PayloadLen == USART_PayloadLen_9B)
		{
			USARTx->DR = (*pTxBuffer & 0x1FF);
		}
		else
		{
			USARTx->DR = (*pTxBuffer & (uint8_t)0xFF);
		}
	}
}

/*=====================================================================
 * @Fn				- MCAL_USART_Wait_Tc
 * @brief 			- Wait until The TC flag is Set
 * @param [in] 		- USARTx: where x can be (1..3 depending on device used) to select the USART peripheral
 * @retval			- none
 * Note				- none
 */
void MCAL_USART_Wait_Tc(USART_TypeDef* USARTx)
{
	//Bit 6 TC: Transmission complete
	//This bit is set by hardware if the transmission of a frame containing data is complete
	while(!(USARTx->SR & 1 << 6));
}

/*=====================================================================
 * @Fn				- MCAL_USART_ReceiveData
 * @brief 			- Receive Buffer from USART
 * @param [in] 		- USARTx: where x can be (1..3 depending on device used) to select the USART peripheral
 * @param [in] 		- pRxBuffer: Buffer that contains that data will be received
 * @param [in] 		- Polling_mechanism: Enable Polling or Disable it
 * @retval			- none
 * Note				- Should init USART firstly
 */
void MCAL_USART_ReceiveData(USART_TypeDef* USARTx, uint16_t* pRxBuffer, enum Polling_mechanism PollingEn)
{
	//Wait until
	//Bit 5 RXNE: Read data register not empty
	//This bit is set by hardware when the content of the RDR shift register has been transferred to the USART_DR register
	if(PollingEn == enable)
	{
		while(!(USARTx->SR & 1 << 5));
	}

	//Check the contents of USART Payload length is 8B or 9B in a frame
	//When receiving with the parity enabled, the value read in the MSB bit is the received parity bit.
	if(USARTx == USART1)
	{
		if(Global_USART_Config[0].PayloadLen == USART_PayloadLen_9B)
		{
			//No parity in a frame
			if(Global_USART_Config[0].Parity == USART_Parity_NONE)
			{
				//No parity ,so all 9bits are considered as data
				*((uint16_t*)pRxBuffer) = USARTx->DR;
			}
			//Parity in a frame
			else
			{
				//Parity is used, so, 8bits will be of user data and 1 bit is parity
				*((uint16_t*)pRxBuffer) = (USARTx->DR & (uint8_t)0xFF);
			}
		}
		//Payload length is 8B in a frame
		else
		{
			//No parity in a frame
			if(Global_USART_Config[0].Parity == USART_Parity_NONE)
			{
				//No parity ,so all 8bits are considered as data
				*((uint16_t*)pRxBuffer) = (USARTx->DR & (uint8_t)0xFF);
			}
			//Parity in a frame
			else
			{
				//Parity is used, so, 7bits will be of user data and 1 bit is parity
				*((uint16_t*)pRxBuffer) = (USARTx->DR & (uint8_t)0x7F);
			}
		}
	}

	else if(USARTx == USART2)
	{
		if(Global_USART_Config[1].PayloadLen == USART_PayloadLen_9B)
		{
			//No parity in a frame
			if(Global_USART_Config[1].Parity == USART_Parity_NONE)
			{
				//No parity ,so all 9bits are considered as data
				*((uint16_t*)pRxBuffer) = USARTx->DR;
			}
			//Parity in a frame
			else
			{
				//Parity is used, so, 8bits will be of user data and 1 bit is parity
				*((uint16_t*)pRxBuffer) = (USARTx->DR & (uint8_t)0xFF);
			}
		}
		//Payload length is 8B in a frame
		else
		{
			//No parity in a frame
			if(Global_USART_Config[1].Parity == USART_Parity_NONE)
			{
				//No parity ,so all 8bits are considered as data
				*((uint16_t*)pRxBuffer) = (USARTx->DR & (uint8_t)0xFF);
			}
			//Parity in a frame
			else
			{
				//Parity is used, so, 7bits will be of user data and 1 bit is parity
				*((uint16_t*)pRxBuffer) = (USARTx->DR & (uint8_t)0x7F);
			}
		}

	}

	else if(USARTx == USART3)
	{
		if(Global_USART_Config[2].PayloadLen == USART_PayloadLen_9B)
		{
			//No parity in a frame
			if(Global_USART_Config[2].Parity == USART_Parity_NONE)
			{
				//No parity ,so all 9bits are considered as data
				*((uint16_t*)pRxBuffer) = USARTx->DR;
			}
			//Parity in a frame
			else
			{
				//Parity is used, so, 8bits will be of user data and 1 bit is parity
				*((uint16_t*)pRxBuffer) = (USARTx->DR & (uint8_t)0xFF);
			}
		}
		//Payload length is 8B in a frame
		else
		{
			//No parity in a frame
			if(Global_USART_Config[2].Parity == USART_Parity_NONE)
			{
				//No parity ,so all 8bits are considered as data
				*((uint16_t*)pRxBuffer) = (USARTx->DR & (uint8_t)0xFF);
			}
			//Parity in a frame
			else
			{
				//Parity is used, so, 7bits will be of user data and 1 bit is parity
				*((uint16_t*)pRxBuffer) = (USARTx->DR & (uint8_t)0x7F);
			}
		}

	}

}

/*
 * =====================================================================================
 * 							ISR Functions
 * =====================================================================================
 */
void USART1_IRQHandler(void)
{
	Global_USART_Config[0].P_IRQ_CallBack();
}

void USART2_IRQHandler(void)
{
	Global_USART_Config[1].P_IRQ_CallBack();
}

void USART3_IRQHandler(void)
{
	Global_USART_Config[2].P_IRQ_CallBack();
}
