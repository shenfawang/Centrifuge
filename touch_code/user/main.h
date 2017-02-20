#ifndef		__MAIN_H
#define		__MAIN_H
#include "stm32f10x.h"
#define     STRING_BUF_SIZE   64  
#define     data_size   16

#define	PRINT_BUF_SIZE	1024

typedef struct PIRNT_BUF_STRUCT
{
	char buf[PRINT_BUF_SIZE];	
	char *write;
	char *read;
}print_buf_struct;

void SystemReportInterrupt(void);
void PrintfInterruptParam(uint8_t type,uint8_t *buf,uint8_t len);
void PrintfParam(char *buf,uint8_t *temp,uint8_t num);
void PrintfWrite(char *buf);
#endif
