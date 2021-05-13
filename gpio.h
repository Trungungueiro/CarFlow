#ifndef __GPIO_H__
#define __GPIO_H__
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"

void portInit(int port);
void pinInit(int port, int pin, GPIOMode_TypeDef mode);
void pinInit_pull(int port, int pin, GPIOMode_TypeDef mode, GPIOPuPd_TypeDef pull);
void analogInit(ADC_TypeDef* adc);
void analogCommonInit(void);
void analogChannelInit(ADC_TypeDef* adc, int channel);

int getChannelPort(int channel);
int getChannelPin(int channel);

uint8_t digitalRead(int port, int pin);
void digitalWrite(int port, int pin, int value);
uint16_t analogRead(ADC_TypeDef* adc);
#endif
