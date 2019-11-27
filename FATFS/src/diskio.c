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

#define SD_CARD	 0  //SD��,���Ϊ0
//#define EX_FLASH 1	//�ⲿflash,���Ϊ1

#define FLASH_SECTOR_SIZE 	512			  
//����W25Q128
//ǰ12M�ֽڸ�fatfs��,12M�ֽں�,���ڴ���ֿ�,�ֿ�ռ��3.09M.	ʣ�ಿ��,���ͻ��Լ���	 			    
//u16	    FLASH_SECTOR_COUNT=2048*12;	//W25Q1218,ǰ12M�ֽڸ�FATFSռ��
#define FLASH_BLOCK_SIZE   	8     	//ÿ��BLOCK��8������

//��ʼ������
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	uint8_t res=0;
	switch(pdrv)
	{
		case SD_CARD://SD��
			MX_SDIO_SD_Init();//SD����ʼ��
  			break;
//		case EX_FLASH://�ⲿflash
//			W25QXX_Init();
//			FLASH_SECTOR_COUNT=2048*12;//W25Q1218,ǰ12M�ֽڸ�FATFSռ��
// 			break;
		default:
			res=1; 
	}		 

	return (DSTATUS)res; //��ʼ���ɹ�
}  

//��ô���״̬
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

//������
//drv:���̱��0~9
//*buff:���ݽ��ջ����׵�ַ
//sector:������ַ
//count:��Ҫ��ȡ��������
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	uint8_t res=0;
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD://SD��
#ifndef SD_DMA
			res=HAL_SD_ReadBlocks(&hsd,buff,(uint32_t)sector,count,500U);
			while(res)//������
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
//		case EX_FLASH://�ⲿflash
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
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}

//д����
//drv:���̱��0~9
//*buff:���������׵�ַ
//sector:������ַ
//count:��Ҫд���������
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	uint8_t res=0;
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD://SD��
#ifndef SD_DMA
			res=HAL_SD_WriteBlocks(&hsd,(uint8_t *)buff,(uint32_t)sector,count,500U);
			while(res)//д����
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
//		case EX_FLASH://�ⲿflash
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
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
}
#endif



/*
 * Block���ļ�ϵͳ�ϵĸ��һ���ļ�ϵͳblock��СΪ4K��

Sector�ǴŽ���Ӳ����С��Ԫ��һ��Ϊ512�ֽڡ�
Ӳ�̵���С�洢��λ���������ˣ�����Ӳ�̱���û��block�ĸ��
�ļ�ϵͳ����һ������һ���������������ݣ�̫���ˣ���������block���飩�ĸ������һ����һ����Ķ�ȡ�ģ�block�����ļ���ȡ����С��λ��
 * */

//����������Ļ��
 //drv:���̱��0~9
 //ctrl:���ƴ���
 //*buff:����/���ջ�����ָ��
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	HAL_SD_CardInfoTypeDef hSDcardInfo;
	if(pdrv==SD_CARD)//SD��
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
//	else if(pdrv==EX_FLASH)	//�ⲿFLASH
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
	else res=RES_ERROR;//�����Ĳ�֧��
    return res;
}
#endif
//���ʱ��
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 
//��̬�����ڴ�
void *ff_memalloc (UINT size)			
{
	return (void*)malloc(size);
}
//�ͷ��ڴ�
void ff_memfree (void* mf)		 
{
	free(mf);
}

















