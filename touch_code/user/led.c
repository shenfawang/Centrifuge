#include "led.h"
#include "spi.h"
#include <stdio.h>

void LEDInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB,ENABLE );     
/***********************LED******************************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
	GPIO_SetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	/***********************LED******************************/	
}

void LED_Draw_s(uint8_t state)
{
	if(state==0)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_0);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);
	}
}

void LED_Return_s(uint8_t state)
{
	if(state==0)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_1);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_1);
	}
}

void LED_Caution_s(uint8_t state)
{
	if(state==0)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_2);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
	}
}

void LED_Warnning_s(uint8_t state)
{
	if(state==0)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_3);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_3);
	}
}

void LED_Stop(uint8_t state)
{
	if(state)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
		printf("LED Stop is on\n");
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		printf("LED Stop is off\n");
	}
}

void LED_Return(uint8_t state)
{
	if(state)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
		printf("LED Return is on\n");
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);
		printf("LED Return is off\n");
	}
}

void LED_Pause(uint8_t state)
{
	if(state)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
		printf("LED Pause is on\n");
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		printf("LED Pause is off\n");
	}
}

void LED_Modify(uint8_t state)
{
	if(state)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_7);
		printf("LED Modify is on\n");
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		printf("LED Modify is off\n");
	}
}


void LED_Draw(uint8_t state)
{
	if(state)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
		printf("LED Draw is on\n");
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);
		printf("LED Draw is off\n");
	}
}

void LED_Prime(uint8_t state)
{
	if(state)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	}
}
