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

//Dropdown控件的自定义绘制函数
int SKIN_dropdown(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;
	WIN_BITMAP *winBMP;
	GUI_BITMAP *pBMP;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	case WIDGET_ITEM_DRAW_ARROW:
	//case WIDGET_ITEM_DRAW_BACKGROUND:
	//case WIDGET_ITEM_DRAW_TEXT:
		break;
	default: return DROPDOWN_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

	case WIDGET_ITEM_DRAW_BACKGROUND:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//位于第几层修改这里(WM_GetParent数=层数-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//从用户数据区读出"页面图片结构体"指针
		//获取此控件相对于主页面(比如:WINDOW)位置偏移坐标
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		//备注：pDrawItemInfo->ItemIndex有BUG,无法定位状态
		if(pDrawItemInfo->ItemIndex == DROPDOWN_SKINFLEX_PI_ENABLED)
			GUI_DrawBitmap(winBMP->normal, x0, y0);//从页面坐标显示图片,emWin会自己剪切出这个控件范围的图片
		else if(pDrawItemInfo->ItemIndex == DROPDOWN_SKINFLEX_PI_EXPANDED)
			GUI_DrawBitmap(winBMP->mark, x0, y0);
		else if(pDrawItemInfo->ItemIndex == DROPDOWN_SKINFLEX_PI_FOCUSED)
			GUI_DrawBitmap(winBMP->focus, x0, y0);
		else if(pDrawItemInfo->ItemIndex == DROPDOWN_SKINFLEX_PI_DISABLED)
			GUI_DrawBitmap(winBMP->disable, x0, y0);
		else
			GUI_DrawBitmap(winBMP->normal, x0, y0);
		break;
	}
	return 0;
}

//Dropdown控件的自定义绘制函数,位于第2层(比如:WINDOW->DROPDOWN)
int SKIN_dropdown2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_dropdown(pDrawItemInfo, 2);
}
//Dropdown控件的自定义绘制函数,位于第3层(比如:FRAMEWIN->CLIENT->DROPDOWN)
int SKIN_dropdown3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_dropdown(pDrawItemInfo, 3);
}
//Dropdown控件的自定义绘制函数,位于第4层(比如:WINDOW->FRAMEWIN->CLIENT->DROPDOWN)
int SKIN_dropdown4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_dropdown(pDrawItemInfo, 4);
}
//Dropdown控件的自定义绘制函数,位于第5层(比如:WINDOW->MULTIPAGE->CLIENT->WINDOW->DROPDOWN)
int SKIN_dropdown5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_dropdown(pDrawItemInfo, 5);
}
