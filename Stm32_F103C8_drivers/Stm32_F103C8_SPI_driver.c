/*
 * Stm32_F103C8_SPI_driver.c
 *
 *  Created on: Nov 17, 2023
 *      Author: galal
 */


#include "Stm32_F103C8_SPI_driver.h"


/*
 * =====================================================================================
 * 							Generic Macros
 * =====================================================================================
 */
#define SPI_NUM		2
#define SPI1_INDEX	0
#define SPI2_INDEX	1

#define SPI_SR_TXE	0x02		// TXE: Transmit buffer empty
#define SPI_SR_RXNE	0x01		// RXNE: Receive buffer not empty
/*
 * =====================================================================================
 * 							Generic Variables
 * =====================================================================================
 */
static SPI_Config_t Global_SPI_Config[SPI_NUM];

/*
 * =====================================================================================
 * 							APIS Supported by "MCAL SPI driver"
 * =====================================================================================
 */

/*=====================================================================
 * @Fn				- MCAL_SPI_Init
 * @brief 			- initializes SPI registers According to specified parameters
 * @param [in] 		- USARTx: where x can be (1..2 depending on device used) to select the SPI peripheral
 * @param [in] 		- SPI_Config : All SPI Configurations
 * @retval			- none
 * Note				- Support for SPI Full Duplex Master/Slave only & NSS HW/SW
 * 					- in case of Master you have to configure pins and drive it
 */
void MCAL_SPI_Init(SPI_TypeDef* SPIx, SPI_Config_t* SPI_Config)
{
	//Safety for Registers
	uint16_t tmpReg_CR1 = 0;
	uint16_t tmpReg_CR2 = 0;

	if(SPIx == SPI1)
	{
		Global_SPI_Config[SPI1_INDEX] = *SPI_Config;
		RCC_SPI1_CLK_EN();
	}
	else if(SPIx == SPI2)
	{
		Global_SPI_Config[SPI2_INDEX] = *SPI_Config;
		RCC_SPI2_CLK_EN();
	}

	//Communication Mode
	tmpReg_CR1 |= SPI_Config->Communication_Mode;

	//Frame Format
	tmpReg_CR1 |= SPI_Config->Frame_Format;

	//Data Size
	tmpReg_CR1 |= SPI_Config->Data_Size;

	//Clock Polarity
	tmpReg_CR1 |= SPI_Config->CLK_Polarity;

	//Clock Phase
	tmpReg_CR1 |= SPI_Config->CLK_PHASE;

	/* ========================== NSS ========================== */
	if(SPI_Config->NSS == SPI_NSS_HW_Master_SS_output_Enable)
	{
		tmpReg_CR2 |= SPI_Config->NSS;
	}
	else if(SPI_Config->NSS == SPI_NSS_HW_Master_SS_output_Disable)
	{
		tmpReg_CR2 &= SPI_Config->NSS;
	}
	else
	{
		tmpReg_CR1 |= SPI_Config->NSS;
	}

	//Baud Rate only in Master mode
	if(SPI_Config->SPI_Mode == SPI_Mode_Master)
	{
		tmpReg_CR1 |= SPI_Config->Baud_Rate_PreSCLR;
	}

	//Mode of SPI(Master or Slave)
	tmpReg_CR1 |= SPI_Config->SPI_Mode;

	//Enable SPI
	tmpReg_CR1 |= 1 << 6;

	//IRQ
	if(SPI_Config->IRQ_Enable != SPI_IRQ_NONE)
	{
		tmpReg_CR2 |= SPI_Config->IRQ_Enable;

		if(SPIx == SPI1)
		{
			NVIC_IRQ35_SPI1_Enable();
		}
		else if(SPIx == SPI2)
		{
			NVIC_IRQ36_SPI2_Enable();
		}
	}

	SPIx->CR1 = tmpReg_CR1;
	SPIx->CR2 = tmpReg_CR2;
}

void MCAL_SPI_DeInit(SPI_TypeDef* SPIx)
{
	if(SPIx == SPI1)
	{
		//Disable NVIC Interrupt
		NVIC_IRQ35_SPI1_Disable();
		//RESET CLK Set and cleared by software.
		RCC_SPI1_CLK_RESET();
		RCC_SPI1_CLK_DERESET();
	}
	else if(SPIx == SPI2)
	{
		//Disable NVIC Interrupt
		NVIC_IRQ36_SPI2_Disable();
		//RESET CLK Set and cleared by software.
		RCC_SPI2_CLK_RESET();
		RCC_SPI2_CLK_DERESET();
	}
}

void MCAL_SPI_GPIO_Set_Pins(SPI_TypeDef* SPIx)
{
	GPIO_PinConfig_t PinCfg;

	if(SPIx == SPI1)
	{

		if(Global_SPI_Config[SPI1_INDEX].SPI_Mode == SPI_Mode_Master)
		{
			//NSS  : PA4
			switch(Global_SPI_Config[SPI1_INDEX].NSS)
			{
			//Hardware master output disabled : Input floating
			case SPI_NSS_HW_Master_SS_output_Disable:

				PinCfg.GPIO_PinNumber = GPIO_PIN_4;
				PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
				MCAL_GPIO_init(GPIOA, &PinCfg);

				break;

			//Hardware master/ NSS output enabled : Alternate function push-pull
			case SPI_NSS_HW_Master_SS_output_Enable:

				PinCfg.GPIO_PinNumber = GPIO_PIN_4;
				PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
				PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
				MCAL_GPIO_init(GPIOA, &PinCfg);

				break;

			//Software Not used.
			default:
				break;
			}

			//SCK  : PA5
			//Master Alternate function push-pull
			PinCfg.GPIO_PinNumber = GPIO_PIN_5;
			PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
			PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
			MCAL_GPIO_init(GPIOA, &PinCfg);

			//MISO : PA6 (Supported only Full Duplex)
			//Full duplex / master Input floating
			PinCfg.GPIO_PinNumber = GPIO_PIN_6;
			PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
			MCAL_GPIO_init(GPIOA, &PinCfg);
			//todo to support half duplex and simplex

			//MOSI : PA7 (Supported only Full Duplex)
			//Full duplex / master Alternate function push-pull
			PinCfg.GPIO_PinNumber = GPIO_PIN_7;
			PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
			PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
			MCAL_GPIO_init(GPIOA, &PinCfg);
			//todo to support half duplex and simplex

		}

		//Slave Mode
		else
		{
			//NSS  : PA4
			switch(Global_SPI_Config[SPI1_INDEX].NSS)
			{
			//Hardware slave : Input floating
			case SPI_NSS_HW_Slave:

				PinCfg.GPIO_PinNumber = GPIO_PIN_4;
				PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
				MCAL_GPIO_init(GPIOA, &PinCfg);

				break;

			//Software Not used.
			default:
				break;
			}

			//SCK  : PA5
			//Slave : Input floating
			PinCfg.GPIO_PinNumber = GPIO_PIN_5;
			PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
			MCAL_GPIO_init(GPIOA, &PinCfg);

			//MISO : PA6 (Supported only Full Duplex)
			//Full duplex / slave (point to point) : Alternate function push-pull
			PinCfg.GPIO_PinNumber = GPIO_PIN_6;
			PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
			PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
			MCAL_GPIO_init(GPIOA, &PinCfg);
			//todo to support Full duplex (multi-slave)

			//MOSI : PA7 (Supported only Full Duplex)
			//Full duplex / slave : Input floating
			PinCfg.GPIO_PinNumber = GPIO_PIN_7;
			PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
			MCAL_GPIO_init(GPIOA, &PinCfg);
			//todo to support half duplex and simplex
		}
	}

	else if(SPIx == SPI2)
	{

		if(Global_SPI_Config[SPI2_INDEX].SPI_Mode == SPI_Mode_Master)
		{
			//NSS  : PB12
			switch(Global_SPI_Config[SPI2_INDEX].NSS)
			{
			//Hardware master output disabled : Input floating
			case SPI_NSS_HW_Master_SS_output_Disable:

				PinCfg.GPIO_PinNumber = GPIO_PIN_12;
				PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
				MCAL_GPIO_init(GPIOB, &PinCfg);

				break;

			//Hardware master/ NSS output enabled : Alternate function push-pull
			case SPI_NSS_HW_Master_SS_output_Enable:

				PinCfg.GPIO_PinNumber = GPIO_PIN_12;
				PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
				PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
				MCAL_GPIO_init(GPIOB, &PinCfg);

				break;

			//Software Not used.
			default:
				break;
			}

			//SCK  : PB13
			//Master Alternate function push-pull
			PinCfg.GPIO_PinNumber = GPIO_PIN_13;
			PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
			PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
			MCAL_GPIO_init(GPIOB, &PinCfg);

			//MISO : PB14 (Supported only Full Duplex)
			//Full duplex / master Input floating
			PinCfg.GPIO_PinNumber = GPIO_PIN_14;
			PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
			MCAL_GPIO_init(GPIOB, &PinCfg);
			//todo to support half duplex and simplex

			//MOSI : PB15 (Supported only Full Duplex)
			//Full duplex / master Alternate function push-pull
			PinCfg.GPIO_PinNumber = GPIO_PIN_15;
			PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
			PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
			MCAL_GPIO_init(GPIOB, &PinCfg);
			//todo to support half duplex and simplex

		}

		//Slave Mode
		else
		{
			//NSS  : PB12
			switch(Global_SPI_Config[SPI2_INDEX].NSS)
			{
			//Hardware slave : Input floating
			case SPI_NSS_HW_Slave:

				PinCfg.GPIO_PinNumber = GPIO_PIN_12;
				PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
				MCAL_GPIO_init(GPIOB, &PinCfg);

				break;

			//Software Not used.
			default:
				break;
			}

			//SCK  : PB13
			//Slave : Input floating
			PinCfg.GPIO_PinNumber = GPIO_PIN_13;
			PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
			MCAL_GPIO_init(GPIOB, &PinCfg);

			//MISO : PB14 (Supported only Full Duplex)
			//Full duplex / slave (point to point) : Alternate function push-pull
			PinCfg.GPIO_PinNumber = GPIO_PIN_14;
			PinCfg.GPIO_mode = GPIO_OUTPUT_AF_PP_MODE;
			PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
			MCAL_GPIO_init(GPIOB, &PinCfg);
			//todo to support Full duplex (multi-slave)

			//MOSI : PB15 (Supported only Full Duplex)
			//Full duplex / slave : Input floating
			PinCfg.GPIO_PinNumber = GPIO_PIN_15;
			PinCfg.GPIO_mode = GPIO_INPUT_FLO_MODE;
			MCAL_GPIO_init(GPIOB, &PinCfg);
			//todo to support half duplex and simplex
		}

	}
}

void MCAL_SPI_SendData(SPI_TypeDef* SPIx, uint16_t* pTxBuffer, enum SPI_Polling_mechanism PollingEn)
{
	if(PollingEn == PollingEnable)
	{
		//Wait until TXE flag is set
		while(!(SPIx->SR & SPI_SR_TXE));
	}

	SPIx->DR = *pTxBuffer;
}

void MCAL_SPI_ReceiveData(SPI_TypeDef* SPIx, uint16_t* pRxBuffer, enum SPI_Polling_mechanism PollingEn)
{
	if(PollingEn == PollingEnable)
	{
		//Wait until RXNE flag is set
		//Bit 0 RXNE: Receive Buffer not empty
		while(!(SPIx->SR & SPI_SR_RXNE));
	}

	*pRxBuffer = SPIx->DR;
}

void MCAL_SPI_TxRx(SPI_TypeDef* SPIx, uint16_t* pBuffer, enum SPI_Polling_mechanism PollingEn)
{
	MCAL_SPI_SendData(SPIx, pBuffer, PollingEn);

	MCAL_SPI_ReceiveData(SPIx, pBuffer, PollingEn);
}

/*
 * =====================================================================================
 * 										IRQ
 * =====================================================================================
 */

void SPI1_IRQHandler(void)
{
	struct S_IRQ_SRC irq_src;
	irq_src.TXE =  ( (SPI1->SR & (1<<1) ) >> 1);
	irq_src.RXNE = ( (SPI1->SR & (1<<0) ) >> 0);
	irq_src.ERRI = ( (SPI1->SR & (1<<4) ) >> 4);

	Global_SPI_Config[SPI1_INDEX].P_IRQ_CallBack(irq_src);
}

void SPI2_IRQHandler(void)
{
	struct S_IRQ_SRC irq_src;
	irq_src.TXE =  ( (SPI2->SR & (1<<1) ) >> 1);
	irq_src.RXNE = ( (SPI2->SR & (1<<0) ) >> 0);
	irq_src.ERRI = ( (SPI2->SR & (1<<4) ) >> 4);

	Global_SPI_Config[SPI2_INDEX].P_IRQ_CallBack(irq_src);
}
