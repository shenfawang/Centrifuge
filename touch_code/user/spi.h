#ifndef         __SPI_H
#define         __SPI_H
#include "stm32f10x.h"
#define		SPI_REG_MAX				16
#define   KEY_LOW 					0x00
#define   KEY_HIGH					0x01
#define   LCD_PWM						0x02
#define   LED_LOW           0x03
#define   LED_HIGH          0x04
#define		SPI_REG_INT				0x08	//中断状态	bit0 按键中断
#define   SLAVE_ID          0x0a      

typedef struct SPI_PARAM_STRUCT
{
	uint8_t state;	//0:addr ,1:data
	uint8_t read_write;	//0 :write  . 1:read
	uint8_t addr;
	uint8_t reg[SPI_REG_MAX];
	uint8_t write_enable[SPI_REG_MAX];	//0:对应reg 不可以写    1：对应REG可以写
	uint8_t event;
}spi_param_struct;

void SPIx_Init(void);
void SPIx_SetSpeed(uint8_t SpeedSet);
uint8_t SPIx_ReadWriteByte(uint8_t TxData);
uint8_t SPIx_ReadByte(void);
void SPIxWriteByte(uint8_t RxData);
void EXIT_NSS_INIT(void);
void SPI_NSS_SET(uint8_t state);
spi_param_struct *GetSpiParam(void);
#endif
