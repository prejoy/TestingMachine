//=============================================================================
//             _   __    ______    ____       ______    ______
//            / | / /   / ____/   / __ \     / ____/   / ____/
//           /  |/ /   / __/     / / / /    / __/     / __/
//          / /|  /   / /___    / /_/ /    / /___    / /___
//         /_/ |_/   /_____/    \___\_\   /_____/   /_____/
//        (c)1996-2016 NEQEE Photoelectric Technology Co., Ltd.
//        Internet: www.neqee.com  Designer: Chen ZeHao
//=============================================================================

#include <stddef.h>           /* needed for definition of NULL */
#include "GUI_Private.h"
#include "glcd.h"
 

static const GUI_FONT_PROP_GL GUI_UNI_PTR * _FindChar(const GUI_FONT_PROP_GL GUI_UNI_PTR *pProp, U16P c)
{
	for (; pProp; pProp = pProp->pNext) if ((c>=pProp->First) && (c<=pProp->Last)) break;
	return pProp;
}


void GUIPROP_GL_DispChar(U16P c)
{

}



int GUIPROP_GL_GetCharDistX(U16P c, int * pSizeX)
{
	const GUI_FONT_PROP_GL GUI_UNI_PTR * pProp = _FindChar((const GUI_FONT_PROP_GL GUI_UNI_PTR *)((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->p.pProp, c);
	return (pProp) ? ((((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->Encode&16) ? pProp->pXsize[c-pProp->First] : pProp->pXsize[0]) * ((const GUI_FONT_GL GUI_UNI_PTR*)GUI_pContext->pAFont)->XMag : 0;
}


void GUIPROP_GL_GetFontInfo(const GUI_FONT GUI_UNI_PTR * pFont, GUI_FONTINFO* pfi)
{
	GUI_USE_PARA(pFont);
	pfi->Flags = GUI_FONTINFO_FLAG_PROP;
}


char GUIPROP_GL_IsInFont(const GUI_FONT GUI_UNI_PTR * pFont, U16 c)
{
	const GUI_FONT_PROP_GL GUI_UNI_PTR * pProp = _FindChar((const GUI_FONT_PROP_GL GUI_UNI_PTR *)(const GUI_FONT_GL GUI_UNI_PTR*)pFont->p.pProp, c);
	return (pProp==NULL) ? 0 : 1;
}




	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 




