// GIS_PathpointNode.cpp: implementation of the CGIS_PathpointNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_PathpointNode.h"
#include "GIS_NetworkNodeRut.h"
#include "./MemoryMngLib/IMemoryMng.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_PathpointNode::CGIS_PathpointNode()
{
	m_enPathpointType = EN_PATHPOINTTYPE_NODE;
	m_pNodeRutList = new CPtrList;
	m_szRutAnno = NULL;
	m_nAnnoNum = 0;
	m_nTurnAngle = 0;
	m_byRType = 6;
	m_byRutType = 1;
	m_nRMOrd = 0;
	
//	m_pVRut = new ST_VOICE_RUT;			//语音节点下一路段信息
//	m_pVNode = new ST_VOICE_NODE;		//语音节点的信息
#ifdef MEMORY_SELF_MANAGE
	m_pVRut		= (ST_VOICE_RUT*)m_pMemMngOfBuddyData->newmalloc( sizeof(ST_VOICE_RUT));		//路段信息
	m_pVNode	= (ST_VOICE_NODE*)m_pMemMngOfBuddyData->newmalloc( sizeof(ST_VOICE_NODE));

#else
	m_pVRut		= (ST_VOICE_RUT*)malloc( sizeof(ST_VOICE_RUT));		//路段信息
	m_pVNode	= (ST_VOICE_NODE*)malloc( sizeof(ST_VOICE_NODE));

#endif
	m_pVRut->Init(); 
	m_pVNode->Init();


    m_eCrossStatus = STATUS_NONE;
    m_nImgID = -1;
    m_nArrowID = -1;

    m_pSignInfo = NULL;

    m_byLaneNum = 0;
    m_pbyLaneDir = NULL;
    m_pstLaneInfo = NULL;

}

CGIS_PathpointNode::~CGIS_PathpointNode()
{
#ifdef MEMORY_SELF_MANAGE
	if( m_pVRut )
	{
		m_pMemMngOfBuddyData->newfree(m_pVRut);
		m_pVRut = NULL;
	}
	if( m_pVNode )
	{
		m_pMemMngOfBuddyData->newfree(m_pVNode);
		m_pVNode = NULL;
	}	
#else
	if( m_pVRut )
	{
		free(m_pVRut);
		m_pVRut = NULL;
	}
	if( m_pVNode )
	{
		free(m_pVNode);
		m_pVNode = NULL;
	}	
    
#endif
	if( m_szRutAnno )
	{
		delete[] m_szRutAnno;
		m_szRutAnno = NULL;
	}

	if( m_pNodeRutList )
	{
		CGIS_NetworkNodeRut *pNNRut;
		POSITION pos = m_pNodeRutList->GetHeadPosition();
		while( pos )
		{
			pNNRut = (CGIS_NetworkNodeRut*)m_pNodeRutList->GetNext( pos );
			if( pNNRut )
				delete pNNRut;
		}
		m_pNodeRutList->RemoveAll();
		delete m_pNodeRutList;
	}

    if(m_pSignInfo)
    {
        //if(m_pSignInfo->byNameLen > 0)
        //{
        //    delete []m_pSignInfo->pszName;
        //}
        delete m_pSignInfo;
    }

    if(m_pbyLaneDir)
    {
        free(m_pbyLaneDir);
        m_pbyLaneDir = NULL;
    }

    if(m_pstLaneInfo)
    {
        if(m_pstLaneInfo->pbyLaneDir)
        {
            delete m_pstLaneInfo->pbyLaneDir;
            m_pstLaneInfo->pbyLaneDir = NULL;
        }
        if(m_pstLaneInfo->pbyOutLaneDir )
        {
            delete m_pstLaneInfo->pbyOutLaneDir;
            m_pstLaneInfo->pbyOutLaneDir = NULL;
        }
        delete m_pstLaneInfo;
        m_pstLaneInfo = NULL;
    }
    
	m_pNodeRutList = NULL;
	m_pVRut = NULL;
	m_pVNode = NULL;
	m_nAnnoNum = 0;
	m_nTurnAngle = 0;
}
CPtrList* CGIS_PathpointNode::GetNodeRutList( )
{
	return m_pNodeRutList;
}
void CGIS_PathpointNode::SetNextRutID( int nNextRutID )
{
	m_nNextRutID = nNextRutID;
}
int CGIS_PathpointNode::GetNextRutID( )
{
	return m_nNextRutID;
}
void CGIS_PathpointNode::SetRutAnno( char *szAnno )
{
	m_szRutAnno = szAnno;
}
swString CGIS_PathpointNode::GetRutAnno( )
{
	return swString::FromUTF8(m_szRutAnno);
}
BYTE CGIS_PathpointNode::GetRutAnnoNum( )
{
	return m_nAnnoNum;
}
void CGIS_PathpointNode::SetRutAnnoNum( BYTE nAnnoNum )
{
	m_nAnnoNum = nAnnoNum;
}
void CGIS_PathpointNode::SetNodeVtxID( int nNodeVtxID )
{
	m_nNodeVtxID = nNodeVtxID;
}
int CGIS_PathpointNode::GetNodeVtxID( )
{
	return m_nNodeVtxID;
}
ST_VOICE_RUT* CGIS_PathpointNode::GetVRutInfo( )
{
	ASSERT( m_pVRut );
	return m_pVRut;
}
ST_VOICE_NODE* CGIS_PathpointNode::GetVNodeInfo( )
{
	ASSERT( m_pVNode );
	return m_pVNode;
}
void CGIS_PathpointNode::SetTurnAngle( int nTurnAngle )
{
	m_nTurnAngle = nTurnAngle;
}
int  CGIS_PathpointNode::GetTurnAngle( )
{
	return m_nTurnAngle;
}
void CGIS_PathpointNode::SetRType( BYTE byRType )
{
	m_byRType = byRType;
}
BYTE CGIS_PathpointNode::GetRType( )
{
	return m_byRType;
}
void CGIS_PathpointNode::SetRutType( BYTE byRutType )
{
	m_byRutType = byRutType;
}
BYTE CGIS_PathpointNode::GetRutType( )
{
	return m_byRutType;
}
void CGIS_PathpointNode::SetPBreadthID( int nBID )
{
	m_nBID = nBID;
}
int CGIS_PathpointNode::GetPBreadthID( )
{
	return m_nBID;
}
void CGIS_PathpointNode::CreateCRect(ST_GEO_PTXY pPtList[], int nPtNum ,float dExtendX,float dExtendY )
{
	ASSERT( nPtNum > 0 );
	if( nPtNum == 1 )
	{
		m_rtCross.dLeft = m_rtCross.dRight = pPtList[0].dx;
		m_rtCross.dTop = m_rtCross.dBottom = pPtList[0].dy;
	}
	else
	{
		CalcRect( pPtList, nPtNum, m_rtCross );
		float dDetaX = m_rtCross.dRight     -   m_rtCross.dLeft;
		float dDetaY = m_rtCross.dBottom    -   m_rtCross.dTop;
      
		dExtendX = dDetaX*0.50;
		dExtendY = dDetaY*0.50;

		if( dExtendX > 0.0003 )
			dExtendX = 0.0003;
		else if( dExtendX < 0.00004 )
			dExtendX = 0.00004;

		if( dExtendY > 0.0003 )
			dExtendY = 0.0003;
		else if( dExtendY < 0.00004 )
			dExtendY = 0.00004;
	}
	m_rtCross.dLeft -= dExtendX;
	m_rtCross.dRight += dExtendX;
	m_rtCross.dTop -= dExtendY;
	m_rtCross.dBottom += dExtendY;
}
void CGIS_PathpointNode::SetCRect( ST_GEO_RECT rtCross )
{
	memcpy( &m_rtCross, &rtCross, sizeof(ST_GEO_RECT) );
}
ST_GEO_RECT CGIS_PathpointNode::GetCRect( )
{
	return m_rtCross;
}
void CGIS_PathpointNode::SetRMOrder( short nRMOrd )
{
	m_nRMOrd = nRMOrd;
}
short CGIS_PathpointNode::GetRMOrder( )
{
	return m_nRMOrd;
}
BOOL CGIS_PathpointNode::SetLaneInfoDirect(ST_LANDINFO* pLaneInfo)
{
    ////////////////////////////////////
    //旧的数据空间一定不存在
    ASSERT(!m_pstLaneInfo);
    m_pstLaneInfo = pLaneInfo;
     
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_PathpointNode::DumpState() const	
{  
    TRACE0("----------------\n");
    CGIS_Pathpoint::DumpState();

    TRACE1("\tNodeVtxID:%d\n",m_nNodeVtxID);
    TRACE1("\tNextRutID:%d\n",m_nNextRutID);
    TRACE1("\tNextRutName:%s\n",swString(m_szRutAnno));
    TRACE1("\tRoundMobileOrder:%d\n",m_nRMOrd);
    TRACE1("\tNextRutType:%d\n",m_byRType);
    TRACE1("\tNextRutType:%d\n",m_byRType);
    TRACE1("\tNextRutType2:%d\n",m_byRutType);
    TRACE0("----------------\n");

//	int				m_nTurnAngle;			//路段的夹角
//	CPtrList		*m_pNodeRutList;		//该节点上其余路段信息链表
//	ST_VOICE_RUT	*m_pVRut;				//语音节点下一路段信息
//	ST_VOICE_NODE	*m_pVNode;				//语音节点的信息
// 
    Sleep(100);
}

void CGIS_PathpointNode::AssertValid() const	
{   


}
#endif
//////////////////////////////////////////////////////////////////////////