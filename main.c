#include "uart.h"
#include "terminal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"



int main(void){
	
	USART_setup(9600);
	printf("Terminal\r\n>");
	while(1);
	
}
 
