// IGIS_MapNetwork.cpp: implementation of the IGIS_MapNetwork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IGIS_MapNetwork.h"

//////////////////////////////////////////////////////////////////////////
const float dDRate[] = {3.0, 2.5, 2.25, 2.0, 1.5, 1.2};//��·����Ȩֵ
#define CALCULATE_LENGTH_VALUE_BY_SCENE(len,m_eCalculateScene,nRoadSectionLevel) {\
    switch(m_eCalculateScene)\
    {\
    case SCENE_SHORT:\
		len *= 0.3;\
        break;\
    case SCENE_NORMAL:\
        break;\
    case SCENE_FAR:\
		len *= dDRate[(nRoadSectionLevel)];\
        break;\
    default:\
        ASSERT(FALSE);\
        break;\
    }\
}
#define CALCULATE_ANGLE_VALUE_BY_SCENE(fAngleValue,m_eCalculateScene) {\
    switch(m_eCalculateScene)\
    {\
    case SCENE_SHORT:\
		fAngleValue *= 0.1;\
        break;\
    case SCENE_NORMAL:\
        break;\
    case SCENE_FAR:\
        break;\
    default:\
        ASSERT(FALSE);\
        break;\
    }\
}
//////////////////////////////////////////////////////////////////////////
IMemoryMng* IGIS_MapNetwork::m_pMemMngOfBuddyData = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IGIS_MapNetwork::IGIS_MapNetwork()
{
	m_pNetworkRoute = NULL;
	m_pMap = NULL;
	m_dbMaxDis = 100000000.0;
	m_nRBNum = 0;

	m_bCalculateStopSign = 0;
    m_eCalculateScene = SCENE_NONE;

}

IGIS_MapNetwork::~IGIS_MapNetwork()
{

}
void IGIS_MapNetwork::SetNetworkRoute( CGIS_NetworkRoute *pNetworkRoute )
{
	m_pNetworkRoute = pNetworkRoute;
}
CGIS_NetworkRoute* IGIS_MapNetwork::GetNetworkRoute( )
{
	return m_pNetworkRoute;
}
void IGIS_MapNetwork::SetGeoMap( CGIS_GeoMap *pMap )
{
	m_pMap = pMap;
}
void IGIS_MapNetwork::InsertNode(CGIS_NetworkNode* pNode,CPtrList *pList, BOOL bHead )
{
	if( bHead )
	{
		//˳�����
		POSITION oldPos,pos = pList->GetHeadPosition();
		
		CGIS_NetworkNode *pRtnNode = NULL;
		while(pos != NULL)
		{
			oldPos = pos;
			pRtnNode = (CGIS_NetworkNode*)pList->GetNext(pos);
			if(pNode->m_f < pRtnNode->m_f)
			{
				pNode->m_pVtx->GetVtxSInfo()->posOHead = pList->InsertBefore(oldPos,pNode);
				return;
			}
		}

		pNode->m_pVtx->GetVtxSInfo()->posOHead = pList->AddTail(pNode);
	}
	else
	{
		//˳�����
		POSITION oldPos,pos = pList->GetHeadPosition();
		
		CGIS_NetworkNode *pRtnNode = NULL;
		while(pos != NULL)
		{
			oldPos = pos;
			pRtnNode = (CGIS_NetworkNode*)pList->GetNext(pos);
			if(pNode->m_f < pRtnNode->m_f)
			{
				pNode->m_pVtx->GetVtxSInfo()->posOEnd = pList->InsertBefore(oldPos,pNode);
				return;
			}
		}

		pNode->m_pVtx->GetVtxSInfo()->posOEnd = pList->AddTail(pNode);
	}
}
BOOL IGIS_MapNetwork::CalculateNetworkNode( CGIS_NetworkNode *pNode, CPtrList *pOList, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead )
{
    ASSERT(m_eCalculateScene != SCENE_NONE);
	if( !CalculateRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead ) )
	{
		delete pNode;
		return FALSE;
	}
	CGIS_NetworkNode *pOldNode = NULL;
	if( bHead )
	{
		if(pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch == 1)
		{
			//���뵽OPEN��
			if( pNode->m_pVtx->GetVtxSInfo()->posOHead )
			{
				pOldNode = (CGIS_NetworkNode*)pOList->GetAt(pNode->m_pVtx->GetVtxSInfo()->posOHead);
				if(pNode->m_g < pOldNode->m_g)
				{
					pOList->RemoveAt(pOldNode->m_pVtx->GetVtxSInfo()->posOHead);
					delete pOldNode;
					InsertNode(pNode,pOList);
				}else
				{
					delete pNode;
				}
			}
			else
			{
				InsertNode(pNode,pOList);
				pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = 1;
			}
		}
		else	if(pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch == 0)
		{
			//���뵽OPEN��
			InsertNode(pNode,pOList);
			pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = 1;

		}else
		{
			delete pNode;
		}
	}
	else
	{
		if(pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch == 1)
		{
			//���뵽OPEN��
			if( pNode->m_pVtx->GetVtxSInfo()->posOEnd )
			{
				pOldNode = (CGIS_NetworkNode*)pOList->GetAt(pNode->m_pVtx->GetVtxSInfo()->posOEnd);
				if(pNode->m_g < pOldNode->m_g)
				{
					pOList->RemoveAt(pOldNode->m_pVtx->GetVtxSInfo()->posOEnd);
					delete pOldNode;
					InsertNode(pNode,pOList,FALSE);
				}else
				{
					delete pNode;
				}
			}
			else
			{
				//���뵽OPEN��
				InsertNode(pNode,pOList, FALSE);
				pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
			}
		}
		else	if(pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch == 0)
		{
			//���뵽OPEN��
			InsertNode(pNode,pOList, FALSE);
			pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;

		}else
		{
			delete pNode;
		}
	}
	return TRUE;
}
BOOL IGIS_MapNetwork::CalculateRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	switch( pPathInfo->GetVehicleType() )
	{
	case EN_VEHICLETYPE_CAR:	//С����
		return CalculateCarRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_TRUCK:		//�󿨳�
		return CalculateTruckRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_BUS:			//������
		return CalculateBusRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_EMERGENCY:	//�Ȼ���/������
		return CalculateEmergencyRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_TAXI:		//���⳵
		return CalculateTaxiRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_DELIVERY:	//������
		return CalculateDeliveryRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_BICYCLE:		//���г�
		return CalculateBicycleRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_PEDESTRIAN:		//����
		return CalculatePedestrianRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return FALSE;
}
const int	dCVel[]	= {20000, 25000, 30000, 40000, 50000, 70000, 110000};//�ٶ�
const float dTAVal[] = {0.0,0.16,0.68,2.8,2.9,3.0};	//ת��Ȩֵ
const float	fM[]	= {1.0, 1.25, 1.5, 1.75, 2.0};		//·��Ȩֵ
//���㳵������ΪС����ʱ�ýڵ������ŵ�·��
BOOL IGIS_MapNetwork::CalculateCarRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead )
{
    float  fMaxVelocity = 0.0;
    float  gn = 0.0;
    float  hn = 0.0;
    
    //////////////////////////////////////////////////////////////////////////
    //////------�����������ǰ·�ε���ǰ���ʵ�ʴ���--------------------------
    {
        CGIS_NetworkRut* pRut = pNode->m_pRut;
        float dbAVal = dTAVal[byAngle];             //ȡ��ǰת��Ƕ�Ȩֵ

        if( pRut == NULL )
            return FALSE;
        if( pRut->GetRutDirection() == 1 )
        {	
            if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
            {
                pNode->m_f = pNode->m_g = 10000.0;
                return FALSE;
            }
            else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
            {
                pNode->m_f = pNode->m_g = 10000.0;
                return FALSE;
            }
        }
        else if( pRut->GetRutDirection() == 2 )
        {
            if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
            {
                pNode->m_f = pNode->m_g = 10000.0;
                return FALSE;
            }
            else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
            {
                pNode->m_f = pNode->m_g = 10000.0;
                return FALSE;
            }
        }
        else if( pRut->GetRutDirection() == 3 )
        {
            pNode->m_f = pNode->m_g = 10000.0;
            return FALSE;
        }
        if( pPathInfo->IsTollEvite() )
        {
            if( pRut->GetRutCharge() == 1 || pRut->GetRutCharge() == 2 )
                gn += 5.0;
        }
        if( pPathInfo->IsHWayEvite() )
        {
            if( pRut->GetRutLevel() == 0 )
                gn += 5.0;
        }
        if( pPathInfo->IsSelEvite() )
        {
            ;	
        }
        if( pPathInfo->IsUnlayEvite() )
        {
            if( pRut->GetRutType() == 1 || pRut->GetType() == 6 )
                gn += 5.0;
        }
        if( pRut->GetRutType() == 1 )
            gn += 0.2;
        
        //////////////////////////////////////////////////////////////////////////
        //��ָ����·�Ļر�
        UINT nCount = 0;
        int *pMIDs = NULL;
        int *pBIDs = NULL;
        int *pRIDs = NULL;
        BOOL bInAvoid = FALSE;
        int nMID = m_pMap->GetMapInfo()->GetMapID();
        pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
        for( int i=0;	i<nCount;	i++ )
        {
            if( pMIDs[i] == nMID && pBIDs[i] == pRut->GetRBreadthID() )
            {
                if( pRut->GetRutID() == pRIDs[i] )
                {
                    bInAvoid = TRUE;
                    break;
                }
            }
        }
        if( bInAvoid )
            gn += 20.0;
        //////////////////////////////////////////////////////////////////////////
        
        
        //////////////////////////////////////////////////////////////////////////
        fMaxVelocity = dCVel[pRut->GetRutLevel()];  //ȡ����ٶ�
        //���ʱ��(�Ͽ�·��)
        if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
        {
            gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
        }
        //��̾���(�϶�·��)
        else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
        {
            gn += (float)(pRut->GetRutLength() / 1000.);
        }
        //�����շ�·��
        else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
        {
            gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
        }
        //����ת��
        else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
        {
            gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
            
            if( byAngle == 1 || byAngle == 2 )
                dbAVal *= 1.2;
            else
                dbAVal *= 1.25;
        }
        CALCULATE_ANGLE_VALUE_BY_SCENE(dbAVal,m_eCalculateScene);
    	gn += dbAVal;
    }
    //////////////////////////////////////////////////////////////////////////





    //////////////////////////////////////////////////////////////////////////
    //////------���ݵ�ǰ�㵽Ŀ���ľ���������ƴ���--------------------------
    {
        hn = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*53.483); ///0.0006
        if( hn > m_dbMaxDis )
        {
            pNode->m_f = pNode->m_g = 10000.0;
            return FALSE;
        }
        CALCULATE_LENGTH_VALUE_BY_SCENE(hn,m_eCalculateScene,pNode->m_pRut->GetRutLevel());
    }
    //////////////////////////////////////////////////////////////////////////


	pNode->m_g = pNode->m_pParent->m_g + gn;
	pNode->m_f = pNode->m_g + hn;
	return TRUE;
}







//���㳵������Ϊ�󿨳�ʱ�ýڵ������ŵ�·��
BOOL IGIS_MapNetwork::CalculateTruckRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRut *pRut = pNode->m_pRut;
	if( pRut->GetRutDirection() == 1 )
	{	
		if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
		else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
	}
	else if( pRut->GetRutDirection() == 2 )
	{
		if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
		else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
	}
	else if( pRut->GetRutDirection() == 3 )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
	if( pPathInfo->IsTollEvite() )
	{
		if( pRut->GetRutType() == 24 || pRut->GetRutType() == 25 || pRut->GetRutType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRut->GetRutLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
        ;   
    }
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRut->GetType() == 6 )
			gn += 5.0;
	}
	if( pRut->GetRutType() == 6 || 
		pRut->GetRutType() == 7 || pRut->GetRutType() == 3 )
		gn += 3.0;
	if( pRut->GetType() == 6 )
		gn += 2.0;
	if( pRut->GetRutType() == 1 )
		gn += 1.0;

	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
	int nMID = m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRut->GetRBreadthID() )
		{
			if( pRut->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRut->GetRutLevel()];
	//���ʱ��(�Ͽ�·��)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//��̾���(�϶�·��)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRut->GetRutLength() / 1000.);
	}
	//�����շ�·��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//����ת��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->m_pVtx->GetVtxPosition().dx, pNode->m_pVtx->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
    CALCULATE_LENGTH_VALUE_BY_SCENE(len,m_eCalculateScene,pRut->GetRutLevel());
    CALCULATE_ANGLE_VALUE_BY_SCENE(dbAVal,m_eCalculateScene);
	gn += dbAVal;
	pNode->m_g = pNode->m_pParent->m_g + gn;
	pNode->m_f =  len + pNode->m_g ;

    
	return TRUE;
}
//���㳵������Ϊ������ʱ�ýڵ������ŵ�·��
BOOL IGIS_MapNetwork::CalculateBusRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRut *pRut = pNode->m_pRut;
	if( pRut->GetRutDirection() == 1 )
	{	
		if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
		else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
	}
	else if( pRut->GetRutDirection() == 2 )
	{
		if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
		else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
	}
	else if( pRut->GetRutDirection() == 3 )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
	if( pPathInfo->IsTollEvite() )
	{
		if( pRut->GetRutType() == 24 || pRut->GetRutType() == 25 || pRut->GetRutType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRut->GetRutLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
        ;
    }
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRut->GetType() == 6 )
			gn += 5.0;
	}
	if( pRut->GetRutType() == 6 || 
		pRut->GetRutType() == 7 || pRut->GetRutType() == 3 )
		gn += 2.0;
	if( pRut->GetType() == 6 )
		gn += 1.5;
	if( pRut->GetRutType() == 1 )
		gn += 0.2;
	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
	int nMID = m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRut->GetRBreadthID() )
		{
			if( pRut->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRut->GetRutLevel()];
	//���ʱ��(�Ͽ�·��)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//��̾���(�϶�·��)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRut->GetRutLength() / 1000.);
	}
	//�����շ�·��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//����ת��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->m_pVtx->GetVtxPosition().dx, pNode->m_pVtx->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
    CALCULATE_LENGTH_VALUE_BY_SCENE(len,m_eCalculateScene,pRut->GetRutLevel());
    CALCULATE_ANGLE_VALUE_BY_SCENE(dbAVal,m_eCalculateScene);
	gn += dbAVal;
	pNode->m_g = pNode->m_pParent->m_g + gn;
	pNode->m_f =  len + pNode->m_g ;

	return TRUE;
}
//���㳵������Ϊ�Ȼ���/������ʱ�ýڵ������ŵ�·��
BOOL IGIS_MapNetwork::CalculateEmergencyRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRut *pRut = pNode->m_pRut;
	if( pPathInfo->IsTollEvite() )
	{
		if( pRut->GetRutType() == 24 || pRut->GetRutType() == 25 || pRut->GetRutType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRut->GetRutLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
        ;
    }
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRut->GetType() == 6 )
			gn += 5.0;
	}
	if( pRut->GetRutType() == 6 || 
		pRut->GetRutType() == 7 || pRut->GetRutType() == 3 )
		gn += 1.0;
	if( pRut->GetType() == 6 )
		gn += 0.5;
	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
	int nMID = m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRut->GetRBreadthID() )
		{
			if( pRut->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRut->GetRutLevel()];
	//���ʱ��(�Ͽ�·��)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//��̾���(�϶�·��)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRut->GetRutLength() / 1000.);
	}
	//�����շ�·��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//����ת��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->m_pVtx->GetVtxPosition().dx, pNode->m_pVtx->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
    CALCULATE_LENGTH_VALUE_BY_SCENE(len,m_eCalculateScene,pRut->GetRutLevel());
    CALCULATE_ANGLE_VALUE_BY_SCENE(dbAVal,m_eCalculateScene);
	gn += dbAVal;
	pNode->m_g = pNode->m_pParent->m_g + gn;
	pNode->m_f =  len + pNode->m_g ;


    return TRUE;
}
//���㳵������Ϊ���⳵ʱ�ýڵ������ŵ�·��
BOOL IGIS_MapNetwork::CalculateTaxiRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRut *pRut = pNode->m_pRut;
	if( pRut->GetRutDirection() == 1 )
	{	
		if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
		else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
	}
	else if( pRut->GetRutDirection() == 2 )
	{
		if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
		else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
	}
	else if( pRut->GetRutDirection() == 3 )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
	if( pPathInfo->IsTollEvite() )
	{
		if( pRut->GetRutType() == 24 || pRut->GetRutType() == 25 || pRut->GetRutType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRut->GetRutLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
        ;
    }
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRut->GetType() == 6 )
			gn += 5.0;
	}
	if( pRut->GetRutType() == 6 || 
		pRut->GetRutType() == 7 || pRut->GetRutType() == 3 )
		gn += 1.0;
	if( pRut->GetType() == 6 )
		gn += 0.5;
	if( pRut->GetRutType() == 1 )
		gn += 0.5;
	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
	int nMID = m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRut->GetRBreadthID() )
		{
			if( pRut->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRut->GetRutLevel()];
	//���ʱ��(�Ͽ�·��)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//��̾���(�϶�·��)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRut->GetRutLength() / 1000.);
	}
	//�����շ�·��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//����ת��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->m_pVtx->GetVtxPosition().dx, pNode->m_pVtx->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
    CALCULATE_LENGTH_VALUE_BY_SCENE(len,m_eCalculateScene,pRut->GetRutLevel());
    CALCULATE_ANGLE_VALUE_BY_SCENE(dbAVal,m_eCalculateScene);
	gn += dbAVal;
	pNode->m_g = pNode->m_pParent->m_g + gn;
	pNode->m_f =  len + pNode->m_g ;


	return TRUE;
}
//���㳵������Ϊ������ʱ�ýڵ������ŵ�·��
BOOL IGIS_MapNetwork::CalculateDeliveryRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRut *pRut = pNode->m_pRut;
	if( pRut->GetRutDirection() == 1 )
	{	
		if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
		else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
	}
	else if( pRut->GetRutDirection() == 2 )
	{
		if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
		else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
	}
	else if( pRut->GetRutDirection() == 3 )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
	if( pPathInfo->IsTollEvite() )
	{
		if( pRut->GetRutType() == 24 || pRut->GetRutType() == 25 || pRut->GetRutType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRut->GetRutLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
        ;
    }
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRut->GetType() == 6 )
			gn += 5.0;
	}
	if( pRut->GetRutType() == 6 || 
		pRut->GetRutType() == 7 || pRut->GetRutType() == 3 )
		gn += 1.0;
	if( pRut->GetType() == 6 )
		gn += 0.5;
	if( pRut->GetRutType() == 1 )
		gn += 0.5;
	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
	int nMID = m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRut->GetRBreadthID() )
		{
			if( pRut->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRut->GetRutLevel()];
	//���ʱ��(�Ͽ�·��)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//��̾���(�϶�·��)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRut->GetRutLength() / 1000.);
	}
	//�����շ�·��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//����ת��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->m_pVtx->GetVtxPosition().dx, pNode->m_pVtx->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
    CALCULATE_LENGTH_VALUE_BY_SCENE(len,m_eCalculateScene,pRut->GetRutLevel());
    CALCULATE_ANGLE_VALUE_BY_SCENE(dbAVal,m_eCalculateScene);
	gn += dbAVal;
	pNode->m_g = pNode->m_pParent->m_g + gn;
	pNode->m_f =  len + pNode->m_g ;

	return TRUE;
}
//���㳵������Ϊ���г�ʱ�ýڵ������ŵ�·��
BOOL IGIS_MapNetwork::CalculateBicycleRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRut *pRut = pNode->m_pRut;
	if( pRut->GetRutDirection() != 0 )
	{
		if( pRut->GetRutStartVtxID() == pNode->m_pVtx->GetVtxID() && !bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
		else if( pRut->GetRutEndVtxID() == pNode->m_pVtx->GetVtxID() && bHead )
		{
			pNode->m_f = pNode->m_g = 10000.0;
			return FALSE;
		}
	}
	float dbAVal = 0.0;
	if( pPathInfo->IsTollEvite() )
	{
		if( pRut->GetRutType() == 24 || pRut->GetRutType() == 25 || pRut->GetRutType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRut->GetRutLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
        ;
    }
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRut->GetType() == 6 )
			gn += 5.0;
	}
	if( pRut->GetType() == 1  || pRut->GetType() == 2 )
		gn += 10.0;
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
	int nMID = m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRut->GetRBreadthID() )
		{
			if( pRut->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRut->GetRutLevel()];
	//���ʱ��(�Ͽ�·��)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//��̾���(�϶�·��)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRut->GetRutLength() / 1000.);
	}
	//�����շ�·��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//����ת��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->m_pVtx->GetVtxPosition().dx, pNode->m_pVtx->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
    CALCULATE_LENGTH_VALUE_BY_SCENE(len,m_eCalculateScene,pRut->GetRutLevel());
    CALCULATE_ANGLE_VALUE_BY_SCENE(dbAVal,m_eCalculateScene);
	gn += dbAVal;
	pNode->m_g = pNode->m_pParent->m_g + gn;
	pNode->m_f =  len + pNode->m_g ;


	return TRUE;
}
//���㳵������Ϊ����ʱ�ýڵ������ŵ�·��
BOOL IGIS_MapNetwork::CalculatePedestrianRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRut *pRut = pNode->m_pRut;
	float dbAVal = 0.0;
	if( pPathInfo->IsTollEvite() )
	{
		if( pRut->GetRutType() == 24 || pRut->GetRutType() == 25 || pRut->GetRutType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRut->GetRutLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
        ;
	}
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRut->GetType() == 6 )
			gn += 5.0;
	}
	if( pRut->GetType() == 1 || pRut->GetType() == 2 )
		gn += 10.0;
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
	int nMID = m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRut->GetRBreadthID() )
		{
			if( pRut->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRut->GetRutLevel()];
	//���ʱ��(�Ͽ�·��)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//��̾���(�϶�·��)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRut->GetRutLength() / 1000.);
	}
	//�����շ�·��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//����ת��
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRut->GetRutLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->m_pVtx->GetVtxPosition().dx, pNode->m_pVtx->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->m_pVtx->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->m_pVtx->GetVtxPosition().dy - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->m_f = pNode->m_g = 10000.0;
		return FALSE;
	}
    CALCULATE_LENGTH_VALUE_BY_SCENE(len,m_eCalculateScene,pRut->GetRutLevel());
    CALCULATE_ANGLE_VALUE_BY_SCENE(dbAVal,m_eCalculateScene);
	gn += dbAVal;
	pNode->m_g = pNode->m_pParent->m_g + gn;
	pNode->m_f =  len + pNode->m_g ;


	return TRUE;
}

int IGIS_MapNetwork::CalculateStop()
{
    ASSERT(m_bCalculateStopSign == 0);
	InterlockedIncrement(&m_bCalculateStopSign);
	return TRUE;
}
