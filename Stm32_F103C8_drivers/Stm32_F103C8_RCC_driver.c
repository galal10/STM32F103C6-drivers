/*
 * Stm32_F103C8_RCC_driver.c
 *
 *  Created on: Nov 3, 2023
 *      Author: galal
 */

#include "Stm32_F103C8_RCC_driver.h"

//Bits 10:8 PPRE1: APB low-speed prescaler (APB1)
//Set and cleared by software to control the division factor of the APB low-speed clock
//(PCLK1).
//Warning: the software has to set correctly these bits to not exceed 36 MHz on this domain.
//0xx: HCLK not divided
//100: HCLK divided by 2
//101: HCLK divided by 4
//110: HCLK divided by 8
//111: HCLK divided by 16
static const uint8_t APBPrescTable[8u] = {0, 0, 0, 0, 1, 2, 3, 4};

//Bits 7:4 HPRE: AHB prescaler
//Set and cleared by software to control the division factor of the AHB clock.
//0xxx: SYSCLK not divided
//1000: SYSCLK divided by 2
//1001: SYSCLK divided by 4
//1010: SYSCLK divided by 8
//1011: SYSCLK divided by 16
//1100: SYSCLK divided by 64
//1101: SYSCLK divided by 128
//1110: SYSCLK divided by 256
//1111: SYSCLK divided by 512
static const uint32_t AHBPrescTable[16u] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

uint32_t MCAL_RCC_GetSYSCLKFreq(void)
{
//	Bits 3:2 SWS: System clock switch status
//	Set and cleared by hardware to indicate which clock source is used as system clock.
//	00: HSI oscillator used as system clock
//	01: HSE oscillator used as system clock
//	10: PLL used as system clock
//	11: not applicable

	switch( ((RCC->CFGR >> 2) & 0b11) )
	{
	case 0:
		return HSI_CLK;
		break;

	case 1:
		//todo need to calculate it
		// HSE user should specify it
		return HSE_CLK;
		break;

	case 2:
		//todo need to calculate it PLL_CLK and PLL_MUL & PLL Source MUX
		return 16000000;
		break;
	}
}
uint32_t MCAL_RCC_GetHCLKFreq(void)
{
	//Bits 7:4 HPRE: AHB prescaler
	return (MCAL_RCC_GetSYSCLKFreq() >> AHBPrescTable[((RCC->CFGR >> 4) & 0xF)] );
}
uint32_t MCAL_RCC_GetPCLK1Freq(void)
{
	//Bits 10:8 PPRE1: APB low-speed prescaler (APB1)
	return (MCAL_RCC_GetHCLKFreq() >> APBPrescTable[((RCC->CFGR >> 8) & 0b111)]);
}
uint32_t MCAL_RCC_GetPCLK2Freq(void)
{
	//Bits 13:11 PPRE2: APB high-speed prescaler (APB2)
	return (MCAL_RCC_GetHCLKFreq() >> APBPrescTable[((RCC->CFGR >> 11) & 0b111)]);
}
