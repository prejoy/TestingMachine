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

static void RADIO_SetUserClip(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	GUI_RECT r;
	r.x0 = pDrawItemInfo->x0; r.y0 = pDrawItemInfo->y0;
	r.x1 = pDrawItemInfo->x1; r.y1 = pDrawItemInfo->y1;
	WM_SetUserClipRect(&r);
}

//Radio控件的自定义绘制函数
static int SKIN_radio(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;
	WIN_BITMAP *winBMP;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	//case WIDGET_ITEM_DRAW_BUTTON:
	//case WIDGET_ITEM_DRAW_FOCUS:
	//case WIDGET_ITEM_DRAW_TEXT:
	case WIDGET_ITEM_GET_BUTTONSIZE:
		break;
	default: return RADIO_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

	case WIDGET_ITEM_DRAW_BUTTON:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//位于第几层修改这里(WM_GetParent数=层数-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//从用户数据区读出"页面图片结构体"指针
		//获取此控件相对于主页面(比如:WINDOW)位置偏移坐标
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		RADIO_SetUserClip(pDrawItemInfo);//设置1个用户剪切区
		if(RADIO_GetValue(pDrawItemInfo->hWin) != pDrawItemInfo->ItemIndex)
			GUI_DrawBitmap(winBMP->normal, x0, y0);//从页面坐标显示图片,emWin会自己剪切出这个控件范围的图片
		else
			GUI_DrawBitmap(winBMP->mark, x0, y0);
		WM_SetUserClipRect(0);
		break;
	}
	return 0;
}

//Radio控件的自定义绘制函数,位于第2层(比如:WINDOW->RADIO)
int SKIN_radio2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_radio(pDrawItemInfo, 2);
}
//Radio控件的自定义绘制函数,位于第3层(比如:FRAMEWIN->CLIENT->RADIO)
int SKIN_radio3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_radio(pDrawItemInfo, 3);
}
//Radio控件的自定义绘制函数,位于第4层(比如:WINDOW->FRAMEWIN->CLIENT->RADIO)
int SKIN_radio4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_radio(pDrawItemInfo, 4);
}
//Radio控件的自定义绘制函数,位于第5层(比如:WINDOW->RADIO->CLIENT->WINDOW->RADIO)
int SKIN_radio5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_radio(pDrawItemInfo, 5);
}
