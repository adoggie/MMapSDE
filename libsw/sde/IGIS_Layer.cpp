
#include "IGIS_Layer.h"

IGIS_Layer::IGIS_Layer():m_pInfo(NULL){
	//m_pObjListMap = new CListMap;
}

IGIS_Layer::~IGIS_Layer(){
	/*
	CPtrList *pObjList = NULL;
	POSITION pos = m_pObjListMap->GetStartPosition();
	int    nKey;
	while( pos != NULL )
	{
		m_pObjListMap->GetNextAssoc( pos, nKey, pObjList );
		if( pObjList )
		{
			POSITION ps = pObjList->GetHeadPosition();
			CGIS_Feature *pObj = NULL;
			while( ps )
			{
				pObj = (CGIS_Feature*)pObjList->GetNext(ps);
				if( pObj )
					delete pObj;
			}
			pObjList->RemoveAll();
			delete pObjList;
		}
		pObjList = NULL;
	}
	m_pObjListMap->RemoveAll();
	if( m_pObjListMap )
		delete m_pObjListMap;
	m_pObjListMap = NULL;
	*/
	BreadthObjectListT::iterator itr;
	std::list<CGIS_Feature*>::iterator itrObj;
	CGIS_Feature* feature;
	for(itr=m_pObjListMap.begin();itr!=m_pObjListMap.end();itr++){
		std::list<CGIS_Feature*>& objlist = itr->second;
		for(itrObj = objlist.begin();itrObj!=objlist.end();itrObj++){
			feature = (*itrObj);
			delete feature;
		}
	}

	if( m_pInfo )
		delete m_pInfo;
	m_pInfo = NULL;
}

void IGIS_Layer::InitObjectListMap( int nBreadthID ){
	//m_pObjListMap->SetAt( nBreadthID, new CPtrList );
	m_pObjListMap[nBreadthID] = std::list<CGIS_Feature*>();
}

EnLayType IGIS_Layer::GetLayerType( ){
	return m_enLType;
}

CGIS_LayerInfo* IGIS_Layer::GetLayerInfo( ){
	return m_pInfo;
}

IGIS_Layer::BreadthObjectListT& IGIS_Layer::GetObjectListMap( ){
	return m_pObjListMap;
}
void IGIS_Layer::DrawPoint( IGIS_MapView *pView, CGIS_FeaturePoint *pPoint, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno )
{
//	if( IsPtInRect(pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, rtDraw.dLeft, 
//				  rtDraw.dTop, rtDraw.dRight, rtDraw.dBottom) )
//	{
//		int nx,ny;
////		pView->XyMapToDraw( pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, nx, ny );
//		pView->XyMapToView( pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, nx, ny );
//		if( IsPtInScreen( nx, ny, rtPlot.left, rtPlot.right, rtPlot.top, rtPlot.bottom ) )
//		{
//			pView->DrawPoint( pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, pPoint->GetIConID() );
//			if(bAnno && pPoint->GetAnnotation()!=NULL )
//			{
//				pView->AddAnnotate( pPoint->GetPtList(), 1, pPoint->GetAnnotation() );
//				//Annotate(pView, m_pListObj + i);
//			}
//		}
//	}
}

/*
void IGIS_Layer::DrawLine( IGIS_MapView *pView, CGIS_FeatureLine *pLine, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno )
{
	int nClipType = GeoRectIntersect( &rtDraw, &pLine->GetRectObj() );
	if(nClipType == 0)
		return;

	BOOL bState = FALSE;
	if(nClipType == 2)
	{
		bState = TRUE;
	}
	else
	{
		if( pLine->GetPartNum() > 1 )
			bState = TRUE;
		else
			bState = FALSE;
	}
	if( bState )
	{
		pView->DrawLine( pLine->GetPtList(), pLine->GetPtNum(), pLine->GetPart(),
						pLine->GetPartNum(), m_pInfo->GetPen().nStyle);
		
		if(bAnno )
		{
			int nStrNum = pLine->GetAnnotation().Length();

			if( nStrNum <= pLine->GetPtNum() )
			{
				ST_GEO_PTXY *pPt = new ST_GEO_PTXY[nStrNum];
				if(GetPointsOnLine( pLine->GetPtList(), pLine->GetPtNum(), nStrNum, pPt) )
				{
					pView->AddAnnotate( pPt, nStrNum, pLine->GetAnnotation() );
				}
				delete [] pPt;
				pPt = NULL;
			}	
		}
	}
	else
	{
		int nNum;
		ST_GEO_PTXY* pPtList = GetClipRgn( pLine->GetPtList(), pLine->GetPtNum(), 
								rtDraw, &nNum, pLine->GetObjType() );
		if(nNum > 0)
		{
			pView->DrawLine( pPtList, nNum, &nNum, 1, m_pInfo->GetPen().nStyle );
			//if(bAnno && pLine->GetAnnotation()!=NULL )
			if(bAnno  )
			{
				int nStrNum = swString(pLine->GetAnnotation()).Length();

				if( nStrNum <= nNum )
				{
					ST_GEO_PTXY *pPt = new ST_GEO_PTXY[nStrNum];
					if(GetPointsOnLine( pPtList, nNum, nStrNum, pPt) )
					{
						pView->AddAnnotate( pPt, nStrNum, pLine->GetAnnotation() );
					}
					delete [] pPt;
					pPt = NULL;
				}	
			}
		}
		delete []pPtList;
	}
}
*/
/*
void IGIS_Layer::DrawPoly( IGIS_MapView *pView, CGIS_FeaturePoly *pPoly, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno )
{
	int nClipType = GeoRectIntersect( &rtDraw, &pPoly->GetRectObj() );
	if(nClipType == 0)
		return;
	
	BOOL bState = FALSE;
	if(nClipType == 2)
	{
		bState = TRUE;
	}
	else
	{
		if( pPoly->GetPartNum() > 1 )
			bState = TRUE;
		else
			bState = FALSE;
	}
	
	if( bState ) //多边形完全在当前可视区域内，简单绘制
	{
		pView->DrawPoly( pPoly->GetPtList(), pPoly->GetPtNum(), 
						pPoly->GetPart(), pPoly->GetPartNum() );
		if(bAnno && pPoly->GetAnnotation()!=NULL )
		{
			pView->AddAnnotate( &pPoly->GetCenterPt(), 1, pPoly->GetAnnotation() );
		}
	}
	else //多边形矩形区域大于当前可是区域，为了提高性能进行裁剪优化
	{
//		if( pPoly->GetPtNum() <= 5 )
//			ASSERT(FALSE);
		int nNum;
		ST_GEO_PTXY* pPtList = GetClipRgn( pPoly->GetPtList(), pPoly->GetPtNum(), //运算裁剪区域内的点
						rtDraw, &nNum, pPoly->GetObjType() );
		if(nNum > 0)
		{
//			if( nNum <= 3 )
//				ASSERT(FALSE);
			pView->DrawPoly(pPtList, nNum, &nNum,1);
			if(bAnno && pPoly->GetAnnotation()!=NULL )
			{
//				CGIS_FeaturePoly poly(EN_GOTYPE_POLY);
//				poly.SetPtNum(nNum);
//				poly.SetPtList(pPtList);
//				poly.SetAnnotation(pPoly->GetAnnotation());
//				::CalcRect(pPtList,nNum,poly.GetRectObj());
//				CalcCenter(&poly);
				pView->AddAnnotate( &pPoly->GetCenterPt(), 1, pPoly->GetAnnotation() );
			}
		}
		delete [] pPtList;
	}
}
*/

/*
void IGIS_Layer::DrawRut( IGIS_MapView *pView, CGIS_FeatureRut *pRut, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno )
{
	int nClipType = GeoRectIntersect(&rtDraw, &pRut->GetRectObj());
	if(nClipType == 0)	// 没有交集
		return;
	
	int nNum = 0;
	ST_GEO_PTXY* pPtList = NULL;
	if(nClipType == 2)
	{
		pView->DrawLine( pRut->GetPtList(), pRut->GetPtNum() );
		if( bAnno && pRut->GetAnnotation()!=NULL )
		{
			int nStrNum = swString(pRut->GetAnnotation()).GetLength();
			
			if(nStrNum <= pRut->GetPtNum())
			{
				ST_GEO_PTXY *pPt = new ST_GEO_PTXY[nStrNum];
				if(GetPointsOnLine(pRut->GetPtList(), pRut->GetPtNum(), nStrNum, pPt))
				{
					pView->AddAnnotate(pPt, nStrNum, pRut->GetAnnotation());
				}

				delete [] pPt;
			}
		}
	}
	else
	{
		pPtList = GetClipRgn(pRut->GetPtList(), pRut->GetPtNum(), 
										  rtDraw, &nNum, 1);
		if(nNum > 0)
		{
			pView->DrawLine(pPtList, nNum);
			if(bAnno && pRut->GetAnnotation()!=NULL )
			{
				int nStrNum = swString(pRut->GetAnnotation()).GetLength();
				
				if(nStrNum <= pRut->GetPtNum())
				{
					ST_GEO_PTXY *pPt = new ST_GEO_PTXY[nStrNum];
					if(GetPointsOnLine(pPtList, pRut->GetPtNum(), nStrNum, pPt))
					{
						pView->AddAnnotate(pPt, nStrNum, pRut->GetAnnotation());
					}

					delete [] pPt;
				}
			}
		}
		delete[] pPtList;
	}
}

*/
