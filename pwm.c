#include "pwm.h"
#include "gpio.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"

short signal = 0;
uint16_t TimerPeriod = 0;
int dutyCycle = 0;
int currentDutyCycle = 0;
uint8_t isGoingUp = 1;


void TIM4_Config(uint32_t period, uint16_t prescaler)
{
  NVIC_InitTypeDef NVIC_TIM3;                                 // NVIC structure
  TIM_TimeBaseInitTypeDef TIM3_TimeBase;                      // Time base structure
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        // Clocking TIM3  (APB1 = 42MHz)

  NVIC_TIM3.NVIC_IRQChannel                   = TIM3_IRQn;    // Specifying the channel (stm32f4xx.h)
  NVIC_TIM3.NVIC_IRQChannelPreemptionPriority = 0;            // Only matters for multiple interrupts
  NVIC_TIM3.NVIC_IRQChannelSubPriority        = 0;            // Only matters for multiple interrupts
  NVIC_TIM3.NVIC_IRQChannelCmd                = ENABLE;       // Enabling global interrupt
  NVIC_Init(&NVIC_TIM3);                                      // Initializing NVIC structure
	
	TIM3_TimeBase.TIM_ClockDivision = 0;                        // Not dividing
  TIM3_TimeBase.TIM_CounterMode   = TIM_CounterMode_Up;       // Upcounting configuration
  TIM3_TimeBase.TIM_Period        = period;                    // Autoreload value (ARR) 
  TIM3_TimeBase.TIM_Prescaler     = prescaler;            // Dividing APB1 by 2
  TIM_TimeBaseInit(TIM3, &TIM3_TimeBase);                     // Initializing Time Base structure

	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}
int a =0;
/*
void TIM3_IRQHandler(void){
	
	  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)             // Just a precaution (RESET = 0) 
  {
		if(currentDutyCycle != dutyCycle){
			if(currentDutyCycle < dutyCycle)
				currentDutyCycle ++;
			else if(currentDutyCycle > dutyCycle)
				currentDutyCycle--;
			int dc = currentDutyCycle;
			if (dc < 0){
				digitalWrite(0,9,0);
				dc *= -1;		
			}
			else digitalWrite(0,9,1);
			uint16_t ChannelPulse = (uint16_t) (((uint32_t) dc * (TimerPeriod - 1)) / 100);
			TIM1->CCR1 = ChannelPulse;
			
		}
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                  // Clear TIM3 Ch.1 flag
  }

}*/

void PWMPinConfig(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA Clocks enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* GPIOA Configuration: Channel 1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
}


void PWMTimConfig(uint32_t period, uint16_t prescaler){

	NVIC_InitTypeDef NVIC_TIM1;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* Compute the value to be set in ARR register to generate signal frequency at 17.57 Khz / 17570 */
  TimerPeriod = period-1;//( SystemCoreClock / freq ) - 1;

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
  
	//Interrupt
	NVIC_TIM1.NVIC_IRQChannel                   = TIM1_CC_IRQn;    // Specifying the channel (stm32f4xx.h)
  NVIC_TIM1.NVIC_IRQChannelPreemptionPriority = 0;            // Only matters for multiple interrupts
  NVIC_TIM1.NVIC_IRQChannelSubPriority        = 1;            // Only matters for multiple interrupts
  NVIC_TIM1.NVIC_IRQChannelCmd                = ENABLE;       // Enabling global interrupt
  NVIC_Init(&NVIC_TIM1); 
	
	
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

}

void TIM1_CC_IRQHandler(void){
	
	

		//printf("%d,",currentDutyCycle);

	
	/*
	if (isGoingUp){
		if (currentDutyCycle < dutyCycle)
			currentDutyCycle++;
		else isGoingUp = 0;
	}
	else {
		if (currentDutyCycle > 0)
			currentDutyCycle--;
		else isGoingUp = 1;
	}*/
}

void PWMChannelConfig(uint32_t dc){
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t ChannelPulse = 0;
	dutyCycle = dc;
	if (!signal) dutyCycle *= -1;
	ChannelPulse = (uint16_t) (((uint32_t) currentDutyCycle * (TimerPeriod - 1)) / 100);
	  /* ChannelConfiguration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ChannelPulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	
	isGoingUp = 1;
	//currentDutyCycle = 0;
	
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
}


void PWMStart(){
  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);
}

void PWMStop(){
  TIM_Cmd(TIM1, DISABLE);
  TIM_CtrlPWMOutputs(TIM1, DISABLE);
}
