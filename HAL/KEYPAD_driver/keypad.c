/*
 * keypad.c
 *
 *  Created on: Jul 8, 2023
 *      Author: galal
 */

#include "keypad.h"

int keypad_rows[] = {R0, R1, R2, R3};
int keypad_cols[] = {C0, C1, C2, C3};

void keypad_init()
{
	pin.GPIO_PinNumber = R0;
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	MCAL_GPIO_init(KEYPAD_PORT, &pin);

	pin.GPIO_PinNumber = R1;
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	MCAL_GPIO_init(KEYPAD_PORT, &pin);

	pin.GPIO_PinNumber = R2;
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	MCAL_GPIO_init(KEYPAD_PORT, &pin);

	pin.GPIO_PinNumber = R3;
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	MCAL_GPIO_init(KEYPAD_PORT, &pin);

	pin.GPIO_PinNumber = C0;
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	MCAL_GPIO_init(KEYPAD_PORT, &pin);

	pin.GPIO_PinNumber = C1;
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	MCAL_GPIO_init(KEYPAD_PORT, &pin);

	pin.GPIO_PinNumber = C2;
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	MCAL_GPIO_init(KEYPAD_PORT, &pin);

	pin.GPIO_PinNumber = C3;
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	MCAL_GPIO_init(KEYPAD_PORT, &pin);

//	KEYPAD_PORT = 0xFF;
	MCAL_GPIO_WritePort(KEYPAD_PORT, (0x1FF | MCAL_GPIO_ReadPort(GPIOB)) );
}

char keypad_getChar()
{
	int i, j;

	for(i = 0; i < 4; i++)
	{
		MCAL_GPIO_WritePin(KEYPAD_PORT, keypad_cols[0], GPIO_PIN_SET);
		MCAL_GPIO_WritePin(KEYPAD_PORT, keypad_cols[1], GPIO_PIN_SET);
		MCAL_GPIO_WritePin(KEYPAD_PORT, keypad_cols[2], GPIO_PIN_SET);
		MCAL_GPIO_WritePin(KEYPAD_PORT, keypad_cols[3], GPIO_PIN_SET);

		// make one pin with 0
		MCAL_GPIO_WritePin(KEYPAD_PORT, keypad_cols[i], GPIO_PIN_RESET);

		for(j = 0; j < 4; j++)
		{
			if( (MCAL_GPIO_ReadPin(KEYPAD_PORT, keypad_rows[j]) ) == GPIO_PIN_RESET )
			{
				while( (MCAL_GPIO_ReadPin(KEYPAD_PORT, keypad_rows[j]) ) == GPIO_PIN_RESET );
				switch(i)
				{
					case(0):
						if( j == 0) return '7';
						else if( j == 1) return '4';
						else if( j == 2) return '1';
						else if( j == 3) return '!';
						break;
					case(1):
						if( j == 0) return '8';
						else if( j == 1) return '5';
						else if( j == 2) return '2';
						else if( j == 3) return '0';
						break;
					case(2):
						if( j == 0) return '9';
						else if( j == 1) return '6';
						else if( j == 2) return '3';
						else if( j == 3) return '=';
						break;
					case(3):
						if( j == 0) return '/';
						else if( j == 1) return '*';
						else if( j == 2) return '-';
						else if( j == 3) return '+';
						break;
				}
			}
		}
	}
	// if it is not pressed
	return 'N';
}
