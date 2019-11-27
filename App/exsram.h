/*
 * exsram.h
 *
 *  Created on: 2019年8月5日
 *      Author: admin
 */

#ifndef APP_EXSRAM_H_
#define APP_EXSRAM_H_

#include "stdint.h"


//一页实际大小,可用为2000

#define FGPIMAGE_MAXSIZE	32*1024

#define EXSRAM_STARTADDR	(0x68000000)
#define EXSRAM_LENGTH		(1024*1024)
#define EXSRAM_ENDADDR		(EXSRAM_STARTADDR+EXSRAM_LENGTH)

#define EXSRAM_PAGESIZE		(2*1024)			//2K存取
#define EXSRAM_PAGENUM		(EXSRAM_LENGTH/EXSRAM_PAGESIZE)

#define MAX_TVCCSAVE_LEN		(4*1024)	//目前要求tvcc长度与mvcc长度相等
#define MAX_MVCCSAVE_LEN		(4*1024)
#define MAX_RESULT_LEN			(8*1024)
#define MAX_FGPIMAGE_LEN		(32*1024)
#define MAX_ZOOMIN_LEN			(4*MAX_FGPIMAGE_LEN)
//#define ONE_TEST_MALLOC_SIZE	(MAX_TVCCSAVE_LEN+MAX_MVCCSAVE_LEN+MAX_RESULT_LEN+MAX_FGPIMAGE_LEN+MAX_ZOOMIN_LEN)


//后面可以请求添加专用的测试flash指令，加快速速。
extern const uint8_t STDFEATRUE[8192+1];			//对外输出一个标准的特征值,多最后多加一个校验值


typedef struct{
	uint32_t availablepagenum;
	uint8_t exsramtable[EXSRAM_PAGENUM];			//0 = 可用   >0 不可用
}xEXSRAM_CB;


#define INTTYPE		1
#define FLOATTYPE	2

typedef struct{
	void *datastart;		//def = null
	void *nowaddr;
	int32_t len;			//def = 0
	int32_t dattype;		//def = 0 标记数据记录者
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
	uint8_t proportion;		//1= 1:1 不需要开辟额外的放大区域
}xrecordparas;

//还需要加入临界保护！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！，涉及到全局结构体变量xEXSRAM_CB
//！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
void *getpages(uint32_t requestsize);		//单位为字节数！！，使用时要乘以size
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
