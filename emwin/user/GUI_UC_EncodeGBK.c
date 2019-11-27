//#include "gui_private.h"
#include "GUI_Private.h"
//GBK encode range: 0x8140->0xfefe

static U16 _GetCharCode(const char GUI_UNI_PTR * s) {
	U8 ch = *(const U8*)s;
	if (ch <= 0x7f) return ch;
	else if (ch >= 0x81) return ((ch & 0xff) << 8) | (*(s+1) & 0xff);
	else GUI_DEBUG_ERROROUT("Illegal chacter during GBK decoding!\n");
	return 0;
}

static int _GetCharSize(const char GUI_UNI_PTR * s) {
	U8 ch = *s;
	if (ch <= 0x7f) return 1;
	else if (ch >= 0x81) return 2;
	else GUI_DEBUG_ERROROUT("Illegal chacter during GBK decoding!\n");
	return 1;
}

static int _CalcSizeOfChar(U16 ch) {
	if (ch <= 0x7f) return 1;
	else if(((ch>>8) >= 0x81) && ((ch>>8) <= 0xfe) && ((ch&0xff) >= 0x40) && ((ch&0xff) <= 0xfe)) return 2;
	else GUI_DEBUG_ERROROUT("Illegal chacter during GBK decoding!\n");
	return 1;
}

static int _Encode(char *s, U16 ch) {
	int r;
	if((r = _CalcSizeOfChar(ch)) == 1) *s = (char)ch;
	else if(r == 2) {*s++ = (ch>>8)&0xff; *s = ch&0xff;}
	else GUI_DEBUG_ERROROUT("Illegal chacter during GBK decoding!\n");
	return r;
}

const GUI_UC_ENC_APILIST GUI_UC_GBK = {
	_GetCharCode,
	_GetCharSize,
	_CalcSizeOfChar,
	_Encode
};

void GUI_UC_SetEncodeGBK(void) {
	GUI_LOCK();
	GUI_pUC_API = &GUI_UC_GBK;
	GUI_UNLOCK();
}


	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
