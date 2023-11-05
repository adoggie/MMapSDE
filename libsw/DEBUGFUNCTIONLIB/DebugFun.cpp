// DebugFun.cpp: implementation of the CDebugFun class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ErrorLog.h"
#include "DebugFun.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDebugFun CDebugFun::_instance;
CDebugFun::CDebugFun()
{
    VERIFY(InitialOperation());
}

CDebugFun::~CDebugFun()
{
    VERIFY(ClearOperation());
}

BOOL CDebugFun::InitialOperation()
{
    //////////////////////////////////////////////////////////////////////////
    //取运行时应用程序名称作为记录文件. 
    TCHAR tchDir = '\\';
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
#ifdef _WIN32_WCE
    memset(szPath,0,sizeof(TCHAR)*MAX_PATH);
    _tcscpy(szPath,_T("\\"));
#endif
    
    _tcscpy ( _tcsrchr(szPath,tchDir),_T("\\DebugInfo.log"));
    Open_Error_File(szPath);
    
    return TRUE;
}

BOOL CDebugFun::ClearOperation()
{
    Close_Error_File();
    return TRUE;
}

void CDebugFun::DebugToScreen(int x, int y, LPCTSTR lpsz)
{
	HDC hDC = ::GetDC(NULL);
	RECT  rtScreen;
	SetRectEmpty(&rtScreen);

	::SetTextColor(hDC, RGB(0,0,0));
	::SetBkColor(hDC, RGB(255,255,255));
	::DrawText(hDC, lpsz, _tcslen(lpsz) , &rtScreen, DT_CALCRECT );
	::OffsetRect(&rtScreen, x, y );
	::DrawText(hDC, lpsz, _tcslen(lpsz) , &rtScreen, DT_LEFT|DT_TOP );	
	::ReleaseDC(NULL, hDC);
}
/************************************************************************/
/* 注意,此函数使用了_vstprintf,所以不能保证其安全性.                                                                     */
/************************************************************************/
void CDebugFun::DebugToScreenLine(LPCTSTR lpszFormat,...)
{
	static int nCurrentLine = 0;

    //////////////////////////////////////////////////////////////////////////
    if (!lpszFormat)
        return;
    
    TCHAR buffer[1024]; // working buffer
    va_list arglist; // variable argument list
    
    // print out the string using the variable number of arguments on stack
    va_start(arglist,lpszFormat);
    _vstprintf(buffer,lpszFormat,arglist);
    va_end(arglist);

	DebugToScreen(10,100+14*nCurrentLine,buffer);
    
    ////////////////////////////////////////////////////////////////////////// 
	if(nCurrentLine > 4)
	{
		nCurrentLine = 0;
	}
	else
	{
		nCurrentLine++;
	}
    
}

/************************************************************************/
/* 注意,此函数使用了_vstprintf,所以不能保证其安全性.                                                                     */
/************************************************************************/
void CDebugFun::DebugToLog(LPCTSTR lpszFormat,...)
{
    ASSERT(lpszFormat);
    TCHAR buffer[1024]; // working buffer
    va_list arglist; // variable argument list
    
    // print out the string using the variable number of arguments on stack
    va_start(arglist,lpszFormat);
    _vstprintf(buffer,lpszFormat,arglist);
    va_end(arglist);

	Write_Error(_T("%s\r\n"),buffer);
}
//////////////////////////////////////////////////////////////////////////
CMsgWatch::CMsgWatch(LPCTSTR lpsz)
{
    if(_tcslen(lpsz)>31)
    {
        memcpy(m_szText,lpsz,31*sizeof(TCHAR));
        m_szText[31] = _T('\0');
    }
    else
    {
        _tcscpy(m_szText,lpsz);
    }
    
	m_dwTickStart = GetTickCount();
}

CMsgWatch::~CMsgWatch()
{
    Write_Error(_T("%s use %.8d ms.\r\n"),m_szText, GetTickCount() - m_dwTickStart);
}
//////////////////////////////////////////////////////////////////////////
int CTrackFuntion::m_nPreSignCount = 0;
CTrackFuntion::CTrackFuntion(LPCTSTR lpsz)
{
    if(_tcslen(lpsz)>31)
    {
        memcpy(m_szText,lpsz,31*sizeof(TCHAR));
        m_szText[31] = _T('\0');
    }
    else
    {
        _tcscpy(m_szText,lpsz);
    }



    for(int i = 0; i < m_nPreSignCount; i++)
    {
        TRACE0(" ");
    }
    TRACE1("%s Enter.\n",m_szText);
    m_nPreSignCount++;
}
CTrackFuntion::~CTrackFuntion()
{
    m_nPreSignCount--;
    for(int i = 0; i < m_nPreSignCount; i++)
    {
        TRACE0(" ");
    }
    TRACE1("%s Leave.\n",m_szText);

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int CTrackFuntion_Write2Log::m_nPreSignCount = 0;
CTrackFuntion_Write2Log::CTrackFuntion_Write2Log(LPCTSTR lpsz)
{
    if(_tcslen(lpsz)>31)
    {
        memcpy(m_szText,lpsz,31*sizeof(TCHAR));
        m_szText[31] = _T('\0');
    }
    else
    {
        _tcscpy(m_szText,lpsz);
    }



    for(int i = 0; i < m_nPreSignCount; i++)
    {
        TRACE0(" ");
    }
    WRITE2LOG1("%s Enter.\n",m_szText);
    m_nPreSignCount++;
}
CTrackFuntion_Write2Log::~CTrackFuntion_Write2Log()
{
    m_nPreSignCount--;
    for(int i = 0; i < m_nPreSignCount; i++)
    {
        TRACE0(" ");
    }
    WRITE2LOG1("%s Leave.\n",m_szText);
}
//////////////////////////////////////////////////////////////////////////
CMemoryWatch::CMemoryWatch(LPCTSTR lpsz,DWORD dwDifferLimit)
{
    if(_tcslen(lpsz)>31)
    {
        memcpy(m_szText,lpsz,31*sizeof(TCHAR));
        m_szText[31] = _T('\0');
    }
    else
    {
        _tcscpy(m_szText,lpsz);
    }   
    m_dwDifferLimit = dwDifferLimit;
    ::GlobalMemoryStatus( &m_MemoryStatus );  
}

CMemoryWatch::~CMemoryWatch()
{
    MEMORYSTATUS mst;    
    ::GlobalMemoryStatus( &mst); 
    DWORD dwDiffer = 0;
    if(mst.dwAvailPhys > m_MemoryStatus.dwAvailPhys)
    {
        dwDiffer = mst.dwAvailPhys - m_MemoryStatus.dwAvailPhys;
    }
    else
    {
        dwDiffer = m_MemoryStatus.dwAvailPhys - mst.dwAvailPhys;
    }

    
    if(dwDiffer >= m_dwDifferLimit)
    {
        TRACE2("When %s Memory Differ %d kb.\n",m_szText,dwDiffer>>10);
    }
}
//////////////////////////////////////////////////////////////////////////
