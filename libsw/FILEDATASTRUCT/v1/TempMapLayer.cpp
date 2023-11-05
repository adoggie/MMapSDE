// TempMapLayer.cpp: implementation of the CTempMapLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TempMapLayer.h"
#include "TempLayerInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempMapLayer::CTempMapLayer(CTempLayerInfo* pInfo)
{
	m_pInfo = pInfo;
}

CTempMapLayer::~CTempMapLayer()
{
	if(m_pInfo)
	{
		delete m_pInfo;
		m_pInfo = NULL;
	}
}

void CTempMapLayer::InitLBCount( int nMaxNum )
{
	if( m_pBLCount )
		delete []m_pBLCount;

	m_pBLCount = new unsigned char[nMaxNum+1];
	memset( m_pBLCount, 0, sizeof(unsigned char)*(nMaxNum+1) );

}