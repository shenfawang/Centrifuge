#ifndef         __NEW_KEY_H
#define         __NEW_KEY_H
#include "stm32f10x.h"

typedef struct KEY_PARAM_STRUCT
{
	uint8_t event;
	uint16_t val;
//	uint8_t interrupt; //0 产生中断 1 无中断
}key_param_struct;

static uint8_t IsKey1Down(void) 		{if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == Bit_SET) return 0; return 1;}
static uint8_t IsKey2Down(void) 	  {if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == Bit_SET) return 0; return 1;}
static uint8_t IsKey3Down(void) 	  {if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == Bit_SET) return 0; return 1;}
static uint8_t IsKey4Down(void) 		{if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == Bit_SET) return 0; return 1;}
static uint8_t IsKey5Down(void) 		{if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == Bit_SET) return 0; return 1;}
static uint8_t IsKey6Down(void) 		{if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2) == Bit_SET) return 0; return 1;}
static uint8_t IsKey7Down(void) 	  {if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == Bit_SET) return 0; return 1;}
static uint8_t IsKey8Down(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == Bit_SET) return 0; return 1;}
static uint8_t IsKey9Down(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == Bit_SET) return 0; return 1;}
//static uint8_t IsKey1_2Down(void) {if (IsKey1Down() && IsKey2Down()) return 1; return 0;}	/* 组合键 */

void KEYInit(void);
uint8_t key_loop(void);
key_param_struct *GetKeyParam(void);
#endif
