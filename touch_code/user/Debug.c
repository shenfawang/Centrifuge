#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "string.h"
#include "usart.h"
#include "stdio.h"
#include "Debug.h"
#include "misc.h"
#include "delay.h"
#include "spi.h"
uart_data_struct debug_data;
void DebugInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);

  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);



/* USART1 configuration ------------------------------------------------------*/
  // USART1 configured as follow:
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure the USART1 */
  USART_Init(USART1, &USART_InitStructure);

  /* Enable the USART Transmoit interrupt: this interrupt is generated when the 
     USART1 transmit data register is empty */  
  //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

  /* Enable the USART Receive interrupt: this interrupt is generated when the 
     USART1 receive data register is not empty */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
	
	memset(&debug_data,0,sizeof(debug_data));
	
}

void uart_loop(void)
{
	if(debug_data.flag)
	{
		char buf[UART_BUF_SIZE];
		delay_ms(20);
		//
		memset(buf,0,UART_BUF_SIZE);
		memcpy(buf,debug_data.buf,debug_data.count);
		debug_data.flag = 0;
		if(strncmp(buf,"DRAW1",5)==0)
		{
			printf("DRAW1");
		}
	}
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	   //ÅĞ¶Ï¶Á¼Ä´æÆ÷ÊÇ·ñ·Ç¿Õ
	{	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
		
     if(debug_data.flag == 0)
       debug_data.count = 0;
     debug_data.flag = 1;
     debug_data.buf[debug_data.count++] = USART_ReceiveData(USART1);
     if(debug_data.count>UART_BUF_SIZE)
       debug_data.count = 0;		
	}
}

/*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
	int count=0;
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (u8) ch);
  
  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
		if(count++>2000)
			break;
  }

  return ch;
}
