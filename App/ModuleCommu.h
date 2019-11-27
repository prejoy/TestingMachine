#ifndef __BSP_MODULECOMMU_H__
#define __BSP_MODULECOMMU_H__

#include "stm32f4xx.h"
#include "ff.h"

/*返回值*/
#define TA_SUCCESS					0x00
#define TA_FAIL						0x01
#define TA_FALSE            		0x00
#define TA_TRUE             		0x01

#define TA_ALL_USER     			0x00
#define TA_GUEST_USER   			0x01
#define TA_NORMAL_USER 			    0x02
#define TA_MASTER_USER				0x03
#define TA_FULL         			0x04
#define TA_NO_USER					0x05
#define TA_USER_EXIST 				0x07
#define TA_TIME_OUT					0x08
#define TA_FLASH_ERROR				0x09
#define TA_HARDWARE_ERROR           0x0a
#define TA_IMAGE_ERROR              0x10
#define TA_RECAP_IMAGE              0x17
#define TA_BREAK					0x18
#define TA_OTP_ERROR                0x19
#define TA_FPCSLEEP_ERROR           0x20

/*串口命令字*/
#define CMD_REGISTER_USER			(0x01)
#define CMD_REGISTER_TWO			(0X02)
#define CMD_REGISTER_THREE			(0X03)
#define CMD_DELETE_SPECIFIC_USER	(0x04)
#define CMD_DELETE_ALL_USER		    (0x05)
#define CMD_GET_TOTAL_USER_NUM	    (0x09)
#define CMD_VERIFY_ONE_TO_ONE		(0x0B)
#define CMD_VERIFY_ONE_TO_ALL		(0x0C)
#define CMD_GET_FEA				    (0x23)
#define CMD_GET_FINGER_IMAGE		(0x24)
#define CMD_GET_MODULE_VERSION		(0x26)
#define CMD_PROG_UPGRADE			(0x57)
#define CMD_TEST_SENSOR             (0xF4)
#define CMD_CALIBRATION             (0x80)
/*串口命令字 补充*/
#define CMD_GETXOR	             	(0xE1)
#define CMD_LED						(0xC3)
#define CMD_READFEA					(0x31)
#define CMD_WRITEFEA				(0x41)
#define CMD_GETUID					(0x60)
#define CMD_CHANGEMODULEBAUDRATE	(0x21)


/*升级包长度设定*/
#define  REAL_PRO_PKG_LEN						(0x78)	//120

#define _1CNR						0
#define _NCNR						1
#define NOFGPUPCHK					0
#define FGPUPCHK					1
#define NOFAKEFGPCHK				0
#define FAKEFGPCHK					1
#define BACKFIRST					0
#define LEARNFIRST					1


//typedef struct{
//	unsigned mode:1;
//	unsigned fgpupchk:1;
//	unsigned fakefgpchk:1;
//	unsigned learnprio:1;
//	unsigned tongyuan:3;
//	unsigned capturetimes:5;
//}xRegMode;


//extern xRegMode RegMode;

uint32_t Api_CmdSend(uint8_t ucCmd, uint8_t ucVar1, uint8_t ucVar2, uint8_t ucVar3);
uint32_t cmdGetFingerImage(uint8_t *width,uint8_t *height,uint8_t *IMGSAVADDR);
uint32_t cmdGetModuleVersion(uint8_t *pmsg);
uint32_t cmdGetModuleUserNums(uint32_t overcnt,uint32_t *duartion);
uint32_t cmdGetModuleUID(uint8_t *pUID);
uint32_t cmdAdapterModuleBaudrate(uint32_t targetbaud,uint32_t resolution);
uint32_t cmdChangeModuleBaudrate(uint32_t targetbaudrate,uint32_t iffoeever);

//uint32_t cmdRegUser(xRegMode *RegMode,uint32_t *result);
uint32_t cmdVfyUser(uint32_t *result);							//只有1：N的比对
uint32_t cmdDelAllUser(uint32_t *result);		//删除全部user
uint32_t cmdLedTest(uint32_t *result);
uint32_t cmdLedSingleColor(uint32_t color);
uint32_t cmdGetXorVal(uint8_t *xorarray,uint16_t len,uint8_t addr);		//数组长度用12的，实际大小为8+3 , len=120字节单位， binfilesize/120+1，addr 单位1KB
																		//发送E1指令，参数1和2合起来用作校验长度，单位是120字节，参数3是起始地址，单位是1024.

uint32_t cmdReadFreture(uint32_t *result,uint8_t *recvbuf);
uint32_t cmdWriteFreture(uint32_t *result,uint8_t *recvbuf,uint32_t fealen);
uint32_t cmdFlashCheck(uint8_t *allcont,uint8_t *badcont);

#endif

