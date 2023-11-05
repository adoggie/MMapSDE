
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
//设置图层内会涉及的最大的图块数量
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
	m_pObjListMap->Lookup( nBID, pOList ); //一个层里面已经链接了n个图块，每个图块里面有一堆的feature对象
	if( pOList == NULL )
		InitObjectListMap( nBID );//map<>添加一个图块编号
		*/
	if(m_pObjListMap.find(nBID) ==  m_pObjListMap.end()){
		InitObjectListMap(nBID);
	}
}

void CGIS_MapLayer::IncreaseLCount( int nBID ){
	m_pBLCount[nBID]++; //每个图块都有引用计数
}

void CGIS_MapLayer::DecreaseLCount( int nBID ){
	if( m_pBLCount[nBID] == 0 )
		return;
	m_pBLCount[nBID]--;
	if( m_pBLCount[nBID] == 0 ) //图块引用为0了必须删除此图块相关的地图数据对象
	{
		BreadthObjectListT::iterator itr;
		itr = m_pObjListMap.find(nBID);
		if( itr!=m_pObjListMap.end()){
			std::list<CGIS_Feature*>& ftrList = *itr;
			std::list<CGIS_Feature*>::iterator itrftr;
			//删除图块涉及的feature对象数据
			for(itrftr = ftrList.begin();itrftr!=ftrList.end();itrftr++){
				CGIS_Feature* f = *itr;
				delete f;
			}
			ftrList.clear(); //删除所有对象
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

//获取图块引用的数量
unsigned short CGIS_MapLayer::GetBLCount( int nBID ){
	return m_pBLCount[nBID];
}
/*
BOOL CGIS_MapLayer::EstimateDrawCondition( double dbScale, double dbMinGRScale, double dbMaxGRScale ){
	if( dbScale< dbMinGRScale	||	dbScale > dbMaxGRScale )//不在可见的绘制比例下则无需绘制
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
	// 增加一个对空指针的判断
	if (m_pBLCount)
		memset( m_pBLCount, 0, sizeof(unsigned char)*m_nBandMaxID );
}
