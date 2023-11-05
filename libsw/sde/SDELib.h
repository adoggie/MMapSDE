#ifndef _SDELIB_H
#define _SDELIB_H

#include "swBase.h"

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols



class CSDELibApp : public CWinApp
{
public:
	CSDELibApp();
	DECLARE_MESSAGE_MAP()
};
#endif 
