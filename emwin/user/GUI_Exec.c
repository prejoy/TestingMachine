//=============================================================================
//             _   __    ______    ____       ______    ______
//            / | / /   / ____/   / __ \     / ____/   / ____/
//           /  |/ /   / __/     / / / /    / __/     / __/
//          / /|  /   / /___    / /_/ /    / /___    / /___
//         /_/ |_/   /_____/    \___\_\   /_____/   /_____/
//        (c)1996-2016 NEQEE Photoelectric Technology Co., Ltd.
//        Internet: www.neqee.com  Designer: Chen ZeHao
//=============================================================================

#include "GUI_Private.h"
#if GUI_WINSUPPORT
#include "WM.h"
#include "WM_Intern.h"
#endif

#include "glcd.h"
int GUI_Exec1(void)
{
	int r = 0;
	int WM__NumInvalidWindowsP;
	GLCD_read_command();
	if(GUI_pfTimerExec) if((*GUI_pfTimerExec)()) r = 1;
	#if GUI_WINSUPPORT
	WM__NumInvalidWindowsP = WM__NumInvalidWindows;
	if(WM_Exec()) r = 1;
	if(WM__NumInvalidWindowsP&&!WM__NumInvalidWindows) r = 1;
	#endif
	if(r) GLCD_refresh_screen();
	return r;
}

int GUI_Exec(void)
{
	int r = 0;
	while(GUI_Exec1()) r = 1;
	return r;
}



