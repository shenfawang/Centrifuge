#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "timer.h"
#include "spi.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "key.h"

TIMER_STRUCT KeyIntTimer,KeyEventTimer;

char PrintBuff[64];

void TimerInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);// | RCC_APB2Periph_TIM1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				//PB0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Enable timer3 clock   */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_DeInit(TIM3); //reset timer
	TIM_TimeBaseStructure.TIM_Period= 1000; //timer preiod 1 ms
	TIM_TimeBaseStructure.TIM_Prescaler=(72-1); //Timer prescaler ,the timer clock 1Mhz
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //  div is one
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //UP mode
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure); //
	
	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update); //clear timer it flags
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //enable interrupt 	
	TIM_Cmd(TIM3,ENABLE);
	
	KeyIntTimer.set_val = SECONDS_HZ/10;
	KeyIntTimer.flag = 0;
	KeyIntTimer.count = 0;
	
	KeyEventTimer.set_val = SECONDS_HZ/2;
	KeyEventTimer.flag = 0;
	KeyEventTimer.count = 0;

	TIM_TimeBaseStructure.TIM_Period = 1000;//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 160hz
	TIM_TimeBaseStructure.TIM_Prescaler = (72-1); //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;      // 采样分频      
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 500; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

//	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIMx在CCR3上的预装载寄存器
	
//	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	//死区设置
/*	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStructure.TIM_DeadTime = 0;  //这里调整死区大小0-0xff
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure); */
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	TIM_SetCompare1(TIM1,0);	  //输出PWM	   
}

void StartKeyIntTimer(uint16_t val)
{
	if(KeyIntTimer.flag == 0)
	{
		KeyIntTimer.flag = 1;
		KeyIntTimer.set_val = val;
		KeyIntTimer.count = 0;
	}
}

void StartKeyEventTimer(void)
{
	if(KeyEventTimer.flag == 0)
	{
		KeyEventTimer.flag = 1;
		KeyEventTimer.count = 0;
	}
}

void StopKeyEventTimer(void)
{
	KeyEventTimer.flag = 0;
	KeyEventTimer.count = 0;
}

void TIM3_IRQHandler(void)//1MS period
{
	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) {
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);//clear flag
		if(KeyIntTimer.flag)
		{
			KeyIntTimer.count++;
			if(KeyIntTimer.count>=KeyIntTimer.set_val)
			{	
				key_param_struct *pKEY_Param;
				pKEY_Param = GetKeyParam();
				pKEY_Param->event = 1;
				StartKeyEventTimer();
				KeyIntTimer.flag = 0;
				KeyIntTimer.count = 0;
			}
		}
	/*	if(KeyEventTimer.flag)
		{
			KeyEventTimer.count++;
			if(KeyEventTimer.count>=KeyEventTimer.set_val)
			{
				key_param_struct *pKEY_Param;
				spi_param_struct *pSPI_Param;
				pKEY_Param = GetKeyParam();
				pSPI_Param = GetSpiParam();
				KeyEventTimer.count = 0;
				KeyEventTimer.flag = 0;
				if(pKEY_Param->event == 1)
				{
//					printf("key event over time \r\n");
					pKEY_Param->event = 0;
					pSPI_Param->reg[KEY_HIGH] = 0;
					pSPI_Param->reg[KEY_LOW] = 0;
				}
			}
		}
        */
		
	}
}

