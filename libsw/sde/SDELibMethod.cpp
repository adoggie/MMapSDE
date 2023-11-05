// SDELibMethod.cpp: implementation of the CSDELibMethod class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SDELibMethod.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////
CImageList * CSDELibMethod::m_pImageList = new CImageList;
CPtrList* CSDELibMethod::m_pAnnoList = new CPtrList;
BOOL CSDELibMethod::m_bDrawState = FALSE;

void CSDELibMethod::DestructResource( )
{
	ClearAnnoList();
	delete m_pAnnoList;
	delete m_pImageList;
}
void CSDELibMethod::CreateImageList( )
{
	if( m_pImageList)
		m_pImageList = new CImageList;
//	m_pImageList->Create(IDB_OBJICON, 16, 0, RGB(255, 255, 255));
}
BOOL CSDELibMethod::IsDrawState( )
{
	return m_bDrawState;
}
void CSDELibMethod::SetDrawState( BOOL bDrawState )
{
	m_bDrawState = bDrawState;
}
CImageList* CSDELibMethod::GetImageList( )
{
	return m_pImageList;
}
void CSDELibMethod::AddToAnnoList( ST_GEO_ANNO *pAnnotation )
{
	ASSERT( m_pAnnoList );
	m_pAnnoList->AddTail( pAnnotation );
}
void CSDELibMethod::ClearAnnoList( )
{
	ST_GEO_ANNO* pAnno;
	POSITION pos = m_pAnnoList->GetHeadPosition();
	while(pos != NULL)
	{
		pAnno = (ST_GEO_ANNO*)m_pAnnoList->GetNext(pos);
		delete [] pAnno->pRect;
		delete pAnno;
	}
	m_pAnnoList->RemoveAll();
}
void CSDELibMethod::xyMapToView(CRect plotRect, ST_GEO_RECT drawRect, double dScale, 
				 double dx, double dy, int& nx, int& ny)
{
	int nAdjustx,nAdjusty;

	nAdjustx = int((plotRect.right -(drawRect.dRight - drawRect.dLeft) * dScale) / 2);
	nAdjusty = int((plotRect.bottom-(drawRect.dBottom - drawRect.dTop) * dScale) / 2);
	nx = nAdjustx + int((dx - drawRect.dLeft) * dScale);
	ny = int((drawRect.dBottom - dy) * dScale) + nAdjusty;
}
EnClipType CSDELibMethod::GeoRectIntersect( ST_GEO_RECT *sourceRect, ST_GEO_RECT *refRect )
{
	EnClipType enClipType = CLIP_NULL;
	if((sourceRect->dRight	<= refRect->dLeft) ||
   	   (sourceRect->dBottom <= refRect->dTop) ||
	   (sourceRect->dLeft	>= refRect->dRight) ||
	   (sourceRect->dTop	>= refRect->dBottom))
	{
		enClipType = CLIP_NULL;		// source和ref无交叉
	}
	else if((sourceRect->dLeft		<= refRect->dLeft) &&
			(sourceRect->dRight		>= refRect->dRight) &&
			(sourceRect->dTop		<= refRect->dTop) &&
			(sourceRect->dBottom	>= refRect->dBottom))
	{
		enClipType = CLIP_OUT;		// source包含ref
	}
	else if((sourceRect->dLeft		>= refRect->dLeft) &&
			(sourceRect->dRight		<= sourceRect->dRight) &&
			(sourceRect->dTop		<= refRect->dTop) &&
			(sourceRect->dBottom	>= refRect->dBottom))
	{
		enClipType = CLIP_IN;		// ref包含source
	}
	else
	{
		enClipType = CLIP_CROSS;		// source和ref交叉
	}
	return enClipType;
}
ST_GEO_PTXY* CSDELibMethod::GetClipRgn(ST_GEO_PTXY* pPt, int nNum, ST_GEO_RECT &rect, 
						int *pnPtNum, EnLObjectType enObjType)
{
	int i, nSide;			// nSide：变量指定剪切边的左、上、右、下
    double dx, dy;

	ST_GEO_PTXY *pTempList = pPt, *pRtnList;

    double dx1, dy1, dx2, dy2;
    BOOL bAccepted, bLastAccepted;
	
	// 判断第一点是否是求出来的，1：初始值，2：原来链表值，3：求出的值
    int nFirstPointCalc;
	int nRtnNo;

	*pnPtNum = nNum;
	for(nSide = 1; nSide <= 4; nSide++)
	{
		pRtnList	= new ST_GEO_PTXY[nNum * 2];
        bAccepted	= bLastAccepted = TRUE;
		nRtnNo		= 0;

        nFirstPointCalc = 1;
		for(i = 0; i < *pnPtNum; i++)
		{
         	// 判断点是否该被取得
			switch (nSide)
			{
			case 1 :
				bAccepted = pTempList[i].dx > rect.dLeft;
				dx1 = rect.dLeft;
				dy1 = rect.dTop;
				dx2 = rect.dLeft;
				dy2 = rect.dBottom;
                break;
            case 2 : 
				bAccepted = pTempList[i].dy > rect.dTop;
				dx1 = rect.dLeft;
				dy1 = rect.dTop;
				dx2 = rect.dRight;
				dy2 = rect.dTop;
				break;
			case 3 : 
                bAccepted = pTempList[i].dx < rect.dRight;
				dx1 = rect.dRight;
				dy1 = rect.dTop;
				dx2 = rect.dRight;
				dy2 = rect.dBottom;
				break;
            case 4 :
                bAccepted = pTempList[i].dy < rect.dBottom;
				dx1 = rect.dLeft;
				dy1 = rect.dBottom;
				dx2 = rect.dRight;
				dy2 = rect.dBottom;
				break;
			}

			// 在范围内
			if(bAccepted)
			{
				if(!bLastAccepted)
				{
					if(GetIntersectedPoint(dx1, dy1, dx2, dy2, pTempList[i].dx,
						pTempList[i].dy, pTempList[i-1].dx, pTempList[i-1].dy, dx, dy, 2))
					{
						pRtnList[nRtnNo].dx = (float)dx;
						pRtnList[nRtnNo].dy = (float)dy;
						nRtnNo++;
					}
				}

				pRtnList[nRtnNo].dx = pTempList[i].dx;
				pRtnList[nRtnNo].dy = pTempList[i].dy;
				nRtnNo++;
                if(1 == nFirstPointCalc)
				{
					nFirstPointCalc = 2;
				}
			}
			else
			{
				if(1 == nFirstPointCalc)
				{
					nFirstPointCalc = 3;
				}

				if(i > 0)
				{
					if(bLastAccepted)
					{
						if(GetIntersectedPoint(dx1, dy1, dx2, dy2, pTempList[i].dx,
							pTempList[i].dy, pTempList[i-1].dx, pTempList[i-1].dy, dx, dy, 2))
						{
							pRtnList[nRtnNo].dx = (float)dx;
							pRtnList[nRtnNo].dy = (float)dy;
							nRtnNo++;
						}
					}
				}
			}

			bLastAccepted = bAccepted;
		}

		if(enObjType == LOTYPE_POLY && nRtnNo > 0 && nFirstPointCalc == 3)
		{
			pRtnList[nRtnNo].dx = pRtnList[0].dx;
			pRtnList[nRtnNo].dy = pRtnList[0].dy;
			nRtnNo++;
		}

		if(pTempList != pPt)
		{
			delete [] pTempList;
		}

		pTempList	= pRtnList;
		*pnPtNum	= nRtnNo;
	}

	return pRtnList;
}
// 计算添加注释信息的位置
BOOL CSDELibMethod::CalcAnnoStation( RECT &rect )
{
	int nW = int((rect.right - rect.left) / 2);
	int nH = int((rect.bottom - rect.top) / 2);

	CRect rIn;
	
	// 九个方向遍历，0为右上，顺时针依次旋转，7为正上方，8为覆盖文字位置
	for (int i = 0; i < 8; i ++)
	{
		if(EstimateAnnotateRect(rect))
		{
			return TRUE;
		}

		switch(i)
		{
			case 0 :
			case 1 : 
				rect.top += nH + ANNOTATION_Y_OFF;
				rect.bottom += nH + ANNOTATION_Y_OFF;
				break;
			case 2 :
			case 3 :
				rect.left -= nW + ANNOTATION_X_OFF;
				rect.right -= nW + ANNOTATION_X_OFF;
				break;
			case 4 :
			case 5 : 
				rect.top -= nH + ANNOTATION_Y_OFF;
				rect.bottom -= nH + ANNOTATION_Y_OFF;
				break;
			case 6 : 
			case 7 : 
				rect.left += nW + ANNOTATION_X_OFF;
				rect.right += nW + ANNOTATION_X_OFF;
				break;
		}
	}
	
	return FALSE;
}
BOOL CSDELibMethod::EstimateAnnotateRect(RECT rect)
{
	POSITION pos = m_pAnnoList->GetHeadPosition();
	while(pos != NULL)
	{
		ST_GEO_ANNO *pAnno;
		CRect rIn;
		pAnno = (ST_GEO_ANNO *)m_pAnnoList->GetNext(pos);
		for(int k = 0; k < pAnno->nRectNum; k++)
		{
			rIn.SetRect(pAnno->pRect[k].left - 20, pAnno->pRect[k].top - 20, 
					    pAnno->pRect[k].right + 20, pAnno->pRect[k].bottom + 20);
			
			if(rIn.IntersectRect(&rIn, &rect))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

// 计算多边形的重心
void CSDELibMethod::CalcCenter(CGIS_FeaturePoly* pObj)
{
    double X1, Y1, X2, Y2, LX, LY, RX, RY, IX, IY;
    double MaxV;
	CPtrList listPt;

	// 获得地物水平方向的中心线
	LX = pObj->GetRectObj().dLeft;
    LY = pObj->GetRectObj().dTop + (pObj->GetRectObj().dBottom - pObj->GetRectObj().dTop) / 2;
	RX = pObj->GetRectObj().dRight;
    RY = LY;

	BOOL bFind;
	POSITION pos, posLast;
	double *dbTmp, *dbTmp2;

	for(int i = 0; i < pObj->GetPtNum() - 1; i++)
	{
		X1 = pObj->GetPtList()[i].dx;
        Y1 = pObj->GetPtList()[i].dy;
		X2 = pObj->GetPtList()[i+1].dx;
        Y2 = pObj->GetPtList()[i+1].dy;
		// 计算真交点
		if(GetIntersectedPoint(LX, LY, RX, RY, X1, Y1, X2, Y2, IX, IY, 2))
		{
			bFind = FALSE;
			pos = listPt.GetHeadPosition();
			while(pos != NULL)
			{
				posLast = pos;
				dbTmp = (double*)listPt.GetNext(pos);
				if(IX <= *dbTmp)
				{
					dbTmp = new double;
					*dbTmp = IX;
					listPt.InsertBefore(posLast, dbTmp);
					bFind = TRUE;
				}
			}

			if(!bFind)
			{
				dbTmp = new double;
				*dbTmp = IX;
				listPt.AddTail(dbTmp);
			}
		}
	}
	
    //计算最大线段
    MaxV = 0;
	double dbCenter = 0.;

	pos = listPt.GetHeadPosition();
	while(pos != NULL)
	{
		posLast = pos;
		dbTmp = (double*)listPt.GetNext(pos);
		if(pos != NULL)
		{
			dbTmp2 = (double*)listPt.GetNext(pos);
			IX = *dbTmp2 - *dbTmp;
			if(IX > MaxV)
			{
				MaxV = IX;
				dbCenter = *dbTmp + MaxV / 2;
			}

			delete dbTmp2;	// 释放已经用过的指针
			dbTmp2 = NULL;
		}
		else if(dbCenter == 0.)
		{
			dbCenter = *dbTmp;
		}
		
		delete dbTmp;
		dbTmp = NULL;
	}

//	pObj->CenterPt.dx = (float)dbCenter;
//	pObj->CenterPt.dy = (float)LY;
	pObj->SetCenterPt( (float)dbCenter, (float)LY );

	listPt.RemoveAll();
}
