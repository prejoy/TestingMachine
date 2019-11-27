#ifndef _APP_DEFINE_H_
#define _APP_DEFINE_H_



/*算法版本号*/
#define ALGO_8V8X                   3
#define ALGO_VERSION                ALGO_8V8X






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
#define CMD_FLASHCHECK				(0x62)

/*api相关*/

#define REPEAT_ID_START             10000

#define MAX_USER_NUM				1024
#define PRE_MATCH_2ND_NUM           (MAX_USER_NUM*16/100+1)

#define DB_SMALL_SECTION			4096
#define FEA_HEAD_8K_LEN             1600    // 快速索引阶段仅需要读部分特征值
#define FAST_MATCH_THR_NUM          (MAX_USER_NUM/2/2)
#define REAL_FEA_LEN				8192
#define FEA_LEN                     (8*1024)
#define FEA_STORE_LEN				(8*1024*1)

#define SPI_FLASH					0
#define SPI_SENSOR					1

/*升级包长度设定*/
#define  REAL_PRO_PKG_LEN						(0x78)	//120

/*格式化删除与标记删除设定*/
#define  Format_Deletion    1       //1使能;0失能：格式化删除

#define OFFSET_TEST_H    200
#define OFFSET_TEST_L    50

/*算法相关*/
#define MATCH_PERFECT               1000
#define MATCH_THRESHOLD			    800	
#define MATCH_L3_THRHLD             600
#define MATCH_POSSIBLE				400	
#define MERGE_LIMIT 100


/*芯片相关*/
#define TICK_CNT (6000000 / 100)
#define SENSOR_SPI_CLK 24000000
#define FLASH_SPI_CLK 24000000
#define HEAP_SIZE 48*1024
#define IMG_ADDR		            0x20001800
#define ALGO_START_ADDR         (IMG_ADDR + 32*1024)



#endif

