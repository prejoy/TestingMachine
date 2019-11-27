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
static WM_HWIN WM_GetParent1C(WM_HWIN hChild) {return hChild;}
static WM_HWIN WM_GetParent2C(WM_HWIN hChild) {return WM_GetParent(hChild);}
static WM_HWIN WM_GetParent3C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(hChild));}
static WM_HWIN WM_GetParent4C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(hChild)));}
static WM_HWIN WM_GetParent5C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))));}
static WM_HWIN WM_GetParent6C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))));}
static WM_HWIN WM_GetParent7C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild))))));}
static WM_HWIN WM_GetParent8C(WM_HWIN hChild) {return WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(hChild)))))));}
static WM_HWIN (*WM_GET_PARENT[])(WM_HWIN) = {WM_GetParent1C,WM_GetParent2C,WM_GetParent3C,WM_GetParent4C,WM_GetParent5C,WM_GetParent6C,WM_GetParent7C,WM_GetParent8C};

//Dropdown�ؼ����Զ�����ƺ���
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
	default: return DROPDOWN_DrawSkinFlex(pDrawItemInfo);//emWinĬ�Ͽؼ����ƺ���

	case WIDGET_ITEM_DRAW_BACKGROUND:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//���û�����������"ҳ��ͼƬ�ṹ��"ָ��
		//��ȡ�˿ؼ��������ҳ��(����:WINDOW)λ��ƫ������
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		//��ע��pDrawItemInfo->ItemIndex��BUG,�޷���λ״̬
		if(pDrawItemInfo->ItemIndex == DROPDOWN_SKINFLEX_PI_ENABLED)
			GUI_DrawBitmap(winBMP->normal, x0, y0);//��ҳ��������ʾͼƬ,emWin���Լ����г�����ؼ���Χ��ͼƬ
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

//Dropdown�ؼ����Զ�����ƺ���,λ�ڵ�2��(����:WINDOW->DROPDOWN)
int SKIN_dropdown2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_dropdown(pDrawItemInfo, 2);
}
//Dropdown�ؼ����Զ�����ƺ���,λ�ڵ�3��(����:FRAMEWIN->CLIENT->DROPDOWN)
int SKIN_dropdown3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_dropdown(pDrawItemInfo, 3);
}
//Dropdown�ؼ����Զ�����ƺ���,λ�ڵ�4��(����:WINDOW->FRAMEWIN->CLIENT->DROPDOWN)
int SKIN_dropdown4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_dropdown(pDrawItemInfo, 4);
}
//Dropdown�ؼ����Զ�����ƺ���,λ�ڵ�5��(����:WINDOW->MULTIPAGE->CLIENT->WINDOW->DROPDOWN)
int SKIN_dropdown5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_dropdown(pDrawItemInfo, 5);
}
