#ifndef _ADC_H_
#define _ADC_H_
#include "gpio.h"
#include "stm32f4xx_tim.h"

extern uint8_t flag_sr;
extern uint8_t flag_sc;
extern uint8_t flag_fl;
extern uint8_t flag_ma;
extern uint8_t flag_fad;
extern uint8_t flag_limsat;
extern uint8_t flag_valork;



extern float FW;
extern float KP;
extern float KI;
extern float KD;
extern float YR;
extern float y_ant;
extern float e_ant;
extern float sum_ant;
extern float KDF;
extern float KF;
extern float u_d_ant;
extern float U_sat_a;
extern float U_sat_b;
extern float U_0;


void analogInitCont(void);
void analogStart(void);
void analogStop(void);

void setBufferSize(int size);
void clearBuffer(void);
void TIM3_Config(uint32_t period, uint16_t prescaler);
void getValueFilter(void);

void printAnalogBuffer(void);
void printAnalogBufferVolts(void);


float control_rule(void);
void calc_error(void);
void lowpass_PD(void);
void calc_cmd(void);
void lim_sum_error(void);
void const_cmd(void);


#endif
