#ifndef		__DEBUG_H
#define		__DEBUG_H
#include "stdio.h"

void DebugInit(void);
int fputc(int ch, FILE *f);
void uart_loop(void);
#endif
