// TempMapInfo.cpp: implementation of the CTempMapInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TempMapInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempMapInfo::CTempMapInfo()
{
	m_strMFPath.Empty();
}

CTempMapInfo::~CTempMapInfo()
{

}

void CTempMapInfo::SetMapFilePath(CString strMFPath)
{
	m_strMFPath = strMFPath;
}

void CTempMapInfo::SetMapLayNum(int nLNum)
{
	m_nLayerNum = nLNum;
}

unsigned short CTempMapInfo::GetMapID( )
{
	return m_nMapID;
}

void CTempMapInfo::SetMapID( unsigned short nMapID )
{
	m_nMapID = nMapID;
}

ST_GEO_PTXY* CTempMapInfo::GetMapBndList( )
{
	return m_pBndPtList;
}
void CTempMapInfo::SetMapBndList( ST_GEO_PTXY *pBndPtList )
{
	m_pBndPtList = pBndPtList;
}
unsigned short CTempMapInfo::GetMapBndPtNum( )
{
	return m_nBndPtNum;
}
void CTempMapInfo::SetMapBndPtNum( unsigned short nPtNum )
{
	m_nBndPtNum = nPtNum;
}

ST_GEO_RECT CTempMapInfo::GetMapRect( )
{
	return m_rtMapRect;
}
void CTempMapInfo::SetMapRect( ST_GEO_RECT rtMapRect )
{
	memcpy( &m_rtMapRect, &rtMapRect, sizeof(ST_GEO_RECT) );
}