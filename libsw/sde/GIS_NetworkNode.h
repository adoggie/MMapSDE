
#ifndef _GIS_NETWORK_NODE_H
#define _GIS_NETWORK_NODE_H

#include "swBase.h"
#include "GIS_NetworkRut.h"
#include "GIS_NetworkVtx.h"


class IMemoryMng;
class IMemoryMng2;


class CGIS_NetworkNode  
{
public:
	CGIS_NetworkNode();
	~CGIS_NetworkNode();

public:
	CGIS_NetworkRut *m_pRut;
	CGIS_NetworkVtx *m_pVtx;
	float m_g, m_f;
	BYTE	m_nLRNum;		//高等级道路的个数
	BYTE	m_bHaveJCT;		//是否有交流道
	CGIS_NetworkNode *m_pParent;

//////////////////////////////////////////////////////////////////////////
//lijun add for memmory manage
#ifdef MEMORY_SELF_MANAGE
public:
	void* operator new(size_t size);
	void operator delete (void*,size_t);

    static IMemoryMng* m_pMemMngOfSelf;
#endif	
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////	
};

#endif 
