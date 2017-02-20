#ifndef		__TIMER_H
#define		__TIMER_H
#include "stm32f10x.h"
#define		SECONDS_HZ			1000

typedef struct TIMER_S
{
  	uint8_t flag;
		uint8_t times;
		uint16_t set_val;
		uint16_t count;
}TIMER_STRUCT;

void TimerInit(void);
void StartKeyIntTimer(uint16_t val);
void StartKeyEventTimer(void);
void StopKeyEventTimer(void);

#endif
