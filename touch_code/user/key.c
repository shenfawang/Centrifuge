#include "key.h"
#include "timer.h"
#include "debug.h"
#include "delay.h"
#include "stm32f10x.h"

key_param_struct key_param;
uint32_t scan_val = 0;

key_param_struct *GetKeyParam(void)
{
	return (&key_param);
}

void KEYInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE );       
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_15;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;	 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;	 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
//	memset(key_param,0,sizeof(key_param));
	key_param.val = 0;
	key_param.event = 0;
}

uint8_t key_loop(void)
{
	uint8_t low = 0;
	uint8_t high = 0;
	if(IsKey1Down())
	{
		low = low | 0x01;
	}
	else
	{
		low = low & ~0x01;
	}
	
	if(IsKey2Down())
	{
		low = low | (0x01<<1);
	}
	else
	{
		low = low & ~(0x01<<1);
	}

	if(IsKey3Down())
	{
		low = low | (0x01<<2);
	}
	else
	{
		low = low & ~(0x01<<2);
	}

	if(IsKey4Down())
	{
		low = low | (0x01<<3);
	}
	else
	{
		low = low & ~(0x01<<3);
	}	
	
	if(IsKey5Down())
	{
		low = low | (0x01<<4);
	}
	else
	{
		low = low & ~(0x01<<4);
	}	
	
	if(IsKey6Down())
	{
		low = low | (0x01<<5);
	}
	else
	{
		low = low & ~(0x01<<5);
	}	
	
	if(IsKey7Down())
	{
		low = low | (0x01<<6);
	}
	else
	{
		low = low & ~(0x01<<6);
	}	

	if(IsKey8Down())
	{
		low = low | (0x01<<7);
	}
	else
	{
		low = low & ~(0x01<<7);
	}	
	
	if(IsKey9Down())
	{
		high = high | 0x01;
	}
	else
	{
		high = high & ~0x01;
	}
	key_param.val = low + ((uint16_t)high<<8);
	if(scan_val != key_param.val)
	{
		scan_val = key_param.val;
		return 1;
	}
	
	return 0;
}

