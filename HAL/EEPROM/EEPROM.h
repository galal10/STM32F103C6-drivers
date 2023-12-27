/*
 * EEPROM.h
 *
 *  Created on: Dec 10, 2023
 *      Author: galal
 */

#ifndef EEPROM_EEPROM_H_
#define EEPROM_EEPROM_H_

#include "Stm32_F103C6_I2C_driver.h"

// EEPROM is an I2C Slave only.
// Idle Mode: device is high-impedance state and waits for data.
// Master Transmitter Mode: the MCU device transmits data to a slave receiver.
// Master Receiver Mode: the MCU device receives data from a slave transmitter.

#define EEPROM_Slave_Address	0x2A

void HAL_EEPROM_Init(void);
uint8_t HAL_EEPROM_Write_NBytes(uint32_t Memory_Address, uint8_t* Data, uint8_t dataLen);
uint8_t HAL_EEPROM_Read_NBytes(uint32_t Memory_Address, uint8_t* Buffer, uint8_t dataLen);


#endif /* EEPROM_EEPROM_H_ */
