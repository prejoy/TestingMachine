//=============================================================================
//             _   __    ______    ____       ______    ______
//            / | / /   / ____/   / __ \     / ____/   / ____/
//           /  |/ /   / __/     / / / /    / __/     / __/
//          / /|  /   / /___    / /_/ /    / /___    / /___
//         /_/ |_/   /_____/    \___\_\   /_____/   /_____/
//        (c)1996-2016 NEQEE Photoelectric Technology Co., Ltd.
//        Internet: www.neqee.com  Designer: Chen ZeHao
//=============================================================================

#include <stddef.h>
#include "GUI_Private.h"
#include "glcd.h"


static U16 _2BIG5(U8 c0, U8 c1) {return c1|(((U16)c0)<<8);}

static int _GetLineDistX(const char GUI_UNI_PTR *s, int Len)
{
	int Dist =0; U8 c0;
	if(!s) return 0;
	while(((c0=*(const U8*)s) != 0) && Len > 0)
	{
		s++; Len--;
		if (c0 > 127)
		{
			U8  c1 = *(const U8*)s++;
			Dist += GUI_GetCharDistX(_2BIG5(c0, c1));
		}
		else Dist += GUI_GetCharDistX(c0);
	}
	return Dist;
}

static int _GetLineLen(const char GUI_UNI_PTR *s, int MaxLen)
{
	int Len =0;
	U8 c0;
	while(((c0=*(const U8*)s) != 0) && Len < MaxLen)
	{
		s++;
		if(c0 > 127) {s++;}
		else {if(c0 == '\n') return Len;}
		Len++;
	}
	return Len;
}

static const GUI_FONT_PROP_GL GUI_UNI_PTR * _FindChar(const GUI_FONT_PROP_GL GUI_UNI_PTR *pProp, U16P c)
{
	for (; pProp; pProp = pProp->pNext) if ((c>=pProp->First) && (c<=pProp->Last)) break;
	return pProp;
}


static void _DispLine(const char GUI_UNI_PTR *s, int Len)
{
	U8 c0, c1;
	U16 sizeX = 0;
	GUI_DRAWMODE DrawMode; GUI_DRAWMODE OldDrawMode;
	LCD_PIXELINDEX ColorIndex, BkColorIndex;
	short dispX0, dispY0, clipX0, clipY0, clipX1, clipY1;
	const GUI_FONT_GL GUI_UNI_PTR *pAFont;
	const GUI_FONT_PROP_GL GUI_UNI_PTR *pProp;

	DrawMode = GUI_pContext->TextMode;
	OldDrawMode  = LCD_SetDrawMode(DrawMode);
	ColorIndex = LCD__GetColorIndex();
	BkColorIndex = LCD__GetBkColorIndex();
	clipX0 = GUI_pContext->ClipRect.x0;
	clipY0 = GUI_pContext->ClipRect.y0;
	clipX1 = GUI_pContext->ClipRect.x1;
	clipY1 = GUI_pContext->ClipRect.y1;
	dispX0 = GUI_pContext->DispPosX;
	dispY0 = GUI_pContext->DispPosY;
	pAFont = (const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont;

	GLCD_bfcolor_set(0, ColorIndex&0xffffff, BkColorIndex&0xffffff);
	GLCD_fontlib_set(0, pAFont->Encode, pAFont->YSize, (const char *)pAFont->pFont);
	GLCD_WCMD_STA(0x45);
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
	GLCD_WCMD_PAR(DrawMode==LCD_DRAWMODE_TRANS);

	while(--Len >= 0)
	{
		c0 = *(const U8*)s++;
		if(c0 > 127)
		{
			c1 = *(const U8*)s++;
			pProp = _FindChar((const GUI_FONT_PROP_GL GUI_UNI_PTR *)pAFont->p.pProp, _2BIG5(c0, c1));
			if(!pProp) {GLCD_WCMD_END(0); goto EXIT;}
			GLCD_WCMD_PAR(_2BIG5(c0, c1)>>8); GLCD_WCMD_PAR(_2BIG5(c0, c1));
			sizeX += (pAFont->Encode&16) ? pProp->pXsize[_2BIG5(c0, c1)-pProp->First] : pProp->pXsize[0];
		}
		else
		{
			pProp = _FindChar((const GUI_FONT_PROP_GL GUI_UNI_PTR *)pAFont->p.pProp, c0);
			if(!pProp) {GLCD_WCMD_END(0); goto EXIT;}
			GLCD_WCMD_PAR(c0);
			sizeX += (pAFont->Encode&16) ? pProp->pXsize[c0-pProp->First] : pProp->pXsize[0];
		}
	}
	GLCD_WCMD_END(0);











	EXIT:


	// Fill empty pixel lines
	if (((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->YDist > ((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->YSize)
	{
		int YMag = ((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->YMag;
		int YDist = ((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->YDist * YMag;
		int YSize = ((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->YSize * YMag;
		if (DrawMode != LCD_DRAWMODE_TRANS)
		{
			LCD_COLOR OldColor = GUI_GetColor();
			GUI_SetColor(GUI_GetBkColor());
			LCD_FillRect(GUI_pContext->DispPosX, GUI_pContext->DispPosY + YSize, GUI_pContext->DispPosX + sizeX, GUI_pContext->DispPosY + YDist);
			GUI_SetColor(OldColor);
		}
	}
	LCD_SetDrawMode(OldDrawMode); // Restore draw mode
	//if (!GUI_MoveRTL)
	{
		GUI_pContext->DispPosX += sizeX * ((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->XMag;
	}
}

const tGUI_ENC_APIList GUI_ENC_APIList_BIG5 = {
	_GetLineDistX,
	_GetLineLen,
	_DispLine
};


	 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 




