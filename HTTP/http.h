#ifndef __HTTP_H
#define __HTTP_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//ESP8266��AT�����ʵ�֣������޸�WiFiΪAPģʽ��STAģʽ��������WiFi������TCP/UDPģʽ
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


typedef enum _http_request_state_{
    HTTP_REQUEST_IDLE=0,//����
    HTTP_REQUEST_POST,//������POSE����
    HTTP_REQUEST_GET,//������GET����
    HTTP_REQUEST_Register,//������ע������
    HTTP_REQUEST_Triggert,//�����˴�������
} HTTP_REQUEST_STATE;


extern u8 NB_RxData(u8* data, u16 len);
extern u8 HTTP_Post_OneNet(char *devices_id,char *api_key,char *value);

#endif
