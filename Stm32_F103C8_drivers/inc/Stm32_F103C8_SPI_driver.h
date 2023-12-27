/*
 * Stm32_F103C8_SPI_driver.h
 *
 *  Created on: Nov 17, 2023
 *      Author: galal
 */

#ifndef INC_STM32_F103C8_SPI_DRIVER_H_
#define INC_STM32_F103C8_SPI_DRIVER_H_

#include "stm32f103x8.h"
#include "Stm32_F103C8_gpio_driver.h"
#include "Stm32_F103C8_RCC_driver.h"


struct S_IRQ_SRC
{
	uint8_t TXE:1;		//TX buffer empty interrupt
	uint8_t RXNE:1;		//RX buffer not empty interrupt
	uint8_t ERRI:1;		//Error interrupt
	uint8_t Reserved:5;
};


//Configuration structure
typedef struct
{
	uint8_t	SPI_Mode;				//Specifies the SPI mode
									// this parameter must be set based on @ref SPI_Mode_define

	uint16_t Communication_Mode;	//Specifies the SPI Bidirectional mode state
									// this parameter must be set based on @ref SPI_Communication_Mode_define

	uint8_t	Frame_Format;			//Specifies MSB transmitted first or LSB transmitted first
									// this parameter must be set based on @ref SPI_Frame_Format_define

	uint16_t Data_Size;				//Specifies the size of SPI frame if it's 8 or 16 bit
									// this parameter must be set based on @ref SPI_Data_Size_define

	uint8_t	CLK_Polarity;			//Specifies the SPI Bidirectional mode state
									// this parameter must be set based on @ref SPI_CLK_Polarity_define

	uint8_t	CLK_PHASE;				//Specifies the SPI Bidirectional mode state
									// this parameter must be set based on @ref SPI_CLK_PHASE_define

	uint16_t NSS;					//Specifies whether NSS signal is managed by H.W or S.W using SSI bit enable
									// this parameter must be set based on @ref SPI_NSS_define

	uint8_t	Baud_Rate_PreSCLR;		/* Specifies the Baud Rate prescaler value which will be
									used to configure the transmit and receive SCK clock.
									@note The communication clock is derived from the master
									clock. The slave clock does not need to be set.
									this parameter must be set based on @ref SPI_Baud_Rate_PreSCLR_define */

	uint8_t IRQ_Enable;				//Specifies whether Interrupt is will be Enabled or Disabled
									// this parameter must be set based on @ref SPI_IRQ_define

	void (* P_IRQ_CallBack)(struct S_IRQ_SRC irq_src);	//set the C Function() which will be called once the IRQ Happen
}SPI_Config_t;

//==================================================
//Macros Configurations References
//==================================================

//@ref SPI_Mode_define
#define SPI_Mode_Master			1<<2	//CR1.MSTR = 1
#define SPI_Mode_Slave			0<<2	//CR1.MSTR = 0

//@ref SPI_Communication_Mode_define
#define SPI_Communication_Mode_2LINES			0x0000U 			//CR1.BIDIMODE = 0 & CR1.RXONLY = 0
#define SPI_Communication_Mode_2LINES_RXONLY	1<<10				//CR1.BIDIMODE = 0 & CR1.RXONLY = 1
#define SPI_Communication_Mode_1LINE_RXONLY		1<<15				//CR1.BIDIMODE = 1 & CR1.BIDIOE = 0
#define SPI_Communication_Mode_1LINE_TXONLY	    (1<<15 | 1 <<14) 	//CR1.BIDIMODE = 1 & CR1.BIDIOE = 1


//@ref SPI_Frame_Format_define
#define SPI_Frame_Format_LSB	1<<7	//CR1.LSBFIRST = 1
#define SPI_Frame_Format_MSB	0x00U	//CR1.LSBFIRST = 0

//@ref SPI_Data_Size_define
#define SPI_Data_Size_8bit		0x0000U	//CR1.DFF = 0
#define SPI_Data_Size_16bit		1<<11	//CR1.DFF = 1

//@ref SPI_CLK_Polarity_define
#define SPI_CLK_Polarity_LOW_idle	0x00U //CR1.CPOL = 0
#define SPI_CLK_Polarity_HIGH_idle	1<<1  //CR1.CPOL = 1

//@ref SPI_CLK_PHASE_define
#define SPI_CLK_PHASE_1EDGE_Sampling	0x00U //CR1.CPHA = 0
#define SPI_CLK_PHASE_2EDGE_Sampling	1<<0  //CR1.CPHA = 1

//@ref SPI_NSS_define
/* H.W  Bit9 : SSM in CR1 is low by default */
#define SPI_NSS_HW_Slave					0x0000U					//CR1.SSM = 0
#define SPI_NSS_HW_Master_SS_output_Enable	1<<2					//CR2.SSOE = 1 & CR1.SSM = 0
#define SPI_NSS_HW_Master_SS_output_Disable	((uint8_t)(~(1<<2)))	//CR2.SSOE = 0 & CR1.SSM = 0

//S.W (Master or Slave)
#define SPI_NSS_SW_RESET	1<<9			//CR1.SSM = 0 & CR1.SSI = 0
#define SPI_NSS_SW_SET		(1<<9 | 1 <<8)	//CR1.SSM = 0 & CR1.SSI = 1


//@ref SPI_Baud_Rate_PreSCLR_define
#define SPI_Baud_Rate_PreSCLR_2		0x00U		//CR1.BR[2:0] = 000
#define SPI_Baud_Rate_PreSCLR_4		0b001<<3	//CR1.BR[2:0] = 001
#define SPI_Baud_Rate_PreSCLR_8		0b010<<3	//CR1.BR[2:0] = 010
#define SPI_Baud_Rate_PreSCLR_16	0b011<<3	//CR1.BR[2:0] = 011
#define SPI_Baud_Rate_PreSCLR_32	0b100<<3	//CR1.BR[2:0] = 100
#define SPI_Baud_Rate_PreSCLR_64	0b101<<3	//CR1.BR[2:0] = 101
#define SPI_Baud_Rate_PreSCLR_128	0b110<<3	//CR1.BR[2:0] = 110
#define SPI_Baud_Rate_PreSCLR_256	0b111<<3	//CR1.BR[2:0] = 111

//@ref SPI_IRQ_define
#define SPI_IRQ_NONE		0x00U
#define SPI_IRQ_TXEIE		1<<7	//CR2.TXEIE = 1
#define SPI_IRQ_RXNEIE		1<<6	//CR2.RXNEIE = 1
#define SPI_IRQ_ERRIE		1<<5	//CR2.ERRIE = 1


enum SPI_Polling_mechanism
{
	PollingEnable,
	PollingDisable
};
/*
 * =====================================================================================
 * 							APIS Supported by "MCAL SPI driver"
 * =====================================================================================
 */
void MCAL_SPI_Init(SPI_TypeDef* SPIx, SPI_Config_t* SPI_Config);
void MCAL_SPI_DeInit(SPI_TypeDef* SPIx);

void MCAL_SPI_GPIO_Set_Pins(SPI_TypeDef* SPIx);

void MCAL_SPI_SendData(SPI_TypeDef* SPIx, uint16_t* pTxBuffer, enum SPI_Polling_mechanism PollingEn);
void MCAL_SPI_ReceiveData(SPI_TypeDef* SPIx, uint16_t* pRxBuffer, enum SPI_Polling_mechanism PollingEn);

void MCAL_SPI_TxRx(SPI_TypeDef* SPIx, uint16_t* pBuffer, enum SPI_Polling_mechanism PollingEn);

#endif /* INC_STM32_F103C8_SPI_DRIVER_H_ */
