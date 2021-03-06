#include "buzzer.h"
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//蜂鸣器控制　使用PC1控制，低电平触发
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************


//功能：蜂鸣器初始化
//参数：on　1：响　0不响
//返回：无
void Buzzer_Init(void)
{ 
    GPIO_InitTypeDef  GPIO_InitStructure;
    		

    //buzzer
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC1端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PC1 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC1
    GPIO_SetBits(GPIOC,GPIO_Pin_1);						 //PC1输出高
}

//功能：控制蜂鸣器
//参数：on　1：响　0不响
//返回：无
void Buzzer_on(int on){
    if(on>0) BUZZER=0;
    else BUZZER=1;
}

//功能：控制蜂鸣器响一声
//参数：无
//返回：无
void Buzzer_flash(void){

    Buzzer_on(1);
    delay_ms(100);
    Buzzer_on(0);
}


 
