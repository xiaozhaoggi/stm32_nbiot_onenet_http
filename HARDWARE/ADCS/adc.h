#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

#define GP2Y_High GPIO_SetBits(GPIOC,GPIO_Pin_12);
#define GP2Y_Low  GPIO_ResetBits(GPIOC,GPIO_Pin_12);

void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 

float GP2Y_datajudge();
void GP2Y_LED_Init();
u16 MQ7_datajudege();
u16 MQ131_datajudege();

#endif 
