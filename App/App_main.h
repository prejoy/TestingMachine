/*
 * App_main.h
 *
 *  Created on: 2019年7月22日
 *      Author: admin
 */

#ifndef APP_APP_MAIN_H_
#define APP_APP_MAIN_H_

#include "exsram.h"
#include "TestFuncs.h"

#define MAX_TEST_ITEM		18

typedef enum {
	ERR = 0,

	C_TOUCHOUT,				//作为硬件功能实现，在检测触控电流时测试
	C_CALIBOUDRATE,
	C_STARTDUARTION,
	C_GET_MODULE_VERSION,
	C_CHECKOUTXOR,
	C_TEST_SENSOR,

	C_FLASH,
//	C_REGISTER_USER,
//	C_VERIFY_ONE_TO_ALL,
//	C_DELETE_ALL_USER,
//	C_WRITE_FEATURE=C_REGISTER_USER,
//	C_READ_FEATURE=C_VERIFY_ONE_TO_ALL,
//	C_DEL_FEATURE=C_DELETE_ALL_USER,

	C_LED,
	C_GET_FINGER_IMAGE,
	C_STATICPOWER,
	C_DYNAMICPOWER,

//	//wifi配置相关
//	C_WIFI_INITCONFIG,        //配置wifi接入，工作模式等
//	C_WIFI_ECHO,              //测试主控MCU与wifi模块的通信正常
//	C_WIFI_NETTICKCONFIG,     //设置wifi的网络tick
//	C_WIFI_TEST,              //临时测试用
	Max_eTestFunc
}eTestFunc;



typedef struct struTestTask{
	eTestFunc  TestFunc;
	struct struTestTask	*Next;
}xTestTask;
//typedef struct struTestTask xTestTask;



typedef struct{
	RTC_DateTypeDef hdate;
	RTC_TimeTypeDef htime;
}xCalendar;


#define OSRUN				(1<<0)
#define WIFIUARTCONNECT		(1<<0)
#define WIFINETCONNECT		(1<<1)
#define WIFIBUSY			(1<<2)		//结果发送状态，如果前一次没有发送完成，后一次刚开始就测试不合格，又想发送，则需要等待前一次发送完成
										//改：使用队列缓冲，一个可以保存最多三份结果的队列，后台不断发送，直到队列空。
#define WIFILINK			(1<<6)
#define WIFITCPLINK			(1<<7)

#define TESTIDLE			0
#define TESTTING			1
#define TESTWAITPRESS		2			//等待手指按压
#define TESTSAVING			3			//准备换下一个模组
#define TESTFINSIH			4
#define TESTPASS			5
#define TESTNG				6

typedef struct {
	uint32_t osstates;			//bit0: os runs
	uint32_t wifistates;		//up line 5
	uint32_t Teststate;
	uint32_t HostUID[3];		//96bit unqiue id
	xCalendar Calendar;
	uint32_t ModuleBaudrate;
}xSysState;



typedef enum{
	AmClose=0,
	AmOpen,
	AmAck,
	AmGetResult,
	AmShortCurrent,
	AmPeek,
	AmPeekAck
}eAmmeterSignal;

typedef struct {
	eAmmeterSignal signal;			// 1=启动  0=关闭  2=应答 3=结果返回   4=短路
	int8_t sampletimes;				//不能超过10，如果要超过10，修改cmdGetModuleVccPower函数
	int8_t samplevalidindex;		//75%-85%
	float std_short_circuit;		//短路电流 标准为50mA
	float SinglePeek;
	xCurrentDetection hCurrentDetection;
	xExSRAMInstance *hResultSav;
}xCurrentQueueMessage;


typedef struct{
	char dirname[25];
	char filename[80];
}xWifiUploadMessage;


#define TestMsgToUser	1
#define TestResItem		2
#define WifiStateMsg	3


typedef enum{
	INDEX_TOUCHOUT,
	INDEX_CALIBAUD,
	INDEX_STARTDUARTION,
	INDEX_GETVERSION,
	INDEX_XORCHECK,
	INDEX_FLASH,
	INDEX_LED,
	INDEX_IMAGE,
	INDEX_TVCC,
	INDEX_MVCC,
	INDEX_ALLRESULT
}eTextObjItemIndex;


typedef struct{
	int32_t MsgType;
	int32_t SignalNo;
	char msg[8];
}xScreenMsg;

typedef struct{
	xTestState hteststate;
	xTestResult htestresult;
	xAllTestContents *pAllRecords;
}xRecordMsg;


extern xSysState SysStates;
extern xAllTestContents *pAllRecords;
uint32_t AnalyTestCmd(uint8_t rxbuf[],uint8_t len,eTestFunc *eTest);		//自定义字符test命令 TZst_0001
uint32_t AddTestItem(xTestTask *xTestList,eTestFunc *eTestItem);

static uint32_t GetTestCmd(xTestTask *xTestItem);
int App_main(void);
int32_t TestInitialConfig(void);
int32_t CreateTestList(void);

uint32_t SetCalendar(xCalendar *pCalendar);
uint32_t GetCalendar(xCalendar *pCalendar);

#define BeepOn()	HostBeep(1)
#define BeepOff()	HostBeep(0)
void HostBeep(uint32_t state);
#endif /* APP_APP_MAIN_H_ */
