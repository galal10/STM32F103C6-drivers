/*
 * lcd.c
 *
 *  Created on: Jul 4, 2023
 *      Author: galal
 */


#include "lcd.h"

void delay(uint32_t time)
{
	uint32_t i,j;

	for(i = 0; i < time; i++)
	{
		for(j = 0; j < 255; j++);
	}
}

static void LCD_GPIO_init()
{
	// CTRL as output
	// Enable_SWITCH, RS_SWITCH, RW_SWITCH output push pull mode
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = Enable_SWITCH;
	MCAL_GPIO_init(LCD_CTRL, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = RW_SWITCH;
	MCAL_GPIO_init(LCD_CTRL, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = RS_SWITCH;
	MCAL_GPIO_init(LCD_CTRL, &pin);
	/* ################################### */

	// data pins as output push pull with speed 10MHZ
	// A1==>A7 push pull output mode
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_0;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_1;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_2;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_3;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_4;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_5;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_6;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_7;
	MCAL_GPIO_init(LCD_PORT, &pin);
	/* ################################### */

	MCAL_GPIO_WritePin(LCD_CTRL, Enable_SWITCH, GPIO_PIN_RESET);
	MCAL_GPIO_WritePin(LCD_CTRL, RW_SWITCH, GPIO_PIN_RESET);
	MCAL_GPIO_WritePin(LCD_CTRL, RS_SWITCH, GPIO_PIN_RESET);
}

static void LCD_PORT_output()
{
	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_0;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_1;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_2;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_3;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_4;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_5;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_6;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_OUTPUT_PP_MODE;
	pin.GPIO_OutputSpeed = GPIO_SPEED_10M;
	pin.GPIO_PinNumber = GPIO_PIN_7;
	MCAL_GPIO_init(LCD_PORT, &pin);
	/* ################################### */
}

static void LCD_PORT_input()
{
	pin.GPIO_mode = GPIO_INPUT_FLO_MODE;
	pin.GPIO_PinNumber = GPIO_PIN_0;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_INPUT_FLO_MODE;
	pin.GPIO_PinNumber = GPIO_PIN_1;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_INPUT_FLO_MODE;
	pin.GPIO_PinNumber = GPIO_PIN_2;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_INPUT_FLO_MODE;
	pin.GPIO_PinNumber = GPIO_PIN_3;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_INPUT_FLO_MODE;
	pin.GPIO_PinNumber = GPIO_PIN_4;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_INPUT_FLO_MODE;
	pin.GPIO_PinNumber = GPIO_PIN_5;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_INPUT_FLO_MODE;
	pin.GPIO_PinNumber = GPIO_PIN_6;
	MCAL_GPIO_init(LCD_PORT, &pin);

	pin.GPIO_mode = GPIO_INPUT_FLO_MODE;
	pin.GPIO_PinNumber = GPIO_PIN_7;
	MCAL_GPIO_init(LCD_PORT, &pin);
	/* ################################### */
}

void LCD_KICK()
{
	MCAL_GPIO_WritePin(LCD_CTRL, Enable_SWITCH, GPIO_PIN_SET); // enable off
	delay(50);
	MCAL_GPIO_WritePin(LCD_CTRL, Enable_SWITCH, GPIO_PIN_RESET); // enable on
}

void LCD_isBusy()
{
	// data pins as input floating
	// A1==>A7
	LCD_PORT_input();

	MCAL_GPIO_WritePin(LCD_CTRL, RW_SWITCH, GPIO_PIN_SET); // read mode
	MCAL_GPIO_WritePin(LCD_CTRL, RS_SWITCH, GPIO_PIN_RESET); // RS off

	LCD_KICK();

	// data pins as output push pull with speed 10MHZ
	// A1==>A7 push pull output mode
	LCD_PORT_output();

	MCAL_GPIO_WritePin(LCD_CTRL, RW_SWITCH, GPIO_PIN_RESET); // write mode
}

void LCD_init()
{
	// wait for the hardware to initialize
	delay(20);
	//LCD_isBusy();

	LCD_GPIO_init();

	delay(15);
	LCD_ClearScreen();

	LCD_Write_Command(LCD_FUNCTION_8BIT_2LINES);

	LCD_Write_Command(LCD_DISP_ON_CURSOR_BLINK);
	LCD_Write_Command(LCD_ENTRY_MODE);
}

void LCD_ClearScreen()
{
	LCD_Write_Command(LCD_CLEAR_SCREEN);
}

void LCD_Write_Command(uint8_t command)
{
	LCD_isBusy();

	MCAL_GPIO_WritePort(LCD_PORT, command);
	MCAL_GPIO_WritePin(LCD_CTRL, RS_SWITCH, GPIO_PIN_RESET);
	MCAL_GPIO_WritePin(LCD_CTRL, RW_SWITCH, GPIO_PIN_RESET);

	delay(1);
	LCD_KICK();
}

void LCD_Write_Char(uint8_t data)
{
	LCD_isBusy();

	MCAL_GPIO_WritePort(LCD_PORT, data);
	MCAL_GPIO_WritePin(LCD_CTRL, RS_SWITCH, GPIO_PIN_SET);
	MCAL_GPIO_WritePin(LCD_CTRL, RW_SWITCH, GPIO_PIN_RESET);

	delay(1);
	LCD_KICK();
}

void LCD_goto_xy(int line, int position)
{
	if( line == 1 )
	{
		if( position < 16 && position >=0 )
		{
			LCD_Write_Command(LCD_BEGIN_AT_FIRST_ROW + position);
		}
	}

	else if( line == 2)
	{
		if( position < 32 && position >=0 )
		{
			LCD_Write_Command(LCD_BEGIN_AT_SECOND_ROW + position);
		}
	}
}

void LCD_Write_String(uint8_t* data)
{
	int count = 0;

	while( *data > 0)
	{
		count++;
		LCD_Write_Char(*data++);

		if( count == 16 )
		{
			LCD_goto_xy(2, 0);
		}

		else if( count == 32 )
		{
			LCD_ClearScreen();
			LCD_goto_xy(1, 0);
			count = 0;
		}
	}
}

