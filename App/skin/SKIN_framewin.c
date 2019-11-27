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
#include "GUI.h"
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

//Framewin�ؼ����Զ�����ƺ���
static int SKIN_framewin(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;
	WIN_BITMAP *winBMP;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	//case WIDGET_ITEM_DRAW_BACKGROUND:
	case WIDGET_ITEM_DRAW_FRAME:
	case WIDGET_ITEM_DRAW_SEP:
	//case WIDGET_ITEM_DRAW_TEXT:
	//case WIDGET_ITEM_GET_BORDERSIZE_L:
	//case WIDGET_ITEM_GET_BORDERSIZE_R:
	//case WIDGET_ITEM_GET_BORDERSIZE_T:
	//case WIDGET_ITEM_GET_BORDERSIZE_B:
	//case WIDGET_ITEM_GET_RADIUS:
		break;
	case WIDGET_ITEM_GET_BORDERSIZE_L:
	case WIDGET_ITEM_GET_BORDERSIZE_R:
	case WIDGET_ITEM_GET_BORDERSIZE_T:
	case WIDGET_ITEM_GET_BORDERSIZE_B:
	case WIDGET_ITEM_GET_RADIUS:
		return 0;
	case WIDGET_ITEM_DRAW_BACKGROUND:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//���û�����������"ҳ��ͼƬ�ṹ��"ָ��
		//��ȡ�˿ؼ��������ҳ��(����:WINDOW)λ��ƫ������
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		GUI_DrawBitmap(winBMP->normal, x0, y0);//��ҳ��������ʾͼƬ,emWin���Լ����г�����ؼ���Χ��ͼƬ
		break;
	default: return FRAMEWIN_DrawSkinFlex(pDrawItemInfo);//emWinĬ�Ͽؼ����ƺ���
	}
	return 0;
}

//Framewin�ؼ����Զ�����ƺ���,λ�ڵ�1��(����:FRAMEWIN)
int SKIN_framewin1C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_framewin(pDrawItemInfo, 1);
}
//Framewin�ؼ����Զ�����ƺ���,λ�ڵ�2��(����:WINDOW->FRAMEWIN)
int SKIN_framewin2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_framewin(pDrawItemInfo, 2);
}
//Framewin�ؼ����Զ�����ƺ���,λ�ڵ�3��(����:FRAMEWIN->CLIENT->FRAMEWIN)
int SKIN_framewin3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_framewin(pDrawItemInfo, 3);
}
//Framewin�ؼ����Զ�����ƺ���,λ�ڵ�4��(����:WINDOW->FRAMEWIN->CLIENT->FRAMEWIN)
int SKIN_framewin4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_framewin(pDrawItemInfo, 4);
}
//Framewin�ؼ����Զ�����ƺ���,λ�ڵ�5��(����:WINDOW->MULTIPAGE->CLIENT->WINDOW->FRAMEWIN)
int SKIN_framewin5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_framewin(pDrawItemInfo, 5);
}

//Multipage��ҳ�汳���ػ溯��
static int SKIN_framewin_paint(WM_HWIN hPage, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;
	WIN_BITMAP *winBMP;

	hWin = (WM_GET_PARENT[C-1])(hPage);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
	WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//���û�����������"ҳ��ͼƬ�ṹ��"ָ��
	//��ȡ�˿ؼ��������ҳ��(����:WINDOW)λ��ƫ������
	x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(hPage);
	y0 = WM_GetWindowOrgY(hWin) - WM_GetWindowOrgY(hPage);
	GUI_DrawBitmap(winBMP->normal, x0, y0);//��ҳ��������ʾͼƬ,emWin���Լ����г�����ؼ���Χ��ͼƬ
	return 0;
}

//Multipage��ҳ�汳���ػ溯��,λ�ڵ�1��(����:FRAMEWIN)
int SKIN_framewin_paint1C(WM_HWIN hWin){
	return SKIN_framewin_paint(hWin, 1);
}
//Multipage��ҳ�汳���ػ溯��,λ�ڵ�2��(����:WINDOW->FRAMEWIN)
int SKIN_framewin_paint2C(WM_HWIN hWin){
	return SKIN_framewin_paint(hWin, 2);
}
//Multipage��ҳ�汳���ػ溯��,λ�ڵ�3��(����:FRAMEWIN->CLIENT->FRAMEWIN)
int SKIN_framewin_paint3C(WM_HWIN hWin){
	return SKIN_framewin_paint(hWin, 3);
}
//Multipage��ҳ�汳���ػ溯��,λ�ڵ�4��(����:WINDOW->FRAMEWIN->CLIENT->FRAMEWIN)
int SKIN_framewin_paint4C(WM_HWIN hWin){
	return SKIN_framewin_paint(hWin, 4);
}
//Multipage��ҳ�汳���ػ溯��,λ�ڵ�5��(����:FRAMEWIN->CLIENT->MULTIPAGE->CLIENT->WINDOW)
int SKIN_framewin_paint5C(WM_HWIN hWin){
	return SKIN_framewin_paint(hWin, 5);
}

