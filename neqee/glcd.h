//=============================================================================
//             _   __    ______    ____       ______    ______
//            / | / /   / ____/   / __ \     / ____/   / ____/
//           /  |/ /   / __/     / / / /    / __/     / __/
//          / /|  /   / /___    / /_/ /    / /___    / /___
//         /_/ |_/   /_____/    \___\_\   /_____/   /_____/
//        (c)1996-2016 NEQEE Photoelectric Technology Co., Ltd.
//        Internet: www.neqee.com  Designer: Chen ZeHao
//=============================================================================

#ifndef __GLCD_H_
#define __GLCD_H_
#include "spi.h"

#define	GLCD_send(s)				ScreenSPI_transfer(s)
#define	GLCD_read(s)				ScreenSPI_transfer(s)



#define GLCD_WDAT_BASE			(8<<12)
#define GLCD_WPOS_BASE			(10<<12)
#define GLCD_WCMD_BASE			(3<<12)
#define GLCD_WXOR_BASE			(2<<12)
#define GLCD_RCMD_BASE			(12<<12)
#define GLCD_RSTA_BASE			(13<<12)

#define	GLCD_RSTA()					GLCD_read(GLCD_RSTA_BASE)
#define	GLCD_RCMD()					GLCD_read(GLCD_RCMD_BASE)

#define	GLCD_RSTA_IDONE()		(GLCD_RSTA()&0x001)				//system init done(ready)
#define GLCD_RSTA_CFULL()		(GLCD_RSTA()&0x008)				//command buffer is full
#define GLCD_RSTA_CBUSY()		(GLCD_RSTA()&0x020)				//command is executing(busy)

#define	GLCD_WDAT(H, L)			{GLCD_send(GLCD_WDAT_BASE|((H)&0x7ff)); GLCD_send(GLCD_WDAT_BASE|(1<<12)|((L)&0xfff));}
#define	GLCD_WPYI(Y, X)			{GLCD_send(GLCD_WPOS_BASE|((Y)&0x3ff)); GLCD_send(GLCD_WPOS_BASE|(1<<12)|((X)&0xfff)|0x800);}
#define	GLCD_WPXI(Y, X)			{GLCD_send(GLCD_WPOS_BASE|((Y)&0x3ff)); GLCD_send(GLCD_WPOS_BASE|(1<<12)|((X)&0x7ff));}
#define	GLCD_WXOR()					{GLCD_send(GLCD_WXOR_BASE);}
#define	GLCD_WCMD(C)				{GLCD_send(GLCD_WCMD_BASE|((C)&0xfff));}

#define GLCD_WCMD_STA(P)		while(GLCD_RSTA_CFULL()); GLCD_WCMD(((P)&0xff)|0x200)	//Command start
#define GLCD_WCMD_PAR(P)		GLCD_WCMD(((P)&0xff)|0x000)								//Command parameter
#define GLCD_WCMD_END(P)		GLCD_WCMD(((P)&0xff)|0x100)								//Command end
#define GLCD_WCMD_STR(S)		while(*S) GLCD_WCMD_PAR(*S++)							//Command string



extern unsigned char GLCD_readRTC[7];

void GLCD_wait_power_on(void);
void GLCD_system_reset(void);
void GLCD_bfcolor_set(unsigned char init, int fcolor, int bcolor);
void GLCD_trcolor_set(unsigned char init, int tcolor);
void GLCD_fontlib_set(unsigned char init, unsigned char encode, unsigned char fontH, const char *pfont);
void GLCD_read_command(void);
void GLCD_refresh_auto(unsigned char en, unsigned char time);
void GLCD_refresh_screen(void);


#endif





