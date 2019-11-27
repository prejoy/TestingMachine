#include <string.h>
#include "ModuleCommu.h"
#include "usart.h"
#include "App_define.h"
#include "App_main.h"
#include "stm32f4xx.h"
#include "ff.h"
#include "exsram.h"
#include "TestFuncs.h"
#include "FreeRTOS.h"
#include "task.h"

//放到外部
//uint8_t FINGERIMAGE[176*176];		//暂时支持最大的传感器为176*176的，如果有更大的需要修改
//uint8_t FINGERIMAGE[512];
//xRegMode RegMode={
//		_NCNR,
//		NOFGPUPCHK,
//		NOFAKEFGPCHK,
//		BACKFIRST,
//		4,
//		3
//};
uint32_t Api_CmdSend(uint8_t ucCmd, uint8_t ucVar1, uint8_t ucVar2, uint8_t ucVar3)
{
    uint8_t data[8];
    data[0]=0xf5;
    data[1]=ucCmd;
    data[2]=ucVar1;
    data[3]=ucVar2;
    data[4]=ucVar3;
    data[5]=0;
    data[6]=data[1]^data[2]^data[3]^data[4]^data[5];
    data[7]=0xf5;
    if(SysStates.osstates >0)	taskENTER_CRITICAL();
    ModuleUartSend(data,8);
    if(SysStates.osstates >0)	taskEXIT_CRITICAL();
    return 0;
}



uint32_t cmdGetFingerImage(uint8_t *width,uint8_t *height,uint8_t *IMGSAVADDR)
{
	uint32_t RecvLen,retval=1;
	uint8_t CmdAckRecv[8]={0,0};

//	if(IMGSAVADDR == 0){
//		IMGSAVADDR = (uint8_t *)pAllRecords->hImgSaves.oriImgaddr;
//	}

	*width = 0;
	*height = 0;
	if(SysStates.osstates>0)taskENTER_CRITICAL();
	Api_CmdSend(CMD_GET_FINGER_IMAGE,0,0,0);
	HAL_UART_Receive(&ModulePort,CmdAckRecv,8,300);
	if(SysStates.osstates>0)taskEXIT_CRITICAL();

	if(CmdAckRecv[0]!=0xF5 || CmdAckRecv[1]!=CMD_GET_FINGER_IMAGE || CmdAckRecv[4]>0)
	{
		assert_failed(__FILE__,__LINE__);
		if((CmdAckRecv[0]|CmdAckRecv[1])==0){
			Api_CmdSend(CMD_GET_FINGER_IMAGE,0,0,0);		//停止采图
			HAL_UART_Receive(&ModulePort,CmdAckRecv,8,200);
			if(CmdAckRecv[0]==0xF5 && CmdAckRecv[1]==CMD_GET_FINGER_IMAGE && CmdAckRecv[4]==0x18)
			{
				return 2;
			}
		}
		return 1;
	}

	*width = CmdAckRecv[2]<<2;
	*height = CmdAckRecv[3]<<2;

	RecvLen = ((CmdAckRecv[2]<<2) * CmdAckRecv[3]<<2)+3;		//最后保存的数据包括F5头尾及校验值的
	HAL_UART_Receive_DMA(&ModulePort,IMGSAVADDR,RecvLen);		//补充完成标志,注意CCM内存不可使用

	for(int i=0;i<18;i++){
		if(IMGSAVADDR[0]!=0xF5 || IMGSAVADDR[RecvLen-1]!=0xF5){
			if(SysStates.osstates!=0)		vTaskDelay( pdMS_TO_TICKS(200) );				//32K -> 115200 > 2.9S
			else							HAL_Delay(200);
		}
		else{
			// 后续可添加校验，判断其正确性
			retval = 0;
			break;

		}
	}
	HAL_UART_DMAStop(&ModulePort);

	if(retval)		//暂时没有添加校验!!
	{
		assert_failed(__FILE__,__LINE__);
	}

	return retval;

}


uint32_t cmdGetModuleVersion(uint8_t *pmsg)
{
	uint32_t RecvLen;
	uint8_t CmdAckRecv[8];

	memset(pmsg,0,256);
	Api_CmdSend(CMD_GET_MODULE_VERSION,0,0,0);
	HAL_UART_Receive(&ModulePort,CmdAckRecv,8,500);
//	HAL_UART_Receive_DMA(&ModulePort,CmdAckRecv,8);	//配置DMA接收
//	while((HAL_GetTick()<overtick)&&(__HAL_DMA_GET_COUNTER(&hdma_usart2_rx)>0));	//获取剩余字节数 ，得到当前还剩余多少个数据


	if(CmdAckRecv[0]!=0xF5 || CmdAckRecv[1]!=CMD_GET_MODULE_VERSION || (CmdAckRecv[1]^CmdAckRecv[2]^CmdAckRecv[3]^CmdAckRecv[4]^CmdAckRecv[5])!=CmdAckRecv[6])
	{
		assert_failed(__FILE__,__LINE__);
		return 1;
	}

	RecvLen = ((CmdAckRecv[2]<<8) | CmdAckRecv[3])+3;
	HAL_UART_Receive(&ModulePort,pmsg,RecvLen,1000);	//配置DMA接收
//	strcpy((char *)pmsg,(const char *)(pmsg+1));
	assert_failed(pmsg+1,0);		//0xF5为数据头，不需要显示

	return 0;
}

uint32_t cmdGetXorVal(uint8_t *xorarray,uint16_t len,uint8_t addr)		//数组长度用12的，实际大小为8+3 , len=120字节单位， binfilesize/120+1，addr 单位1KB
{
	uint32_t RecvLen;
	uint8_t CmdAckRecv[8];

	memset(xorarray,0,12);
	Api_CmdSend(CMD_GETXOR,len>>8,len&0x00FF,addr);
	HAL_UART_Receive(&ModulePort,CmdAckRecv,8,1500);

	if(CmdAckRecv[0]!=0xF5 || CmdAckRecv[1]!=CMD_GETXOR )
	{
		assert_failed(__FILE__,__LINE__);
		return 1;
	}

	RecvLen = ((CmdAckRecv[2]<<8) | CmdAckRecv[3])+3;
	HAL_UART_Receive(&ModulePort,xorarray,RecvLen,1500);	//配置DMA接收

	uint8_t xortemp=xorarray[1]^xorarray[2]^xorarray[3]^xorarray[4]^xorarray[5]^xorarray[6]^xorarray[7]^xorarray[8];
	if(xortemp!=xorarray[9]){
		return 1;
	}else{
		return 0;
	}

}


//过程修改：去掉平台本身浪费的时间
uint32_t cmdGetModuleUserNums(uint32_t overcnt,uint32_t *duartion)
{
	uint32_t i,tickbefore,step=60;
	uint8_t CmdAckRecv[8];

	*duartion = 0;

	//显然模组断电200ms，保证掉电状态
	ModuleVccOff();
	HAL_Delay(200);
	for(i=0;i<overcnt;i++)
	{
		//模组上电
		ModuleVccOn();
		tickbefore = HAL_GetTick();
		HAL_Delay(step);
		Api_CmdSend(CMD_GET_TOTAL_USER_NUM,0,0,0);
		if(HAL_UART_Receive(&ModulePort,CmdAckRecv,8,40)!=HAL_OK){
			ModuleVccOff();HAL_Delay(200);	//放电
			step+=40;
			HAL_UART_AbortReceive(&ModulePort);
			continue;
		}
		else{
			for(int i=0;i<7;i++){
				if(CmdAckRecv[i]==0xF5 && CmdAckRecv[i+1]==CMD_GET_TOTAL_USER_NUM){
					*duartion = HAL_GetTick() - tickbefore;
					return 0;
				}
			}

			{
				//模组断电,200ms后再次尝试
				ModuleVccOff();
				HAL_Delay(200);
				continue;
			}
		}

	}
	assert_failed(__FILE__,__LINE__);
	return 1;
}

uint32_t cmdWriteFreture(uint32_t *result,uint8_t *recvbuf,uint32_t fealen)
{
	uint8_t CmdAckRecv[8]={0xF5,0,1,1,0};

	*result = 1;
	CmdAckRecv[6]=CmdAckRecv[1]^CmdAckRecv[2]^CmdAckRecv[3]^recvbuf[fealen];
	CmdAckRecv[7]=0xf5;
	Api_CmdSend(CMD_WRITEFEA,0x20,0x03,0);				//只确认1号ID的feature
	ModuleUartSend(CmdAckRecv,4);
	ModuleUartSend(recvbuf,fealen);
//	ModuleUartSend_DMA(recvbuf,fealen);
//	while(HAL_UART_GetState(&ModulePort) != HAL_UART_STATE_READY);
	ModuleUartSend(CmdAckRecv+6,2);

	HAL_UART_AbortReceive(&ModulePort);
	if(HAL_UART_Receive(&ModulePort,CmdAckRecv,8,5000)!=0){
		assert_failed(__FILE__,__LINE__);
		return 1;
	}

	if(CmdAckRecv[0]!=0xF5 || CmdAckRecv[1]!=CMD_WRITEFEA)
	{
		assert_failed(__FILE__,__LINE__);
		return 1;
	}
	else{
		*result = CmdAckRecv[4];
		return 0;
	}


}

uint32_t cmdReadFreture(uint32_t *result,uint8_t *recvbuf)
{
	uint8_t CmdAckRecv[8];

	*result = 1;
	if(recvbuf == 0){
		recvbuf = (uint8_t *)pAllRecords->hImgSaves.oriImgaddr;
		memset(recvbuf,0,32*1024);			//64K 特征值需要修改
	}
	Api_CmdSend(CMD_READFEA,0,1,0);				//只确认1号ID的feature，一次接收
	HAL_UART_Receive(&ModulePort,CmdAckRecv,8,500);
	if(CmdAckRecv[0]==0xF5 && CmdAckRecv[1]==CMD_READFEA && CmdAckRecv[4]==0x00)	//OK
	{
//		HAL_UART_Receive(&ModulePort,recvbuf,8206-8,30*1000);		//8206-8 = 8198

//		taskENTER_CRITICAL();
//		uint32_t tempresult = HAL_UART_Receive(&ModulePort,recvbuf,8206-8,3*1000);
//		taskEXIT_CRITICAL();

		uint32_t tempresult = HAL_UART_Receive_DMA(&ModulePort,recvbuf,8206-8);
		if(SysStates.osstates!=0)
			vTaskDelay( pdMS_TO_TICKS(1000) );				//8K -> 115200 > 0.8S
		else
			HAL_Delay(1000);
		HAL_UART_DMAStop(&ModulePort);

		if(tempresult!=0){
			assert_failed(__FILE__,__LINE__);
			return 1;
		}
		if(recvbuf[0]==0xF5 && recvbuf[8197]==0xF5){
			*result = 0;
			return 0;
		}else
		{
			assert_failed(__FILE__,__LINE__);
			return 1;
		}
	}
	else
	{
		assert_failed(__FILE__,__LINE__);
		return 1;
	}

}




//uint32_t cmdRegUser(xRegMode *RegMode,uint32_t *result)
//{
//	int i;
//	uint32_t udret=1;
//	uint8_t CmdAckRecv[8];
//
//	*result = 1;
//	switch (RegMode->mode)
//	{
//		case _1CNR:
//			assert_failed(__FILE__,__LINE__);
//			udret = 1;
//			break;
//		case _NCNR:
//			for (i=0;i<RegMode->capturetimes;i++){
//				HAL_Delay(5);
//				Api_CmdSend(CMD_REGISTER_USER,0,0,1);		//分配ID，role为1
//				HAL_UART_Receive(&ModulePort,CmdAckRecv,8,8000);
//				if(CmdAckRecv[0]==0xF5 && CmdAckRecv[4]==0x00)	// recv OK
//				{
//					if(CmdAckRecv[1]==1)
//					{
//						continue;
//					}
//					else if (CmdAckRecv[1]==3)
//					{
//						*result = 0;
//						break;
//					}
//					else
//					{
//						assert_failed(__FILE__,__LINE__);
//					}
//
//					continue;
//				}
//				else if(CmdAckRecv[0]==0xF5 && CmdAckRecv[4]>0)		//reg fail
//				{
//					assert_failed(__FILE__,__LINE__);
//					*result = CmdAckRecv[4];
//					assert_failed("reg err no",CmdAckRecv[4]);
//					break;
//				}
//			}
//			if(i>=RegMode->capturetimes-1)
//				udret = 0;
//			break;
//		default:
//			assert_failed(__FILE__,__LINE__);
//			break;
//	}
//
//	return udret;
//}

uint32_t cmdVfyUser(uint32_t *result)
{
	uint32_t udret=1;
	uint8_t CmdAckRecv[8];

	*result = 1;
	Api_CmdSend(CMD_VERIFY_ONE_TO_ALL,0,0,0);
	HAL_UART_Receive(&ModulePort,CmdAckRecv,8,8000);

	if(CmdAckRecv[0]==0xF5 && (CmdAckRecv[2]>0 || CmdAckRecv[3]>0)){
		*result = 0;
		udret = 0;
	}else if (CmdAckRecv[0]==0xF5 && CmdAckRecv[2]==0 && CmdAckRecv[3]==0){
		*result = 1;
		udret = 0;
	}else{
		udret=1;
		assert_failed(__FILE__,__LINE__);
	}

	return udret;
}

uint32_t cmdDelAllUser(uint32_t *result)
{
	uint32_t udret=1;
	uint8_t CmdAckRecv[8];

	*result = 1;
	Api_CmdSend(CMD_DELETE_ALL_USER,0,0,0);
	HAL_UART_Receive(&ModulePort,CmdAckRecv,8,5000);		//删除操作话费时间久

	if(CmdAckRecv[0]==0xF5 && CmdAckRecv[4]==0x00){
		*result = 0;
		udret = 0;
	}else if (CmdAckRecv[0]==0xF5 && CmdAckRecv[4]>0){
		*result = 1;
		udret = 0;
		assert_failed(__FILE__,__LINE__);
	}else{
		udret=1;
		assert_failed(__FILE__,__LINE__);
	}

	return udret;
}


uint32_t cmdLedTest(uint32_t *result)
{
	uint32_t udret=1;
	uint8_t CmdAckRecv[8];
	uint8_t ledcolorarray[]={3,5,6,0,7};	//红绿蓝白灭

	for (int i=0;i<5;i++){
		*result = 1;
		Api_CmdSend(CMD_LED,ledcolorarray[i],ledcolorarray[i],0xC8);
		HAL_UART_Receive(&ModulePort,CmdAckRecv,8,200);

		if(CmdAckRecv[0]==0xF5 && CmdAckRecv[4]==0x00){
				*result = 0;
				udret = 0;
				HAL_Delay(80);
				continue;
		}else if (CmdAckRecv[0]==0xF5 && CmdAckRecv[4]>0){
			udret = 0;
			assert_failed(__FILE__,__LINE__);
			break;
		}else{
			udret=1;
			assert_failed(__FILE__,__LINE__);
			break;
		}
	}
	return udret;
}

uint32_t cmdLedSingleColor(uint32_t color)
{
	uint8_t CmdAckRecv[8];

	Api_CmdSend(CMD_LED,(uint8_t)color,(uint8_t)color,0xC8);
	HAL_UART_Receive(&ModulePort,CmdAckRecv,8,2000);

	if(CmdAckRecv[0]==0xF5 && CmdAckRecv[4]==0x00){
		return 0;
	}else{
		assert_failed(__FILE__,__LINE__);
		return 1;
	}
}

uint32_t cmdGetModuleUID(uint8_t *pUID)
{
	uint8_t CmdAckRecv[19];

	Api_CmdSend(CMD_GETUID,0,0,0);
	if(HAL_UART_Receive(&ModulePort,CmdAckRecv,19,1500) == 0)
	{
		if(CmdAckRecv[0]==0xF5 && CmdAckRecv[8]==0xF5 \
				&& CmdAckRecv[9]^CmdAckRecv[10]^CmdAckRecv[11]^CmdAckRecv[12]^CmdAckRecv[13]^CmdAckRecv[14]^CmdAckRecv[15]^CmdAckRecv[16] == CmdAckRecv[17]\
		){
			memcpy(pUID,CmdAckRecv+9,8);
			return 0;
		}
		else{
			memset(pUID,0,8);
			assert_failed(__FILE__,__LINE__);
			return 1;
		}
	}else
	{
		memset(pUID,0,8);
		assert_failed(__FILE__,__LINE__);
		return 1;
	}


}


static void hostchangemodulebaudrate()
{
	switch(ModulePort.Init.BaudRate){
		case 115200:
			ModulePort.Init.BaudRate=9600;
			break;
		case 9600:
			ModulePort.Init.BaudRate=115200;
			break;
		default:
			ModulePort.Init.BaudRate=115200;
			break;
	}
	HAL_UART_DeInit(&ModulePort);
	if(HAL_UART_Init(&ModulePort)!=HAL_OK){
		assert_failed(__FILE__,__LINE__);
		while(HAL_UART_Init(&ModulePort)>0);
	}

	return ;
}

uint32_t cmdAdapterModuleBaudrate(uint32_t targetbaud,uint32_t resolution)
{
	uint8_t CmdAckRecv[10]={0};
	int  negcnt=0,poscnt=0,negindex = 0,posindex = 0;;
	int offsetbaud = 0,step = 576,basebaud = (int)targetbaud,realbaud = (int)targetbaud;
	const int caliStep[]={576,288,144};
	int connectedDomain[10][2]={0};		//最多6个连通域，记录区域的首位索引
	int domainindex = 0,continuousFlag = 0,recordFlag = 0;

	if(basebaud == 115200)
		step = caliStep[resolution];
	else if(basebaud == 9600)
		step = 48;

	for (int i=-20;i<20+1;i++){
		offsetbaud = step*i;
		realbaud = basebaud + offsetbaud;

		HAL_UART_DeInit(&ModulePort);
		ModulePort.Init.BaudRate = realbaud;
		if(HAL_UART_Init(&ModulePort)!=HAL_OK){
			assert_failed(__FILE__,__LINE__);
			while(HAL_UART_Init(&ModulePort)>0);
		}


		Api_CmdSend(0xf1,0,0,0xf1);
		HAL_UART_Receive(&ModulePort,CmdAckRecv,10,20);
		HAL_UART_Abort(&ModulePort);
		for(int i=0;i<5;i++){
			if(CmdAckRecv[i]==0xF5 && CmdAckRecv[i+1]==0xf1 \
					&& CmdAckRecv[i+2]==0 && CmdAckRecv[i+3]==0\
					&& CmdAckRecv[i+4]==1 && CmdAckRecv[i+5]==0\
					&& CmdAckRecv[i+6]==0xF0 && CmdAckRecv[i+7]==0xF5)
			{
				if(continuousFlag == 0)	{
					continuousFlag = 1;
					domainindex++;
					recordFlag = 2|1;
					if(domainindex == 10){
						domainindex = 0;
						assert_failed(__FILE__,__LINE__);
					}
				}
//				++negcnt;
				break;
			}else{
				if(i>2){
					continuousFlag = 0;
					break;
				}
			}
		}
		memset(CmdAckRecv,0,sizeof(CmdAckRecv));
		if(continuousFlag == 0){
			if (recordFlag & 1){
				connectedDomain[domainindex-1][1] = i;
				recordFlag &=~1;
			}
		}else if(continuousFlag == 1){
			if(recordFlag & 2){
				connectedDomain[domainindex-1][0] = i;
				recordFlag &=~2;
			}
		}


	}

	int maxDomainLength=0;
	realbaud = targetbaud;
	for(int i = 0;i<domainindex;i++){
		int temp = connectedDomain[i][1]-connectedDomain[i][0];
		if(temp > maxDomainLength){
			maxDomainLength = temp;
			realbaud = targetbaud + step/2*((connectedDomain[i][1]+connectedDomain[i][0])-2);
		}
	}


/*
	for(uint32_t i=20;i;--i){
		offsetbaud = step*i;
		realbaud = basebaud-offsetbaud;

		HAL_UART_DeInit(&ModulePort);
		ModulePort.Init.BaudRate = realbaud;
		if(HAL_UART_Init(&ModulePort)!=HAL_OK){
			assert_failed(__FILE__,__LINE__);
			while(HAL_UART_Init(&ModulePort)>0);
		}

		Api_CmdSend(0xf1,0,0,0xf1);
		HAL_UART_Receive(&ModulePort,CmdAckRecv,10,20);
		HAL_UART_Abort(&ModulePort);
		for(int i=0;i<3;i++){
			if(CmdAckRecv[i]==0xF5 && CmdAckRecv[i+1]==0xf1 \
					&& CmdAckRecv[i+2]==0 && CmdAckRecv[i+3]==0\
					&& CmdAckRecv[i+4]==1 && CmdAckRecv[i+5]==0\
					&& CmdAckRecv[i+6]==0xF0 && CmdAckRecv[i+7]==0xF5)
			{
				++negcnt;
				break;
			}
		}
		memset(CmdAckRecv,0,sizeof(CmdAckRecv));
		if(negcnt == 1){
			negindex = i;
		}
	}

	for(uint32_t i=20;i;--i){
		offsetbaud = step*i;
		realbaud = basebaud+offsetbaud;

		HAL_UART_DeInit(&ModulePort);
		ModulePort.Init.BaudRate = realbaud;
		if(HAL_UART_Init(&ModulePort)!=HAL_OK){
			assert_failed(__FILE__,__LINE__);
			while(HAL_UART_Init(&ModulePort)>0);
		}

		Api_CmdSend(0xf1,0,0,0xf1);
		HAL_UART_Receive(&ModulePort,CmdAckRecv,10,20);
		HAL_UART_Abort(&ModulePort);
		for(int i=0;i<3;i++){
			if(CmdAckRecv[i]==0xF5 && CmdAckRecv[i+1]==0xf1 \
					&& CmdAckRecv[i+2]==0 && CmdAckRecv[i+3]==0\
					&& CmdAckRecv[i+4]==1 && CmdAckRecv[i+5]==0\
					&& CmdAckRecv[i+6]==0xF0 && CmdAckRecv[i+7]==0xF5)
			{
				++poscnt;
				break;
			}
		}
		memset(CmdAckRecv,0,sizeof(CmdAckRecv));
		if(poscnt == 1){
			posindex = i;
		}
	}

	//filter
	if(abs(negcnt-poscnt) > 6){
		if(negcnt >18 || poscnt < 4) posindex=0;
		if(poscnt >18 || negcnt < 4) negindex=0;
	}



	realbaud = ((115200 - step*negindex)+(115200 + step*posindex))/2;
*/


	if(ModulePort.Init.BaudRate != realbaud){
		HAL_UART_Abort(&ModulePort);
		HAL_UART_DeInit(&ModulePort);
		ModulePort.Init.BaudRate = realbaud;
		if(HAL_UART_Init(&ModulePort)!=HAL_OK){
			assert_failed(__FILE__,__LINE__);
			while(HAL_UART_Init(&ModulePort)>0);
		}
	}
	return realbaud;
}


uint32_t cmdChangeModuleBaudrate(uint32_t targetbaudrate,uint32_t iffoeever)
{
	uint32_t udret=1;
	uint8_t CmdAckRecv[8],bdrt=0;

	switch(targetbaudrate){
		case 9600:
			bdrt=1;
			break;
		case 19200:
			bdrt=2;
			break;
		case 38400:
			bdrt=3;
			break;
		case 57600:
			bdrt=4;
			break;
		case 115200:
			bdrt=5;
			break;
		default:
			bdrt=5;
			break;
	}
	HAL_UART_AbortReceive(&ModulePort);
	Api_CmdSend(CMD_CHANGEMODULEBAUDRATE,0,bdrt,iffoeever>0?1:0);
	HAL_UART_Receive(&ModulePort,CmdAckRecv,8,2000);		//删除操作话费时间久

	udret=1;
	for (int i = 0;i<4;i++){
		if(CmdAckRecv[i]==0xF5 && CmdAckRecv[i+1]==CMD_CHANGEMODULEBAUDRATE && CmdAckRecv[i+4]==0x00){
			udret = 0;
			break;
		}
	}
	if(udret == 1){
		assert_failed(__FILE__,__LINE__);
	}
	return udret;
}

uint32_t cmdFlashCheck(uint8_t *badcont,uint8_t *allcont)
{
	uint32_t udret=1;
	uint8_t CmdAckRecv[8];

	*allcont = 0;*badcont = 0;
	Api_CmdSend(CMD_FLASHCHECK,0,0,0);
	if(HAL_UART_Receive(&ModulePort,CmdAckRecv,8,2000)!=HAL_OK)		//删除操作话费时间久
		return udret;

	if(CmdAckRecv[0]==0xF5 && CmdAckRecv[1]==CMD_FLASHCHECK){
		udret = 0;
		*allcont = CmdAckRecv[3];*badcont = CmdAckRecv[2];
	}else{
		udret=1;
		assert_failed(__FILE__,__LINE__);
	}

	return udret;
}
