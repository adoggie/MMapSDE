
#ifndef _GIS_PATHPOINT_NODEV2_H
#define _GIS_PATHPOINT_NODEV2_H

#include "swBase.h"

#include "GIS_PathpointNode.h"
#include "GIS_NetworkRut.h"

class CGIS_PathpointNodeV2 : public CGIS_PathpointNode  
{
public:
    struct stuRoadSectionInfo
    {
        long nLength;
        BYTE nType;
    };


public:
	CGIS_PathpointNodeV2();
	virtual ~CGIS_PathpointNodeV2();


    void SetPosOfPathPointList(POSITION pos);
    POSITION GetPosOfPathPointList();
    CGIS_NetworkRut::TURNINFO* GetTurnInfo();
    BOOL IfOnlyPassRoadSection();
    int GetPassTimeByMinute();
    long GetRoadSectionLength();

    stuRoadSectionInfo* GetNextRoadSectionInfo()
    {
        return &m_NextRoadSectionInfo;
    }
    void SetNextRoadSectionInfo(stuRoadSectionInfo* pInfo)
    {
        m_NextRoadSectionInfo.nLength = pInfo->nLength;
        m_NextRoadSectionInfo.nType = pInfo->nType;
    }
    void SetOnlyPassSign(BOOL bSign)
    {
        m_bOnlyPass = bSign;
    }
    
    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* GetLimitInfo()
    {
        return m_pLimitInfoGroup;
    }
    BOOL SetLimitInfo(CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup)
    {
        if(!pLimitInfoGroup)
            return FALSE;
        ASSERT(!m_pLimitInfoGroup);
        m_pLimitInfoGroup = new CGIS_NetworkRut::stuRoadSectionLimitInfoGroup;
        m_pLimitInfoGroup->byTurnCount = pLimitInfoGroup->byTurnCount;
        m_pLimitInfoGroup->pTurnInfoArray = new CGIS_NetworkRut::TURNINFO[pLimitInfoGroup->byTurnCount];
        for(int i = 0; i < pLimitInfoGroup->byTurnCount; i++)
        {
            CGIS_NetworkRut::TURNINFO* pTurnInfoSrc = &pLimitInfoGroup->pTurnInfoArray[i];
            CGIS_NetworkRut::TURNINFO* pTurnInfoDest = &m_pLimitInfoGroup->pTurnInfoArray[i];

            pTurnInfoDest->byToRouteIDCount = pTurnInfoSrc->byToRouteIDCount;
            pTurnInfoDest->pwToRutID = (WORD*)malloc(sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
            memcpy(pTurnInfoDest->pwToRutID,pTurnInfoSrc->pwToRutID,sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
            pTurnInfoDest->byStartTime = pTurnInfoSrc->byStartTime;
            pTurnInfoDest->byEndTime = pTurnInfoSrc->byEndTime;

        }
        return TRUE;
    }

protected:
    stuRoadSectionInfo m_NextRoadSectionInfo;//
    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup*  m_pLimitInfoGroup;//前一路段对下一路段的限行信息,如里没有时间限行则此指针为空.
    BOOL m_bOnlyPass;
    POSITION m_posOfPathPointList;
    
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////	

};

#endif 
