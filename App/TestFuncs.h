/*
 * TestFuncs.h
 *
 *  Created on: 2019��7��23��
 *      Author: admin
 */

#ifndef APP_TESTFUNCS_H_
#define APP_TESTFUNCS_H_

#include <stdint.h>
#include <stdio.h>

#include "exsram.h"

typedef struct{
	uint8_t isCaliBaudrateOK;
	uint8_t isTouchPass;				//�����Ƿ�
	uint8_t isXorCheckOK;
	uint8_t reguserstate;	//д����ֵ
	uint8_t verifystate;	//������ֵ
	uint8_t deluserstate;	//ɾ����ֵ
	uint8_t ledstate;		//0= bad 1=OK
	uint8_t isShortCircuit;
	uint8_t isstaticpowerpass;
	uint8_t isdynamicpowerpass;
	uint8_t isGetImgOK;
	uint8_t SensorState;
	uint8_t ModuleUID[8];
	uint8_t StartDuartion;				//ms��λ
	uint8_t FgpWidth;				//����ͼ�ɹ�������ָ�ƵĿ�߲���
	uint8_t FgpHeight;
	xExSRAMInstance *staticpower;				//��̬���ģ���λ��΢�� uA
	xExSRAMInstance *dynamicpower;				//��̬���ģ���λ��΢�� mA
	uint8_t  ModuleVersionMessage[256];
	char testerid[8+1];
	char testername[8+1];
	char Models[50];
	char Binfiles[100];
	uint8_t *pImage;
}xTestResult;

//�����������ƶ�����Ϣ�ṹ��
//������Ϣ�Ķ��룿��
typedef struct{
	uint32_t Switch;
	uint32_t Frequency;
	int (*writeresult)(xExSRAMInstance *pWriteInstance,void *dat,int32_t len);
	int (*readresult)(xExSRAMInstance *pWriteInstance,void *dat,int32_t len);
	int (*clearresult)(xExSRAMInstance *pWriteInstance);
}xCurrentDetection;


typedef enum{
	eMET_Pass=0,
	eMET_Touchout,
	eMET_CaliBaudRate,
	eMET_StartDuartion,
	eMET_GetVersion,
	eMET_XorCheck,
	eMET_Flash,
	eMET_LED,
	eMET_GetIMAGE,
	eMET_TouchPower,
	eMET_ModulePower,
	eMET_Sensor
}eModuleErrTypes;


#define CONFIG_TOUCH	0x1
#define CONFIG_CALIBAUD	0x2
#define CONFIG_STARTDUARTION	0x4
#define CONFIG_GETVERSION	0x8
#define CONFIG_XORCHECK		0x10
#define CONFIG_FLASH		0x20
#define CONFIG_GETIMAGE		0x40
#define CONFIG_TVCCPOWER		0x80
#define CONFIG_MVCCPOWER	0x100
#define CONFIG_LED			0x200
#define CONFIG_SENSOR		0x400

#define MODULE_TYPE_ACT		1
#define MODULE_TYPE_PASSIVE	2
#define MODULE_TYPE_SENSOR	3

typedef struct{
	uint8_t moduletype;			//1=����ʽ   2=����ʽ 3=������
	uint8_t isqualified;		//�Ƿ�ϸ�
	uint8_t isTestStart;		//����������־
	uint8_t staticpowerstep;
	uint8_t dynamicpowerstep;
	eModuleErrTypes errorNo;	//�ڼ������
	uint16_t StdStartDuartion;
	uint32_t testconfigs;		//������
	uint32_t targetbaudrate;
	float staticpowerThreshold_default;
	float staticpowerThreshold_withurge;
	float dynamicpowerThreshold_default;
	float dynamicpowerThreshold_withurge;
	xTestResult *theResult;
}xTestState;



#define MAXLISTNUM		9
#define SPOWER_MIN		0
#define DPOWER_MIN		0
#define STARTTIME_MIN   0
#define SPOWER_MAX		100
#define DPOWER_MAX		100
#define STARTTIME_MAX	3000
typedef struct{
	char defaultwifimsg[100];
	char configwifimsg[100];
	char *Models[MAXLISTNUM];			//��ȷ��ʱ��������ڴ棬ֻ�����һ����Ϊ���β��Զ���
	char *Binfiles[MAXLISTNUM];			//��ȷ��ʱ��������ڴ棬ֻ�����һ����Ϊ���β��Զ���
	float staticpow_default;
	float staticpow_withurge;
	float dynamicpow_default;
	float dynamicpow_withurge;
	uint16_t StdStartDuartion;
	char testerid[8+1];
	char testername[8+1];
	uint8_t isPassive;
	uint8_t isSensor;
	uint8_t isLed;
	uint32_t targetbaudrate;
}xTestConfig;	//~300B


#define MAX_MODELSELECT_NUM		9
#define MAX_MODELFILE_NUM		20
#define MAX_TESTNUM				9
#define MAX_TESTIDLEN			8
#define MAX_TESTNAMELEN			8
typedef struct{
	char model[MAX_MODELSELECT_NUM][MAX_MODELFILE_NUM+1];
	char testid[MAX_TESTNUM][MAX_TESTIDLEN+1];
	char testname[MAX_TESTNUM][MAX_TESTNAMELEN+1];
	char defaultwifimsg[100];
	char configwifimsg[100];
}xTestSelection;


typedef enum{
	null_test=0,
	step_calibrabaudrate,
	step_startduartion,
	step_staticpower,
	step_dynamicpower,
	step_getversion,
	step_checkxor,
	step_testsensor,
	stpe_registeruser,
	step_verifyuser,
	step_deleteuser,
	step_led,
	step_nopasshandle,
	step_max
}eteststeps;


extern const char strErrorTxt[50][20];
extern xTestState hteststate;
extern xTestSelection *pTestconfig;

uint32_t cmdGetTouchVccPower(float *result);			//�������ΪuAֵ������λ��΢����
uint32_t cmdGetModuleVccPower(uint8_t maxtimes,uint8_t validindex,float *result);
uint32_t cmdDetectShortCircuit(float ShortCircuit);
uint32_t cmdCalibrationBaudRate(uint32_t *result);
uint32_t cmdGetBaudRateCnt(void);
uint32_t cmdCaliBaud(uint32_t ifsave,int* offsetBaudrate);
#define CurrentAmterContinousOn()	setCurrentAmter(1)
#define CurrentAmterContinousOff()	setCurrentAmter(0)
uint32_t setCurrentAmter(uint32_t state);

#define LinkIronRing_Sigel()	Module_IronRingSwitch(1)
#define UnlinkIronRing_Sigel()	Module_IronRingSwitch(0)

#define Urge1On()			Urge1Switch(1)
#define Urge1Off()			Urge1Switch(0)
#define Urge2On()			Urge2Switch(1)
#define Urge2Off()			Urge2Switch(0)
#define TouVccOn()			SwitchTouchVcc(1)
#define TouVccOff()			SwitchTouchVcc(0)
#define ModuleVccOn()		SwitchModuleVcc(1)
#define ModuleVccOff()		SwitchModuleVcc(0)

uint32_t Urge1Switch(uint32_t state);
uint32_t Urge2Switch(uint32_t state);
uint32_t SwitchTouchVcc(uint32_t state);				// 1 = �����ص�Դ    0 = ��....
uint32_t SwitchModuleVcc(uint32_t state);
uint32_t Module_IronRingSwitch(uint32_t state);
uint32_t NUM2Switch(uint32_t state);

#endif /* APP_TESTFUNCS_H_ */
