/*
 * Stm32_F103C6_I2C_driver.h
 *
 *  Created on: Nov 26, 2023
 *      Author: galal
 */

#ifndef INC_STM32_F103C6_I2C_DRIVER_H_
#define INC_STM32_F103C6_I2C_DRIVER_H_

#include "stm32f103x6.h"
#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_RCC_driver.h"


struct S_I2C_Slave_Device_Address
{
	uint8_t 		Enable_Dual_ADD;			// 1-Enable  0-Disable
	uint16_t 		Primary_Slave_ADD;
	uint8_t 		Secondary_Slave_ADD;
	uint16_t 		I2C_Addressing_SLave_Mode;	//@ref I2C_Addressing_SLave_Mode
};

typedef enum
{
	I2C_EV_STOP,
	I2C_ERROR_AF,
	I2C_EV_ADDR_Matched,
	I2C_EV_DATA_REQ,				//The APP layer should send the data (I2C slave Send data)
	I2C_EV_DATA_RCV					//The APP layer should read the data (I2C slave receive data)
}Slave_State;

//Configuration structure
typedef struct
{
	uint32_t 							I2C_ClockSpeed;		//Specifies I2C Clock Frequency
															//This parameter must be set based on @ref I2C_SCLK

	uint8_t								StretchMode;		//Specifies enable or disable stretching mode
															//This parameter must be set based on @ref I2C_StretchMode

	uint8_t								I2C_Mode;			//Specifies the I2C mode
															//This parameter must be set based on @ref I2C_Mode

	struct S_I2C_Slave_Device_Address	I2C_Slave_Address;
	uint16_t							I2C_ACK_Control;	//Specifies that acknowledge is enable or disable
															//This parameter must be set based on @ref I2C_ACK_Control

	uint8_t								ENGC;				//Specifies that General call is enable or disable
															//This parameter must be set based on @ref I2C_ENGC
	void (* P_Slave_Event_CallBack)(Slave_State state);
}I2C_Config_t;


//==================================================
//Macros Configurations References
//==================================================

//@ref I2C_SCLK
//– Standard Speed (up to 100 kHz)
//– Fast Speed (up to 400 kHz)

/**** To configure clock before enable the peripheral
 * 1- I2C_CR2.FREQ[5:0]: Peripheral clock frequency
 *
 * 2- Configure the clock control registers SM/FM
 * 	  CCR[11:0]: Clock control register
 * 	  Thigh = Ti2c/2 = CCR * TPCLK1
 * 	  CCR = TI2C / (2 * TPCLK1) = FPCLK1 / (2 * FI2C)
 *
 *
 * 3- Configure the rise time register
 * 	  TRISE[5:0] = (maximum allowed SCL rise time "1000ns"/  TPCLK1) + 1
 *	  TRISE[5:0] = Freq of PCLK1 / 1000000
 */
#define I2C_SCLK_SM_50K				(50000U)
#define I2C_SCLK_SM_100K			(100000U)
#define I2C_SCLK_FM_200K			(200000U)	//Fast mode not supported yet
#define I2C_SCLK_FM_400K			(400000U)	//Fast mode not supported yet

//@ref I2C_StretchMode
//I2C_CR1.Bit 7 NOSTRETCH: Clock stretching disable (Slave mode)
#define I2C_StretchMode_Enable		0x00U
#define I2C_StretchMode_Disable		I2C_CR1_NOSTRETCH


//@ref I2C_Mode
//I2C_CR1.Bit 1 SMBUS: SMBus mode
#define I2C_Mode_I2C		0x00U
#define I2C_Mode_SMBus		I2C_CR1_SMBUS

//@ref I2C_Addressing_SLave_Mode
//I2C_OAR1.Bit 15 ADDMODE Addressing mode (slave mode)
#define I2C_Addressing_SLave_Mode_7B		0x0000U
#define I2C_Addressing_SLave_Mode_10B		(uint16_t)1<<15

//@ref I2C_ACK_Control
//I2C_CR1.Bit 10 ACK: Acknowledge enable
#define I2C_ACK_Control_Enable		I2C_CR1_ACK
#define I2C_ACK_Control_Disable		0x0000U

//@ref I2C_ENGC
//I2C_CR1.Bit 6 ENGC: General call enable
#define I2C_ENGC_Enable				I2C_CR1_ENGC
#define I2C_ENGC_Disable			0x00U

typedef enum
{
	With_Stop,
	Without_Stop
}Stop_Condition;

typedef enum
{
	Start,
	Repeated_Start
}Start_State;

typedef enum
{
	Disable,
	Enable
}FunctionalState;

typedef enum
{
	Reset,
	Set
}FlagStatus;

typedef enum
{
	I2C_FLAG_BUSY,
	EV5,									//SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	EV6,									//ADDR=1, cleared by reading SR1 register followed by reading SR2.
	EV8,									//TxE=1, shift register not empty, data register empty, cleared by writing DR register
	EV7,									//RXNE = 1 cleared by reading DR Register
	EV8_2 = 0x84,							//TxE=1, BTF = 1, Program Stop request. TxE and BTF are cleared by hardware by the Stop condition

	MASTER_BYTE_TRANSMITTING= 0x00070080	//concatenate SR1, SR2 together and set bit of TRA, BUSY, MSL, TXE
}Status;

typedef enum
{
	Transmitter,
	Receiver
}I2C_Direction;

/*
 * =====================================================================================
 * 							APIS Supported by "MCAL I2C driver"
 * =====================================================================================
 */
void MCAL_I2C_Init(I2C_TypeDef* I2Cx, I2C_Config_t* I2C_Config);
void MCAL_I2C_DeInit(I2C_TypeDef* I2Cx);

void MCAL_I2C_GPIO_Set_Pins(I2C_TypeDef* I2Cx);

//Master Polling Mechanism
void MCAL_I2C_Master_Tx(I2C_TypeDef* I2Cx, uint16_t devAddr, uint8_t* dataOut, uint32_t dataLen, Stop_Condition Stop, Start_State start);
void MCAL_I2C_Master_Rx(I2C_TypeDef* I2Cx, uint16_t devAddr, uint8_t* Buffer, uint32_t dataLen, Stop_Condition stop, Start_State start);

//Slave interrupt Mechanism
void MCAL_I2C_SlaveSendData(I2C_TypeDef* I2Cx, uint8_t data);
uint8_t MCAL_I2C_SlaveReceiveData(I2C_TypeDef* I2Cx);

//Generic APIs
void I2C_GenerateStart(I2C_TypeDef* I2Cx, FunctionalState NewState, Start_State start);
void I2C_GenerateStop(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_SendAddress(I2C_TypeDef* I2Cx, uint16_t Address, I2C_Direction Direction); //I2C_Direction : Transmit(0) or Receive(1)
void I2C_ACK_Config(I2C_TypeDef* I2Cx, FunctionalState NewState);
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, Status flag);
void Slave_States(I2C_TypeDef* I2Cx, Slave_State state);

#endif /* INC_STM32_F103C6_I2C_DRIVER_H_ */
