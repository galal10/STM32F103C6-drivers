/*
 * Stm32_F103C8_I2C_driver.c
 *
 *  Created on: Nov 26, 2023
 *      Author: galal
 */


#include "Stm32_F103C8_I2C_driver.h"

/*
 * =====================================================================================
 * 							Generic Macros
 * =====================================================================================
 */
#define I2C1_INDEX		0
#define I2C2_INDEX		1

/*
 * =====================================================================================
 * 							Generic Variables
 * =====================================================================================
 */
static I2C_Config_t Global_I2C_Config[2] = {0};

/*
 * =====================================================================================
 * 							APIS Supported by "MCAL SPI driver"
 * =====================================================================================
 */

/*=====================================================================
 * @Fn				- MCAL_I2C_Init
 * @brief 			- initializes I2C registers According to specified parameters
 * @param [in] 		- I2Cx: where x can be (1..2 depending on device used) to select the I2C peripheral
 * @param [in] 		- I2C_Config : All I2C Configurations
 * @retval			- none
 * Note				- Support for SPI Full Duplex Master/Slave only & NSS HW/SW
 * 					- in case of Master you have to configure pins and drive it
 */

void MCAL_I2C_Init(I2C_TypeDef* I2Cx, I2C_Config_t* I2C_Config)
{
	uint16_t Freq = 0, tmpAddr = 0;
	uint32_t pclk1 = 8000000;

	//Enable RCC
	if(I2Cx == I2C1)
	{
		RCC_I2C1_CLK_EN();
		Global_I2C_Config[I2C1_INDEX] = *I2C_Config;
	}
	else
	{
		RCC_I2C2_CLK_EN();
		Global_I2C_Config[I2C2_INDEX] = *I2C_Config;
	}

	//Disable Peripheral to configure time
	I2Cx->CR1 &= ~I2C_CR1_PE_Msk;

	//I2C mode or SMBUS
	if(I2C_Config->I2C_Mode == I2C_Mode_I2C)
	{
		/* Init Timing */
		//1- I2C_CR2.FREQ[5:0]: Peripheral clock frequency
		//CLR frequency bits FREQ[5:0}
		I2Cx->CR2 &= ~I2C_CR2_FREQ_Msk;
		pclk1 = MCAL_RCC_GetPCLK1Freq();
		Freq = (uint16_t)(pclk1 / 1000000);

		I2Cx->CR2 |= Freq;

		//2- Configure the clock control registers SM/FM
		//CCR[11:0]: Clock control register
		//Thigh = Ti2c/2 = CCR * TPCLK1
		//CCR = TI2C / (2 * TPCLK1) = FPCLK1 / (2 * FI2C)

		//Configure Speed in Standard mode
		if((I2C_Config->I2C_ClockSpeed == I2C_SCLK_SM_50K) || (I2C_Config->I2C_ClockSpeed == I2C_SCLK_SM_100K))
		{
			I2Cx->CCR &= ~(0xFFF << 0);
			I2Cx->CCR |= (uint16_t)(pclk1 / (I2C_Config->I2C_ClockSpeed << 1));

			//3- Configure the rise time register
			//TRISE[5:0] = (maximum allowed SCL rise time "1000ns"/  TPCLK1) + 1
			I2Cx->TRISE = Freq + 1;
		}
		else
		{
			//Fast mode not supported
		}

		/*    I2C CR1 Configurations   */
		I2Cx->CR1 &= ((~(I2C_CR1_SMBUS_Msk)) & (~(I2C_CR1_ACK_Msk)) & (~(I2C_CR1_NOSTRETCH_Msk)) & (~(I2C_CR1_ENGC_Msk)));
		I2Cx->CR1 |= I2C_Config->I2C_Mode | I2C_Config->StretchMode | I2C_Config->ENGC | I2C_Config->I2C_ACK_Control;

		/*    I2C OAR1 & OAR2 Configurations   */
		if(I2C_Config->I2C_Slave_Address.Enable_Dual_ADD == 1)
		{
			I2Cx->OAR2 = (I2C_Config->I2C_Slave_Address.Secondary_Slave_ADD << I2C_OAR2_ADD2_Pos) | I2C_OAR2_ENDUAL;
		}

		//mode of Addressing 10B or 7B
		tmpAddr = I2C_Config->I2C_Slave_Address.I2C_Addressing_SLave_Mode;

		//check if it 10 address mode
		if(I2C_Config->I2C_Slave_Address.I2C_Addressing_SLave_Mode == I2C_Addressing_SLave_Mode_10B)
		{
			tmpAddr |= I2C_Config->I2C_Slave_Address.Primary_Slave_ADD;
		}
		else //7B address mode
		{
			tmpAddr |= I2C_Config->I2C_Slave_Address.Primary_Slave_ADD << 1;
		}
		//Write on OAR1 Register
		I2Cx->OAR1 = tmpAddr;

	}
	else
	{
		//SMBUS not supported
	}

	//Interrupt Mode(Slave Mode) //Check CallBack not equal NULL
	if(I2C_Config->P_Slave_Event_CallBack != NULL) //That Enable Slave IRQ states mode
	{
		I2Cx->CR2 |= I2C_CR2_ITERREN;
		I2Cx->CR2 |= I2C_CR2_ITEVTEN;
		I2Cx->CR2 |= I2C_CR2_ITBUFEN;

		if(I2Cx == I2C1)
		{
			NVIC_IRQ31_I2C1_EV_Enable();
			NVIC_IRQ32_I2C1_ER_Enable();
		}
		else if(I2Cx == I2C2)
		{
			NVIC_IRQ33_I2C2_EV_Enable();
			NVIC_IRQ34_I2C2_ER_Enable();
		}

		I2Cx->SR1 = 0;
		I2Cx->SR2 = 0;
	}


	/* Enable I2C Peripheral */
	I2Cx->CR1 |= I2C_CR1_PE;
}

void MCAL_I2C_DeInit(I2C_TypeDef* I2Cx)
{
	if(I2Cx == I2C1)
	{
		NVIC_IRQ31_I2C1_EV_Disable();
		NVIC_IRQ32_I2C1_ER_Disable();
		RCC_I2C1_CLK_RESET();
		RCC_I2C1_CLK_DERESET();
	}
	else if(I2Cx == I2C2)
	{
		NVIC_IRQ33_I2C2_EV_Disable();
		NVIC_IRQ34_I2C2_ER_Disable();
		RCC_I2C2_CLK_RESET();
		RCC_I2C2_CLK_DERESET();
	}
}

void MCAL_I2C_GPIO_Set_Pins(I2C_TypeDef* I2Cx)
{
	GPIO_PinConfig_t PinCfg;
	if(I2Cx == I2C1)
	{
		//I2C1_SCL: PB6
		PinCfg.GPIO_PinNumber = GPIO_PIN_6;
		PinCfg.GPIO_mode = GPIO_OUTPUT_AF_OD_MODE;
		PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
		MCAL_GPIO_init(GPIOB, &PinCfg);

		//I2C1_SDA: PB7
		PinCfg.GPIO_PinNumber = GPIO_PIN_7;
		PinCfg.GPIO_mode = GPIO_OUTPUT_AF_OD_MODE;
		PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
		MCAL_GPIO_init(GPIOB, &PinCfg);
	}
	else if(I2Cx == I2C2)
	{
		//I2C2_SCL: PB10
		PinCfg.GPIO_PinNumber = GPIO_PIN_10;
		PinCfg.GPIO_mode = GPIO_OUTPUT_AF_OD_MODE;
		PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
		MCAL_GPIO_init(GPIOB, &PinCfg);

		//I2C2_SDA: PB11
		PinCfg.GPIO_PinNumber = GPIO_PIN_11;
		PinCfg.GPIO_mode = GPIO_OUTPUT_AF_OD_MODE;
		PinCfg.GPIO_OutputSpeed = GPIO_SPEED_10M;
		MCAL_GPIO_init(GPIOB, &PinCfg);
	}
}

void MCAL_I2C_Master_Tx(I2C_TypeDef* I2Cx, uint16_t devAddr, uint8_t* dataOut, uint32_t dataLen, Stop_Condition Stop, Start_State start)
{
	uint32_t dataIndex;
	//todo
	//support timeout(configure timer working for specific duration rise interrupt)
	//timer_isr(){flag = 1}
	//so any code exist in while ( check flag || and polling conditions )

	// 1- Set start bit in I2C.CR1 to generate a start condition
	I2C_GenerateStart(I2Cx, Enable, start);

	// 2- wait for EV5
	//SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	while(!(I2C_GetFlagStatus(I2Cx, EV5)));

	// 3- Send address
	I2C_SendAddress(I2Cx, devAddr, Transmitter);

	// 4- wait for EV6
	//ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while(!(I2C_GetFlagStatus(I2Cx, EV6)));

	// 5- check if (I am Master) && (Busy[that you master the Bus]) && (Transmit empty) && (Data Transmit or Data Reception)
	// TRA, BUSY, MSL, TXE flags
	while(!(I2C_GetFlagStatus(I2Cx, MASTER_BYTE_TRANSMITTING)));

	// 6- send the data
	for(dataIndex = 0; dataIndex < dataLen; dataIndex++)
	{
		/* Writing the data in Data Register to be sent */
		I2Cx->DR = dataOut[dataIndex];

		// 7- wait for EV8
		//TxE=1, shift register not empty, data register empty, cleared by writing DR register
		while(!(I2C_GetFlagStatus(I2Cx, EV8)));
	}

	// 8- wait for EV8_2
	// TxE=1, BTF = 1, Program Stop request. TxE and BTF are cleared by hardware by the Stop condition
	while(!(I2C_GetFlagStatus(I2Cx, EV8_2)));

	if(Stop == With_Stop)
	{
		// 9- Send Stop condition
		I2C_GenerateStop(I2Cx, Enable);
	}

}

void MCAL_I2C_Master_Rx(I2C_TypeDef* I2Cx, uint16_t devAddr, uint8_t* Buffer, uint32_t dataLen, Stop_Condition stop, Start_State start)
{
	uint8_t index = I2Cx == I2C1? I2C1_INDEX : I2C2_INDEX;

	uint32_t dataIndex;
	//todo
	//support timeout(configure timer working for specific duration rise interrupt)
	//timer_isr(){flag = 1}
	//so any code exist in while ( check flag || and polling conditions )

	// 1- Set start bit in I2C.CR1 to generate a start condition
	I2C_GenerateStart(I2Cx, Enable, start);

	// 2- wait for EV5
	//SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	while(!(I2C_GetFlagStatus(I2Cx, EV5)));

	// 3- Send address
	I2C_SendAddress(I2Cx, devAddr, Receiver);

	// 4- wait for EV6
	//ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while(!(I2C_GetFlagStatus(I2Cx, EV6)));

	//Note : By default the ACK is Set
	//also Set the ACK for Sure
	I2C_ACK_Config(I2Cx, Enable);

	//Check if there is a length or not
	if(dataLen)
	{
		for(dataIndex = dataLen; dataIndex > 0; dataIndex--)
		{
			// 5- wait for EV7
			//RXNE = 1 cleared by reading DR Register
			while(!(I2C_GetFlagStatus(I2Cx, EV7)));
			/* Read DR Register */
			*Buffer = I2Cx->DR;
			/* increment the Address */
			Buffer++;
		}

		/* Disable ACK after the data is finished */
		I2C_ACK_Config(I2Cx, Disable);
	}

	if(stop == With_Stop)
	{
		// - Send Stop condition
		I2C_GenerateStop(I2Cx, Enable);
	}

	//Re-enable the default the ACK is Set
	if(Global_I2C_Config[index].I2C_ACK_Control == I2C_ACK_Control_Enable)
	{
		I2C_ACK_Config(I2Cx, Enable);
	}
}

void MCAL_I2C_SlaveSendData(I2C_TypeDef* I2Cx, uint8_t data)
{
	I2Cx->DR = data;
}

uint8_t MCAL_I2C_SlaveReceiveData(I2C_TypeDef* I2Cx)
{
	return (uint8_t)I2Cx->DR;
}

void I2C_GenerateStart(I2C_TypeDef* I2Cx, FunctionalState NewState, Start_State start)
{
	if(start != Repeated_Start)
	{
		//Check if the Bus is Busy or not
		while( I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) );
	}
	//Start condition
//	Bit 8 START: Start generation
//	This bit is set and cleared by software and cleared by hardware when start is sent or PE=0.
//	In Master Mode:
//	0: No Start generation
//	1: Repeated start generation
//	In Slave mode:
//	0: No Start generation
//	1: Start generation when the bus is free
	if(NewState == Enable)
	{
		//Generate start condition
		I2Cx->CR1 |= I2C_CR1_START;
	}
	else
	{
		//Disable start condition generation
		I2Cx->CR1 &= ~(I2C_CR1_START_Msk);
	}
}

void I2C_GenerateStop(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
//Bit 9 STOP: Stop generation
//The bit is set and cleared by software, cleared by hardware when a Stop condition is
//detected, set by hardware when a timeout error is detected.
//In Master Mode:
//0: No Stop generation.
//1: Stop generation after the current byte transfer or after the current Start condition is sent.
//In Slave mode:
//0: No Stop generation.
//1: Release the SCL and SDA lines after the current byte transfer
	if(NewState == Enable)
	{
		I2Cx->CR1 |= I2C_CR1_STOP;
	}
	else
	{
		I2Cx->CR1 &= ~I2C_CR1_STOP_Msk;
	}

}

void I2C_ACK_Config(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
	if(NewState == Enable)
	{
		I2Cx->CR1 |= I2C_CR1_ACK;
	}
	else
	{
		I2Cx->CR1 &= ~I2C_CR1_ACK_Msk;
	}
}

//Supported only 7B mode, 10B addressing mode is not supported
void I2C_SendAddress(I2C_TypeDef* I2Cx, uint16_t Address, I2C_Direction Direction)
{
	//Reset the bit 0 for write
	//Set the bit 0 for Read
	Address = ((Address << 1) | (Direction));

	/* Write in Data Register */
	I2Cx->DR = Address;
}

FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, Status flag)
{
	uint32_t flag1 = 0, flag2 = 0, lastEvent = 0;
	volatile uint32_t dummyRead;
	FlagStatus bitStatus = Reset;

	switch(flag)
	{
	case I2C_FLAG_BUSY:
//		Bit 1 BUSY: Bus busy
//		0: No communication on the bus
//		1: Communication ongoing on the bus
//		– Set by hardware on detection of SDA or SCL low
//		– cleared by hardware on detection of a Stop condition.
//		It indicates a communication in progress on the bus. This information is still updated when
//		the interface is disabled (PE=0).
		if(I2Cx->SR2 & I2C_SR2_BUSY_Msk)
			bitStatus = Set;
		break;

	case EV5:
//		Bit 0 SB: Start bit (Master mode)
//		0: No Start condition
//		1: Start condition generated.
//		– Set when a Start condition generated.
//		– Cleared by software by reading the SR1 register followed by writing the DR register, or by
//		hardware when PE=0
		if(I2Cx->SR1 & I2C_SR1_SB_Msk)
			bitStatus = Set;
		break;

	case EV6:
//		Bit 1 ADDR: Address sent (master mode)/matched (slave mode)
//		This bit is cleared by software reading SR1 register followed reading SR2, or by hardware
//		when PE=0.
//		Address matched (Slave)
//		0: Address mismatched or not received.
//		1: Received address matched.
//		– Set by hardware as soon as the received slave address matched with the OAR registers
//		content or a general call or a SMBus Device Default Address or SMBus Host or SMBus Alert
//		is recognized. (when enabled depending on configuration).
//		Note: In slave mode, it is recommended to perform the complete clearing sequence (READ
//		SR1 then READ SR2) after ADDR is set. Refer to Figure 272: Transfer sequence
//		diagram for slave receiver on page 755.
//		Address sent (Master)
//		0: No end of address transmission
//		1: End of address transmission
//		– For 10-bit addressing, the bit is set after the ACK of the 2nd byte.
//		– For 7-bit addressing, the bit is set after the ACK of the byte.
//		Note: ADDR is not set after a NACK reception
		if(I2Cx->SR1 & I2C_SR1_ADDR_Msk)
			bitStatus = Set;

		//to clear ADDR field cleared by reading SR2 Register
		dummyRead = I2Cx->SR2;
		break;

	case EV8:
//		Bit 7 TxE: Data register empty (transmitters)
//		0: Data register not empty
//		1: Data register empty
//		– Set when DR is empty in transmission. TxE is not set during address phase.
//		– Cleared by software writing to the DR register or by hardware after a start or a stop condition
//		or when PE=0.
//		TxE is not set if either a NACK is received, or if next byte to be transmitted is PEC (PEC=1)
//		Note: TxE is not cleared by writing the first data being transmitted, or by writing data when
//		BTF is set, as in both cases the data register is still empty.
		if(I2Cx->SR1 & I2C_SR1_TXE_Msk)
			bitStatus = Set;
		break;

	case EV8_2:
//		TxE=1, BTF = 1, Program Stop request. TxE and BTF are cleared by hardware by the Stop condition
		if((I2Cx->SR1 & EV8_2) == EV8_2 )
			bitStatus = Set;

		break;


	case EV7:
		//RXNE = 1 cleared by reading DR Register
//		Bit 6 RxNE: Data register not empty (receivers)
//		0: Data register empty
//		1: Data register not empty
//		– Set when data register is not empty in receiver mode. RxNE is not set during address phase.
//		– Cleared by software reading or writing the DR register or by hardware when PE=0.
//		RxNE is not set in case of ARLO event.
//		Note: RxNE is not cleared by reading data when BTF is set, as the data register is still full
		if(I2Cx->SR1 & I2C_SR1_RXNE_Msk)
			bitStatus = Set;

		break;

	case MASTER_BYTE_TRANSMITTING:
		/* read I2Cx status Registers */
		flag1 = I2Cx->SR1;
		flag2 = I2Cx->SR2;
		flag2 = flag2 << 16;

		lastEvent = (flag1 | flag2) & ((uint32_t)0x00FFFFFF);

		if((lastEvent & flag) == flag)
			bitStatus = Set;
		break;


	}


	return bitStatus;
}

void Slave_States(I2C_TypeDef* I2Cx, Slave_State state)
{
	uint8_t index = I2Cx == I2C1? I2C1_INDEX : I2C2_INDEX;

	switch(state)
	{
	case I2C_ERROR_AF:
		//make sure that the slave is really in transmitter mode
		if(I2Cx->SR2 & (I2C_SR2_TRA))
		{
			//Slave shouldn't send anything else
		}
		break;

	case I2C_EV_STOP:
		//make sure that the slave is really in transmitter mode
		if(I2Cx->SR2 & (I2C_SR2_TRA))
		{
			//Notify APP that the Stop Condition is Sent by the master
			Global_I2C_Config[index].P_Slave_Event_CallBack(I2C_EV_STOP);
		}
		break;

	case I2C_EV_ADDR_Matched:
		//Notify APP that The address is matched with The slave address
		Global_I2C_Config[index].P_Slave_Event_CallBack(I2C_EV_ADDR_Matched);
		break;

	case I2C_EV_DATA_REQ:
		//make sure that the slave is really in transmitter mode
		if(I2Cx->SR2 & (I2C_SR2_TRA))
		{
			//The APP Layer should send the data (MCAL_I2C_SlaveSendData)in this state
			Global_I2C_Config[index].P_Slave_Event_CallBack(I2C_EV_DATA_REQ);
		}

		break;

	case I2C_EV_DATA_RCV:
		//make sure that the slave is really in transmitter mode
		if(!(I2Cx->SR2 & (I2C_SR2_TRA)))
		{
			//The APP Layer should send the data (MCAL_I2C_SlaveReceiveData)in this state
			Global_I2C_Config[index].P_Slave_Event_CallBack(I2C_EV_DATA_RCV);
		}
		break;

	default :
		break;
	}
}

/*
 * =====================================================================================
 * 										IRQ
 * =====================================================================================
 */

void I2C1_EV_IRQHandler(void)
{
	volatile uint32_t dummyRead = 0;
	I2C_TypeDef* I2Cx = I2C1;
	//Interrupt handling for both master and slave mode of a device
	uint32_t temp1, temp2, temp3;

	temp1 = ( (I2Cx->CR2) & (I2C_CR2_ITEVTEN) );
	temp2 = ( (I2Cx->CR2) & (I2C_CR2_ITBUFEN) );
	temp3 = ( (I2Cx->SR1) & (I2C_SR1_STOPF) );

	//Handle for interrupt generated by STOPF event
	//Note : stop detection flag is applicable only slave mode
	if(temp1 && temp3)
	{
		//STOPF is set
		//clear the STOPF by reading SR1 register followed by writing to CR1 register
		I2Cx->CR1 |= 0x0000;
		Slave_States(I2Cx, I2C_EV_STOP);
	}

	temp3 = ( (I2Cx->SR1) & (I2C_SR1_ADDR) );
	//Handle For Interrupt generated by ADDR event
	//Note :When master mode : Address is sent
	//When slave mode  : Address is matched with own address
	if(temp1 && temp3)
	{
		// Interrupt is generated because of ADDR event
		// check for device mode
		if(I2Cx->SR2 & (I2C_SR2_MSL))
		{
			//Master mode
		}
		else
		{
			//Slave
			//Clear the ADDR flag (Read SR1 , Read SR2)
			dummyRead = I2Cx->SR1;
			dummyRead = I2Cx->SR2;
			Slave_States(I2Cx, I2C_EV_ADDR_Matched);
		}
	}

	temp3 = ( (I2Cx->SR1) & (I2C_SR1_TXE) );
	//Handle For Interrupt generated by TXE event
	if(temp1 && temp3)
	{
		// Interrupt is generated because of TXE event
		// check for device mode
		if(I2Cx->SR2 & (I2C_SR2_MSL))
		{
			//Master mode
		}
		else
		{
			//slave mode
			Slave_States(I2Cx, I2C_EV_DATA_REQ);
		}
	}

	temp3 = ( (I2Cx->SR1) & (I2C_SR1_RXNE) );
	//Handle For Interrupt generated by RXNE event
	if(temp1 && temp3)
	{
		// Interrupt is generated because of RXNE event
		// check for device mode
		if(I2Cx->SR2 & (I2C_SR2_MSL))
		{
			//Master mode
		}
		else
		{
			//slave mode
			Slave_States(I2Cx, I2C_EV_DATA_RCV);
		}
	}

}

void I2C1_ER_IRQHandler(void)
{

}

void I2C2_EV_IRQHandler(void)
{
	volatile uint32_t dummyRead = 0;
	I2C_TypeDef* I2Cx = I2C2;
	//Interrupt handling for both master and slave mode of a device
	uint32_t temp1, temp2, temp3;

	temp1 = ( (I2Cx->CR2) & (I2C_CR2_ITEVTEN) );
	temp2 = ( (I2Cx->CR2) & (I2C_CR2_ITBUFEN) );
	temp3 = ( (I2Cx->SR1) & (I2C_SR1_STOPF) );

	//Handle for interrupt generated by STOPF event
	//Note : stop detection flag is applicable only slave mode
	if(temp1 && temp3)
	{
		//STOPF is set
		//clear the STOPF by reading SR1 register followed by writing to CR1 register
		I2Cx->CR1 |= 0x0000;
		Slave_States(I2Cx, I2C_EV_STOP);
	}

	temp3 = ( (I2Cx->SR1) & (I2C_SR1_ADDR) );
	//Handle For Interrupt generated by ADDR event
	//Note :When master mode : Address is sent
	//When slave mode  : Address is matched with own address
	if(temp1 && temp3)
	{
		// Interrupt is generated because of ADDR event
		// check for device mode
		if(I2Cx->SR2 & (I2C_SR2_MSL))
		{
			//Master mode
		}
		else
		{
			//Slave
			//Clear the ADDR flag (Read SR1 , Read SR2)
			dummyRead = I2Cx->SR1;
			dummyRead = I2Cx->SR2;
			Slave_States(I2Cx, I2C_EV_ADDR_Matched);
		}
	}

	temp3 = ( (I2Cx->SR1) & (I2C_SR1_TXE) );
	//Handle For Interrupt generated by TXE event
	if(temp1 && temp3)
	{
		// Interrupt is generated because of TXE event
		// check for device mode
		if(I2Cx->SR2 & (I2C_SR2_MSL))
		{
			//Master mode
		}
		else
		{
			//slave mode
			Slave_States(I2Cx, I2C_EV_DATA_REQ);
		}
	}

	temp3 = ( (I2Cx->SR1) & (I2C_SR1_RXNE) );
	//Handle For Interrupt generated by RXNE event
	if(temp1 && temp3)
	{
		// Interrupt is generated because of RXNE event
		// check for device mode
		if(I2Cx->SR2 & (I2C_SR2_MSL))
		{
			//Master mode
		}
		else
		{
			//slave mode
			Slave_States(I2Cx, I2C_EV_DATA_RCV);
		}
	}
}

void I2C2_ER_IRQHandler(void)
{

}
