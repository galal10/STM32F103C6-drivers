/*
 * EEPROM.c
 *
 *  Created on: Dec 10, 2023
 *      Author: galal
 */

#include "EEPROM.h"

void HAL_EEPROM_Init(void)
{
	/*-------------------------------I2C Init--------------------------*/
	//			PB6: I2C1_SCL
	//			PB7: I2C2_SDA
	I2C_Config_t I2C_Config;

	/*-------------------------------I2C controller act as a Master--------------------------*/
	I2C_Config.ENGC = I2C_ENGC_Enable;
	I2C_Config.I2C_ACK_Control = I2C_ACK_Control_Enable;
	I2C_Config.I2C_ClockSpeed = I2C_SCLK_SM_100K;
	I2C_Config.I2C_Mode = I2C_Mode_I2C;
	I2C_Config.StretchMode = I2C_StretchMode_Enable;
	I2C_Config.P_Slave_Event_CallBack = NULL;

	MCAL_I2C_GPIO_Set_Pins(I2C1);
	MCAL_I2C_Init(I2C1, &I2C_Config);
}

uint8_t HAL_EEPROM_Write_NBytes(uint32_t Memory_Address, uint8_t* Data, uint8_t dataLen)
{
	//MISRA C++ 2008, 18-4-1 : Dynamic Memory Allocation shall not be used
	//Violate MISRA Rule
	//uint8_t *buffer = (unsigned char *)malloc(Data_Length+2);

	//So we'll use static buffer 1st and 2nd location for address
	uint8_t buffer[256] ;
	buffer[0] = (uint8_t)(Memory_Address >> 8); //Upper Byte
	buffer[1] = (uint8_t)Memory_Address;		//Lower Byte

	//Save data in Buffer
	uint8_t index = 0;
	for(index = 2; index < (dataLen + 2); index++)
	{
		buffer[index] = Data[index - 2];
	}

	MCAL_I2C_Master_Tx(I2C1, EEPROM_Slave_Address, buffer, (dataLen + 2), With_Stop, Start);

	return 0;
}

uint8_t HAL_EEPROM_Read_NBytes(uint32_t Memory_Address, uint8_t* Buffer, uint8_t dataLen)
{
	uint8_t bufferAddr[2];
	bufferAddr[0] = (uint8_t)(Memory_Address >> 8);	//Upper Byte
	bufferAddr[1] = (uint8_t)Memory_Address;		//Lower Byte

	MCAL_I2C_Master_Tx(I2C1, EEPROM_Slave_Address, bufferAddr, 2, Without_Stop, Start);			//Write Address only

	MCAL_I2C_Master_Rx(I2C1, EEPROM_Slave_Address, Buffer, dataLen, With_Stop, Repeated_Start);	//Read Data

	return 0;
}
