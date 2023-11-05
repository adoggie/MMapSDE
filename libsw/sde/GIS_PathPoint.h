
#ifndef _GIS_PATHPOINT_H
#define _GIS_PATHPOINT_H

#include "swBase.h"

class IMemoryMng;



class swEXPORT CGIS_Pathpoint  
{
public:
	CGIS_Pathpoint();
	virtual ~CGIS_Pathpoint();
	
	void SetPathpointType( EnPathpointType enPathpointType );
	EnPathpointType GetPathpointType( );
	void SetPathpointPosition( ST_GEO_PTXY ptPos );
	ST_GEO_PTXY GetPathpointPosition( );
	
	void CalcDis2Next( ST_GEO_PTXY ptPos );
	float GetDis2Next( );
protected:
	ST_GEO_PTXY m_ptPos;	//位置坐标
	EnPathpointType m_enPathpointType;	//类型
	float m_dDisNext;
//////////////////////////////////////////////////////////////////////////
//lijun add for memmory manage
#ifdef MEMORY_SELF_MANAGE
public:
	void* operator new(size_t size);
	void operator delete (void*,size_t);

protected:
    static IMemoryMng* m_pMemMngOfSelf;
	static IMemoryMng* m_pMemMngOfPathpointNode;
	static IMemoryMng* m_pMemMngOfBuddyData;

#endif	
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
public:
	static	int GetNowObjectCount();
protected:
	static unsigned long m_nObjectCount;
#endif


#ifdef _DEBUG
public:
	virtual void DumpState() const;		
	virtual void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////	
};

#endif 
