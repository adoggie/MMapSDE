// GIS_Middle.cpp: implementation of the CGIS_Middle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sdelib.h"
#include "GIS_Middle.h"
#include "GIS_TrackPoint.h"
#include "GIS_FeatureTrack.h"
#include "GIS_Address.h"
#include "GIS_FeatureRoute.h"
#include "GIS_PathPoint.h"
#include "GIS_WayPoint.h"
#include "GIS_PathpointNode.h"
#include "GIS_PathpointMark.h"
#include "Winarchive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_Middle::CGIS_Middle()
{
	m_pArchive = new CWinArchive;

	m_bIsFeatureExistance = FALSE;
}

CGIS_Middle::~CGIS_Middle()
{
	if ( m_pArchive )
	{
		delete m_pArchive;
		m_pArchive =NULL;
	}
}
//#include "SDEControler.h"
CGIS_Feature * CGIS_Middle::LoadFeatureRoute(CGIS_UserLayer *pLayer, swString strFPath, UINT nID)
{
	CGIS_FeatureRoute * pFRouteTmp = NULL;
	TRY
	{
		CPtrList* pRList;
		pLayer->GetObjectListMap()->Lookup( 0, pRList );
		
		int nNum = 0;
		BOOL bFlag = TRUE;
		char szTmp[MAX_PATH];
		int nLen = 0;
		
		if ( pRList->GetCount() <= 0 )
		{
			bFlag = m_pArchive->AddRef( strFPath + L"\\Route.dat", IArchive::load );
			if ( !bFlag )
			{
				return NULL;
			}
			
			*m_pArchive>>nNum;//FeatureRoute������
			
			CPtrList * pWPList = NULL;
			CGIS_FeatureRoute * pFRoute;
			while ( nNum-- )
			{
				int		nRouteID		= 0;
				swString	strRouteName	= "";
				
				*m_pArchive>>nRouteID>>strRouteName;
				
				pFRoute = new CGIS_FeatureRoute( nRouteID );
				CGIS_NetworkRoute * pNWRoute = pFRoute->GetNetworkRoute();
				pNWRoute->SetRouteName( strRouteName );
				
				
				pWPList = pNWRoute->GetWayPointList();
				
				UINT		uID = 0;//;����ID
				int			wpType = 0;//����
				swString		strAnno = "";//��ע
				swString		strWPRName = "";
				BYTE		nAnnoNum = 0;//��ע����
				float		fTmp = 0.0f;
				int nCount = 0;
				*m_pArchive>>nCount;
				
				ST_GEO_PTXY pt;//;����λ��
				pt.dx = 0.0f;
				pt.dy = 0.0f;
				
				//��ȡwaypoint
				while ( nCount-- )
				{
					*m_pArchive>>uID;
					*m_pArchive>>fTmp;
					pt.dx = fTmp;
					*m_pArchive>>fTmp;
					pt.dy = fTmp;
					*m_pArchive>>wpType>>strAnno>>nAnnoNum>>strWPRName;
//					*m_pArchive>>wpType>>strAnno>>nAnnoNum;//>>strWPRName;
					nLen = UnicodeToChar( strAnno, szTmp, MAX_PATH );
					char * szAnno = NULL;
					if ( nLen )
					{
						szAnno = new char[ nLen ];
						memcpy( szAnno, szTmp, nLen );
					}
					//				char * szAnno = new char[strAnno.GetLength()+1];
					//				WideCharToMultiByte(CP_ACP, 0, strAnno.GetBuffer(strAnno.GetLength()), -1, szAnno, strAnno.GetLength(), NULL, NULL);
					
					CGIS_WayPoint * pWPoint = new CGIS_WayPoint( uID );
					pWPoint->SetWPPosition( pt );
					pWPoint->SetWPType( (EnWaypointType)wpType );
					pWPoint->SetWPAnnoation( szAnno );
					pWPoint->SetWPAnnoNum( nAnnoNum );
					pWPoint->SetWPRutName( strWPRName );
					pWPList->AddTail( pWPoint );
				}
				
				pRList->AddTail( pFRoute );
			}
			
			m_pArchive->ReleaseRef();
		}
//		/////////////////////
//		CGIS_NetworkPathInfo *pPathInfo = new CGIS_NetworkPathInfo( L"\\Storage Card\\MobileNavigator\\map\\voice.txt" );
//		pPathInfo->SetVehicleType( EN_VEHICLETYPE_CAR );
//		pPathInfo->SetRoutePreference( EN_RUTPREF_FASTERTIME );
//		POSITION psRout = pRList->GetHeadPosition();
//		CSDEControler* pControl = CSDEControler::GetSDEControl();
//		if( pControl == NULL )
//		{
//			ASSERT( FALSE );
//		}
//		IGIS_NetWork *pNetwork = (IGIS_NetWork*)pControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );//new CGIS_NetWork;
//		while( psRout )
//		{
//			CGIS_FeatureRoute *pFRTemp = (CGIS_FeatureRoute*)pRList->GetNext(psRout);
//			pFRTemp->GetNetworkRoute()->SetNetworkPathInfo(pPathInfo);
//			pNetwork->SetNetworkRoute( pFRTemp->GetNetworkRoute() );
//			pNetwork->CalculateOptimalRoute();
//			SaveFeatureRoute( pLayer, pFRTemp, strFPath, pFRTemp->GetNetworkRoute()->GetRouteID() );
//		}
//		AfxMessageBox( _T("Finish") );
//		////////////////////////////////
		
		if ( nID < 1 )
		{
			return NULL;
		}
		
		bFlag = m_pArchive->AddRef( strFPath, IArchive::load, nID );
		if ( !bFlag )
		{
			return NULL;
		}
		
		float		fTmp = 0.0f;
		
		//	CPtrList * pWPList = NULL;
		
		CGIS_FeatureRoute * pFRouteC = NULL;
		POSITION pos = pRList->GetHeadPosition();
		while ( pos ) 
		{
			pFRouteC = (CGIS_FeatureRoute *)pRList->GetNext( pos );
			CGIS_NetworkRoute * pNWRouteTmp = pFRouteC->GetNetworkRoute();
			
			if ( nID == pNWRouteTmp->GetRouteID() )
			{
				pFRouteTmp = (CGIS_FeatureRoute *)pFRouteC->Clone();//////
				pNWRouteTmp = pFRouteTmp->GetNetworkRoute();
				
				//��ȡpath
				int nPType = 0;
				ST_GEO_PTXY pt;
				pt.dx = 0;
				pt.dy = 0;
				*m_pArchive>>nNum;
				
				CGIS_NetworkPath * pNWPath = pNWRouteTmp->GetNetworkPath();
				CPtrList * pPathPointList = pNWPath->GetPathpointList();
				CGIS_Pathpoint * pPPoint = NULL;
				while ( nNum-- )
				{
					pPPoint = NULL;
					*m_pArchive>>nPType;
					*m_pArchive>>fTmp;
					pt.dx = fTmp;
					*m_pArchive>>fTmp;
					pt.dy = fTmp;
					///*
					if ( nPType == EN_PATHPOINTTYPE_NODE )
					{
						CGIS_PathpointNode * pPNode = new CGIS_PathpointNode;
						pPNode->SetPathpointType( (EnPathpointType)nPType );
						pPNode->SetPathpointPosition( pt );
						
						int nNextRutID = 0;	//�¸�·�ε�ID
						int nNodeVtxID = 0;	//��ǰ�ڵ��ID
						BYTE byRType = 0;
						BYTE byRutType = 0;
						int nBID = 0;
						BYTE nAnnoNum   = 0;
						swString strAnno = "";
						int nTurnAngle = 0;
						short nRMOrd = 0;
						ST_GEO_RECT rtCross;
						
						*m_pArchive>>nNextRutID>>nNodeVtxID>>byRType>>byRutType>>nBID>>nRMOrd>>nAnnoNum;
						pPNode->SetNextRutID( nNextRutID );
						pPNode->SetNodeVtxID( nNodeVtxID );
						pPNode->SetRType(byRType);
						pPNode->SetRutType(byRutType);
						pPNode->SetPBreadthID(nBID);
						pPNode->SetRMOrder( nRMOrd );
						*m_pArchive>>fTmp;
						rtCross.dLeft = fTmp;
						*m_pArchive>>fTmp;
						rtCross.dTop = fTmp;
						*m_pArchive>>fTmp;
						rtCross.dRight = fTmp;
						*m_pArchive>>fTmp;
						rtCross.dBottom = fTmp;
						pPNode->SetCRect( rtCross );
						pPNode->SetRutAnnoNum( nAnnoNum );
						if ( nAnnoNum )
						{
							*m_pArchive>>strAnno;
							char * szAnno = new char[ nAnnoNum ];
							UnicodeToChar( strAnno, szAnno, nAnnoNum );
							//						WideCharToMultiByte(CP_ACP, 0, strAnno.GetBuffer(strAnno.GetLength()), -1, szAnno, strAnno.GetLength(), NULL, NULL);
							pPNode->SetRutAnno( szAnno );
						}
						
						ST_VOICE_NODE * pNode = pPNode->GetVNodeInfo();
						BYTE byTmp = 0;
						
						*m_pArchive>>byTmp;
						pNode->byAngle = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->byConfusion = byTmp;
						*m_pArchive>>byTmp;
						pNode->byConfusion2 = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->byDestination = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->byEnName = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->byExName = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->byIMI = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->byOtherStreet = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->byRound = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->bySameName = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->bySameRoad = byTmp;
						
						*m_pArchive>>byTmp;
						pNode->byTurn = byTmp;
						
						ST_VOICE_RUT * pRut = pPNode->GetVRutInfo();
						
						*m_pArchive>>byTmp;
						pRut->byAdd = byTmp;
						
						*m_pArchive>>byTmp;
						pRut->byCharge = byTmp;
						
						*m_pArchive>>byTmp;
						pRut->byDrivable = byTmp;
						
						*m_pArchive>>byTmp;
						pRut->byType = byTmp;
						
						*m_pArchive>>nTurnAngle;
						pPNode->SetTurnAngle( nTurnAngle );
						
						int nNum1 = 0;
						*m_pArchive>>nNum1;
						while ( nNum1-- )
						{
							CGIS_NetworkNodeRut *pNRut = new CGIS_NetworkNodeRut;
							int nTmp = 0;
							
							*m_pArchive>>nTmp;
							pNRut->SetRutID( nTmp );
							BYTE byAnno = 0;
							*m_pArchive>>byAnno;
							pNRut->SetRutAnnoNum( byAnno );
							if ( byAnno > 0 )
							{
								swString str = "";
								*m_pArchive>>str;
								char * szStr = new char[byAnno];
								UnicodeToChar( str, szStr, byAnno );
								
								//							WideCharToMultiByte(CP_ACP, 0, str.GetBuffer(str.GetLength()), -1, szStr, str.GetLength(), NULL, NULL);
								pNRut->SetRutAnno( szStr );
							}
							int nPtCount = 0;
							*m_pArchive>>nPtCount;
							pNRut->SetRutPtNum( nPtCount );
							//						BYTE bWidth = 0;
							//						*m_pArchive>>bWidth;
							//						pNRut->SetRutWidth( bWidth );
							
							ST_GEO_PTXY * pPt = new ST_GEO_PTXY[nPtCount];
							ST_GEO_PTXY * pTmp = pPt;
							while ( nPtCount-- )
							{
								*m_pArchive>>fTmp;
								pTmp->dx = fTmp;
								*m_pArchive>>fTmp;
								pTmp->dy = fTmp;
								
								pTmp++;
							}
							
							pNRut->SetRutPtList( pPt );
							pPNode->GetNodeRutList()->AddTail(pNRut);
						}
						
						pPPoint = pPNode;
				}
				else if ( nPType == EN_PATHPOINTTYPE_START||
					nPType == EN_PATHPOINTTYPE_INTERMEDIATE||
					nPType == EN_PATHPOINTTYPE_END )
				{
					CGIS_PathpointMark * pPMark = new CGIS_PathpointMark;
					
					pPMark->SetPathpointType( (EnPathpointType)nPType );
					pPMark->SetPathpointPosition( pt );
					
					ST_VOICE_RUT * pVRut = pPMark->GetVRutInfo();
					ST_VOICE_NODE * pVNode = pPMark->GetVNodeInfo();
					
					int nID = 0;
					*m_pArchive>>nID;
					CGIS_WayPoint *pWPoint = pFRouteTmp->GetNetworkRoute()->GetWayPointByID( nID );
					if( pWPoint )
					{
						pPMark->SetWayPoint( pWPoint );
						pWPoint->SetWPProjPosition( pt, EN_MAPTYPE_NATION );
					}
					
					BYTE byTmp = 0;
					
					*m_pArchive>>byTmp;
					pVRut->byAdd = byTmp;
					*m_pArchive>>byTmp;
					pVRut->byCharge = byTmp;
					*m_pArchive>>byTmp;
					pVRut->byDrivable = byTmp;
					*m_pArchive>>byTmp;
					pVRut->byType = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->byAngle = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->byConfusion = byTmp;
					*m_pArchive>>byTmp;
					pVNode->byConfusion2 = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->byDestination = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->byEnName = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->byExName = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->byIMI = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->byOtherStreet = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->byRound = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->bySameName = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->bySameRoad = byTmp;
					
					*m_pArchive>>byTmp;
					pVNode->byTurn = byTmp;
					
					pPPoint = pPMark;
				}
				else if ( nPType == EN_PATHPOINTTYPE_INFLEXION )
				{
					pPPoint = new CGIS_Pathpoint;
					pPPoint->SetPathpointType( (EnPathpointType)nPType );
					pPPoint->SetPathpointPosition( pt );
				}
				
				pPathPointList->AddTail( pPPoint );
				//*/			
			}
			
			break;
		}
	}
	
	m_pArchive->ReleaseRef();
	}
	CATCH_ALL(e)
	{
		m_pArchive->ReleaseRef();
//		THROW(e);
	}
	END_CATCH_ALL

	return pFRouteTmp;
}

BOOL CGIS_Middle::SaveFeatureRoute(CGIS_UserLayer * pUserLayer,CGIS_Feature * pFeature, swString strFPath, UINT nID)
{
	//�������溽�ߵ�Ŀ¼
	BOOL bCreate = CreateDirectory( strFPath, NULL );

	//�ڴ��еĺ����б�
	CPtrList * pList;
	pUserLayer->GetObjectListMap()->Lookup( 0, pList );
	
	CGIS_FeatureRoute * pF = (CGIS_FeatureRoute *)pFeature;

	//Ҫ����ĺ����Ƿ��Ѿ����ڱ�־��FALSE:���߲�����
	BOOL	bIsExistance = FALSE;
	
	//���������б�,��麽���Ƿ��Ѿ�����
	POSITION p = pList->GetHeadPosition();
	while ( p )
	{
		CGIS_FeatureRoute * pFTmp = (CGIS_FeatureRoute *)pList->GetNext( p );
		if ( pFeature == pFTmp )
		{
			//�����Ѿ����ڣ�����־��ΪTRUE
			bIsExistance = TRUE;
			break;
		}
	}

	//��������
	int nNum = pList->GetCount();
	
	CGIS_FeatureRoute * pFeatureNew = NULL;
	//������߲����ڣ������Ӻ���
	if ( !bIsExistance )
	{
		//Modified by HB.Deng�κ�����¶����溽�ߵĿ�¡
		pFeatureNew = (CGIS_FeatureRoute *)pFeature->Clone();
		pFeatureNew->GetNetworkRoute()->SetRouteID( pUserLayer->GetFeatureMaxID() + 1 );

		pList->AddTail( pFeatureNew );
		nNum += 1;//�������µ�Route
	}

	//����ͷ���ļ�
	BOOL bFlag = m_pArchive->AddRef( strFPath + L"\\Route.dat", IArchive::store );
	if ( !bFlag )
	{
		return FALSE;
	}

	*m_pArchive<<nNum;//����FeatureRoute����
	
	//����waypoint�����ڴ��е�waypoint����д���ļ�
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		CGIS_FeatureRoute* pFRoute = ( CGIS_FeatureRoute * )pList->GetNext( pos );
		if ( pFRoute )
		{
			CGIS_NetworkRoute * pNWRouteTmp = pFRoute->GetNetworkRoute();

			*m_pArchive<<pNWRouteTmp->GetRouteID()<<pNWRouteTmp->GetRouteName();
///*
			CPtrList * pPList = pNWRouteTmp->GetWayPointList();
			nNum = pPList->GetCount();
			
			*m_pArchive<<nNum;//����waypoint�ĸ���
			
			if ( pPList )
			{
				POSITION posWPoint = pPList->GetHeadPosition();
				while ( posWPoint )
				{
					CGIS_WayPoint * pWPoint = (CGIS_WayPoint *)pPList->GetNext( posWPoint );
					if ( pWPoint )
					{
						UINT		uID		 =	 pWPoint->GetWPID();//;����ID
						ST_GEO_PTXY pt		 =	 pWPoint->GetWPPosition();//;����λ��
						int			wpType	 =	 pWPoint->GetWPType();//����
						swString		strAnno;//	 =	 pWPoint->GetWPAnnoation();//��ע
						CharToUnicode( pWPoint->GetWPAnnoation() , &strAnno );
						BYTE		nAnnoNum =	 pWPoint->GetWPAnnoNum();//��ע����
						swString		strWPRName = pWPoint->GetWPRutName();
						*m_pArchive<<uID<<pt.dx<<pt.dy<<wpType<<strAnno<<nAnnoNum<<strWPRName;
					}
				}
			}
		}
	}

	m_pArchive->ReleaseRef();//�ر�

	//û�к������ݣ�ֱ�ӷ���
	if ( pF->GetNetworkRoute()->GetNetworkPath()->GetPathpointList()->GetCount() <= 0 )
	{
		return FALSE;
	}

	//��(û���򴴽�)���������ļ���׼�����溽������
	bFlag = m_pArchive->AddRef( strFPath, IArchive::store, bIsExistance ? nID : pFeatureNew->GetNetworkRoute()->GetRouteID() );
	if ( !bFlag )
	{
		return FALSE;
	}

	CGIS_NetworkRoute* pNWRoute = pF->GetNetworkRoute();
	CGIS_NetworkPath * pNWPath = pNWRoute->GetNetworkPath();
	if ( pNWPath )
	{
		CPtrList * pPPList = pNWPath->GetPathpointList();
		nNum = pPPList->GetCount();
		*m_pArchive<<nNum;//����pathpoint�ĸ���

		POSITION posPP =  pPPList->GetHeadPosition();
		while ( posPP )
		{
			CGIS_Pathpoint * pPathPoint = (CGIS_Pathpoint *)pPPList->GetNext( posPP );
			if ( pPathPoint )
			{
				int nPType = pPathPoint->GetPathpointType();
				ST_GEO_PTXY pt = pPathPoint->GetPathpointPosition();
				*m_pArchive<<nPType;//��������
				*m_pArchive<<pt.dx<<pt.dy;//��������
//==========================================================================================
				
				if ( nPType == EN_PATHPOINTTYPE_NODE )
				{
					CGIS_PathpointNode * pPNode = (CGIS_PathpointNode *)pPathPoint;
					*m_pArchive<<pPNode->GetNextRutID();
					*m_pArchive<<pPNode->GetNodeVtxID();
					*m_pArchive<<pPNode->GetRType();
					*m_pArchive<<pPNode->GetRutType();
					*m_pArchive<<pPNode->GetPBreadthID();
					*m_pArchive<<pPNode->GetRMOrder();
					*m_pArchive<<pPNode->GetRutAnnoNum();
					*m_pArchive<<pPNode->GetCRect().dLeft<<pPNode->GetCRect().dTop<<pPNode->GetCRect().dRight<<pPNode->GetCRect().dBottom;
					if ( pPNode->GetRutAnnoNum() > 0 )
					{
						swString strTmp;// = pPNode->GetRutAnno();
						CharToUnicode( pPNode->GetRutAnno() , &strTmp );
						*m_pArchive<<strTmp;
					}
					ST_VOICE_NODE * node =  pPNode->GetVNodeInfo();
					*m_pArchive<<node->byAngle;
					*m_pArchive<<node->byConfusion;
					*m_pArchive<<node->byConfusion2;
					*m_pArchive<<node->byDestination;
					*m_pArchive<<node->byEnName;
					*m_pArchive<<node->byExName;
					*m_pArchive<<node->byIMI;
					*m_pArchive<<node->byOtherStreet;
					*m_pArchive<<node->byRound;
					*m_pArchive<<node->bySameName;
					*m_pArchive<<node->bySameRoad;
					*m_pArchive<<node->byTurn;
					ST_VOICE_RUT * rut = pPNode->GetVRutInfo();
					*m_pArchive<<rut->byAdd<<rut->byCharge<<rut->byDrivable<<rut->byType; 
					*m_pArchive<<pPNode->GetTurnAngle();

					nNum = pPNode->GetNodeRutList()->GetCount();
					*m_pArchive<<nNum;
					
					CGIS_NetworkNodeRut *pNRut = NULL;
					POSITION ps = pPNode->GetNodeRutList()->GetHeadPosition();
					while ( ps )
					{
						pNRut = (CGIS_NetworkNodeRut*)pPNode->GetNodeRutList()->GetNext(ps);
						if( pNRut )
						{
							nNum = pNRut->GetRutID();
							*m_pArchive<<pNRut->GetRutID()<<pNRut->GetRutAnnoNum();
							if ( pNRut->GetRutAnnoNum() > 0 )
							{
								swString str;// = pNRut->GetRutAnno();
								CharToUnicode( pNRut->GetRutAnno() , &str );
								*m_pArchive<<str;
							}
							nNum = pNRut->GetRutPtNum();
//							*m_pArchive<<nNum<<pNRut->GetRutWidth();
							*m_pArchive<<nNum;
							ST_GEO_PTXY * pt = pNRut->GetRutPtList();
							ASSERT( pt );
							while ( nNum-- )
							{
								*m_pArchive<<pt->dx<<pt->dy;
								pt++;
							}
						}
					}
				}
				else if ( nPType == EN_PATHPOINTTYPE_START||
							nPType == EN_PATHPOINTTYPE_INTERMEDIATE||
							nPType == EN_PATHPOINTTYPE_END) 
				{
					CGIS_PathpointMark * pPMark = (CGIS_PathpointMark *)pPathPoint;
					nNum = pPMark->GetWayPoint()->GetWPID();
					ST_VOICE_RUT * pVRut = pPMark->GetVRutInfo();
					ST_VOICE_NODE * pVNode = pPMark->GetVNodeInfo();
					
					*m_pArchive<<nNum;
					*m_pArchive<<pVRut->byAdd<<pVRut->byCharge<<pVRut->byDrivable<<pVRut->byType;
					
					*m_pArchive<<pVNode->byAngle;
					*m_pArchive<<pVNode->byConfusion;
					*m_pArchive<<pVNode->byConfusion2;
					*m_pArchive<<pVNode->byDestination;
					*m_pArchive<<pVNode->byEnName;
					*m_pArchive<<pVNode->byExName;
					*m_pArchive<<pVNode->byIMI;
					*m_pArchive<<pVNode->byOtherStreet;
					*m_pArchive<<pVNode->byRound;
					*m_pArchive<<pVNode->bySameName;
					*m_pArchive<<pVNode->bySameRoad;
					*m_pArchive<<pVNode->byTurn;
				}
			}
		}
	}

	m_pArchive->ReleaseRef();
	
	return TRUE;
}

CGIS_Feature * CGIS_Middle::LoadFeatureTrack(CGIS_UserLayer *pLayer, swString strFPath, UINT nID)
{
	CGIS_FeatureTrack * PNewTrack = NULL;
	TRY
	{
		CPtrList * pTList;
		pLayer->GetObjectListMap()->Lookup( 0, pTList );
		
		int nNum = 0;
		BOOL bFlag = TRUE;
		
		if ( pTList->GetCount() <= 0 )
		{
			bFlag = m_pArchive->AddRef( strFPath + L"\\Track.dat", IArchive::load );
			if ( !bFlag )
			{
				return NULL;
			}
			
			*m_pArchive>>nNum;
			
			CGIS_FeatureTrack * pFTrack;
			while ( nNum-- )
			{
				int		nID = 0;
				swString strTFName = "";
				
				*m_pArchive>>nID>>strTFName;
				
				pFTrack = new CGIS_FeatureTrack(nID);
				pFTrack->SetTrackFeatureName( strTFName );
				
				pTList->AddTail( pFTrack );
			}
			
			m_pArchive->ReleaseRef();
		}
		
		if ( nID < 1 )//ID��СֵΪ1
		{
			return NULL;
		}
		
		CGIS_TrackPoint * pTPoint;
		ST_GEO_PTXY		location;
		short			orientation;
		double			time;
		//	float			velocity;
		CPtrList	*	pTPList = NULL;
		
		bFlag = m_pArchive->AddRef( strFPath, IArchive::load, nID );
		if ( !bFlag) 
		{
			return NULL;
		}
		
		//��ȡ��������
		m_pArchive->Seek( -sizeof(int), CFile::end );
		*m_pArchive>>nNum;
		
		m_pArchive->Seek( 0, CFile::begin );
		CGIS_FeatureTrack * pFeatureTrack = NULL;
		
		POSITION	pos = pTList->GetHeadPosition();
		while ( pos )
		{
			pFeatureTrack = (CGIS_FeatureTrack *)pTList->GetNext( pos );
			if ( pFeatureTrack )
			{
				if ( nID == pFeatureTrack->GetTrackFeatureID() )
				{
					PNewTrack = (CGIS_FeatureTrack *)pFeatureTrack->Clone();
					pTPList = /*pFeatureTrack*/PNewTrack->GetTrackPointList();
					
					while ( nNum-- )
					{
						pTPoint = new CGIS_TrackPoint;
						*m_pArchive>>location.dx>>location.dy>>orientation>>time;
						
						pTPoint->SetLocation( location );
						pTPoint->SetOrientation( orientation );
						pTPoint->SetCollectionTime( time );
						
						pTPList->AddTail( pTPoint );
					}
					
					break;
				}
			}
		}
		
		m_pArchive->ReleaseRef();
	}
	CATCH_ALL(e)
	{
		m_pArchive->ReleaseRef();
//		THROW(e);
	}
	END_CATCH_ALL;
	
	return /*pFeatureTrack*/PNewTrack;
}

BOOL CGIS_Middle::SaveFeatureTrack(CGIS_UserLayer * pUserLayer,CGIS_Feature * pFeature, swString strFPath, UINT nID)
{
	BOOL bCreate = CreateDirectory( strFPath, NULL );

	CPtrList * pList;
	pUserLayer->GetObjectListMap()->Lookup( 0, pList );

	int	nNum = pList->GetCount();

	CGIS_FeatureTrack * pT = (CGIS_FeatureTrack *)pFeature;
	UINT uID = pT->GetTrackFeatureID();

	BOOL bExistance = FALSE;

	POSITION p = pList->GetHeadPosition();
	while ( p )
	{
		CGIS_FeatureTrack * pTTmp = (CGIS_FeatureTrack *)pList->GetNext( p );
		if ( pT->GetTrackFeatureID() == pTTmp->GetTrackFeatureID() )
		{
			bExistance = TRUE;
		}
	}

	if ( !bExistance )
	{
		m_bIsFeatureExistance = FALSE;
	}
	
	BOOL bFlag = FALSE;
	int nNewTrackID = 0;
	//�������������,���Ӻ���
	if ( !m_bIsFeatureExistance )
	{
		CGIS_FeatureTrack * pFeatureNew = NULL;
		if ( !bExistance )//����
		{
			//�κ�����¶����溽���Ŀ�¡
			pFeatureNew = (CGIS_FeatureTrack *)pFeature->Clone();
			nNewTrackID = pUserLayer->GetFeatureMaxID() + 1;
			pFeatureNew->SetTrackTrackID( nNewTrackID );
			
			pList->AddTail( pFeatureNew );
			nNum += 1;//�������µ�Track
		}
	}
	
	//����ͷ���ļ�
	bFlag = m_pArchive->AddRef( strFPath + L"\\Track.dat", IArchive::store );
	if ( !bFlag )
	{
		return FALSE;
	}
	
	*m_pArchive<<nNum;//��������
	
	CGIS_FeatureTrack * pFTrackTmp = NULL;
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		CGIS_FeatureTrack * pFTrack = ( CGIS_FeatureTrack * )pList->GetNext( pos );
		if ( pFTrack )
		{
			*m_pArchive<<pFTrack->GetTrackFeatureID()<<pFTrack->GetTrackFeatureName();
		}
	}
	
	m_pArchive->ReleaseRef();//�ر�

	//������£��򷵻�(���ܱ༭����)
	if ( pT->GetTrackPointList()->GetCount() <= 0 )
	{
		return FALSE;
	}
	
	CPtrList * pTList = pT->GetTrackPointList();
	nNum = pTList->GetCount();

	//��������Ѿ�����
	if ( m_bIsFeatureExistance )
	{
		bFlag = m_pArchive->AddRef( strFPath, IArchive::load, m_bIsFeatureExistance ? nID : nNewTrackID );
		if ( !bFlag )
		{
			return FALSE;
		}

		m_pArchive->Seek( -sizeof(int), CFile::end );

		//д����ǰ�ȶ���������
		*m_pArchive>>nNum;
		nNum += pTList->GetCount();//�������������

		m_pArchive->ReleaseRef();
		
	}

	bFlag = m_pArchive->AddRef( strFPath, IArchive::store, m_bIsFeatureExistance ? nID : nNewTrackID );
	if ( !bFlag )
	{
		return FALSE;
	}

	if ( m_bIsFeatureExistance )
	{
		m_pArchive->Seek( -sizeof(int), CFile::end );
	}

	POSITION posTrack = pTList->GetHeadPosition();
	while ( posTrack )
	{
		CGIS_TrackPoint * pTPoint = ( CGIS_TrackPoint * )pTList->GetNext( posTrack );
		if ( pTPoint )
		{
			*m_pArchive<<pTPoint->GetLocation().dx<<pTPoint->GetLocation().dy<<pTPoint->GetOrientation()<<pTPoint->GetCollectionTime();
		}
	}

	*m_pArchive<<nNum;//д�غ�������

	m_pArchive->ReleaseRef();
	
	m_bIsFeatureExistance = TRUE;
	return TRUE;
}

//Histroy:
//2006-12-07	HB.Deng		�����쳣����������ַ�����쳣ʱ������Ѿ������ĵ�ַ
//
CGIS_Feature * CGIS_Middle::LoadFeatureAddress(CGIS_UserLayer *pLayer, swString strFPath, UINT nID)
{
	CPtrList * pTList;
	pLayer->GetObjectListMap()->Lookup( 0, pTList );

	CGIS_FeatureAddress * pFAddress = NULL;

	TRY
	{
		if ( pTList->GetCount() <= 0 )
		{
			BOOL bFlag = m_pArchive->AddRef( strFPath + L"\\Address.dat", IArchive::load );
			if ( !bFlag )
			{
				return NULL;
			}
			
			int	nNum = 0;
			*m_pArchive>>nNum;
			
			int			nID = 0;
			swString		strAddName = "";
			UINT		nAdminID = 0;
			swString		strAdminName = "";
			swString		strStreetName = "";
			swString		strZipCode = "";
			UINT		nCityID = 0;
			UINT		nMapID = 0;
			swString		strPhoneNum = "";//�绰����
			BOOL		bVoice = FALSE;//����������õ�ַ50��ʱ��ʾ����
			BOOL		bAvoid = FALSE;//����·��ʱ�ܿ�
			ST_GEO_PTXY pt;
			float		fTmp = 0.0f;
			
			while ( nNum-- )
			{
				*m_pArchive>>bAvoid>>bVoice>>nAdminID>>nCityID>>nID>>nMapID>>strAddName>>strAdminName;
				*m_pArchive>>strPhoneNum>>strStreetName>>strZipCode;
				*m_pArchive>>fTmp;
				pt.dx = fTmp;
				*m_pArchive>>fTmp;
				pt.dy = fTmp;
				
				CGIS_FeatureAddress * pTmp = new CGIS_FeatureAddress(nID);
				
				pTmp->SetAvoid( bAvoid );
				pTmp->SetVoice( bVoice );
				pTmp->SetAdminID( nAdminID );
				pTmp->SetCityID( nCityID );
				pTmp->SetMapID( nMapID );
				pTmp->SetAddressName( strAddName );
				pTmp->SetAdminName( strAdminName );
				pTmp->SetPhoneNum( strPhoneNum );
				pTmp->SetStreetName( strStreetName );
				pTmp->SetZipCode( strZipCode );
				pTmp->SetLocation( pt );
				
				pTList->AddTail( pTmp );
				
				if ( pTmp->GetAddressID() == nID )
				{
					pFAddress = pTmp;
				}
			}
			
//			m_pArchive->ReleaseRef();
		}
		else
		{
			POSITION pos = pTList->GetHeadPosition();
			while ( pos )
			{
				CGIS_FeatureAddress * pTmp = ( CGIS_FeatureAddress * )pTList->GetNext( pos );
				if ( pTmp )
				{
					if ( nID == pTmp->GetAddressID() )
					{
						pFAddress = pTmp;
					}
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		//���쳣����ʱ������Ѿ����ص�����
		int nCount = pTList->GetCount();
		for( int i = 0; i < nCount; i++ )
		{
			CGIS_FeatureAddress * pTmp = (CGIS_FeatureAddress*)pTList->GetAt( pTList->FindIndex(i) );
			if( pTmp )
			{
				delete pTmp;
				pTmp = NULL;
			}
		}

		pTList->RemoveAll();
		pFAddress = NULL;
		m_pArchive->ReleaseRef();

		THROW_LAST();
	}
	END_CATCH_ALL

	m_pArchive->ReleaseRef();
	
	return pFAddress;
}

//Histroy:
//2006-12-07	HB.Deng		�����쳣������д��ַ�����쳣ʱ������Ѿ������ĵ�ַ
//
BOOL CGIS_Middle::SaveFeatureAddress(CGIS_UserLayer * pUserLayer, swString strFPath, UINT nID)
{
	BOOL bCreate = CreateDirectory( strFPath, NULL );

	CPtrList * pList;
	pUserLayer->GetObjectListMap()->Lookup( 0, pList );

	TRY
	{
		int nNum = pList->GetCount();
		//����ͷ���ļ�
		BOOL bFlag = m_pArchive->AddRef( strFPath + L"\\Address.dat", IArchive::store );
		if ( !bFlag )
		{
			return FALSE;
		}
		
		*m_pArchive<<nNum;
		
		POSITION pos = pList->GetHeadPosition();
		while ( pos )
		{
			CGIS_FeatureAddress* pFAddress = ( CGIS_FeatureAddress * )pList->GetNext( pos );
			if ( pFAddress )
			{
				*m_pArchive<<pFAddress->GetAvoid()<<pFAddress->GetVoice()<<pFAddress->GetAdminID();
				*m_pArchive<<pFAddress->GetCityID()<<pFAddress->GetAddressID()<<pFAddress->GetMapID();
				*m_pArchive<<pFAddress->GetAddressName()<<pFAddress->GetAdminName()<<pFAddress->GetPhoneNum();
				*m_pArchive<<pFAddress->GetStreetName()<<pFAddress->GetZipCode();
				*m_pArchive<<pFAddress->GetLocation().dx<<pFAddress->GetLocation().dy;
			}
		}
	}
	CATCH_ALL(e)
	{
		//���쳣����ʱ������ڴ��е�����
		int nCount = pList->GetCount();
		for( int i = 0; i < nCount; i++ )
		{
			CGIS_FeatureAddress * pTmp = (CGIS_FeatureAddress*)pList->GetAt( pList->FindIndex(i) );
			if( pTmp )
			{
				delete pTmp;
				pTmp = NULL;
			}
		}
		
		pList->RemoveAll();

		m_pArchive->ReleaseRef();
		THROW_LAST();
	}
	END_CATCH_ALL

	m_pArchive->ReleaseRef();//�ر�

	return TRUE;
}


CGIS_Feature * CGIS_Middle::LoadFeature(CGIS_UserLayer *pLayer, swString strFPath, UINT nID )
{
	if ( !pLayer )
	{
		return NULL;
	}
	
	int nFeatureType = pLayer->GetUserMapLayerType();

	if ( EN_UMLAYERTYPE_ROUTE == nFeatureType )
	{
		return LoadFeatureRoute( pLayer, strFPath, nID ); //�켣·�ɶ�ȡ
	}
	else if ( EN_UMLAYERTYPE_TRACK == nFeatureType )
	{
		return LoadFeatureTrack( pLayer, strFPath, nID );
	}
	else if ( EN_UMLAYERTYPE_ADDRESSBOOK == nFeatureType )
	{
		return LoadFeatureAddress( pLayer, strFPath, nID );
	}
	else
	{
		return NULL;
	}
}

BOOL CGIS_Middle::SaveFeature(CGIS_UserLayer * pLayer, CGIS_Feature * pFeature, swString strFPath, UINT nID )
{
	if ( !pLayer )
	{
		return FALSE;
	}

	int nFeatureType = pLayer->GetUserMapLayerType();
	
	if ( EN_UMLAYERTYPE_ROUTE == nFeatureType )
	{
		return SaveFeatureRoute( pLayer, pFeature, strFPath, nID );
	}
	else if ( EN_UMLAYERTYPE_TRACK == nFeatureType )
	{
		return SaveFeatureTrack( pLayer, pFeature, strFPath, nID );
	}
	else if ( EN_UMLAYERTYPE_ADDRESSBOOK == nFeatureType )
	{
		return SaveFeatureAddress( pLayer, strFPath, nID );
	}
	else
	{
		return FALSE;
	}
}

BOOL CGIS_Middle::DeleteFeature(CGIS_UserLayer * pLayer, swString strFPath, UINT nID )
{
	if ( !pLayer )
	{
		return FALSE;
	}

	int nFeatureType = pLayer->GetUserMapLayerType();
	
	if ( EN_UMLAYERTYPE_ROUTE == nFeatureType )
	{
		return DeleteFeatureRoute( pLayer, strFPath, nID);
	}
	else if ( EN_UMLAYERTYPE_TRACK == nFeatureType )
	{
		return DeleteFeatureTrack( pLayer, strFPath, nID );
	}
	else if ( EN_UMLAYERTYPE_ADDRESSBOOK == nFeatureType )
	{
		return DeleteFeatureAddress( pLayer, strFPath, nID );
	}
	else
	{
		return FALSE;
	}
}

BOOL CGIS_Middle::DeleteFeatureTrack(CGIS_UserLayer * pLayer, swString strFPath, UINT nID)
{
	CPtrList * pList;
	pLayer->GetObjectListMap()->Lookup( 0, pList );

	CGIS_FeatureTrack * pFTrack = NULL;

	//���ڴ���ɾ��
	POSITION psPre = NULL;
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		psPre = pos;
		pFTrack = (CGIS_FeatureTrack *)pList->GetNext( pos );
		if ( pFTrack )
		{
			if ( nID == pFTrack->GetTrackFeatureID() )
			{
				delete pFTrack;
				pFTrack = NULL;

				pList->RemoveAt( psPre );
				break;
			}
		}
	}

/*
	//д��ͷ��Ϣ�ļ�
	BOOL bFlag = m_pArchive->Destroy( strFPath );//д��ǰ��ɾ��ԭ�ļ�
	if ( !bFlag )
	{
		return FALSE;
	}
*/
	BOOL b = m_pArchive->AddRef( strFPath + L"\\Track.dat", IArchive::store );
	if ( !b )
	{
		return FALSE;
	}

	int nNum = pList->GetCount();
	*m_pArchive<<nNum;

	pos = pList->GetHeadPosition();
	while ( pos )
	{
		pFTrack = (CGIS_FeatureTrack *)pList->GetNext( pos );
		if ( pFTrack )
		{
			*m_pArchive<<pFTrack->GetTrackFeatureID()<<pFTrack->GetTrackFeatureName();
		}
	}

	m_pArchive->ReleaseRef();

	//ɾ�������ļ�
	swString strTmp;// = strFPath.Left( strFPath.ReverseFind( L'\\' ) );
	strTmp.Format( L"%s\\%d.dat", strFPath, nID );

	return m_pArchive->Destroy( strTmp );
}

BOOL CGIS_Middle::DeleteFeatureAddress(CGIS_UserLayer * pLayer, swString strFPath, UINT nID)
{
	CPtrList * pList;
	pLayer->GetObjectListMap()->Lookup( 0, pList );

	CGIS_FeatureAddress * pFAddress = NULL;
	
	//���ڴ���ɾ��
	POSITION psPre = NULL;
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		psPre = pos;
		pFAddress = (CGIS_FeatureAddress *)pList->GetNext( pos );
		if ( pFAddress )
		{
			if ( nID == pFAddress->GetAddressID() )
			{
				delete pFAddress;
				pFAddress = NULL;

				pList->RemoveAt( psPre );
			}
		}
	}
/*	
	m_pArchive->AddRef( strFPath + L"\\Address.dat", IArchive::store );
	
	int nNum = pList->GetCount();
	*m_pArchive<<nNum;
	
	pos = pList->GetHeadPosition();
	while ( pos )
	{
		pFAddress = (CGIS_FeatureAddress *)pList->GetNext( pos );
		if ( pFAddress )
		{
			*m_pArchive<<pFAddress->GetAvoid()<<pFAddress->GetVoice()<<pFAddress->GetAdminID();
			*m_pArchive<<pFAddress->GetCityID()<<pFAddress->GetAddressID()<<pFAddress->GetMapID();
			*m_pArchive<<pFAddress->GetAddressName()<<pFAddress->GetAdminName()<<pFAddress->GetPhoneNum();
			*m_pArchive<<pFAddress->GetStreetName()<<pFAddress->GetZipCode();
			*m_pArchive<<pFAddress->GetLocation().dx<<pFAddress->GetLocation().dy;
		}
	}

	m_pArchive->ReleaseRef();
//*/

	return SaveFeatureAddress( pLayer, strFPath, nID );
}

BOOL CGIS_Middle::DeleteFeatureRoute(CGIS_UserLayer * pLayer, swString strFPath, UINT nID)
{
	CPtrList * pList;
	pLayer->GetObjectListMap()->Lookup( 0, pList );
	
	CGIS_FeatureRoute * pFRoute = NULL;
	
	//���ڴ���ɾ��
	POSITION psPre = NULL;
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		psPre = pos;
		pFRoute = (CGIS_FeatureRoute *)pList->GetNext( pos );
		if ( pFRoute )
		{
			if ( nID == pFRoute->GetNetworkRoute()->GetRouteID() )
			{
				delete pFRoute;
				pFRoute = NULL;
				
				pList->RemoveAt( psPre );
			}
		}
	}
	
	BOOL b = m_pArchive->AddRef( strFPath + L"\\Route.dat", IArchive::store );
	if ( !b )
	{
		return FALSE;
	}

	int nNum = pList->GetCount();
	*m_pArchive<<nNum;//����FeatureRoute����

	pos = pList->GetHeadPosition();
	while ( pos )
	{
		CGIS_FeatureRoute* pFRoute = ( CGIS_FeatureRoute * )pList->GetNext( pos );
		if ( pFRoute )
		{
			CGIS_NetworkRoute * pNWRouteTmp = pFRoute->GetNetworkRoute();

			*m_pArchive<<pNWRouteTmp->GetRouteID()<<pNWRouteTmp->GetRouteName();

			CPtrList * pPList = pNWRouteTmp->GetWayPointList();
			nNum = pPList->GetCount();
			
			*m_pArchive<<nNum;//����waypoint�ĸ���
			
			if ( pPList )
			{
				POSITION posWPoint = pPList->GetHeadPosition();
				while ( posWPoint )
				{
					CGIS_WayPoint * pWPoint = (CGIS_WayPoint *)pPList->GetNext( posWPoint );
					if ( pWPoint )
					{
						UINT		uID		 =	 pWPoint->GetWPID();//;����ID
						ST_GEO_PTXY pt		 =	 pWPoint->GetWPPosition();//;����λ��
						int			wpType	 =	 pWPoint->GetWPType();//����
						swString		strAnno;//	 =	 pWPoint->GetWPAnnoation();//��ע
						CharToUnicode( pWPoint->GetWPAnnoation() , &strAnno );
						BYTE		nAnnoNum =	 pWPoint->GetWPAnnoNum();//��ע����
						swString		strWPRName = pWPoint->GetWPRutName();
						*m_pArchive<<uID<<pt.dx<<pt.dy<<wpType<<strAnno<<nAnnoNum<<strWPRName;
					}
				}
			}
		}
	}
	
	m_pArchive->ReleaseRef();

	//ɾ�������ļ�
	swString strTmp;
	strTmp.Format( L"%s\\%d.dat", strFPath, nID );
	
	m_pArchive->Destroy( strTmp );
	
	return m_pArchive->Destroy( strTmp );
}

BOOL CGIS_Middle::SaveCurrentRoute(CGIS_Feature *pFeature,swString strFPath)
{
	BOOL bCreate = CreateDirectory( strFPath, NULL );

	BOOL b = m_pArchive->AddRef(strFPath + L"\\CurrentRoute.dat", IArchive::store);
	if ( !b )
	{
		return FALSE;
	}

	CGIS_FeatureRoute * pCurrentRoute = (CGIS_FeatureRoute *)pFeature;
	CGIS_NetworkRoute * pNWRoute = pCurrentRoute->GetNetworkRoute();
	
	*m_pArchive<<pNWRoute->GetRouteID()<<pNWRoute->GetRouteName();
	
	CPtrList * pPList = pNWRoute->GetWayPointList();
	int nNum = pPList->GetCount();
	
	*m_pArchive<<nNum;//����waypoint�ĸ���
	
	if ( pPList )
	{
		POSITION pos = pPList->GetHeadPosition();
		while ( pos )
		{
			CGIS_WayPoint * pWPoint = (CGIS_WayPoint *)pPList->GetNext( pos );
			if ( pWPoint )
			{
				UINT		uID		 =	 pWPoint->GetWPID();//;����ID
				ST_GEO_PTXY pt		 =	 pWPoint->GetWPPosition();//;����λ��
				int			wpType	 =	 pWPoint->GetWPType();//����
				swString		strAnno;//	 =	 pWPoint->GetWPAnnoation();//��ע
				CharToUnicode( pWPoint->GetWPAnnoation() , &strAnno );
				BYTE		nAnnoNum =	 pWPoint->GetWPAnnoNum();//��ע����
				swString		strWPRName = pWPoint->GetWPRutName();
				*m_pArchive<<uID<<pt.dx<<pt.dy<<wpType<<strAnno<<nAnnoNum<<strWPRName;
			}
		}
	}

	CGIS_NetworkPath * pNWPath = pNWRoute->GetNetworkPath();
	if ( pNWPath )
	{
		CPtrList * pPPList = pNWPath->GetPathpointList();
		nNum = pPPList->GetCount();
		*m_pArchive<<nNum;//����pathpoint�ĸ���

		POSITION posPP =  pPPList->GetHeadPosition();
		while ( posPP )
		{
			CGIS_Pathpoint * pPathPoint = (CGIS_Pathpoint *)pPPList->GetNext( posPP );
			if ( pPathPoint )
			{
				int nPType = pPathPoint->GetPathpointType();
				ST_GEO_PTXY pt = pPathPoint->GetPathpointPosition();
				*m_pArchive<<nPType;//��������
				*m_pArchive<<pt.dx<<pt.dy;//��������
//==========================================================================================
				
				if ( nPType == EN_PATHPOINTTYPE_NODE )
				{
					CGIS_PathpointNode * pPNode = (CGIS_PathpointNode *)pPathPoint;
					*m_pArchive<<pPNode->GetNextRutID();
					*m_pArchive<<pPNode->GetNodeVtxID();
					*m_pArchive<<pPNode->GetRType();
					*m_pArchive<<pPNode->GetRutType();
					*m_pArchive<<pPNode->GetPBreadthID();
					*m_pArchive<<pPNode->GetRMOrder();
					*m_pArchive<<pPNode->GetRutAnnoNum();
					*m_pArchive<<pPNode->GetCRect().dLeft<<pPNode->GetCRect().dTop<<pPNode->GetCRect().dRight<<pPNode->GetCRect().dBottom;
					if ( pPNode->GetRutAnnoNum() > 0 )
					{
						swString strTmp;// = pPNode->GetRutAnno();
						CharToUnicode( pPNode->GetRutAnno() , &strTmp );
						*m_pArchive<<strTmp;
					}
					ST_VOICE_NODE * node =  pPNode->GetVNodeInfo();
					*m_pArchive<<node->byAngle;
					*m_pArchive<<node->byConfusion;
					*m_pArchive<<node->byConfusion2;
					*m_pArchive<<node->byDestination;
					*m_pArchive<<node->byEnName;
					*m_pArchive<<node->byExName;
					*m_pArchive<<node->byIMI;
					*m_pArchive<<node->byOtherStreet;
					*m_pArchive<<node->byRound;
					*m_pArchive<<node->bySameName;
					*m_pArchive<<node->bySameRoad;
					*m_pArchive<<node->byTurn;
					ST_VOICE_RUT * rut = pPNode->GetVRutInfo();
					*m_pArchive<<rut->byAdd<<rut->byCharge<<rut->byDrivable<<rut->byType; 
					*m_pArchive<<pPNode->GetTurnAngle();

					nNum = pPNode->GetNodeRutList()->GetCount();
					*m_pArchive<<nNum;
					
					CGIS_NetworkNodeRut *pNRut = NULL;
					POSITION ps = pPNode->GetNodeRutList()->GetHeadPosition();
					while ( ps )
					{
						pNRut = (CGIS_NetworkNodeRut*)pPNode->GetNodeRutList()->GetNext(ps);
						if( pNRut )
						{
							nNum = pNRut->GetRutID();
							*m_pArchive<<pNRut->GetRutID()<<pNRut->GetRutAnnoNum();
							if ( pNRut->GetRutAnnoNum() > 0 )
							{
								swString str;// = pNRut->GetRutAnno();
								CharToUnicode( pNRut->GetRutAnno() , &str );
								*m_pArchive<<str;
							}
							nNum = pNRut->GetRutPtNum();
//							*m_pArchive<<nNum<<pNRut->GetRutWidth();
							*m_pArchive<<nNum;
							ST_GEO_PTXY * pt = pNRut->GetRutPtList();
							ASSERT( pt );
							while ( nNum-- )
							{
								*m_pArchive<<pt->dx<<pt->dy;
								pt++;
							}
						}
					}
				}
				else if ( nPType == EN_PATHPOINTTYPE_START||
							nPType == EN_PATHPOINTTYPE_INTERMEDIATE||
							nPType == EN_PATHPOINTTYPE_END) 
				{
					CGIS_PathpointMark * pPMark = (CGIS_PathpointMark *)pPathPoint;
					nNum = pPMark->GetWayPoint()->GetWPID();
					ST_VOICE_RUT * pVRut = pPMark->GetVRutInfo();
					ST_VOICE_NODE * pVNode = pPMark->GetVNodeInfo();
					
					*m_pArchive<<nNum;
					*m_pArchive<<pVRut->byAdd<<pVRut->byCharge<<pVRut->byDrivable<<pVRut->byType;
					
					*m_pArchive<<pVNode->byAngle;
					*m_pArchive<<pVNode->byConfusion;
					*m_pArchive<<pVNode->byConfusion2;
					*m_pArchive<<pVNode->byDestination;
					*m_pArchive<<pVNode->byEnName;
					*m_pArchive<<pVNode->byExName;
					*m_pArchive<<pVNode->byIMI;
					*m_pArchive<<pVNode->byOtherStreet;
					*m_pArchive<<pVNode->byRound;
					*m_pArchive<<pVNode->bySameName;
					*m_pArchive<<pVNode->bySameRoad;
					*m_pArchive<<pVNode->byTurn;
				}
			}
		}
	}

	m_pArchive->ReleaseRef();
	
	return TRUE;
}


//========================================================================
// ���溽��
//
// ������pRoute���߽ṹ��strFPath����·��
//========================================================================
BOOL CGIS_Middle::SaveRoute(CGIS_Feature *pRoute, swString strFPath)
{
	// �����ļ�·��
	int nPos = strFPath.ReverseFind(_T('\\'));
	swString strDir = strFPath.Left(nPos);
	::CreateDirectory(strDir,  NULL);

	BOOL b = m_pArchive->AddRef(strFPath, IArchive::store);
	if ( !b )
		return FALSE;

	CGIS_FeatureRoute * pCurrentRoute = (CGIS_FeatureRoute *)pRoute;
	CGIS_NetworkRoute * pNWRoute = pCurrentRoute->GetNetworkRoute();
	
	*m_pArchive<<pNWRoute->GetRouteID()<<pNWRoute->GetRouteName();
	
	CPtrList * pPList = pNWRoute->GetWayPointList();
	int nNum = pPList->GetCount();
	
	*m_pArchive<<nNum;//����waypoint�ĸ���
	
	if ( pPList )
	{
		POSITION pos = pPList->GetHeadPosition();
		while ( pos )
		{
			CGIS_WayPoint * pWPoint = (CGIS_WayPoint *)pPList->GetNext( pos );
			if ( pWPoint )
			{
				UINT		uID		 =	 pWPoint->GetWPID();//;����ID
				ST_GEO_PTXY pt		 =	 pWPoint->GetWPPosition();//;����λ��
				int			wpType	 =	 pWPoint->GetWPType();//����
				swString		strAnno;//	 =	 pWPoint->GetWPAnnoation();//��ע
				CharToUnicode( pWPoint->GetWPAnnoation() , &strAnno );
				BYTE		nAnnoNum =	 pWPoint->GetWPAnnoNum();//��ע����
				swString		strWPRName = pWPoint->GetWPRutName();
				*m_pArchive<<uID<<pt.dx<<pt.dy<<wpType<<strAnno<<nAnnoNum<<strWPRName;
			}
		}
	}

	CGIS_NetworkPath * pNWPath = pNWRoute->GetNetworkPath();
	if ( pNWPath )
	{
		CPtrList * pPPList = pNWPath->GetPathpointList();
		nNum = pPPList->GetCount();
		*m_pArchive<<nNum;//����pathpoint�ĸ���

		POSITION posPP =  pPPList->GetHeadPosition();
		while ( posPP )
		{
			CGIS_Pathpoint * pPathPoint = (CGIS_Pathpoint *)pPPList->GetNext( posPP );
			if ( pPathPoint )
			{
				int nPType = pPathPoint->GetPathpointType();
				ST_GEO_PTXY pt = pPathPoint->GetPathpointPosition();
				*m_pArchive<<nPType;//��������
				*m_pArchive<<pt.dx<<pt.dy;//��������
//==========================================================================================
				
				if ( nPType == EN_PATHPOINTTYPE_NODE )
				{
					CGIS_PathpointNode * pPNode = (CGIS_PathpointNode *)pPathPoint;
					*m_pArchive<<pPNode->GetNextRutID();
					*m_pArchive<<pPNode->GetNodeVtxID();
					*m_pArchive<<pPNode->GetRType();
					*m_pArchive<<pPNode->GetRutType();
					*m_pArchive<<pPNode->GetPBreadthID();
					*m_pArchive<<pPNode->GetRMOrder();
					*m_pArchive<<pPNode->GetRutAnnoNum();
					*m_pArchive<<pPNode->GetCRect().dLeft<<pPNode->GetCRect().dTop<<pPNode->GetCRect().dRight<<pPNode->GetCRect().dBottom;
					if ( pPNode->GetRutAnnoNum() > 0 )
					{
						swString strTmp;// = pPNode->GetRutAnno();
						CharToUnicode( pPNode->GetRutAnno() , &strTmp );
						*m_pArchive<<strTmp;
					}
					ST_VOICE_NODE * node =  pPNode->GetVNodeInfo();
					*m_pArchive<<node->byAngle;
					*m_pArchive<<node->byConfusion;
					*m_pArchive<<node->byConfusion2;
					*m_pArchive<<node->byDestination;
					*m_pArchive<<node->byEnName;
					*m_pArchive<<node->byExName;
					*m_pArchive<<node->byIMI;
					*m_pArchive<<node->byOtherStreet;
					*m_pArchive<<node->byRound;
					*m_pArchive<<node->bySameName;
					*m_pArchive<<node->bySameRoad;
					*m_pArchive<<node->byTurn;
					ST_VOICE_RUT * rut = pPNode->GetVRutInfo();
					*m_pArchive<<rut->byAdd<<rut->byCharge<<rut->byDrivable<<rut->byType; 
					*m_pArchive<<pPNode->GetTurnAngle();

					nNum = pPNode->GetNodeRutList()->GetCount();
					*m_pArchive<<nNum;
					
					CGIS_NetworkNodeRut *pNRut = NULL;
					POSITION ps = pPNode->GetNodeRutList()->GetHeadPosition();
					while ( ps )
					{
						pNRut = (CGIS_NetworkNodeRut*)pPNode->GetNodeRutList()->GetNext(ps);
						if( pNRut )
						{
							nNum = pNRut->GetRutID();
							*m_pArchive<<pNRut->GetRutID()<<pNRut->GetRutAnnoNum();
							if ( pNRut->GetRutAnnoNum() > 0 )
							{
								swString str;// = pNRut->GetRutAnno();
								CharToUnicode( pNRut->GetRutAnno() , &str );
								*m_pArchive<<str;
							}
							nNum = pNRut->GetRutPtNum();
//							*m_pArchive<<nNum<<pNRut->GetRutWidth();
							*m_pArchive<<nNum;
							ST_GEO_PTXY * pt = pNRut->GetRutPtList();
							ASSERT( pt );
							while ( nNum-- )
							{
								*m_pArchive<<pt->dx<<pt->dy;
								pt++;
							}
						}
					}
				}
				else if ( nPType == EN_PATHPOINTTYPE_START||
							nPType == EN_PATHPOINTTYPE_INTERMEDIATE||
							nPType == EN_PATHPOINTTYPE_END) 
				{
					CGIS_PathpointMark * pPMark = (CGIS_PathpointMark *)pPathPoint;
					nNum = pPMark->GetWayPoint()->GetWPID();
					ST_VOICE_RUT * pVRut = pPMark->GetVRutInfo();
					ST_VOICE_NODE * pVNode = pPMark->GetVNodeInfo();
					
					*m_pArchive<<nNum;
					*m_pArchive<<pVRut->byAdd<<pVRut->byCharge<<pVRut->byDrivable<<pVRut->byType;
					
					*m_pArchive<<pVNode->byAngle;
					*m_pArchive<<pVNode->byConfusion;
					*m_pArchive<<pVNode->byConfusion2;
					*m_pArchive<<pVNode->byDestination;
					*m_pArchive<<pVNode->byEnName;
					*m_pArchive<<pVNode->byExName;
					*m_pArchive<<pVNode->byIMI;
					*m_pArchive<<pVNode->byOtherStreet;
					*m_pArchive<<pVNode->byRound;
					*m_pArchive<<pVNode->bySameName;
					*m_pArchive<<pVNode->bySameRoad;
					*m_pArchive<<pVNode->byTurn;
				}
			}
		}
	}

	m_pArchive->ReleaseRef();
	
	return TRUE;
}


CGIS_Feature* CGIS_Middle::LoadCurrentRoute( swString strFPath )
{
	return LoadRoute(strFPath + L"\\CurrentRoute.dat");
}

//========================================================================
// ����·�����غ���
//
// ����������·��
//========================================================================
CGIS_Feature* CGIS_Middle::LoadRoute(swString strFullPath)
{
	CGIS_FeatureRoute *pFRoute = NULL;
	TRY
	{
		int nNum = 0;
		BOOL bFlag = TRUE;
		char szTmp[MAX_PATH];
		int nLen = 0;
		
		bFlag = m_pArchive->AddRef(strFullPath, IArchive::load); //�򿪺��������ļ�
		if ( !bFlag )
			return NULL;
		
		int		nRouteID		= 0;
		swString	strRouteName	= "";
		
		*m_pArchive>>nRouteID>>strRouteName;//������źͺ�������
		
		pFRoute = new CGIS_FeatureRoute( nRouteID );
		CGIS_NetworkRoute * pNWRoute = pFRoute->GetNetworkRoute();
		pNWRoute->SetRouteName( strRouteName );
		
		CPtrList * pWPList = pNWRoute->GetWayPointList();//�����ϵ�;����wayPoint
		
		UINT		uID = 0;//;����ID
		int			wpType = 0;//����
		swString		strAnno = "";//��ע
		swString		strWPRName = "";
		BYTE		nAnnoNum = 0;//��ע����
		float		fTmp = 0.0f;
		int nCount = 0;
		*m_pArchive>>nCount;
		
		ST_GEO_PTXY pt;//;����λ��
		pt.dx = 0.0f;
		pt.dy = 0.0f;
		
		//��ȡwaypoint
		while ( nCount-- )
		{
			*m_pArchive>>uID;
			*m_pArchive>>fTmp;
			pt.dx = fTmp;
			*m_pArchive>>fTmp;
			pt.dy = fTmp;
			*m_pArchive>>wpType>>strAnno>>nAnnoNum>>strWPRName;
			
			nLen = UnicodeToChar( strAnno, szTmp, MAX_PATH );
			char * szAnno = NULL;
			if ( nLen )
			{
				szAnno = new char[ nLen ];
				memcpy( szAnno, szTmp, nLen );
				szAnno[nLen-1] = '\0';
			}
			
			CGIS_WayPoint * pWPoint = new CGIS_WayPoint( uID );
			pWPoint->SetWPPosition( pt );
			pWPoint->SetWPType( (EnWaypointType)wpType );
			pWPoint->SetWPAnnoation( szAnno );
			pWPoint->SetWPAnnoNum( nAnnoNum );
			pWPoint->SetWPRutName( strWPRName );
			
			pWPList->AddTail( pWPoint );
		}
		
		//��ȡpath
		int nPType = 0;
		pt.dx = 0;
		pt.dy = 0;
		*m_pArchive>>nNum;
		
		CGIS_NetworkPath * pNWPath = pNWRoute->GetNetworkPath();
		CPtrList * pPathPointList = pNWPath->GetPathpointList();
		CGIS_Pathpoint * pPPoint = NULL;
		while ( nNum-- )
		{
			pPPoint = NULL;
			*m_pArchive>>nPType;
			*m_pArchive>>fTmp;
			pt.dx = fTmp;
			*m_pArchive>>fTmp;
			pt.dy = fTmp;
			///*
			if ( nPType == EN_PATHPOINTTYPE_NODE )
			{
				CGIS_PathpointNode * pPNode = new CGIS_PathpointNode;
				pPNode->SetPathpointType( (EnPathpointType)nPType );
				pPNode->SetPathpointPosition( pt );
				
				int nNextRutID = 0;	//�¸�·�ε�ID
				int nNodeVtxID = 0;	//��ǰ�ڵ��ID
				BYTE nAnnoNum   = 0;
				BYTE byType = 0;
				BYTE byRutType = 0;
				int nBID = 0;
				swString strAnno = "";
				int nTurnAngle = 0;
				short nRMOrd = 0;
				ST_GEO_RECT rtCross;
				
				*m_pArchive>>nNextRutID>>nNodeVtxID>>byType>>byRutType>>nBID>>nRMOrd>>nAnnoNum;
				pPNode->SetNextRutID( nNextRutID );
				pPNode->SetNodeVtxID( nNodeVtxID );
				pPNode->SetRType(byType);
				pPNode->SetRutType(byRutType);
				pPNode->SetPBreadthID(nBID);
				pPNode->SetRMOrder( nRMOrd );
				*m_pArchive>>fTmp;
				rtCross.dLeft = fTmp;
				*m_pArchive>>fTmp;
				rtCross.dTop = fTmp;
				*m_pArchive>>fTmp;
				rtCross.dRight = fTmp;
				*m_pArchive>>fTmp;
				rtCross.dBottom = fTmp;
				pPNode->SetCRect( rtCross );
				if ( nAnnoNum>0 )
				{
					*m_pArchive>>strAnno;
					char * szAnno = new char[ nAnnoNum ];
					UnicodeToChar( strAnno, szAnno, nAnnoNum );
					pPNode->SetRutAnno( szAnno );
				}
				pPNode->SetRutAnnoNum( nAnnoNum );
				
				ST_VOICE_NODE * pNode = pPNode->GetVNodeInfo();
				BYTE byTmp = 0;
				
				*m_pArchive>>byTmp;
				pNode->byAngle = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->byConfusion = byTmp;
				*m_pArchive>>byTmp;
				pNode->byConfusion2 = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->byDestination = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->byEnName = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->byExName = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->byIMI = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->byOtherStreet = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->byRound = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->bySameName = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->bySameRoad = byTmp;
				
				*m_pArchive>>byTmp;
				pNode->byTurn = byTmp;
				
				ST_VOICE_RUT * pRut = pPNode->GetVRutInfo();
				
				*m_pArchive>>byTmp;
				pRut->byAdd = byTmp;
				
				*m_pArchive>>byTmp;
				pRut->byCharge = byTmp;
				
				*m_pArchive>>byTmp;
				pRut->byDrivable = byTmp;
				
				*m_pArchive>>byTmp;
				pRut->byType = byTmp;
				
				*m_pArchive>>nTurnAngle;
				pPNode->SetTurnAngle( nTurnAngle );
				
				int nNum1 = 0;
				*m_pArchive>>nNum1;
				while ( nNum1-- )
				{
					CGIS_NetworkNodeRut *pNRut = new CGIS_NetworkNodeRut;
					int nRID = 0;
					
					*m_pArchive>>nRID;
					pNRut->SetRutID( nRID );
					BYTE nTmp = 0;
					*m_pArchive>>nTmp;
					pNRut->SetRutAnnoNum( nTmp );
					if ( nTmp > 0 )
					{
						swString str = "";
						*m_pArchive>>str;
						char * szStr = new char[nTmp];
						UnicodeToChar( str, szStr, nTmp );
						pNRut->SetRutAnno( szStr );
					}
					int nPtCount = 0;
					*m_pArchive>>nPtCount;
					pNRut->SetRutPtNum( nPtCount );
					//				BYTE bWidth = 0;
					//				*m_pArchive>>bWidth;
					//				pNRut->SetRutWidth( bWidth );
					
					ST_GEO_PTXY * pPt = new ST_GEO_PTXY[nPtCount];
					ST_GEO_PTXY * pTmp = pPt;
					while ( nPtCount-- )
					{
						*m_pArchive>>fTmp;
						pTmp->dx = fTmp;
						*m_pArchive>>fTmp;
						pTmp->dy = fTmp;
						
						pTmp++;
					}
					
					pNRut->SetRutPtList( pPt );
					pPNode->GetNodeRutList()->AddTail( pNRut );
				}
				
				pPPoint = pPNode;
		}
		else if ( nPType == EN_PATHPOINTTYPE_START||
			nPType == EN_PATHPOINTTYPE_INTERMEDIATE||
			nPType == EN_PATHPOINTTYPE_END )
		{
			CGIS_PathpointMark * pPMark = new CGIS_PathpointMark;
			
			pPMark->SetPathpointType( (EnPathpointType)nPType );
			pPMark->SetPathpointPosition( pt );
			
			ST_VOICE_RUT * pVRut = pPMark->GetVRutInfo();
			ST_VOICE_NODE * pVNode = pPMark->GetVNodeInfo();
			
			int nID = 0;
			*m_pArchive>>nID;
			CGIS_WayPoint *pWPoint = pFRoute->GetNetworkRoute()->GetWayPointByID( nID );
			if( pWPoint )
			{
				pPMark->SetWayPoint( pWPoint );
				pWPoint->SetWPProjPosition( pt, EN_MAPTYPE_NATION );
			}
			
			BYTE byTmp = 0;
			
			*m_pArchive>>byTmp;
			pVRut->byAdd = byTmp;
			*m_pArchive>>byTmp;
			pVRut->byCharge = byTmp;
			*m_pArchive>>byTmp;
			pVRut->byDrivable = byTmp;
			*m_pArchive>>byTmp;
			pVRut->byType = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->byAngle = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->byConfusion = byTmp;
			*m_pArchive>>byTmp;
			pVNode->byConfusion2 = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->byDestination = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->byEnName = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->byExName = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->byIMI = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->byOtherStreet = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->byRound = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->bySameName = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->bySameRoad = byTmp;
			
			*m_pArchive>>byTmp;
			pVNode->byTurn = byTmp;
			
			pPPoint = pPMark;
		}
		else if ( nPType == EN_PATHPOINTTYPE_INFLEXION )
		{
			pPPoint = new CGIS_Pathpoint;
			pPPoint->SetPathpointType( (EnPathpointType)nPType );
			pPPoint->SetPathpointPosition( pt );
		}
		
		pPathPointList->AddTail( pPPoint );			
	}
	
	m_pArchive->ReleaseRef();
	}
	CATCH_ALL(e)
	{
		//�����쳣ʱ��ɾ���ļ�
		m_pArchive->ReleaseRef();
		delete pFRoute;
		pFRoute = NULL;
		::DeleteFile(strFullPath);
		return NULL;
	}
	END_CATCH_ALL

	return pFRoute;
}


DWORD CGIS_Middle::GetFeatureSize( swString strFPath,  UINT nID )
{
	BOOL b = m_pArchive->AddRef( strFPath, IArchive::load, nID );
	if ( !b )
	{
		return 0;
	}
	
	DWORD dwFileSize = ((CWinArchive*)m_pArchive)->GetFileSize();
	m_pArchive->ReleaseRef();

	return dwFileSize;
}

BOOL CGIS_Middle::DeleteAllFeatureRoute(CGIS_UserLayer * pLayer, swString strFPath)
{
	//step 1:delete all from memory
	CPtrList * pList;
	pLayer->GetObjectListMap()->Lookup( 0, pList );
	
	CGIS_FeatureRoute * pFRoute = NULL;
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		pFRoute = (CGIS_FeatureRoute *)pList->GetNext( pos );
		if ( pFRoute )
		{
			delete pFRoute;
			pFRoute = NULL;
		}
	}

	pList->RemoveAll();

	//step 2:delete all files
	WIN32_FIND_DATA findData;
	BOOL	bFileExist = TRUE;
	HANDLE hFind = ::FindFirstFile(strFPath + _T("\\*.*"),&findData);

	while( hFind != INVALID_HANDLE_VALUE && bFileExist )
	{
		
		//��ɾ����ǰ����
		if( wcscmp( findData.cFileName,_T("CurrentRoute.dat") ) == 0 )
		{
			bFileExist = ::FindNextFile(hFind,&findData);
			continue;
		}

		::DeleteFile( strFPath + _T("\\") +findData.cFileName );

		bFileExist = ::FindNextFile(hFind,&findData);
	}

	::FindClose(hFind);

	return TRUE;
}

BOOL CGIS_Middle::DeleteAllFeatureTrack(CGIS_UserLayer *pLayer, swString strFPath)
{
	//step 1:delete all from memory
	CPtrList * pList;
	pLayer->GetObjectListMap()->Lookup( 0, pList );
	
	CGIS_FeatureTrack * pTrack = NULL;
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		pTrack = (CGIS_FeatureTrack *)pList->GetNext( pos );
		if ( pTrack )
		{
			delete pTrack;
			pTrack = NULL;
		}
	}
	
	pList->RemoveAll();
	
	//step 2:delete all files
	WIN32_FIND_DATA findData;
	BOOL	bFileExist = TRUE;
	HANDLE hFind = ::FindFirstFile(strFPath + _T("\\*.*"),&findData);
	
	while( hFind != INVALID_HANDLE_VALUE && bFileExist )
	{
		::DeleteFile( strFPath + _T("\\") + findData.cFileName );
		
		bFileExist = ::FindNextFile(hFind,&findData);
	}
	
	::FindClose(hFind);

	return TRUE;
}

BOOL CGIS_Middle::DeleteAllFeature(CGIS_UserLayer * pLayer, swString strFPath)
{
	if ( !pLayer )
	{
		return FALSE;
	}
	
	int nFeatureType = pLayer->GetUserMapLayerType();
	
	if ( EN_UMLAYERTYPE_ROUTE == nFeatureType )
	{
		return DeleteAllFeatureRoute( pLayer, strFPath);
	}
	else if ( EN_UMLAYERTYPE_TRACK == nFeatureType )
	{
		return DeleteAllFeatureTrack( pLayer, strFPath );
	}
	else if( EN_UMLAYERTYPE_ADDRESSBOOK == nFeatureType )
	{
		return DeleteAllFeatureAddress( pLayer, strFPath );
	}
	else
	{
		return FALSE;
	}
}

BOOL CGIS_Middle::DeleteAllFeatureAddress(CGIS_UserLayer *pLayer, swString strFPath)
{
	CPtrList * pList;
	pLayer->GetObjectListMap()->Lookup( 0, pList );
	
	CGIS_FeatureAddress * pAddr = NULL;
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		pAddr = (CGIS_FeatureAddress *)pList->GetNext( pos );
		if ( pAddr )
		{
			delete pAddr;
			pAddr = NULL;
		}
	}
	
	pList->RemoveAll();

	return 	::DeleteFile( strFPath + L"\\Address.dat" );
}
