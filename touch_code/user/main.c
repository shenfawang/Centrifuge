/**
  ******************************************************************************
  * @file    main.c 
  * @author  Shenfawang
  * @version V3.3.0
  * @date    10/15/2010
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 
	
/** @addtogroup IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_usart.h"
#include "timer.h"
#include "delay.h"
#include <stdio.h>
#include "string.h"
#include "misc.h"
#include "Debug.h"
#include "spi.h"
#include "led.h"
#include "main.h"
#include "key.h"
uint32_t print_val=0;

print_buf_struct print_buf;
void RCC_Configuration(void)
{   
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO  | RCC_APB2Periph_GPIOA |
													RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
													RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
													RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG , ENABLE);
}
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		   

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //select timer3 IRQ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//enable
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //select timer2 IRQ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//enable
	NVIC_Init(&NVIC_InitStructure);
}

void SystemReportInterrupt(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	delay_us(10);
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
	//按键事件产生
}

void ClearSystemReportInterrupt(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
	
}
uint8_t printf_type=0,printf_len;
uint8_t printf_buf[16];
void PrintfInterruptParam(uint8_t type,uint8_t *buf,uint8_t len)
{
	printf_type = type;
	printf_len = len;
	memcpy(printf_buf,buf,len);
}

/*void PrintfLoop(void)
{
	if(printf_type == 1)//spi read
	{
		//printf("sd:%d,%d \r\n",printf_buf[0],printf_buf[1]);
		printf_type = 0;
	}
	else if(printf_type == 2)//spi write
	{
		printf("sw:%d,%d \r\n",printf_buf[0],printf_buf[1]);
		printf_type = 0;
	}
}*/



void PrintfWrite(char *buf)
{
	uint16_t index;
	uint16_t count;
	index= print_buf.write - print_buf.buf;
	count = strlen(buf);
	if((PRINT_BUF_SIZE-index)>=count)
	{
		memcpy(print_buf.write,buf,count);
//		print_buf.write+=count;
	}
	else
	{
		uint16_t count1,count2;
		count1 =PRINT_BUF_SIZE - index;
		memcpy(print_buf.write,buf,count1);
		count2 = count - count1;
		memcpy(&print_buf.buf,&buf[count1],count2);
	}
	
	index+=count;
	index = index%PRINT_BUF_SIZE;
	print_buf.write = print_buf.buf+index;
}
char buf[PRINT_BUF_SIZE];
void PrintfLoop(void)
{
	uint16_t count;
	memset(buf,0,sizeof(buf));
	if(print_buf.write!=print_buf.read)
	{
		if(print_buf.write>print_buf.read)
		{
			count = print_buf.write - print_buf.read;
			memcpy(buf,print_buf.read,count);
			print_buf.read = print_buf.read+count;//print_buf.write;
			printf("%s \r\n",buf);
		}
		else
		{
			uint16_t count1,count2;
			count1 = print_buf.buf + PRINT_BUF_SIZE - print_buf.read;
			memcpy(buf,print_buf.buf,count1);
			count2 = print_buf.write - print_buf.buf;
			memcpy(&buf[count1],print_buf.buf,count2);
	//		print_buf.read = print_buf.write;
			print_buf.read = print_buf.read+count1+count2-PRINT_BUF_SIZE;
			printf("%s \r\n",buf);
		}
	}
}

int main(void)
{
	uint16_t val=0;
	uint8_t event=0;	
	spi_param_struct *pSPI_Param;
	key_param_struct *pKEY_Param;
 	
	pSPI_Param = GetSpiParam();
	pKEY_Param = GetKeyParam();
	
	RCC_Configuration();
	NVIC_Configuration();
	DebugInit();
	LEDInit();
	KEYInit();
	TimerInit();
	SPIx_Init();
	SPI_NSS_SET(0);
	ClearSystemReportInterrupt();
	print_buf.write = print_buf.buf;
	print_buf.read = print_buf.buf;
	while(1)
	{
		PrintfLoop();
		if(key_loop())
		{
            val = pKEY_Param->val;

            pSPI_Param->reg[KEY_HIGH] = (uint8_t)(val>>8);
            pSPI_Param->reg[KEY_LOW] = (uint8_t)(val&0xff);
            printf("key value %x\n",val);
            pKEY_Param->event ^= 1;
         /*
			if(pKEY_Param->event == 0)
			{
				val = pKEY_Param->val;

				pSPI_Param->reg[KEY_HIGH] = (uint8_t)(val>>8);
				pSPI_Param->reg[KEY_LOW] = (uint8_t)(val&0xff);
				pKEY_Param->event = 1;
				StartKeyEventTimer();
				pSPI_Param->reg[SPI_REG_INT] |= 0x01;

				printf("key value %x\n",val);
			}*/
		}
		if(event!=pKEY_Param->event)
		{
			//if(pKEY_Param->event)
			{
				SystemReportInterrupt();//产生外部中断
			}
		
			event = pKEY_Param->event;
          //  printf("event= %d\n",event);
		}
	}
}


