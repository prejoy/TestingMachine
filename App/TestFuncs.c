/*
 * TestFuncs.c
 *
 *  Created on: 2019年7月23日
 *      Author: admin
 */
#include <stdlib.h>
#include <string.h>

#include "TestFuncs.h"
#include "ModuleCommu.h"
#include "dev_INA219.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "tim.h"
#include "App_main.h"
#include "FreeRTOS.h"
#include "task.h"

xTestResult htestresult={
		0
};

xTestState hteststate={.theResult=&htestresult};

xTestSelection *pTestconfig=NULL;

const char strErrorTxt[50][20]={
		"PASS",
		"Touchout",
		"CaliBaudRate",
		"StartDuartion",
		"GetVersion",
		"XorCheck",
		"Flash",
		"LED",
		"GetIMAGE",
		"TouchPower",
		"ModulePower",
		"Sensor"
};


//测三次取平均
uint32_t cmdGetTouchVccPower(float *result)
{
	uint8_t HostSends[4]={0x88,0xAE,0x00,0x11};
	uint8_t HostRecvs[8]={0};
	unsigned char pMem[4] = {0,0,0,0};
	float *p = (float*)pMem;
	float DetectionRes=0.0; //[3]={0.0,0.0,0.0};

	*result = 0.000;

//	__disable_irq();
	CurrentUartSend(HostSends,4);
	HAL_UART_Receive(&CurrentPort,HostRecvs,8,100);
//	__enable_irq();
	if(HostRecvs[0]==0xFA && HostRecvs[1]==0xFB)
	{
		pMem[3]=HostRecvs[3];
		pMem[2]=HostRecvs[4];
		pMem[1]=HostRecvs[5];
		pMem[0]=HostRecvs[6];
		DetectionRes = *p;
	}
	else if(HostRecvs[1]==0xFA && HostRecvs[2]==0xFB)		//容错
	{
		pMem[3]=HostRecvs[4];
		pMem[2]=HostRecvs[5];
		pMem[1]=HostRecvs[6];
		pMem[0]=HostRecvs[7];
		DetectionRes = *p;
	}
	else
	{
//		HAL_UART_Receive(&CurrentPort,HostRecvs,8,5);
		HAL_UART_AbortReceive(&CurrentPort);
		assert_failed(__FILE__,__LINE__);   //接收错误
		return 1;
	}

	*result = DetectionRes;

//	assert_failed("static power",*result);
//	printf("float dat:%f\r\n",DetectionRes);
	return 0;
}


uint32_t setCurrentAmter(uint32_t state)
{
	if(state>0){
		uint8_t HostSends[4]={0x88,0xAE,0x00,0x21};
		CurrentUartSend(HostSends,4);
		return 0;
	}else{
		uint8_t HostSends[4]={0x88,0xAE,0x00,0x01};
		CurrentUartSend(HostSends,4);
		return 0;
	}
	return 0;
}

uint32_t cmdGetModuleVccPower(uint8_t maxtimes,uint8_t validindex,float *result){
#if 1
	uint8_t len;
	float arrayval[maxtimes];
	for (len=0;len<maxtimes;len++){
//		taskENTER_CRITICAL();
		arrayval[len] = ina219_getCurrent_mA(1);
//		taskEXIT_CRITICAL();
//		vTaskDelay(pdMS_TO_TICKS(5));
	}

	float minval=arrayval[0],temp;
	int i,j,minindex=0;
	for (i=0;i<len;i++){
		for (j=i;j<len;j++){
			if(arrayval[j]<minval){
				minval = arrayval[j];
				minindex = j;
			}
		}
		temp = arrayval[i];
		arrayval[i] = arrayval[minindex];
		arrayval[minindex] = temp;
	}

	*result=arrayval[validindex];

	return 0;
#else
	taskENTER_CRITICAL();
	*result = ina219_getCurrent_mA(1);
	taskEXIT_CRITICAL();
	return 0;
#endif
}


uint32_t cmdDetectShortCircuit(float ShortCircuit)
{
	float result;
	taskENTER_CRITICAL();
	result = ina219_getCurrent_mA(2);
	taskEXIT_CRITICAL();

	if(result >= ShortCircuit)
		return 1;
	else
		return 0;
}

//=============          cali boudrate   start          ==================//
//  based on 9600 baudrate ,if not,will communication err
//Next Use for Cali boudrate only
void OEM_Send_Command(void);
uint32_t get_baudrate_cnt(void);
//Macro need to mov to use
#define OEM_Ack_Command   Api_CmdSend
#define GPIO_ReadInputDataBit	HAL_GPIO_ReadPin
#define STD_baud_count   20400
#define BaudRate         9600
uint32_t cmdCalibrationBaudRate(uint32_t *result)
{

	uint32_t Cnt1=0,Cnt2=0,Cnt3=0,Cnt4=10,count=0,baudcount=0;
	uint32_t max,min,ave,subtract,udbaud=0,caliBaud=0;
	uint8_t i=0,j=0,k=0,symbol=0;
	uint32_t buf[10];
	const uint8_t  buf1[3]={0x55,0x55,0x55};
	uint8_t DATA_BUFF[8];


	*result = 1;	//先标记好默认返回结果

	//change module port boudtate to 9600 firstly
#define COMMUTIMES		5
	for(i=0;i<COMMUTIMES;i++){
		OEM_Send_Command();
		HAL_UART_AbortReceive(&ModulePort);
		if(HAL_UART_Receive(&ModulePort,DATA_BUFF,8,20)==HAL_OK)
			break;
		if(i >= COMMUTIMES-1)
			return 1;
	}
#undef COMMUTIMES
//	printf("i=%d\r\n",i);

	memset(DATA_BUFF,0x00,sizeof(DATA_BUFF));

	if(SysStates.osstates == 1)
		vTaskDelay( pdMS_TO_TICKS(20) );
	else
		HAL_Delay(20);

	Cnt4=20;
	while(Cnt4--)
	{
		OEM_Ack_Command(0xa0,00,00,00);

		if(SysStates.osstates == 1)
			vTaskDelay( pdMS_TO_TICKS(20) );
		else
			HAL_Delay(20);

		ModuleUartSend((uint8_t *)buf1,3);
		if(HAL_UART_Receive(&ModulePort,DATA_BUFF,8,20)==HAL_OK)
		{
			if(DATA_BUFF[1]==0xa0)
				break;
		}
		if(Cnt4<=1)
			return 1;
	}
	memset(DATA_BUFF,0x00,sizeof(DATA_BUFF));

	if(SysStates.osstates == 1)
		vTaskDelay( pdMS_TO_TICKS(20) );
	else
		HAL_Delay(20);
	Cnt3=0;
	j=0;
	Cnt4=60;
	while(Cnt4--)
	{
		Cnt1=get_baudrate_cnt();
		if((Cnt1>=10000)&&(Cnt1<=40000))   //计数限制，给定一个有效的计数范围
		 {
			 buf[j%5]=Cnt1;
			 j++;
			 if(j>=5)
			 {
				 min=~0;
				 max=0;
				 ave=0;
				 for(k=0;k<5;k++)
				 {
					 ave +=buf[k];

					 if(buf[k]<min)
						 min=buf[k];
					 if(buf[k]>max)
						 max=buf[k];
				 }

				 if((max-min)<=300)
					{
						ave = (ave)/5;
						if(ave>=STD_baud_count)
						{
							symbol = 1;
						}
						else
						{
							symbol = 2;
						}
						udbaud = ave*BaudRate/STD_baud_count;
						subtract=abs(STD_baud_count-ave);
						if(baudcount==0)
						{
								caliBaud = udbaud;
						}

						if((baudcount>=1)&&(subtract>=1000))
						{
							if(symbol==1)
							{
								caliBaud = caliBaud + 20;
							}
							else if(symbol==2)
							{
								caliBaud = caliBaud - 20;
							}
						}
						else if((baudcount>=1)&&(subtract>=500))
						{
							if(symbol==1)
							{
								caliBaud = caliBaud + 15;
							}
							else if(symbol==2)
							{
								caliBaud = caliBaud - 15;
							}
						}
						else if((baudcount>=1)&&(subtract>=250))
						{
							if(symbol==1)
							{
								caliBaud = caliBaud + 10;
							}
							else if(symbol==2)
							{
								caliBaud = caliBaud - 10;
							}
						}
						else
						{
							if(symbol==1)
							{
								caliBaud = caliBaud + 5;
							}
							else if(symbol==2)
							{
								caliBaud = caliBaud - 5;
							}
						}

						assert_failed("ave",ave);
						assert_failed("udbaud",udbaud);
						assert_failed("caliBaud",caliBaud);
						assert_failed("subtract",subtract);
						if(subtract>=60)
						{
							memset(DATA_BUFF,0x00,sizeof(DATA_BUFF));
							OEM_Ack_Command(0xa0,caliBaud>>8,caliBaud,0);
							baudcount++;
						}
						else
						{
							OEM_Ack_Command(0xa0,caliBaud>>8,caliBaud,0x11);
							assert_failed("baud calibrate success\r\n",0);
							*result = 0;		//校准波特率成功
							HAL_UART_AbortReceive(&ModulePort);
							return 0;
//								break;
						}

					}
					else
					{
						Cnt3++;
					}
					if(Cnt3 >= 30)
					{
						assert_failed("fail\r\n",0);
					}
				}
			}
	}
	HAL_UART_AbortReceive(&ModulePort);
	assert_failed("ave",ave);
	assert_failed("udbaud",udbaud);
	assert_failed("caliBaud",caliBaud);
	assert_failed("subtract",subtract);
	return 1;
}

uint32_t cmdGetBaudRateCnt(void)
{
	uint32_t baudcnt[5]={0};
	uint32_t tempcnt;

	for(int i=0;i<5;++i){
		baudcnt[i] = get_baudrate_cnt();
	}

	for(int i=0;i<4;++i){
		for(int j=i+1;j<5;++j){
			if(baudcnt[i]>=baudcnt[j]){
				tempcnt = baudcnt[i];
				baudcnt[i] = baudcnt[j];
				baudcnt[j] = tempcnt;
			}
		}
	}

	return baudcnt[2];
}

uint32_t cmdCaliBaud(uint32_t ifsave,int* offsetBaudrate)
{
#define STD115200CNT	1685
	uint32_t nowcnt=0;
	int16_t nowbaud =0;
	uint8_t CmdAckRecv[10]={0};
	uint32_t okflag=0;

	if(ifsave == 1){
		Api_CmdSend(0xa0,0,0,(1<<4)|1);
		HAL_UART_Receive(&ModulePort,CmdAckRecv,10,200);
		HAL_UART_Abort(&ModulePort);
		for(int i=0;i<3;i++){
			if(CmdAckRecv[i]==0xF5 && CmdAckRecv[i+1]==0xa0 \
					&& CmdAckRecv[i+7]==0xF5)
			{
				return 0;
			}
		}
		return 1;
	}

	if(SysStates.osstates >0)	vTaskDelay( pdMS_TO_TICKS(200) );
	else 						HAL_Delay(200);

	Api_CmdSend(0xf1,0,0,0xf1);
	HAL_UART_Receive(&ModulePort,CmdAckRecv,10,100);
	HAL_UART_Abort(&ModulePort);
	for(int i=0;i<5;i++){
		if(CmdAckRecv[i]==0xF5 && CmdAckRecv[i+1]==0xf1 \
				&& CmdAckRecv[i+2]==0 && CmdAckRecv[i+3]==0\
				&& CmdAckRecv[i+4]==1 && CmdAckRecv[i+5]==0\
				&& CmdAckRecv[i+6]==0xF0 && CmdAckRecv[i+7]==0xF5)
		{
			break;
		}

		if(i>2){
			assert_failed("adapter baud unconnect\r\n",0);
			return 1;
		}
	}
	memset(CmdAckRecv,0,sizeof(CmdAckRecv));


	nowcnt = cmdGetBaudRateCnt();
	nowbaud = STD115200CNT-nowcnt;
	nowbaud = nowbaud*115200/STD115200CNT;
	*offsetBaudrate = nowbaud;

	Api_CmdSend(0xa0,nowbaud>>8,nowbaud,(1<<4)|0);
	HAL_UART_Receive(&ModulePort,CmdAckRecv,10,100);
	HAL_UART_Abort(&ModulePort);
	for(int i=0;i<3;i++){
		if(CmdAckRecv[i]==0xF5 && CmdAckRecv[i+1]==0xa0 \
				&& CmdAckRecv[i+7]==0xF5)
		{
			return 0;
		}
	}

	return 1;

}



//Next Use for Cali boudrate only
void OEM_Send_Command(void)
{
	uint8_t data[8];

	data[0]=0xf5;
	data[1]=0x55;
	data[2]=0x55;
	data[3]=0x55;
	data[4]=0x55;
	data[5]=0x55;
	data[6]=data[1]^data[2]^data[3]^data[4]^data[5];
	data[7]=0xf5;
	if(SysStates.osstates >0)	taskENTER_CRITICAL();
	ModuleUartSend(data,8);
	if(SysStates.osstates >0)	taskEXIT_CRITICAL();
}


uint32_t get_baudrate_cnt(void)
{
	uint32_t i,Cnt1=0,Cnt2=0;
	uint32_t systicksoverflow;
	//j=0,			//以后如果硬件出硬件故障，在while(1)中加入j变量用于超时退出

	if(SysStates.osstates == 1)
		taskENTER_CRITICAL();

	OEM_Ack_Command(0xf1,0,0,0xf1);
	 for(i=0;i<5000000;i++)
		{
			 if(GPIO_ReadInputDataBit(BDRT_CALIBRATION_GPIO_Port, BDRT_CALIBRATION_Pin) == 1)
			 {
					Cnt2++;
			 }
			 if(GPIO_ReadInputDataBit(BDRT_CALIBRATION_GPIO_Port, BDRT_CALIBRATION_Pin) == 0)
			 {
				 Cnt2=0;
				 __disable_irq();
				 while(1)		//超时50ms
				 {
						Cnt1++;
						if(GPIO_ReadInputDataBit(BDRT_CALIBRATION_GPIO_Port, BDRT_CALIBRATION_Pin) == 1){
							__enable_irq();
							break;
						}
				 }
			 }
			 if(Cnt2>=30000 )
			 {
				 break;
			 }
		}
		if(SysStates.osstates == 1)
			taskEXIT_CRITICAL();

		return Cnt1;
}
//=============          cali boudrate   end          ==================//


uint32_t SwitchTouchVcc(uint32_t state)
{
	HAL_GPIO_WritePin(TOUCHVCC_CTRL_GPIO_Port,TOUCHVCC_CTRL_Pin,(GPIO_PinState)(1 - (state&GPIO_PIN_SET) ));
	return 0;
}

uint32_t SwitchModuleVcc(uint32_t state)
{
	HAL_GPIO_WritePin(MODULE_PWRCTRL_GPIO_Port,MODULE_PWRCTRL_Pin,(GPIO_PinState)(1 - (state&GPIO_PIN_SET)));
	return 0;
}

uint32_t Urge1Switch(uint32_t state)
{
	if(state>0)
		return HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	else
		return HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
}


uint32_t Urge2Switch(uint32_t state)
{
	if(state>0)
		return HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	else
		return HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_2);
}

uint32_t Module_IronRingSwitch(uint32_t state)
{
	HAL_GPIO_WritePin(RelaySwitch1_GPIO_Port,RelaySwitch1_Pin,(GPIO_PinState)((state&GPIO_PIN_SET)));
	return 0;
}

uint32_t NUM2Switch(uint32_t state)
{
	HAL_GPIO_WritePin(RelaySwitch2_GPIO_Port,RelaySwitch2_Pin,(GPIO_PinState)(1 - (state&GPIO_PIN_SET)));
	return 0;
}

