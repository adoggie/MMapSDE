// DebugFun.h: interface for the CDebugFun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGFUN_H__A56778EC_6497_42E9_A00D_A5E23F1F88F8__INCLUDED_)
#define AFX_DEBUGFUN_H__A56778EC_6497_42E9_A00D_A5E23F1F88F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#undef TRACE0
#undef TRACE1
#undef TRACE2
#define TRACE0(x) 	
#define TRACE1(x,y)
#define TRACE2(x,y,z)

#ifdef _DEBUG

#define DBG2SCR0(sz)                CDebugFun::DebugToScreenLine(_T("%s"), _T(sz))
#define DBG2SCR1(sz, p1)            CDebugFun::DebugToScreenLine(_T(sz), p1)
#define DBG2SCR2(sz, p1, p2)        CDebugFun::DebugToScreenLine(_T(sz), p1, p2)
#define DBG2SCR3(sz, p1, p2, p3)    CDebugFun::DebugToScreenLine(_T(sz), p1, p2, p3)
#define TRACK_FUNCTION(sz)          CTrackFuntion aTrack(_T(sz))
#define DBG2LOG0(sz)                CDebugFun::DebugToLog(_T("%s"), _T(sz))
#define DBG2LOG1(sz, p1)            CDebugFun::DebugToLog(_T(sz), p1)
#define DBG2LOG2(sz, p1, p2)        CDebugFun::DebugToLog(_T(sz), p1, p2)
#define DBG2LOG3(sz, p1, p2, p3)    CDebugFun::DebugToLog(_T(sz), p1, p2, p3)
#define MEMORYWATCH(sz,p1)          CMemoryWatch aMemoryWatch(_T(sz),p1)

#else   //_DEBUG

#define DBG2SCR0(sz)              
#define DBG2SCR1(sz, p1)          
#define DBG2SCR2(sz, p1, p2)      
#define DBG2SCR3(sz, p1, p2, p3) 
#define TRACK_FUNCTION(sz) 
#define DBG2LOG0(sz)                
#define DBG2LOG1(sz, p1)            
#define DBG2LOG2(sz, p1, p2)        
#define DBG2LOG3(sz, p1, p2, p3)    
#define MEMORYWATCH(sz,p1)     

#endif  //_DEBUG


//////////////////////////////////////////////////////////////////////////
//写日志文件
#define WATCH2LOG(sz)               CMsgWatch aMsgWatch(_T(sz))

#define WRITE2LOG0(sz)                CDebugFun::DebugToLog(_T("%s"), _T(sz))
#define WRITE2LOG1(sz, p1)            CDebugFun::DebugToLog(_T(sz), p1)
#define WRITE2LOG2(sz, p1, p2)        CDebugFun::DebugToLog(_T(sz), p1, p2)
#define WRITE2LOG3(sz, p1, p2, p3)    CDebugFun::DebugToLog(_T(sz), p1, p2, p3)
#define WRITE2LOG_TRACK_FUNCTION_(sz)          CTrackFuntion_Write2Log aTrack2Log(_T(sz))
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
class CDebugFun  
{
public:
    static void DebugToScreenLine(LPCTSTR lpszFormat,...);
    static void DebugToLog(LPCTSTR lpszFormat,...);
    //-------------------------------------------------------------    
    virtual ~CDebugFun();
protected:
    CDebugFun();
    static CDebugFun _instance;
    static BOOL InitialOperation();
    static BOOL ClearOperation();
    static void DebugToScreen(int x, int y, LPCTSTR lpsz);
};
//////////////////////////////////////////////////////////////////////////
class CMsgWatch  
{
public:
	CMsgWatch(LPCTSTR);
	~CMsgWatch();
    //-------------------------------------------------------------
protected:
    TCHAR m_szText[32];
	DWORD m_dwTickStart;
};
//////////////////////////////////////////////////////////////////////////
//这个类用以跟踪函数调用.
class CTrackFuntion
{
public:
    CTrackFuntion(LPCTSTR);
    ~CTrackFuntion();
protected:
    TCHAR m_szText[32];
    static int m_nPreSignCount;
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//这个类用以跟踪函数调用.
class CTrackFuntion_Write2Log
{
public:
    CTrackFuntion_Write2Log(LPCTSTR);
    ~CTrackFuntion_Write2Log();
protected:
    TCHAR m_szText[32];
    static int m_nPreSignCount;
};
//////////////////////////////////////////////////////////////////////////
//这个类用来监视内存变化
class CMemoryWatch  
{
public:
	CMemoryWatch(LPCTSTR,DWORD);
	~CMemoryWatch();
    //-------------------------------------------------------------
protected:
    TCHAR m_szText[32];
	MEMORYSTATUS m_MemoryStatus;
    DWORD m_dwDifferLimit;
};
//////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_DEBUGFUN_H__A56778EC_6497_42E9_A00D_A5E23F1F88F8__INCLUDED_)
