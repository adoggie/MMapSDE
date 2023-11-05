#ifndef _IGIS_MAPNETWORK_H
#define _IGIS_MAPNETWORK_H

#include "swBase.h"
#include "GIS_GeoMap.h"
#include "GIS_WayPoint.h"
#include "GIS_NetworkRoute.h"
#include "GIS_NetworkNode.h"

class IMemoryMng;
//////////////////////////////////////////////////////////////////////////
class IGIS_MapNetwork  
{
public:
	IGIS_MapNetwork();
	virtual ~IGIS_MapNetwork();

public:
	virtual BOOL LoadMapNetwork( ) = 0;
//	virtual BOOL LoadMapNetwork( swString strFName ) = 0;
	void SetNetworkRoute( CGIS_NetworkRoute *pNetworkRoute );
	CGIS_NetworkRoute* GetNetworkRoute( );
	void SetGeoMap( CGIS_GeoMap *pMap );
	virtual int CalculateNavigatePath(CPtrList* WayPointList ,CPtrList* pPathpointlist ) = 0;

protected:
	BOOL CalculateNetworkNode( CGIS_NetworkNode *pNode, CPtrList *pOList, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead );
	//�õ��ýڵ��ϵ�ǰ·�ε�Ȩֵ
	inline BOOL CalculateRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead );
	//���㳵������ΪС����ʱ�ýڵ��ϵ�ǰ·�ε�Ȩֵ
	inline BOOL CalculateCarRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead );
	//���㳵������Ϊ�󿨳�ʱ�ýڵ��ϵ�ǰ·�ε�Ȩֵ
	inline BOOL CalculateTruckRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//���㳵������Ϊ������ʱ�ýڵ��ϵ�ǰ·�ε�Ȩֵ
	inline BOOL CalculateBusRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//���㳵������Ϊ�Ȼ���/������ʱ�ýڵ��ϵ�ǰ·�ε�Ȩֵ
	inline BOOL CalculateEmergencyRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//���㳵������Ϊ���⳵ʱ�ýڵ��ϵ�ǰ·�ε�Ȩֵ
	inline BOOL CalculateTaxiRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//���㳵������Ϊ������ʱ�ýڵ��ϵ�ǰ·�ε�Ȩֵ
	inline BOOL CalculateDeliveryRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//���㳵������Ϊ���г�ʱ�ýڵ��ϵ�ǰ·�ε�Ȩֵ
	inline BOOL CalculateBicycleRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//���㳵������Ϊ����ʱ�ýڵ��ϵ�ǰ·�ε�Ȩֵ
	BOOL CalculatePedestrianRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	///�������в���ڵ�
	inline void InsertNode(CGIS_NetworkNode* pNode, CPtrList *pList, BOOL bHead = TRUE);


protected:
	CGIS_GeoMap *m_pMap;	//map
	CGIS_NetworkRoute *m_pNetworkRoute;

	double m_dbMaxDis;	//����յ���������

	ST_GEO_PTXY m_pRBuf[128];
	int			m_nRBNum;
    
public:
    //����·�ߵĳ���
    enum enCalculateScene
    {
        SCENE_NONE,//��Ч����
        SCENE_SHORT,//�ܽ�
        SCENE_NORMAL,//ͨ��
        SCENE_FAR,//��Զ
    };
protected:
	enCalculateScene m_eCalculateScene;	

public:
	static IMemoryMng* m_pMemMngOfBuddyData;

//////////////////////////////////////////////////////////////////////////
	
public:
	int CalculateStop();//ֹͣ����
protected:
	LONG m_bCalculateStopSign;

//////////////////////////////////////////////////////////////////////////

};

#endif 
