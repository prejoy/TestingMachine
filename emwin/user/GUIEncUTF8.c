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



static U16 _GetCharCode(const char GUI_UNI_PTR * s)
{
	U16 r;
	U8 ch = *(const U8*)s;
	if((ch & 0x80) == 0)
	{
		r = ch;
	}
	else if((ch & 0xe0) == 0xc0)
	{
		r = (ch & 0x1f) << 6;
		ch = *(++s) & 0x3f;
		r |= ch;
	}
	else if ((ch & 0xf0) == 0xe0)
	{
		r = (ch & 0x0f) << 12;
		ch = *(++s) & 0x3f;
		r |= (ch << 6);
		ch = *(++s) & 0x3f;
		r |= ch;
	}
	else
	{
		printf("Illegal character during UTF-8 decoding(01)!\n");
		r = 1;
	}
	return r;
}


static int _GetCharSize(const char GUI_UNI_PTR * s)
{
	U8 ch = *s;
	if((ch & 0x80) == 0) return 1;
	else if ((ch & 0xe0) == 0xc0) return 2;
	else if ((ch & 0xf0) == 0xe0) return 3;

	printf("Illegal character during UTF-8 decoding(02)!\n");
	return 1;
}


static int _ConvertUTF82UNI(const char GUI_UNI_PTR * s, int Len, U16 * pBuffer, int BufferSize)
{
	int LenDest = 0;
	while(Len > 0)
	{
		int num;
		num = _GetCharSize(s); Len -= 1;
		*(pBuffer++) = _GetCharCode(s);
		s += num;
		LenDest++;
		if (LenDest >= BufferSize) break;
	}
	return LenDest;
}













static int _GetLineDistX(const char GUI_UNI_PTR *s, int Len)
{
	int Dist = 0;
	while((*(const U8*)s != 0) && Len > 0)
	{
		int num;
		num = _GetCharSize(s); Len -= 1;
		Dist += GUI_GetCharDistX(_GetCharCode(s));
		s += num;
	}
	return Dist;
}

static int _GetLineLen(const char GUI_UNI_PTR *s, int MaxLen)
{
	int Len =0;
	while((*(const U8*)s != 0) && MaxLen > 0)
	{
		int num;
		num = _GetCharSize(s); MaxLen -= 1;
		if(num==1) if(*s == '\n') return Len;
		Len += 1;
		s += num;
	}
	return Len;
}

static const GUI_FONT_PROP_GL GUI_UNI_PTR * _FindChar(const GUI_FONT_PROP_GL GUI_UNI_PTR *pProp, U16P c)
{
	for (; pProp; pProp = pProp->pNext) if ((c>=pProp->First) && (c<=pProp->Last)) break;
	return pProp;
}

#define MAX_CHAR_LINE	(128)
static void _DispLine(const char GUI_UNI_PTR *s, int Len)
{
	U16 uniBUF[MAX_CHAR_LINE], uniNUM; U16 *puni = uniBUF;
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

	
	uniNUM = _ConvertUTF82UNI(s, Len, uniBUF, MAX_CHAR_LINE);
	for(; uniNUM>0; uniNUM--, puni++)
	{
		pProp = _FindChar((const GUI_FONT_PROP_GL GUI_UNI_PTR *)pAFont->p.pProp, *puni);
		if(!pProp) {GLCD_WCMD_END(0); goto exit;}
		GLCD_WCMD_PAR(*puni>>8); GLCD_WCMD_PAR(*puni);
		sizeX += (pAFont->Encode&16) ? pProp->pXsize[*puni-pProp->First] : pProp->pXsize[0];
	}
	GLCD_WCMD_END(0);




	exit:


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
		GUI_pContext->DispPosX += /*pCharInfo*/sizeX * ((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->XMag;
	}
}

const tGUI_ENC_APIList GUI_ENC_APIList_UTF8 = {
	_GetLineDistX,
	_GetLineLen,
	_DispLine
};


	 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 




