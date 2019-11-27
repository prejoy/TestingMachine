//=============================================================================
//             _   __    ______    ____       ______    ______
//            / | / /   / ____/   / __ \     / ____/   / ____/
//           /  |/ /   / __/     / / / /    / __/     / __/
//          / /|  /   / /___    / /_/ /    / /___    / /___
//         /_/ |_/   /_____/    \___\_\   /_____/   /_____/
//        (c)1996-2016 NEQEE Photoelectric Technology Co., Ltd.
//        Internet: www.neqee.com  Designer: Chen ZeHao
//=============================================================================

#ifndef BITMAP_H
#define BITMAP_H

#include "GUI.h"

extern const GUI_BITMAP bmfgp;
extern const GUI_BITMAP bmNG;
extern const GUI_BITMAP bmPASS;
extern const GUI_BITMAP bmtester;
extern const GUI_BITMAP bmwifi;

WM_HWIN CreateUI_Config(void);
WM_HWIN CreateUI_ActModTest(void);


#endif
