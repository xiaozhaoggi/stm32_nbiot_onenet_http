#include "adc.h"
#include "delay.h"
#include "math.h"
//CO��
#define CAL_PPM  10  // У׼������PPMֵ
#define RL	10  // RL��ֵ
static float R0 = 6.00; 	// Ԫ���ڽྻ�����е���ֵ

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PC5 2 3��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}				  
//���ADCֵ
//ch:ͨ��ֵ 
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(1);
	}
	return temp_val/times;
} 	 

void GP2Y_LED_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 
	GPIO_SetBits(GPIOC,GPIO_Pin_12);						
}
float GP2Y_datajudge()
{
	float pm;
	float ad;
	GP2Y_Low;
	delay_us(280);
	ad = Get_Adc(ADC_Channel_15);	
	delay_us(40);
	GP2Y_High;
	delay_us(9680);
	pm = 0.17*(ad/4096*3.3+0.9)-0.1; //��ѹ-�ҳ�ת��  ��λ mg/m3
	return pm;
}


void MQ131_PPM_Calibration(float RS)
{
	
  R0 = RS / pow(CAL_PPM / 36.577, 1 / -1.082f);
}

u16 MQ131_datajudege()
{
	u16 ad;
	float Vrl,RS,ppm;
	ad = Get_Adc_Average(ADC_Channel_13,10);
	Vrl = 3.3f * ad / 4096.f;
	Vrl = ( (float)( (int)( (Vrl+0.005)*100 ) ) )/100;
  RS = (3.3f - Vrl) / Vrl * RL;
	MQ131_PPM_Calibration(RS);
  ppm = 36.577f * pow(RS/R0, -1.082f);
	return ppm;
}

void MQ7_PPM_Calibration(float RS)
{
	
  R0 = RS / pow(CAL_PPM / 98.322, 1 / -1.458f);
}

u16 MQ7_datajudege()
{
	u16 ad;
	float Vrl,RS,ppm;
	ad = Get_Adc_Average(ADC_Channel_12,10);
  Vrl = 3.3f * ad / 4096.f;
	Vrl = ( (float)( (int)( (Vrl+0.005)*100 ) ) )/100;
  RS = (3.3f - Vrl) / Vrl * RL;
	MQ7_PPM_Calibration(RS);
  ppm = 98.322f * pow(RS/R0, -1.458f);
	
	return ppm;
}

 // ������У׼����




















