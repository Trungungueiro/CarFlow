#include "terminal.h"
#include "uart.h"
#include "eeprom.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
#define COMANDOS_SIZE 42
#define BUFFER_SIZE 25

#define CR_KEY 0x0D
#define BCKSP_KEY 0x08
#define ESC_KEY 0x1B
#define BETA_KEY 0x20
#define SRAM_ADDRESS 0x20000000

uint16_t uhADCxConvertedValue = 0;
short lastCom = -1;

char BUFFER[BUFFER_SIZE];
int SIZE = 0;
int ARGS[4];

struct cmd comando[] = {

	{ "?",0,help,"Help" },
	{ "VER",0,ver,"version" },
	{ ".",0,stop,"STOP" },
	{ "RP",2,rp,"Ramp PWM" },
	{ "DC",1,dcr,"Duty Cycle" }
};

void input(char c) {
	
	switch (c){
  	case CR_KEY:
			USART_putchar('\r');
			USART_putchar('\n');
			BUFFER[SIZE] = '\0';
			SIZE++;
			parse();
			USART_putchar('>');
			SIZE = 0;
  		break;
  	case BCKSP_KEY:
			if(SIZE!=0){
				SIZE--;
				USART_putchar(0x08);
				USART_putchar(' ');
				USART_putchar(0x08);
			}
  		break;
		case ESC_KEY:
			SIZE=0;
			USART_putchar(0x0C);
			USART_putchar('>');
			break;
  	default:
			USART_putchar(c);
			BUFFER[SIZE] = c;
			SIZE++;
  		break;
  }
}

void parse() {
	short x;
	short argSize;
	//comando p[];
	/*for (int i = 0; i < SIZE; i++) {
		printf("%c", BUFFER[i]);
	}*/

	x = getCom();
	if (x == -1) {
		printf("ERRO: Comando Desconhecido.\r\n");
		return;
	}

	if (comando[x].args > 0){
		argSize = getArgs();
		if (argSize < comando[x].args) {
			printf("ERRO: Args Insuficientes.\r\n");
			return;
		}
		else if (argSize > comando[x].args) {
			printf("ERRO: Demasiados Args.\r\n");
			return;
		}
	}
	
	comando[x].fn();
	if (x != 10)
		lastCom = x;
}

short getCom() {
	char com[5];
	int i = 0;

	while (BUFFER[i] != BETA_KEY && BUFFER[i] != '\0') {
		com[i] = BUFFER[i];
		i++;
		if (i == 5)
			return -1;
	}

	int isequal;
	for (int j = 0; j < COMANDOS_SIZE; j++) {
		isequal = 0;
		for (int k = 0; k < i; k++)
			if (com[k] == comando[j].com[k]) {
				isequal++;
			}	
		if (isequal == i)
			return j;
	}
	return -1;
}

int getArgs() {
	char argBuffer[15];
	int j = 0;
	int argSize = 0;

	for (int i = 3; i < SIZE; i++) {
		if (BUFFER[i] == BETA_KEY || BUFFER[i] == '\0' || BUFFER[i] == '.') {
			ARGS[argSize] = convBase(argBuffer, j);
			if(ARGS[argSize] == -1)
				return -1;
			argSize++;
			j = 0;
			if (BUFFER[i] == '.') {
				argBuffer[j] = BUFFER[i];
				j++;
			}
		}
		else {
			argBuffer[j] = BUFFER[i];
			j++;
		}
	}
	return argSize;
}

int convBase(char arg[], int argSize) {
	int res = 0;

	switch (arg[0])
	{
	case 'H':
		for (int i = 1; i < argSize; i++) {
			res *= 16;
			if (arg[i] > '9')
				res += arg[i] - 'A' + 10;
			else
				res += arg[i] - '0';
		}
		break;
	case '+':
		signal = 1;
	case '-':
	case 'D':
	case '.':
		for (int i = 1; i < argSize; i++) {
			res *= 10;
			res += arg[i] - '0';
		}
		break;
	case 'B':
		for (int i = 1; i < argSize; i++) {
			res *= 2;
			res += arg[i] - '0';
		}
		break;
	case 's':
	case 'm':
	case 'u':
		res = arg[0];
	break;
	default:
		printf("ERRO: Conversao da Base.\r\n");
		return -1;
	}
	return res;
}

/*FUNCTIONS*/


	

void mw(){
	uint32_t *ptr;
	ptr = (uint32_t *) (ARGS[0]+SRAM_ADDRESS);
	
	while(ARGS[1] != 0)
	{
		printf("%X=%d\r\n", ptr, *ptr);
		*ptr = ARGS[2];
		printf("%X=%d\r\n", ptr, *ptr);
	  ARGS[1]--;
		(*ptr)++;
	}	
}
void mc(){
  uint32_t *m1;
  uint32_t *m2;
  m1 = (uint32_t *)(ARGS[0]+SRAM_ADDRESS);
  m2 = (uint32_t *)(ARGS[2]+SRAM_ADDRESS);
 
	while(ARGS[1] != 0)
	{
	printf("%X=%d,%X=%d\r\n", m1, *m1, m2, *m2); 	
	*m2 = *m1;
  printf("%X=%d,%X=%d\r\n", m1, *m1, m2, *m2); 	
	ARGS[1]--;
	m1++;
	m2++;
	}
}
void ri(){
	uint32_t p;
	uint32_t val;
	p= ARGS[0]+SRAM_ADDRESS;
	
	printf(" Memoria %d=", p);
	val=*(uint32_t*)p;
	printf("%d\r\n", val);
}
void wi(){
	uint32_t *ptr;
	ptr = (uint32_t *) (ARGS[0]+SRAM_ADDRESS);
	printf("%d=%d\r\n", ptr, *ptr);
	*ptr = (uint32_t)ARGS[1];
	printf("%d=%d\r\n", ptr, *ptr);
}
	
void rd(){
	if (ARGS[0]>10){
		printf("ERRO: PORTO nao existente\r\n");
		return;
	}
	if (ARGS[1]>15){
		printf("ERRO: PIN nao existente\r\n");
		return;
	}
	portInit(ARGS[0]);
	pinInit(ARGS[0],ARGS[1],GPIO_Mode_IN);
	
	uint8_t val = digitalRead(ARGS[0],ARGS[1]);
	printf("Valor do Pin %c%d = %x \r\n", ARGS[0]+'A', ARGS[1], val);
}

void wd(){
	if (ARGS[0]>10){
		printf("ERRO: PORTO nao existente.\r\n");
		return;
	}
	if (ARGS[1]>15){
		printf("ERRO: PIN nao existente.\r\n");
		return;
	}
	if (ARGS[2]>1){
		printf("ERRO: Valores possives 0-1.\r\n");
		return;
	}
	
	portInit(ARGS[0]);
	pinInit(ARGS[0],ARGS[1],GPIO_Mode_OUT);
	
	digitalWrite(ARGS[0],ARGS[1],ARGS[2]);
	printf("Valor do Pin %c%d = %d \r\n", ARGS[0]+'A', ARGS[1], ARGS[2]);
}

void ra(){
	if (ARGS[0]>15){
		printf("ERRO: CHANNEL nao existente\r\n");
		return;
	}
	analogCommonInit();
	analogInit(ADC1);
	analogChannelInit(ADC1, ARGS[0]);
	
	uint16_t val = analogRead(ADC1);
	float vi = val * 0.00080586;
	printf("Valor do 0x%03X \r\n",  val);
	printf("Tensao = %f V \r\n",  vi);
}

void last(){
	if(lastCom != -1)
		comando[lastCom].fn();
	else
		printf("ERRO: Comando Indisponivel.");
}
void help(){
	static int i=0;
	int pageSize = 10;
	int j=pageSize;
	printf("Centro De Ajuda:%d\r\n",i);
	if( i>(COMANDOS_SIZE-pageSize) )
		j=COMANDOS_SIZE;
	else j+=i;
	
	for( ;i<j; i++){
		printf("%s\t-\t%s\r\n",comando[i].com, comando[i].help);
	}
	if(i>=COMANDOS_SIZE)
		i=0;
}

void ver(){

	printf("v0.8 grupo 11 LPI-II DEI UM2017\r\n");
	
}

void sp() {
	 if(ARGS[0] != 's' && ARGS[0] != 'u' && ARGS[0] != 'm')
		 return;
	  if(ARGS[1] < 0)
      return;

		if(ARGS[0] == 's'){
			TIM3_Config(2000*ARGS[1],50000);
		}
    else if(ARGS[0] == 'm'){
			  TIM3_Config(20*ARGS[1],5000);
		}
		else{
			TIM3_Config(2*ARGS[1],50);
		}
}

void ac() {

    if (ARGS[0]>15){
			printf("ERRO: CHANNEL nao existente\r\n");
		return;
		}
			
		analogCommonInit();
		analogInitCont();
	  analogChannelInit(ADC1, ARGS[0]);
}

void bl() {
		setBufferSize(ARGS[0]);
}

void sr() {
	clearBuffer();
	analogStart();
	flag_sr = 1;
}


void sc (){
	clearBuffer();
	analogStart();
	flag_sc = 1;
}

void stop () {
	if(flag_sc == 1){
		analogStop();
		flag_sc = 0;
	}
}


void rp(){
	 if(ARGS[0] != 's' && ARGS[0] != 'u' && ARGS[0] != 'm')
		 return;
	  if(ARGS[1] < 0)
      return;
		PWMTimConfig(2*100,50);
		PWMPinConfig();
		if(ARGS[0] == 's'){
			TIM4_Config(2000*ARGS[1],50000);
		}
    else if(ARGS[0] == 'm'){
			TIM4_Config(20*ARGS[1],5000);
		}
		else{
			TIM4_Config(2*ARGS[1],50);
		}
}
	
void dcr(){
	if(ARGS[0] < 0 || ARGS[0] > 100)
      return;
	PWMChannelConfig(ARGS[0]);
	PWMStart();
	portInit(0);
	pinInit(0, 9, GPIO_Mode_OUT);
	signal = 0;
}

