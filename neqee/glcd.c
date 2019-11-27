//=============================================================================
//             _   __    ______    ____       ______    ______
//            / | / /   / ____/   / __ \     / ____/   / ____/
//           /  |/ /   / __/     / / / /    / __/     / __/
//          / /|  /   / /___    / /_/ /    / /___    / /___
//         /_/ |_/   /_____/    \___\_\   /_____/   /_____/
//        (c)1996-2016 NEQEE Photoelectric Technology Co., Ltd.
//        Internet: www.neqee.com  Designer: Chen ZeHao
//=============================================================================

#include <string.h>
#include "gui.h"
#include "glcd.h"



#define		GLCD_FRAME_MAXL		(64)			//max lenth for one frame
static unsigned char GLCD_read_frame(unsigned char **buff, unsigned short *len)
{
	static unsigned char frame_buff[GLCD_FRAME_MAXL];
	static unsigned short frame_len = 0;
	unsigned short dat_t;
	
	BEGIN:
	
	if(!((dat_t=GLCD_RCMD())&0x400)) return 0;	//nothing
	
	if(frame_len>(GLCD_FRAME_MAXL-1)) {frame_len = 0; goto ERROR;}
	if(!frame_len && !(dat_t&0x200)) {frame_len = 0; goto ERROR;}
	if(frame_len && (dat_t&0x200)) {frame_len = 0; frame_buff[frame_len] = dat_t; frame_len++; goto ERROR;}
	
	frame_buff[frame_len] = dat_t; frame_len++;
	if(!(dat_t&0x100)) goto BEGIN;				//frame not end
	
	*len = frame_len; *buff = &frame_buff[0]; frame_len = 0;
	return 1;									//get one frame

	ERROR:										//add error handle code here

	return 0;
}

unsigned char GLCD_readRTC[7] = {0,0,0,0,0,0,0};//note: prevent conflict between threads when used RTOS!!!
void GLCD_read_command(void)
{
	unsigned short frame_len = 0;
	unsigned char *frame_buff;
	
	if(!GLCD_read_frame(&frame_buff, &frame_len)) return;
	
	switch(frame_buff[0])
	{
	case 0x20:									//ps2-keyboard or matrix-keyboard
		if((frame_buff[1]==1) || (frame_buff[1]==2))
		{
			GUI_StoreKeyMsg(frame_buff[2], 1);
			CMDA_buzzer_beep(50);
		}
		else if(frame_buff[1]==3) GUI_StoreKeyMsg(frame_buff[2], 0);
		//printf("K-%03d S-%d\n", frame_buff[2], frame_buff[1]);
		break;
	case 0x21:									//ps2-mouse or touch-pannel
		if((frame_buff[1]==1) || (frame_buff[1]==2))
			GUI_TOUCH_StoreState(((unsigned short)frame_buff[3]<<8)+frame_buff[4], ((unsigned short)frame_buff[5]<<8)+frame_buff[6]);
		else if(frame_buff[1]==3) GUI_TOUCH_StoreState(-1,-1);
		//printf("K-%d S-%d X-%04d Y-%04d\n", frame_buff[2], frame_buff[1], (frame_buff[3]<<8)+frame_buff[4], (frame_buff[5]<<8)+frame_buff[6]);
		break;
	case 0x59:									//RTC
		GLCD_readRTC[0] = frame_buff[1];		//second
		GLCD_readRTC[1] = frame_buff[2];		//minute
		GLCD_readRTC[2] = frame_buff[3];		//hour
		GLCD_readRTC[3] = frame_buff[4];		//dat
		GLCD_readRTC[4] = frame_buff[5];		//week
		GLCD_readRTC[5] = frame_buff[6];		//month
		GLCD_readRTC[6] = frame_buff[7];		//year
		//printf("20%02d/%02d/%02d %02d:%02d:%02d [%d]\n", GLCD_readRTC[6], GLCD_readRTC[5], GLCD_readRTC[3], GLCD_readRTC[2], GLCD_readRTC[1], GLCD_readRTC[0], GLCD_readRTC[4]);
		break;
	default:
		break;
	}
}











void GLCD_wait_power_on(void)
{
	while(!GLCD_RSTA_IDONE());	//wait idone = 1;
	while(GLCD_RSTA_IDONE());	//wait idone = 0;
	while(!GLCD_RSTA_IDONE());	//wait idone = 1;
	while(GLCD_RSTA_IDONE());	//wait idone = 0;
	while(!GLCD_RSTA_IDONE());	//wait idone = 1;
	while(GLCD_RSTA_IDONE());	//wait idone = 0;
	while(!GLCD_RSTA_IDONE());	//wait idone = 1;
	while(GLCD_RSTA_IDONE());	//wait idone = 0;
	while(!GLCD_RSTA_IDONE());	//wait idone = 1;
	while(GLCD_RSTA_IDONE());	//wait idone = 0;
}

void GLCD_system_reset(void)
{
	GLCD_WCMD_STA(0x05);
	GLCD_WCMD_END(0x00);
}


void GLCD_bfcolor_set(unsigned char init, int fcolor, int bcolor)
{
	static int cl_front, cl_back;
	if(!init && (cl_front==fcolor) && (cl_back==bcolor)) return;
	cl_front = fcolor; cl_back = bcolor;
	GLCD_WCMD_STA(0x10);
	GLCD_WCMD_PAR(cl_front>>16);
	GLCD_WCMD_PAR(cl_front>>8);
	GLCD_WCMD_PAR(cl_front);
	GLCD_WCMD_PAR(cl_back>>16);
	GLCD_WCMD_PAR(cl_back>>8);
	GLCD_WCMD_END(cl_back);
}


void GLCD_trcolor_set(unsigned char init, int tcolor)
{
	static int cl_trans;
	if(!init && (cl_trans==tcolor)) return;
	cl_trans = tcolor;
	GLCD_WCMD_STA(0x11);
	GLCD_WCMD_PAR(cl_trans>>16);
	GLCD_WCMD_PAR(cl_trans>>8);
	GLCD_WCMD_END(cl_trans);
}


void GLCD_fontlib_set(unsigned char init, unsigned char encode, unsigned char fontH, const char *pfont)
{
	static unsigned char fn_encode, fn_height; static char fn_font[8];
	if(!init && (fn_encode==encode) && (fn_height==fontH) && (fn_font[0]==pfont[0]) && (fn_font[1]==pfont[1]) && (fn_font[2]==pfont[2]) && (fn_font[3]==pfont[3]) && (fn_font[4]==pfont[4])) return;
	fn_encode = encode; fn_height = fontH;
	fn_font[0]=pfont[0]; fn_font[1]=pfont[1]; fn_font[2]=pfont[2]; fn_font[3]=pfont[3];
	fn_font[4]=pfont[4]; fn_font[5]=pfont[5]; fn_font[6]=pfont[6]; fn_font[7]=0;
	GLCD_WCMD_STA(0x42);
	GLCD_WCMD_PAR(fn_encode);
	GLCD_WCMD_PAR(fn_height);
	GLCD_WCMD_PAR(fn_font[0]);
	GLCD_WCMD_PAR(fn_font[1]);
	GLCD_WCMD_PAR(fn_font[2]);
	GLCD_WCMD_PAR(fn_font[3]);
	GLCD_WCMD_PAR(fn_font[4]);
	GLCD_WCMD_PAR(fn_font[5]);
	GLCD_WCMD_PAR(fn_font[6]);
	GLCD_WCMD_END(fn_font[7]);
}




void GLCD_refresh_auto(unsigned char en, unsigned char time)	//enable auto refresh
{
	GLCD_WCMD_STA(0xe5);
	GLCD_WCMD_PAR(en);
	GLCD_WCMD_END(time);				//auto refresh time = 10ms*time; not change if time<5 or time>30
}


void GLCD_refresh_screen(void)			//display refresh
{
	GLCD_WCMD_STA(0xe1);
	GLCD_WCMD_END(0x00);
}









