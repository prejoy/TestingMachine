#include "GUI_Private.h"
#include "glcd.h"

void GL_FillPolygon  (const GUI_POINT*paPoint, int NumPoints, int xOff, int yOff)
{
	LCD_PIXELINDEX ColorIndex, BkColorIndex;
	short dispX0, dispY0, clipX0, clipY0, clipX1, clipY1;
	unsigned char i;

	ColorIndex = LCD__GetColorIndex();
	BkColorIndex = LCD__GetBkColorIndex();
	clipX0 = GUI_pContext->ClipRect.x0;
	clipY0 = GUI_pContext->ClipRect.y0;
	clipX1 = GUI_pContext->ClipRect.x1;
	clipY1 = GUI_pContext->ClipRect.y1;
	dispX0 = xOff;//dispX0 = GUI_pContext->DispPosX;
	dispY0 = yOff;//dispY0 = GUI_pContext->DispPosY;

	
	
	
	GLCD_bfcolor_set(0, ColorIndex&0xffffff, BkColorIndex&0xffffff);
	GLCD_WCMD_STA(0x38);
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
	GLCD_WCMD_PAR(NumPoints);
	for(i=0; i<NumPoints; i++)
	{
		GLCD_WCMD_PAR(paPoint[i].x>>8);
		GLCD_WCMD_PAR(paPoint[i].x);
		GLCD_WCMD_PAR(paPoint[i].y>>8);
		GLCD_WCMD_PAR(paPoint[i].y);
	}
	GLCD_WCMD_END(0);
}


void GUI_FillPolygon(const GUI_POINT* pPoints, int NumPoints, int x0, int y0)
{
	GUI_LOCK();
	#if (GUI_WINSUPPORT)
	WM_ADDORG(x0, y0);
	WM_ITERATE_START(NULL); {
	#endif
	GL_FillPolygon (pPoints, NumPoints, x0, y0);
	#if (GUI_WINSUPPORT)
	} WM_ITERATE_END();
	#endif
	GUI_UNLOCK();
}

/*************************** End of file ****************************/
