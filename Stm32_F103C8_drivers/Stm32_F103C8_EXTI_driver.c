/*
 * Stm32_F103C8_EXTI_driver.c
 *
 *  Created on: Aug 1, 2023
 *      Author: galal
 */

#include "Stm32_F103C8_EXTI_driver.h"

/*
 * =====================================================================================
 * 							Generic Variables
 * =====================================================================================
 */

static void (* GP_IRQ_CallBack[15]) (void);

/*
 * =====================================================================================
 * 							Generic Macros
 * =====================================================================================
 */

#define AFIO_GPIO_EXTI_Mapping(x)		(	(x==GPIOA)? 0:\
											(x==GPIOB)? 1:\
											(x==GPIOC)? 2:\
											(x==GPIOD)? 3:0		)

/*
 * =====================================================================================
 * 							Generic Functions
 * =====================================================================================
 */

static void Enable_NVIC(uint8_t IRQ)
{
	switch(IRQ)
	{
	case EXTI0:
		NVIC_IRQ6_EXTI0_Enable();
		break;

	case EXTI1:
		NVIC_IRQ7_EXTI1_Enable();
		break;

	case EXTI2:
		NVIC_IRQ8_EXTI2_Enable();
		break;

	case EXTI3:
		NVIC_IRQ9_EXTI3_Enable();
		break;

	case EXTI4:
		NVIC_IRQ10_EXTI4_Enable();
		break;

	case EXTI5:
	case EXTI6:
	case EXTI7:
	case EXTI8:
	case EXTI9:
		NVIC_IRQ23_EXTI5_9_Enable();
		break;

	case EXTI10:
	case EXTI11:
	case EXTI12:
	case EXTI13:
	case EXTI14:
	case EXTI15:
		NVIC_IRQ40_EXTI10_15_Enable();
		break;
	}
}

static void Disable_NVIC(uint8_t IRQ)
{
	switch(IRQ)
	{
	case EXTI0:
		NVIC_IRQ6_EXTI0_Disable();
		break;

	case EXTI1:
		NVIC_IRQ7_EXTI1_Disable();
		break;

	case EXTI2:
		NVIC_IRQ8_EXTI2_Disable();
		break;

	case EXTI3:
		NVIC_IRQ9_EXTI3_Disable();
		break;

	case EXTI4:
		NVIC_IRQ10_EXTI4_Disable();
		break;

	case EXTI5:
	case EXTI6:
	case EXTI7:
	case EXTI8:
	case EXTI9:
		NVIC_IRQ23_EXTI5_9_Disable();
		break;

	case EXTI10:
	case EXTI11:
	case EXTI12:
	case EXTI13:
	case EXTI14:
	case EXTI15:
		NVIC_IRQ40_EXTI10_15_Disable();
		break;
	}
}

static void Update_EXTI(EXTI_PinConfig_t* EXTI_Config)
{
	// 1- Configure GPIO to be AF input -> Floating input
	GPIO_PinConfig_t PinConfig;
	PinConfig.GPIO_mode = GPIO_INPUT_FLO_MODE;
	PinConfig.GPIO_PinNumber = EXTI_Config->EXTI_PIN.GPIO_PIN;
	MCAL_GPIO_init(EXTI_Config->EXTI_PIN.GPIO_Port, &PinConfig);
	//===============================================================

	// 2- Update AFIO to route between EXTI Line with GPIO A,B,C,D
	uint8_t AFIO_EXTICR_index = EXTI_Config->EXTI_PIN.EXTI_InputLineNumber / 4;
	uint8_t AFIO_EXTICR_position = (EXTI_Config->EXTI_PIN.EXTI_InputLineNumber % 4 ) * 4;

	// we should clear the 4 bits before writing on it
	AFIO->EXTICR[AFIO_EXTICR_index] &=~( 0xF << AFIO_EXTICR_position);
	AFIO->EXTICR[AFIO_EXTICR_index] |= ( (AFIO_GPIO_EXTI_Mapping(EXTI_Config->EXTI_PIN.GPIO_Port) &0xF) << AFIO_EXTICR_position );
	//===============================================================

	// 3- Update Rising, Falling edge
	// we must clear the bits of trigger before writing on it
	EXTI->RTSR &= ~(1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	EXTI->FTSR &= ~(1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);

	if( EXTI_Config->trigger_case == EXTI_Trigger_RISING )
	{
		EXTI->RTSR |= (1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}

	else if( EXTI_Config->trigger_case == EXTI_Trigger_FALLING )
	{
		EXTI->FTSR |= (1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}

	else if( EXTI_Config->trigger_case == EXTI_Trigger_RisingAndFalling )
	{
		EXTI->RTSR |= (1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
		EXTI->FTSR |= (1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}
	//===============================================================

	// 4- Update IRQ Handling CallBack

	GP_IRQ_CallBack[EXTI_Config->EXTI_PIN.EXTI_InputLineNumber] = EXTI_Config->P_IRQ_CallBack;

	//===============================================================

	// 5- Enable\Disable IRQ EXTI & NVIC
	if( EXTI_Config->IRQ_EN == EXTI_IRQ_Enable )
	{
		EXTI->IMR |= 1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber;

		// NVIC
		Enable_NVIC(EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}

	else if( EXTI_Config->IRQ_EN == EXTI_IRQ_Disable )
	{
		EXTI->IMR &= ~(1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
		Disable_NVIC(EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}
	//===============================================================

}

//========================================================================================

/*
 * =====================================================================================
 * 							APIS Supported by "MCAL GPIO driver"
 * =====================================================================================
 */

/*=====================================================================
 * @Fn				- MCAL_EXTI_GPIO_Deinit
 * @brief 			- Reset EXTI registers and NVIC corresponding to IRQ Mask
 * @retval			- none
 * Note				- none
 */


void MCAL_EXTI_GPIO_Deinit(void)
{
	EXTI->IMR   = 0x00000000;
	EXTI->EMR   = 0x00000000;
	EXTI->RTSR  = 0x00000000;
	EXTI->FTSR  = 0x00000000;
	EXTI->SWIER = 0x00000000;

	// rc_w1: cleared by writing a ‘1’ into the bit.
	EXTI->PR    = 0xFFFFFFFF;

	// Disable EXTI IRQ from NVIC
	NVIC_IRQ6_EXTI0_Disable();
	NVIC_IRQ7_EXTI1_Disable();
	NVIC_IRQ8_EXTI2_Disable();
	NVIC_IRQ9_EXTI3_Disable();
	NVIC_IRQ10_EXTI4_Disable();
	NVIC_IRQ23_EXTI5_9_Disable();
	NVIC_IRQ40_EXTI10_15_Disable();
}

/*=====================================================================
 * @Fn				- MCAL_EXTI_GPIO_Init
 * @brief 			- this is used to Initializes the EXTI from specific GPIO PIN and specify the Mask/Trigger condition and IRQ CallBack
 * @param [in] 		- EXTI_Config set by @ref EXTI_define, EXTI_Trigger_define and EXTI_IRQ_define
 * @retval			- none
 * Note				- stm32F103c8 MCU has GPIO A,B,C,D,E Modules
 * 					- but LQFP48 package has only GPIO A,B,Part of C/D exported as external PINS from the MCU
 * 					- Also Mandatory to enable RCC clock for AFIO and corresponding GPIO
 */

void MCAL_EXTI_GPIO_Init(EXTI_PinConfig_t* EXTI_Config)
{
	Update_EXTI(EXTI_Config);
}

/*=====================================================================
 * @Fn				- MCAL_EXTI_GPIO_Update
 * @brief 			- this is used to update the EXTI from specific GPIO PIN and specify the Mask/Trigger condition and IRQ CallBack
 * @param [in] 		- EXTI_Config set by @ref EXTI_define, EXTI_Trigger_define and EXTI_IRQ_define
 * @retval			- none
 * Note				- stm32F103c8 MCU has GPIO A,B,C,D,E Modules
 * 					- but LQFP48 package has only GPIO A,B,Part of C/D exported as external PINS from the MCU
 * 					- Also Mandatory to enable RCC clock for AFIO and corresponding GPIO
 */
void MCAL_EXTI_GPIO_Update(EXTI_PinConfig_t* EXTI_Config)
{
	Update_EXTI(EXTI_Config);
}
//========================================================================================

/*
 * =====================================================================================
 * 							ISR Functions
 * =====================================================================================
 */

// cleared by writing a ‘1’ into the bit Pending register (EXTI_PR)
void EXTI0_IRQHandler(void)
{
	EXTI->PR |= 1<<0;
	GP_IRQ_CallBack[0]();
}

void EXTI1_IRQHandler(void)
{
	EXTI->PR |= 1<<1;
	GP_IRQ_CallBack[1]();
}

void EXTI2_IRQHandler(void)
{
	EXTI->PR |= 1<<2;
	GP_IRQ_CallBack[2]();
}

void EXTI3_IRQHandler(void)
{
	EXTI->PR |= 1<<3;
	GP_IRQ_CallBack[3]();
}

void EXTI4_IRQHandler(void)
{
	EXTI->PR |= 1<<4;
	GP_IRQ_CallBack[4]();
}

void EXTI9_5_IRQHandler(void)
{
	if( EXTI->PR & 1<<5 ) { EXTI->PR |= 1<<5; GP_IRQ_CallBack[5](); }
	if( EXTI->PR & 1<<6 ) { EXTI->PR |= 1<<6; GP_IRQ_CallBack[6](); }
	if( EXTI->PR & 1<<7 ) { EXTI->PR |= 1<<7; GP_IRQ_CallBack[7](); }
	if( EXTI->PR & 1<<8 ) { EXTI->PR |= 1<<8; GP_IRQ_CallBack[8](); }
	if( EXTI->PR & 1<<9 ) { EXTI->PR |= 1<<9; GP_IRQ_CallBack[9](); }
}

void EXTI15_10_IRQHandler(void)
{
	if( EXTI->PR & 1<<10 ) { EXTI->PR |= 1<<10; GP_IRQ_CallBack[10](); }
	if( EXTI->PR & 1<<11 ) { EXTI->PR |= 1<<11; GP_IRQ_CallBack[11](); }
	if( EXTI->PR & 1<<12 ) { EXTI->PR |= 1<<12; GP_IRQ_CallBack[12](); }
	if( EXTI->PR & 1<<13 ) { EXTI->PR |= 1<<13; GP_IRQ_CallBack[13](); }
	if( EXTI->PR & 1<<14 ) { EXTI->PR |= 1<<14; GP_IRQ_CallBack[14](); }
	if( EXTI->PR & 1<<15 ) { EXTI->PR |= 1<<15; GP_IRQ_CallBack[15](); }
}
