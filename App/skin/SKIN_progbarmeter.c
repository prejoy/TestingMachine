//=============================================================================
//             _   __    ______    ____       ______    ______
//            / | / /   / ____/   / __ \     / ____/   / ____/
//           /  |/ /   / __/     / / / /    / __/     / __/
//          / /|  /   / /___    / /_/ /    / /___    / /___
//         /_/ |_/   /_____/    \___\_\   /_____/   /_____/
//        (c)1996-2016 NEQEE Photoelectric Technology Co., Ltd.
//        Internet: www.neqee.com  Designer: Chen ZeHao
//=============================================================================

#include "DIALOG.h"
#include "gui.h"
#include "skin.h"
#include <stdlib.h>

//获取父窗口句柄,即主页面句柄,保存着页面位图的指针
static WM_HWIN WM_GetParent1C(WM_HWIN hChild) {return hChild;}
static WM_HWIN WM_GetParent2C(WM_HWIN hChild) {return WM_GetParent(hChild);}
static WM_HWIN WM_GetParent3C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(hChild));}
static WM_HWIN WM_GetParent4C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(hChild)));}
static WM_HWIN WM_GetParent5C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))));}
static WM_HWIN WM_GetParent6C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))));}
static WM_HWIN WM_GetParent7C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))))));}
static WM_HWIN WM_GetParent8C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))))));}
static WM_HWIN (*WM_GET_PARENT[])(WM_HWIN) = {WM_GetParent1C,WM_GetParent2C,WM_GetParent3C,WM_GetParent4C,WM_GetParent5C,WM_GetParent6C,WM_GetParent7C,WM_GetParent8C};

static void PROGBARMETER_SetUserClip(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	GUI_RECT r;
	r.x0 = pDrawItemInfo->x0; r.y0 = pDrawItemInfo->y0;
	r.x1 = pDrawItemInfo->x1; r.y1 = pDrawItemInfo->y1;
	WM_SetUserClipRect(&r);
}

static const GUI_POINT _aNeedle[] = {{-3, 0}, {-2, -55}, {0, -65}, {2, -55}, {3, 0}};
//ProgbarMeter控件指针绘制函数
static void PROGBARMETER_DispNeedle(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, short angleBEG, short angleEND, short valMIN, short valMAX)
{
	GUI_POINT aPoints[GUI_COUNTOF(_aNeedle)];
	GUI_RECT r;
	float Angle;

	short Value; char i, Text[8], *p = (char *)((PROGBAR_SKINFLEX_INFO *)pDrawItemInfo->p)->pText;
	for(i=0; i<7; i++){Text[i] = *p++; Text[i+1] = 0; if(*p == '%') break;}
	Value = atoi(Text); if(Value<valMIN) Value = valMIN; if(Value>valMAX) Value = valMAX;
	Angle = angleBEG - (abs(angleEND-angleBEG)/(valMAX-valMIN))*Value;
	//PROGBAR_GetMinMax(pDrawItemInfo->hWin, &valMIN, &valMAX);//emwin v5.30
	//Angle = angleBEG - (abs(angleEND-angleBEG)/(valMAX-valMIN))*PROGBAR_GetValue(pDrawItemInfo->hWin);//emwin v5.30

	Angle *= 3.1415926f / 180;
	GUI_SetColor(0x5fbf35);
	GUI_RotatePolygon(aPoints, _aNeedle, GUI_COUNTOF(_aNeedle), Angle);
	WM_GetWindowRectEx(pDrawItemInfo->hWin, &r);
	GUI_AA_FillPolygon(aPoints, GUI_COUNTOF(aPoints), (r.x1-r.x0)/2, (r.y1-r.y0)/2);
}

//ProgbarMeter控件的自定义绘制函数
static int SKIN_progbarmeter(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;
	WIN_BITMAP *winBMP;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	//case WIDGET_ITEM_DRAW_BACKGROUND:
	case WIDGET_ITEM_DRAW_FRAME:
	//case WIDGET_ITEM_DRAW_TEXT:
		break;
	default: return PROGBAR_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

	case WIDGET_ITEM_DRAW_BACKGROUND:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//位于第几层修改这里(WM_GetParent数=层数-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//从用户数据区读出"页面图片结构体"指针
		//获取此控件相对于主页面(比如:WINDOW)位置偏移坐标
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		PROGBARMETER_SetUserClip(pDrawItemInfo);//设置1个用户剪切区
		if(((PROGBAR_SKINFLEX_INFO *)pDrawItemInfo->p)->Index == PROGBAR_SKINFLEX_L)
			GUI_DrawBitmap(winBMP->normal, x0, y0);//从页面坐标显示图片,emWin会自己剪切出这个控件范围的图片
		else if(((PROGBAR_SKINFLEX_INFO *)pDrawItemInfo->p)->Index == PROGBAR_SKINFLEX_R)
			GUI_DrawBitmap(winBMP->normal, x0, y0);
		else PROGBAR_DrawSkinFlex(pDrawItemInfo);
		WM_SetUserClipRect(0);
		break;
  case WIDGET_ITEM_DRAW_TEXT:
		PROGBARMETER_DispNeedle(pDrawItemInfo, 150, -150, 0, 100);
		return PROGBAR_DrawSkinFlex(pDrawItemInfo);
	}
	return 0;
}

//ProgbarMeter控件的自定义绘制函数,位于第2层(比如:WINDOW->PROGBAR)
int SKIN_progbarmeter2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_progbarmeter(pDrawItemInfo, 2);
}
//ProgbarMeter控件的自定义绘制函数,位于第3层(比如:FRAMEWIN->CLIENT->PROGBAR)
int SKIN_progbarmeter3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_progbarmeter(pDrawItemInfo, 3);
}
//ProgbarMeter控件的自定义绘制函数,位于第4层(比如:WINDOW->FRAMEWIN->CLIENT->PROGBAR)
int SKIN_progbarmeter4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_progbarmeter(pDrawItemInfo, 4);
}
//ProgbarMeter控件的自定义绘制函数,位于第5层(比如:WINDOW->PROGBAR->CLIENT->WINDOW->PROGBAR)
int SKIN_progbarmeter5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_progbarmeter(pDrawItemInfo, 5);
}


