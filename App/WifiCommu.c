#include "stm32f4xx.h"
#include "string.h"
#include "stdlib.h"
#include "usart.h"
#include "WifiCommu.h"
#include "App_define.h"


xWifiRecvBuf WifiRxBuf={0};
xSSIDItem theSSID={"\0","\0",100,0,{0,0,0,0},eBAUDRATE_460800};

//��Ҫ��ӣ��������
const char WifiCmds[][10]={
    "VER","E","Z","RELD","ENTM","WMODE","TMODE","WSCAN","WSSSIDA","WSSSIDB","WSSSIDC",
    "WSKEYA","WSKEYB","WSKEYC","WANN","WSQY","UART","NETP"
};


const uint16_t CmdAckDelay[]={
    200,200,250,200,200,1000,1000,9000,1200,200,200,1200,200,200,200,200,1000,1500
};


uint32_t wifiEnterUartATCmd(void)
{
	uint32_t udret=1,i;//,udresult=0xff;
  xWifiCmd WifiCmd={
      eWifi_OffEcho,0,0
  };

  HAL_UART_AbortReceive_IT(&WifiPort);
  memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
//  WifiUartSend("+++",3);
//  HAL_UART_Receive(&WifiPort,WifiRxBuf.dat+WifiRxBuf.len,1,500);
//
//  if(WifiRxBuf.dat[0]!='a'){
//	  assert_failed("Enter Uart AT Cmd Mode fail , wifi not init\r\n",0);
//	  return udret;
//  }
//
//  WifiRxBuf.len++;
//  WifiUartSend("a",1);
//  HAL_UART_Receive(&WifiPort,WifiRxBuf.dat+WifiRxBuf.len,3+4,500);

  HAL_UART_Receive_IT(&WifiPort,WifiRxBuf.dat,1);
  memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
  WifiUartSend("+++",3);
  HAL_Delay(500);
  WifiUartSend("a",1);
  HAL_Delay(200);
  HAL_UART_AbortReceive_IT(&WifiPort);
  for (i=0;i<20;i++){
	  if(WifiRxBuf.dat[i]!='+'){

	  }
	  else
		  break;
  }
  if(i>=20){
	  assert_failed("Enter Uart AT Cmd Mode fail , wifi not init\r\n",0);
	  return udret;
  }


  if(memcmp(WifiRxBuf.dat+i,"+ok",3)==0)   //������buff
    {
	  udret = wifiATCmdSend(&WifiCmd);		//�������һ�Σ�Ĭ�Ϲرջ���
      if(udret>0)
      {
    	  assert_failed(__FILE__,__LINE__);
    	  assert_failed("wifiCmdErr:",udret);
      }
      else
      {
    	  assert_failed("enter AT Mode\r\n",0);
      }
      memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
    }
  else
    {
      assert_failed("wifi AT Open Fail\r\n",0);
      memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
      udret = 0xff;
    }
  if(udret!=0)
	  wifiExitUartATCmd();
  return udret;
}

// CR = \r =0x0D  LF = \n =0x0A
uint32_t wifiATCmdSend(xWifiCmd *WifiCmd)
{
  uint32_t udret=1;
  char CmdToSend[100]="AT+";

  if(WifiCmd->cmdlen>0)
    sprintf(CmdToSend+3,"%s=%s\r",WifiCmds[WifiCmd->theCmd],WifiCmd->SettingVars);
  else
    sprintf(CmdToSend+3,"%s\r",WifiCmds[WifiCmd->theCmd]);
  HAL_UART_AbortReceive_IT(&WifiPort);
  memset(WifiRxBuf.dat,0,sizeof(WifiRxBuf.dat)+2); 			 //���dat��len+flag���
  HAL_UART_Receive_IT(&WifiPort,WifiRxBuf.dat+WifiRxBuf.len,1);
  WifiUartSend((uint8_t *)CmdToSend,strlen(CmdToSend));   //send cmd

    //һ��ָ�100�ֽ��㹻������Ҫ���ٿռ�
    //����������<4,Ĭ���л��Թ���,���������ͳ���4��,�Ͳ�����ܽ����

//  HAL_Delay(CmdAckDelay[WifiCmd->theCmd]);		//�����жϽ��շ�ʽ
  while(WifiRxBuf.dat[WifiRxBuf.len-1]!='\n' || WifiRxBuf.dat[WifiRxBuf.len-2]!='\r'\
		  || WifiRxBuf.dat[WifiRxBuf.len-3]!='\n' || WifiRxBuf.dat[WifiRxBuf.len-4]!='\r');
  for (int i=0;i<WifiRxBuf.len-4;i++)
	{
	  if(WifiRxBuf.dat[i]!='+')
		continue;
	  if((WifiRxBuf.dat[i+1]=='o')&&(WifiRxBuf.dat[i+2]=='k'))
		{
		  memset(WifiCmd->SettingVars,0,CHARVARLEN);
		  strcpy(WifiCmd->SettingVars,(const char *)WifiRxBuf.dat+i);
		  udret = 0;
		  i+=6-1;
		}
	  else if ((WifiRxBuf.dat[i+1]=='E')&&(WifiRxBuf.dat[i+2]=='R')&&(WifiRxBuf.dat[i+3]=='R'))
		{
		  memset(WifiCmd->SettingVars,0,CHARVARLEN);
		  strcpy(WifiCmd->SettingVars,(const char *)WifiRxBuf.dat+i);
		  udret = (WifiRxBuf.dat[i+6]-'0');
		  i+=10-1;
		}
	  else
		continue;
	}

  return udret;
}



uint32_t wifiExitUartATCmd(void)
{
  xWifiCmd ExitATCmd={
      eWifi_ExitATMode,0
  };

  return (wifiATCmdSend(&ExitATCmd));
}


//�ر�ģ���ָ����Թ��ܣ����Ч�� AT+E
uint32_t wifiSwitchEcho(uint32_t *result)
{
  xWifiCmd WifiCmd={
      eWifi_OffEcho,0,0
  };
  uint32_t ret=0x1;
  ret = wifiEnterUartATCmd();
  if(ret!=0)
    {
      assert_failed(__FILE__,__LINE__);
      return 1;
    }

  *result = wifiATCmdSend(&WifiCmd);    //  �رջ��Թ��� AT+E

  ret = wifiExitUartATCmd();
//retlab:
    return ret;
}



//���ԣ� ����ATģʽ ��get version ָ�� �˳�ATģʽ AT+VERSION
uint32_t wifiEcho(void)
{
  xWifiCmd WifiCmd={
      eWifi_HandShake,0,0
  };
  uint32_t ret=0x1;
  ret = wifiEnterUartATCmd();
  if(ret!=0)
      return ret;

//  ����wifi uart ������ȷ����Ҫ��ɣ�
//    �߼����ڷ���ǰ���buff�����պ󣬴��������buf��������������bufû�������buf����֮ǰ������
//    �������ж�
  ret = wifiATCmdSend(&WifiCmd);  //  �رջ��Թ��� AT+E
  if(ret == 0)
	  printf("module ack:%s",WifiCmd.SettingVars);
  else{
	  assert_failed(__FILE__,__LINE__);
	  assert_failed("wifiCmdErr:",ret);
  }
  memset(&WifiRxBuf,0,sizeof(WifiRxBuf));

  ret = wifiExitUartATCmd();
  if(ret == 0)
	  assert_failed("Exit At Mode\r\n",0);
  else{
	  assert_failed(__FILE__,__LINE__);
	  assert_failed("wifiCmdErr:",ret);
  }
  memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
  HAL_UART_AbortReceive_IT(&WifiPort);
  return ret;
}


uint32_t wifidefaultconfig(xSSIDItem *wifiaccount,uint32_t ifinitialconf,eBaudRate newboudrate,uint32_t newssid)
{
  uint32_t ret=0x1;
  xWifiCmd WifiCmd;

  while(wifiEnterUartATCmd())
  {
	  wifiExitUartATCmd();
	  assert_failed(__FILE__,__LINE__);
  }
//origin
//  ret = wifiEnterUartATCmd();
//  if(ret!=0){
//      assert_failed(__FILE__,__LINE__);
//      return ret;
//  }

  if(ifinitialconf>0)
  {
	  memset(&WifiCmd,0,sizeof(WifiCmd));
	  WifiCmd.theCmd = eWifi_WorkMode;
	  strcpy(WifiCmd.SettingVars,"STA");
	  WifiCmd.cmdlen = strlen("STA");
	  ret = wifiATCmdSend(&WifiCmd);
	  if(ret!=0){
		  assert_failed(__FILE__,__LINE__);
		  return ret;
	  }
	  assert_failed("Work Mode set : STA\r\n",0);

	  memset(&WifiCmd,0,sizeof(WifiCmd));
	  WifiCmd.theCmd = eWifi_ThroughMode;
	  strcpy(WifiCmd.SettingVars,"Through");
	  WifiCmd.cmdlen = strlen("Through");
	  ret = wifiATCmdSend(&WifiCmd);
	  if(ret!=0){
		  assert_failed(__FILE__,__LINE__);
		  return ret;
	  }
	  assert_failed("Trans Mode set : Through\r\n",0);

	  //��ӹ���ģʽ����
	  memset(&WifiCmd,0,sizeof(WifiCmd));
	  WifiCmd.theCmd = eWifi_NetProtocol;
	  sprintf(WifiCmd.SettingVars,"TCP,CLIENT,%d,%d.%d.%d.%d",wifiaccount->Port,\
			  wifiaccount->SeverIPAddr[0],wifiaccount->SeverIPAddr[1],wifiaccount->SeverIPAddr[2],wifiaccount->SeverIPAddr[3]);
	  WifiCmd.cmdlen = strlen(WifiCmd.SettingVars);
	  ret = wifiATCmdSend(&WifiCmd);
	  if(ret!=0){
		  assert_failed(__FILE__,__LINE__);
		  return ret;
	  }
	  assert_failed("Net Protocol set Ok \r\n",0);
  }

  if(newssid ||  ifinitialconf > 0){
	  ret = wifiConnect(wifiaccount);
	  if(ret!=0){
		  assert_failed(__FILE__,__LINE__);
		  return ret;
	  }
  }

  //��߲�����
  if(newboudrate ||  ifinitialconf > 0){
	  ret = wifiUartConfig(newboudrate);
	  if(ret!=0){
		  return ret;
	  }
  }


  //������Ч
  memset(&WifiCmd,0,sizeof(WifiCmd));
  WifiCmd.theCmd = eWifi_Reboot;
  ret = wifiATCmdSend(&WifiCmd);
  if(ret != 0)
    {
      assert_failed(__FILE__,__LINE__);
      assert_failed("wifiCmdErr:",ret);
      return ret;
    }
  assert_failed("wifi reboot ok\r\n",0);

  //����ı��˲����ʣ������Ĳ�����Ҳ�����ű仯
  if(newboudrate ||  ifinitialconf > 0)
  {
	  HAL_UART_DeInit(&WifiPort);
	  switch(newboudrate)
	  {
	  	  case eBAUDRATE_9600:
		  WifiPort.Init.BaudRate=9600;
		  break;
	  	  case eBAUDRATE_57600:
		  WifiPort.Init.BaudRate=57600;
		  break;
	  	  case eBAUDRATE_115200:
		  WifiPort.Init.BaudRate=115200;
		  break;
	  	  case eBAUDRATE_230400:
		  WifiPort.Init.BaudRate=230400;
		  break;
	  	  case eBAUDRATE_460800:
		  WifiPort.Init.BaudRate=460800;
		  break;
	  	  default:
	  		WifiPort.Init.BaudRate=57600;
	  		break;
	  }
	  if (HAL_UART_Init(&WifiPort) != HAL_OK)
	  {
	    assert_failed("host wifi port reinit fail\r\n",0);
	  }
	  assert_failed("host wifi port reinit OK\r\n",0);
  }

  //�ȴ���������ʵ�ֽ���
  while(wifiEcho());
  ret=0;
  assert_failed("wifi port restart OK\r\n",0);

   return ret;
}

uint32_t wifiConnect(xSSIDItem *temp)
{
  uint32_t ret=0xff;
  xWifiCmd WifiCmd;
  xSSIDItem *pSSIDList;

  if(temp==NULL){
	  xSSIDItem SSIDItem={"Tz_test01","124578369",90};
	  pSSIDList=&SSIDItem;
  }else{
	  pSSIDList=temp;
  }


  WifiCmd.theCmd = eWifi_SSIDAAccount;
  strcpy(WifiCmd.SettingVars,pSSIDList->thsSSID);
  WifiCmd.cmdlen = strlen(pSSIDList->thsSSID);
  ret = wifiATCmdSend(&WifiCmd);
  if(ret != 0)
    {
      assert_failed(__FILE__,__LINE__);
      assert_failed("wifiCmdErr:",ret);
      return ret;
    }

  WifiCmd.theCmd = eWifi_SSIDAKey;
  strcpy(WifiCmd.SettingVars,"WPA2PSK,AES,");
  strcpy(WifiCmd.SettingVars+12,pSSIDList->thePASSWD);
  WifiCmd.cmdlen = strlen("WPA2PSK,AES,")+strlen(pSSIDList->thePASSWD);
  ret = wifiATCmdSend(&WifiCmd);
  if(ret != 0)
    {
      assert_failed(__FILE__,__LINE__);
      assert_failed("wifiCmdErr:",ret);
      return ret;
    }
  assert_failed("ssid config ok \r\n",0);
  return ret;
}

uint32_t wifiUartConfig(eBaudRate wifiuartbdrt)
{
	uint32_t ret=0xff;
	xWifiCmd WifiCmd={
		eWifi_UartConfig,1,0
	};
  switch(wifiuartbdrt)
  {
//    case eBAUDRATE_9600:
//    	strcpy(WifiCmd.SettingVars,"9600,8,1,None,NFC");
//      break;
    case eBAUDRATE_57600:
    	strcpy(WifiCmd.SettingVars,"57600,8,1,None,NFC");
      break;
    case eBAUDRATE_115200:
    	strcpy(WifiCmd.SettingVars,"115200,8,1,None,NFC");
      break;
    case eBAUDRATE_230400:
    	strcpy(WifiCmd.SettingVars,"230400,8,1,None,NFC");
      break;
    case eBAUDRATE_460800:
    	strcpy(WifiCmd.SettingVars,"460800,8,1,None,NFC");
      break;
    default:
        assert_failed(__FILE__,__LINE__);
        strcpy(WifiCmd.SettingVars,"57600,8,1,None,NFC");
    	break;
  }

  WifiCmd.cmdlen = strlen(WifiCmd.SettingVars);
  ret = wifiATCmdSend(&WifiCmd);
  if(ret != 0)
    {
      assert_failed(__FILE__,__LINE__);
      assert_failed("wifiCmdErr:",ret);
      return ret;
    }
  assert_failed("wifi uart config OK \r\n",0);
  return ret;
}

uint32_t hostwifiUartportboudratechange(eBaudRate wifiuartbdrt)
{
	  HAL_UART_DeInit(&WifiPort);
	  switch(wifiuartbdrt)
	  {
	  	  case eBAUDRATE_9600:
		  WifiPort.Init.BaudRate=9600;
		  break;
	  	  case eBAUDRATE_57600:
		  WifiPort.Init.BaudRate=57600;
		  break;
	  	  case eBAUDRATE_115200:
		  WifiPort.Init.BaudRate=115200;
		  break;
	  	  case eBAUDRATE_230400:
		  WifiPort.Init.BaudRate=230400;
		  break;
	  	  case eBAUDRATE_460800:
		  WifiPort.Init.BaudRate=460800;
		  break;
	  	  default:
	  		WifiPort.Init.BaudRate=57600;
	  		break;
	  }
	  if (HAL_UART_Init(&WifiPort) != HAL_OK)
	  {
	    assert_failed("host wifi port reinit fail\r\n",0);
	    return 1;
	  }
	  assert_failed("host wifi port reinit OK\r\n",0);
	  return 0;
}


uint32_t wifiGetConfigState(xSSIDItem *wifiState)
{
  xWifiCmd WifiCmd={
	  eWifi_SSIDAAccount,0,0
  };
  uint32_t ret=0x1;
  ret = wifiEnterUartATCmd();
  if(ret!=0)
	  return ret;

  ret = wifiATCmdSend(&WifiCmd);  //  ssid
  if(ret == 0){
	  assert_failed((uint8_t*)WifiCmd.SettingVars,0);
	  strcpy(wifiState->thsSSID,WifiCmd.SettingVars+4);		//���Ϊ\r\n\r\n
	  memset(wifiState->thsSSID+strlen(wifiState->thsSSID)-4,0,4);
  }
  else{
	  assert_failed(__FILE__,__LINE__);
	  assert_failed("wifiCmdErr:",ret);
	  return ret;
  }

  WifiCmd.theCmd=eWifi_SSIDAKey;
  WifiCmd.cmdlen=0;
  ret = wifiATCmdSend(&WifiCmd);  //  passwd
  if(ret == 0){
	  assert_failed((uint8_t*)WifiCmd.SettingVars,0);
	  strcpy(wifiState->thePASSWD,WifiCmd.SettingVars+16);	//���Ϊ\r\n\r\n
	  memset(wifiState->thePASSWD+strlen(wifiState->thePASSWD)-4,0,4);
  }
  else{
	  assert_failed(__FILE__,__LINE__);
	  assert_failed("wifiCmdErr:",ret);
  }

  WifiCmd.theCmd=eWifi_NetProtocol;
  WifiCmd.cmdlen=0;
  ret = wifiATCmdSend(&WifiCmd);  //  net protocol
  if(ret == 0){
	  assert_failed((uint8_t*)WifiCmd.SettingVars,0);
	  uint32_t i;
	  uint8_t *p;
	  for(p=(uint8_t*)WifiCmd.SettingVars;*p!=',';p++)
	  {

	  }
	  for(++p;*p!=',';p++)
	  {

	  }
	  for (wifiState->Port=0,++p;*p!=',';p++)
	  {
		  wifiState->Port = wifiState->Port * 10 + *p -'0';
	  }
	  memset(wifiState->SeverIPAddr,0,4);

	  for (i=0,++p;;p++)
	  {
		  if((*p>='0')&&(*p<='9'))
			  wifiState->SeverIPAddr[i] = wifiState->SeverIPAddr[i] * 10 + *p -'0';
		  else
			  i++;
		  if(i>3)
			  break;
	  }

  }
  else{
	  assert_failed(__FILE__,__LINE__);
	  assert_failed("wifiCmdErr:",ret);
  }

  ret = wifiExitUartATCmd();
  if(ret == 0)
	  assert_failed("Exit At Mode\r\n",0);
  else{
	  assert_failed(__FILE__,__LINE__);
	  assert_failed("wifiCmdErr:",ret);
  }
  memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
  HAL_UART_AbortReceive_IT(&WifiPort);
  return ret;
}


