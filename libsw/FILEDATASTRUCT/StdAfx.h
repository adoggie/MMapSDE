// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DCF7E992_E672_4810_82A3_AC61C5BCBE33__INCLUDED_)
#define AFX_STDAFX_H__DCF7E992_E672_4810_82A3_AC61C5BCBE33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

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

#endif // !defined(AFX_STDAFX_H__DCF7E992_E672_4810_82A3_AC61C5BCBE33__INCLUDED_)
