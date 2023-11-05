
#include "stdafx.h"
#include "SDELib.h"
#include "GIS_UserLayer.h"
#include "GIS_Feature.h"
#include "GIS_FeatureRoute.h"
#include "GIS_FeatureTrack.h"
#include "GIS_Address.h"
#include "GIS_UFNavi.h"
#include "GIS_Middle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif


CGIS_UserLayer::CGIS_UserLayer(CGIS_LayerInfo *pInfo)
{
	m_enLType = EN_LAYTYPE_USER;
	m_pInfo = pInfo;
	m_pLAttInfo = NULL;

	m_pMiddle = new CGIS_Middle;//Added by HB.Deng
}

CGIS_UserLayer::CGIS_UserLayer(CGIS_LayerInfo *pInfo,CGIS_LayerAttInfo *pLAttInfo)
{
	m_enLType = EN_LAYTYPE_USER;
	m_pInfo = pInfo;
	m_pLAttInfo = pLAttInfo;
	
	m_pMiddle = new CGIS_Middle;//Added by HB.Deng
}

CGIS_UserLayer::~CGIS_UserLayer()
{
	if( m_pLAttInfo )
		delete m_pLAttInfo;
	m_pLAttInfo = NULL;

	if ( m_pMiddle )
	{
		delete m_pMiddle;
		m_pMiddle =NULL;
	}
}
void CGIS_UserLayer::SetUserMapLayerType( EnUserMapLayerType enUMLType )
{
	m_enUMLType = enUMLType;
}
EnUserMapLayerType CGIS_UserLayer::GetUserMapLayerType( )
{
	return m_enUMLType;
}
CGIS_LayerAttInfo* CGIS_UserLayer::GetLayerAttInfo( )
{
	return m_pLAttInfo;
}
BOOL CGIS_UserLayer::LoadLayer( swString strFPath )
{
	CGIS_UFNavi *pUFNavi = CGIS_UFNavi::GetUFNavi();
	if( pUFNavi )
		return pUFNavi->LoadNaviLayer( this, strFPath );
	return FALSE;
}
CGIS_Feature* CGIS_UserLayer::LoadFeatureObject( UINT nID, swString strFPath, int nFlag/* = 0*/ )
{
	ASSERT( nFlag == 0 || nFlag == 1 );

	if ( nFlag == 0 )
	{
		return m_pMiddle->LoadFeature( this, strFPath, nID );
	}
	else
		return m_pMiddle->LoadCurrentRoute( strFPath );
}


//========================================================================
// 根据名称加载航线
//
// strFullPath(in)：完整路径，返回：航线信息
//========================================================================
CGIS_Feature* CGIS_UserLayer::LoadFeatureObject(swString strFullPath)
{
	if (strFullPath.IsEmpty())
		return NULL;
	return m_pMiddle->LoadRoute(strFullPath);
}

//========================================================================
// 将航线保存到完整路径
//
// pFeature(in)：需要保存的对象，strFPath(in)：对象保存的完整路径
//========================================================================
BOOL CGIS_UserLayer::SaveFeatureObject(CGIS_Feature *pFeature, swString strFPath)
{
	ASSERT(pFeature != NULL);
	return m_pMiddle->SaveRoute(pFeature, strFPath);
}


CGIS_Feature* CGIS_UserLayer::CreateFeature( )
{
	UINT nMaxID = GetFeatureMaxID();
	CGIS_Feature *pFeature = NULL;
	CPtrList *pOList = NULL;

	if( m_enUMLType == EN_UMLAYERTYPE_ROUTE )
	{
		pFeature = new CGIS_FeatureRoute( nMaxID+1 );

//		m_pObjListMap->Lookup( 0, pOList );
//		pOList->AddTail( pFeature );
	}
	else if( m_enUMLType == EN_UMLAYERTYPE_TRACK )
	{
		pFeature = new CGIS_FeatureTrack( nMaxID+1 );
		CPtrList *pOList;
//		m_pObjListMap->Lookup( 0, pOList );
//		pOList->AddTail( pFeature );
	}
	else if( m_enUMLType == EN_UMLAYERTYPE_ADDRESSBOOK )
	{
		pFeature = new CGIS_FeatureAddress( nMaxID+1 );
		CPtrList *pOList;
		m_pObjListMap->Lookup( 0, pOList );
		pOList->AddTail( pFeature );
	}
	return pFeature;
}
BOOL CGIS_UserLayer::DeleteFeature( /*CGIS_Feature *pFeature*/UINT nID, swString strFPath )
{
/*
	UINT nID = 0;
	if( m_enUMLType == EN_UMLAYERTYPE_ROUTE )
		nID = ((CGIS_FeatureRoute*)pFeature)->GetNetworkRoute()->GetRouteID();
	else if( m_enUMLType == EN_UMLAYERTYPE_TRACK )
		nID = ((CGIS_FeatureTrack*)pFeature)->GetTrackFeatureID();
	CPtrList *pOList;
	m_pObjListMap->Lookup( 0, pOList );
	POSITION pos = pOList->GetHeadPosition();
	CGIS_Feature *pFTem = NULL;
	POSITION ps = NULL;
	while( pos )
	{
		ps = pos;
		pFTem = (CGIS_Feature*)pOList->GetNext( pos );
		if( pFTem )
		{
			if( pFTem == pFeature )
			{
				delete pFTem;
				pOList->RemoveAt(ps);
				break;
			}
		}
	}
	CGIS_UFNavi *pUFNavi = CGIS_UFNavi::GetUFNavi();
	if( pUFNavi )
		return pUFNavi->SaveNaviLayer( this, nID, strFPath );
*/

	m_pMiddle->DeleteFeature( this, strFPath, nID );
	
	return TRUE;
}

//增加了参数 nFlag,用来标识是增加Feature还是更新Feature.0-增加,1-更新
//11/07/2005 by HB.Deng
//07/09/2005 by HB.Deng		去掉了nFlag参数,多余
//26/09/2005 by HB.Deng		增加nFlag,保存当前航线标识1
BOOL CGIS_UserLayer::UpdateFeature( CGIS_Feature *pFeature, swString strFPath, int nFlag/* = 0*/ )
{
	if ( nFlag == 1 )
	{
		return m_pMiddle->SaveCurrentRoute( pFeature, strFPath );
	}
	else
	{
		UINT nID = 0;
		if( m_enUMLType == EN_UMLAYERTYPE_ROUTE )
			nID = ((CGIS_FeatureRoute*)pFeature)->GetNetworkRoute()->GetRouteID();
		else if( m_enUMLType == EN_UMLAYERTYPE_TRACK )
			nID = ((CGIS_FeatureTrack*)pFeature)->GetTrackFeatureID();
		
		return m_pMiddle->SaveFeature( this, pFeature, strFPath, nID );
	}
}

BOOL CGIS_UserLayer::EstimateDrawCondition( double dbScale, double dbMinGRScale, double dbMaxGRScale )
{
	if( dbScale< dbMinGRScale	||	dbScale > dbMaxGRScale )
		return FALSE;
	return TRUE;
}

void CGIS_UserLayer::DrawLayer( IGIS_LayerFile *pLFile, IGIS_MapView *pView, swRect rtPolt, ST_GEO_RECT rtDraw, double dbScale )
{
	
}
void CGIS_UserLayer::DrawLayer( IGIS_MapView *pView, swRect rtPolt, ST_GEO_RECT rtDraw, double dbScale )
{
	if( !EstimateDrawCondition( dbScale, m_pInfo->GetMinGRScale(),m_pInfo->GetMaxGRScale() ) )
		return;
	BOOL bAnno = ( (dbScale<m_pInfo->GetMaxARScale()) && (dbScale>m_pInfo->GetMinARScale()) ) 
					|| (m_pInfo->GetMaxARScale() == -1);
	
	CPtrList pRutList[RUT_TYPE_NUM];
	pView->PreparePen( m_pInfo->GetPen() );
	pView->PrepareBrush( m_pInfo->GetBrush() );

	POSITION pos1 = m_pObjListMap->GetStartPosition();
	CPtrList *pObjList = NULL;
	int    nKey;
	while( pos1 )
	{
		m_pObjListMap->GetNextAssoc( pos1, nKey, pObjList );
		if( !pObjList )
			break;

		POSITION pos = pObjList->GetHeadPosition();
		CGIS_Feature *pObj = NULL;
		while( pos )
		{
			pObj = (CGIS_Feature*)pObjList->GetNext( pos );
			switch( pObj->GetObjType( ) )
			{
			case EN_GOTYPE_POINT:
			case EN_GOTYPE_UPOINT:
				DrawPoint( pView, (CGIS_FeaturePoint*)pObj, rtPolt, rtDraw, dbScale, bAnno );
				break;
			case EN_GOTYPE_LINE:
			case EN_GOTYPE_ULINE:
				DrawLine( pView, (CGIS_FeatureLine*)pObj, rtPolt, rtDraw, dbScale, bAnno );
				break;
			case EN_GOTYPE_POLY:
			case EN_GOTYPE_UPOLY:
				DrawPoly( pView, (CGIS_FeaturePoly*)pObj, rtPolt, rtDraw, dbScale, bAnno );
				break;
			case EN_GOTYPE_RUT:
			case EN_GOTYPE_URUT:
				{
/*					switch(((CGIS_FeatureRut*)pObj)->GetRutType())
					{
					case 0:
						if(dbScale < 8000)
							continue;
						pRutList[0].AddTail( pObj );
						break;
					case 1:
						if(dbScale < 500)
							continue;
						pRutList[1].AddTail( pObj );
						break;
					case 2:
						if(dbScale < 500)
							continue;
						pRutList[2].AddTail( pObj );
						break;
					case 3:
						if(dbScale < 1000)
							continue;
						pRutList[3].AddTail( pObj );
						break;
					case 4:
						if(dbScale < 5000)
							continue;
						pRutList[4].AddTail( pObj );
						break;
					case 5:
						if(dbScale < 10000)
							continue;
						pRutList[5].AddTail( pObj );
						break;
					case 6:
						if(dbScale < 20000)
							continue;
						pRutList[6].AddTail( pObj );
						break;
					default:
						ASSERT( FALSE );
						break;
					}
*/				}
				break;
			default:
				break;
			}
		}
		pView->ReleasePen();
		pView->ReleaseBrush();

		ST_GEO_PEN pen;
		int nWidth[RUT_TYPE_NUM-1];
		CGIS_FeatureRut *pRut = NULL;
		int i;
		for( i =0;	i<RUT_TYPE_NUM;	i++ )
		{
			if( pRutList[i].GetCount() <= 0 )
				continue;
			pen.nStyle = PS_SOLID;
			switch( i )
			{
			case 0:
				pen.nColor = 8421504;
				pen.nWidth = 4;
				nWidth[i] = 2;
				break;
			case 1:
				pen.nColor = 1858269;
				pen.nWidth = 7;
				nWidth[i] = 3;
				break;
			case 2:
				pen.nColor = 8831743;
				pen.nWidth = 7;
				nWidth[i] = 3;
				break;
			case 3:
				pen.nColor = 8831743;
				pen.nWidth = 5;
				nWidth[i] = 3;
				break;
			case 4:
				pen.nColor = 1858269;
				pen.nWidth = 5;
				nWidth[i] = 3;
				break;
			case 5:
				pen.nColor = 8831743;
				pen.nWidth = 4;
				nWidth[i] = 2;
				break;
			case 6:
				pen.nColor = 8831743;
				pen.nStyle = PS_DASH;
				pen.nWidth = 1;
				break;
			}
			pView->PreparePen(pen);
			pos = pRutList[i].GetHeadPosition();
			pRut = NULL;
			while( pos )
			{
				pRut = (CGIS_FeatureRut*)pRutList[i].GetNext(pos);
				if( pRut )
					DrawRut( pView, pRut, rtPolt, rtDraw, dbScale, bAnno );
			}
			pView->ReleasePen();
		}
		pen.nColor = RGB(255, 255, 255);
		pen.nStyle = PS_SOLID;
		for( i =0;	i<RUT_TYPE_NUM-1;	i++ )
		{
			if( pRutList[i].GetCount() <= 0 )
				continue;
			pen.nWidth = nWidth[i];
			pView->PreparePen(pen);
			pos = pRutList[i].GetHeadPosition();
			while( pos )
			{
				pRut = (CGIS_FeatureRut*)pRutList[i].GetNext(pos);
				if( pRut )
					DrawRut( pView, pRut, rtPolt, rtDraw, dbScale, FALSE );
			}
			pView->ReleasePen();
		}
	}
}

UINT CGIS_UserLayer::GetFeatureMaxID()
{
	UINT nMaxID = 0;
	
	CPtrList * pList;
	m_pObjListMap->Lookup( 0, pList );
	if( pList )
	{
		if( pList->GetCount() == 0 )
			return nMaxID;
		CGIS_Feature* pFeature = NULL;
		pFeature = (CGIS_Feature*)pList->GetTail();
		if( !pFeature )
			return nMaxID;

		if( m_enUMLType == EN_UMLAYERTYPE_ROUTE )
		{
			nMaxID = ((CGIS_FeatureRoute*)pFeature)->GetNetworkRoute()->GetRouteID();
		}
		else if( m_enUMLType == EN_UMLAYERTYPE_TRACK )
		{
			nMaxID = ((CGIS_FeatureTrack*)pFeature)->GetTrackFeatureID();
		}
		else if( m_enUMLType == EN_UMLAYERTYPE_ADDRESSBOOK )
		{
			nMaxID = ((CGIS_FeatureAddress*)pFeature)->GetAddressID();
		}		
	}	
	return nMaxID;
}

DWORD CGIS_UserLayer::GetTotalFeatrueSize( swString strPath, long & nNum/*The number of the features,default as 0*/ )
{
	DWORD dwTotalSize = 0;
	nNum = 0;

	CPtrList * pList = NULL;
	m_pObjListMap->Lookup( 0, pList );
	nNum = pList->GetCount();
	if ( nNum <= 0 )
	{
		LoadFeatureObject( 0, strPath );
	}

	CGIS_Feature * pFeature = NULL;
	POSITION pos = pList->GetHeadPosition();
	if ( m_enUMLType == EN_UMLAYERTYPE_TRACK )
	{
		CGIS_FeatureTrack * pFTrack = NULL;
		while ( pos )
		{
			pFTrack = (CGIS_FeatureTrack *)pList->GetNext( pos );
			if ( pFTrack )
			{
				dwTotalSize += m_pMiddle->GetFeatureSize( strPath, pFTrack->GetTrackFeatureID() );
			}
		}

		dwTotalSize += m_pMiddle->GetFeatureSize( strPath + L"\\Track.dat", 0 );
	}
	else if ( m_enUMLType == EN_UMLAYERTYPE_ADDRESSBOOK )
	{
		CGIS_FeatureAddress * pFAddress = NULL;
		while ( pos )
		{
			pFAddress = (CGIS_FeatureAddress *)pList->GetNext( pos );
			if ( pFAddress )
			{
				dwTotalSize += m_pMiddle->GetFeatureSize( strPath, pFAddress->GetAddressID() );
			}
		}

		dwTotalSize += m_pMiddle->GetFeatureSize( strPath + L"\\Address.dat", 0 );
	}
	else if ( m_enUMLType == EN_UMLAYERTYPE_ROUTE )
	{
		CGIS_FeatureRoute* pFRoute = NULL;
		while ( pos )
		{
			pFRoute = (CGIS_FeatureRoute *)pList->GetNext( pos );
			if ( pFRoute )
			{
				dwTotalSize += m_pMiddle->GetFeatureSize( strPath, pFRoute->GetNetworkRoute()->GetRouteID() );
			}
		}

		dwTotalSize += m_pMiddle->GetFeatureSize( strPath + L"\\Route.dat", 0 );
	}

	return dwTotalSize;
}

BOOL CGIS_UserLayer::DeleteAllFeature(swString strFPath)
{
	return m_pMiddle->DeleteAllFeature(this,strFPath);
}
