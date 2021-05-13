#ifndef _PWM_H_
#define _PWM_H_
#include "stm32f4xx.h" 

void PWMPinConfig(void);
void PWMTimConfig(uint32_t period, uint16_t prescaler);
void TIM4_Config(uint32_t period, uint16_t prescaler);
void PWMChannelConfig(uint32_t dutyCycle);

extern short signal;
void PWMStart(void);
void PWMStop(void);

#endif
