#ifndef __BSP_WIFICOMMU_H__
#define __BSP_WIFICOMMU_H__

#include "stm32f4xx.h"


typedef enum{
  eWifi_HandShake=0,
  eWifi_OffEcho,
  eWifi_Reboot,
  eWifi_Reload,
  eWifi_ExitATMode,
  eWifi_WorkMode,
  eWifi_ThroughMode,
  eWifi_ScanWifiSSID,				//ȥ���ù���,����SD����ȡwifi SSID������
  eWifi_SSIDAAccount,
  eWifi_SSIDBAccount,
  eWifi_SSIDCAccount,
  eWifi_SSIDAKey,
  eWifi_SSIDBKey,
  eWifi_SSIDCKey,
  eWifi_CheckWAN,
  eWifi_QualityThreshold,
  eWifi_UartConfig,
  eWifi_NetProtocol
}eWifiCmd;

typedef enum{
  eErrCmdFormat=1,
  eErrCmd,
  eErrCmdOperations,
  eErrCmdParas,
  eErrUnlimitedOperate,
  eErrUnknown
}eWifiErrAck;

typedef struct{
  uint8_t dat[100];
  uint8_t len;
  uint8_t flag;         //0xA5 ���Ϊbuff�������ڴ����
  uint16_t pBuffLen;
  uint8_t* pBuff;
}xWifiRecvBuf;

typedef enum{
	eBAUDRATE_ERR=0,
	eBAUDRATE_1200,
	eBAUDRATE_2400,
	eBAUDRATE_4800,
	eBAUDRATE_9600,
	eBAUDRATE_14400,
	eBAUDRATE_19200,
	eBAUDRATE_38400,
	eBAUDRATE_43000,
	eBAUDRATE_57600,
	eBAUDRATE_76800,
	eBAUDRATE_115200,
	eBAUDRATE_218750,
	eBAUDRATE_128000,
	eBAUDRATE_230400,
	eBAUDRATE_256000,
	eBAUDRATE_437500,
	eBAUDRATE_460800,
	eBAUDRATE_875000,
	eBAUDRATE_921600,
	eBAUDRATE_1000000,
	eBAUDRATE_1382400,
	eBAUDRATE_2000000,
}eBaudRate;

#define CHARVARLEN 60
#define SSIDTEXT_MAXSIZE 2048
typedef struct{
  eWifiCmd theCmd;
  uint8_t cmdlen;               //���в��������һ��ҪҪ����0
  char SettingVars[CHARVARLEN];       //��Ҫ�����õĲ�����Ҳ���ԣ���ѯ���أ����
//  uint8_t isrecvok;				//�Ƿ���ȷ����
}xWifiCmd;


typedef struct SSIDItems{
  char    thsSSID[25];    //24+1
  char    thePASSWD[16];  //8-15����15+1
  uint8_t Quality;          //�ź�ǿ�ȴ���50�����
  uint16_t Port;
  uint8_t SeverIPAddr[4];
//  uint8_t Encryption;      //������ʱֻ��AES
//  uint8_t Authentication;  //��ֻ֤��WPA2PSK
  eBaudRate WifiUartbps;
}xSSIDItem;




#define WIFITICKPERIOD		29			//wifi tick ���ڣ���x����



extern xWifiRecvBuf WifiRxBuf;
extern xSSIDItem theSSID;

//��Ӳ�����ƥ�书��
uint32_t wifiEnterUartATCmd(void);  //Ĭ�Ϲر�������Թ���,���Ч��,����ͨ����������wifiTurnOffEcho���򿪹ر�.
uint32_t wifiEcho(void);    //����MCU��wifiģ���uartͨ������
uint32_t wifiATCmdSend(xWifiCmd *WifiCmd);
uint32_t wifiExitUartATCmd(void);
uint32_t wifiSwitchEcho(uint32_t *result);

uint32_t wifiUartConfig(eBaudRate wifiuartbdrt);
uint32_t wifidefaultconfig(xSSIDItem *wifiaccount,uint32_t ifinitialconf,eBaudRate newboudrate,uint32_t newssid);
//#define wifiinitialconnect((wifiaccount),(newboudrate))   wifidefaultconfig((wifiaccount),1,(newboudrate),0)
uint32_t wifiGetConfigState(xSSIDItem *wifiState);			//�����ȴ�wifiģ��OK������ã����Զೢ�Լ���
uint32_t hostwifiUartportboudratechange(eBaudRate wifiuartbdrt);

//static xSSIDItem* wifiSSIDMake(uint8_t *origintext);
static uint32_t wifiConnect(xSSIDItem *temp);
#endif
