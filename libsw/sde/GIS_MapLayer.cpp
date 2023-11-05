
#include "GIS_MapLayer.h"
#include "IGIS_LayerFile.h"
#include "GeoView.h"

CGIS_MapLayer::CGIS_MapLayer(CGIS_LayerInfo *pInfo){
	m_enLType = EN_LAYTYPE_MAP;
	m_pInfo = pInfo;
	m_pBLCount = NULL;
	m_nBandMaxID = 0;
}

CGIS_MapLayer::~CGIS_MapLayer(){
	if(m_pBLCount){
		delete[] m_pBLCount;
	}
}
//����ͼ���ڻ��漰������ͼ������
void CGIS_MapLayer::InitLBCount( int nMaxNum ){
	if( m_pBLCount )
		delete []m_pBLCount;
//	if( m_pBIFList )
//		delete []m_pBIFList;
	m_pBLCount = new unsigned char[nMaxNum+1];
	memset( m_pBLCount, 0, sizeof(unsigned char)*(nMaxNum+1) );
	m_nBandMaxID = nMaxNum+1;
//	m_pBIFList = new BYTE[nMaxNum+1];
//	memset( m_pBIFList, 0, sizeof(BYTE)*(nMaxNum+1) );
}

void CGIS_MapLayer::InitLBOList( int nBID ){
	/*
	CPtrList *pOList = NULL;
	m_pObjListMap->Lookup( nBID, pOList ); //һ���������Ѿ�������n��ͼ�飬ÿ��ͼ��������һ�ѵ�feature����
	if( pOList == NULL )
		InitObjectListMap( nBID );//map<>���һ��ͼ����
		*/
	if(m_pObjListMap.find(nBID) ==  m_pObjListMap.end()){
		InitObjectListMap(nBID);
	}
}

void CGIS_MapLayer::IncreaseLCount( int nBID ){
	m_pBLCount[nBID]++; //ÿ��ͼ�鶼�����ü���
}

void CGIS_MapLayer::DecreaseLCount( int nBID ){
	if( m_pBLCount[nBID] == 0 )
		return;
	m_pBLCount[nBID]--;
	if( m_pBLCount[nBID] == 0 ) //ͼ������Ϊ0�˱���ɾ����ͼ����صĵ�ͼ���ݶ���
	{
		BreadthObjectListT::iterator itr;
		itr = m_pObjListMap.find(nBID);
		if( itr!=m_pObjListMap.end()){
			std::list<CGIS_Feature*>& ftrList = *itr;
			std::list<CGIS_Feature*>::iterator itrftr;
			//ɾ��ͼ���漰��feature��������
			for(itrftr = ftrList.begin();itrftr!=ftrList.end();itrftr++){
				CGIS_Feature* f = *itr;
				delete f;
			}
			ftrList.clear(); //ɾ�����ж���
		}
	}
		/*
		CPtrList *pOList = NULL;
		m_pObjListMap->Lookup( nBID, pOList );
		if( pOList )
		{
			POSITION pos = pOList->GetHeadPosition();
			CGIS_Feature *pFeature = NULL;
			while( pos )
			{
				pFeature = (CGIS_Feature*)pOList->GetNext( pos );
				if( pFeature )
				{
					delete pFeature;
					pFeature = NULL;
				}
			}
			pOList->RemoveAll( );
		}*/
	//}
}

//��ȡͼ�����õ�����
unsigned short CGIS_MapLayer::GetBLCount( int nBID ){
	return m_pBLCount[nBID];
}
/*
BOOL CGIS_MapLayer::EstimateDrawCondition( double dbScale, double dbMinGRScale, double dbMaxGRScale ){
	if( dbScale< dbMinGRScale	||	dbScale > dbMaxGRScale )//���ڿɼ��Ļ��Ʊ��������������
		return FALSE;
	return TRUE;
}*/
/*
void CGIS_MapLayer::DrawLayer( IGIS_LayerFile *pLFile, IGIS_MapView *pView, 
															swRect rtPolt, ST_GEO_RECT rtDraw, double dbScale ){
	if( !EstimateDrawCondition( dbScale, m_pInfo->GetMinGRScale(), m_pInfo->GetMaxGRScale() ) )
		return;
	ASSERT( pLFile );
//	if( !pLFile->IsLoadMode() )
//		LoadLayer( pLFile );
	DrawLayer( pView, rtPolt, rtDraw, dbScale );
//	if( !pLFile->IsLoadMode() )
//		UnloadLayer( );
}

void CGIS_MapLayer::DrawLayer( IGIS_MapView *pView, swRect rtPolt, ST_GEO_RECT rtDraw, double dbScale )
{
	if( !EstimateDrawCondition( dbScale, m_pInfo->GetMinGRScale(),m_pInfo->GetMaxGRScale() ) )
		return;
	BOOL bAnno = ( (dbScale<m_pInfo->GetMaxARScale()) && (dbScale>m_pInfo->GetMinARScale()) ) 
					|| (m_pInfo->GetMaxARScale() == -1);
	
	POSITION pos1 = m_pObjListMap->GetStartPosition();
	CPtrList *pObjList = NULL;
	int    nKey;
	while( pos1 )
	{
		m_pObjListMap->GetNextAssoc( pos1, nKey, pObjList );
		if( !pObjList )
			break;

///		if( m_pInfo->GetLayerID() != 8 )
//			continue;
		pView->PreparePen( m_pInfo->GetPen() );
		pView->PrepareBrush( m_pInfo->GetBrush() );
		POSITION pos = pObjList->GetHeadPosition();
		CGIS_Feature *pObj = NULL;
		while( pos )
		{
			pObj = (CGIS_Feature*)pObjList->GetNext( pos );
			switch( pObj->GetObjType( ) )
			{
			case EN_GOTYPE_POINT:
				DrawPoint( pView, (CGIS_FeaturePoint*)pObj, rtPolt, rtDraw, dbScale, bAnno );
				break;
			case EN_GOTYPE_LINE:
				DrawLine( pView, (CGIS_FeatureLine*)pObj, rtPolt, rtDraw, dbScale, bAnno );
				break;
			case EN_GOTYPE_POLY:
				DrawPoly( pView, (CGIS_FeaturePoly*)pObj, rtPolt, rtDraw, dbScale, bAnno );
				break;
			case EN_GOTYPE_RUT:
				{
					DrawRut( pView, (CGIS_FeatureRut*)pObj, rtPolt, rtDraw, dbScale, bAnno );
				}
				break;
			default:
				ASSERT( FALSE );
				break;
			}
		}
		pView->ReleasePen( );
		pView->ReleaseBrush();
	}

//	CGeoView *pGView = (CGeoView*)pView;
//	if( pGView )
//	{
//		CDC *pBackDC = pGView->GetBackDC();
//		CDC *pBufDC = pGView->GetBufDC();
//		CDC *pCurrentDC = pGView->GetCurrentDC();
//		CBitmap *pBufBMP = pGView->GetBackBMP();
//		CBitmap* pOldBMP;
//		pOldBMP = (CBitmap*)(pBufDC->SelectObject(pBufBMP));
//		if( pBackDC && pBufDC && pCurrentDC )
//			pBackDC->BitBlt(0, 0, rtPolt.right - rtPolt.left, rtPolt.bottom - rtPolt.top, 
//					pBufDC, 0, 0, SRCCOPY);
//		pBufDC->SelectObject(pOldBMP);
//	}
}
*/

int CGIS_MapLayer::GetBandMaxID( ){
	return m_nBandMaxID;
}

void CGIS_MapLayer::ClearLCount(){
	// ����һ���Կ�ָ����ж�
	if (m_pBLCount)
		memset( m_pBLCount, 0, sizeof(unsigned char)*m_nBandMaxID );
}
