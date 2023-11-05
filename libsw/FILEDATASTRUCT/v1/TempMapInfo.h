// TempMapInfo.h: interface for the CTempMapInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPMAPINFO_H__601CE231_F842_451D_8FEE_1113B0F5FE4B__INCLUDED_)
#define AFX_TEMPMAPINFO_H__601CE231_F842_451D_8FEE_1113B0F5FE4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoGlobe.h"

class CTempMapInfo  
{

public:
	void SetMapLayNum( int nLNum );
	void SetMapFilePath ( CString strMFPath );

	unsigned short GetMapID( );
	void SetMapID( unsigned short nMapID );

	ST_GEO_PTXY* GetMapBndList( );
	void SetMapBndList( ST_GEO_PTXY *pBndPtList );
	unsigned short GetMapBndPtNum( );
	void SetMapBndPtNum( unsigned short nPtNum );

	ST_GEO_RECT GetMapRect( );
	void SetMapRect( ST_GEO_RECT rtMapRect );

	CTempMapInfo();
	virtual ~CTempMapInfo();

//////////////////////////////////////////////////////////////////////////
protected:
	int		m_nLayerNum;
	CString m_strMFPath;
	unsigned short m_nMapID;		//地图ID
	
	ST_GEO_PTXY*	m_pBndPtList;		//地图边界坐标点链表
	unsigned short	m_nBndPtNum;		//地图边界坐标点数目

	ST_GEO_RECT m_rtMapRect;
};

#endif // !defined(AFX_TEMPMAPINFO_H__601CE231_F842_451D_8FEE_1113B0F5FE4B__INCLUDED_)
