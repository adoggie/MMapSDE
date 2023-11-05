
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

//�����·�Ľ�����Ϣ
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
	unsigned short	m_nRutID;	//·��ID
	BYTE	m_byDirection;		//·�ε�˫�б�־
								//(0��ʾ˫�У�1��ʾend��start,2��ʾstart��end,3��ʾ˫�����)
	BYTE	m_byType;			//��·���:(0--���٣�1--���и���·��2--������3--��Ҫ�ط���&�ֶ�
									//4--һ���ص���5--��������6--������·��
	BYTE	m_byCharge;			//�շ�:0--���շ�1--˫���շ�2--�����շ�3--�����շ�
//	BYTE	m_byLaneNum;			//·�γ�����:�����ݵĸ��ݵ�·�����ȱʡֵ
//	BYTE	m_bySpeed;			//�ٶȼ���:���ݵ�·�����ȱʡ�ٶ�
	BYTE	m_byRutLevel;		//��·���ܼ���:5--һ����·4--������·3--������·
									//2--�ļ���·1--�弶��·0--������·
	BYTE	m_byRutType;		//��·����:0--����1--δ�ɼ�����2--�����߷���3--��������·֮��ĵ�·
									//4--����·���ڵĵ�·5--���Ӹ���·����ͨ��·�ĵ�·6--ͣ����
									//7--������15--���
    //////////////////////////////////////////////////////////////////////////
    unsigned char m_nUsedStatus;//(FALSE-ʹ�� TRUE-δʹ��)
    //////////////////////////////////////////////////////////////////////////
	float	m_dRLength;			//·�γ���
	unsigned short m_nVtxStartID;//·����ʼ�ڵ�ID
	unsigned short m_nVtxEndID;	//·��β�ڵ�ID
	int		m_nRBID;		//·������ͼ��ID
	
//	BYTE	m_byLimit;			//�Ƿ���ڽ�ͨ����:0-û�н�ͨ����;��0- �н�ͨ����
//	unsigned short *m_pLRList;	//��ֹͨ�еĵ�·id
//	BYTE	m_byRLimit;			//�Ƿ�����ڽ�ͨ����:0-û�н�ͨ����;��0- �н�ͨ����
//	unsigned short *m_pRLRList;	//�����ֹͨ�еĵ�·id
//	ST_GEO_RVDRAFT *m_pRVInfo;	//��·����ͼ��Ϣ
//	BYTE	m_byVirtual;		//�Ƿ��������ͼ:0- û������ͼ;��0- ������ͼ
//	unsigned short *m_pVORList;	//��ȥ�ĵ�·ID����
//	int		*m_pVIDs;			//����ͼ�������
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
