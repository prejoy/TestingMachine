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

//��ȡ�����ھ��,����ҳ����,������ҳ��λͼ��ָ��
static WM_HWIN WM_GetParent2C(WM_HWIN hChild) {return hChild;}
static WM_HWIN WM_GetParent3C(WM_HWIN hChild) {return WM_GetParent(hChild);}
static WM_HWIN WM_GetParent4C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(hChild));}
static WM_HWIN WM_GetParent5C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(hChild)));}
static WM_HWIN WM_GetParent6C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))));}
static WM_HWIN WM_GetParent7C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))));}
static WM_HWIN WM_GetParent8C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))))));}
static WM_HWIN WM_GetParent9C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))))));}
static WM_HWIN (*WM_GET_PARENT[])(WM_HWIN) = {WM_GetParent2C,WM_GetParent3C,WM_GetParent4C,WM_GetParent5C,WM_GetParent6C,WM_GetParent7C,WM_GetParent8C,WM_GetParent9C};

//Checkbox�ؼ����Զ�����ƺ���
static int SKIN_checkbox(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;
	GUI_RECT r;
	WIN_BITMAP *winBMP;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	//case WIDGET_ITEM_DRAW_BUTTON:
	//case WIDGET_ITEM_DRAW_BITMAP:
	//case WIDGET_ITEM_DRAW_FOCUS:
	//case WIDGET_ITEM_DRAW_TEXT:
		break;
	default: return CHECKBOX_DrawSkinFlex(pDrawItemInfo);//emWinĬ�Ͽؼ����ƺ���

	case WIDGET_ITEM_DRAW_BUTTON:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//���û�����������"ҳ��ͼƬ�ṹ��"ָ��
		//��ȡ�˿ؼ��������ҳ��(����:����WM_HBKWIN->WINDOW)λ��ƫ������
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		GUI_DrawBitmap(winBMP->normal, x0, y0);//��ҳ��������ʾͼƬ,emWin���Լ����г�����ؼ���Χ��ͼƬ
		break;
	case WIDGET_ITEM_DRAW_BITMAP://ѡ���
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		GUI_DrawBitmap(winBMP->mark, x0, y0);
		break;
	case WIDGET_ITEM_DRAW_FOCUS:
		CHECKBOX_DrawSkinFlex(pDrawItemInfo);
		break;

	}
	return 0;
}

//Checkbox�ؼ����Զ�����ƺ���,λ�ڵ�2��(����:����WM_HBKWIN->WINDOW->CHECKBOX)
int SKIN_checkbox2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_checkbox(pDrawItemInfo, 2);
}
//Checkbox�ؼ����Զ�����ƺ���,λ�ڵ�3��(����:����WM_HBKWIN->FRAMEWIN->CLIENT->CHECKBOX)
int SKIN_checkbox3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_checkbox(pDrawItemInfo, 3);
}
//Checkbox�ؼ����Զ�����ƺ���,λ�ڵ�4��(����:����WM_HBKWIN->WINDOW->FRAMEWIN->CLIENT->CHECKBOX)
int SKIN_checkbox4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_checkbox(pDrawItemInfo, 4);
}
//Checkbox�ؼ����Զ�����ƺ���,λ�ڵ�5��(����:����WM_HBKWIN->WINDOW->MULTIPAGE->CLIENT->WINDOW->CHECKBOX)
int SKIN_checkbox5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_checkbox(pDrawItemInfo, 5);
}

