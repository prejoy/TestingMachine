#ifndef __CMD_A_H_
#define __CMD_A_H_

void CMDA_rtc_display(unsigned int color, unsigned short x0, unsigned short y0, unsigned char mode, unsigned char on);
void CMDA_rtc_read(void);
void CMDA_rtc_setting(unsigned char *rtc);
void CMDA_buzzer_beep(unsigned short val);
void CMDA_backlight_setting(unsigned char val);

#endif


