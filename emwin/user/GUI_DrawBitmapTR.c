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
#include "glcd.h"


#define BITMAP_TRANS_COLOR 0xd0f8d0		//R8G8B8=0xd2fcd2 -> R5G6B5=0xd0f8d0

static void _DrawBitmap(int x0, int y0, int xsize, int ysize, const U8 * pPixel, const LCD_LOGPALETTE * pLogPal, int xMag, int yMag)
{
	U8 *p_path;
	short dispX0, dispY0, clipX0, clipY0, clipX1, clipY1;

	
	p_path = (unsigned char *)pPixel;
	clipX0 = GUI_pContext->ClipRect.x0 - x0; clipX0 = (clipX0<0) ? 0 : clipX0;
	clipY0 = GUI_pContext->ClipRect.y0 - y0; clipY0 = (clipY0<0) ? 0 : clipY0;
	clipX1 = (GUI_pContext->ClipRect.x1 - x0 + 1); if(clipX1<=0) return;
	clipY1 = (GUI_pContext->ClipRect.y1 - y0 + 1); if(clipY1<=0) return;
	dispX0 = x0 + clipX0;
	dispY0 = y0 + clipY0;

	GLCD_trcolor_set(0, BITMAP_TRANS_COLOR&0xffffff);

	GLCD_WCMD_STA(0x53);
	GLCD_WCMD_PAR(dispX0>>8);
	GLCD_WCMD_PAR(dispX0);
	GLCD_WCMD_PAR(dispY0>>8);
	GLCD_WCMD_PAR(dispY0);
	GLCD_WCMD_PAR(clipX0>>8);
	GLCD_WCMD_PAR(clipX0);
	GLCD_WCMD_PAR(clipY0>>8);
	GLCD_WCMD_PAR(clipY0);
	GLCD_WCMD_PAR(clipX1>>8);
	GLCD_WCMD_PAR(clipX1);
	GLCD_WCMD_PAR(clipY1>>8);
	GLCD_WCMD_PAR(clipY1);
	GLCD_WCMD_STR(p_path);
	GLCD_WCMD_END(0);
}


const GUI_BITMAP_METHODS GUI_BitmapMethodsTR =
{
	_DrawBitmap,
	LCD_Index2Color_M888,
	_DrawBitmap,
	GUICC_M888
};


