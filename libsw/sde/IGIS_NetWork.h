
#ifndef _IGIS_NETWORK_H
#define _IGIS_NETWORK_H

#include "swBase.h"

#include "IGIS_SDETool.h"
#include "GIS_NetworkRoute.h"
#include "IGIS_MapManager.h"


#ifdef _DEBUG
#include "GeoView.h"
#endif

#ifdef _DEBUG
#ifdef ANALYSE_DATA
#undef ANALYSE_DATA
#endif
#define ANALYSE_DATA
#endif

class IMemoryMng;
class CGIS_NetworkRut;
class CGIS_NetworkVtx;
class CGIS_NetworkRoadSectionV7;
class CGIS_NetworkVertexV7;


class swEXPORT IGIS_NetWork  : public IGIS_SDETool
{
public:
	IGIS_NetWork();
	virtual ~IGIS_NetWork();

public:
    /**
     * @brief ·�߹滮
     *
     * @param byCType-(0.������·�߹滮 1.ƫ��ʱ��·�߹滮 2.��·ʱ·�߹滮),
     *       ����byCTypeΪ1.2���������Ҫ���복�������
     *              
     * @return  1.����ɹ�
     *          0.·��������ж��     
     *         -1.����û��ƥ���·��         
     *         -2.·����ͨ     
     *         -3.�ļ�I/O�쳣     
     *         -4.�ڴ治��
     *         -5.���߹���,��������.
     *         -6.������������
     *         -7.���㱻�ⲿ��ֹ
    */	
	virtual int CalculateOptimalRoute( BYTE byCType, CGIS_WayPoint *pWPoint = NULL ) = 0;
	virtual int CalculateStop() = 0;
//////////////////////////////////////////////////////////////////////////
    //2007-04-14 ����¹���ŵĽ���.����˽ӿ�����pc�˵Ĳ���.
#ifndef _WIN32_WCE
public:
    virtual CGIS_NetworkRut* GetRoadSectionByBreadthIDAndRoadSectionID(int nBreadthID,int nRoadSectionID,int& nLayer) = 0;
    virtual CGIS_NetworkVtx* GetVertexByBreadthIDAndVertexID(int nBreadthID,int nRoadSectionID,int& nLayer) = 0;
    virtual CGIS_NetworkRoadSectionV7* GetTrunkRoadSectionByBreadthIDAndRoadSectionID(int nBreadthID,int nID) = 0;
    virtual CGIS_NetworkVertexV7* GetTrunkVertexByBreadthIDAndVertexID(CGIS_GeoMap* pMap,int nBreadthID,int nID) = 0;
#endif
public:
	void SetNetworkRoute( CGIS_NetworkRoute *pRoute );
	CGIS_NetworkRoute* GetNetworkRoute( );
	void SetMapManager( IGIS_MapManager *pMapManager );
protected:
	CGIS_NetworkRoute *m_pNetworkRoute;
	IGIS_MapManager *m_pMapManager;
public:
	static IMemoryMng* m_pMemMngOfBuddyData;
#ifdef _DEBUG
public:
	CGeoView* m_pMapView;
	//����ģ��״̬�Ĺ�ϵ,Ӧ�ò����Ե��ô˽ӿ�,ֻ����sde�ڲ�ʹ��
	void SetMapViewSrc(CGeoView* pView)
	{
		m_pMapView = pView;
	}
	CGeoView* GetMapView()
	{
		 return m_pMapView;
	}
	
#ifdef ANALYSE_DATA
    DWORD m_dwMaxHeapUsedSize;
    DWORD GetMaxHeapUsedSize(){return m_dwMaxHeapUsedSize;};
#endif
    
#endif  
//////////////////////////////////////////////////////////////////////////

};

#endif 
