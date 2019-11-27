/*
 * App_main.c
 *
 *  Created on: 2019年7月22日
 *      Author: prejoy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "crc.h"
#include "dma.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"
#include "usb_otg.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "App_main.h"
#include "ModuleCommu.h"
#include "WifiCommu.h"
#include "TestFuncs.h"
#include "dev_INA219.h"
#include "exsram.h"

#include "exfuns.h"
#include "ff.h"

#include "dialog.h"
#include "GUI.h"
#include "bitmap.h"



xAllTestContents *pAllRecords=NULL;
WM_HWIN AllWindows[3] = {0, 0, 0};
xTestTask xTestList[MAX_TEST_ITEM]={(eTestFunc)0};			//测试项目存放在静态链表中
static void getWifiMesg(xTestSelection *pTestconfig);
static void getModelSelectionList(xTestSelection *pTestconfig);
//==============     Semaphore      ===============
//os state
xSysState SysStates={0,0,0,{0},{0},0};
SemaphoreHandle_t xMutexHandleCriticalAssertProtection=NULL,  //如果xMutexHandleCriticalAssertProtection为NULL，则会进入configasser -> while(1)
				xBinaryHandleWIFINETCONNECT=NULL;


QueueHandle_t xQueueTestResultUpload,
				xQueueTouchVccCtrlMsg,
				xQueueModuleVccCtrlMsg,
				xQueueWifiUploadMsg,
				xQueueScreenMsg,
				xQueueRecordMsg;
#define QUEUE_LENGTH_TestResultUpload			3
#define QUEUE_ITEM_SIZE_TestResultUpload	sizeof(xTestResult)

#define QUEUE_LENGTH_TouchVccCtrlMsg			2
#define QUEUE_ITEM_SIZE_TouchVccCtrlMsg		sizeof(xCurrentQueueMessage)

#define QUEUE_LENGTH_ModuleVccCtrlMsg			2
#define QUEUE_ITEM_SIZE_ModuleVccCtrlMsg	sizeof(xCurrentQueueMessage)

#define QUEUE_LENGTH_WifiUploadMsg				4
#define QUEUE_ITEM_SIZE_WifiUploadMsg		sizeof(xWifiUploadMessage)

#define QUEUE_LENGTH_ScreenMsg					8	//7
#define QUEUE_ITEM_SIZE_ScreenMsg			sizeof(xScreenMsg)

#define QUEUE_LENGTH_RecordMsg					5
#define QUEUE_ITEM_SIZE_RecordMsg			sizeof(xRecordMsg)

//================        TCBs      ===============
static void vTaskStatusIndication( void * pvParameters );
#define Prio_StatusIndication		2
static TaskHandle_t TaskHandle_StatusIndication;
#define STK_SIZE_StatusIndication	256

static void vTaskDebug( void * pvParameters );
#define Prio_Debug		2
static TaskHandle_t TaskHandle_Debug;
#define STK_SIZE_Debug	256

static void vTaskSaveRecord( void * pvParameters );
#define Prio_SaveRecord		6
static TaskHandle_t TaskHandle_SaveRecord;
#define STK_SIZE_SaveRecord	  (1024*3)

static void vTaskWifiUploadbg( void * pvParameters );
#define Prio_WifiUploadbg		3
static TaskHandle_t TaskHandle_WifiUploadbg;
#define STK_SIZE_WifiUploadbg	1536

static void vTaskModuleFunc( void * pvParameters );
#define Prio_ModuleFunc		7
static TaskHandle_t TaskHandle_ModuleFunc;
#define STK_SIZE_ModuleFunc	(1024*5)

static void vTaskTouchPower( void * pvParameters );
#define Prio_TouchPower		5
static TaskHandle_t TaskHandle_TouchPower;
#define STK_SIZE_TouchPower	768

static void vTaskModulePower( void * pvParameters );
#define Prio_ModulePower		5
static TaskHandle_t TaskHandle_ModulePower;
#define STK_SIZE_ModulePower	768

static void vTaskMonitor( void * pvParameters );
#define Prio_Monitor		4
static TaskHandle_t TaskHandle_Monitor;
#define STK_SIZE_Monitor	1536

//============       static funcs       ==================
static int cmp2float(const void *a, const void *b)
{
	return *(float*)a > *(float*)b ? 1 : -1;
}


//============       task funcs       ==================
void vTaskStatusIndication( void * pvParameters )
{
    for(;;)
    {
    	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
//		assert_failed("LED1 ON\r\n",0);
//		vTaskDelay( pdMS_TO_TICKS(1000) );
//		assert_failed("LED1 OFF\r\n",0);
		vTaskDelay( pdMS_TO_TICKS(1000) );
    }
}

void vTaskDebug( void * pvParameters )
{
    for(;;)
    {
    	if(DebugRxBuf[100]>0 && DebugRxBuf[DebugRxBuf[100]-1] == '\n'){
//    		assert_failed(DebugRxBuf,0);
    		if(strcmp((const char *)DebugRxBuf,"T0001\r\n")==0){

     		}


    		while(HAL_UART_AbortReceive_IT(&DebugPort));
    		memset(DebugRxBuf,0,sizeof(DebugRxBuf));
    		while(HAL_UART_Receive_IT(&DebugPort,DebugRxBuf+DebugRxBuf[100],1));
    	}
		vTaskDelay( pdMS_TO_TICKS(20) );
    }
}



const char MsgstoUsers[6][30]={
		"请按KEY1开始测试",
		"测试中...",
		"请按压操作手到位",
		"保存记录...",
		"测试完成"
};
const int TextObjItem[3][15]={
		{
			(GUI_ID_USER + 0x18)	//mesg to user
		},
		{
			(GUI_ID_USER + 0x3),	//the test item result
			(GUI_ID_USER + 0x17),
			(GUI_ID_USER + 0xC),
			(GUI_ID_USER + 0xF),
			(GUI_ID_USER + 0x10),
			(GUI_ID_USER + 0x11),
			(GUI_ID_USER + 0x12),
			(GUI_ID_USER + 0x13),
			(GUI_ID_USER + 0xD),
			(GUI_ID_USER + 0xE),
		},
		{
			(GUI_ID_USER + 0x15),	//wifi state
			(GUI_ID_USER + 0x01),	//img fgp
			(GUI_ID_USER + 0x1E)	//img pass or ng
		}
};
extern GUI_BITMAP StdImgBitMap;
void vTaskMonitor( void * pvParameters )
{
    WM_HWIN      hItem;
	xScreenMsg ScreenMsg={0};

	//在开始测试前，配置好测试设置,uart sd,usb
//	TestInitialConfig();
	pTestconfig = (xTestSelection *)malloc(1500);
	memset(pTestconfig,0,sizeof(xTestSelection));
	getWifiMesg(pTestconfig);
	getModelSelectionList(pTestconfig);

	//页面初始化函数需要修改，将逻辑分离
	AllWindows[0] = CreateUI_Config();  //配置页面
//	GUI_EndDialog(,0);
	AllWindows[1] = CreateUI_ActModTest();    //主页面
    WM_HideWindow(AllWindows[1]);
    WM_ShowWindow(AllWindows[0]);
    WM_SetFocus(AllWindows[0]);

    for(;;)
    {
    	if(xQueueReceive(xQueueScreenMsg,&ScreenMsg,pdMS_TO_TICKS(20))==pdPASS){
    		if(ScreenMsg.MsgType == TestMsgToUser){
    			hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[0][0]);
    			TEXT_SetText(hItem, MsgstoUsers[ScreenMsg.SignalNo]);
    			switch(ScreenMsg.SignalNo){
					case TESTTING:
						for(int i=0;i<10;i++){					//item 增加需要修改！！！！
							hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][i]);
							EDIT_SetText(hItem, "-");
							EDIT_SetTextColor(hItem,EDIT_CI_ENABLED,0x00000000);
						}
						uint8_t temp=0x00;
						hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[2][1]);
						IMAGE_SetBMP(hItem, &temp, 1);

						hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[2][2]);
						IMAGE_SetBMP(hItem, &temp, 1);
						break;
					default:
						break;
    			}
    		}

    		else if(ScreenMsg.MsgType == TestResItem){
    			switch(ScreenMsg.SignalNo){
    				case INDEX_TOUCHOUT:
    					hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_TOUCHOUT]);
    					break;
    				case  INDEX_CALIBAUD:
						hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_CALIBAUD]);
    					break;
    				case INDEX_STARTDUARTION:
    					hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_STARTDUARTION]);
    					break;
    				case INDEX_GETVERSION:
    					hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_GETVERSION]);
    					break;
    				case INDEX_XORCHECK:
    					hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_XORCHECK]);
    					break;
    				case INDEX_FLASH:
    					hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_FLASH]);
    					break;
    				case INDEX_LED:
    					hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_LED]);
    					break;
    				case INDEX_IMAGE:
    					hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_IMAGE]);
    					break;
    				case INDEX_TVCC:
    					hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_TVCC]);
    					break;
    				case INDEX_MVCC:
    					hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[1][INDEX_MVCC]);
    					break;
    				default:
    					break;
    			}
    			if(ScreenMsg.msg[0]=='y'){
    				EDIT_SetText(hItem, "通过");
    				EDIT_SetTextColor(hItem,EDIT_CI_ENABLED,0x0000FF00);
    			}
    			else if(ScreenMsg.msg[0]=='n'){
    				EDIT_SetText(hItem, "失败");
    				EDIT_SetTextColor(hItem,EDIT_CI_ENABLED,0x000000FF);
    			}
    			else{
    				EDIT_SetText(hItem, "-");
    				EDIT_SetTextColor(hItem,EDIT_CI_ENABLED,0x00000000);
    			}

    			if(ScreenMsg.SignalNo == INDEX_IMAGE && ScreenMsg.msg[0]=='y'){
    				hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[2][1]);
    				StdImgBitMap.XSize = pAllRecords->hImgSaves.width;
    				StdImgBitMap.YSize = pAllRecords->hImgSaves.height;
    				StdImgBitMap.BytesPerLine = StdImgBitMap.XSize;
    				StdImgBitMap.pData = (uint8_t *)(pAllRecords->hImgSaves.oriImgaddr);
    				StdImgBitMap.pData++;						//跳过F5的头
    				IMAGE_SetBitmap(hItem,&StdImgBitMap);
    			}else if(ScreenMsg.SignalNo == INDEX_ALLRESULT ){
    				hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[2][2]);
    				IMAGE_SetBitmap(hItem,ScreenMsg.msg[0]=='y'?&bmPASS:&bmNG);
    			}

    		}

    		else if(ScreenMsg.MsgType == WifiStateMsg){
    			hItem = WM_GetDialogItem(AllWindows[1], TextObjItem[2][0]);
				TEXT_SetText(hItem, "这里是状态信息");
    		}
    	}

    	GUI_Exec();
//    	GUI_Delay(2);
//		vTaskDelay( pdMS_TO_TICKS(20) );
    }
}

int32_t WifiRxAnalyse(int type,uint32_t CntNo)
{
	char *p = WifiRxBuf.dat;
	if(WifiRxBuf.len==0){
		return -1;
	}
	uint32_t len = WifiRxBuf.len;		//!!目前wifi收发的三种类型不会出现并行的情况
	for(uint32_t i=0;i<len;i++){
		if(WifiRxBuf.dat[i]!=':')continue;
		if(WifiRxBuf.dat[i-1]!='K'||WifiRxBuf.dat[i-2]!='C')continue;
		++i;
		switch(type){
		case 1:		//data
			if(memcmp(&WifiRxBuf.dat[i],"data",4)==0){
				i+=5;
				uint32_t Cnttemp=0;
				do{
					Cnttemp = Cnttemp*10+WifiRxBuf.dat[i++]-'0';
				}while(WifiRxBuf.dat[i]!=',');
				if(Cnttemp==CntNo){
					if(WifiRxBuf.dat[++i]=='o'&&WifiRxBuf.dat[++i]=='k'){
						return 0;		//正确接收完毕，next
					}
					else{
						return 1;		//重发
					}
				}
				else{
					return 2;			//数据序号不匹配
				}
			}
			break;
		case 2:		//tick
			if(memcmp(&WifiRxBuf.dat[i],"tick",4)==0){
				i+=5;
				return 10;
//				uint32_t Cnttemp=0;
//				do{
//					Cnttemp = Cnttemp*10+WifiRxBuf.dat[i++]-'0';
//				}while(WifiRxBuf.dat[i]!=',');
//				if(Cnttemp==CntNo){
//					if(WifiRxBuf.dat[++i]=='o'&&WifiRxBuf.dat[++i]=='k'){
//						return 10;		//tick 序号相同
//					}
//					else{
//						return 11;		//重发
//					}
//				}
//				else{
//					return 12;			//数据序号不同
//				}
			}
			else{
				return 11;
			}
			break;
		case 3:		//rtc
			if(memcmp(&WifiRxBuf.dat[i],"rtc",3)==0){
				i+=5;
				if(WifiRxBuf.dat[i]>='0'&&WifiRxBuf.dat[i]<='9'&&WifiRxBuf.dat[i+1]>='0'&&WifiRxBuf.dat[i+1]<='9'){
					SysStates.Calendar.hdate.WeekDay = RTC_WEEKDAY_MONDAY;
					SysStates.Calendar.hdate.Year = (WifiRxBuf.dat[i]-'0')*10+(WifiRxBuf.dat[i+1]-'0');
					SysStates.Calendar.hdate.Month = (WifiRxBuf.dat[i+3]-'0')*10+(WifiRxBuf.dat[i+4]-'0');
					SysStates.Calendar.hdate.Date = (WifiRxBuf.dat[i+6]-'0')*10+(WifiRxBuf.dat[i+7]-'0');
					SysStates.Calendar.htime.Hours = (WifiRxBuf.dat[i+9]-'0')*10+(WifiRxBuf.dat[i+10]-'0');
					SysStates.Calendar.htime.Minutes = (WifiRxBuf.dat[i+12]-'0')*10+(WifiRxBuf.dat[i+13]-'0');
					SysStates.Calendar.htime.Seconds = (WifiRxBuf.dat[i+15]-'0')*10+(WifiRxBuf.dat[i+16]-'0');
					SetCalendar(&SysStates.Calendar);
					i+=16;
					return 30;
				}
				return 31;
			}
			break;
		default:
			break;
		}
	}

	return -1;
}
void vTaskWifiUploadbg( void * pvParameters )
{
	uint32_t NextTickToSendWifiTick=HAL_GetTick()+WIFITICKPERIOD*1000;			//Wifi Tick 周期 ，xx秒
	xWifiUploadMessage hFRestouploadMsg={0};
	uint8_t dmabuf[512]={0};
	FIL hFile;
	FRESULT fRet;
	uint32_t tickNo=0,MessageNo=0,tickbreakingtimes=0;
	char Mac[50]={0},DatHead[50];
	sprintf(Mac,"Machine:%08X%08X%08X TkCnt:%u\n",SysStates.HostUID[0],SysStates.HostUID[1],SysStates.HostUID[2],tickNo);
	sprintf(DatHead,"Machine:%08X%08X%08X MsgNo:%u\n",SysStates.HostUID[0],SysStates.HostUID[1],SysStates.HostUID[2],MessageNo);

	if( xSemaphoreTake( xBinaryHandleWIFINETCONNECT, portMAX_DELAY ) == pdTRUE ){

	}

    for(;;)
    {
    	if(xQueueReceive(xQueueWifiUploadMsg,&hFRestouploadMsg,pdMS_TO_TICKS(1000))==pdPASS && (SysStates.wifistates&WIFINETCONNECT)){
			char tempbuf[256];
			do{
				sprintf(tempbuf,"0:/3.Result/%s/%s.txt",hFRestouploadMsg.dirname,hFRestouploadMsg.filename);
				fRet = f_open(&hFile,tempbuf,FA_OPEN_EXISTING|FA_READ);
				if(fRet!=FR_OK){
					assert_failed(__FILE__,__LINE__);
					while(f_open(&hFile,tempbuf,FA_OPEN_EXISTING|FA_READ)>0);
				}

	    		//=========   send message  head first  macid+msgno
				sprintf(DatHead+39,"%u\n",MessageNo);				//=============  后续添加Message正确接收后，MessageNo增1，否则重新发送==================
				HAL_UART_Transmit_DMA(&WifiPort,DatHead,strlen(DatHead));			//缓存在栈，注意不能把栈分配到ccm，否则出错
				vTaskDelay(pdMS_TO_TICKS(50));
//				WifiUartSend(DatHead,strlen(DatHead)+1);

				uint32_t br=0,allbr=0;
				for(int i=0;i<8*1024/sizeof(dmabuf);i++){
					if(allbr < hFile.fsize){
						fRet = f_read(&hFile,dmabuf,512,&br);
						if(fRet!=FR_OK){
							assert_failed(__FILE__,__LINE__);
							while(f_read(&hFile,dmabuf,512,&br)>0);
						}
						if(br<512){
							memset(dmabuf+br,0,512-br);
						}
					}
					else
					{
						memset(dmabuf,0,512);
					}

					/* upload the test result */
					HAL_UART_Transmit_DMA(&WifiPort,dmabuf,512);		//change
					vTaskDelay(pdMS_TO_TICKS(100));					//512B->115200->理论值约44.5ms，可以在这里加快速度
				}
				fRet = f_close(&hFile);
				if(fRet!=FR_OK){
					assert_failed(__FILE__,__LINE__);
					while(f_close(&hFile)>0);
				}

				//if image saves,send
				{
					sprintf(tempbuf,"0:/3.Result/%s.dat",hFRestouploadMsg.filename);
					fRet = f_open(&hFile,tempbuf,FA_OPEN_EXISTING|FA_READ);
					if(fRet==FR_OK){
						for(int i=0;i<32*1024/sizeof(dmabuf);i++){
							fRet = f_read(&hFile,dmabuf,512,&br);
							if(fRet!=FR_OK){
								assert_failed(__FILE__,__LINE__);
								while(f_read(&hFile,dmabuf,512,&br)>0);
							}
							HAL_UART_Transmit_DMA(&WifiPort,dmabuf,512);
							vTaskDelay(pdMS_TO_TICKS(100));			//512B->115200->理论值约44.5ms，可以在这里加快速度
						}
					}
				}
//============================================================================
				memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
				HAL_UART_Receive_IT(&WifiPort,WifiRxBuf.dat,1);
				//if recevive OK within 1000ms
#define MAX_PEEKTIMES		2
				int i=0;
				for(i=0;i<MAX_PEEKTIMES;i++){
					if(WifiRxAnalyse(1,MessageNo)==0){
						MessageNo++;
						i=0;
						break;
					}
					else if(WifiRxAnalyse(1,MessageNo)>0){
						i=MAX_PEEKTIMES-1;
					}
					else{
						vTaskDelay(pdMS_TO_TICKS(200));
					}
				}

				HAL_UART_AbortReceive_IT(&WifiPort);
				memset(&WifiRxBuf,0,sizeof(WifiRxBuf));

				if(i>=MAX_PEEKTIMES){
					continue;								//目前就一次
				}else if(i==0){
					break;
				}
//============================================================================

			}while(0);												//目前无论如何，数据只发一次，需要联机调试，单机时间上达不到要求

    	}


		if(HAL_GetTick() >= NextTickToSendWifiTick){
			//use to recv ack
			memset(&WifiRxBuf,0,sizeof(WifiRxBuf));						//可以不接受应答
			HAL_UART_Receive_IT(&WifiPort,WifiRxBuf.dat,1);

			assert_failed("Sending Wifi Tick...\r\n",0);
			sprintf(Mac+39,"%u\n",tickNo++);
			WifiUartSend(Mac,strlen(Mac));
			HAL_UART_AbortTransmit(&WifiPort);
			NextTickToSendWifiTick=HAL_GetTick()+WIFITICKPERIOD*1000;		//已将1S从tick延时中剔除，可以近似的保证时间间隔准确性
			vTaskDelay(pdMS_TO_TICKS(1000));
			if(WifiRxAnalyse(2,tickNo)==10){
				tickbreakingtimes = 0;
			}else{
				tickbreakingtimes++;
			}
			HAL_UART_AbortReceive_IT(&WifiPort);
			memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
		}
    }
}

//考虑结果必须正确保存保存，采用串行方式，hTeststate不能释放和修改
static int32_t SaveTestResult(char *testfile,char *dirname,xTestState *hteststate,xAllTestContents *pAllRecords)
{
	FIL hResultFile;
	FRESULT retFR=0;
	uint32_t brw=0;
	char acharbuf[512]={0};
	char *pdataarrange=(char *)pAllRecords->hResultSaves.resultstart;


	sprintf(pdataarrange,"TZ Module Test Result  --start");
	sprintf(acharbuf,"  -IsSensor %c",hteststate->moduletype>=MODULE_TYPE_SENSOR?'Y':'N');strcat(pdataarrange,acharbuf);
	sprintf(acharbuf,"  -IsPassive %c",hteststate->moduletype>MODULE_TYPE_ACT?'Y':'N');	strcat(pdataarrange,acharbuf);
	sprintf(acharbuf,"  -HostID %x%x%x",SysStates.HostUID[0],SysStates.HostUID[1],SysStates.HostUID[2]);strcat(pdataarrange,acharbuf);

	sprintf(acharbuf,"  -HostVersion %s","debugV0.1.0");strcat(pdataarrange,acharbuf);
	sprintf(acharbuf,"  -ModuleName %s",hteststate->theResult->Models);strcat(pdataarrange,acharbuf);
	sprintf(acharbuf,"  -TestName %s",hteststate->theResult->testername);strcat(pdataarrange,acharbuf);
	sprintf(acharbuf,"  -TestID %s",hteststate->theResult->testerid);strcat(pdataarrange,acharbuf);
	sprintf(acharbuf,"  -Result %s",strErrorTxt[hteststate->errorNo]);strcat(pdataarrange,acharbuf);

	//================          针对不同的测试对象，填写对应结果           ===========================
	if(hteststate->moduletype < MODULE_TYPE_SENSOR){							//标准的模组方案
		sprintf(acharbuf,"  -ModuleID %x%x%x%x%x%x%x%x",\
				hteststate->theResult->ModuleUID[0],hteststate->theResult->ModuleUID[1],\
				hteststate->theResult->ModuleUID[2],hteststate->theResult->ModuleUID[3],\
				hteststate->theResult->ModuleUID[4],hteststate->theResult->ModuleUID[5],\
				hteststate->theResult->ModuleUID[6],hteststate->theResult->ModuleUID[7]);
		strcat(pdataarrange,acharbuf);

		sprintf(acharbuf,"  -CheckBinFile %s",hteststate->theResult->Binfiles);strcat(pdataarrange,acharbuf);
		sprintf(acharbuf,"  -Touchout %c",(hteststate->testconfigs&CONFIG_TOUCH)>0?(hteststate->theResult->isTouchPass > 2 ? 'Y' : 'N'):'-');strcat(pdataarrange,acharbuf);
		sprintf(acharbuf,"  -CaliBaudRate %c (%u)",(hteststate->testconfigs&CONFIG_CALIBAUD)>0?hteststate->theResult->isCaliBaudrateOK >0 ? 'Y' : 'N':'-',\
				(hteststate->testconfigs&CONFIG_CALIBAUD)>0?hteststate->targetbaudrate:0);strcat(pdataarrange,acharbuf);
		sprintf(acharbuf,"  -StartDuartion %c (%u)",\
						(hteststate->testconfigs&CONFIG_STARTDUARTION)>0?hteststate->theResult->StartDuartion <= hteststate->StdStartDuartion ? 'Y' : 'N':'-',\
								(uint32_t)(hteststate->theResult->StartDuartion&0x000000FF));strcat(pdataarrange,acharbuf);
		strcat(pdataarrange,"  -ModuleVersion ");
		strcat(pdataarrange,hteststate->theResult->ModuleVersionMessage);

		sprintf(acharbuf,"  -CheckXorVal %c",(hteststate->testconfigs&CONFIG_XORCHECK)>0?hteststate->theResult->isXorCheckOK>0?'Y':'N':'-');strcat(pdataarrange,acharbuf);
		sprintf(acharbuf,"  -ModuleFlash %c%c%c",\
						(hteststate->testconfigs&CONFIG_FLASH)>0?hteststate->theResult->reguserstate > 0?'Y':'N':'-',\
						(hteststate->testconfigs&CONFIG_FLASH)>0?hteststate->theResult->verifystate > 0?'Y':'N':'-',\
						(hteststate->testconfigs&CONFIG_FLASH)>0?hteststate->theResult->deluserstate > 0?'Y':'N':'-');
		strcat(pdataarrange,acharbuf);
		sprintf(acharbuf,"  -ModuleLed %c",(hteststate->testconfigs & CONFIG_LED) > 0?hteststate->theResult->ledstate > 0?'Y':'N':'-');strcat(pdataarrange,acharbuf);
		sprintf(acharbuf,"  -GetImage %c,%d,%d",(hteststate->testconfigs & CONFIG_GETIMAGE) > 0?hteststate->theResult->isGetImgOK > 0?'Y':'N':'-',\
						(hteststate->testconfigs & CONFIG_GETIMAGE) > 0?hteststate->theResult->FgpWidth:0,\
						(hteststate->testconfigs & CONFIG_GETIMAGE) > 0?hteststate->theResult->FgpHeight:0);
		strcat(pdataarrange,acharbuf);
		if(hteststate->testconfigs & CONFIG_TVCCPOWER){
			sprintf(acharbuf,"  -TouchCurrent Threshold_default=%.3f,Threshold_withurge=%.3f,step=%u,data=",\
					hteststate->staticpowerThreshold_withurge,\
					hteststate->staticpowerThreshold_default,\
					hteststate->staticpowerstep);	//touchvcc采样间隔
		}else{
			sprintf(acharbuf,"  -TouchCurrent -");
		}
		strcat(pdataarrange,acharbuf);

		if(hteststate->testconfigs & CONFIG_TVCCPOWER){
			float *ptempfloat=(float*)hteststate->theResult->staticpower->datastart;
			for (int i=0;i<hteststate->theResult->staticpower->len;i++){
				if(i!=hteststate->theResult->staticpower->len-1){
					sprintf(acharbuf,"%.3f,",*ptempfloat);
				}else{
					sprintf(acharbuf,"%.3f ",*ptempfloat);
				}
				ptempfloat++;
				strcat(pdataarrange,acharbuf);
			}
		}

		if(hteststate->testconfigs & CONFIG_MVCCPOWER){
			sprintf(acharbuf,"  -ModuleCurrent Threshold_default=%.1f,Threshold_max=%.1f,step=%u,data=",\
					hteststate->dynamicpowerThreshold_default,\
					hteststate->dynamicpowerThreshold_withurge,\
					hteststate->dynamicpowerstep);	//touchvcc采样间隔
		}else{
			sprintf(acharbuf,"  -ModuleCurrent -");
		}
		strcat(pdataarrange,acharbuf);

		if(hteststate->testconfigs & CONFIG_MVCCPOWER){
			float *ptempfloat=(float*)hteststate->theResult->dynamicpower->datastart;
			for (int i=0;i<hteststate->theResult->dynamicpower->len;i++){
				if(i!=hteststate->theResult->dynamicpower->len-1){
					sprintf(acharbuf,"%.1f,",*ptempfloat);
				}else{
					sprintf(acharbuf,"%.1f ",*ptempfloat);
				}
				ptempfloat++;
				strcat(pdataarrange,acharbuf);
			}
		}

		strcat(pdataarrange,"--End");

		uint8_t ResultXor=0,ImageXor=0;
		uint32_t datapackagelen=strlen(pdataarrange);
		for(int i=0;i<datapackagelen;i++){
			ResultXor^=pdataarrange[i];
		}

		if(hteststate->theResult->isGetImgOK > 0){
			uint8_t *p=(uint8_t*)hteststate->theResult->pImage;
			for(int i=0;i<32*1024/4;i++){
				ImageXor^=*p++;
				ImageXor^=*p++;
				ImageXor^=*p++;
				ImageXor^=*p++;
			}
		}

		sprintf(acharbuf,"  -ResultXor=%#X,-ImageXor=%#X \n\0",ResultXor,ImageXor);
		strcat(pdataarrange,acharbuf);
		strcat(pdataarrange,"\0");
	}

#ifdef __PRINTSAV
		assert_failed("data package OK\r\n",0);
#endif

	GetCalendar(&SysStates.Calendar);
	sprintf(testfile,"%X%X%X_%d.%d.%d_%d-%d-%d",SysStates.HostUID[0],SysStates.HostUID[1],SysStates.HostUID[2],\
			SysStates.Calendar.hdate.Year,SysStates.Calendar.hdate.Month,SysStates.Calendar.hdate.Date,\
						SysStates.Calendar.htime.Hours,SysStates.Calendar.htime.Minutes,SysStates.Calendar.htime.Seconds);

	char datedir[20]={0};		//="19-08-10";	//19-08-10(a)
	static uint32_t directoryExistMark = 0;

	//mkdirs
	if(directoryExistMark != SysStates.Calendar.hdate.Year*10000+SysStates.Calendar.hdate.Month*100+SysStates.Calendar.hdate.Date){
		sprintf(acharbuf,"0:/3.Result/%d-%d-%d",SysStates.Calendar.hdate.Year,SysStates.Calendar.hdate.Month,SysStates.Calendar.hdate.Date);
		retFR = f_mkdir(acharbuf);
		while(retFR!=FR_OK && retFR!=FR_EXIST){
			vTaskDelay(pdMS_TO_TICKS(1000));
			assert_failed(__FILE__,__LINE__);
			assert_failed("Error No",retFR);
			retFR = f_mkdir(acharbuf);
		}
		sprintf(acharbuf,"0:/3.Result/%d-%d-%d(a)",SysStates.Calendar.hdate.Year,SysStates.Calendar.hdate.Month,SysStates.Calendar.hdate.Date);
		retFR = f_mkdir(acharbuf);
		while(retFR!=FR_OK &&retFR!=FR_EXIST){
			vTaskDelay(pdMS_TO_TICKS(1000));
			assert_failed(__FILE__,__LINE__);
			assert_failed("Error No",retFR);
			retFR = f_mkdir(acharbuf);
		}

		directoryExistMark = SysStates.Calendar.hdate.Year*10000+SysStates.Calendar.hdate.Month*100+SysStates.Calendar.hdate.Date;
	}

	//choose directory
	if(SysStates.Calendar.htime.Hours >7 && SysStates.Calendar.htime.Hours <21) sprintf(datedir,"%d-%d-%d",SysStates.Calendar.hdate.Year,SysStates.Calendar.hdate.Month,SysStates.Calendar.hdate.Date);
	else																		sprintf(datedir,"%d-%d-%d(a)",SysStates.Calendar.hdate.Year,SysStates.Calendar.hdate.Month,SysStates.Calendar.hdate.Date);

	sprintf(dirname,"%s",datedir);

	sprintf(acharbuf,"0:/3.Result/%s/%s.txt",datedir,testfile);
	retFR = f_open(&hResultFile,acharbuf,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
	if(retFR!= FR_OK){
			assert_failed("Error No",retFR);
			if(retFR == FR_NO_PATH){					//could remove
				sprintf(acharbuf,"0:/3.Result/%s",datedir);
				retFR = f_mkdir(acharbuf);
				while(retFR != FR_OK){
					vTaskDelay(pdMS_TO_TICKS(1000));
					assert_failed(__FILE__,__LINE__);
					assert_failed("Error No",retFR);
					retFR = f_mkdir(acharbuf);
				}

				sprintf(acharbuf,"0:/3.Result/%s/%s.txt",datedir,testfile);
			}

			do{
				retFR = f_open(&hResultFile,acharbuf,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
				if(retFR!=FR_OK){
					assert_failed(__FILE__,__LINE__);
					assert_failed("Error No",retFR);
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
			}while(retFR!=FR_OK);
/*
	while(f_open(&hResultFile,acharbuf,FA_OPEN_ALWAYS|FA_WRITE|FA_READ) > FR_OK){
			assert_failed(__FILE__,__LINE__);
			assert_failed("Error No",retFR);
			vTaskDelay(pdMS_TO_TICKS(1000));
		}
*/
	}
	retFR = f_lseek(&hResultFile,0);
	if(retFR!= FR_OK){
		assert_failed(__FILE__,__LINE__);
		while(f_lseek(&hResultFile,0) > FR_OK);
	}
	retFR = f_write(&hResultFile,pdataarrange,strlen(pdataarrange),&brw);
	if(retFR!= FR_OK){
		assert_failed(__FILE__,__LINE__);
		while(f_write(&hResultFile,acharbuf,strlen(acharbuf),&brw) > FR_OK);
	}
	retFR = f_close(&hResultFile);
	if(retFR!= FR_OK){
		assert_failed(__FILE__,__LINE__);
		while(f_close(&hResultFile) > FR_OK);
	}

#ifdef __PRINTSAV
		assert_failed("save test para result OK\r\n",0);
#endif

	//Image saves
	if(hteststate->theResult->isGetImgOK > 0 ){
		sprintf(acharbuf,"0:/3.Result/%s/%s.dat",datedir,testfile);
		retFR = f_open(&hResultFile,acharbuf,FA_OPEN_ALWAYS|FA_WRITE);
		if(retFR!= FR_OK){
			assert_failed("Error No",retFR);
			do{
				retFR = f_open(&hResultFile,acharbuf,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
				assert_failed(__FILE__,__LINE__);
				assert_failed("Error No",retFR);
				vTaskDelay(pdMS_TO_TICKS(1000));
			}while(retFR!=FR_OK);

/*
			assert_failed(__FILE__,__LINE__);
			while(f_open(&hResultFile,acharbuf,FA_OPEN_ALWAYS|FA_WRITE) > FR_OK)
				vTaskDelay(pdMS_TO_TICKS(1000));
*/

		}
		retFR = f_lseek(&hResultFile,0);
		if(retFR!= FR_OK){
			assert_failed(__FILE__,__LINE__);
			while(f_lseek(&hResultFile,0) > FR_OK);
		}
		uint8_t *p=(uint8_t *)hteststate->theResult->pImage;
		if(p == NULL){
			assert_failed("NULL pointer\r\n",0);while(1);
		}
		retFR = f_write(&hResultFile,p,(pAllRecords->hImgSaves.width*pAllRecords->hImgSaves.height+3),&brw);
		if(retFR!= FR_OK){
			assert_failed(__FILE__,__LINE__);
			while(f_write(&hResultFile,p,(pAllRecords->hImgSaves.width*pAllRecords->hImgSaves.height+3),&brw) > FR_OK);
		}
		retFR = f_close(&hResultFile);
		if(retFR!= FR_OK){
			assert_failed(__FILE__,__LINE__);
			while(f_close(&hResultFile) > FR_OK);
		}
		*p = 0;
		p+=hteststate->theResult->FgpHeight*hteststate->theResult->FgpWidth+3;
		*--p=0;*--p=0;
	}
#ifdef __PRINTSAV
		assert_failed("save test imge result OK\r\n",0);
#endif
	return 0;
}

void vTaskModuleFunc( void * pvParameters )
{
	uint8_t temptodel[20];
	BaseType_t udRet=0;
	xCurrentQueueMessage TouchVccMsg={0},ModuleVccMsg={0};
	TouchVccMsg.sampletimes = TouchVccMsg.samplevalidindex = 0;
	ModuleVccMsg.sampletimes = 7; ModuleVccMsg.samplevalidindex = 4;
	TouchVccMsg.std_short_circuit = ModuleVccMsg.std_short_circuit = 50.000;
	TouchVccMsg.SinglePeek = ModuleVccMsg.SinglePeek = 0.000;
	TouchVccMsg.hCurrentDetection.Frequency = hteststate.staticpowerstep;
	ModuleVccMsg.hCurrentDetection.Frequency = hteststate.dynamicpowerstep;
	TouchVccMsg.hCurrentDetection.Switch = ModuleVccMsg.hCurrentDetection.Switch =  0;
	TouchVccMsg.hCurrentDetection.writeresult = ModuleVccMsg.hCurrentDetection.writeresult = power_write;
	TouchVccMsg.hCurrentDetection.readresult = ModuleVccMsg.hCurrentDetection.readresult = power_read;
	TouchVccMsg.hCurrentDetection.clearresult = ModuleVccMsg.hCurrentDetection.clearresult = power_clear;

	xScreenMsg ScreenMsg={0};
	xrecordparas settings={.proportion=2};

	for(;;)
	{
		while(hteststate.isTestStart == 0){			//wait the start key down
			vTaskDelay(pdMS_TO_TICKS(200));
		}
		assert_failed("ModuleFunc started \r\n",0);

		if(xTestList[0].TestFunc == ERR){
			hteststate.isTestStart = 0;
			continue;
		}

		taskENTER_CRITICAL();
		pAllRecords = createrecord(&settings);
		taskEXIT_CRITICAL();
		if(pAllRecords==NULL){
			assert_failed(__FILE__,__LINE__);
			assert_failed("malloc charge !!!\r\n",0);
			while(1);
		}else{
			TouchVccMsg.hResultSav = &pAllRecords->hTVccSaves;
			ModuleVccMsg.hResultSav = &pAllRecords->hMVccSaves;
			TouchVccMsg.hResultSav->dattype=FLOATTYPE;
			ModuleVccMsg.hResultSav->dattype=FLOATTYPE;

			memset(pAllRecords->hTVccSaves.datastart,0,MAX_TVCCSAVE_LEN);
			memset(pAllRecords->hMVccSaves.datastart,0,MAX_MVCCSAVE_LEN);
			memset(pAllRecords->hResultSaves.resultstart,0,MAX_RESULT_LEN);
		}



		hteststate.isqualified=1;
		SysStates.Teststate = TESTTING;

		ScreenMsg.MsgType = TestMsgToUser;
		ScreenMsg.SignalNo = TESTTING;
		xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);


//#ifdef __DEBUG
#if 0
		hteststate.testconfigs&=~(CONFIG_TVCCPOWER/*|CONFIG_MVCCPOWER*/);
#endif
		if(hteststate.testconfigs&CONFIG_TVCCPOWER){
			//start the touch out current ammeter
			TouVccOn();ModuleVccOff();		// open touch power ...
			TouchVccMsg.signal=AmOpen;
			TouchVccMsg.hCurrentDetection.Frequency = 166;
			xQueueReset(xQueueTouchVccCtrlMsg);
			if(xQueueSend(xQueueTouchVccCtrlMsg,&TouchVccMsg,portMAX_DELAY)!=pdPASS)
			{
				assert_failed("touchvcc power msg send fail \r\n",0);
				while(1);
			}
			//等待触控的静态电流和动态电流测试完毕
			vTaskDelay(pdMS_TO_TICKS(3*TouchVccMsg.hCurrentDetection.Frequency));
			while(\
					xQueuePeek(xQueueTouchVccCtrlMsg,&TouchVccMsg,pdMS_TO_TICKS(TouchVccMsg.hCurrentDetection.Frequency))==pdPASS\
					&& TouchVccMsg.signal==AmOpen\
					)
			{
				vTaskDelay( 2*pdMS_TO_TICKS(TouchVccMsg.hCurrentDetection.Frequency) );
			}

			while(xQueueReceive(xQueueTouchVccCtrlMsg,&TouchVccMsg,portMAX_DELAY)!=pdPASS){
				assert_failed(__FILE__,__LINE__);
				vTaskDelay(pdMS_TO_TICKS(TouchVccMsg.hCurrentDetection.Frequency));
			}

			hteststate.errorNo = eMET_Touchout;
			if(TouchVccMsg.signal != AmAck){								//等待返回有效信号，touch vcc 检测完毕
				if(TouchVccMsg.signal != AmShortCurrent){
					assert_failed(__FILE__,__LINE__);
					while(1);											//除非通信队列时间交错
				}
				else{													//检测到短路
					hteststate.isqualified=0;
					hteststate.theResult->isShortCircuit = 1;			//短路结束
					//结束
				}
			}
			else{
				hteststate.theResult->isShortCircuit = 0;
#ifdef __DEBUG
			hteststate.theResult->isTouchPass = 3;
#endif
				if(hteststate.theResult->isTouchPass < 3){
					hteststate.isqualified=0;
					ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
					assert_failed("touch out times smaller than 3 times\r\n",0);
				}
				else{
					ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
					hteststate.errorNo = eMET_Pass;
				}
				ScreenMsg.MsgType = TestResItem;
				ScreenMsg.SignalNo = INDEX_TOUCHOUT;
				xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
			}

		}


		if(hteststate.testconfigs&CONFIG_MVCCPOWER){
			//if touch out test pass,then open module current ammeter ina219
			//需要等待touch vcc的触控电流检测完成后再打开模组电源并将进行测试，由于电流表的采样速度较慢，只能多等待一下
			if(hteststate.isqualified != 0){
				TouVccOn();ModuleVccOn();
				ModuleVccMsg.signal = AmOpen;
				ModuleVccMsg.hCurrentDetection.Frequency = 50;
				xQueueReset(xQueueModuleVccCtrlMsg);
				if(xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY)!=pdPASS)
				{
					assert_failed("modulevcc power msg send fail \r\n",0);
					while(1);
				}
				//等待INA219 开启
				while(\
						xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency))==pdPASS\
						&& ModuleVccMsg.signal==AmOpen\
						)
				{
					vTaskDelay( 2*pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency) );
				}

				if(xQueueReceive(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY)==pdPASS && ModuleVccMsg.signal==AmAck){		//电流表已开启

				}
				else {
					assert_failed(__FILE__,__LINE__);while(1);
				}
			}
		}

		//开始测试项
		if(hteststate.isqualified != 0){
			TouVccOn();ModuleVccOn();
			uint8_t TestItemIndex=0;
			uint32_t result;
		do{
			switch(xTestList[TestItemIndex].TestFunc)   //CMD:TZst_xxxx
			{
				case C_CALIBOUDRATE:
					HAL_UART_Receive(&ModulePort,temptodel,20,5);		//用于清除上次未接收完全等错误
					assert_failed("\r\n\r\n C_CALIBOUDRATE\r\n",0);
					uint32_t udret,res;
					hteststate.errorNo = eMET_CaliBaudRate;
					hteststate.isqualified=0;
					hteststate.theResult->isCaliBaudrateOK=0;

#if 0	//old ways
					//change host uart baudrate to 9600
					if(ModulePort.Init.BaudRate != 9600){
						HAL_UART_AbortReceive(&ModulePort);
						HAL_UART_DeInit(&ModulePort);
						ModulePort.Init.BaudRate=9600;
						if(HAL_UART_Init(&ModulePort)!=HAL_OK){
							assert_failed(__FILE__,__LINE__);
							while(HAL_UART_Init(&ModulePort)>0);
						}
					}

					//cali module baudrate
					udret = cmdCalibrationBaudRate(&res);
					if(udret!=0){
						assert_failed("Module Open fail\r\n",0);
						assert_failed(__FILE__,__LINE__);
					}else{
						if(res!=0){
							assert_failed("Module Baudrate cali fail\r\n",0);
							assert_failed(__FILE__,__LINE__);
						}else{
							assert_failed("Module Baudrate cali success\r\n",0);
						}
					}

					//change module forever
					if((udret|res)==0){
						vTaskDelay(pdMS_TO_TICKS(100) );
						udret = cmdChangeModuleBaudrate(hteststate.targetbaudrate,1);
						if(udret == 0){
							HAL_UART_AbortReceive(&ModulePort);
							HAL_UART_DeInit(&ModulePort);
							ModulePort.Init.BaudRate=hteststate.targetbaudrate;
							if(HAL_UART_Init(&ModulePort)!=HAL_OK){
								assert_failed(__FILE__,__LINE__);
								while(HAL_UART_Init(&ModulePort)>0);
							}
							hteststate.theResult->isCaliBaudrateOK = 1;
							hteststate.isqualified++;
							hteststate.errorNo = eMET_Pass;

						}else{
							assert_failed("Module uart bdrt change forever fail\r\n",0);
						}
					}
#else	//new way,host search the module baudrate ,and send the real baudrate to module
		//	OEM_Ack_Command(0xa0,caliBaud>>8,caliBaud,0);  para1，2:正负偏移波特率，para3:信号，继续，完成退出等
					//change host uart baudrate to target baudrate
					uint32_t resolution = 0 ,basebaudrate = 115200;////以115200波特率测试
					int offsetBaudrate = 0;
					if(hteststate.targetbaudrate != 115200){		//upper to 115200 first
						udret = cmdAdapterModuleBaudrate(hteststate.targetbaudrate + offsetBaudrate,resolution);
						vTaskDelay(200);
						udret = cmdChangeModuleBaudrate(115200,0);
						if(udret == 1){
							goto jmp1108;
						}
					}

					for(int i=0;i<10;i++){
						udret = cmdAdapterModuleBaudrate(basebaudrate + offsetBaudrate,resolution);
						assert_failed("connect baudrate is (0)",udret);
						if(cmdCaliBaud(0,&offsetBaudrate) != 0){
							udret = 1;
							assert_failed(__FILE__,__LINE__);
							continue;
						}else{
							assert_failed("offset baud is ",offsetBaudrate);			//比实际115200 快或慢 xx 波特率
							if(abs(offsetBaudrate)*20 < basebaudrate )	resolution=1;	// 5%
							if(abs(offsetBaudrate)*1000 > 5*115200){			//目前只能到1%以内，达不到0.5%，需要样品统计有效性规律
								continue;
							}else{
//								assert_failed("final baud rate is ",udret);
								if(cmdCaliBaud(1,&offsetBaudrate) != 0){
									assert_failed(__FILE__,__LINE__);
									udret = 1;continue;
								}else{
									udret = 0;break;
								}
							}

						}
					}

jmp1108:
					if (udret == 0) {
						hteststate.theResult->isCaliBaudrateOK = 1;
						hteststate.isqualified++;
						hteststate.errorNo = eMET_Pass;

						if(ModulePort.Init.BaudRate != basebaudrate){
							HAL_UART_Abort(&ModulePort);
							HAL_UART_DeInit(&ModulePort);
							ModulePort.Init.BaudRate = basebaudrate;
							if(HAL_UART_Init(&ModulePort)!=HAL_OK){
								assert_failed(__FILE__,__LINE__);
								while(HAL_UART_Init(&ModulePort)>0);
							}
						}
					} else {
						assert_failed("Module uart bdrt cali fail\r\n", 0);
					}


#endif
					ScreenMsg.MsgType = TestResItem;
					ScreenMsg.SignalNo = INDEX_CALIBAUD;
					if(hteststate.isqualified){
						ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
					}else{
						ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
					}
					xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
					break;
				case C_STARTDUARTION:
					HAL_UART_Receive(&ModulePort,temptodel,20,5);
					assert_failed("\r\n\r\n C_STARTDUARTION\r\n",0);
    				uint32_t theduartion;
    				hteststate.errorNo = eMET_StartDuartion;
    				hteststate.isqualified=0;						//标记不通过
    				hteststate.theResult->StartDuartion = 0;
    				udRet = cmdGetModuleUserNums(10,&theduartion);	//超时5次
    				if(udRet == 0)		//正确打开模组
    				{
    					hteststate.theResult->StartDuartion = (uint8_t)theduartion;	//记录时间结果
    					if(theduartion < hteststate.StdStartDuartion)	//通过
    					{
    						assert_failed("start duartion passed \r\n",0);
    						udRet = cmdGetModuleUID(hteststate.theResult->ModuleUID);
    						if(udRet == 0){
    							assert_failed("geted module UID ok \r\n",0);
								hteststate.isqualified++;
								hteststate.errorNo = eMET_Pass;
    						}else{
        						assert_failed("geted module UID error \r\n",0);
        						assert_failed(__FILE__,__LINE__);
    						}
    					}
    					else					//启动时间不达标
    					{
    						assert_failed("start duartion overtime \r\n",0);
    						assert_failed(__FILE__,__LINE__);
    					}
    				}
    				else
    				{
    					assert_failed("Module open failed\r\n",0);
    					assert_failed(__FILE__,__LINE__);
    				}

    				ScreenMsg.MsgType = TestResItem;
    				ScreenMsg.SignalNo = INDEX_STARTDUARTION;
					if(hteststate.isqualified){
						ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
					}else{
						ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
					}
					xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
					break;

				case C_GET_MODULE_VERSION:
					HAL_UART_Receive(&ModulePort,temptodel,20,5);
					assert_failed("\r\n\r\n C_GET_MODULE_VERSION\r\n",0);
    				uint8_t tempbuf[256]={0};
					hteststate.errorNo = eMET_GetVersion;
    				hteststate.isqualified=0;						//标记不通过
    				udRet = cmdGetModuleVersion(tempbuf);
    				if(udRet == 0){
    					strcpy((char *)hteststate.theResult->ModuleVersionMessage,(const char *)(tempbuf+1));
    					assert_failed("\r\nget version finish \r\n",0);
    					hteststate.isqualified++;
    					hteststate.errorNo = eMET_Pass;
    				}else{
    					assert_failed("Module open failed\r\n",0);
    					assert_failed(__FILE__,__LINE__);
    				}

					ScreenMsg.MsgType = TestResItem;
					ScreenMsg.SignalNo = INDEX_GETVERSION;
					if(hteststate.isqualified){
						ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
					}else{
						ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
					}
					xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
					break;

				case C_CHECKOUTXOR:
					HAL_UART_Receive(&ModulePort,temptodel,20,5);
					assert_failed("\r\n\r\n C_CHECKOUTXOR\r\n",0);
					uint8_t xorval[12];

					hteststate.errorNo = eMET_XorCheck;
    				hteststate.isqualified=0;						//标记不通过
    				hteststate.theResult->isXorCheckOK = 0;

    				static uint8_t isXorValCaled=0;
    				static uint16_t binlen=0;
					static uint8_t Xorvals[9]={0,0,0,0,0,0,0,0,0};

    				if(!isXorValCaled){
						FIL fBinfile;

						char tempstrings[100]={0};
						 //pTestconfig->Binfiles[0]=;
						 sprintf(tempstrings,"0:/2.Moudle/%s",hteststate.theResult->Binfiles);// pTestconfig->Binfiles[0]);
						if(f_open(&fBinfile,tempstrings,FA_OPEN_EXISTING|FA_READ)!=FR_OK)
						{
							assert_failed(__FILE__,__LINE__);
							//err handle ~~s
							break;
						}
						binlen=fBinfile.fsize/120+1;

						//check bin file xor val
						for (int i=0;i<binlen-1;i++){		// -1 用于最后一包先不校验,不满120字节长度模组发送的校验值不正常！最后一包单独计算
							uint32_t BytesHasRead;
							uint8_t package[120];
							if(f_read(&fBinfile,package,120,&BytesHasRead)!=FR_OK){
								assert_failed(__FILE__,__LINE__);
							}
							if(BytesHasRead == 120){
								for(int j=0;j<BytesHasRead;j+=8){			//save time
									Xorvals[0]^=package[j];	 Xorvals[1]^=package[j+1];
									Xorvals[2]^=package[j+2];Xorvals[3]^=package[j+3];
									Xorvals[4]^=package[j+4];Xorvals[5]^=package[j+5];
									Xorvals[6]^=package[j+6];Xorvals[7]^=package[j+7];
								}
							}
							else{
								for(int j=0;j<BytesHasRead;j++){
									Xorvals[j%8]^=package[j];
								}
							}
						}
						Xorvals[8]=Xorvals[0]^Xorvals[1]^Xorvals[2]^Xorvals[3]^Xorvals[4]^Xorvals[5]^Xorvals[6]^Xorvals[7];
						isXorValCaled = 1;			//host 只计算一次即可
    				}

    				udRet = cmdGetXorVal(xorval,(uint16_t)binlen-1,0);			//最后一包不校验
    				if(udRet == 0){
    					if(memcmp(Xorvals,xorval+1,9)!=0){
    						assert_failed(__FILE__,__LINE__);
    						assert_failed("bin file xor check error\r\n",0);
    					}
    					else{
    						hteststate.theResult->isXorCheckOK = 1;
    						hteststate.isqualified++;
    						hteststate.errorNo = eMET_Pass;
    						assert_failed("bin file xor check OK\r\n",0);
    					}
    				}else
    				{
    					assert_failed(__FILE__,__LINE__);
    					assert_failed("Module open failed\r\n",0);
    				}

					ScreenMsg.MsgType = TestResItem;
					ScreenMsg.SignalNo = INDEX_XORCHECK;
					if(hteststate.isqualified){
						ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
					}else{
						ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
					}
					xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
					break;

				//先删除，再写读，再擦除
				case C_FLASH:		//默认3C3R注册方式 010203
					HAL_UART_Receive(&ModulePort,temptodel,20,5);
					assert_failed("\r\n\r\n C_FLASH\r\n",0);

					hteststate.errorNo = eMET_Flash;
    				hteststate.isqualified=0;						//标记不通过
#if 1
    				do{
    					uint8_t badcnt,allcnt;
    					udRet = cmdFlashCheck(&badcnt,&allcnt);
    					if(udRet!=0){
        					assert_failed("Module open failed\r\n",0);
        					assert_failed(__FILE__,__LINE__);
    					}else{
							hteststate.isqualified++;
							hteststate.theResult->reguserstate = hteststate.theResult->verifystate = hteststate.theResult->deluserstate = 1;
							hteststate.errorNo = eMET_Pass;
							assert_failed("Flash test OK\r\n",0);
    					}
    				}while(0);
#else		//origin use
    				do{
						hteststate.theResult->reguserstate = 0;
						udRet = cmdDelAllUser(&result);					//先清除，保证能够写入ID为1的特征值
						if(udRet || result){
							assert_failed(__FILE__,__LINE__);
							assert_failed("Module open fail\r\n",0);
							break;
						}

						udRet = cmdWriteFreture(&result,(uint8_t *)STDFEATRUE,8192);
						if(udRet == 0){
							if(result == 0){
	//							hteststate.isqualified++;
								hteststate.theResult->reguserstate = 1;
	//							hteststate.errorNo = eMET_Pass;
								assert_failed("write fea OK\r\n",0);
							}
							else{
								assert_failed("write fea fail\r\n",0);
								assert_failed(__FILE__,__LINE__);
								break;
							}
						}else{
							assert_failed("Module open fail\r\n",0);
							assert_failed(__FILE__,__LINE__);
							break;
						}

						//测试读flash
						hteststate.theResult->verifystate = 0;
						udRet = cmdReadFreture(&result,0);
						if(udRet == 0){
							if(result == 0){
								assert_failed("read featrue OK\r\n",0);
	//							printf("\r\n\r\n\r\n");			//	特征值查看
	//							for(int i=4;i<8206-8-2;i++){
	//								printf("%#x,",*(uint8_t *)(FGPFEA_ADDR+i));
	//							}
	//							printf("\r\n\r\n\r\n");

								//特征值比较
								if(memcmp((uint8_t *)pAllRecords->hImgSaves.oriImgaddr+4,(uint8_t *)STDFEATRUE,8192)!=0){
									assert_failed("featrue data error\r\n",0);
									assert_failed(__FILE__,__LINE__);
									break;
								}else{
									assert_failed("featrue correct\r\n",0);
	//								hteststate.isqualified++;
									hteststate.theResult->verifystate = 1;
	//								hteststate.errorNo = eMET_Pass;
								}
							}else{
								assert_failed("communication error\r\n",0);
								assert_failed(__FILE__,__LINE__);
								break;
							}
						}else{
							assert_failed("Module open fail\r\n",0);
							assert_failed(__FILE__,__LINE__);
							break;
						}


						hteststate.theResult->deluserstate = 0;
						udRet = cmdDelAllUser(&result);
						if(udRet == 0){
							if(result == 0){
								hteststate.isqualified++;
								hteststate.theResult->deluserstate = 1;
								hteststate.errorNo = eMET_Pass;
								assert_failed("delete all fea OK\r\n",0);
							}
							else{
								assert_failed("delete fea fail\r\n",0);
								assert_failed(__FILE__,__LINE__);
								break;
							}
						}else
						{
							assert_failed("Module open fail\r\n",0);
							assert_failed(__FILE__,__LINE__);
							break;
						}

    				}while(0);
#endif
					ScreenMsg.MsgType = TestResItem;
					ScreenMsg.SignalNo = INDEX_FLASH;
					if(hteststate.isqualified){
						ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
					}else{
						ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
					}
					xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
					break;

			case C_GET_FINGER_IMAGE:
				HAL_UART_Receive(&ModulePort,temptodel,20,5);
				assert_failed("\r\n\r\n C_GET_FINGER_IMAGE\r\n",0);

				hteststate.errorNo = eMET_GetIMAGE;
				hteststate.isqualified=0;						//标记不通过
				hteststate.theResult->isGetImgOK = 0;

				SysStates.Teststate = TESTWAITPRESS;
				ScreenMsg.MsgType = TestMsgToUser;
				ScreenMsg.SignalNo = TESTWAITPRESS;
				xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
				while(SysStates.Teststate != TESTTING){
					vTaskDelay(pdMS_TO_TICKS(100));
				}

//				uint8_t width,height;
				LinkIronRing_Sigel();vTaskDelay(pdMS_TO_TICKS(40));
				udRet = cmdGetFingerImage(&pAllRecords->hImgSaves.width,&pAllRecords->hImgSaves.height,(uint8_t *)pAllRecords->hImgSaves.oriImgaddr);
				UnlinkIronRing_Sigel();
				hteststate.theResult->FgpWidth = pAllRecords->hImgSaves.width;
				hteststate.theResult->FgpHeight = pAllRecords->hImgSaves.height;
				hteststate.theResult->pImage = NULL;
				if(udRet == 0){
					if(pAllRecords->hImgSaves.width > 0 && pAllRecords->hImgSaves.height > 0){
						hteststate.isqualified++;
						hteststate.theResult->isGetImgOK = 1;
						hteststate.errorNo = eMET_Pass;
						hteststate.theResult->pImage = (uint8_t *)pAllRecords->hImgSaves.oriImgaddr;
						assert_failed("get image OK\r\n",0);
					}
					else{
						assert_failed("get image fail\r\n",0);
						assert_failed(__FILE__,__LINE__);
					}
				}else if(udRet == 1)
				{
					assert_failed("Module open fail\r\n",0);
					assert_failed(__FILE__,__LINE__);
				}else if(udRet == 2)
				{
					assert_failed("can't get image,interrupt it\r\n",0);
					assert_failed(__FILE__,__LINE__);
				}

				ScreenMsg.MsgType = TestResItem;
				ScreenMsg.SignalNo = INDEX_IMAGE;
				if(hteststate.isqualified){
					ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
				}else{
					ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
				}
				xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
				break;
			default:
				break;
			}

			if(hteststate.isqualified == 0)		//出现不合格
			{
				break;
			}
    	}while(xTestList[TestItemIndex++].Next != NULL);
    	TestItemIndex = 0;
		}
		//after power
		if(hteststate.testconfigs&CONFIG_MVCCPOWER){
			//关闭INA219 芯片
			ModuleVccMsg.signal=AmClose;
			xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);
			while(\
					xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency))==pdPASS\
					&& ModuleVccMsg.signal==AmClose\
					)
			{
				vTaskDelay( pdMS_TO_TICKS(100) );
			}

			if(xQueueReceive(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY)==pdPASS && ModuleVccMsg.signal == AmGetResult){
				assert_failed("INA219 module Close\r\n",0);
			}else{
				assert_failed(__FILE__,__LINE__);while(1);
			}
		}

		if(hteststate.testconfigs&CONFIG_TVCCPOWER){
			//关闭电流表
			TouchVccMsg.signal=AmClose;
			xQueueSend(xQueueTouchVccCtrlMsg,&TouchVccMsg,portMAX_DELAY);
			while(\
					xQueuePeek(xQueueTouchVccCtrlMsg,&TouchVccMsg,pdMS_TO_TICKS(TouchVccMsg.hCurrentDetection.Frequency))==pdPASS\
					&& TouchVccMsg.signal==AmClose\
					)
			{
				vTaskDelay(pdMS_TO_TICKS(100) );
			}

			if(xQueueReceive(xQueueTouchVccCtrlMsg,&TouchVccMsg,portMAX_DELAY)==pdPASS && TouchVccMsg.signal == AmGetResult){
				assert_failed("AmCurrent Close\r\n",0);
			}else{
				assert_failed(__FILE__,__LINE__);while(1);
			}
		}

		//additional test items, place after power task
		if(hteststate.isqualified >0 && (hteststate.testconfigs&CONFIG_LED)){
			uint32_t result;
			xCurrentQueueMessage ModuleVccMsg={.sampletimes=7,.samplevalidindex=4};
			ModuleVccMsg.hCurrentDetection.Frequency = 50;
			assert_failed("\r\n\r\n C_LED\r\n",0);
			float PowerwithLedOff=100.0,PowerwithLedOn=0.000;

			hteststate.errorNo = eMET_LED;
			hteststate.isqualified=0;						//标记不通过
			hteststate.theResult->ledstate = 0;

			//Peek 不开灯的电流值,可以考虑peek三次取最大值
#if 0
			for(int i=0;i<3;i++){
				ModuleVccMsg.signal = AmPeek;
				xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);
				while(\
					xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency))==pdPASS\
					&& ModuleVccMsg.signal==AmPeek\
				)
				{
					vTaskDelay(pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency) );
				}
				if(xQueueReceive(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY)==pdPASS && ModuleVccMsg.signal == AmPeekAck){
					PowerwithLedOff = PowerwithLedOff < ModuleVccMsg.SinglePeek?PowerwithLedOff:ModuleVccMsg.SinglePeek;
				}else{
					assert_failed(__FILE__,__LINE__);
				}
			}
#else
			ModuleVccMsg.signal = AmPeek;
			xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);
			while(\
				xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency))==pdPASS\
				&& ModuleVccMsg.signal==AmPeek\
			)
			{
				vTaskDelay(pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency) );
			}
			if(xQueueReceive(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY)==pdPASS && ModuleVccMsg.signal == AmPeekAck){
				PowerwithLedOff = ModuleVccMsg.SinglePeek;
			}else{
				assert_failed(__FILE__,__LINE__);
			}
#endif
			while(hteststate.theResult->isGetImgOK == 0){
				vTaskDelay(pdMS_TO_TICKS(200));
			}
			udRet = cmdLedTest(&result);
			if(udRet == 0){
				if(result == 0){
					hteststate.theResult->ledstate = 1;
					hteststate.isqualified++;		//后续 再比较一下电流值，先以通信为准
					hteststate.errorNo = eMET_Pass;
					assert_failed("Led test commu OK\r\n",0);
				}
				else{
					assert_failed(__FILE__,__LINE__);
				}
			}else
			{
				assert_failed("Module open fail\r\n",0);
				assert_failed(__FILE__,__LINE__);
			}

			if(udRet == 0 && result == 0){
				if(cmdLedSingleColor(0)==0){		//white ,max power
					vTaskDelay(pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency) );
#if 0
					//Peek 开灯的电流值,可以考虑peek三次取最大值
					for(int i=0;i<3;i++){
						ModuleVccMsg.signal = AmPeek;
						xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);
						while(\
							xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency))==pdPASS\
							&& ModuleVccMsg.signal==AmPeek\
						)
						{
							vTaskDelay(pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency) );
						}
						if(xQueueReceive(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY)==pdPASS && ModuleVccMsg.signal == AmPeekAck){
							PowerwithLedOn = PowerwithLedOn > ModuleVccMsg.SinglePeek ? PowerwithLedOn : ModuleVccMsg.SinglePeek;
						}else{
							assert_failed(__FILE__,__LINE__);
						}
						vTaskDelay( pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency));	//power stable
					}
#else
					ModuleVccMsg.signal = AmPeek;
					xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);
					while(\
						xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency))==pdPASS\
						&& ModuleVccMsg.signal==AmPeek\
					)
					{
						vTaskDelay(pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency) );
					}
					if(xQueueReceive(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY)==pdPASS && ModuleVccMsg.signal == AmPeekAck){
						PowerwithLedOff = ModuleVccMsg.SinglePeek;
					}else{
						assert_failed(__FILE__,__LINE__);
					}
#endif

					if(PowerwithLedOn-PowerwithLedOff > 2.0){			//电流有变化，且正确
						hteststate.errorNo = eMET_Pass;
						hteststate.theResult->ledstate = 1;
						hteststate.isqualified++;
					}
				}else
				{
					assert_failed(__FILE__,__LINE__);
				}
				cmdLedSingleColor(7);		//关灯
				PowerwithLedOn = PowerwithLedOff = 0.00;
			}

			ScreenMsg.MsgType = TestResItem;
			ScreenMsg.SignalNo = INDEX_LED;
			if(hteststate.isqualified){
				ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
			}else{
				ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
			}
			xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
		}

		//test use start
		//test use end
#ifdef __D1024
		assert_failed("handling the amcurrent...",0);
#endif
		TouVccOff();ModuleVccOff();
		if(hteststate.testconfigs&(CONFIG_TVCCPOWER|CONFIG_MVCCPOWER)){
			//------------------------------------------------------------------------------------
			//-----------------添加动静态电流的阈值指标判断筛选出电流不合格的---------------------
			//result save and handle then upload if wifi connected
			//release some source to prepare for the next module
			hteststate.theResult->dynamicpower = ModuleVccMsg.hResultSav;
			hteststate.theResult->staticpower = TouchVccMsg.hResultSav;
			pAllRecords->hTVccSaves = *TouchVccMsg.hResultSav;
			pAllRecords->hMVccSaves = *ModuleVccMsg.hResultSav;

			//copy to target addr
#ifdef __D1024
		assert_failed("copy TVCCs...\r\n",0);
#endif
			//TVCCs
			if(hteststate.testconfigs&CONFIG_TVCCPOWER){
				char *pfloatcopy,*pfloatpsate;
				pfloatpsate = pfloatcopy = pAllRecords->hTVccSaves.datastart;
				while(*pfloatcopy != 0xFA || *(pfloatcopy+1) != 0xFB)
					pfloatcopy++;
				do{
					*pfloatpsate++ = *(pfloatcopy+6);
					*pfloatpsate++ = *(pfloatcopy+5);
					*pfloatpsate++ = *(pfloatcopy+4);
					*pfloatpsate++ = *(pfloatcopy+3);
					pAllRecords->hTVccSaves.len++;
					pfloatcopy+=8;
				}while((*pfloatcopy == 0xFA) && (*(pfloatcopy+1)==0xFB));
				*pfloatpsate++ = 0;*pfloatpsate++ = 0;*pfloatpsate++ = 0;*pfloatpsate++ = 0;
			}

			//MVCCs
#ifdef __D1024
			assert_failed("copy MVCCs...\r\n",0);
#endif
			float *ppowerdat,*pfrom,*pto;
			ppowerdat = malloc(((pAllRecords->hMVccSaves.len > pAllRecords->hTVccSaves.len ?pAllRecords->hMVccSaves.len:pAllRecords->hTVccSaves.len)-1) \
					* sizeof(float) /100 *100 +100);
			if(hteststate.testconfigs&CONFIG_TVCCPOWER){
				pto = (float*)ppowerdat;pfrom = (float*)pAllRecords->hTVccSaves.datastart;
				//tvcc check
				for(int i=0;i<pAllRecords->hTVccSaves.len;i++){
					*pto++=*pfrom++;
				}
				qsort(ppowerdat,pAllRecords->hTVccSaves.len,sizeof(float),cmp2float);
				int validoffset;
				for(validoffset=0;validoffset<pAllRecords->hTVccSaves.len;validoffset++){
					if(*(ppowerdat+validoffset)!=0)break;
				}
				if(*(ppowerdat+validoffset) > (hteststate.staticpowerThreshold_default > 40.0 ?40.0:hteststate.staticpowerThreshold_default)){
					hteststate.isqualified = 0;
					assert_failed("TVCC static power over\r\n",0);
				}

				if(*(ppowerdat + pAllRecords->hTVccSaves.len -2 ) > hteststate.staticpowerThreshold_withurge){
					hteststate.isqualified = 0;
					assert_failed("TVCC dynamic power over\r\n",0);
				}

				ScreenMsg.MsgType = TestResItem;
				ScreenMsg.SignalNo = INDEX_TVCC;
				if(hteststate.isqualified){
					ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
				}else{
					ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
					if(hteststate.errorNo == eMET_Pass)hteststate.errorNo = eMET_TouchPower;
				}
				xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
			}

			//mvcc check
#ifdef __D1024
			assert_failed("checking MVCCs...\r\n",0);
#endif
			if(hteststate.testconfigs&CONFIG_MVCCPOWER){
				pto = (float*)ppowerdat;pfrom = (float*)pAllRecords->hMVccSaves.datastart;
				for(int i=0;i<pAllRecords->hMVccSaves.len;i++){
					*pto++=*pfrom++;
				}
				qsort(ppowerdat,pAllRecords->hMVccSaves.len,sizeof(float),cmp2float);
				if(*(ppowerdat + (pAllRecords->hMVccSaves.len>>1)) > hteststate.dynamicpowerThreshold_default){	//常态功耗，取中间值
					hteststate.isqualified = 0;
					assert_failed("MVCC static power over\r\n",0);
				}

				if(*(ppowerdat + pAllRecords->hMVccSaves.len -6 ) > hteststate.dynamicpowerThreshold_withurge){	//避开LED电流，加上采像时间较长
					hteststate.isqualified = 0;
					assert_failed("MVCC dynamic power over\r\n",0);
				}

				ScreenMsg.MsgType = TestResItem;
				ScreenMsg.SignalNo = INDEX_MVCC;
				if(hteststate.isqualified){
					ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
				}else{
					ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
					if(hteststate.errorNo == eMET_Pass)hteststate.errorNo = eMET_ModulePower;
				}
				xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
			}
			free(ppowerdat);
		}
#ifdef __D1024
			assert_failed("sending screen msg_allresult...\r\n",0);
#endif
		ScreenMsg.MsgType = TestResItem;
		ScreenMsg.SignalNo = INDEX_ALLRESULT;
		if(hteststate.isqualified){
			ScreenMsg.msg[0]='y';ScreenMsg.msg[1]=0;
		}else{
			ScreenMsg.msg[0]='n';ScreenMsg.msg[1]=0;
		}
		xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
		//------------------------------------------------------------------------------------

		//BEEP to tell result
//		if(hteststate.isqualified){
//			BeepOn();vTaskDelay( pdMS_TO_TICKS(200) );BeepOff();
//		}else{
//			BeepOn();vTaskDelay( pdMS_TO_TICKS(200) );BeepOff();vTaskDelay( pdMS_TO_TICKS(200) );
//			BeepOn();vTaskDelay( pdMS_TO_TICKS(200) );BeepOff();vTaskDelay( pdMS_TO_TICKS(200) );
//			BeepOn();vTaskDelay( pdMS_TO_TICKS(200) );BeepOff();vTaskDelay( pdMS_TO_TICKS(200) );
//		}
#ifdef __D1024
			assert_failed("sending screen msg_saving...\r\n",0);
#endif
		ScreenMsg.MsgType = TestMsgToUser;
		ScreenMsg.SignalNo = TESTSAVING;
		xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);

		//trans origin test result to chars
#ifdef __D1024
			assert_failed("sending record queue...\r\n",0);
#endif
		xRecordMsg RecordSend={0};
		RecordSend.hteststate = hteststate;
		RecordSend.htestresult = *(hteststate.theResult);
		RecordSend.pAllRecords = pAllRecords;
		xQueueSend(xQueueRecordMsg,&RecordSend,portMAX_DELAY);
		memset(&RecordSend,0,sizeof(xRecordMsg));

#ifdef __D1024
			assert_failed("cleaning records...\r\n",0);
#endif
		hteststate.theResult->isTouchPass = 0;hteststate.theResult->StartDuartion = 0;
		hteststate.theResult->isCaliBaudrateOK = 0;hteststate.theResult->isGetImgOK = 0;
		hteststate.theResult->isShortCircuit = 0;hteststate.theResult->isXorCheckOK = 0;
		hteststate.theResult->SensorState = 0;hteststate.theResult->ledstate = 0;
		memset(hteststate.theResult->ModuleVersionMessage,0,sizeof(hteststate.theResult->ModuleVersionMessage));
		memset(hteststate.theResult->ModuleUID,0,sizeof(hteststate.theResult->ModuleUID));

		//clean uart recv states
#ifdef __D1024
			assert_failed("clean uart recv states...\r\n",0);
#endif
		HAL_UART_Receive(&ModulePort,temptodel,20,2);
		HAL_UART_Receive(&CurrentPort,temptodel,20,2);
		HAL_UART_Receive(&DebugPort,temptodel,20,2);
		HAL_UART_AbortReceive(&DebugPort);
		HAL_UART_AbortReceive(&ModulePort);
		HAL_UART_AbortReceive(&CurrentPort);
retlab:
#ifdef __D1024
			assert_failed("prepare the next one...\r\n",0);
#endif
		hteststate.isTestStart = 0;
		SysStates.Teststate = TESTIDLE;
		ScreenMsg.MsgType = TestMsgToUser;
		ScreenMsg.SignalNo = TESTFINSIH;
		xQueueSend(xQueueScreenMsg,&ScreenMsg,portMAX_DELAY);
#ifdef __D1024
			assert_failed("next one...\r\n",0);
#endif
		vTaskDelay( pdMS_TO_TICKS(200) );
	}
}

void vTaskSaveRecord( void * pvParameters )
{
	int32_t retsav;
	char testfile[150],fileDirectory[25];
	xWifiUploadMessage hFRestouploadMsg;
	xRecordMsg RecordRecv;

	for(;;)
	{
		xQueueReceive(xQueueRecordMsg,&RecordRecv,portMAX_DELAY);
		RecordRecv.htestresult.staticpower = &RecordRecv.pAllRecords->hTVccSaves;
		RecordRecv.htestresult.dynamicpower = &RecordRecv.pAllRecords->hMVccSaves;
		RecordRecv.hteststate.theResult = &RecordRecv.htestresult;
		retsav = SaveTestResult(testfile,fileDirectory,&RecordRecv.hteststate,RecordRecv.pAllRecords);
		while(retsav!=0){
			assert_failed(__FILE__,__LINE__);
			retsav = SaveTestResult(testfile,fileDirectory,&RecordRecv.hteststate,RecordRecv.pAllRecords);
		}
		assert_failed("result saved OK\r\n",0);

		taskENTER_CRITICAL();
		releaserecord(RecordRecv.pAllRecords);				//======================         change after             ==================
		taskEXIT_CRITICAL();

		sprintf(hFRestouploadMsg.filename,"%s",testfile);
		sprintf(hFRestouploadMsg.dirname,"%s",fileDirectory);
		if((SysStates.wifistates & WIFINETCONNECT) && xQueueSend(xQueueWifiUploadMsg,&hFRestouploadMsg,portMAX_DELAY)!=pdPASS){
			assert_failed(__FILE__,__LINE__);
		}
	}
}

//模组电源监测，由vTaskModuleFunc 直接管理，采样频率根据动静态调整
void vTaskModulePower( void * pvParameters )
{
	float ftemp=0.000;
	xCurrentQueueMessage ModuleVccMsg = {0};
	xExSRAMInstance BkpMsg={0};


	for(;;)
	{
		if(xQueueReceive(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY)!=pdPASS)
		{
			assert_failed("vTaskTouchPower error:",__LINE__);
		}
		else
		{
			if(ModuleVccMsg.signal != AmOpen){
				if(ModuleVccMsg.signal == AmClose){
					ModuleVccMsg.signal = AmGetResult;
					ModuleVccMsg.hResultSav = &BkpMsg;
					xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);
					while(\
							xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(10))==pdPASS\
							&& ModuleVccMsg.signal==AmGetResult\
							)
					{
						vTaskDelay( pdMS_TO_TICKS(10) );
					}
				}else if(ModuleVccMsg.signal == AmPeek){
					float peekcurrent=0;
					cmdGetModuleVccPower(ModuleVccMsg.sampletimes,ModuleVccMsg.samplevalidindex,&peekcurrent);

					ModuleVccMsg.signal = AmPeekAck;	//如果直接返回结果，电流值记录会丢失
					ModuleVccMsg.SinglePeek = peekcurrent;

					xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);
					while(\
							xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency))==pdPASS\
							&& ModuleVccMsg.signal==AmPeekAck\
							)
					{
						vTaskDelay(pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency)/2 );
					}
				}

				continue;
			}
			else{
				ModuleVccMsg.hCurrentDetection.Switch = 1;
				BkpMsg = *ModuleVccMsg.hResultSav;
			}
		}
		assert_failed("module vcc power msg open recved\r\n",0);

		ModuleVccMsg.signal = AmAck;
		xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);

		while(\
				xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency))==pdPASS\
				&& ModuleVccMsg.signal == AmAck\
				)
		{
			vTaskDelay(2*pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency) );
		}

		//后续每个一个周期自动记录
		for(;;)
		{
			if(xQueueReceive(xQueueModuleVccCtrlMsg,&ModuleVccMsg,0)!=pdPASS){
				if(cmdGetModuleVccPower(ModuleVccMsg.sampletimes,ModuleVccMsg.samplevalidindex,&ftemp)!=0){
					assert_failed(__FILE__,__LINE__);
				}
				else{
					taskENTER_CRITICAL();
					ModuleVccMsg.hCurrentDetection.writeresult(&BkpMsg,&ftemp,1);
					taskEXIT_CRITICAL();
				}
				vTaskDelay( pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency) );
			}
			else{
				if(ModuleVccMsg.signal != AmClose){
					if(ModuleVccMsg.signal == AmPeek){		//获取上一次电流值
						ModuleVccMsg.signal = AmPeekAck;	//如果直接返回结果，电流值记录会丢失
						ModuleVccMsg.SinglePeek = ftemp;
						xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);
						while(\
								xQueuePeek(xQueueModuleVccCtrlMsg,&ModuleVccMsg,pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency))==pdPASS\
								&& ModuleVccMsg.signal==AmPeekAck\
								)
						{
							vTaskDelay(pdMS_TO_TICKS(ModuleVccMsg.hCurrentDetection.Frequency)/2 );
						}
					}else{									//err signal
						assert_failed(__FILE__,__LINE__);
					}
					continue;
				}
				else	{									//测试完毕，关闭电表
					ModuleVccMsg.hCurrentDetection.Switch = 0;
					//通过队列将结果传出去
					ModuleVccMsg.signal = AmGetResult;
					ModuleVccMsg.hResultSav = &BkpMsg;
					xQueueSend(xQueueModuleVccCtrlMsg,&ModuleVccMsg,portMAX_DELAY);
					break;
				}
			}
		}
		vTaskDelay( pdMS_TO_TICKS(200) );
	}
}

/*触控电源监测，上电后先采集3次静态的touchvcc功耗，大概是2个uA，超过2（3）个uA即不合格 ，取平均值，然后采集三次给激励的，然后再开始给模组上电，开始测试模组电流曲线。
 * */
void vTaskTouchPower( void * pvParameters )
{
	float ftemp=0.000;
	uint8_t shortcircuittimes=0;
	xCurrentQueueMessage TouchVccMsg={0};
	xExSRAMInstance BkpMsg={0};

	for(;;)
	{
		shortcircuittimes = 0;
		if(xQueueReceive(xQueueTouchVccCtrlMsg,&TouchVccMsg,portMAX_DELAY)!=pdPASS)
		{
			assert_failed("vTaskTouchPower error:",__LINE__);
		}
		else
		{
			if(TouchVccMsg.signal != AmOpen){
				if(TouchVccMsg.signal == AmClose){
					TouchVccMsg.signal = AmGetResult;
					TouchVccMsg.hResultSav = &BkpMsg;
					xQueueSend(xQueueModuleVccCtrlMsg,&TouchVccMsg,portMAX_DELAY);
					while(\
							xQueuePeek(xQueueModuleVccCtrlMsg,&TouchVccMsg,pdMS_TO_TICKS(10))==pdPASS\
							&& TouchVccMsg.signal==AmGetResult\
							)
					{
						vTaskDelay( pdMS_TO_TICKS(10) );
					}
				}

				continue;
			}
			else{
				TouchVccMsg.hCurrentDetection.Switch = 1;
				BkpMsg = *TouchVccMsg.hResultSav;
			}
		}
		assert_failed("touch vcc power msg Open recved\r\n",0);
		taskENTER_CRITICAL();
		HAL_UART_Receive_DMA(&CurrentPort,(uint8_t *)(BkpMsg.nowaddr),MAX_TVCCSAVE_LEN);		//record tvcc power
		taskEXIT_CRITICAL();

		UnlinkIronRing_Sigel();						//模拟开关恢复默认
		vTaskDelay( pdMS_TO_TICKS(300));			//闭合动作给一个等待，同时等待触控电流回到正常值，（上电后瞬间触控是激活状态）
		for(int i=2;i>0;i--)									//测试短路
		{
			//同时检测一下短路电流
			if(cmdDetectShortCircuit(TouchVccMsg.std_short_circuit) >0 ){
				shortcircuittimes++;
				if(shortcircuittimes>0){
					TouVccOff();ModuleVccOff();
					break;
				}
			}
			vTaskDelay( pdMS_TO_TICKS(200));
		}

		if(shortcircuittimes<1 && (hteststate.testconfigs&CONFIG_TOUCH)){						//没短路
			Urge1On();									//给激励，再测3次
			for(int i=5;i>0;i--)
			{
				//测试一下触控
				if(HAL_GPIO_ReadPin(TOUCHOUT_GPIO_Port,TOUCHOUT_Pin)==1){		//触控一次后要释放，持续按住触控芯片复位，就不对了。
					hteststate.theResult->isTouchPass++;
					if(hteststate.theResult->isTouchPass>=3){
						break;
					}
				}
				Urge1Off();
				vTaskDelay( pdMS_TO_TICKS(100));
				Urge1On();
				vTaskDelay( pdMS_TO_TICKS(100));
			}
			Urge1Off();
		}


		if(shortcircuittimes>0)	TouchVccMsg.signal = AmShortCurrent;
		else TouchVccMsg.signal = AmAck;
		xQueueSend(xQueueTouchVccCtrlMsg,&TouchVccMsg,portMAX_DELAY);

		while(xQueuePeek(xQueueTouchVccCtrlMsg,&TouchVccMsg,pdMS_TO_TICKS(TouchVccMsg.hCurrentDetection.Frequency))==pdPASS\
			&& (TouchVccMsg.signal == AmShortCurrent || TouchVccMsg.signal == AmAck)){
			vTaskDelay(2*pdMS_TO_TICKS(TouchVccMsg.hCurrentDetection.Frequency) );
		}

		//后续每个一个周期自动记录
		for(;;)
		{
			if(xQueueReceive(xQueueTouchVccCtrlMsg,&TouchVccMsg,portMAX_DELAY)!=pdPASS){
//				vTaskDelay( pdMS_TO_TICKS(TouchVccMsg.hCurrentDetection.Frequency) );		//use dma
			}
			else{
				if(TouchVccMsg.signal != AmClose) 	continue;
				else	{
					//通过队列将结果传出去
					HAL_UART_AbortReceive_IT(&CurrentPort);
					TouchVccMsg.hCurrentDetection.Switch = 0;
					TouchVccMsg.signal =AmGetResult;
					TouchVccMsg.hResultSav = &BkpMsg;
					xQueueSend(xQueueTouchVccCtrlMsg,&TouchVccMsg,portMAX_DELAY);
					break;
				}
			}
		}
		vTaskDelay( pdMS_TO_TICKS(500));
	}
}


//串口输入必须以\n结尾！！！
//ST的HAL库提供了设备的文件锁保护,相当于已经有了临界资源保护功能,
//如果直接打印一长条字符串是可以直接打印的,如果是一个字符一个字符循环打印的,就需要加入临界资源保护
int App_main()
{
	uint32_t i=0,Retn;

	HAL_GetUID(SysStates.HostUID);

	//  RTC
	SysStates.Calendar.hdate.WeekDay = RTC_WEEKDAY_MONDAY;
	SysStates.Calendar.hdate.Year = 19;
	SysStates.Calendar.hdate.Month = 8;
	SysStates.Calendar.hdate.Date = 26;
	SysStates.Calendar.htime.Hours = 9;
	SysStates.Calendar.htime.Minutes = 59;
	SysStates.Calendar.htime.Seconds = 30;
	SetCalendar(&SysStates.Calendar);
//	while(1){
//		GetCalendar(&SysStates.Calendar);
//		printf("\r\n20%d/%d/%d\r\n%2d:%2d:%2d\r\n",SysStates.Calendar.hdate.Year,SysStates.Calendar.hdate.Month,SysStates.Calendar.hdate.Date,\
//				SysStates.Calendar.htime.Hours,SysStates.Calendar.htime.Minutes,SysStates.Calendar.htime.Seconds);
//		HAL_Delay(1000);
//	}

	//fatfs mount
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
	uint32_t total,free;
	exfuns_init();
	f_mount(fs[0],"0:",1); 					//挂载SD卡
	while(exf_getfree("0",&total,&free));	//得到SD卡的总容量和剩余容量
	assert_failed("SD Card Mount On\r\n",0);
	assert_failed("SD Total Size:",total);		//MB
	assert_failed("SD Free Size:",free);		//MB
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
	//HAL_UART_Receive_IT(&DebugPort,DebugRxBuf,1);		//开启调式串口接收

	//可以考虑放在后台任务中，一直尝试连接
	//标记与wifi模块的串口端通信正常与否
	for(i=0*4+1;i>0;i--)		//加入超时次数
	{
		if(wifiEcho() == 0){
			SysStates.wifistates |= WIFIUARTCONNECT;
			break;
		}
		else{
			assert_failed("wifi connect error \r\n",0);
		}
		SysStates.wifistates &= ~(uint32_t)WIFIUARTCONNECT;
		if(theSSID.WifiUartbps == eBAUDRATE_460800)		theSSID.WifiUartbps = eBAUDRATE_230400;
		else if(theSSID.WifiUartbps == eBAUDRATE_230400)theSSID.WifiUartbps = eBAUDRATE_115200;
		else if(theSSID.WifiUartbps == eBAUDRATE_115200)theSSID.WifiUartbps = eBAUDRATE_57600;
		else if(theSSID.WifiUartbps == eBAUDRATE_57600)	theSSID.WifiUartbps = eBAUDRATE_460800;
		hostwifiUartportboudratechange(theSSID.WifiUartbps);
		HAL_Delay(800);					//每次命令间隔，不能太小，会当成错误信息处理,也需要等待wifi就绪
	}
	//wifi uart_RX_IT 到此已关闭
	//Require Internet RTC
#if 0			//get eth rtc
	memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
	HAL_UART_Receive_IT(&WifiPort,WifiRxBuf.dat,1);
	for(i=0;i<10;i++){
		WifiUartSend("Require RTC \n\0",sizeof("Require RTC \n\0"));
		if(WifiRxAnalyse(3,0)==30){
			GetCalendar(&SysStates.Calendar);
			printf("\r\n20%d/%d/%d\r\n%2d:%2d:%2d\r\n",SysStates.Calendar.hdate.Year,SysStates.Calendar.hdate.Month,SysStates.Calendar.hdate.Date,\
					SysStates.Calendar.htime.Hours,SysStates.Calendar.htime.Minutes,SysStates.Calendar.htime.Seconds);
			break;
		}
		HAL_Delay(1000);
	}
	HAL_UART_AbortReceive_IT(&WifiPort);
	memset(&WifiRxBuf,0,sizeof(WifiRxBuf));
#endif


	//================================      app_main   func    start =======================================================
	  //==============================      Semaphore Create        ====================================================
	xMutexHandleCriticalAssertProtection = xSemaphoreCreateMutex(  );			//互斥信号量，创建后是有效的，可以take，即资源可用状态
	if(xMutexHandleCriticalAssertProtection == NULL){							//二值信号量和互斥信号量还是不同的！！这里使用二值信号量用于同步，互斥会导致优先级反转，死机
		assert_failed("xMutexHandleCriticalAssertProtection  Create Failed\r\n",0);	//二值信号量默认是无效的，需要用户先给出信号
	}

	xBinaryHandleWIFINETCONNECT = xSemaphoreCreateMutex(  );
	if(xBinaryHandleWIFINETCONNECT == NULL){
		assert_failed("xBinaryHandleWIFINETCONNECT  Create Failed\r\n",0);
	}else{
		xSemaphoreTake( xBinaryHandleWIFINETCONNECT, 20 );
	}

	//==============================          Queue        Create   =================================
	xQueueTestResultUpload = xQueueCreate( QUEUE_LENGTH_TestResultUpload, QUEUE_ITEM_SIZE_TestResultUpload );
	if(xQueueTestResultUpload == NULL){
		assert_failed("xQueueTestResultUpload  Create Failed\r\n",0);
	}
	xQueueTouchVccCtrlMsg = xQueueCreate( QUEUE_LENGTH_TouchVccCtrlMsg, QUEUE_ITEM_SIZE_TouchVccCtrlMsg );
	if(xQueueTouchVccCtrlMsg == NULL){
		assert_failed("xQueueTouchVccCtrlMsg  Create Failed\r\n",0);
	}
	xQueueModuleVccCtrlMsg = xQueueCreate( QUEUE_LENGTH_ModuleVccCtrlMsg, QUEUE_ITEM_SIZE_ModuleVccCtrlMsg );
	if(xQueueModuleVccCtrlMsg == NULL){
		assert_failed("xQueueModuleVccCtrlMsg  Create Failed\r\n",0);
	}
	xQueueWifiUploadMsg = xQueueCreate( QUEUE_LENGTH_WifiUploadMsg, QUEUE_ITEM_SIZE_WifiUploadMsg );
	if(xQueueWifiUploadMsg == NULL){
		assert_failed("xQueueWifiUploadMsg  Create Failed\r\n",0);
	}
	xQueueScreenMsg = xQueueCreate( QUEUE_LENGTH_ScreenMsg, QUEUE_ITEM_SIZE_ScreenMsg );
	if(xQueueScreenMsg == NULL){
		assert_failed("xQueueScreenMsg  Create Failed\r\n",0);
	}
	xQueueRecordMsg = xQueueCreate( QUEUE_LENGTH_RecordMsg, QUEUE_ITEM_SIZE_RecordMsg );
	if(xQueueRecordMsg == NULL){
		assert_failed("xQueueRecordMsg  Create Failed\r\n",0);
	}
	  //================      Task Create        ========================
		//最大任务吗长度支持32字节
//	  Retn = xTaskCreate( vTaskStatusIndication, "Task_StatusIndication", STK_SIZE_StatusIndication, NULL, Prio_StatusIndication, &TaskHandle_StatusIndication );
//	  if(Retn!=pdPASS)
//	  {
//	      assert_failed("Task_Led1 create fail\r\n",0);
//	  }

//	  Retn = xTaskCreate( vTaskDebug, "Task_Debug", STK_SIZE_Debug, NULL, Prio_Debug, &TaskHandle_Debug );
//	  if(Retn!=pdPASS)
//	  {
//		  assert_failed("Task_Debug create fail\r\n",0);
//	  }

	  if(SysStates.wifistates & WIFIUARTCONNECT){			//wifi 硬件设备连接OK
		  Retn = xTaskCreate( vTaskWifiUploadbg, "Task_WifiUploadbg", STK_SIZE_WifiUploadbg, NULL, Prio_WifiUploadbg, &TaskHandle_WifiUploadbg );
		  if(Retn!=pdPASS)
		  {
			  assert_failed("Task_WifiUploadbg create fail\r\n",0);
		  }
	  }

	  Retn = xTaskCreate( vTaskModuleFunc, "Task_ModuleFunc", STK_SIZE_ModuleFunc, NULL, Prio_ModuleFunc, &TaskHandle_ModuleFunc );
	  if(Retn!=pdPASS)
	  {
		  assert_failed("Task_ModuleFunc create fail\r\n",0);
	  }

	  Retn = xTaskCreate( vTaskModulePower, "Task_ModulePower", STK_SIZE_ModulePower, NULL, Prio_ModulePower, &TaskHandle_ModulePower );
	  if(Retn!=pdPASS)
	  {
		  assert_failed("Task_ModulePower create fail\r\n",0);
	  }

	  Retn = xTaskCreate( vTaskTouchPower, "Task_TouchPower", STK_SIZE_TouchPower, NULL, Prio_TouchPower, &TaskHandle_TouchPower );
	  if(Retn!=pdPASS)
	  {
		  assert_failed("Task_TouchPower create fail\r\n",0);
	  }

	  Retn = xTaskCreate( vTaskSaveRecord, "Task_SaveRecord", STK_SIZE_SaveRecord, NULL, Prio_SaveRecord, &TaskHandle_SaveRecord);
	  if(Retn!=pdPASS)
	  {
		  assert_failed("Task_SaveRecordcreate fail\r\n",0);
	  }

//==================	GUI part   ==================
	  extern void GUI_UC_SetEncodeGBK(void);
      GUI_Init();
      GUI_UC_SetEncodeGBK();
	  Retn = xTaskCreate( vTaskMonitor, "Task_Monitor", STK_SIZE_Monitor, NULL, Prio_Monitor, &TaskHandle_Monitor );
	  if(Retn!=pdPASS)
	  {
		  assert_failed("Task_Monitor create fail\r\n",0);
	  }

	  SysStates.osstates = 1;
	  vTaskStartScheduler();
	  for(;;);
	  return 0;
	//==================      app_main   func    end  ============================


}

//阻塞等待实现，保证有一个测试项
//临界保护
static uint32_t GetTestCmd(xTestTask *xTestItem)
{
	eTestFunc eTest[MAX_TEST_ITEM]={ERR};

	//	xTestList[0].TestFunc = C_GET_MODULE_VERSION;	//默认第一项为检测版本号
	//	xTestList[0].Next = NULL;

	if(xTestItem!=NULL)
	{
		memcpy(xTestList,xTestItem,sizeof(xTestTask));
	}

	//from HMI


	//from WIFI


	//from current (debug port)
	// (to deal , add critical protect)
	AnalyTestCmd(DebugRxBuf,DebugRxBuf[100],eTest);		//支持多任务
	if(eTest[0]>0)
	{
		memset(DebugRxBuf,0,sizeof(DebugRxBuf));
		AddTestItem(xTestList,eTest);
		HAL_UART_Receive_IT(&DebugPort,DebugRxBuf,1);

		return 0;
	}
	else
	{
		HAL_UART_Receive_IT(&DebugPort,DebugRxBuf,1);
	}
	return 0xFFFFFFFF;
}


uint32_t AnalyTestCmd(uint8_t rxbuf[],uint8_t len,eTestFunc *eTest)		//自定义字符test命令 TZst_0001
{
	uint8_t i,j=0;
	uint16_t cmd=0;
	for (i=0;i<len;i++)
	{
		if(rxbuf[i]!='T')
			continue;
		if((rxbuf[i]=='T')&&(rxbuf[i+1]=='Z')&&(rxbuf[i+2]=='s')&&(rxbuf[i+3]=='t')&&(rxbuf[i+4]=='_'))
		{
			cmd += (rxbuf[i+5]-'0')*1000;
			cmd += (rxbuf[i+6]-'0')*100;
			cmd += (rxbuf[i+7]-'0')*10;
			cmd += (rxbuf[i+8]-'0');
			if((cmd >= Max_eTestFunc ))
				continue;
			else
			{
				eTest[j++]=(eTestFunc)cmd;
				cmd = 0;
			}
		}
	}
	return 0;
}

uint32_t AddTestItem(xTestTask *xTestList,eTestFunc *eTestItem)
{
//	uint8_t j=0;
//	while((eTestItem[j] < Max_eTestFunc)&&(eTestItem[j]!=ERR)) j++;
//	j%=MAX_TEST_ITEM;
//	if(j<1)
//		return 1;

	uint8_t j=1;
	uint8_t i=0;
	xTestTask *pListItem;

	if((xTestList->TestFunc == ERR))
	{
		xTestList->TestFunc = eTestItem[i++];
		xTestList->Next = NULL;
	}

	for (pListItem=xTestList; pListItem->Next!=NULL; pListItem=pListItem->Next)
	{

	}

	while(i<j)
	{
		pListItem->Next = pListItem+1;
		pListItem=pListItem->Next;
		pListItem->TestFunc = eTestItem[i++];
	}

	return 0;
}




int32_t CreateTestList()
{
	eTestFunc aTestItem;
	if(hteststate.testconfigs&CONFIG_TOUCH){
		aTestItem = C_TOUCHOUT;
		AddTestItem(xTestList,&aTestItem);
	}

	if(hteststate.testconfigs&CONFIG_CALIBAUD){
		aTestItem = C_CALIBOUDRATE;
		AddTestItem(xTestList,&aTestItem);
	}

	if(hteststate.testconfigs&CONFIG_STARTDUARTION){
		aTestItem = C_STARTDUARTION;
		AddTestItem(xTestList,&aTestItem);
	}

	if(hteststate.testconfigs&CONFIG_GETVERSION){
		aTestItem = C_GET_MODULE_VERSION;
		AddTestItem(xTestList,&aTestItem);
	}

	if(hteststate.testconfigs&CONFIG_XORCHECK){
		aTestItem = C_CHECKOUTXOR;
		AddTestItem(xTestList,&aTestItem);
	}

	if(hteststate.testconfigs&CONFIG_FLASH){
		aTestItem = C_FLASH;
		AddTestItem(xTestList,&aTestItem);
	}

	if(hteststate.testconfigs&CONFIG_GETIMAGE){
		aTestItem = C_GET_FINGER_IMAGE;
		AddTestItem(xTestList,&aTestItem);
	}


	//根据配置，选择性开启，不作为测试项
//	if(hteststate.testconfigs&CONFIG_LED){
//		aTestItem = C_LED;
//		AddTestItem(xTestList,&aTestItem);
//	}

//	if(hteststate.testconfigs&CONFIG_TVCCPOWER){
//		aTestItem = C_STATICPOWER;
//		AddTestItem(xTestList,&aTestItem);
//	}
//
//	if(hteststate.testconfigs&CONFIG_MVCCPOWER){
//		aTestItem = C_DYNAMICPOWER;
//		AddTestItem(xTestList,&aTestItem);
//	}


/*	if(hteststate.moduletype < MODULE_TYPE_SENSOR){
//		aTestItem= C_CALIBOUDRATE;					//只有海康用的模组需要校准波特率，标准模组还没有该功能  2019-8-19
//		AddTestItem(xTestList,&aTestItem);
		aTestItem = C_STARTDUARTION;
		AddTestItem(xTestList,&aTestItem);
		aTestItem = C_GET_MODULE_VERSION;
		AddTestItem(xTestList,&aTestItem);
		aTestItem = C_CHECKOUTXOR;
		AddTestItem(xTestList,&aTestItem);
		aTestItem = C_WRITE_FEATURE;			//但海康的模组没有该功能
		AddTestItem(xTestList,&aTestItem);
		aTestItem = C_READ_FEATURE;
		AddTestItem(xTestList,&aTestItem);
		aTestItem = C_DEL_FEATURE;				//temp
		AddTestItem(xTestList,&aTestItem);
		if((hteststate.testconfigs&CONFIG_LED) > 0 ){
			aTestItem = C_LED;
			AddTestItem(xTestList,&aTestItem);
		}
		aTestItem = C_GET_FINGER_IMAGE;
		AddTestItem(xTestList,&aTestItem);
	}*/
	return 0;
}

void getModelSelectionList(xTestSelection *pTestconfig)
{
	uint32_t br;
	FIL fmodelselection;
	FRESULT fres;
	char *p;

	fres = f_open(&fmodelselection,"0:/2.Moudle/AllMods.txt",FA_OPEN_EXISTING|FA_READ);
	if(fres != FR_OK){
		assert_failed("AllMods.txt open fail\r\n",0);
		assert_failed(__FILE__,__LINE__);
	}

	char *tempbuff=(char *)malloc(2000);
	fres = f_read(&fmodelselection,tempbuff,2000,&br);
	if(fres != FR_OK){
		assert_failed(__FILE__,__LINE__);
	}

	for (p=tempbuff;*p!=0;p++)
	{
		if(*p!='[')
			continue;
		switch(*(p+1))
		{
			case '1':
				while(*p++!=']');				//忽略中间说明字符，判断 [x ]:
				if(*p++ == ':')
				{
					int j=0;
					do{
						while(*p++!='(');
						//pTestconfig->model[*p-'1'] =
						while(*p++!=')');
						char *ppaste=pTestconfig->model[j++];		//0-8  -> 1-9
						while(*p!=0x0D && *p!=0x0A && *p!=0x00)
							*ppaste++ = *p++;
						*ppaste = '\0';
						if(j>=MAX_MODELSELECT_NUM){
							assert_failed("num overflow\r\n",0);
							assert_failed(__FILE__,__LINE__);
							j=0;
						}
					}while(*(p+1)!='[' && *(p+2)!='[' && *(p+3)!='[' && *(p+4)!='['); //这里只判断判断后面4个字符！！
				}
				break;
			case '2':
				while(*p++!=']');				//忽略中间说明字符，判断 [x ]:
				if(*p++ == ':')
				{
					char *ppaste;
					int j=0;
					do{
						while(*p++!='(');
						while(*p++!=')');
						ppaste=pTestconfig->testid[j];
						for(int k=0;k<8;k++){
							if(*p!=' ' &&*p!=0x0D && *p!=0x0A && *p!=0x00)
								*ppaste++ = *p++;
							else{
								*ppaste = '\0';
								break;
							}
						}

						if(*p == ' '){
							p++;
							ppaste = pTestconfig->testname[j];
							while(*p!=0x0D && *p!=0x0A && *p!=0x00)
								*ppaste++ = *p++;
							*ppaste = '\0';
						}else{
							pTestconfig->testname[j][0]='-';
						}

						j++;
						if(j>=MAX_TESTNUM){
							assert_failed("num overflow\r\n",0);
							assert_failed(__FILE__,__LINE__);
							j=0;
						}
					}while(*(p+1)!='[' && *(p+2)!='[' && *(p+3)!='[' && *(p+4)!='[' \
							&& *(p+1)!=0 && *(p+2)!=0 && *(p+3)!=0 && *(p+4)!=0);     //这里只判断判断后面4个字符！！
				}
				break;
			default:
				assert_failed(__FILE__,__LINE__);
				break;
		}
	}
	free(tempbuff);
	fres = f_close(&fmodelselection);
	if(fres != FR_OK){
		assert_failed(__FILE__,__LINE__);
	}

	return ;
}

void getWifiMesg(xTestSelection *pTestconfig)
{
	uint32_t udResult;
	char tempstrings[100]={0},tempstrings2[100]={0};
	  //===============          wifi config   start       =======================//
	if(SysStates.wifistates & WIFIUARTCONNECT){
		theSSID.Port = theSSID.Quality = 0;
		memset(theSSID.thsSSID,0,sizeof(theSSID.thsSSID));
		memset(theSSID.thePASSWD,0,sizeof(theSSID.thePASSWD));
		memset(theSSID.SeverIPAddr,0,sizeof(theSSID.SeverIPAddr));
		udResult = HAL_GetTick()+20*1000;	//wifi读取 超时20秒
		while(wifiGetConfigState(&theSSID)&&(HAL_GetTick()<udResult));

		if(HAL_GetTick() <= udResult){
			sprintf(tempstrings,"%s,%s,%d,%d.%d.%d.%d",theSSID.thsSSID,theSSID.thePASSWD,theSSID.Port,\
				    			theSSID.SeverIPAddr[0],theSSID.SeverIPAddr[1],theSSID.SeverIPAddr[2],theSSID.SeverIPAddr[3]);
		    strcpy(pTestconfig->defaultwifimsg,tempstrings);

			uint32_t br;
			FIL fwificonfig;
			FRESULT fres;
			char *p;
			fres = f_open(&fwificonfig,"0:/1.Wifi/wifi.txt",FA_OPEN_EXISTING|FA_READ);
			if(fres != FR_OK){
				assert_failed("wifi.txt open fail\r\n",0);
				assert_failed(__FILE__,__LINE__);
				tempstrings2[0]=0;
		//		goto next07310925;
				while(1);
			}
			fres = f_read(&fwificonfig,tempstrings,100,&br);
			if(fres != FR_OK){
				assert_failed(__FILE__,__LINE__);
			}
			for (p=tempstrings;*p!=0;p++)
			{
				if(*p!='[')
					continue;
				while(*p++!=']');					//忽略中间说明字符
				if(*p++ == ':')
				{
					char *ppaste=tempstrings2+strlen(tempstrings2);
					while(*p!=0x0D && *p!=0x0A && *p!=0x00)
						*ppaste++ = *p++;
					*ppaste = ',';
				}
			}
			fres = f_close(&fwificonfig);
			if(fres != FR_OK){
				assert_failed(__FILE__,__LINE__);
			}
			*(tempstrings2+strlen(tempstrings2)-1)='\0';
			strcpy(pTestconfig->configwifimsg,tempstrings2);

			assert_failed(pTestconfig->defaultwifimsg,0);assert_failed("\r\n",0);
			assert_failed(pTestconfig->configwifimsg,0);assert_failed("\r\n",0);

			if(strcmp(pTestconfig->defaultwifimsg,pTestconfig->configwifimsg)!=0){
				SysStates.wifistates &=(uint32_t)(~WIFINETCONNECT);
			}
			else
			{
				SysStates.wifistates |=WIFINETCONNECT;
			}
		}
		else{
			SysStates.wifistates &=(uint32_t)(~WIFINETCONNECT|WIFIUARTCONNECT);
		}
	}
	else			//uart wifi comni fail
	{
		SysStates.wifistates &= (uint32_t)(~WIFINETCONNECT);
		strcpy(pTestconfig->configwifimsg,"wifi module uartport open error. ");
		strcpy(pTestconfig->defaultwifimsg,"wifi module uartport open error. ");
	}
	//===============          wifi config   end       =======================//
}


extern volatile GUI_TIMER_TIME OS_TimeMS;
void vApplicationTickHook( void )		//OS tick hook func
{
    OS_TimeMS++;
    if(OS_TimeMS%1000 == 0){
    	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
    }
//    if(HAL_GetTick() % 1000 == 0)
//    	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
}

void HAL_Delay(uint32_t Delay)
{
	if(SysStates.osstates >0){
		vTaskDelay( pdMS_TO_TICKS(Delay) );
	}
	else{
	  uint32_t tickstart = HAL_GetTick();
	  uint32_t wait = Delay;

	  /* Add a freq to guarantee minimum wait */
	  if (wait < HAL_MAX_DELAY)
	  {
		wait += (uint32_t)(1);
	  }

	  while((HAL_GetTick() - tickstart) < wait)
	  {
	  }
	}
}

void vApplicationIdleHook(void)			//idle task hook func
{
	uint32_t KeyInState;
	if(!SysStates.osstates)
		SysStates.osstates=1;		//标记OS启动

//	//KEY1 = Start Test Button
//	KeyInState = HAL_GPIO_ReadPin(KEY_1_GPIO_Port,KEY_1_Pin);
//	if(KeyInState == 0 && hteststate.isTestStart == 0 && SysStates.Teststate == TESTIDLE)	//key down && 测试没有启动
#ifdef __KEYKEEPON
	if(hteststate.isTestStart == 0 && SysStates.Teststate == TESTIDLE)
#else
	//KEY1 = Start Test Button
	KeyInState = HAL_GPIO_ReadPin(KEY_1_GPIO_Port,KEY_1_Pin);
	if(KeyInState == 0 && hteststate.isTestStart == 0 && SysStates.Teststate == TESTIDLE)	//key down && 测试没有启动
#endif
	{
			hteststate.isTestStart = 1;
	}

//	KeyInState = HAL_GPIO_ReadPin(KEY_2_GPIO_Port,KEY_2_Pin);		//KEY2
//	if(KeyInState == 0 && hteststate.isTestStart == 1 && SysStates.Teststate == TESTWAITPRESS)	//key down && 测试没有启动
#ifdef __KEYKEEPON
	if(hteststate.isTestStart == 1 && SysStates.Teststate == TESTWAITPRESS)
#else
	KeyInState = HAL_GPIO_ReadPin(KEY_2_GPIO_Port,KEY_2_Pin);		//KEY2
	if(KeyInState == 0 && hteststate.isTestStart == 1 && SysStates.Teststate == TESTWAITPRESS)	//key down && 测试没有启动
#endif
	{
			SysStates.Teststate = TESTTING;
	}
//	KeyInState = HAL_GPIO_ReadPin(KEY_2_GPIO_Port,KEY_3_Pin);		//KEY3
//	KeyInState = HAL_GPIO_ReadPin(KEY_2_GPIO_Port,KEY_4_Pin);		//KEY4


//	GUI_Exec1();

}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	while(1)
		printf("%s stack overflow\r\n",pcTaskName);
}

void vApplicationMallocFailedHook( void )
{
	while(1)
		printf("heap charge\r\n");
}

