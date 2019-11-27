#include "glcd.h"


void CMDA_rtc_display(unsigned int color, unsigned short x0, unsigned short y0, unsigned char mode, unsigned char on)
{
	GLCD_WCMD_STA(0x58);
	GLCD_WCMD_PAR(color>>16);
	GLCD_WCMD_PAR(color>>8);
	GLCD_WCMD_PAR(color);
	GLCD_WCMD_PAR(x0>>8);
	GLCD_WCMD_PAR(x0);
	GLCD_WCMD_PAR(y0>>8);
	GLCD_WCMD_PAR(y0);
	GLCD_WCMD_PAR(mode);
	GLCD_WCMD_END(on);
}
void CMDA_rtc_read(void)
{
	GLCD_WCMD_STA(0x59);
	GLCD_WCMD_END(0);
}
void CMDA_rtc_setting(unsigned char *rtc)
{
	GLCD_WCMD_STA(0x5a);
	GLCD_WCMD_PAR(0x55);
	GLCD_WCMD_PAR(0xaa);
	GLCD_WCMD_PAR(0x5a);
	GLCD_WCMD_PAR(0xa5);
	GLCD_WCMD_PAR(rtc[0]);	//second
	GLCD_WCMD_PAR(rtc[1]);	//minute
	GLCD_WCMD_PAR(rtc[2]);	//hour
	GLCD_WCMD_PAR(rtc[3]);	//dat
	GLCD_WCMD_PAR(rtc[4]);	//week
	GLCD_WCMD_PAR(rtc[5]);	//month
	GLCD_WCMD_END(rtc[6]);	//year
}


void CMDA_buzzer_beep(unsigned short val/*0~4095*/)
{
	GLCD_WCMD_STA(0xd0);
	GLCD_WCMD_PAR(val>>8);
	GLCD_WCMD_END(val);
}


void CMDA_backlight_setting(unsigned char val/*0~15*/)
{
	GLCD_WCMD_STA(0xd1);
	GLCD_WCMD_END(val&0xf);
}
