/*
 * Stm32_F103C8_RCC_driver.h
 *
 *  Created on: Nov 3, 2023
 *      Author: galal
 */

#ifndef INC_STM32_F103C8_RCC_DRIVER_H_
#define INC_STM32_F103C8_RCC_DRIVER_H_

#include "stm32f103x8.h"
#include "Stm32_F103C8_gpio_driver.h"

#define HSI_CLK			(uint32_t)8000000
#define HSE_CLK			(uint32_t)16000000

/*
 * =====================================================================================
 * 							APIS Supported by "MCAL RCC driver"
 * =====================================================================================
 */

uint32_t MCAL_RCC_GetSYSCLKFreq(void);
uint32_t MCAL_RCC_GetHCLKFreq(void);
uint32_t MCAL_RCC_GetPCLK1Freq(void);
uint32_t MCAL_RCC_GetPCLK2Freq(void);

#endif /* INC_STM32_F103C8_RCC_DRIVER_H_ */
