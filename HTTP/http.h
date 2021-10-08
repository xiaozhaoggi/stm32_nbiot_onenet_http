#ifndef __HTTP_H
#define __HTTP_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//ESP8266　AT命令交互实现，可以修改WiFi为AP模式，STA模式，可以让WiFi工作于TCP/UDP模式
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************


typedef enum _http_request_state_{
    HTTP_REQUEST_IDLE=0,//空闲
    HTTP_REQUEST_POST,//发起了POSE请求
    HTTP_REQUEST_GET,//发起了GET请求
    HTTP_REQUEST_Register,//发起了注册请求
    HTTP_REQUEST_Triggert,//发起了触发请求
} HTTP_REQUEST_STATE;


extern u8 NB_RxData(u8* data, u16 len);
extern u8 HTTP_Post_OneNet(char *devices_id,char *api_key,char *value);

#endif
