// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently

#if !defined(AFX_STDAFX_H__EDDB2422_059C_4D77_9CD5_D16A5EE168DC__INCLUDED_)
#define AFX_STDAFX_H__EDDB2422_059C_4D77_9CD5_D16A5EE168DC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#if defined(_WIN32_WCE) && (_WIN32_WCE >= 211) && (_AFXDLL)
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
/*
#if !defined(AFX_STDAFX_H__7814E96F_1ADA_4333_AEF0_9FB9E8FE34FA__INCLUDED_)
#define AFX_STDAFX_H__7814E96F_1ADA_4333_AEF0_9FB9E8FE34FA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
// OLE support available only for dynamic builds in MFC for Windows CE
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
*/
#include "Afxtempl.h"
#include "resource.h"
#include "GeoGlobe.h"
#include "Globe.h"

#ifdef _DEBUG
#define DUMPSTATE(p) ((p)->DumpState());
#define ASSERTVALID(p) ((p)->AssertValid());
#else
#define DUMPSTATE(p) 
#define ASSERTVALID(p) 
#endif 

#ifdef _DEBUG
#define TRACE_SLEEP_BY_TIMES(n)   static unsigned char nTraceCount = 0;\
                if((nTraceCount++) == (n))\
                {\
                    nTraceCount = 0;\
                    Sleep(100);\
                }
#else
#define TRACE_SLEEP_BY_TIMES(n)
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7814E96F_1ADA_4333_AEF0_9FB9E8FE34FA__INCLUDED_)
