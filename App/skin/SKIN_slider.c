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

extern GUI_CONST_STORAGE GUI_BITMAP bmslider_thumb0;
extern GUI_CONST_STORAGE GUI_BITMAP bmslider_thumb2;

//��ȡ�����ھ��,����ҳ����,������ҳ��λͼ��ָ��
static WM_HWIN WM_GetParent1C(WM_HWIN hChild) {return hChild;}
static WM_HWIN WM_GetParent2C(WM_HWIN hChild) {return WM_GetParent(hChild);}
static WM_HWIN WM_GetParent3C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(hChild));}
static WM_HWIN WM_GetParent4C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(hChild)));}
static WM_HWIN WM_GetParent5C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))));}
static WM_HWIN WM_GetParent6C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))));}
static WM_HWIN WM_GetParent7C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))))));}
static WM_HWIN WM_GetParent8C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))))));}
static WM_HWIN (*WM_GET_PARENT[])(WM_HWIN) = {WM_GetParent1C,WM_GetParent2C,WM_GetParent3C,WM_GetParent4C,WM_GetParent5C,WM_GetParent6C,WM_GetParent7C,WM_GetParent8C};

//Slider�ؼ����Զ�����ƺ���
static int SKIN_slider(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;
	WIN_BITMAP *winBMP;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	//case WIDGET_ITEM_DRAW_FOCUS:
	//case WIDGET_ITEM_DRAW_SHAFT:
	//case WIDGET_ITEM_DRAW_THUMB:
	case WIDGET_ITEM_DRAW_TICKS:
		break;
	default: return SLIDER_DrawSkinFlex(pDrawItemInfo);//emWinĬ�Ͽؼ����ƺ���

	case WIDGET_ITEM_DRAW_SHAFT:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//���û�����������"ҳ��ͼƬ�ṹ��"ָ��
		//��ȡ�˿ؼ��������ҳ��(����:WINDOW)λ��ƫ������
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		GUI_DrawBitmap(winBMP->normal, x0, y0);//��ҳ��������ʾͼƬ,emWin���Լ����г�����ؼ���Χ��ͼƬ
		break;
	case WIDGET_ITEM_DRAW_THUMB:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));
		if(((SLIDER_SKINFLEX_INFO *)(pDrawItemInfo->p))->IsPressed != 1)
			GUI_DrawBitmap(winBMP->thumbN, pDrawItemInfo->x0, pDrawItemInfo->y0);
		else
			GUI_DrawBitmap(winBMP->thumbM, pDrawItemInfo->x0, pDrawItemInfo->y0);
		break;

	}
	return 0;
}

//Slider�ؼ����Զ�����ƺ���,λ�ڵ�2��(����:WINDOW->SLIDER)
int SKIN_slider2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_slider(pDrawItemInfo, 2);
}
//Slider�ؼ����Զ�����ƺ���,λ�ڵ�3��(����:FRAMEWIN->CLIENT->SLIDER)
int SKIN_slider3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_slider(pDrawItemInfo, 3);
}
//Slider�ؼ����Զ�����ƺ���,λ�ڵ�4��(����:WINDOW->FRAMEWIN->CLIENT->SLIDER)
int SKIN_slider4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_slider(pDrawItemInfo, 4);
}
//Slider�ؼ����Զ�����ƺ���,λ�ڵ�5��(����:WINDOW->SLIDER->CLIENT->WINDOW->SLIDER)
int SKIN_slider5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_slider(pDrawItemInfo, 5);
}
