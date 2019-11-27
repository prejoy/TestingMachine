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

static void SCROLLBAR_SetUserClip(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	GUI_RECT r;
	if(((SCROLLBAR_SKINFLEX_INFO *)pDrawItemInfo->p)->IsVertical) {//����SCROLLBAR
		r.x0 = pDrawItemInfo->y0; r.y0 = pDrawItemInfo->x0;
		r.x1 = pDrawItemInfo->y1; r.y1 = pDrawItemInfo->x1;
	}
	else {//����SCROLLBAR
		r.x0 = pDrawItemInfo->x0; r.y0 = pDrawItemInfo->y0;
		r.x1 = pDrawItemInfo->x1; r.y1 = pDrawItemInfo->y1;
	}
	WM_SetUserClipRect(&r);
}

//Scrollbar�ؼ����Զ�����ƺ���
static int SKIN_scrollbar(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, unsigned char C)
{
	WM_HWIN hWin;
	int x0, y0;
	WIN_BITMAP *winBMP;

	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	//case WIDGET_ITEM_DRAW_BUTTON_L:
	//case WIDGET_ITEM_DRAW_BUTTON_R:
	//case WIDGET_ITEM_DRAW_OVERLAP:
	//case WIDGET_ITEM_DRAW_SHAFT_L:
	//case WIDGET_ITEM_DRAW_SHAFT_R:
	//case WIDGET_ITEM_DRAW_THUMB:
	//case WIDGET_ITEM_GET_BUTTONSIZE:
	break;

	default: return SCROLLBAR_DrawSkinFlex(pDrawItemInfo);//emWinĬ�Ͽؼ����ƺ���

	case WIDGET_ITEM_DRAW_BUTTON_L:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//���û�����������"ҳ��ͼƬ�ṹ��"ָ��
		//��ȡ�˿ؼ��������ҳ��(����:WINDOW)λ��ƫ������
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		SCROLLBAR_SetUserClip(pDrawItemInfo);//����1���û�������
		if(((SCROLLBAR_SKINFLEX_INFO *)(pDrawItemInfo->p))->State == 0)
			GUI_DrawBitmap(winBMP->normal, x0, y0);//��ҳ��������ʾͼƬ,emWin���Լ����г�����ؼ���Χ��ͼƬ
		else if(((SCROLLBAR_SKINFLEX_INFO *)(pDrawItemInfo->p))->State == 1)
			GUI_DrawBitmap(winBMP->normal, x0, y0);
		else if(((SCROLLBAR_SKINFLEX_INFO *)(pDrawItemInfo->p))->State == 2)
			GUI_DrawBitmap(winBMP->mark, x0, y0);
		else if(((SCROLLBAR_SKINFLEX_INFO *)(pDrawItemInfo->p))->State == 3)
			GUI_DrawBitmap(winBMP->normal, x0, y0);
		else
			SCROLLBAR_DrawSkinFlex(pDrawItemInfo);
		WM_SetUserClipRect(0);
		break;
	case WIDGET_ITEM_DRAW_BUTTON_R:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		SCROLLBAR_SetUserClip(pDrawItemInfo);
		if(((SCROLLBAR_SKINFLEX_INFO *)(pDrawItemInfo->p))->State == 0)
			GUI_DrawBitmap(winBMP->normal, x0, y0);
		else if(((SCROLLBAR_SKINFLEX_INFO *)(pDrawItemInfo->p))->State == 1)
			GUI_DrawBitmap(winBMP->mark, x0, y0);
		else if(((SCROLLBAR_SKINFLEX_INFO *)(pDrawItemInfo->p))->State == 2)
			GUI_DrawBitmap(winBMP->normal, x0, y0);
		else if(((SCROLLBAR_SKINFLEX_INFO *)(pDrawItemInfo->p))->State == 3)
			GUI_DrawBitmap(winBMP->normal, x0, y0);
		else
			SCROLLBAR_DrawSkinFlex(pDrawItemInfo);
		WM_SetUserClipRect(0);
		break;
	case WIDGET_ITEM_DRAW_THUMB:
		SCROLLBAR_DrawSkinFlex(pDrawItemInfo);
		break;
	case WIDGET_ITEM_DRAW_SHAFT_L:
	case WIDGET_ITEM_DRAW_SHAFT_R:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		SCROLLBAR_SetUserClip(pDrawItemInfo);
		GUI_DrawBitmap(winBMP->normal, x0, y0);
		WM_SetUserClipRect(0);
		break;
	case WIDGET_ITEM_DRAW_OVERLAP:
		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);
		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));
		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
		SCROLLBAR_SetUserClip(pDrawItemInfo);
		GUI_DrawBitmap(winBMP->normal, x0, y0);
		WM_SetUserClipRect(0);
		break;
	}
	return 0;
}

//Scrollbar�ؼ����Զ�����ƺ���,λ�ڵ�2��(����:WINDOW->SCROLLBAR)
int SKIN_scrollbar2C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_scrollbar(pDrawItemInfo, 2);
}
//Scrollbar�ؼ����Զ�����ƺ���,λ�ڵ�3��(����:FRAMEWIN->CLIENT->SCROLLBAR)
int SKIN_scrollbar3C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_scrollbar(pDrawItemInfo, 3);
}
//Scrollbar�ؼ����Զ�����ƺ���,λ�ڵ�4��(����:WINDOW->FRAMEWIN->CLIENT->SCROLLBAR)
int SKIN_scrollbar4C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_scrollbar(pDrawItemInfo, 4);
}
//Scrollbar�ؼ����Զ�����ƺ���,λ�ڵ�5��(����:WINDOW->SCROLLBAR->CLIENT->WINDOW->SCROLLBAR)
int SKIN_scrollbar5C(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo){
	return SKIN_scrollbar(pDrawItemInfo, 5);
}
