// IMemoryMng3.h: interface for the IMemoryMng3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMEMORYMNG3_H__AE409A8C_E536_490D_B6A4_990A79BDC276__INCLUDED_)
#define AFX_IMEMORYMNG3_H__AE409A8C_E536_490D_B6A4_990A79BDC276__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMemoryMng.h"

class IMemoryMng3  : public IMemoryMng
{
public:
	IMemoryMng3(){};
	virtual ~IMemoryMng3(){};
    
    virtual BOOL InitialHeap(unsigned long nHeapSize,unsigned long nCellSize) = 0;
	virtual BOOL InitialHeap(void* pHeap,unsigned long nHeapSize,unsigned long nCellSize) = 0;
//////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_DATA
    virtual DWORD GetMaxCellCount() = 0;	
#endif
};

#endif // !defined(AFX_IMEMORYMNG3_H__AE409A8C_E536_490D_B6A4_990A79BDC276__INCLUDED_)
