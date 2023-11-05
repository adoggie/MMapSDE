
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GeoGlobe.h"
#include "Globe.h"
//#include "GeoFlagModule.h"
//#include "Option.h"
//#include "GeoView.h"

// added by scott for overload fabs(int)
 

extern BOOL m_bGPSInfo;
//extern 	ST_GEO_RECT	NewDrawRect;
///////////////////////////////////////////////////////////////////////////////
// �ھ����б���ѡȡһ�����ľ������򷵻�
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
// �淶������
void NormalizeRect(ST_GEO_RECT &rect)
{
	// �����һ���
	if(rect.dLeft > rect.dRight)
	{
		rect.dLeft	= rect.dLeft + rect.dRight; 
		rect.dRight = rect.dLeft - rect.dRight;
		rect.dLeft	= rect.dLeft - rect.dRight;
	}
	
	// �����׻���
	if(rect.dTop > rect.dBottom)
	{
		rect.dTop	= rect.dTop + rect.dBottom;
		rect.dBottom = rect.dTop - rect.dBottom;
//		rect.dLeft	= rect.dTop - rect.dBottom;
		rect.dTop	= rect.dTop - rect.dBottom;
	}
}
// ��ȡ�ļ�����,�ӻ��������ж�ȡָ�����ֽ�����
// dwStation - ƫ����
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
// �жϾ��ν��������0�޽�����1�н�����2��3������ϵ
int GeoRectIntersect(ST_GEO_RECT *sourceRect, ST_GEO_RECT *refRect)
{
	if((sourceRect->dRight	< refRect->dLeft) ||
   	   (sourceRect->dBottom < refRect->dTop) ||
	   (sourceRect->dLeft	> refRect->dRight) ||
	   (sourceRect->dTop	> refRect->dBottom) )
	{
		return 0;		// source��ref�޽���
	}
	else if((sourceRect->dLeft		< refRect->dLeft) &&
			(sourceRect->dRight		> refRect->dRight) &&
			(sourceRect->dTop		< refRect->dTop) &&
			(sourceRect->dBottom	> refRect->dBottom))
	{
		return 2;		// source����ref
	}
	else if((sourceRect->dLeft		> refRect->dLeft) &&
			(sourceRect->dRight		< refRect->dRight) &&
			(sourceRect->dTop		> refRect->dTop) &&
			(sourceRect->dBottom	< refRect->dBottom))
	{
		return 3;		// ref����source
	}
	else
	{
		return 1;		// source��ref����
	}
}

///////////////////////////////////////////////////////////////////////////////
// �ж���֪һ���Ƿ�����������������ڣ���������ڲ�����TRUE
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
/*/�жϵ�;��ο�Ĺ�ϵ(0~9)
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
// �����������Ľ���λ���������εĹ���������Ϊ�潻�㣬����TRUE
BOOL GetIntersectedPoint(double X1, double Y1, double X2, double Y2, double X3,
						 double Y3, double X4, double Y4, double &XX, double &YY, int Flag)
{
	double K1, K2, B1, B2;		// ����ֱ�ߵ�б��
    BOOL V1, V2;				// ��б��Ϊ�������ߴ���
	BOOL retV = FALSE;

	double m_dbMinOffset = 0;
	// X1, Y1, X2, Y2Ϊ��һ��ֱ��
	// X3, Y3, X4, Y4Ϊ�ڶ���ֱ��
	V1 = FALSE;
    V2 = FALSE;

    K1 = 1.7e308;
    K2 = 5 * 10 - 324;
    B1 = K1;
    B2 = K2;

	// ���ֱ��һ��ֱ
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

	// ���ֱ�߶���ֱ
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
		// �����������Ľ���λ���������εĹ���������Ϊ�潻��
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
// ������߶��ϵ���ɢ������
BOOL GetPointsOnLine(ST_GEO_PTXY* pLine, int nPtNum, int nPart, ST_GEO_PTXY* pPtList)
{
//	if (nPtNum < nPart)
//	{
//		return FALSE;
//	}

	double dAve			= GetTotalLength(pLine, nPtNum) / (nPart + 1);
	double dAccuLen		= 0;	// ʣ��γ�
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
// ����һ���߶εĳ���
double GetLength(double dx1, double dy1, double dx2, double dy2)
{
	//return sqrt(pow(dx1 - dx2, 2) + pow(dy1 - dy2, 2));
	return sqrt( (dx1 - dx2)*(dx1 - dx2) + (dy1 - dy2)*(dy1 - dy2) );
	
}

///////////////////////////////////////////////////////////////////////////////
// �������������ܳ���
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
// ����б���ڻ������һ������
void GetPointOnArc(double dx1, double dy1, double dx2, double dy2,
				   double dRatio, double& xx, double& yy)
{
	xx = dx1 + (dx2- dx1) * dRatio;
	yy = dy1 + (dy2- dy1) * dRatio;
}

///////////////////////////////////////////////////////////////////////////////
// �õ����о����ڳ��ֵĵ㼯��
static ST_GEO_PTXY g_ptClip1[40000];
static ST_GEO_PTXY g_ptClip2[40000];
ST_GEO_PTXY* GetClipRgn(ST_GEO_PTXY* pPt, int nNum, ST_GEO_RECT &rect, 
						int *pnPtNum, int nObjType)
{
	int i, nSide;			// nSide������ָ�����бߵ����ϡ��ҡ���
    double dx, dy;

	ST_GEO_PTXY *pTempList = pPt, *pRtnList;
	pRtnList = NULL;

    double dx1, dy1, dx2, dy2;
    BOOL bAccepted, bLastAccepted;
	
	// �жϵ�һ���Ƿ���������ģ�1����ʼֵ��2��ԭ������ֵ��3�������ֵ
    int nFirstPointCalc;
	int nRtnNo;

	*pnPtNum = nNum;
	for(nSide = 1; nSide <= 4; nSide++) //�������ĸ����ڵĵ�
	{
		if( !pRtnList || pRtnList == g_ptClip2 )
			pRtnList = g_ptClip1;
		else
			pRtnList = g_ptClip2;
//		int nPtNum = *pnPtNum * 2;
//		if( nPtNum > 5000 )
//			nPtNum = *pnPtNum + 1000;
//		pRtnList = NULL;
//		TRY
//		{
//			pRtnList	= new ST_GEO_PTXY[nPtNum];
//		}
//		CATCH( CMemoryException, e )
//		{
//			if( pTempList )
//				delete[] pTempList;
//			*pnPtNum = 0;
//			return NULL;
//		}
//		AND_CATCH( CException, e )
//		{
//			if( pTempList )
//				delete[] pTempList;
//			*pnPtNum = 0;
//			return NULL;
//		}
//		END_CATCH

//		if( !pRtnList )
//		{
//			if( pTempList )
//				delete[] pTempList;
//			*pnPtNum = 0;
//			return NULL;
//		}
        bAccepted	= bLastAccepted = TRUE;
		nRtnNo		= 0;

        nFirstPointCalc = 1;
		for(i = 0; i < *pnPtNum; i++)
		{
         	// �жϵ��Ƿ�ñ�ȡ��
			switch (nSide)
			{
			case 1 :
				bAccepted = pTempList[i].dx >= rect.dLeft;
				dx1 = rect.dLeft;
				dy1 = rect.dTop;
				dx2 = rect.dLeft;
				dy2 = rect.dBottom;
                break;
            case 2 : 
				bAccepted = pTempList[i].dy >= rect.dTop;
				dx1 = rect.dLeft;
				dy1 = rect.dTop;
				dx2 = rect.dRight;
				dy2 = rect.dTop;
				break;
			case 3 : 
                bAccepted = pTempList[i].dx <= rect.dRight;
				dx1 = rect.dRight;
				dy1 = rect.dTop;
				dx2 = rect.dRight;
				dy2 = rect.dBottom;
				break;
            case 4 :
                bAccepted = pTempList[i].dy <= rect.dBottom;
				dx1 = rect.dLeft;
				dy1 = rect.dBottom;
				dx2 = rect.dRight;
				dy2 = rect.dBottom;
				break;
			}

			// �ڷ�Χ��
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

//		if(pTempList != pPt)
//		{
//			delete [] pTempList;
//		}

		pTempList	= pRtnList;
		*pnPtNum	= nRtnNo;
	}

	return pRtnList;
}

///////////////////////////////////////////////////////////////////////////////
// �ж��������ڵľ����н�����������ڽ���������TRUE
BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2)
{
	swRect rect;
	
	// This method makes a swRect object equal to the intersection of two existing rectangles.
	// The intersection is the largest rectangle contained in both existing rectangles.
	return rect.IntersectRect(lpRect1, lpRect2);
}

/*
void SetOwnCar(double dx,double dy,BOOL havePT)//���ñ���λ��
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
		TEMPFLAG->bcomplete = 3;	// ��־Ϊ��㣬3˵��Ϊģ�Ȿ������
		m_FlagModule.m_pListFlagVtx->AddHead(TEMPFLAG);
	}
	else	
	{
		TEMPFLAG = (ST_GEO_FLAG*)m_pListF->GetHead();
		m_FlagModule.m_pListFlagVtx->RemoveHead();
		TEMPFLAG->bcomplete = 3;	// ��־Ϊ��㣬3˵��Ϊģ�Ȿ������
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
//�����ַ��Ļ�����
int   GetCharCode(swString strIn)
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
//��ͼ����IDת��Ϊ��������ĵĸ���
void ParseBMapID( int nBMID, int &nBMXNum, int &nBMYNum )
{
	swString str;
	str.Format(_T("%d"), nBMID );
	nBMXNum = _wtoi(str.Mid(2,2))*8+_wtoi(str.Right(1));
	nBMYNum = _wtoi(str.Left(2))*8 + _wtoi(str.Mid(4,1));
}
//����ƫ�Ƶõ�ͼ����ID
int CalculateBMapID( int nBMXID, int nBMYID )
{
	int nXX = nBMXID/8;
	int nX = nBMXID%8;
	int nYY = nBMYID/8;
	int nY = nBMYID%8;
	int nBMID = nYY*10000+nXX*100+nY*10+nX;
	return nBMID;
}
//���ݵ�λ�õõ�ͼ����ID
int CalculateBMapID( float dx, float dy )
{
	double	dOff = (dy-MAP_ORIGIN_CENTERY)*12.0; //γ�ȷ����������5��Ϊһ����λ
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
//���ݵ�λ�õõ�ͼ����ID
void CalculateBMapIDInfo( float dx, float dy, int &nXNum, int &nYNum )
{	//0.5ֵָԭ�������������
	double	dOff = (dy-MAP_ORIGIN_CENTERY)*12.0;
	int nYOff = (int)(dOff>0.0?(dOff+0.5):(dOff-0.5));
	dOff = (dx-MAP_ORIGIN_CENTERX)*8.0;
	int nXOff = (int)(dOff>0.0?(dOff+0.5):(dOff-0.5));
	nXNum = MAP_ORIGIN_XNUM+nXOff;
	nYNum = MAP_ORIGIN_YNUM+nYOff;
}
//���ݵ�λ�õõ�ͼ����ID,��ȡ�뵱ǰ���ӵ��������ཻ���������б�
//���������ڲ�ѯ���ֵ�����ͼ����
void CalculateBMapIDInRect( ST_GEO_RECT rt, int nBMIDs[3000], int &nBNum )
{
	nBNum = 0;
	int nTop = 0;
	int nLeft = 0;
	int nBottom = 0;
	int nRight = 0;
	CalculateBMapIDInfo( rt.dLeft, rt.dTop, nLeft, nTop );  //���������ԽǶ��������ڵ����ݴ洢������
	CalculateBMapIDInfo( rt.dRight, rt.dBottom, nRight, nBottom );
	for( int i=nLeft;	i<=nRight;	i++ ) //ɨ��ÿ����������
	{
		for( int j=nTop;	j<=nBottom;	j++ )
		{
			nBMIDs[nBNum] = CalculateBMapID( i, j ); //����xy��λ���Ӵճ�ͼ��ı��
			nBNum++;
		}
	}
}
//�õ�ͼ���ľ��ο�
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
 * @brief �����ߵ�buffer
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
	/**************����ֱ�ߴ���*********************/
	
	/***********************************************/

	ST_GEO_PTXY *pPtLList = new ST_GEO_PTXY[1024];
	ST_GEO_PTXY *pPtRList = new ST_GEO_PTXY[1024];
	int nPtLNum = 0;
	int nPtRNum = 0;
	if((nSmooth%2)!=0)
		nSmooth--;
	double delta_a = (2.0*PI)/(nSmooth);
			
	ST_GEO_PTXY pa,pb,pc,pd,pe,pf;

	/****************ֻ��һ��������*********************/
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
		
	/*******��ĩ���غϣ�����ı߽��ߣ�ʱ�Ĵ���*********/		
	ST_GEO_PTXY ps,pt;
	ps = pPtList[0];
	pt = pPtList[nPtNum-1];	
	if(!(ps.dx>pt.dx)&&!(ps.dx<pt.dx)&&!(ps.dy>pt.dy)&&!(ps.dy<pt.dy))//����߱պ�
	{
		ST_GEO_PTXY p1=pPtList[0];			
		ST_GEO_PTXY p2=pPtList[1];
		ST_GEO_PTXY pnew;
		pnew.dx = (p1.dx+p2.dx)/2.0;
		pnew.dy = (p1.dy+p2.dy)/2.0;
		pPtList[0] = pnew;
	}

	/**************�׶˵㴦��**********************/
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

	/****************�м�㴦��********************/
	int i;
	for(i=1;i<nPtNum-1;i++)
	{
		ST_GEO_PTXY former;
		former = pPtList[i-1];
		pa = pPtList[i];
		pb = pPtList[i+1];
		CreateRect(dbWidth,pa,pb,pc,pd,pe,pf);
		double bulge;
		double multi=sqrt((pa.dx-former.dx)*(pa.dx-former.dx)+(pa.dy-former.dy)*(pa.dy-former.dy))*sqrt((pb.dx-pa.dx)*(pb.dx-pa.dx)+(pb.dy-pa.dy)*(pb.dy-pa.dy));
		bulge = ((pa.dx- former.dx)*(pb.dy-pa.dy)-(pb.dx-pa.dx)*(pa.dy-former.dy))/multi;
		//bulge�ľ�������??????

		if(bulge>1.0e-3)//����
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
			if(s>0.0&&s<1.0&&t>0.0&&t<1.0)//�ཻ
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
		if(bulge<-1.0e-3)//͹��
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
			if(s>0.0&&s<1.0&&t>0.0&&t<1.0)//�ཻ
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

	/**************ĩ�˵㴦��**********************/
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

	/****************�߽��ߵ��ཻ����****************************/		
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

			//����1.0e-10����Ϊ������ԭ������ڽ�������ж�ʱȴ����ȣ�ֻ�зſ���ȵ��ж�����
			if(min(p1.dx,p2.dx)<=max(p3.dx,p4.dx)+1.0e-10&&min(p1.dy,p2.dy)<=max(p3.dy,p4.dy)+1.0e-10&&max(p1.dx,p2.dx)>=min(p3.dx,p4.dx)-1.0e-10&&max(p1.dy,p2.dy)>=min(p3.dy,p4.dy)-1.0e-10)				
			{
				double s,t;
				s = ((p3.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p3.dy-p1.dy))/((p2.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p2.dy-p1.dy));
				t = ((p2.dx-p1.dx)*(p3.dy-p1.dy)-(p3.dx-p1.dx)*(p2.dy-p1.dy))/((p2.dx-p1.dx)*(p3.dy-p4.dy)-(p3.dx-p4.dx)*(p2.dy-p1.dy));
				int x=s+t;
				//if(s>0.0&&s<=1.0&&t>0.0&&t<=1.0)//��ȡs=0��t=0�������Ϊ�˱��⽻����ظ�����	
				if(s>-1.0e-10&&s<1.0+1.0e-10&&t>-1.0e-10&&t<1.0+1.0e-10)
				{						
					if(fabs(s)>1.0e-7&&fabs(1.0-s)>1.0e-7)//���߶��м��ཻ�������
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
						
						//if fabs(s-1)<1.0e-7,�ж�i+1�Ƿ���inter�У���û���ټ���
						int m;
						for(m=0;m<nINum;m++)
						{
							if(fabs(pIList[m])==i+1)
								break;
						}
						//���У�continue;
						if(m!=nINum)
							continue;							
					}
					if(fabs(t)>1.0e-7&&fabs(1.0-t)>1.0e-7)//���߶��м��ཻ
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

						//if fabs(t-1)<1.0e-6,�ж�j+1�Ƿ���inter�У���û���ټ���							
						int m;
						for(m=0;m<nINum;m++)
						{
							if(fabs(pIList[m])==j+1)
								break;
						}
						//���У�continue;
						if(m!=nINum)
							continue;		
					}

					//ֻ�м����˽��㣬���ж����������						
					//centerΪboundary->m_points[i+1]��boundary->m_points[j+1]��
					//��centerΪԲ��,�ֱ�����p1��p2��p3��p4����ʱ��Ƕ�angle,angle1,angle2��
					//��angle1<angle<angle2��angle2<angle<angle1,����뽻��	
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
					int ii;
					for( ii=0;	ii<nINum;	ii++ )
					{
						if( fabs(pIList[ii])>=(i+1) )
							break;
						nICount++;
					}
					//������ڱ߽��غϳ����ཻ����ʱ�Ľ���������ж��Ը���,�����ж�����м����˵Ⱥ�
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
	if(nINum==0)//û�н��㡣
	{
		for(int j=0;j<nPtPNum;j++)
		{
			pPtBuf[nPtBuf] = pPtPList[j];
			nPtBuf++;
		}
	}
	else//���ڽ���
	{	int i;	
		for(i=0;i<nINum-1;i++)
		{
			int from,to;
			from = pIList[i];
			to = pIList[i+1];
			ST_GEO_PTXY p1,p2;
			p1 = pPtPList[abs(from)];
			p2 = pPtPList[abs(to)];
			if(from<0&&to>0&&fabs(from)!=fabs(to))//�������㵽���Ĳ���				
			{
				for(int j=fabs(from);j<=to;j++)
				{
					pPtBuf[nPtBuf] = pPtPList[j];
					nPtBuf++;
				}
			}
		}		
		if(pIList[i]<0&&pIList[0]>0&&fabs(pIList[i])!=fabs(pIList[0]))//ĩβ�ĳ��㵽��ʼ�����
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
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void ST_TURN_INFO::DumpState() const	
{  
    if(byRMType == 0)
    {
    TRACE3("%.1f km(%.1f m)·��[%s] .\n",dbD2Pre/1000,dbD2Pre,strRName);
    TRACE2("\t%s\t%s",strLVoice,strSVoice);
    }
    else
    {
    TRACE3("%.1f km(%.1f m)������[%d] .\n",dbD2Pre/1000,dbD2Pre,byRMobile);
    TRACE2("\t%s\t%s",strLVoice,strSVoice);

    }
}
void ST_TURN_INFO::AssertValid() const	
{   
}
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG

struct stuVoiceTokenItem
{
    int nID;
    TCHAR* pStr;
};

stuVoiceTokenItem VoiceTokenTable[] = 
{
    {1,_T("ֱ��")},
    {2,_T("����")},
    {3,_T("����")},
    {4,_T("����")},
    {5,_T("U�͹�")},
    {6,_T("����·")},
    {7,_T("�ѵ�")},
    {8,_T("��ͨ��·")},
    {33,_T("���")},
    {34,_T("�ɿ�")}, 
    {9, _T("�ǻ���")},
    {10,_T("���ǻ���")},
    {11,_T("�����··��:һ��")},
    {12,_T("�����··��:��һ��")},

    {13,_T("��·���Լݳ�:��")},
    {14,_T("��·���Լݳ�:��")},

    {15,_T("����:��")},
    {16,_T("����:��")},
    {37,_T("����:��")},
    {38,_T("����:��")},

    {17,_T("��������ڵ�·ͬ���ĵ�·(����ڵ�·��):��")},
    {18,_T("��������ڵ�·ͬ���ĵ�·(����ڵ�·��):��")},
    {19,_T("��������ڵ�·ͬ���ĵ�·(�����ڵ�·��):��")},
    {20,_T("��������ڵ�·ͬ���ĵ�·(�����ڵ�·��):��")},
    {21,_T("�Ƿ���ڳ�����ڵ�·���������·:��")},
    {22,_T("�Ƿ���ڳ�����ڵ�·���������·:��")},
    {23,_T("�Ƿ����IMI:��")},
    {24,_T("�Ƿ����IMI:��")},
    {25,_T("�Ƿ񵼵�Ŀ�ĵ�:��")},
    {26,_T("�Ƿ񵼵�Ŀ�ĵ�:��")},
    {27,_T("���䷽��:��")},
    {28,_T("���䷽��:��")},
    {29,_T("��ڵ�·����ڵ�·֮���Ƿ��л���:��")},
    {30,_T("��ڵ�·����ڵ�·֮���Ƿ��л���:��")},
    {31,_T("��ڵ�·�ͳ��ڵ�·�Ƿ�Ϊͬһ����·:��")},
    {32,_T("��ڵ�·�ͳ��ڵ�·�Ƿ�Ϊͬһ����·:��")},
    {35,_T("�Ƿ�Ϊ�շ�·��:��")},
    {36,_T("�Ƿ�Ϊ�շ�·��:��")},

};
#define VOICE_TRACE(p) {\
    BOOL bFind = FALSE;\
    for(int i = 0; i < sizeof(VoiceTokenTable)/sizeof(stuVoiceTokenItem); i++)\
    {\
        if((p) == VoiceTokenTable[i].nID)\
        {\
            bFind = TRUE;\
            TRACE1("%s\n",VoiceTokenTable[i].pStr);\
            break;\
        }\
    }\
    if(!bFind)\
    {\
        TRACE0("error voice token id.\n");\
    }\
}
    
void ST_VOICE_NODE::DumpState() const	
{  
    TRACE0("\tNode:\n");
    TRACE3("\t\t\tAngle[%.3d]\tConfn[%.3d]\tConf2[%.3d]\n",this->byAngle,this->byConfusion,this->byConfusion2);
    TRACE3("\t\t\tDesti[%.3d]\tEnName[%.3d]\tExName[%.3d]\n",this->byDestination,this->byEnName,this->byExName);
    TRACE3("\t\t\tIMI[%.3d]\tOther[%.3d]\tRound[%.3d]\n",this->byIMI,this->byOtherStreet,this->byRound);
    TRACE3("\t\t\tSmName[%.3d]\tSmRoad[%.3d]\tTurn[%.3d]\n",this->bySameName,this->bySameRoad,this->byTurn);
    
    TRACE0("\tNode:\n");
    TRACE0("\t\t\t");VOICE_TRACE(byAngle);
    TRACE0("\t\t\tͬ��");VOICE_TRACE(byConfusion);
    TRACE0("\t\t\t����");VOICE_TRACE(byConfusion2);

    TRACE0("\t\t\t");VOICE_TRACE(byDestination);
    TRACE0("\t\t\t");VOICE_TRACE(byEnName);
    TRACE0("\t\t\t");VOICE_TRACE(byExName);

    TRACE0("\t\t\t");VOICE_TRACE(byIMI);
    TRACE0("\t\t\t");VOICE_TRACE(byOtherStreet);
    TRACE0("\t\t\t");VOICE_TRACE(byRound);

    TRACE0("\t\t\t");VOICE_TRACE(bySameName);
    TRACE0("\t\t\t");VOICE_TRACE(bySameRoad);
    TRACE0("\t\t\t");VOICE_TRACE(byTurn);

    TRACE_SLEEP_BY_TIMES(5);
}
void ST_VOICE_NODE::AssertValid() const	
{   
}
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void ST_VOICE_RUT::DumpState() const	
{  
    TRACE2("\tRut:\tAdd[%.3d]Chg[%.3d]",this->byAdd,this->byCharge);
    TRACE2("Drv[%.3d]Typ[%.3d].\n",this->byDrivable,this->byType);

    TRACE0("\t\t\t");VOICE_TRACE(byAdd);
    TRACE0("\t\t\t");VOICE_TRACE(byCharge);
    TRACE0("\t\t\t");VOICE_TRACE(byDrivable);
    TRACE0("\t\t\t");VOICE_TRACE(byType);

}
void ST_VOICE_RUT::AssertValid() const	
{   
}
#endif
//////////////////////////////////////////////////////////////////////////