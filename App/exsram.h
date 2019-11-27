/*
 * exsram.h
 *
 *  Created on: 2019��8��5��
 *      Author: admin
 */

#ifndef APP_EXSRAM_H_
#define APP_EXSRAM_H_

#include "stdint.h"


//һҳʵ�ʴ�С,����Ϊ2000

#define FGPIMAGE_MAXSIZE	32*1024

#define EXSRAM_STARTADDR	(0x68000000)
#define EXSRAM_LENGTH		(1024*1024)
#define EXSRAM_ENDADDR		(EXSRAM_STARTADDR+EXSRAM_LENGTH)

#define EXSRAM_PAGESIZE		(2*1024)			//2K��ȡ
#define EXSRAM_PAGENUM		(EXSRAM_LENGTH/EXSRAM_PAGESIZE)

#define MAX_TVCCSAVE_LEN		(4*1024)	//ĿǰҪ��tvcc������mvcc�������
#define MAX_MVCCSAVE_LEN		(4*1024)
#define MAX_RESULT_LEN			(8*1024)
#define MAX_FGPIMAGE_LEN		(32*1024)
#define MAX_ZOOMIN_LEN			(4*MAX_FGPIMAGE_LEN)
//#define ONE_TEST_MALLOC_SIZE	(MAX_TVCCSAVE_LEN+MAX_MVCCSAVE_LEN+MAX_RESULT_LEN+MAX_FGPIMAGE_LEN+MAX_ZOOMIN_LEN)


//��������������ר�õĲ���flashָ��ӿ����١�
extern const uint8_t STDFEATRUE[8192+1];			//�������һ����׼������ֵ,�������һ��У��ֵ


typedef struct{
	uint32_t availablepagenum;
	uint8_t exsramtable[EXSRAM_PAGENUM];			//0 = ����   >0 ������
}xEXSRAM_CB;


#define INTTYPE		1
#define FLOATTYPE	2

typedef struct{
	void *datastart;		//def = null
	void *nowaddr;
	int32_t len;			//def = 0
	int32_t dattype;		//def = 0 ������ݼ�¼��
}xExSRAMInstance;


typedef struct {
	void *resultstart;
//	uint32_t len;
}xResSav;

typedef struct{
	void *oriImgaddr;
	void *zoominImgaddr;
	uint8_t width;
	uint8_t height;
	uint8_t proportion;
}xImgSav;

typedef struct{
	xExSRAMInstance hTVccSaves;
	xExSRAMInstance hMVccSaves;
	xResSav	        hResultSaves;
	xImgSav			hImgSaves;
	uint32_t 		pages;
}xAllTestContents;

typedef struct {
	uint8_t proportion;		//1= 1:1 ����Ҫ���ٶ���ķŴ�����
}xrecordparas;

//����Ҫ�����ٽ籣�����������������������������������������������������������������������漰��ȫ�ֽṹ�����xEXSRAM_CB
//����������������������������������������������������������������������������������
void *getpages(uint32_t requestsize);		//��λΪ�ֽ���������ʹ��ʱҪ����size
void releasepages(xExSRAMInstance *pWriteInstance);
void *addonepage(void *addrcontinue);

int32_t exsram_write(xExSRAMInstance *pWriteInstance,void *dat,int32_t len);
int32_t exsram_read(xExSRAMInstance *pWriteInstance,void *dat,int32_t len);		//no finish
int32_t exsram_release(xExSRAMInstance *pWriteInstance);


void *getcontpages(uint32_t requestsize,uint32_t *getpagenum);
void releasecontpages(void *staddr,uint32_t pagenum);

//====================================              new version                     ====================================
void *getcontinouspages(int32_t pagenum);
void releasecontinouspages(void *addr,int32_t pagenum);
void *usermalloc(int32_t size,uint32_t *pages);
void userfree(void *addr,uint32_t pages);
xAllTestContents *createrecord(xrecordparas *settings);
void releaserecord(xAllTestContents *record);

int32_t power_write(xExSRAMInstance *pWriteInstance,void *dat,int32_t len);
int32_t power_read(xExSRAMInstance *pWriteInstance,void *dat,int32_t len);
int32_t power_clear(xExSRAMInstance *pWriteInstance,void *dat,int32_t len);


#endif /* APP_EXSRAM_H_ */
