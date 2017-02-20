#ifndef		__USART_H
#define		__USART_H

#define         UART_BUF_SIZE   256

typedef struct UART_DATA_STRUCT
{
  uint8_t flag;
  uint8_t count;
  uint8_t buf[UART_BUF_SIZE];
}uart_data_struct;
#endif
