#include <stdarg.h>
#include "http.h"
#include "nb.h"

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//ESP8266　AT命令交互实现，可以修改WiFi为AP模式，STA模式，可以让WiFi工作于TCP/UDP模式
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************

//http状态，只能idle过程中能发起请求
HTTP_REQUEST_STATE http_state=HTTP_REQUEST_IDLE;
u8 ontnet_tx_buff[500]={0};//请求数据的缓冲

//NB接收到的onenet数据处理
u8 NB_RxData(u8* data, u16 len)
{
    char *response_p1=NULL;
    u8 result=0;

    if(data==NULL) return 0;
    if(len==0) return 0;

    if(http_state==HTTP_REQUEST_POST)//发起了POSE请求
    {
        response_p1 = (char*)strstr((const char*)data,(const char*)"\"errno\":");
        if(response_p1!=NULL)
        {
          response_p1+=8;
          result=atoi(response_p1);

          if(result==0)
          {
            printf("POST OK!\r\n");
          }
          else
          {
            printf("POST error!\r\n");
          }
        }

    }


    http_state=HTTP_REQUEST_IDLE;//空闲

    return 0;
}

//HTTP POST数据到OneNet
//devices_id:设备的ID号
//api_key：设备APIKEY/产品APIKEY
//value：数据流
u8 HTTP_Post_OneNet(char *devices_id,char *api_key,char *value)
{
    u16 length=0;
    char post_str[100]={0};
    u8 cmd[200]={0};
    u16 len=0;

    //只能idle过程中能发起请求
    if(http_state!=HTTP_REQUEST_IDLE) return 0;
    if(value==NULL) return 0;

    sprintf((char*)post_str,"%s",value);
    length = strlen(post_str);
    if(length==0) return 0;

    memset(ontnet_tx_buff, 0, 500);
    sprintf(ontnet_tx_buff, "POST http://api.heclouds.com/devices/%s/datapoints?type=3 HTTP/1.1\r\n",devices_id);
    strcat(ontnet_tx_buff, "Host: api.heclouds.com\r\n");
    sprintf(cmd, "api-key: %s\r\n",api_key);
    strcat(ontnet_tx_buff, cmd);
    sprintf(cmd, "Content-Length: %d\r\n",length);
    strcat(ontnet_tx_buff, cmd);
    strcat(ontnet_tx_buff, "\r\n");
    strcat(ontnet_tx_buff, post_str);

    len=strlen(ontnet_tx_buff);
    if(1==NB_SendData(ontnet_tx_buff, len))
    {
        http_state=HTTP_REQUEST_POST;
    }

    return 0;
}







