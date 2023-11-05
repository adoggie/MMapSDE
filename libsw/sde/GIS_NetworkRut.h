
#ifndef _GIS_NETWORK_RUT_H
#define _GIS_NETWORK_RUT_H

#include "swBase.h"

#include "GIS_NetworkVtx.h"


class IMemoryMng;
class IMemoryMng2;

class swEXPORT CGIS_NetworkRut  
{
public:
    typedef struct _tagTurnInfo 
{
	BYTE		byToRouteIDCount;
	WORD*		pwToRutID;
	BYTE		byStartTime;
	BYTE		byEndTime;
	_tagTurnInfo()
	{
		pwToRutID = NULL;
	};
	~_tagTurnInfo()
	{
		release();		
	}
	void release()
	{
		if (pwToRutID != NULL)
		{
			//delete [] pwToRutID;
			pwToRutID = NULL;
		}
	};
    void Format(swString& strText)
    {
        swString strDest = _T("");
        swString strTemp = _T("");
        strDest.Format(_T("%d["),byToRouteIDCount);
        for(int i = 0; i < byToRouteIDCount; i++)
        {
            strTemp.Format(_T("%d"),pwToRutID[i]);
            strDest += strTemp;
        }
        strDest += _T("]");;
        strText = strDest;
        
    }

}TURNINFO, *LPTURNINFO;

struct stuRoadSectionLimitInfoGroup
{
    BYTE		byTurnCount;
    TURNINFO *	pTurnInfoArray;
    
    BOOL IfLimit(unsigned short nID,unsigned char nTime)
    {
        for(int i = 0; i < byTurnCount; i++)
        {
            if(pTurnInfoArray[i].pwToRutID[pTurnInfoArray[i].byToRouteIDCount-1] == nID)
            {
                if(pTurnInfoArray[i].byStartTime == 0xff)
                {
                    return TRUE;
                }
                else
                {
                    if(nTime >= pTurnInfoArray[i].byStartTime && nTime <= pTurnInfoArray[i].byEndTime)
                        return TRUE;
                }
            }
        }
        return FALSE;
    }
    BOOL IfAbsoluteLimit(unsigned short nID)
    {
        for(int i = 0; i < byTurnCount; i++)
        {
            if(pTurnInfoArray[i].byStartTime == 0xff && pTurnInfoArray[i].pwToRutID[pTurnInfoArray[i].byToRouteIDCount-1] == nID)
            {
                return TRUE;
            }
        }
        return FALSE;
    }
    void Format(int nTimeZone,swString& strText);
    
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const
    {
        ASSERT(byTurnCount >= 1);
        ASSERT(byTurnCount <=20);
        for(int i = 0; i < byTurnCount; i++)
        {
            CGIS_NetworkRut::TURNINFO* pTurnInfo = &pTurnInfoArray[i];
            ASSERT(pTurnInfo->byToRouteIDCount <= 10);
            if(pTurnInfo->byToRouteIDCount)
            {
                ASSERT(pTurnInfo->pwToRutID);
            }
            if(pTurnInfo->byStartTime != 0xff)
            {
                ASSERT(pTurnInfo->byStartTime > 0);
                ASSERT(pTurnInfo->byStartTime < 144);
            }
            if(pTurnInfo->byEndTime != 0xff)
            {
                ASSERT(pTurnInfo->byEndTime > 0);
                ASSERT(pTurnInfo->byEndTime < 144);
            }
            
        }
    }
#endif
//////////////////////////////////////////////////////////////////////////	
};

//定义道路的禁行信息
struct stuRoadSectionLimitInfo
{
    stuRoadSectionLimitInfoGroup sGroup;
    stuRoadSectionLimitInfoGroup eGroup;

    
    stuRoadSectionLimitInfo()
    {
        memset(this,0,sizeof(struct stuRoadSectionLimitInfo));
    }
    

    stuRoadSectionLimitInfoGroup* GetGroup(BOOL bHead)
    {
        if(bHead)
        {
            if(sGroup.byTurnCount == 0)
                return NULL;
            else
                return &sGroup;
        }
        else
        {
            if(eGroup.byTurnCount == 0)
                return NULL;
            else
                return &eGroup;
        }
        ASSERT(FALSE);
    }
    void Format(BOOL bHead,int nTimeZone,swString& strText);
    
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() ;
#endif
//////////////////////////////////////////////////////////////////////////	

};


public:
	CGIS_NetworkRut();
	~CGIS_NetworkRut();
    CGIS_NetworkRut* CreateCloneObject();
    void DestoryCloneObject();

	void Clear( );
public:
	void SetRutInfo( unsigned short nRID = 0, BYTE byDirtect = 0, BYTE byType = 6, BYTE byCharge = 0,
			BYTE byLNum = 0,	BYTE bySpeed = 6, BYTE byRLevel = 0, BYTE byRType = 1,
			float dRLength = 0.0, unsigned short nVStart = 0, unsigned short nVEnd = 0, int nRBID = 0);
	void SetRutLimitInfo( BYTE byLimit = 0, unsigned short *pLRList = NULL );
	void SetRutRLimitInfo( BYTE byLimit = 0, unsigned short *pLRList = NULL );
	void SetRutVirtualInfo( BYTE byVirtual = 0, unsigned short *pVORList = NULL, int *pVIDs = NULL );
	int GetRutID( );
	BYTE GetRutDirection( );
	BYTE GetType( );
	BYTE GetRutCharge( );
	BYTE GetRutLaneNum();
	BYTE GetRutSpeed( );
	BYTE GetRutType( );
	BYTE GetRutLevel( );
	float GetRutLength( );
	unsigned short GetRutStartVtxID( );
	unsigned short GetRutEndVtxID( );
	int GetRBreadthID( );


	void SetLimitInfoDirect(stuRoadSectionLimitInfo* pInfo)
    {
        m_pRoadSectionLimitInfo = pInfo;
    }
    void SetLimitInfo(stuRoadSectionLimitInfo* pInfo);
    stuRoadSectionLimitInfo* GetLimitInfo()
    {
        return m_pRoadSectionLimitInfo;
    }

private:
	unsigned short	m_nRutID;	//路段ID
	BYTE	m_byDirection;		//路段单双行标志
								//(0表示双行，1表示end到start,2表示start到end,3表示双向禁行)
	BYTE	m_byType;			//道路类别:(0--高速，1--都市高速路，2--国道，3--主要地方道&轮渡
									//4--一般县道，5--市镇村道，6--其他道路）
	BYTE	m_byCharge;			//收费:0--不收费1--双向收费2--正向收费3--反向收费
//	BYTE	m_byLaneNum;			//路段车道数:无数据的根据道路类别有缺省值
//	BYTE	m_bySpeed;			//速度级别:根据道路类别有缺省速度
	BYTE	m_byRutLevel;		//道路功能级别:5--一级道路4--二级道路3--三级道路
									//2--四级道路1--五级道路0--六级道路
	BYTE	m_byRutType;		//道路类型:0--环岛1--未采集属性2--上下线分离3--两条高速路之间的道路
									//4--交叉路口内的道路5--连接高速路和普通道路的道路6--停车区
									//7--服务区15--隧道
    //////////////////////////////////////////////////////////////////////////
    unsigned char m_nUsedStatus;//(FALSE-使用 TRUE-未使用)
    //////////////////////////////////////////////////////////////////////////
	float	m_dRLength;			//路段长度
	unsigned short m_nVtxStartID;//路段起始节点ID
	unsigned short m_nVtxEndID;	//路段尾节点ID
	int		m_nRBID;		//路段所在图幅ID
	
//	BYTE	m_byLimit;			//是否存在交通限制:0-没有交通限制;非0- 有交通限制
//	unsigned short *m_pLRList;	//禁止通行的道路id
//	BYTE	m_byRLimit;			//是否反向存在交通限制:0-没有交通限制;非0- 有交通限制
//	unsigned short *m_pRLRList;	//反向禁止通行的道路id
//	ST_GEO_RVDRAFT *m_pRVInfo;	//道路虚拟图信息
//	BYTE	m_byVirtual;		//是否存在虚拟图:0- 没有虚拟图;非0- 有虚拟图
//	unsigned short *m_pVORList;	//出去的道路ID数组
//	int		*m_pVIDs;			//虚拟图编号数组
    stuRoadSectionLimitInfo* m_pRoadSectionLimitInfo;
      
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//lijun add for memmory manage
#ifdef MEMORY_SELF_MANAGE
public:
	void* operator new(size_t size);
	void operator delete (void*,size_t);

    static IMemoryMng* m_pMemMngOfSelf;
    static IMemoryMng* m_pMemMngOfBuddyData;
#endif	
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////	
public:
    void SetUsedStatus(unsigned char nStatus)
    {
        m_nUsedStatus = nStatus;
    }
    unsigned char GetUsedStatus()
    {
        return m_nUsedStatus ;
    } 
};

#endif 
