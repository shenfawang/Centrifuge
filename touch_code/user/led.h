#ifndef         __LED_H
#define         __LED_H
#include "stm32f10x.h"

void LEDInit(void);
void LED_Draw_s(uint8_t state);
void LED_Return_s(uint8_t state);
void LED_Caution_s(uint8_t state);
void LED_Warnning_s(uint8_t state);
void LED_Stop(uint8_t state);
void LED_Return(uint8_t state);
void LED_Pause(uint8_t state);
void LED_Modify(uint8_t state);
void LED_Draw(uint8_t state);
void LED_Prime(uint8_t state);
#endif
