#include "adc.h"
#include "uart.h"
#define SAMPLEMAXSIZE 128

uint16_t SampleBuffer[SAMPLEMAXSIZE];
float FilterBuffer[SAMPLEMAXSIZE];
int SampleIndex = 0;
uint8_t flag_sr = 0;
uint8_t flag_sc = 0;
uint8_t flag_fl = 0;
uint8_t flag_ma = 0;
uint8_t flag_fad = 0;
uint8_t flag_limsat = 0;
uint8_t flag_valork = 0;


int SampleSize = 0;
float FW = 0.0;
float KP = 0.0;
float KI = 0.0;
float KD = 0.0;
float YR = 0.0;
float y_ant = 0.0;
float e_ant =0.0;
float u_d_ant = 0.0;
float KDF = 0.0;
float KF = 0.0;
float U_sat_a = 0.0;
float U_sat_b = 0.0;
float U_0 = 0.0;


void analogInitCont(){
	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	        /* Configure NVIC */
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_Init(&NVIC_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  //ADC_Cmd(ADC1, ENABLE);
}

void ADC_IRQHandler(void){
	//ADC_SoftwareStartConv(ADC1);
}

void analogStart(){
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	while( ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	//ADC_ContinuousModeCmd(ADC1, ENABLE);
	TIM_Cmd(TIM3, ENABLE);                                      // Ready, Set, Go!
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                     // Enabling TIM3 Ch.1 interrupts
	
}

void analogStop(){
	//ADC_ContinuousModeCmd(ADC1, DISABLE);
	ADC_Cmd(ADC1, DISABLE);
	TIM_Cmd(TIM3, DISABLE);                          
  TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);  
	printf("stop\r\n");
}

void TIM3_Config(uint32_t period, uint16_t prescaler)
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
}

void putBuffer(uint16_t val){
	 SampleBuffer[SampleIndex] = val;
	 SampleIndex  = (SampleIndex +1)%SampleSize;
}

void putFilterBuffer(float val){
   FilterBuffer[SampleIndex] = val;
	 SampleIndex  = (SampleIndex +1)%SampleSize;

}

void getValue(){
	uint16_t val = ADC_GetConversionValue(ADC1);
	putBuffer(val);
	//printf("%d,",val);
}

void clearBuffer(){
	SampleIndex = 0;
}


void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
		
		/*if(flag_sr == 1){
			if(SampleIndex == SampleSize-1){
				analogStop();
				flag_sr = 0;
				if(flag_fl == 0)
					getValue();
				else if(flag_fl == 1)
					getValueFilter();
				TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
				//printf("\r\n>");
				return;
			}
		}
		if(flag_fl == 0)
			getValue();
		else if(flag_fl == 1)
			getValueFilter();
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); */
		
		if(flag_fad == 1)
			f_ad();
		else if(flag_limsat == 1)
			lim_sat();
		else if(flag_valork == 1)
			valor_k();
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
		
		
		
		
  }
	
	
	
	
}

void setBufferSize(int size){
	SampleSize = size;
}

void getValueFilter()
{
  float x = 0.0;
	x = (float)ADC_GetConversionValue(ADC1);
	float y = 0.0;
	y = y_ant*(1-FW) + x*FW;
	//printf("x= %f\r\n y= %f\r\n y_= %f\r\n FW =%f\r\n",x, y, y_ant, FW);
	putFilterBuffer(y);
	y_ant = y;
	//printf("%f,",y);
}


void printAnalogBuffer(){

	if (flag_fl){
		for(int i = SampleIndex; i< SampleSize; i++){
			printf("%f\r\n",FilterBuffer[i]);
		}
		for(int i = 0; i< SampleIndex; i++){
			printf("%f\r\n",FilterBuffer[i]);
		}
	}
	else{
		for(int i = SampleIndex; i< SampleSize; i++){
			printf("%d\r\n",SampleBuffer[i]);
		}
		for(int i = 0; i< SampleIndex; i++){
			printf("%d\r\n",SampleBuffer[i]);
		}
	}
	printf("\r\n");
}


void printAnalogBufferVolts(){
	float temp = 0.0;
	float maxValue = 4095;
	float maxVolt = 3.3;
	for(int i = SampleIndex; i< SampleSize; i++){
		if (flag_fl)
			temp = FilterBuffer[i];
		else
			temp = SampleBuffer[i];
		temp *=maxVolt;
		temp /= maxValue;
		printf("%f\r\n",temp);
	}
	for(int i = 0; i< SampleIndex; i++){
		if (flag_fl)
			temp = FilterBuffer[i];
		else
			temp = SampleBuffer[i];
		temp *=maxVolt;
		temp /= maxValue;
		printf("%f\r\n",temp);
	}
	printf("\r\n");
}




float control_rule(){
    float Ymax = 209.4;
	  float Ymin = -209.4;
	  float yAD = 0.0;
	  float y = 0.0;
	 
	
	  yAD = (float)ADC_GetConversionValue(ADC1);
	
	  y = (yAD*Ymax) / (2 * yAD *0.5);
	  if(y > Ymax || y < Ymin)
			return 0.0;
	 
	 
	   return y;
}


void calc_error()
{
	float e = 0.0;
	float sum_e = 0.0;
	float u = 0.0;
  float y = 0.0;
	y = control_rule();
	
		
		e = YR - y;
	  sum_e = sum_e + e + e_ant;
	  u = KP*e + KI*sum_e + KD*(e-e_ant);
	  e_ant = e;
    
	printf("valor de u: %f\r\n",u);
}

void lowpass_PD()
{
	float u = 0.0;
  float e = 0.0;
	float sum_e = 0.0;
	float u_d = 0.0;
  float y = 0.0;
	y = control_rule();

	e = YR - y;
	sum_e = sum_e + e + e_ant;
	u_d = KDF*(e-e_ant)+KF*u_d_ant;
	u = KP*e + KI*sum_e + u_d;
	e_ant = e;
	u_d_ant = u_d;
	
	printf("valor de u: %f\r\n",u);
	
}

void calc_cmd()
{

  float u = 0.0;
  float e = 0.0;
	float sum_e = 0.0;
	float u_d = 0.0;
  float y = 0.0;
	y = control_rule();
	
	e = YR - y;
	
	if(flag_ma)
	{
	   sum_e = sum_e + e + e_ant;
		 u_d = KDF*(e-e_ant) + KF*u_d_ant;
		 u = KP*e + KI*sum_e + u_d;
		 e_ant = e;
		 u_d_ant = u_d;
	   printf("valor de u: %f\r\n",u);
	}
	else
		e_ant = e;
}



void lim_sum_error()
{
	float u = 0.0;
  float e = 0.0;
	float sum_e = 0.0;
	float u_d = 0.0;
  float y = 0.0;
   
	y = control_rule();
	
	e = YR - y;
	if(flag_ma)
	{
	   sum_e = sum_e + e + e_ant;
		 u_d = KDF*(e-e_ant) + KF*u_d_ant;
		 u = KP*e + KI*sum_e + u_d;
		 e_ant = e;
		 u_d_ant = u_d;
		
		  if( u> U_sat_a)
			{
			  u = U_sat_a;
				sum_e = sum_e - e - e_ant;
			}
			else if( u< U_sat_b)
			{
			   u = U_sat_b;
				 sum_e = sum_e - e - e_ant;
			}
		printf("valor de u: %f\r\n",u);
	
	}
	else 
		e_ant = e;
}	


void const_cmd()
{
  float u = 0.0;
  float e = 0.0;
	float sum_e = 0.0;
	float u_d = 0.0;
	float delta_u = 0.0;
  float y = 0.0;
   
	y = control_rule();

   e = YR - y;
	
	if(flag_ma)
	{
	  sum_e = sum_e + e + e_ant;
		u_d = KDF*(e - e_ant) +KF*u_d_ant;
		e_ant = e;
		u_d_ant = u_d;
		delta_u = KP*e + sum_e + u_d;
		u = U_0 + delta_u;
		if(u > U_sat_a)
		{
		   u = U_sat_a;
			 sum_e = sum_e - e - e_ant;
		}
		else if(u < U_sat_b)
		{
		   u = U_sat_b;
			 sum_e = sum_e - e - e_ant;
		}
	  
		printf("valor de u: %f\r\n",u);
	
	}
	else 
		e_ant = e;
	 
}


