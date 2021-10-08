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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//���ļ�
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************

void STM32_Init(void)
{
    delay_init();            //��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	  Buzzer_Init();
    LED_Init();             //��ʼ����LED���ӵ�Ӳ���ӿ�
    Lcd_Init();             //��ʼ��LCD
		Adc_Init();
		DHT11_Init();
		GP2Y_LED_Init();
		SGP30_Init();
    uart1_init(115200);     //���ڳ�ʼ��Ϊ115200
    W25QXX_Init();      //W25QXX��ʼ��
    uart3_init(9600);       //���ڳ�ʼ��Ϊ115200
}

#define MODE                  NB_TCP           //���紫��ģʽ
//#define ONENET_SERVER                  "api.heclouds.com"	            //OneNet������
#define ONENET_SERVER                  "218.201.45.2"	            //OneNet������
#define ONENET_PORT                    80        	                    //OneNet ����Э���µ�Httpģʽ�˿�
//#define DEVICES_ID                     "688513636"                        //�豸ID
//#define API_KEY                        "nta46MSn6r63T=YaQNuMvQeR9JE="    //APIKEY

#define DEVICES_ID                     "695434956"                        //�豸ID
#define API_KEY                        "Cwpo94DkFHpiizR56HyvRRpsKWo="    //APIKEY

int main(void)
{
    char buff[150]={0};
    u16 count=0;
    float pm_25=0;
    u8 t,h;
		u16 ch2o,o3,co;
    STM32_Init();//ϵͳ��ʼ��
    NB_init(MODE, ONENET_SERVER, ONENET_PORT);//NBģ���ʼ

    while(1)
    {
        //���ڷ���������ģ��״̬
        if((count%2000)==0)
        {
            NB_CycleCheck();
        }

        //����Ȼ���ͷ�����
				
        if(0==(count%3000))
        {
            //��ȡ��ʪ��
            if(DHT11(&t,&h)==0)
            {
                sprintf(buff, "�¶�:%d��", t);
                Gui_DrawFont_GBK16(0,0,BLUE,WHITE,buff);

                sprintf(buff, "ʪ��:%d%",h);
                Gui_DrawFont_GBK16(0,20,BLUE,WHITE,buff);
            }

						//PM2.5 ��λmg/m3
            pm_25=GP2Y_datajudge();//ȡ��
						sprintf(buff, "PM2.5:%.1f",pm_25);
						Gui_DrawFont_GBK16(0,40,BLUE,WHITE,buff);
						//��ȩ  ��λppb
						ch2o = SGP30_datajudge();
						sprintf(buff, "CH2O:%dppb",ch2o);
						Gui_DrawFont_GBK16(0,60,BLUE,WHITE,buff);
						//һ����̼
						co = MQ7_datajudege();
						sprintf(buff, "CO:%dppm",co);
						Gui_DrawFont_GBK16(0,80,BLUE,WHITE,buff);
						//����
						o3 = MQ131_datajudege();
						sprintf(buff, "O3:%dppm",o3);
						Gui_DrawFont_GBK16(0,100,BLUE,WHITE,buff);
						
            
        }
				
				if(0==(count%10000))
				{
					//��ʽ���ϴ�������ΪJSON��ʽ
					sprintf((char*)buff,"{\"temp\":%d,\"hum\":%d,\"pm25\":%.1f,\"CH2O\":%d,\"CO\":%d,\"O3\":%d}",t,h,pm_25,ch2o,co,o3);
					HTTP_Post_OneNet(DEVICES_ID,API_KEY,buff);    //POST���ݵ�OneNet						
				}
        //���ƣ���ʾ����������
        if((count%500)==0) LED1=!LED1;

        //��⴮�ڣ��鿴�Ƿ�����������
        NB_GetData();

				delay_ms(1);
				count++;
    }
}

