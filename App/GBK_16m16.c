#include "GUI.h"

static GUI_CONST_STORAGE unsigned char Xsize002[] = {//字库第2分段字符的宽(数组)
  16
};

static GUI_CONST_STORAGE unsigned char Xsize001[] = {//字库第1分段字符的宽(数组)
  8
};

static GUI_CONST_STORAGE GUI_FONT_PROP_GL Prop002 = {//字库第2分段
   0x0080//起始字符
  ,0xfefe//结束字符
  ,&Xsize002[0]
  ,0
};

static GUI_CONST_STORAGE GUI_FONT_PROP_GL Prop001 = {//字库第1分段
   0x0020//起始字符
  ,0x007f//结束字符
  ,&Xsize001[0]
  ,&Prop002//指向下1分段
}; 

GUI_CONST_STORAGE GUI_FONT_GL GBK_16m16 = {
   GUI_FONTTYPE_PROP_GL
  ,16        //字符的高(所有字符只有1种高度)
  ,16        //字符显示时Y方向的间距，等于字符高时则行距为0
  ,1         //X方向放大倍数，不支持字符放大，永远是1
  ,1         //Y方向放大倍数，不支持字符放大，永远是1
  ,&Prop001  //指向字库第1分段
  ,0         //未知
  ,0         //未知
  ,0         //未知
  ,2         //字库类型 1:gb2312 2:gbk 4:big5 8:unicode(monospaced) 16:unicode(non-monospaced)
  ,"a16:k16" //Nand-Flash中的字库文件名(包括ascii和非ascii两个字库,并用":"隔开)
};








