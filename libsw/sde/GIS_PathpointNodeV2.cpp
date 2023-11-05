// GIS_PathpointNodeV2.cpp: implementation of the CGIS_PathpointNodeV2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_PathpointNodeV2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_PathpointNodeV2::CGIS_PathpointNodeV2()
{
    m_pLimitInfoGroup = NULL;
    m_bOnlyPass = FALSE;
    //m_nPassMinute = 0;
    m_NextRoadSectionInfo.nLength = 0;

    m_posOfPathPointList = NULL;
    memset(&m_NextRoadSectionInfo,0,sizeof(stuRoadSectionInfo));

}

CGIS_PathpointNodeV2::~CGIS_PathpointNodeV2()
{
    if(m_pLimitInfoGroup)
    {
        if(m_pLimitInfoGroup->pTurnInfoArray)
        {
            for(int i = 0; i < m_pLimitInfoGroup->byTurnCount; i++)
            {
                CGIS_NetworkRut::TURNINFO* pTurnInfo = &m_pLimitInfoGroup->pTurnInfoArray[i];
                if(pTurnInfo->byToRouteIDCount)
                {
                    free(pTurnInfo->pwToRutID);
                }
            }
            
            delete [] m_pLimitInfoGroup->pTurnInfoArray;
            m_pLimitInfoGroup->pTurnInfoArray = NULL;
        }
        delete m_pLimitInfoGroup;
        m_pLimitInfoGroup = NULL;
    }
}
CGIS_NetworkRut::TURNINFO* CGIS_PathpointNodeV2::GetTurnInfo()
{
    return m_pLimitInfoGroup->pTurnInfoArray;
}
BOOL CGIS_PathpointNodeV2::IfOnlyPassRoadSection()
{
    return m_bOnlyPass;
}
int CGIS_PathpointNodeV2::GetPassTimeByMinute()
{
    return 0;
}

long CGIS_PathpointNodeV2::GetRoadSectionLength()
{
    return m_NextRoadSectionInfo.nLength;
}

POSITION CGIS_PathpointNodeV2::GetPosOfPathPointList()
{
    return m_posOfPathPointList;
}
void CGIS_PathpointNodeV2::SetPosOfPathPointList(POSITION pos)
{
    m_posOfPathPointList = pos;
}


//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_PathpointNodeV2::DumpState() const	
{  
}

void CGIS_PathpointNodeV2::AssertValid() const	
{   
//    ASSERT(m_NextRoadSectionInfo.nLength > 2);
    ASSERT(m_NextRoadSectionInfo.nType >= 0);
    ASSERT(m_NextRoadSectionInfo.nType <= 6);  

    if(m_pLimitInfoGroup)
    {
        ASSERTVALID(m_pLimitInfoGroup);
    }
}
#endif
//////////////////////////////////////////////////////////////////////////