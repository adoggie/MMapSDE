// TempLayerInfo.cpp: implementation of the CTempLayerInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TempLayerInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempLayerInfo::CTempLayerInfo(int nID)
{
	m_nLayerID = nID;
}
CTempLayerInfo::CTempLayerInfo()
{

}
CTempLayerInfo::~CTempLayerInfo()
{

}

void CTempLayerInfo::SetLayerState(BOOL bState)
{

}

void CTempLayerInfo::SetMaxGRScale(double dbMaxGR)
{

}

void CTempLayerInfo::SetMinGRScale(double dbMinGR)
{

}

void CTempLayerInfo::SetMaxARScale(double dbMaxGR)
{

}

void CTempLayerInfo::SetMinARScale(double dbMinGR)
{

}

void CTempLayerInfo::SetIconID(int nIconID)
{
	m_nIconID = nIconID;
}
BOOL CTempLayerInfo::GetIconID(int& nIconID)
{
	nIconID = m_nIconID;
	return TRUE;
}
unsigned short CTempLayerInfo::GetLayerID()
{
	return m_nLayerID;
}

void CTempLayerInfo::SetLayerID(unsigned short  nLayerID)
{
	m_nLayerID = nLayerID;
}