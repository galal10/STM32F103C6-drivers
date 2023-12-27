/*
 * Stm32_F103C6_gpio_driver.c
 *
 *  Created on: Jul 19, 2023
 *      Author: galal
 */

#include "Stm32_F103C6_gpio_driver.h"


static uint8_t get_CRLH_Position(uint16_t pinNumber)
{
	switch(pinNumber)
	{
	case GPIO_PIN_0:
		return 0;
		break;

	case GPIO_PIN_1:
		return 4;
		break;

	case GPIO_PIN_2:
		return 8;
		break;

	case GPIO_PIN_3:
		return 12;
		break;

	case GPIO_PIN_4:
		return 16;
		break;

	case GPIO_PIN_5:
		return 20;
		break;

	case GPIO_PIN_6:
		return 24;
		break;

	case GPIO_PIN_7:
		return 28;
		break;

	case GPIO_PIN_8:
		return 0;
		break;

	case GPIO_PIN_9:
		return 4;
		break;

	case GPIO_PIN_10:
		return 8;
		break;

	case GPIO_PIN_11:
		return 12;
		break;

	case GPIO_PIN_12:
		return 16;
		break;

	case GPIO_PIN_13:
		return 20;
		break;

	case GPIO_PIN_14:
		return 24;
		break;

	case GPIO_PIN_15:
		return 28;
		break;

	default:
		return 0;
		break;
	}
}

/*=====================================================================
 * @Fn				- MCAL_GPIO_init
 * @brief 			- Initializes the GPIOx PINy according to the specified parameters in PinConfig
 * @param [in] 		- GPIOx: where x can be (A..E depending on device used) to select the GPIO peripheral
 * @param [in] 		- PinConfig : pointer to GPIO_PinConfig_t structure that contains the configurations information for GPIO PIN.
 * @retval			- none
 * Note				- stm32F103c6 MCU has GPIO A,B,C,D,E Modules
 * 					  but LQFP48 package has only GPIO A,B,Part of C/D exported as external PINS from the MCU
 */
void MCAL_GPIO_init(GPIO_TypeDef* GPIOx, GPIO_PinConfig_t* PinConfig)
{
	// Port configuration register low  (GPIOx_CRL) configure PINS from 0 ==> 7
	// Port configuration register high (GPIOx_CRH) configure PINS from 8 ==> 15
	volatile uint32_t* configRegister = NULL;
	uint8_t PINConfig;

	configRegister = (PinConfig->GPIO_PinNumber > GPIO_PIN_7)? &GPIOx->CRH : &GPIOx->CRL;

	//	clear CNFy[1:0] MODEy[1:0]
	(*configRegister) &=~ (0xF << get_CRLH_Position(PinConfig->GPIO_PinNumber) );


	// if pin is output
	if( (PinConfig->GPIO_mode == GPIO_OUTPUT_PP_MODE) || (PinConfig->GPIO_mode == GPIO_OUTPUT_OD_MODE) || (PinConfig->GPIO_mode == GPIO_OUTPUT_AF_PP_MODE) || (PinConfig->GPIO_mode == GPIO_OUTPUT_AF_OD_MODE) )
	{
		// for ( CNF | MODE )
		PINConfig = ( ( ((PinConfig->GPIO_mode - 4) << 2) | PinConfig->GPIO_OutputSpeed) & 0x0f );
	}

	else // in input mode (MODE[1:0]=00)
	{

		if( PinConfig->GPIO_mode == GPIO_INPUT_FLO_MODE || PinConfig->GPIO_mode == GPIO_ANALOG_MODE )
		{
			PINConfig = ( (PinConfig->GPIO_mode << 2) & 0x0f );
		}

		else if ( PinConfig->GPIO_mode == GPIO_INPUT_AF_MODE )
		{
			// AF not in data sheet but AF input = Floating input
			PINConfig = ( (GPIO_INPUT_FLO_MODE << 2) & 0x0f );
		}

		else
		{
			// Pull up = Pull down so we must made it like that
			PINConfig = ( (GPIO_INPUT_PU_MODE <<2 ) & 0x0f );
			if( PinConfig->GPIO_mode == GPIO_INPUT_PU_MODE )
			{
				GPIOx->ODR |= PinConfig->GPIO_PinNumber;
			}
			else
			{
				GPIOx->ODR &= ~(PinConfig->GPIO_PinNumber);
			}
		}

	}
	// write on CRL or CRH
	(*configRegister) |= ( (PINConfig) <<get_CRLH_Position(PinConfig->GPIO_PinNumber) );

}

/*=====================================================================
 * @Fn				- MCAL_GPIO_DeInit
 * @brief 			- reset the GPIOx register
 * @param [in] 		- GPIOx: where x can be (A..E depending on device used) to select the GPIO peripheral
 * @retval			- none
 * Note				- none
 */
void MCAL_GPIO_DeInit(GPIO_TypeDef* GPIOx)
{
	/* 1st way
	 * Reset values for rech register
	 */
	// GPIOx->CRL = 0x44444444;
	// GPIOx->CRH = 0x44444444;
	// GPIOx->ODR = 0x00000000;
	// GPIOx->BSRR= 0x00000000;
	// GPIOx->BRR = 0x00000000;
	// GPIOx->LCKR = 0x00000000;


	// 2nd Port bit reset register (GPIOx_BRR)
	if( GPIOx == GPIOA )
	{
		RCC_GPIOA_CLK_RESET(); 	// Bit 2 IOPARST: I/O port A reset
		RCC_GPIOA_CLK_DERESET();
	}

	else if( GPIOx == GPIOB )
	{
		RCC_GPIOB_CLK_RESET(); 	// Bit 3 IOPBRST: I/O port B reset
		RCC_GPIOB_CLK_DERESET();
	}

	else if( GPIOx == GPIOC )
	{
		RCC_GPIOC_CLK_RESET(); 	// Bit 4 IOPCRST: I/O port C reset
		RCC_GPIOC_CLK_DERESET();
	}

	else if( GPIOx == GPIOD )
	{
		RCC_GPIOD_CLK_RESET(); 	// Bit 5 IOPDRST: I/O port D reset
		RCC_GPIOD_CLK_DERESET();
	}

	else if( GPIOx == GPIOE )
	{
		RCC_GPIOE_CLK_RESET(); 	// Bit 6 IOPERST: I/O port E reset
		RCC_GPIOE_CLK_DERESET();
	}
}

/*=====================================================================
 * @Fn				- MCAL_GPIO_ReadPin
 * @brief 			- Read specific PIN
 * @param [in] 		- GPIOx: where x can be (A..E depending on device used) to select the GPIO peripheral
 * @param [in] 		- PinNumber : Set PinNumber according @ref GPIO_PINS_define
 * @retval			- the input PIN value (two values based on @ref GPIO_PIN_state)
 * Note				- none
 */
uint8_t MCAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t PinNumber)
{
	uint8_t bitStatus;

	if( ((GPIOx->IDR) & PinNumber) != GPIO_PIN_RESET )
	{
		bitStatus = GPIO_PIN_SET;
	}

	else
	{
		bitStatus = GPIO_PIN_RESET;
	}

	return bitStatus;
}

/*=====================================================================
 * @Fn				- MCAL_GPIO_ReadPort
 * @brief 			- Read specific Port
 * @param [in] 		- GPIOx: where x can be (A..E depending on device used) to select the GPIO peripheral
 * @retval			- the input Port value
 * Note				- none
 */
uint16_t MCAL_GPIO_ReadPort(GPIO_TypeDef* GPIOx)
{
	return ((uint16_t)GPIOx->IDR);
}

/*=====================================================================
 * @Fn				- MCAL_GPIO_WritePin
 * @brief 			- Write specific PIN
 * @param [in] 		- GPIOx: where x can be (A..E depending on device used) to select the GPIO peripheral
 * @param [in] 		- PinNumber : Specifies the port bit to write. Set by @ref GPIO_PINS_define
 * @param [in] 		- Value : Pin Value
 * @retval			- none
 * Note				- none
 */
void MCAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t PinNumber, uint8_t value)
{
	if( value != GPIO_PIN_RESET )
	{
		/* 1st way */
		// GPIOx->ODR |= PinNumber;

		/* 2nd way */
		// Bits 15:0 BSy: Port x Set bit y (y= 0 .. 15)
		// These bits are write-only and can be accessed in Word mode only.
		// 0: No action on the corresponding ODRx bit
		// 1: Set the corresponding ODRx bit
		GPIOx->BSRR = PinNumber;
	}

	else
	{
		/* 1st way */
		// GPIOx->ODR &= ~(PinNumber);

		/* 2nd way */
		// Bits 15:0 BRy: Port x Reset bit y (y= 0 .. 15)
		// These bits are write-only and can be accessed in Word mode only.
		// 0: No action on the corresponding ODRx bit
		// 1: Reset the corresponding ODRx bit
		GPIOx->BRR = PinNumber;
	}
}

/*=====================================================================
 * @Fn				- MCAL_GPIO_WritePort
 * @brief 			- Write specific Port
 * @param [in] 		- GPIOx: where x can be (A..E depending on device used) to select the GPIO peripheral
 * @param [in] 		- Value : Port Value
 * @retval			- none
 * Note				- none
 */
void MCAL_GPIO_WritePort(GPIO_TypeDef* GPIOx, uint16_t value)
{
	GPIOx->ODR = (uint32_t)value;
}

/*=====================================================================
 * @Fn				- MCAL_GPIO_TogglePin
 * @brief 			- Toggle specific PIN
 * @param [in] 		- GPIOx: where x can be (A..E depending on device used) to select the GPIO peripheral
 * @param [in] 		- PinNumber : Specifies the port bit to write. Set by @ref GPIO_PINS_define
 * @retval			- none
 * Note				- none
 */
void MCAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t PinNumber)
{
	GPIOx->ODR ^=PinNumber;
}

/*=====================================================================
 * @Fn				- MCAL_GPIO_TogglePort
 * @brief 			- Toggle specific Port
 * @param [in] 		- GPIOx: where x can be (A..E depending on device used) to select the GPIO peripheral
 * @retval			- none
 * Note				- none
 */
void MCAL_GPIO_TogglePort(GPIO_TypeDef* GPIOx)
{
	GPIOx->ODR ^= 0xFFFF;
}

/*=====================================================================
 * @Fn				- MCAL_GPIO_LcokPin
 * @brief 			- The locking mechanism allow the IO configurations to be frozen
 * @param [in] 		- GPIOx: where x can be (A..E depending on device used) to select the GPIO peripheral
 * @param [in] 		- PinNumber : Specifies the port bit to write. Set by @ref GPIO_PINS_define
 * @retval			- Ok if pin config is locked or Error if pin not locked (@ref GPIO_RETURN_LOCK)
 * Note				- none
 */
uint8_t MCAL_GPIO_LockPin(GPIO_TypeDef* GPIOx, uint16_t PinNumber)
{
	//	Bit 16 LCKK[16]: Lock key
	//	This bit can be read anytime. It can only be modified using the Lock Key Writing Sequence.
	//	0: Port configuration lock key not active
	//	1: Port configuration lock key active. GPIOx_LCKR register is locked until the next reset.
	//	LOCK key writing sequence:
	//	Write 1
	//	Write 0
	//	Write 1
	//	Read 0
	//	Read 1 (this read is optional but confirms that the lock is active)
	//	Note: During the LOCK Key Writing sequence, the value of LCK[15:0] must not change.
	//	Any error in the lock sequence will abort the lock.

	//	Bits 15:0 LCKy: Port x Lock bit y (y= 0 .. 15)
	//	These bits are read write but can only be written when the LCKK bit is 0.
	//	0: Port configuration not locked
	//	1: Port configuration locked.

	// LCKK[16]
	uint32_t tmp = 1<<16;

	tmp |= PinNumber;

	//	Write 1
	GPIOx->LCKR = tmp;
	//	Write 0
	GPIOx->LCKR = PinNumber; // this means that I cleared LCKK[16]
	//	Write 1
	GPIOx->LCKR = tmp;
	//	Read 0
	tmp = GPIOx->LCKR;
	//	Read 1
	if( ((uint32_t)GPIOx->LCKR & 1<<16 ) )
	{
		return GPIO_LOCK_EN;
	}

	return GPIO_LOCK_ERROR;
}
