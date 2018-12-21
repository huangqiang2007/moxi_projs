#ifndef __UART_H
#define	__UART_H

#include "stm32f0xx.h"
#include <stdio.h>

void USART_Configuration(void);
int ffputc(uint8_t ch);
void ffputs(uint8_t str[]);
int fputc(int ch, FILE* stream);


#endif /* __UART_H */
