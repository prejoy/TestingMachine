/*
 * App_main.h
 *
 *  Created on: 2019��7��22��
 *      Author: admin
 */

#ifndef APP_APP_MAIN_H_
#define APP_APP_MAIN_H_

#include "exsram.h"
#include "TestFuncs.h"

#define MAX_TEST_ITEM		18

typedef enum {
	ERR = 0,

	C_TOUCHOUT,				//��ΪӲ������ʵ�֣��ڼ�ⴥ�ص���ʱ����
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

//	//wifi�������
//	C_WIFI_INITCONFIG,        //����wifi���룬����ģʽ��
//	C_WIFI_ECHO,              //��������MCU��wifiģ���ͨ������
//	C_WIFI_NETTICKCONFIG,     //����wifi������tick
//	C_WIFI_TEST,              //��ʱ������
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
#define WIFIBUSY			(1<<2)		//�������״̬�����ǰһ��û�з�����ɣ���һ�θտ�ʼ�Ͳ��Բ��ϸ����뷢�ͣ�����Ҫ�ȴ�ǰһ�η������
										//�ģ�ʹ�ö��л��壬һ�����Ա���������ݽ���Ķ��У���̨���Ϸ��ͣ�ֱ�����пա�
#define WIFILINK			(1<<6)
#define WIFITCPLINK			(1<<7)

#define TESTIDLE			0
#define TESTTING			1
#define TESTWAITPRESS		2			//�ȴ���ָ��ѹ
#define TESTSAVING			3			//׼������һ��ģ��
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
	eAmmeterSignal signal;			// 1=����  0=�ر�  2=Ӧ�� 3=�������   4=��·
	int8_t sampletimes;				//���ܳ���10�����Ҫ����10���޸�cmdGetModuleVccPower����
	int8_t samplevalidindex;		//75%-85%
	float std_short_circuit;		//��·���� ��׼Ϊ50mA
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
uint32_t AnalyTestCmd(uint8_t rxbuf[],uint8_t len,eTestFunc *eTest);		//�Զ����ַ�test���� TZst_0001
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
