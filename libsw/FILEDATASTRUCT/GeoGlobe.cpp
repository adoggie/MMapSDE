///////////////////////////////////////////////////////////////////////////////
// GeoGlobe.cpp : implementation of the Globals
//
// Copyright (c) 2004，上海合众思壮科技有限责任公司GIS部
//
// All rights reserved
//
// 文件名称：GeoGlobe.cpp
//
// 摘要    ：GEO公共函数实现
//
// 作者    ：Hansom
//
// 当前版本：1.1
//
// 完成日期：2004年05月15日
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GeoGlobe.h"
#include "Globe.h"
#include <math.h>
//#include "GeoFlagModule.h"
//#include "Option.h"
//#include "GeoView.h"

extern BOOL m_bGPSInfo;
//extern 	ST_GEO_RECT	NewDrawRect;
///////////////////////////////////////////////////////////////////////////////
// 在矩形列表中选取一个最大的矩形区域返回
void CalcRect(ST_GEO_PTXY* pPt, int nNum, ST_GEO_RECT& rect)
{
	double dLeft	= 1.7E308;
	double dTop		= 1.7E308;
	double dRight	= -1.7E308;
	double dBottom	= -1.7E308;

	for(int i = 0; i < nNum; i++)
	{
		if(pPt[i].dx < dLeft)
		{
			dLeft = pPt[i].dx;
		}

		if(pPt[i].dy < dTop)
		{
			dTop = pPt[i].dy;
		}

		if(pPt[i].dx > dRight)
		{
			dRight = pPt[i].dx;
		}
		
		if(pPt[i].dy > dBottom)
		{
			dBottom = pPt[i].dy;
		}
	}

	rect.dLeft		= (float)dLeft;
	rect.dTop		= (float)dTop;
	rect.dRight		= (float)dRight;
	rect.dBottom	= (float)dBottom;
}

///////////////////////////////////////////////////////////////////////////////
// 规范化矩形
void NormalizeRect(ST_GEO_RECT &rect)
{
	// 将左右互换
	if(rect.dLeft > rect.dRight)
	{
		rect.dLeft	= rect.dLeft + rect.dRight; 
		rect.dRight = rect.dLeft - rect.dRight;
		rect.dLeft	= rect.dLeft - rect.dRight;
	}
	
	// 将顶底互换
	if(rect.dTop > rect.dBottom)
	{
		rect.dTop	= rect.dTop + rect.dBottom;
		rect.dBottom = rect.dTop - rect.dBottom;
//		rect.dLeft	= rect.dTop - rect.dBottom;
		rect.dTop	= rect.dTop - rect.dBottom;
	}
}
// 读取文件数据
size_t Fread(char *pFileBuf,DWORD dwSize,DWORD &dwStation,void *buffer, size_t size, size_t count)
{
	if(pFileBuf == NULL)
	{
		return 0;
	}

	int nNum = size * count;
	int nRtn;
	if(dwStation + nNum > dwSize)
	{
		nRtn = dwSize - dwStation;
	}
	else
	{
		nRtn = nNum;
	}
//	strncpy((char*)buffer, pFileBuf + dwStation, nRtn );
	memcpy(buffer, pFileBuf + dwStation, nRtn);
	dwStation += nRtn;

	return nRtn / size;
}
///////////////////////////////////////////////////////////////////////////////
// 判断矩形交叉情况：0无交集，1有交集，2、3包含关系
int GeoRectIntersect(ST_GEO_RECT *sourceRect, ST_GEO_RECT *refRect)
{
	if((sourceRect->dRight	< refRect->dLeft) ||
   	   (sourceRect->dBottom < refRect->dTop) ||
	   (sourceRect->dLeft	> refRect->dRight) ||
	   (sourceRect->dTop	> refRect->dBottom) )
	{
		return 0;		// source和ref无交叉
	}
	else if((sourceRect->dLeft		< refRect->dLeft) &&
			(sourceRect->dRight		> refRect->dRight) &&
			(sourceRect->dTop		< refRect->dTop) &&
			(sourceRect->dBottom	> refRect->dBottom))
	{
		return 2;		// source包含ref
	}
	else if((sourceRect->dLeft		> refRect->dLeft) &&
			(sourceRect->dRight		< refRect->dRight) &&
			(sourceRect->dTop		> refRect->dTop) &&
			(sourceRect->dBottom	< refRect->dBottom))
	{
		return 3;		// ref包含source
	}
	else
	{
		return 1;		// source和ref交叉
	}
}

///////////////////////////////////////////////////////////////////////////////
// 判断已知一点是否在另外两点的区域内，如果在其内部返回TRUE
BOOL IsPtInRect(double dx, double dy, double dx1, double dy1, double dx2, double dy2)
{
	return ((dx >= (min(dx1, dx2) - 0)) &&
			(dx <= (max(dx1, dx2) + 0)) &&
			(dy >= (min(dy1, dy2) - 0)) &&
			(dy <= (max(dy1, dy2) + 0)));
}
BOOL IsPtInScreen(int x,int y,int x1,int x2,int y1,int y2)
{
	return(x >= x1 && x <= x2 && y >= y1 && y <= y2);
}
/*/判断点和矩形框的关系(0~9)
	 1----2-----3
	 |			|
	 4	  5		6		0(out rect)
	 |			|
	 7----8-----9
 */
BYTE GetRelationPt2Rect( double dx, double dy, double dx1, double dy1, double dx2, double dy2)
{
	double dbDeta = 0;
	if( fabs(dx-dx1) == dbDeta) 
	{
		if( fabs(dy-dy1) == dbDeta )
			return 1;
		else if( fabs(dy-dy2) == dbDeta )
			return 7;
		else if( dy > dy1 && dy < dy2 )
			return 4;
		else
			return 0;
	}
	else if( fabs(dx-dx2) == dbDeta )
	{
		if( fabs(dy-dy1) == dbDeta )
			return 3;
		else if( fabs(dy-dy2) == dbDeta )
			return 9;
		else if( dy > dy1 && dy < dy2 )
			return 6;
		else
			return 0;
	}
	else if( dx > dx1 && dx < dx2 )
	{
		if( fabs(dy-dy1) == dbDeta )
			return 2;
		else if( fabs(dy-dy2) == dbDeta )
			return 8;
		else if( dy > dy1 && dy < dy2 )
			return 5;
		else
			return 0;
	}
	else
		return 0;
	return 0;
}
char* getStrItemEx(char* szBuf,char szLim,int nPos,int &nLen)
{
	nLen = -1;
	if (nPos < 1) 
	{
		nPos = 1;
	}

	char* pch = (char*)szBuf;
	
	//if (strchr(pch,szLim) == NULL) return NULL;
	for(int i = 0 ; i < nPos -1 ; i++)
	{
		pch = strchr(pch,szLim);
		if(pch == NULL)
			return NULL;
		pch++;
	}
	
	nLen = 0;
	char ch;
	do
	{
		ch = pch[nLen++];
		if(ch == 0)
		{
			break;
			//nLen = 0;
		}
	}while(ch != szLim);
	nLen--;
	
	char *szInfo =new char[nLen];
	memcpy( szInfo, pch, nLen );

	return szInfo;
}
BOOL ExtractFilePath(char* szFileName,char* szPath)
{
	szPath[0] = 0;
	char *pch = strrchr(szFileName,'\\');
	if(pch == NULL)
		return FALSE;

	int nLen = pch - szFileName +1;
	memcpy(szPath,szFileName,nLen);
	szPath[nLen] = 0;
	return TRUE;
}
void ParseBreadthMapInfo( char* szInfo, int &nIndex, int &nParent, ST_GEO_RECT &rtBreadth )
{
	char *pBuf;
	int nLen;
	pBuf = getStrItemEx( szInfo, ',', 1, nLen );
	nIndex = atoi( pBuf );
	nParent = nIndex;
	delete[] pBuf;
	pBuf = getStrItemEx( szInfo, ',', 2, nLen );
	rtBreadth.dTop = float(atof( pBuf ));
	delete[] pBuf;
	pBuf = getStrItemEx( szInfo, ',', 3, nLen );
	rtBreadth.dBottom = float(atof( pBuf ));
	delete[] pBuf;
	pBuf = getStrItemEx( szInfo, ',', 4, nLen );
	rtBreadth.dLeft = float(atof( pBuf ));
	delete[] pBuf;
	pBuf = getStrItemEx( szInfo, ',', 5, nLen );
	rtBreadth.dRight = float(atof( pBuf ));
	delete[] pBuf;
/*	char *pBuf;
	int nLen;
	pBuf = getStrItemEx( szInfo, ',', 1, nLen );
	nIndex = atoi( pBuf );
	delete pBuf;
	pBuf = getStrItemEx( szInfo, ',', 2, nLen );
	nParent = atoi( pBuf );
	delete pBuf;
	pBuf = getStrItemEx( szInfo, ',', 3, nLen );
	rtBreadth.dTop = float(atof( pBuf ));
	delete pBuf;
	pBuf = getStrItemEx( szInfo, ',', 4, nLen );
	rtBreadth.dBottom = float(atof( pBuf ));
	delete pBuf;
	pBuf = getStrItemEx( szInfo, ',', 5, nLen );
	rtBreadth.dLeft = float(atof( pBuf ));
	delete pBuf;
	pBuf = getStrItemEx( szInfo, ',', 6, nLen );
	rtBreadth.dRight = float(atof( pBuf ));
	delete pBuf;
*/
}

///////////////////////////////////////////////////////////////////////////////
// 如果计算出来的交点位于两个矩形的公共区域，则为真交点，返回TRUE
BOOL GetIntersectedPoint(double X1, double Y1, double X2, double Y2, double X3,
						 double Y3, double X4, double Y4, double &XX, double &YY, int Flag)
{
	double K1, K2, B1, B2;		// 二条直线的斜率
    BOOL V1, V2;				// 有斜率为无穷大的线存在
	BOOL retV = FALSE;

	double m_dbMinOffset = 0;
	// X1, Y1, X2, Y2为第一条直线
	// X3, Y3, X4, Y4为第二条直线
	V1 = FALSE;
    V2 = FALSE;

    K1 = 1.7e308;
    K2 = 5 * 10 - 324;
    B1 = K1;
    B2 = K2;

	// 如果直线一垂直
	if(fabs(X2 - X1) > m_dbMinOffset)
	{
		K1 = (Y2 - Y1) / (X2 - X1);
		B1 = Y1 - K1 * X1;
	}
	else
	{
		XX = X1;
		V1 = TRUE;
	}

	// 如果直线二垂直
	if(fabs(X4 - X3) > m_dbMinOffset)
	{
		K2 = (Y4 - Y3) / (X4 - X3);
		B2 = Y3 - K2 * X3;
	}
	else
	{
		XX = X3;
		V2 = TRUE;
	}

	if(fabs(K1 - K2) < m_dbMinOffset || (V1 && V2))
	{
		return FALSE;
	}
	
	if(!(V1 || V2))
	{
		XX = (B1 - B2) / (K2 - K1);
		YY = K1 * XX + B1;
	}
	else if(V2)
	{
		YY = K1 * XX + B1;
	}
	else
	{
		YY = K2 * XX + B2;
	}

	if(Flag == 1)
	{
		// 如果计算出来的交点位于两个矩形的公共区域，则为真交点
		if(V1)
		{
			if(IsPtInRect(XX, YY, X3, Y3, X4, Y4) && (YY < max(Y1, Y2)) && (YY > min(Y1, Y2)))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else if(V2)
		{
			if(IsPtInRect(XX, YY, X1, Y1, X2, Y2) && (YY < max(Y3, Y4)) && (YY > min(Y3, Y4)))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			if(IsPtInRect(XX, YY, X1, Y1, X2, Y2) && IsPtInRect(XX, YY, X3, Y3, X4, Y4))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	else if(Flag == 2)
	{
		if(V2)
		{
			if((YY < max(Y3, Y4)) && (YY > min(Y3, Y4)))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			if(IsPtInRect(XX, YY, X3, Y3, X4, Y4))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	
	return retV;
}

///////////////////////////////////////////////////////////////////////////////
// 获得折线段上的离散点链表
BOOL GetPointsOnLine(ST_GEO_PTXY* pLine, int nPtNum, int nPart, ST_GEO_PTXY* pPtList)
{
//	if (nPtNum < nPart)
//	{
//		return FALSE;
//	}

	double dAve			= GetTotalLength(pLine, nPtNum) / (nPart + 1);
	double dAccuLen		= 0;	// 剩余段长
	double dRemainLen	= 0;

	double dx, dy;
	int nCurPart = 0;
	ST_GEO_PTXY *pPtStart;
	for(int i = 0; i<nPtNum-1; i++)
	{
		pPtStart = pLine + i;
		dAccuLen += GetLength(pPtStart->dx, pPtStart->dy, pLine[i+1].dx, pLine[i+1].dy);
		while(dAccuLen >= dAve)
		{
			GetPointOnArc(pPtStart->dx, pPtStart->dy, pLine[i+1].dx, pLine[i+1].dy,
						  (dAve - dRemainLen) / (dAccuLen - dRemainLen), dx, dy);

			pPtList[nCurPart].dx = (float)dx;
			pPtList[nCurPart].dy = (float)dy;
			pPtStart = pPtList + nCurPart;

			nCurPart++;
			dAccuLen	-= dAve;
			dRemainLen	= 0;

			if(nPart == nCurPart)
			{
				return TRUE;
			}
		}

		dRemainLen = dAccuLen;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// 计算一条线段的长度
double GetLength(double dx1, double dy1, double dx2, double dy2)
{
	return sqrt(pow(dx1 - dx2, 2) + pow(dy1 - dy2, 2));
}

///////////////////////////////////////////////////////////////////////////////
// 获得折线链表的总长度
double GetTotalLength(ST_GEO_PTXY *pPt, int nNum)
{
	double dTotLen = 0;

	for(int i = 0; i < nNum - 1; i++)
	{
		dTotLen += GetLength(pPt[i].dx, pPt[i].dy, pPt[i+1].dx, pPt[i+1].dy);
	}

	return dTotLen;
}

///////////////////////////////////////////////////////////////////////////////
// 根据斜率在弧上求得一点坐标
void GetPointOnArc(double dx1, double dy1, double dx2, double dy2,
				   double dRatio, double& xx, double& yy)
{
	xx = dx1 + (dx2- dx1) * dRatio;
	yy = dy1 + (dy2- dy1) * dRatio;
}

///////////////////////////////////////////////////////////////////////////////
// 得到剪切矩形
ST_GEO_PTXY* GetClipRgn(ST_GEO_PTXY* pPt, int nNum, ST_GEO_RECT &rect, 
						int *pnPtNum, int nObjType)
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

		if( (nObjType == EN_GOTYPE_POLY || nObjType == EN_GOTYPE_UPOLY)
				&& nRtnNo > 0 && nFirstPointCalc == 3)
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

///////////////////////////////////////////////////////////////////////////////
// 判断两个存在的矩形有交集，如果存在交集，返回TRUE
BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2)
{
	CRect rect;
	
	// This method makes a CRect object equal to the intersection of two existing rectangles.
	// The intersection is the largest rectangle contained in both existing rectangles.
	return rect.IntersectRect(lpRect1, lpRect2);
}

/*
void SetOwnCar(double dx,double dy,BOOL havePT)//设置本车位置
{
	Option.OwnPosition.dx = (float)dx;
	Option.OwnPosition.dy = (float)dy;
	CPtrList *m_pListF;	
	m_pListF = new CPtrList;
	m_FlagModule.AddFlagPt(dx, dy, NewDrawRect, m_pListF);
	ST_GEO_FLAG  *TEMPFLAG;
	if(!havePT)
	{
		m_FlagModule.m_pListFlagVtx->RemoveTail();
	}
	if(m_FlagModule.m_pListFlagVtx->GetCount() == 0)
	{
		TEMPFLAG = (ST_GEO_FLAG*)m_pListF->GetHead();
		TEMPFLAG->bcomplete = 3;	// 标志为起点，3说明为模拟本车数据
		m_FlagModule.m_pListFlagVtx->AddHead(TEMPFLAG);
	}
	else	
	{
		TEMPFLAG = (ST_GEO_FLAG*)m_pListF->GetHead();
		m_FlagModule.m_pListFlagVtx->RemoveHead();
		TEMPFLAG->bcomplete = 3;	// 标志为起点，3说明为模拟本车数据
		m_FlagModule.m_pListFlagVtx->AddHead(TEMPFLAG);
	}
	delete m_pListF;
}
*/
/*
void MoveToCenter(double dX, double dY)
{
	double dW, dH;
	
    dW = NewDrawRect.dRight - NewDrawRect.dLeft;
    dH = NewDrawRect.dBottom - NewDrawRect.dTop;
    NewDrawRect.dLeft	= (float)(dX - dW / 2);
    NewDrawRect.dTop	= (float)(dY - dH / 2);
    NewDrawRect.dRight	= (float)(NewDrawRect.dLeft + dW);
    NewDrawRect.dBottom = (float)(NewDrawRect.dTop + dH);
}
*/
unsigned short  GetPoiID(unsigned short nId, UINT nLayerID)
{
	unsigned short PoiId;
	PoiId = 0;
	switch(nLayerID) 
	{
	case 13: 
		if (nId > 0x5000 && nId < 0x5100) 
			return PoiId = 1;
		if (nId > 0x50ff && nId < 0x5180) 
			return PoiId = 2;
		if (nId == 0x5180) 
			return PoiId = 3;
		if (nId == 0x5200) 
			return PoiId = 4;
		if (nId == 0x5280) 
			return PoiId = 5;
		if (nId > 0x5280 && nId < 0x5380) 
			return PoiId = 6;
		if (nId < 0x5400) 
			return PoiId = 7;
		if (nId < 0x5480) 
			return PoiId = 8;
		if (nId < 0x5700) 
			return PoiId = 9;
		if (nId < 0x5780) 
			return PoiId = 10;
		if (nId < 0x5f00) 
			return PoiId = 11;
		if (nId == 0x5f00) 
			return PoiId = 12;
		break;
	case 14:
		if (nId == 0x4010) 
			return PoiId = 1;
		if (nId < 0x4100) 
			return PoiId = 2;
		if (nId < 0x4180) 
			return PoiId = 3;
		if (nId == 0x4180) 
			return PoiId = 4;
		if (nId == 0x4200) 
			return PoiId = 5;
		if (nId == 0x4280) 
			return PoiId = 6;
		if (nId == 0x4300) 
			return PoiId = 7;
		if (nId < 0x4400) 
			return PoiId = 8;
		if (nId == 0x4400) 
			return PoiId = 9;
		if (nId < 0x4500) 
			return PoiId = 10;
		if (nId < 0x4580) 
			return PoiId = 11;
		if (nId < 0x4600) 
			return PoiId = 12;
		if (nId == 0x4600) 
			return PoiId = 13;
		if (nId < 0x4700) 
			return PoiId = 14;
		if (nId < 0x5000) 
			return PoiId = 15;
		break;
	case 15:
		if (nId < 0xC100) 
			return PoiId = 1;
		if (nId < 0xC130) 
			return PoiId = 2;
		if (nId == 0xC130) 
			return PoiId = 3;
		if (nId < 0xCFFF) 
			return PoiId = 4;
		if (nId == 0xCFFF) 
			return PoiId = 5;
		if (nId > 0xCFFF)
			return PoiId = 6;
		break;
	case 16:
		if (nId == 0xB010) 
			return PoiId = 1;
		if (nId < 0xB100) 
			return PoiId = 2;
		if (nId == 0xB100) 
			return PoiId = 3;
		if (nId < 0xB130) 
			return PoiId = 4;
		if (nId < 0xB800) 
			return PoiId = 5;
		if (nId < 0xB900) 
			return PoiId = 6;
		if (nId < 0xBA00) 
			return PoiId = 7;
		if (nId < 0xBA80) 
			return PoiId = 8;
		if (nId == 0xBA80) 
			return PoiId = 9;
		if (nId < 0xBB80) 
			return PoiId = 10;
		if (nId < 0xBBA0) 
			return PoiId = 11;
		if (nId < 0xC080) 
			return PoiId = 12;
		break;
	case 17:
		if (nId < 0x9880) 
			return PoiId = 1;
		if (nId == 0x9880) 
			return PoiId = 2;
		if (nId < 0x9980) 
			return PoiId = 3;
		if (nId == 0x9980) 
			return PoiId = 4;
		if (nId < 0xA010) 
			return PoiId = 5;
		break;
	case 18:
		if (nId < 0x7080) 
			return PoiId = 1;
		if (nId < 0x7280) 
			return PoiId = 2;
		if (nId < 0x7510) 
			return PoiId = 3;
		if (nId < 0x7530) 
			return PoiId = 4;
		if (nId == 0x7530) 
			return PoiId = 5;
		if (nId == 0x75B0) 
			return PoiId = 6;
		if (nId == 0x7600) 
			return PoiId = 7;
		if (nId < 0x7700) 
			return PoiId = 8;
		if (nId == 0x7700) 
			return PoiId = 9;
		if (nId == 0x7780) 
			return PoiId = 10;
		if (nId < 0x7FFF) 
			return PoiId = 11;
		if (nId == 0x7FFF) 
			return PoiId = 12;
		break;
	case 19:
		if (nId == 0x8010) 
			return PoiId = 1;
		if (nId == 0x8020) 
			return PoiId = 2;
		if (nId < 0x8100) 
			return PoiId = 3;
		if (nId < 0x8700) 
			return PoiId = 4;
		if (nId == 0x8700) 
			return PoiId = 5;
		if (nId == 0x8A00) 
			return PoiId = 6;
		if (nId == 0x8A80) 
			return PoiId = 7;
		if (nId == 0x8B00) 
			return PoiId = 8;
		if (nId < 0x8D80) 
			return PoiId = 9;
		if (nId < 0x8FFF) 
			return PoiId = 10;
		if (nId == 0x8FFF) 
			return PoiId = 11;
		break;
	case 20:
		if (nId < 0x6100) 
			return PoiId = 1;
		if (nId == 0x6100) 
			return PoiId = 2;
		if (nId < 0x6300) 
			return PoiId = 3;
		if (nId == 0x6300) 
			return PoiId = 4;
		if (nId == 0x6400) 
			return PoiId = 5;
		if (nId < 0x6580) 
			return PoiId = 6;
		if (nId == 0x6580) 
			return PoiId = 7;
		break;
	case 21:
		if (nId < 0x2080) 
			return PoiId = 1;
		if (nId < 0x2100) 
			return PoiId = 2;
		if (nId < 0x2180) 
			return PoiId = 3;
		if (nId == 0x2180) 
			return PoiId = 4;
		if (nId == 0x2200) 
			return PoiId = 5;
		if (nId < 0x2800) 
			return PoiId = 6;
		if (nId == 0x2800) 
			return PoiId = 7;
		if (nId < 0x3010) 
			return PoiId = 8;
		break;
	case 22:
		if (nId == 0xA010) 
			return PoiId = 1;
		if (nId == 0xA018) 
			return PoiId = 2;
		if (nId < 0xA101) 
			return PoiId = 3;
		if (nId < 0xA180) 
			return PoiId = 4;
		if (nId < 0xA200) 
			return PoiId = 5;
		if (nId == 0xA200) 
			return PoiId = 6;
		if (nId == 0xA280) 
			return PoiId = 7;
		if (nId < 0xB010) 
			return PoiId = 8;
		break;
	case 23:
		if (nId < 0x3080) 
			return PoiId = 1;
		if (nId < 0x3100) 
			return PoiId = 2;
		if (nId < 0x3300) 
			return PoiId = 3;
		if (nId < 0x3400) 
			return PoiId = 4;
		if (nId == 0x3400) 
			return PoiId = 5;
		if (nId < 0x3500) 
			return PoiId = 6;
		if (nId < 0x3780) 
			return PoiId = 7;
		if (nId == 0x3780) 
			return PoiId = 8;
		if (nId == 0x3800) 
			return PoiId = 9;
		if (nId < 0x3900) 
			return PoiId = 10;
		if (nId < 0x4010) 
			return PoiId = 11;
		break;
	case 24:
		if (nId == 0x1000) 
			return PoiId = 1;
		if (nId < 0x1040) 
			return PoiId = 2;
		if (nId < 0x1050) 
			return PoiId = 3;
		if (nId < 0x1090) 
			return PoiId = 4;
		if (nId < 0x10A0) 
			return PoiId = 5;
		if (nId < 0x1110) 
			return PoiId = 6;
		if (nId < 0x1280) 
			return PoiId = 7;
		if (nId < 0x1800) 
			return PoiId = 8;
		if (nId < 0x1880) 
			return PoiId = 9;
		if (nId < 0x1A80) 
			return PoiId = 10;
		if (nId < 0x1F00) 
			return PoiId = 11;
		if (nId < 0x2010) 
			return PoiId = 12;
		break;
	}
	return PoiId;	
}
//////////////////////////////////////////////////////////////////////////////
//计算字符的机内码
int   GetCharCode(CString strIn)
{
	char *str;
	str = new char[2];
	UnicodeToChar(strIn, str, 2);
	
	unsigned char str1,str2;
	str1 = str[0];
	str2 = str[1];
	int id = str1 * 256 + str2 - 65536;
	
	delete[] str;
	return id;
}
//把图幅的ID转换为横向纵向的的个数
void ParseBMapID( int nBMID, int &nBMXNum, int &nBMYNum )
{
	CString str;
	str.Format(_T("%d"), nBMID );
	nBMXNum = _wtoi(str.Mid(2,2))*8+_wtoi(str.Right(1));
	nBMYNum = _wtoi(str.Left(2))*8 + _wtoi(str.Mid(4,1));
}
//根据偏移得到图幅的ID
int CalculateBMapID( int nBMXID, int nBMYID )
{
	int nXX = nBMXID/8;
	int nX = nBMXID%8;
	int nYY = nBMYID/8;
	int nY = nBMYID%8;
	int nBMID = nYY*10000+nXX*100+nY*10+nX;
	return nBMID;
}
//根据点位置得到图幅的ID
int CalculateBMapID( float dx, float dy )
{
	double	dOff = (dy-MAP_ORIGIN_CENTERY)*12.0;
	int nYOff = (int)(dOff>0.0?(dOff+0.5):(dOff-0.5));
	dOff = (dx-MAP_ORIGIN_CENTERX)*8.0;
	int nXOff = (int)(dOff>0.0?(dOff+0.5):(dOff-0.5));
	int nXX = (MAP_ORIGIN_XNUM+nXOff)/8;
	int nX = (MAP_ORIGIN_XNUM+nXOff)%8;
	int nYY = (MAP_ORIGIN_YNUM+nYOff)/8;
	int nY = (MAP_ORIGIN_YNUM+nYOff)%8;
	int nBMID = nYY*10000+nXX*100+nY*10+nX;
	return nBMID;
}
//根据点位置得到图幅的ID
void CalculateBMapIDInfo( float dx, float dy, int &nXNum, int &nYNum )
{
	double	dOff = (dy-MAP_ORIGIN_CENTERY)*12.0;
	int nYOff = (int)(dOff>0.0?(dOff+0.5):(dOff-0.5));
	dOff = (dx-MAP_ORIGIN_CENTERX)*8.0;
	int nXOff = (int)(dOff>0.0?(dOff+0.5):(dOff-0.5));
	nXNum = MAP_ORIGIN_XNUM+nXOff;
	nYNum = MAP_ORIGIN_YNUM+nYOff;
}
//根据点位置得到图幅的ID
void CalculateBMapIDInRect( ST_GEO_RECT rt, int nBMIDs[3000], int &nBNum )
{
	nBNum = 0;
	int nTop = 0;
	int nLeft = 0;
	int nBottom = 0;
	int nRight = 0;
	CalculateBMapIDInfo( rt.dLeft, rt.dTop, nLeft, nTop );
	CalculateBMapIDInfo( rt.dRight, rt.dBottom, nRight, nBottom );
	for( int i=nLeft;	i<=nRight;	i++ )
	{
		for( int j=nTop;	j<=nBottom;	j++ )
		{
			nBMIDs[nBNum] = CalculateBMapID( i, j );
			nBNum++;
		}
	}
}
//得到图幅的矩形框
ST_GEO_RECT CalculateBMapRect( int nBMID )
{
	int nBMXNum, nBMYNum;
	ParseBMapID( nBMID, nBMXNum, nBMYNum );
	ST_GEO_RECT rt;
	rt.dLeft = MAP_ORIGIN_CENTERX + (nBMXNum-MAP_ORIGIN_XNUM-0.5)*7.5/60.0;
	rt.dRight = MAP_ORIGIN_CENTERX + (nBMXNum-MAP_ORIGIN_XNUM+0.5)*7.5/60.0;
	rt.dTop = MAP_ORIGIN_CENTERY + (nBMYNum-MAP_ORIGIN_YNUM-0.5)*5.0/60.0;
	rt.dBottom = MAP_ORIGIN_CENTERY + (nBMYNum-MAP_ORIGIN_YNUM+0.5)*5.0/60.0;
	return rt;
}
double CalcArea( ST_GEO_PTXY pt1, ST_GEO_PTXY pt2, ST_GEO_PTXY pt3 )
{
	return (pt1.dx*pt2.dy - pt2.dx*pt1.dy + pt2.dx*pt3.dy - pt3.dx*pt2.dy + pt3.dx*pt1.dy - pt1.dx*pt3.dy) * 0.5;
	//(x1*y2 - x2*y1 + x2*y3 - x3*y2 + x3*y1 - x1*y3) * 0.5;
}
double AntiClock(ST_GEO_PTXY& center,ST_GEO_PTXY& outer)
{
	double alpha,delta_x,delta_y;
	delta_x = outer.dx-center.dx;
	delta_y = outer.dy-center.dy;
	alpha = atan2(delta_y,delta_x);
	

	if(alpha<0.0)
	{
		alpha = 2*PI+alpha;
	}
	return alpha;
}
double Clockwise(ST_GEO_PTXY& center,ST_GEO_PTXY& outer)
{
	double alpha,delta_x,delta_y;
	delta_x = outer.dx-center.dx;
	delta_y = outer.dy-center.dy;
	alpha = atan2(delta_y,delta_x);
	
	if(alpha>0.0)
	{
		alpha = 2*PI-alpha;
	}
	else
	{
		alpha = -alpha;
	}	
	return alpha;
}

#define LIMIT 0.001
void CreateRect(double d,ST_GEO_PTXY& pa,ST_GEO_PTXY& pb,ST_GEO_PTXY& pc,ST_GEO_PTXY& pd,ST_GEO_PTXY& pe,ST_GEO_PTXY& pf)
{
	if(fabs(pa.dx-pb.dx)<LIMIT)
	{
		if(pa.dy>pb.dy)
		{
			pc.dx = pa.dx+d;
			pd.dx = pc.dx;
			pe.dx = pa.dx-d;
			pf.dx = pe.dx;			
		}
		else
		{
			if(pa.dy<pb.dy)
			{
				pc.dx = pa.dx-d;
				pd.dx = pc.dx;
				pe.dx = pa.dx+d;
				pf.dx = pe.dx;				
			}
			else
			{
				TRACE(_T("data error!overlapping points."));
			}
		}
		pc.dy = pa.dy;
		pd.dy = pb.dy;
		pe.dy = pa.dy;
		pf.dy = pb.dy;

		return;
	}
	double k;
	k = (pb.dy-pa.dy)/(pb.dx-pa.dx);
	double denom = sqrt(1.0+k*k);
	if(pa.dx>=pb.dx+LIMIT)
	{
		pc.dx = pa.dx+(d*k)/denom;
		pe.dx = 2.0*pa.dx-pc.dx;
		pc.dy = pa.dy-d/denom;
		pe.dy = 2*pa.dy-pc.dy;
		pd.dx = pb.dx+(d*k)/denom;
		pf.dx = 2.0*pb.dx-pd.dx;
		pd.dy = pb.dy-d/denom;
		pf.dy = 2.0*pb.dy-pd.dy;

		return;
	}
	if(pa.dx<=pb.dx-LIMIT)
	{
		pc.dx = pa.dx-(d*k)/denom;
		pe.dx = 2.0*pa.dx-pc.dx;
		pc.dy = pa.dy+d/denom;
		pe.dy = 2.0*pa.dy-pc.dy;
		pd.dx = pb.dx-(d*k)/denom;
		pf.dx = 2.0*pb.dx-pd.dx;
		pd.dy = pb.dy+d/denom;
		pf.dy = 2.0*pb.dy-pd.dy;

		return;
	}	
}

/**
 * @brief 计算线的buffer
 *
 * @param 
 * @return
*/
BOOL CalcLineBuff( 
				  double dbWidth,
				  ST_GEO_PTXY *pPtList,
				  int nPtNum,
				  ST_GEO_PTXY pPtBuf[],
				  int &nPtBuf )	
{
	bool outwards;
	if(dbWidth>0)
		outwards = true;
	else
		outwards = false;
	dbWidth = fabs(dbWidth);
	
	int nSmooth = 4;
	/**************近似直线处理*********************/
	
	/***********************************************/

	ST_GEO_PTXY *pPtLList = new ST_GEO_PTXY[1024];
	ST_GEO_PTXY *pPtRList = new ST_GEO_PTXY[1024];
	int nPtLNum = 0;
	int nPtRNum = 0;
	if((nSmooth%2)!=0)
		nSmooth--;
	double delta_a = (2.0*PI)/(nSmooth);
			
	ST_GEO_PTXY pa,pb,pc,pd,pe,pf;

	/****************只有一个点的情况*********************/
	if(nPtNum==1)
	{
		ST_GEO_RECT rect;

		pa = pPtList[0];
		pb.dx = pa.dx+dbWidth;
		pb.dy = pa.dy;
		pPtBuf[nPtBuf] = pb;
		nPtBuf++;
		rect.dLeft = rect.dRight = pb.dx;
		rect.dBottom = rect.dTop = pb.dy;
		
		double now_angle;
		int count;
		
		for(count=1,now_angle = delta_a;count<nSmooth;count++,now_angle+=delta_a)
		{
			ST_GEO_PTXY p;
			p.dx = pa.dx+dbWidth*cos(now_angle);
			p.dy = pa.dy-dbWidth*sin(now_angle);

			pPtBuf[nPtBuf] = p;
			nPtBuf++;
			if(p.dx<rect.dLeft)rect.dLeft = p.dx;
			if(p.dx>rect.dRight)rect.dRight = p.dx;
			if(p.dy<rect.dBottom)rect.dBottom = p.dy;
			if(p.dy>rect.dTop)rect.dTop = p.dy;
		}
		pPtBuf[nPtBuf] = pb;
		nPtBuf++;
		return true;
	}
		
	/*******首末点重合（如面的边界线）时的处理*********/		
	ST_GEO_PTXY ps,pt;
	ps = pPtList[0];
	pt = pPtList[nPtNum-1];	
	if(!(ps.dx>pt.dx)&&!(ps.dx<pt.dx)&&!(ps.dy>pt.dy)&&!(ps.dy<pt.dy))//如果线闭合
	{
		ST_GEO_PTXY p1=pPtList[0];			
		ST_GEO_PTXY p2=pPtList[1];
		ST_GEO_PTXY pnew;
		pnew.dx = (p1.dx+p2.dx)/2.0;
		pnew.dy = (p1.dy+p2.dy)/2.0;
		pPtList[0] = pnew;
	}

	/**************首端点处理**********************/
	pa = pPtList[0];
	pb = pPtList[1];
	pc.dx = 0;pc.dy = 0;pd.dx = 0;pd.dy = 0;pe.dx = 0;pe.dy = 0;pf.dx = 0;pf.dy = 0;
	CreateRect(dbWidth,pa,pb,pc,pd,pe,pf);
	ST_GEO_PTXY mid;	
	double t = -(dbWidth/sqrt((pb.dx-pa.dx)*(pb.dx-pa.dx)+(pb.dy-pa.dy)*(pb.dy-pa.dy)));
	mid.dx = pa.dx+t*(pb.dx-pa.dx);
	mid.dy = pa.dy+t*(pb.dy-pa.dy);
	pPtLList[nPtLNum] = mid;
	nPtLNum++;
	pPtRList[nPtRNum] = mid;
	nPtRNum++;
	double alpha_1,alpha_2;
	alpha_1 = Clockwise(pa,mid);
	alpha_2 = Clockwise(pa,pc);
	if(alpha_2<alpha_1)
		alpha_2 += 2*PI;
	double now_angle=alpha_1+delta_a;
	while(now_angle<alpha_2)
	{
		ST_GEO_PTXY p;
		p.dx = pa.dx+dbWidth*cos(now_angle);
		p.dy = pa.dy-dbWidth*sin(now_angle);	
		pPtLList[nPtLNum] = p;
		nPtLNum++;
		now_angle += delta_a;
	}
	
	pPtLList[nPtLNum] = pc;
	nPtLNum++;
	pPtLList[nPtLNum] = pd;
	nPtLNum++;
	
	alpha_1 = AntiClock(pa,mid);
	alpha_2 = AntiClock(pa,pe);
	if(alpha_2<alpha_1)
		alpha_2 += 2*PI;
	now_angle=alpha_1+delta_a;
	while(now_angle<alpha_2)
	{
		ST_GEO_PTXY p;
		p.dx = pa.dx+dbWidth*cos(now_angle);
		p.dy = pa.dy+dbWidth*sin(now_angle);	
		pPtRList[nPtRNum] = p;
		nPtRNum++;
		now_angle += delta_a;
	}
	
	pPtRList[nPtRNum] = pe;
	nPtRNum++;
	pPtRList[nPtRNum] = pf;
	nPtRNum++;

	/****************中间点处理********************/
	
	for(int i=1;i<nPtNum-1;i++)
	{
		ST_GEO_PTXY former;
		former = pPtList[i-1];
		pa = pPtList[i];
		pb = pPtList[i+1];
		CreateRect(dbWidth,pa,pb,pc,pd,pe,pf);
		double bulge;
		double multi=sqrt((pa.dx-former.dx)*(pa.dx-former.dx)+(pa.dy-former.dy)*(pa.dy-former.dy))*sqrt((pb.dx-pa.dx)*(pb.dx-pa.dx)+(pb.dy-pa.dy)*(pb.dy-pa.dy));
		bulge = ((pa.dx- former.dx)*(pb.dy-pa.dy)-(pb.dx-pa.dx)*(pa.dy-former.dy))/multi;
		//bulge的具体意义??????

		if(bulge>1.0e-3)//凹点
		{
			double s,t;
			ST_GEO_PTXY p1,p2,p3,p4;
			int k=nPtLNum-1;
			p1 = pc;
			p2 = pd;
			bool intersect;
			intersect = false;
			
			p3 = pPtLList[k-1];
			p4 = pPtLList[k];
			s = ((p3.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p3.dy-p1.dy))/((p2.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p2.dy-p1.dy));
			t = ((p2.dx-p1.dx)*(p3.dy-p1.dy)-(p3.dx-p1.dx)*(p2.dy-p1.dy))/((p2.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p2.dy-p1.dy));
			if(s>0.0&&s<1.0&&t>0.0&&t<1.0)//相交
			{
				ST_GEO_PTXY temp_p;
				temp_p.dx = p1.dx+s*(p2.dx-p1.dx);
				temp_p.dy = p1.dy+s*(p2.dy-p1.dy);

				pPtLList[nPtLNum-1] = temp_p;
				pPtLList[nPtLNum] = pd;
				nPtLNum++;
				intersect = true;
			}
				
			if(!intersect)
			{					
				pPtLList[nPtLNum] = pc;
				nPtLNum++;
				pPtLList[nPtLNum] = pd;
				nPtLNum++;
			}				
		
			ST_GEO_PTXY right_former;
			right_former = pPtRList[nPtRNum-1];
			double alpha_1,alpha_2;
			alpha_1 = AntiClock(pa,right_former);
			alpha_2 = AntiClock(pa,pe);
			if(alpha_2<alpha_1)
				alpha_2 += 2.0*PI;
			double now_angle=alpha_1+delta_a;
			while(now_angle<alpha_2)
			{
				ST_GEO_PTXY p;
				p.dx = pa.dx+dbWidth*cos(now_angle);
				p.dy = pa.dy+dbWidth*sin(now_angle);
				now_angle += delta_a;
				pPtRList[nPtRNum] = p;
				nPtRNum++;
			}
			pPtRList[nPtRNum] = pe;
			nPtRNum++;
			pPtRList[nPtRNum] = pf;
			nPtRNum++;
		}
		if(bulge<-1.0e-3)//凸点
		{
			double s,t;
			ST_GEO_PTXY p1,p2,p3,p4;
			int k=nPtRNum-1;
			p1 = pe;
			p2 = pf;
			bool intersect;
			intersect = false;
			
			
			p3 = pPtRList[k-1];
			p4 = pPtRList[k];
			s = ((p3.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p3.dy-p1.dy))/((p2.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p2.dy-p1.dy));
			t = ((p2.dx-p1.dx)*(p3.dy-p1.dy)-(p3.dx-p1.dx)*(p2.dy-p1.dy))/((p2.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p2.dy-p1.dy));
			if(s>0.0&&s<1.0&&t>0.0&&t<1.0)//相交
			{
				ST_GEO_PTXY temp_p;
				temp_p.dx = p1.dx+s*(p2.dx-p1.dx);
				temp_p.dy = p1.dy+s*(p2.dy-p1.dy);
				pPtRList[nPtRNum-1] = temp_p;
				pPtRList[nPtRNum] = pf;
				nPtRNum++;
				intersect =  true;
			}
			
			if(!intersect)
			{						
				pPtRList[nPtRNum] = pe;
				nPtRNum++;
				pPtRList[nPtRNum] = pf;
				nPtRNum++;
			}
							
			ST_GEO_PTXY left_former;
			left_former = pPtLList[nPtLNum-1];
			double alpha_1,alpha_2;
			alpha_1 = Clockwise(pa,left_former);
			alpha_2 = Clockwise(pa,pc);
			if(alpha_2<alpha_1)
				alpha_2 += 2.0*PI;
			double now_angle=alpha_1+delta_a;
			while(now_angle<alpha_2)
			{
				ST_GEO_PTXY p;
				p.dx = pa.dx+dbWidth*cos(now_angle);
				p.dy = pa.dy-dbWidth*sin(now_angle);
				now_angle += delta_a;
				pPtLList[nPtLNum] = p;
				nPtLNum++;
			}
			pPtLList[nPtLNum] = pc;
			nPtLNum++;
			pPtLList[nPtLNum] = pd;
			nPtLNum++;
		}
		if(fabs(bulge)<=1.0e-3)
		{	
			double x = bulge;
			pPtLList[nPtLNum] = pd;
			nPtLNum++;
			pPtLList[nPtLNum] = pf;
			nPtLNum++;
		}
	}

	/**************末端点处理**********************/
	pa = pPtList[nPtNum-2];
	pb = pPtList[nPtNum-1];	
	CreateRect(dbWidth,pa,pb,pc,pd,pe,pf);
	double temp = sqrt((pb.dx-pa.dx)*(pb.dx-pa.dx)+(pb.dy-pa.dy)*(pb.dy-pa.dy)); 
	t = 1.0+(dbWidth/temp);
	mid.dx = pa.dx+t*(pb.dx-pa.dx);
	mid.dy = pa.dy+t*(pb.dy-pa.dy);
	
	alpha_1 = Clockwise(pb,pd);
	alpha_2 = Clockwise(pb,mid);
	if(alpha_2<alpha_1)
		alpha_2 += 2*PI;
	now_angle=alpha_1+delta_a;		

	while(now_angle<alpha_2)
	{
		ST_GEO_PTXY p;
		p.dx = pb.dx+dbWidth*cos(now_angle);
		p.dy = pb.dy-dbWidth*sin(now_angle);
		pPtLList[nPtLNum] = p;
		nPtLNum++;
		now_angle += delta_a;
	}
	pPtLList[nPtLNum] = mid;
	nPtLNum++;

	alpha_1 = AntiClock(pb,pf);
	alpha_2 = AntiClock(pb,mid);
	if(alpha_2<alpha_1)
		alpha_2 += 2*PI;
	now_angle=alpha_1+delta_a;		
	while(now_angle<alpha_2)
	{
		ST_GEO_PTXY p;
		p.dx = pb.dx+dbWidth*cos(now_angle);
		p.dy = pb.dy+dbWidth*sin(now_angle);
		pPtRList[nPtRNum] = p;
		nPtRNum++;
		now_angle += delta_a;
	}
	pPtRList[nPtRNum] = mid;
	nPtRNum++;

	/****************边界线的相交处理****************************/		
	ST_GEO_PTXY *pPtPList = new ST_GEO_PTXY[1024];
	int nPtPNum = 0;
	pPtPList[nPtPNum] = pPtLList[0];
	nPtPNum++;
	for(i=1;i<nPtLNum;i++)
	{			
		if(fabs(pPtLList[i].dx-pPtLList[i-1].dx)>LIMIT||fabs(pPtLList[i].dy-pPtLList[i-1].dy)>LIMIT)
		{
			pPtPList[nPtPNum] = pPtLList[i];
			nPtPNum++;
		}
	}
	ST_GEO_PTXY fp;
	fp = pPtRList[nPtRNum-1];
	ST_GEO_PTXY lp;
	lp = pPtPList[nPtPNum-1];
	if(fabs(fp.dx-lp.dx)>LIMIT||fabs(fp.dy-lp.dy)>LIMIT)
	{
		pPtPList[nPtPNum] = fp;
		nPtPNum++;
	}
	for(i=nPtRNum-2;i>0;i--)
	{
		if(fabs(pPtRList[i].dx-pPtRList[i+1].dx)>LIMIT||fabs(pPtRList[i].dy-pPtRList[i+1].dy)>LIMIT)
		{
			pPtPList[nPtPNum] = pPtRList[i];
			nPtPNum++;
		}
	}		
	pPtPList[nPtPNum] = pPtRList[0];
	nPtPNum++;
	
	ST_GEO_PTXY p1,p2,p3,p4;
	int *pIList = new int[1024];
	int nINum = 0;
	memset( pIList, 0, sizeof(int)*1024 );
	
	for(i=0;i<nPtPNum-2;i++)
	{				
		for(int j=i+2;j<nPtPNum-1;j++)
		{
			p1 = pPtPList[i];
			p2 = pPtPList[i+1];	
			p3 = pPtPList[j];
			p4 = pPtPList[j+1];

			//加入1.0e-10是因为浮点数原本相等在进行相等判断时却不相等，只有放宽相等的判断条件
			if(min(p1.dx,p2.dx)<=max(p3.dx,p4.dx)+1.0e-10&&min(p1.dy,p2.dy)<=max(p3.dy,p4.dy)+1.0e-10&&max(p1.dx,p2.dx)>=min(p3.dx,p4.dx)-1.0e-10&&max(p1.dy,p2.dy)>=min(p3.dy,p4.dy)-1.0e-10)				
			{
				double s,t;
				s = ((p3.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p3.dy-p1.dy))/((p2.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p2.dy-p1.dy));
				t = ((p2.dx-p1.dx)*(p3.dy-p1.dy)-(p3.dx-p1.dx)*(p2.dy-p1.dy))/((p2.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p2.dy-p1.dy));
				int x=s+t;
				//if(s>0.0&&s<=1.0&&t>0.0&&t<=1.0)//不取s=0和t=0的情况是为了避免交点的重复计数	
				if(s>-1.0e-10&&s<1.0+1.0e-10&&t>-1.0e-10&&t<1.0+1.0e-10)
				{						
					if(fabs(s)>1.0e-7&&fabs(1.0-s)>1.0e-7)//在线段中间相交，插入点
					{
						ST_GEO_PTXY p;
						p.dx = p1.dx+s*(p2.dx-p1.dx);
						p.dy = p1.dy+s*(p2.dy-p1.dy);
						for( int ii = nPtPNum; ii>i;	ii--)
							pPtPList[ii] = pPtPList[ii-1];
						pPtPList[i] = p;
						nPtPNum++;
						j++;
						for(int k=0;k<nINum;k++)
						{
							if(fabs(pIList[k])>i)
							{
								if(pIList[k]>0)
									pIList[k] = pIList[k]+1;					
								else
									pIList[k] = pIList[k]-1;
							}
						}													
					}
					else
					{
						//if fabs(s)<1.0e-7,continue;
						if(fabs(s)<=1.0e-7)
							continue;
						
						//if fabs(s-1)<1.0e-7,判断i+1是否在inter中，若没有再加入
						for(int m=0;m<nINum;m++)
						{
							if(fabs(pIList[m])==i+1)
								break;
						}
						//若有，continue;
						if(m!=nINum)
							continue;							
					}
					if(fabs(t)>1.0e-7&&fabs(1.0-t)>1.0e-7)//在线段中间相交
					{
						ST_GEO_PTXY p;
						p.dx = p3.dx+t*(p4.dx-p3.dx);
						p.dy = p3.dy+t*(p4.dy-p3.dy);
						for( int ii=nPtPNum; ii>j;	ii--)
							pPtPList[ii] = pPtPList[ii-1];
						pPtPList[j] = p;
						nPtPNum++;
						for(int k=0;k<nINum;k++)
						{							
							if(fabs(pIList[k])>j)
							{
								if(pIList[k]>0)
									pIList[k] = pIList[k]+1;					
								else
									pIList[k] = pIList[k]-1;
							}					
						}							
					}
					else
					{
						//if fabs(t)<1.0e-7,continue;
						if(fabs(t)<=1.0e-7)
							continue;

						//if fabs(t-1)<1.0e-6,判断j+1是否在inter中，若没有再加入							
						for(int m=0;m<nINum;m++)
						{
							if(fabs(pIList[m])==j+1)
								break;
						}
						//若有，continue;
						if(m!=nINum)
							continue;		
					}

					//只有加入了交点，才判断其入出特性						
					//center为boundary->m_points[i+1]或boundary->m_points[j+1]点
					//以center为圆心,分别计算从p1到p2、p3、p4的逆时针角度angle,angle1,angle2，
					//若angle1<angle<angle2或angle2<angle<angle1,则插入交点	
					ST_GEO_PTXY center,p1,p2,p3,p4;			
					center = pPtPList[i+1];
					p1.dx = pPtPList[i].dx-center.dx;
					p1.dy = pPtPList[i].dy-center.dy;
					p2.dx = pPtPList[i+2].dx-center.dx;
					p2.dy = pPtPList[i+2].dy-center.dy;
					p3.dx = pPtPList[j].dx-center.dx;
					p3.dy = pPtPList[j].dy-center.dy;
					if(j<nPtPNum-2)
					{
						p4.dx = pPtPList[j+2].dx-center.dx;
						p4.dy = pPtPList[j+2].dy-center.dy;
					}
					else
					{
						p4.dx = pPtPList[1].dx-center.dx;
						p4.dy = pPtPList[1].dy-center.dy;
					}
					
					double real,unreal,angle1,angle2,angle3;
					real = p2.dx*p1.dx+p2.dy*p1.dy;
					unreal = p2.dy*p1.dx-p2.dx*p1.dy;
					angle1 = atan2(unreal,real);
					if(angle1<0)
						angle1 += 2*PI;

					real = p3.dx*p1.dx+p3.dy*p1.dy;
					unreal = p3.dy*p1.dx-p3.dx*p1.dy;
					angle2 = atan2(unreal,real);
					if(angle2<0)
						angle2 += 2*PI;

					real = p4.dx*p1.dx+p4.dy*p1.dy;
					unreal = p4.dy*p1.dx-p4.dx*p1.dy;
					angle3 = atan2(unreal,real);
					if(angle3<0)
						angle3 += 2*PI;

					int nICount = 0;
					for( int ii=0;	ii<nINum;	ii++ )
					{
						if( fabs(pIList[ii])>=(i+1) )
							break;
						nICount++;
					}
					//如果由于边界重合出现相交，此时的交点入出性判断稍复杂,故在判断语句中加入了等号
					//if((angle1>angle2&&angle1<=angle3+1.0e2)||(angle1>=angle2-1.0e12&&angle1<angle3))
					if((angle1>angle2&&angle1<angle3))
					{
						for( int ii=nINum;	ii>nICount; ii-- )
							pIList[ii] = pIList[ii-1];
						pIList[nICount] = i+1;
						nINum++;
					}

					//if((angle1>angle3&&angle1<=angle2+1.0e-12)||(angle1>=angle3-1.0e-12&&angle1<angle2))
					if((angle1>angle3&&angle1<angle2))
					{
						for( int ii=nINum;	ii>nICount; ii-- )
							pIList[ii] = pIList[ii-1];
						pIList[nICount] = -(i+1);
						nINum++;
					}

					nICount = 0;
					for( ii=0;	ii<nINum;	ii++ )
					{
						if( fabs(pIList[ii])>=(j+1) )
							break;
						nICount++;
					}

					//if((angle1>angle2&&angle1<=angle3+1.0e-12)||(angle1>angle2-1.0e-12&&angle1<angle3))
					if((angle1>angle2&&angle1<angle3))
					{
						for( int ii=nINum;	ii>nICount; ii-- )
							pIList[ii] = pIList[ii-1];
						pIList[nICount] = -(j+1);
						nINum++;
						continue;
					}

					//if((angle1>angle3&&angle1<=angle2+1.0e12)||(angle1>=angle3-1.0e12&&angle1<angle2))
					if((angle1>angle3&&angle1<angle2))
					{
						for( int ii=nINum;	ii>nICount; ii-- )
							pIList[ii] = pIList[ii-1];
						pIList[nICount] = (j+1);
						nINum++;
						continue;
					}						
				}				
			}
		}
	}

	/************************************************************/
	if(nINum==0)//没有交点。
	{
		for(int j=0;j<nPtPNum;j++)
		{
			pPtBuf[nPtBuf] = pPtPList[j];
			nPtBuf++;
		}
	}
	else//存在交点
	{		
		for(int i=0;i<nINum-1;i++)
		{
			int from,to;
			from = pIList[i];
			to = pIList[i+1];
			ST_GEO_PTXY p1,p2;
			p1 = pPtPList[abs(from)];
			p2 = pPtPList[abs(to)];
			if(from<0&&to>0&&fabs(from)!=fabs(to))//保留出点到入点的部分				
			{
				for(int j=fabs(from);j<=to;j++)
				{
					pPtBuf[nPtBuf] = pPtPList[j];
					nPtBuf++;
				}
			}
		}		
		if(pIList[i]<0&&pIList[0]>0&&fabs(pIList[i])!=fabs(pIList[0]))//末尾的出点到开始的入点
		{					
			for(int j=fabs(pIList[i]);j<nPtPNum;j++)
			{
				pPtBuf[nPtBuf] = pPtPList[j];
				nPtBuf++;
			}
			for(int k=0;k<=fabs(pIList[0]);k++)
			{
				pPtBuf[nPtBuf] = pPtPList[k];
				nPtBuf++;
			}
		}
	}
	delete[] pPtLList;
	pPtLList = NULL;
	delete[] pPtRList;
	pPtRList = NULL;
	delete[] pPtPList;
	pPtPList = NULL;
	delete[] pIList;
	pIList = NULL;
	return true;		
}
