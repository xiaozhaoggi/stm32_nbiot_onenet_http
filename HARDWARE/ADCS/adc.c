#include "adc.h"
#include "delay.h"
#include "math.h"
//CO用
#define CAL_PPM  10  // 校准环境中PPM值
#define RL	10  // RL阻值
static float R0 = 6.00; 	// 元件在洁净空气中的阻值

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PC5 2 3作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  
//获得ADC值
//ch:通道值 
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
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
	pm = 0.17*(ad/4096*3.3+0.9)-0.1; //电压-灰尘转换  单位 mg/m3
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

 // 传感器校准函数




















