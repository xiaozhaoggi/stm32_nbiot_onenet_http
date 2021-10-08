#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "usart.h"
#include "usart3.h"
#include "w25qxx.h"
#include "nb.h"
#include "http.h"
#include "DHT11.h"
#include "buzzer.h"
#include "sgp30.h"
#include "adc.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//主文件
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************

void STM32_Init(void)
{
    delay_init();            //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	  Buzzer_Init();
    LED_Init();             //初始化与LED连接的硬件接口
    Lcd_Init();             //初始化LCD
		Adc_Init();
		DHT11_Init();
		GP2Y_LED_Init();
		SGP30_Init();
    uart1_init(115200);     //串口初始化为115200
    W25QXX_Init();      //W25QXX初始化
    uart3_init(9600);       //串口初始化为115200
}

#define MODE                  NB_TCP           //网络传输模式
//#define ONENET_SERVER                  "api.heclouds.com"	            //OneNet服务器
#define ONENET_SERVER                  "218.201.45.2"	            //OneNet服务器
#define ONENET_PORT                    80        	                    //OneNet 公有协议下的Http模式端口
//#define DEVICES_ID                     "688513636"                        //设备ID
//#define API_KEY                        "nta46MSn6r63T=YaQNuMvQeR9JE="    //APIKEY

#define DEVICES_ID                     "695434956"                        //设备ID
#define API_KEY                        "Cwpo94DkFHpiizR56HyvRRpsKWo="    //APIKEY

int main(void)
{
    char buff[150]={0};
    u16 count=0;
    float pm_25=0;
    u8 t,h;
		u16 ch2o,o3,co;
    STM32_Init();//系统初始化
    NB_init(MODE, ONENET_SERVER, ONENET_PORT);//NB模块初始

    while(1)
    {
        //周期发送命令，检测模块状态
        if((count%2000)==0)
        {
            NB_CycleCheck();
        }

        //周期然后发送服务器
				
        if(0==(count%3000))
        {
            //读取温湿度
            if(DHT11(&t,&h)==0)
            {
                sprintf(buff, "温度:%d℃", t);
                Gui_DrawFont_GBK16(0,0,BLUE,WHITE,buff);

                sprintf(buff, "湿度:%d%",h);
                Gui_DrawFont_GBK16(0,20,BLUE,WHITE,buff);
            }

						//PM2.5 单位mg/m3
            pm_25=GP2Y_datajudge();//取出
						sprintf(buff, "PM2.5:%.1f",pm_25);
						Gui_DrawFont_GBK16(0,40,BLUE,WHITE,buff);
						//甲醛  单位ppb
						ch2o = SGP30_datajudge();
						sprintf(buff, "CH2O:%dppb",ch2o);
						Gui_DrawFont_GBK16(0,60,BLUE,WHITE,buff);
						//一氧化碳
						co = MQ7_datajudege();
						sprintf(buff, "CO:%dppm",co);
						Gui_DrawFont_GBK16(0,80,BLUE,WHITE,buff);
						//臭氧
						o3 = MQ131_datajudege();
						sprintf(buff, "O3:%dppm",o3);
						Gui_DrawFont_GBK16(0,100,BLUE,WHITE,buff);
						
            
        }
				
				if(0==(count%10000))
				{
					//格式化上传的数据为JSON格式
					sprintf((char*)buff,"{\"temp\":%d,\"hum\":%d,\"pm25\":%.1f,\"CH2O\":%d,\"CO\":%d,\"O3\":%d}",t,h,pm_25,ch2o,co,o3);
					HTTP_Post_OneNet(DEVICES_ID,API_KEY,buff);    //POST数据到OneNet						
				}
        //闪灯，表示程序在运行
        if((count%500)==0) LED1=!LED1;

        //检测串口，查看是否有主动数据
        NB_GetData();

				delay_ms(1);
				count++;
    }
}

