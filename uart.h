#ifndef __UART_H__
#define __UART_H__
#include <stdio.h>

void USART_setup(unsigned long int baudrate);
int USART_putchar (int c);
//int putbuf (char c);
#endif

#ifdef __UART_C__
#define USART3_RECEIVED (USART3->SR & 0X20)
#define USART3_SENT (USART3->SR & 0x80)
#define USART3_INTERRUPT_NO (1 << (USART3_IRQn-32))

#define OLEN 128 // size of serial transmission buffer
unsigned char ostart=0; // transmission buffer start index
unsigned char oend=0; // transmission buffer end index
char outbuf[OLEN]; // storage for transmission buffer

void putbuf (char c);
int sendfull=0;
int sendactive=0;
 // flag: marks transmitter active

#endif
