/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "App_main.h"
#include "sdio.h"
#include "diskio.h"		/* FatFs lower layer API */
#include "stdlib.h"

extern xSysState SysStates;
volatile uint32_t SD_RWState=0;		//bit0=rx state bit1=tx sstate

#define SD_CARD	 0  //SD卡,卷标为0
//#define EX_FLASH 1	//外部flash,卷标为1

#define FLASH_SECTOR_SIZE 	512			  
//对于W25Q128
//前12M字节给fatfs用,12M字节后,用于存放字库,字库占用3.09M.	剩余部分,给客户自己用	 			    
//u16	    FLASH_SECTOR_COUNT=2048*12;	//W25Q1218,前12M字节给FATFS占用
#define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区

//初始化磁盘
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	uint8_t res=0;
	switch(pdrv)
	{
		case SD_CARD://SD卡
			MX_SDIO_SD_Init();//SD卡初始化
  			break;
//		case EX_FLASH://外部flash
//			W25QXX_Init();
//			FLASH_SECTOR_COUNT=2048*12;//W25Q1218,前12M字节给FATFS占用
// 			break;
		default:
			res=1; 
	}		 

	return (DSTATUS)res; //初始化成功
}  

//获得磁盘状态
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	if(HAL_SD_GetState(&hsd) == HAL_SD_STATE_RESET)
		return 1;

	switch (HAL_SD_GetCardState(&hsd))
	{
		case HAL_SD_CARD_READY:
			return 0;
		case HAL_SD_CARD_DISCONNECTED:
			return 2;
		default:
			return 0;
	}
} 

//读扇区
//drv:磁盘编号0~9
//*buff:数据接收缓冲首地址
//sector:扇区地址
//count:需要读取的扇区数
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	uint8_t res=0;
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD://SD卡
#ifndef SD_DMA
			res=HAL_SD_ReadBlocks(&hsd,buff,(uint32_t)sector,count,500U);
			while(res)//读出错
			{

				if(SysStates.osstates>0)	vTaskDelay( pdMS_TO_TICKS(8) );
				else						HAL_Delay(8);
				res=HAL_SD_ReadBlocks(&hsd,buff,(uint32_t)sector,count,500U);

				if(hsd.ErrorCode&SDMMC_ERROR_CMD_RSP_TIMEOUT){
//					assert_failed("happen to error:SDMMC_ERROR_CMD_RSP_TIMEOUT\r\n",0);
					HAL_SD_DeInit(&hsd);
					if(SysStates.osstates>0)vTaskDelay( pdMS_TO_TICKS(500) );
					else					HAL_Delay(500);
					MX_SDIO_SD_Init();
					res=HAL_SD_ReadBlocks(&hsd,(uint8_t *)buff,(uint32_t)sector,count,500U);
				}
//				else if(hsd.ErrorCode&SDMMC_ERROR_ILLEGAL_CMD){
//
//				}
			}
#else
			SD_RWState|=1;
			if(HAL_SD_ReadBlocks_DMA(&hsd,buff,(uint32_t)sector,count)!=HAL_OK){
				assert_failed("SD_DMA config fail\r\n",0);
				res=HAL_SD_ReadBlocks(&hsd,buff,(uint32_t)sector,count,5000U);
				while(res){
					if(SysStates.osstates>0)	vTaskDelay( pdMS_TO_TICKS(60) );
					else						HAL_Delay(60);
					res=HAL_SD_ReadBlocks(&hsd,buff,(uint32_t)sector,count,5000U);
				}
			}else{
				if(SysStates.osstates>0)	vTaskDelay( pdMS_TO_TICKS(60) );
				else						HAL_Delay(60);
				while(HAL_SD_GetState(&hsd)>1){
					if(SysStates.osstates>0)	vTaskDelay( pdMS_TO_TICKS(60) );
					else						HAL_Delay(60);
					printf("SD get state:%d\r\n",HAL_SD_GetState(&hsd));
				}
			}


#endif
			break;
//		case EX_FLASH://外部flash
//			for(;count>0;count--)
//			{
//				W25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
//				sector++;
//				buff+=FLASH_SECTOR_SIZE;
//			}
//			res=0;
//			break;
		default:
			res=1; 
	}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}

//写扇区
//drv:磁盘编号0~9
//*buff:发送数据首地址
//sector:扇区地址
//count:需要写入的扇区数
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	uint8_t res=0;
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD://SD卡
#ifndef SD_DMA
			res=HAL_SD_WriteBlocks(&hsd,(uint8_t *)buff,(uint32_t)sector,count,500U);
			while(res)//写出错
			{
				if(SysStates.osstates>0)
					vTaskDelay( pdMS_TO_TICKS(8) );
				else
					HAL_Delay(8);
				res=HAL_SD_WriteBlocks(&hsd,(uint8_t *)buff,(uint32_t)sector,count,500U);

				if(hsd.ErrorCode&SDMMC_ERROR_CMD_RSP_TIMEOUT){
//					assert_failed("happen to error:SDMMC_ERROR_CMD_RSP_TIMEOUT\r\n",0);
					HAL_SD_DeInit(&hsd);
					if(SysStates.osstates>0)vTaskDelay( pdMS_TO_TICKS(500) );
					else					HAL_Delay(500);
					MX_SDIO_SD_Init();
					res=HAL_SD_WriteBlocks(&hsd,(uint8_t *)buff,(uint32_t)sector,count,500U);
				}
//				else if(hsd.ErrorCode&SDMMC_ERROR_ILLEGAL_CMD){
//
//				}
			}
#else
			SD_RWState|=2;
			if(HAL_SD_WriteBlocks_DMA(&hsd,(uint8_t *)buff,(uint32_t)sector,count)!=HAL_OK){
				assert_failed("SD_DMA config fail\r\n",0);
				res=HAL_SD_WriteBlocks(&hsd,(uint8_t *)buff,(uint32_t)sector,count,5000U);
				while(res){
					if(SysStates.osstates>0)	vTaskDelay( pdMS_TO_TICKS(60) );
					else						HAL_Delay(60);
					res=HAL_SD_WriteBlocks(&hsd,(uint8_t *)buff,(uint32_t)sector,count,5000U);
				}
			}else{
				if(SysStates.osstates>0)	vTaskDelay( pdMS_TO_TICKS(60) );
				else						HAL_Delay(60);
				while(HAL_SD_GetState(&hsd)>1){
					if(SysStates.osstates>0)	vTaskDelay( pdMS_TO_TICKS(60) );
					else						HAL_Delay(60);
					printf("SD get state:%d\r\n",HAL_SD_GetState(&hsd));
				}
			}
#endif
			break;
//		case EX_FLASH://外部flash
//			for(;count>0;count--)
//			{
//				W25QXX_Write((uint8_t*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
//				sector++;
//				buff+=FLASH_SECTOR_SIZE;
//			}
//			res=0;
//			break;
		default:
			res=1; 
	}
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
}
#endif



/*
 * Block是文件系统上的概念，一般文件系统block大小为4K。

Sector是磁介质硬盘最小单元，一般为512字节。
硬盘的最小存储单位就是扇区了，而且硬盘本身并没有block的概念。
文件系统不是一个扇区一个扇区的来读数据，太慢了，所以有了block（块）的概念，它是一个块一个块的读取的，block才是文件存取的最小单位。
 * */

//其他表参数的获得
 //drv:磁盘编号0~9
 //ctrl:控制代码
 //*buff:发送/接收缓冲区指针
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	HAL_SD_CardInfoTypeDef hSDcardInfo;
	if(pdrv==SD_CARD)//SD卡
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_COUNT:
		    	HAL_SD_GetCardInfo(&hsd,&hSDcardInfo);
		        *(DWORD*)buff = hSDcardInfo.BlockNbr;
		        res = RES_OK;
		        break;
		    case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		    	HAL_SD_GetCardInfo(&hsd,&hSDcardInfo);
				*(WORD*)buff = hSDcardInfo.BlockSize;
		        res = RES_OK;
		        break;	 
		    case CTRL_ERASE_SECTOR:
		    	res = RES_OK;
		    	break;

		    default:
		        res = RES_PARERR;
		        break;
	    }
	}
//	else if(pdrv==EX_FLASH)	//外部FLASH
//	{
//	    switch(cmd)
//	    {
//		    case CTRL_SYNC:
//				res = RES_OK;
//		        break;
//		    case GET_SECTOR_SIZE:
//		        *(WORD*)buff = FLASH_SECTOR_SIZE;
//		        res = RES_OK;
//		        break;
//		    case GET_BLOCK_SIZE:
//		        *(WORD*)buff = FLASH_BLOCK_SIZE;
//		        res = RES_OK;
//		        break;
//		    case GET_SECTOR_COUNT:
//		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
//		        res = RES_OK;
//		        break;
//		    default:
//		        res = RES_PARERR;
//		        break;
//	    }
//	}
	else res=RES_ERROR;//其他的不支持
    return res;
}
#endif
//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 
//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)malloc(size);
}
//释放内存
void ff_memfree (void* mf)		 
{
	free(mf);
}

















