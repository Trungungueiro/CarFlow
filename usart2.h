#ifndef __USART2_H__
#define __USART2_H__


void USART2_setup(unsigned long int baudrate);
char USART2_getkey (void);
int USART2_putchar (int c);
void USART2_getstring(char * );

#endif

#ifdef __USART2_C__

#define USART2_RECEIVED (USART2->SR & 0X20)
#define USART2_SENT (USART2->SR & 0x80)
#define USART2_INTERRUPT_NO (1 << (USART2_IRQn-32))

#define XTAL 8000000 // CPU Oscillator Frequency

#define OLEN 128 // size of serial transmission buffer
unsigned char ostart; // transmission buffer start index
unsigned char oend; // transmission buffer end index
char outbuf[OLEN]; // storage for transmission buffer


#define ILEN 128 // size of serial receiving buffer
unsigned char istart; // receiving buffer start index
unsigned char iend; // receiving buffer end index
char inbuf[ILEN]; // storage for receiving buffer

int sendfull; // flag: marks transmit buffer full
int sendactive; // flag: marks transmitter active

#endif



