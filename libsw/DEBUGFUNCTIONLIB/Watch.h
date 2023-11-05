// Watch.h: interface for the CWatch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATCH_H__F7ED1785_B704_41D3_A38D_BFB81B222B9B__INCLUDED_)
#define AFX_WATCH_H__F7ED1785_B704_41D3_A38D_BFB81B222B9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
#define WATCH(sz) CWatch aWatch(_T(sz))
#else
#define WATCH(sz)
#endif

/************************************************************************/
/* 简单的函数执行时间计算类                                                                     */
/************************************************************************/
class CWatch  
{
public:
	CWatch(LPCTSTR);
	virtual ~CWatch();
protected:
	CString m_strText;
	DWORD m_dwTickStart;
};

#endif // !defined(AFX_WATCH_H__F7ED1785_B704_41D3_A38D_BFB81B222B9B__INCLUDED_)
