#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "spi.h"
#include "timer.h"
#include "misc.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
extern uint16_t print_val;
spi_param_struct spi_param;
uint16_t LED_sta;

spi_param_struct *GetSpiParam(void)
{
	return (&spi_param);
}
void SPIx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE );        

/*	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;  //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
	SPI_Cmd(SPI1, DISABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;                //设置SPI工作模式:设置为主SPI        Master                Slave
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                //选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        //数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;                //定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;        //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;        //CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	EXIT_NSS_INIT();
	
//	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE);
	
//	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	
//	SPIx_ReadWriteByte(0xff);//启动传输      

	memset(&spi_param,0,sizeof(spi_param));
	spi_param.write_enable[KEY_LOW] = 0;
	spi_param.write_enable[KEY_HIGH] = 0;
	spi_param.write_enable[LCD_PWM] = 1;
	spi_param.write_enable[LED_LOW] = 1;
	spi_param.write_enable[LED_HIGH] = 1;
	spi_param.write_enable[SPI_REG_INT] = 1;
	spi_param.write_enable[SLAVE_ID] = 0;
	spi_param.reg[LED_LOW] = 0;
	spi_param.reg[LED_HIGH] = 0;
	spi_param.reg[SLAVE_ID] = 0xa5;
}   

void EXIT_NSS_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);   
	/* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;						//PE2 作为键盘的行线。检测状态
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;		    //上升和下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
}

void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)				  //
	{
	  if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 0)
		{
			SPI_NSS_SET(1);
		}
		else if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) != 0)
		{
			SPI_NSS_SET(0);
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

void SPI_NSS_SET(uint8_t state)
{
	if(state == 0)
	{
		spi_param.state = 0;
		SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,DISABLE);
	
		SPI_Cmd(SPI1, DISABLE); //使能SPI外设
	}
	else
	{
		spi_param.state = 0;
		SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE);
	
		SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	}
}
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8分频   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16分频  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256分频 (SPI 281.25K@sys 72M)
  
void SPIx_SetSpeed(uint8_t SpeedSet)
{
/*	SPI_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1,ENABLE);*/
} 

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPIx_ReadWriteByte(uint8_t TxData)
{                
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位                                      

  SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据        
	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); //检查指定的SPI标志位设置与否:接受缓存非空标志位
                                                            
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据        	
}


uint8_t SPIx_ReadByte(void)         //接收数据
{        
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); //检查指定的SPI标志位设置与否:接受缓存非空标志位
                                                            
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据          	                 
}



void SPIx_WriteByte(uint8_t RxData)                 //发送数据
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位                                      

  SPI_I2S_SendData(SPI1, RxData); //通过外设SPIx发送一个数据    
}

void WriteSPIREG(uint8_t addr,uint8_t data)
{
	// addr 一定要判断是否可写
	if(spi_param.write_enable[spi_param.addr])
	{
		
		spi_param.reg[spi_param.addr] = data;
		if((spi_param.addr == SPI_REG_INT)&&(spi_param.reg[spi_param.addr] == 0))
		{
            #if 0
			key_param_struct *pKEY_Param;
			pKEY_Param = GetKeyParam();
			spi_param.reg[KEY_HIGH] = 0;
			spi_param.reg[KEY_LOW] = 0;
			pKEY_Param->event = 0;
			print_val = 0;
			StopKeyEventTimer();
//			printf("key event in time \r\n");
            #endif            
		}
		else if((spi_param.addr == LED_LOW)||(spi_param.addr == LED_HIGH))
		{
			uint16_t LED1,LED2;
			LED_sta = (uint16_t)((uint16_t)spi_param.reg[LED_HIGH]<<8) |  spi_param.reg[LED_LOW];
			LED1 = (~LED_sta) & 0x0f;
			LED2 = LED_sta >>4;
			LED_sta = (LED2 << 4) | LED1;
			GPIO_Write(GPIOC,LED_sta);
//			printf("spi_param.addr is %x,spi_param.reg[spi_param.addr] is %x\n",spi_param.addr,spi_param.reg[spi_param.addr]);
//			printf("LED_sta is %x\n",LED_sta);
		}
		else if(spi_param.addr == LCD_PWM)
		{
			if(data>100)
				data = 100;
			TIM_SetCompare1(TIM1,(uint16_t)(data*10));
		}
	}
}


void SPI1_IRQHandler(void)
{         
	uint8_t buff;
	uint8_t temp[16];
	if(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == SET)
	{
		char buf[64];
		buff = SPI1->DR;
	//	printf("spi buff is %x \r\n",buff);
		if(spi_param.state == 0) //收到addr
		{
			spi_param.addr = buff>>1; //地址附值
			spi_param.read_write = buff&0x01;
			if(spi_param.read_write == 1)	// read
			{
				if(spi_param.addr<SPI_REG_MAX)
				{
					SPI1->DR = spi_param.reg[spi_param.addr];
					temp[0] = spi_param.addr;
					temp[1] = spi_param.reg[spi_param.addr];
				//	PrintfParam("sr",temp,2);
					memset(buf,0,sizeof(buf));
					sprintf(buf,"spi read: %x %x\r\n",temp[0],temp[1]);
					PrintfWrite(buf);
				}
			}
			else if(spi_param.read_write == 0)	//write
			{
				spi_param.state = 1;
			}
		}
		else //收到的是data
		{
				if(spi_param.addr >= SPI_REG_MAX)
					spi_param.addr = 0;
				WriteSPIREG(spi_param.addr,buff);
				temp[0] = spi_param.addr;
				temp[1] = spi_param.reg[spi_param.addr];
				memset(buf,0,sizeof(buf));
				sprintf(buf,"spi write: %x %x\r\n",temp[0],temp[1]);
				PrintfWrite(buf);
		}
	}      
}
