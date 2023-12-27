/*
 * keypad.h
 *
 *  Created on: Jul 8, 2023
 *      Author: galal
 */

#ifndef KEYPAD_KEYPAD_H_
#define KEYPAD_KEYPAD_H_

#include "stm32f103x6.h"
#include "Stm32_F103C6_gpio_driver.h"

#define KEYPAD_PORT			GPIOB

// ROWS
#define R0	GPIO_PIN_0
#define R1	GPIO_PIN_1
#define R2	GPIO_PIN_3
#define R3	GPIO_PIN_4
// Coloumns
#define C0	GPIO_PIN_5
#define C1	GPIO_PIN_6
#define C2	GPIO_PIN_7
#define C3	GPIO_PIN_8

GPIO_PinConfig_t pin;

void keypad_init();
char keypad_getChar();

#endif /* KEYPAD_KEYPAD_H_ */
