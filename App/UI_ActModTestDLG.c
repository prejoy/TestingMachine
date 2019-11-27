/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.30                          *
*        Compiled Jul  1 2015, 10:50:32                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0  (GUI_ID_USER + 0x00)
#define ID_IMAGE_0  (GUI_ID_USER + 0x01)
#define ID_TEXT_0  (GUI_ID_USER + 0x02)
#define ID_EDIT_0  (GUI_ID_USER + 0x03)
#define ID_TEXT_1  (GUI_ID_USER + 0x04)
#define ID_TEXT_2  (GUI_ID_USER + 0x05)
#define ID_TEXT_3  (GUI_ID_USER + 0x06)
#define ID_TEXT_4  (GUI_ID_USER + 0x07)
#define ID_TEXT_5  (GUI_ID_USER + 0x08)
#define ID_TEXT_6  (GUI_ID_USER + 0x09)
#define ID_TEXT_7  (GUI_ID_USER + 0x0A)
#define ID_TEXT_8  (GUI_ID_USER + 0x0B)
#define ID_EDIT_1  (GUI_ID_USER + 0x0C)
#define ID_EDIT_2  (GUI_ID_USER + 0x0D)
#define ID_EDIT_3  (GUI_ID_USER + 0x0E)
#define ID_EDIT_4  (GUI_ID_USER + 0x0F)
#define ID_EDIT_5  (GUI_ID_USER + 0x10)
#define ID_EDIT_6  (GUI_ID_USER + 0x11)
#define ID_EDIT_7  (GUI_ID_USER + 0x12)
#define ID_EDIT_8  (GUI_ID_USER + 0x13)
#define ID_TEXT_9  (GUI_ID_USER + 0x15)
#define ID_TEXT_10  (GUI_ID_USER + 0x16)
#define ID_EDIT_9  (GUI_ID_USER + 0x17)
#define ID_TEXT_11  (GUI_ID_USER + 0x18)
#define ID_IMAGE_1  (GUI_ID_USER + 0x1E)

#define ID_IMAGE_0_IMAGE_0  0x00
#define ID_IMAGE_1_IMAGE_0  0x01

// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _acImage_0, "BMP", ID_IMAGE_0_IMAGE_0
*/
static const U8 _acImage_0[463] = {
  0x42, 0x4D, 0xCE, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0x12, 0x0B, 0x00, 0x00,
  0x12, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21,
  0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00,
  0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84,
  0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08,
  0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10,
  0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00,
  0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42,
  0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84,
  0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08,
  0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00,
  0x00,
};

/*********************************************************************
*
*       _acImage_1, "BMP", ID_IMAGE_1_IMAGE_0
*/
static const U8 _acImage_1[463] = {
  0x42, 0x4D, 0xCE, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0x12, 0x0B, 0x00, 0x00,
  0x12, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21,
  0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00,
  0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84,
  0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08,
  0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10,
  0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00,
  0x88, 0x42, 0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42,
  0x10, 0x84, 0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84,
  0x21, 0x08, 0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0x88, 0x42, 0x10, 0x84, 0x21, 0x08,
  0x40, 0x00, 0x84, 0x21, 0x08, 0x42, 0x10, 0x84, 0x40, 0x00, 0xC2, 0x10, 0x84, 0x21, 0x08, 0x42, 0x40, 0x00, 0xA1, 0x08, 0x42, 0x10, 0x84, 0x21, 0x40, 0x00, 0x90, 0x84, 0x21, 0x08, 0x42, 0x10, 0xC0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00,
  0x00,
};

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "UI_ActModTest", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
  { IMAGE_CreateIndirect, "ImageFGP", ID_IMAGE_0, 460, 135, 320, 320, 0, 0, 0 },
  { TEXT_CreateIndirect, "TextTouchout", ID_TEXT_0, 20, 20, 65, 20, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "EditTouchout", ID_EDIT_0, 95, 20, 120, 20, 0, 0xf, 0 },
  { TEXT_CreateIndirect, "TextStartTime", ID_TEXT_1, 20, 80, 65, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "TextTVcc", ID_TEXT_2, 20, 260, 65, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "TextMVcc", ID_TEXT_3, 20, 290, 65, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "TextGetVersion", ID_TEXT_4, 20, 110, 65, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "TextXorCheck", ID_TEXT_5, 20, 140, 65, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "TextReg", ID_TEXT_6, 20, 170, 65, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "TextLed", ID_TEXT_7, 20, 200, 65, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "TextGetImage", ID_TEXT_8, 20, 230, 65, 20, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "EditStartTime", ID_EDIT_1, 95, 80, 120, 20, 0, 0xf, 0 },
  { EDIT_CreateIndirect, "EditTVcc", ID_EDIT_2, 95, 260, 120, 20, 0, 0xf, 0 },
  { EDIT_CreateIndirect, "EditMVcc", ID_EDIT_3, 95, 290, 120, 20, 0, 0xf, 0 },
  { EDIT_CreateIndirect, "EditGetVersion", ID_EDIT_4, 95, 110, 120, 20, 0, 0xf, 0 },
  { EDIT_CreateIndirect, "EditXorCheck", ID_EDIT_5, 95, 140, 120, 20, 0, 0xf, 0 },
  { EDIT_CreateIndirect, "EditReg", ID_EDIT_6, 95, 170, 120, 20, 0, 0xf, 0 },
  { EDIT_CreateIndirect, "EditLed", ID_EDIT_7, 95, 200, 120, 20, 0, 0xf, 0 },
  { EDIT_CreateIndirect, "EditGetImage", ID_EDIT_8, 95, 230, 120, 20, 0, 0xf, 0 },
  { TEXT_CreateIndirect, "TextStateMsg", ID_TEXT_9, 465, 25, 300, 100, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "TextCaliBaud", ID_TEXT_10, 20, 50, 80, 20, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "EditCaliBaud", ID_EDIT_9, 95, 50, 120, 20, 0, 0x5, 0 },
  { TEXT_CreateIndirect, "TextMessageToUser", ID_TEXT_11, 20, 333, 208, 110, 0, 0x64, 0 },
  { IMAGE_CreateIndirect, "Image", ID_IMAGE_1, 236, 20, 175, 285, 0, 0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetImageById
*/
static const void * _GetImageById(U32 Id, U32 * pSize) {
  switch (Id) {
  case ID_IMAGE_0_IMAGE_0:
    *pSize = sizeof(_acImage_0);
    return (const void *)_acImage_0;
  case ID_IMAGE_1_IMAGE_0:
    *pSize = sizeof(_acImage_1);
    return (const void *)_acImage_1;
  }
  return NULL;
}

// USER START (Optionally insert additional static code)
#undef GUI_MAKE_COLOR
#define GUI_MAKE_COLOR
extern WM_HWIN AllWindows[3];
extern GUI_CONST_STORAGE GUI_FONT GBK_16m16;
#define GUI_FONT_16B_1 &GBK_16m16
extern GUI_BITMAP StdImgBitMap;
#include "string.h"
#include "stdlib.h"
#include "stm32f4xx.h"
#include "App_main.h"
#include "bitmap.h"
#include "skin.h"
#include "WifiCommu.h"
#include "ff.h"
#include "TestFuncs.h"
#include "usart.h"
#include "fattester.h"
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  const void * pData;
  WM_HWIN      hItem;
  U32          FileSize;
  int          NCode;
  int          Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'UI_ActModTest'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00D7D7D7));
    //
    // Initialization of 'ImageFGP'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
    pData = _GetImageById(ID_IMAGE_0_IMAGE_0, &FileSize);
    IMAGE_SetBMP(hItem, pData, FileSize);
    //
    // Initialization of 'TextTouchout'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetText(hItem, "触控测试");
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'EditTouchout'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'TextStartTime'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "启动时间");
    //
    // Initialization of 'TextTVcc'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "触控功耗");
    //
    // Initialization of 'TextMVcc'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "模组功耗");
    //
    // Initialization of 'TextGetVersion'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "获取版本");
    //
    // Initialization of 'TextXorCheck'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "异或校验");
    //
    // Initialization of 'TextReg'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "Flash");
    //
    // Initialization of 'TextLed'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "灯带");
    //
    // Initialization of 'TextGetImage'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "获取图像");
    //
    // Initialization of 'EditStartTime'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_1);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'EditTVcc'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_2);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'EditMVcc'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_3);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'EditGetVersion'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_4);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'EditXorCheck'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_5);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'EditReg'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_6);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'EditLed'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_7);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'EditGetImage'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_8);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'TextStateMsg'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "wifimesg");
    //
    // Initialization of 'TextCaliBaud'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetText(hItem, "校准串口");
    //
    // Initialization of 'EditCaliBaud'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_9);
    EDIT_SetText(hItem, "0");
    EDIT_SetFont(hItem, GUI_FONT_16B_1);
    //
    // Initialization of 'TextMessageToUser'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_11);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x000000FF));
//    TEXT_SetText(hItem, "MessageToUser");
    TEXT_SetText(hItem, "请按KEY1开始测试");
    //
    // Initialization of 'Image'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_1);
    pData = _GetImageById(ID_IMAGE_1_IMAGE_0, &FileSize);
    IMAGE_SetBMP(hItem, pData, FileSize);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_EDIT_0: // Notifications sent by 'EditTouchout'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_1: // Notifications sent by 'EditStartTime'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_2: // Notifications sent by 'EditTVcc'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_3: // Notifications sent by 'EditMVcc'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_4: // Notifications sent by 'EditGetVersion'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_5: // Notifications sent by 'EditXorCheck'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_6: // Notifications sent by 'EditReg'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_7: // Notifications sent by 'EditLed'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_8: // Notifications sent by 'EditGetImage'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_9: // Notifications sent by 'EditCaliBaud'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateUI_ActModTest
*/
WM_HWIN CreateUI_ActModTest(void);
WM_HWIN CreateUI_ActModTest(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
