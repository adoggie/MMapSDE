///////////////////////////////////////////////////////////////////////////////
//geo


#include "stdafx.h"
#include "Geo.h"

//////////////////////////////////////////////////////////////////////
//_tagBoundBox
_tagBoundBox::_tagBoundBox()
{
}

_tagBoundBox::_tagBoundBox(float fLeft, float fTop, float fRight, float fBottom)
{
    left    = fLeft;
    top     = fTop;
    right   = fRight;
    bottom  = fBottom;
}

_tagBoundBox::_tagBoundBox(double dLeft, double dTop, double dRight, double dBottom)
{
    left    = (float)dLeft;
    top     = (float)dTop;
    right   = (float)dRight;
    bottom  = (float)dBottom;
}

_tagBoundBox::_tagBoundBox(_tagBoundBox & box)
{
    _tagBoundBox(box.left, box.top, box.right, box.bottom);
}

_tagBoundBox _tagBoundBox::operator = (const _tagBoundBox & box)
{
    left    = box.left;
    top     = box.top;
    right   = box.right;
    bottom  = box.bottom;

    return (*this);
}

float _tagBoundBox::Height()
{
    return (float)fabs(top - bottom);
}

float _tagBoundBox::Width()
{
    return (float)fabs(right - left);
}




//////////////////////////////////////////////////////////////////////
//_tagLonLat
_tagLonLat::_tagLonLat(double longitude, double latitude)
{
    lon = longitude;
    lat = latitude;
};

_tagLonLat::_tagLonLat(const _tagLonLat & ll)
{
    lon = ll.lon;
    lat = ll.lat;
};

BOOL _tagLonLat::operator == (const _tagLonLat & ll) const
{
    return (lon == ll.lon && lat == ll.lat);
};

BOOL _tagLonLat::operator != (const _tagLonLat & ll) const
{
    return (lon != ll.lon || lat != ll.lat);
};

_tagLonLat _tagLonLat::operator = (const _tagLonLat & ll) 
{
    lon = ll.lon;
    lat = ll.lat;
    return *this;
};

_tagLonLat _tagLonLat::operator + (const _tagLonLat & ll) const
{
    _tagLonLat llRet;
    llRet.lon = lon + ll.lon;
    llRet.lat = lat + ll.lat;
    return llRet;
};

_tagLonLat _tagLonLat::operator - (const _tagLonLat & ll) const
{
    _tagLonLat llRet;
    llRet.lon = lon - ll.lon;
    llRet.lat = lat - ll.lat;
    return llRet;
};

_tagLonLat _tagLonLat::operator += (const _tagLonLat & ll) 
{
    this->lon += ll.lon;
    this->lat += ll.lat;
    return *this;
};

_tagLonLat _tagLonLat::operator -= (const _tagLonLat & ll) 
{
    this->lon -= ll.lon;
    this->lat -= ll.lat;
    return *this;
};

double _tagLonLat::operator * (const _tagLonLat & ll) const
{
    return lon * ll.lat - lat * ll.lon;
};

double _tagLonLat::CrossProduct(const _tagLonLat & ll)
{
    return *this * ll;
}

BOOL _tagLonLat::IsValid()
{
    return IsValidLL((*this));
};

static BOOL IsValid(const _tagLonLat & ll)
{
    return IsValidLL(ll);
}

//////////////////////////////////////////////////////////////////////
//_tagLLSegment

_tagLLSegment::_tagLLSegment()
{
}

_tagLLSegment::_tagLLSegment(LLPOINT & head, LLPOINT & tail)
{
    llHead = head;
    llTail = tail;
}

_tagLLSegment::_tagLLSegment(LLSEGMENT & llSegment)
{
    *this = llSegment;
}

BOOL _tagLLSegment::operator == (const _tagLLSegment &seg) const
{
    return (llHead == seg.llHead && llTail == seg.llTail);
};

_tagLLSegment _tagLLSegment::operator + (const _tagLLSegment &seg) const
{
    _tagLLSegment llRet;
    llRet.llHead = llHead + seg.llHead;
    llRet.llTail = llTail + seg.llTail;
    return llRet;
}

_tagLLSegment _tagLLSegment::operator - (const _tagLLSegment &seg) const
{
    _tagLLSegment llRet;
    llRet.llHead = llHead - seg.llHead;
    llRet.llTail = llTail - seg.llTail;
    return llRet;
}

_tagLLSegment _tagLLSegment::operator += (const _tagLLSegment &seg)
{
    this->llHead += seg.llHead;
    this->llTail += seg.llTail;
    return *this;
}

_tagLLSegment _tagLLSegment::operator -= (const _tagLLSegment &seg)
{
    this->llHead -= seg.llHead;
    this->llTail -= seg.llTail;
    return *this;
}

void _tagLLSegment::Reserve()
{
    Swap(llHead, llTail);
};

static void Reserve(_tagLLSegment & llSegment)
{
    Swap(llSegment.llHead, llSegment.llTail);
}

double _tagLLSegment::CrossProduct()	//*
{
    return llHead * llTail;
};

static double CrossProduct(_tagLLSegment & llSegment)
{
    return llSegment.llHead * llSegment.llTail;
}

//////////////////////////////////////////////////////////////////////
//_tagLLRect
_tagLLRect::_tagLLRect()
{
}

_tagLLRect::_tagLLRect(double Left, double Top, double Right, double Bottom)
{
    left = Left;
    top = Top;
    right = Right;
    bottom = Bottom;
};

_tagLLRect::_tagLLRect(const _tagLLRect &llRect)
{
    _tagLLRect(llRect.left, llRect.top, llRect.right, llRect.bottom);
}

_tagLLRect _tagLLRect::operator = (const _tagLLRect &llRect)
{
    left = llRect.left;
    top  = llRect.top;
    right = llRect.right;
    bottom = llRect.bottom;

    return *this;
};

BOOL _tagLLRect::operator == (const _tagLLRect &llRect) const
{
    return (left    == llRect.left &&
            top     == llRect.top  &&
            right   == llRect.right&&
            bottom  == llRect.bottom);
};

BOOL _tagLLRect::IsPointInRect(LONLAT & point)
{
    return (point.lon >= left && point.lon <= right &&
            point.lat >= bottom && point.lat <= top);
};

double _tagLLRect::Height()
{
    return fabs(top - bottom);
}

double _tagLLRect::Width()
{
    return fabs(right - left);
}


//////////////////////////////////////////////////////////////////////
//CEdge
CEdge::CEdge()
{
    m_lpHLinked = NULL; 
    m_lpTLinked = NULL;
    m_nPtsNum = 0;
    m_pllPts = NULL;
}

CEdge::~CEdge()
{
}


//////////////////////////////////////////////////////////////////////
//math&geo

int Power(int n, int nIndex)
{
    int nTemp = 1;
    for (int i = 0; i < nIndex; i++)
    {
        nTemp *= n;
    }
    return nTemp;
}

//近似算法
double GetDistance2(const LONLAT &point1, const LONLAT &point2)
{
	double Lon = cos( (point1.lat + point2.lat) / (double)2  * RADIANS) * 
				 (point1.lon - point2.lon) * DIS_PER_DEGREE;
	double Lat = (point1.lat - point2.lat) * DIS_PER_DEGREE;
	return sqrt(Lon * Lon + Lat * Lat);
}

//平面算法
double GetDistance1(const LONLAT &point1, const LONLAT &point2)
{
	double Lon = (point1.lon - point2.lon) * DIS_PER_DEGREE;
	double Lat = (point1.lat - point2.lat) * DIS_PER_DEGREE;
	return sqrt(Lon * Lon + Lat * Lat);
}

//大圆算法
inline double GetDistance(double lon1, double lat1, double lon2, double lat2)
{
	double theta = lon1 - lon2;
	double dist = sin(lat1 * RADIANS) * sin(lat2 * RADIANS) + 
		cos(lat1 * RADIANS) * cos(lat2 * RADIANS) * cos(theta * RADIANS);
	dist = acos(dist) * DEGREES;
	dist = dist * DIS_PER_DEGREE;
	return dist;
}

double GetDistance(const LONLAT &point1, const LONLAT &point2)
{
	return GetDistance(point1.lon, point1.lat, point2.lon, point2.lat);
}

double GetSegmentLen(const LLSEGMENT &segment)
{
    return GetDistance(segment.llHead, segment.llTail);
}

//
double GetLineLen(const LONLAT * pPoints, int nCount)
{
	double dLen = 0;
	int nSegCount = nCount - 1;
	for (int i = 0; i < nSegCount; i++)
	{
		dLen += GetDistance(pPoints[i], pPoints[i+1]);
	}
	return dLen;
}

void Swap(LONLAT &point1, LONLAT &point2)
{
	LONLAT llTmp = point1;
	point1 = point2;
	point2 = llTmp;
}

/*
*   function:   ReverseLine
*       reverse a line.
*   parameter:
*       pLine   --- [in/out];
*       nCount  --- [in];
*/
void ReverseLine(LONLAT * pLine, int nCount)
{
    if (nCount == 0 || pLine == NULL)
        return;

    LONLAT * pReverseLine = new LONLAT[nCount];
    for (int i = nCount - 1, j = 0; i >= 0; i--, j++)
    {
        pReverseLine[j] = pLine[i];
    }

    memcpy(pLine, pReverseLine, nCount);
    delete []pReverseLine;
}

BOOL IsPointInStraightLine(const LLPOINT &point, const LLPOINT &point1, const LLPOINT &point2)
{
	double dCrossProduct =  (point1.lat - point2.lat) * (point.lon - point2.lon) - 
							(point1.lon - point2.lon) * (point.lat - point2.lat);

	if(dCrossProduct == .0)
		return TRUE;
	
	return FALSE;
}

BOOL IsPointInSegment(const LLPOINT &point, const LLPOINT &head, const LLPOINT &tail)
{
	if( point.lon < min(head.lon, tail.lon) || 
		point.lon > max(head.lon, tail.lon) || 
		point.lat < min(head.lat, tail.lat) || 
		point.lat > max(head.lat, tail.lat) )
	{
		return FALSE;
	}

	return IsPointInStraightLine(point, head, tail);
}

BOOL IsPointInSegment(const LLPOINT &point, const LLSEGMENT & segment)
{
	return IsPointInSegment(point, segment.llHead, segment.llTail);
}

/*
*   return value:   distance
*
*/
double  GetPtDisFromSegment
        (const LONLAT &point,
         const LLSEGMENT &segment,
               LONLAT &intersection) 
{
    return GetPtDisFromSegment(point, segment.llHead, segment.llTail, intersection);
}

double GetPtDisFromSegment(const LLPOINT & point, 
                           const LLPOINT & point1, 
                           const LLPOINT & point2, 
                           LLPOINT & pointX)
{
    double dDis = GetDisPtFromStraightLine(point, point1, point2, pointX);
    if( IsPointInSegment(pointX, point1, point2) )
    {
        return dDis;
    }
    else 
    {
        double dDis1 = GetDistance(point1, point);
        double dDis2 = GetDistance(point2, point);
        if (dDis1 < dDis2)
        {
            pointX = point1;
            return dDis1;
        }
        else
        {
            pointX = point2;
            return dDis2;
        }
    }
}

double GetDisPtFromStraightLine(const LLPOINT & point, 
                                const LLPOINT & point1, 
                                const LLPOINT & point2, 
                                LLPOINT & pointX)
{
	if( point1.lon == point2.lon)
	{
		pointX.lon = point1.lon;
		pointX.lat = point.lat;
	}
	else if( point1.lat == point2.lat )
	{
		pointX.lat = point1.lat;
		pointX.lon = point.lon;
	}
    else
    {
        double k = ( point2.lat - point1.lat ) / (point2.lon - point1.lon );
	    pointX.lon = ( k * k * point1.lon + k * (point.lat - point1.lat) + point.lon ) / (k * k + 1.0);
	    pointX.lat = k * (pointX.lon - point1.lon) + point1.lat;
    }

	return GetDistance(point, pointX);
}

double GetDisPtFromLine(const LLPOINT & point, 
                        const LLPOINT * pLine, 
                        int nPtsCount, 
                        LLPOINT & pointX)
{
    if (nPtsCount == 0 || pLine == NULL)
    {
        return -1.0;
    }

    int nIdxMinDis;
    double dDis;
    double dMinDis = 1000000000000.0;

    int nSegmentCount = nPtsCount -1;
    for (int i = 0; i < nSegmentCount; i++)
    {
        dDis = GetPtDisFromSegment(point, pLine[i], pLine[i + 1], pointX);
        if (dDis < dMinDis)
        {
            dMinDis = dDis;
            nIdxMinDis = i;
        }
    }

    return GetPtDisFromSegment(point, pLine[nIdxMinDis], pLine[nIdxMinDis + 1], pointX);
}

inline BOOL LineAndLine_I(double x1, double y1,
                          double x2, double y2,
                          double x3, double y3,
                          double x4, double y4,
                          double *x, double *y)
{
	double K1,K2;
	double Y,X;
	double maxy1 = max(y1,y2);
	double maxy2 = max(y3,y4);
	double miny1 = min(y1,y2);
	double miny2 = min(y3,y4);
	if(x1==x2)//线1为垂直线
	{
		if(x3==x4)//线2也为垂直线
		{
			if(x3==x1)
			{
				if(maxy1>maxy2)//线1在线2之下
				{
					if(between_but_self(maxy2,miny1,maxy1))
					{ 
						*x=x1;
						*y=maxy2;			
						return true;
					}				
				}
				else
					if(between_but_self(maxy1,miny2,maxy2))
					{
						*x=x1;
						*y=maxy1;
						return true;
					}
			}
			return false;
		}
		//线2不为垂直线时
		K2=(y3-y4)/(x3-x4);
		Y=K2*(x1-x3)+y3;
		if(between_but_self(Y,miny1,maxy1)&&between_but_self(Y,miny2,maxy2))
		{
			*x=x1;
			*y=(double)Y;
			return true;
		}
		return false;
	}
	if(x3==x4)//线2为垂直线
	{
		K1=(y1-y2)/(x1-x2);
		Y=K1*(x3-x1)+y1;
		if(between_but_self(Y,miny1,maxy1)&&between_but_self(Y,miny2,maxy2))
		{
			*x=x3;
			*y=(double)Y;
			return true;
		}
		return false;
	}
	//皆非垂直线
	K1=(y1-y2)/(x1-x2);
	K2=(y3-y4)/(x3-x4);
	if(K1==K2)
		return false; 
	X=(K1*x1-y1-K2*x3+y3)/(K1-K2);
	Y=K1*(X-x1)+y1;
	if(between_but_self(Y,miny1,maxy1)&&between_but_self(Y,miny2,maxy2))
	{
		if(y)
		{
			*x=X;
			*y=Y;
		}
		return true;
	}
	return false;
}

BOOL IsIntersectWith(const LLPOINT &point1, 
                     const LLPOINT &point2, 
                     const LLPOINT &point3, 
                     const LLPOINT &point4, 
                     LLPOINT & pointX)
{
	return LineAndLine_I(point1.lon, point1.lat, 
                         point2.lon, point2.lat, 
                         point3.lon, point3.lat, 
                         point4.lon, point4.lat, 
                         &(pointX.lon), &(pointX.lat));
}

BOOL IsIntersectWith(const LLSEGMENT segment1, 
                     const LLSEGMENT segment2,
                     LLPOINT & pointX)
{
    return IsIntersectWith(segment1.llHead, segment1.llTail,
                           segment2.llHead, segment2.llTail, 
                           pointX);
}

/*
* function: IsPtInPolygon
*    ascertain that a point in Polygon or not.
* parameter: 
*	 llPt 		--- coordinate of point;
*	 plls  		--- climax of Polygon;
*	 nPtsCount 	--- count of climax
* return value:
*  -1 --- parameter error.
*   0 --- out of the Polygon;
*   1 --- in;
*   2 --- is climax;
*   3 --- in edge.
*/
int IsPointInPolygon(const LONLAT &llPt, const LONLAT * pPolygon, int nPtsCount)
{
    if (nPtsCount == 0 || pPolygon == NULL)
        return -1;

    int nStartP, nEndP;
    LONLAT llPtX;
    llPtX.lat = llPt.lat;
    
    int nCrossCount = 0;
    
    for (int i = 0; i < nPtsCount; i++)
    {
        nStartP = i;
        if(i + 1 == nPtsCount)
            nEndP = 0;
        else
            nEndP = i + 1;

        if(llPt.lon > max(pPolygon[nStartP].lon, pPolygon[nEndP].lon))		//right
			continue ;															
        else if(llPt.lat > max(pPolygon[nStartP].lat, pPolygon[nEndP].lat))	//on top of
            continue ;
        else if(llPt.lat < min(pPolygon[nStartP].lat, pPolygon[nEndP].lat))	//under
            continue ;
        else if((llPt.lat == pPolygon[nStartP].lat && llPt.lon == pPolygon[nStartP].lon) || 
                (llPt.lat == pPolygon[nEndP].lat && llPt.lon == pPolygon[nEndP].lon))
            return 2;														//climax
        else if(llPt.lat == pPolygon[nStartP].lat && llPt.lat == pPolygon[nEndP].lat)//parallel
        {
//            if(llPt.lon > max(pPolygon[nStartP].lon, pPolygon[nEndP].lon))
//                continue ;												//right
            //else 
			if(llPt.lon < max(pPolygon[nStartP].lon, pPolygon[nEndP].lon) && 
               llPt.lon > min(pPolygon[nStartP].lon, pPolygon[nEndP].lon))
                return 3;													//on edge
            else	//llPt.lon < min(pPolygon[i].lon, pPolygon[i + 1].lon)
            {
                if(i - 1 < 0)
                    nStartP = nPtsCount - 1;
                else
                    nStartP = i - 1;
                if(i + 2 >= nPtsCount)
                    nEndP = i + 2 - nPtsCount;
                else
                    nEndP = i + 2;
                i++;
                if(llPt.lat > max(pPolygon[nStartP].lat, pPolygon[nEndP].lat) ||
                   llPt.lat < min(pPolygon[nStartP].lat, pPolygon[nEndP].lat))
                {
                    nCrossCount ++ ;
                    continue ;
                }
                else 
                    continue ;
            }
        }
        else if(llPt.lat == pPolygon[nStartP].lat)//but llPt.lat != pPolygon[nEndP].lat
        {
            if(llPt.lon < pPolygon[nStartP].lon)
                nCrossCount ++ ;				
            else
                continue ;
        }
        else if(llPt.lat == pPolygon[nEndP].lat)//but llPt.lat != pPolygon[nStartP].lat
        {
			if(llPt.lon > pPolygon[nEndP].lon)
				continue ;
            nStartP = i;
            if(i + 2 >= nPtsCount)
                nEndP = i + 2 - nPtsCount;
            else
                nEndP = i + 2;

            if(llPt.lat > max(pPolygon[nStartP].lat, pPolygon[nEndP].lat) ||
               llPt.lat < min(pPolygon[nStartP].lat, pPolygon[nEndP].lat))
            {
                nCrossCount ++ ;/////////////////////////
                continue ;
            }
            else 
                continue ;
        }
        else //if	(llPt.lat < max(pPolygon[nStartP].lat, pPolygon[nEndP].lat) &&
            //    (llPt.lat > min(pPolygon[nStartP].lat, pPolygon[nEndP].lat)) &&
			//	   (llPt.lon <= max(pPolygon[nStartP].lon, pPolygon[nEndP].lon))
        {
			if(llPt.lon <= min(pPolygon[nStartP].lon, pPolygon[nEndP].lon))
            {
                nCrossCount ++ ;
                continue ;
            }

            // get cross-point.  formula: x0 = x1 + (x2 - x1) * (y0 - y1) / (y2 - y1)
            llPtX.lon = pPolygon[nStartP].lon + 
                        (pPolygon[nEndP].lon - 
                        pPolygon[nStartP].lon) * 
                        (llPt.lat - pPolygon[nStartP].lat) / 
                        (pPolygon[nEndP].lat - pPolygon[nStartP].lat);
            if(llPtX.lon == llPt.lon)			//edge
                return 3;
            else if(llPtX.lon > llPt.lon)		//right
            {
                nCrossCount ++ ;
                continue ;
            }
            else // llPtX.lon < llPt.lon		//left
                continue ;
        }
    }	//	end for (int i = 0; i < nPtsCount; i++)
    
    return nCrossCount % 2;			//0 or 1
}

/*
*   return value:
*       0   --- out;
*       1   --- in;
*       2   --- in edge
*/
int IsPointInRect(const LLPOINT & point, const LLRECT &llRect)
{
    if ( (point.lon > llRect.left && point.lon < llRect.right) && 
         (point.lat < llRect.top && point.lat > llRect.bottom) )
    {
        return 1;
    }
    else if (point.lon == llRect.left || point.lon == llRect.right || 
             point.lat == llRect.top || point.lat == llRect.bottom)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

int IsPointInRect(const FLOAT_LONLAT & point, const FLOAT_LLRECT &llRect)
{
    if ( (point.lon > llRect.left && point.lon < llRect.right) && 
         (point.lat < llRect.top && point.lat > llRect.bottom) )
    {
        return 1;
    }
    else if (point.lon == llRect.left || point.lon == llRect.right || 
             point.lat == llRect.top || point.lat == llRect.bottom)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

BOOL IsIntersectWithRect(const FLOAT_LLRECT &llRect1, const FLOAT_LLRECT &llRect2)
{
	//this code is the same as the follow code that is commented
	return ( ( between_but_self(llRect1.bottom, llRect2.bottom, llRect2.top) && between_but_self(llRect1.left, llRect2.left, llRect2.right) ) ||
			 ( between_but_self(llRect1.bottom, llRect2.bottom, llRect2.top) && between_but_self(llRect1.right, llRect2.left, llRect2.right) ) ||
			 ( between_but_self(llRect1.top, llRect2.bottom, llRect2.top) && between_but_self(llRect1.left, llRect2.left, llRect2.right) ) ||
			 ( between_but_self(llRect1.top, llRect2.bottom, llRect2.top) && between_but_self(llRect1.right, llRect2.left, llRect2.right) ) || 
			 ( between_but_self(llRect2.bottom, llRect1.bottom, llRect1.top) && between_but_self(llRect2.left, llRect1.left, llRect1.right) ) ||
			 ( between_but_self(llRect2.bottom, llRect1.bottom, llRect1.top) && between_but_self(llRect2.right, llRect1.left, llRect1.right) ) ||
			 ( between_but_self(llRect2.top, llRect1.bottom, llRect1.top) && between_but_self(llRect2.left, llRect1.left, llRect1.right) ) ||
			 ( between_but_self(llRect2.top, llRect1.bottom, llRect1.top) && between_but_self(llRect2.right, llRect1.left, llRect1.right) ) );
}

BOOL IsIntersectWithRect(const LLRECT &llRect1, const LLRECT &llRect2)
{
	//this code is the same as the follow code that is commented
	return ( ( between_but_self(llRect1.bottom, llRect2.bottom, llRect2.top) && between_but_self(llRect1.left, llRect2.left, llRect2.right) ) ||
			 ( between_but_self(llRect1.bottom, llRect2.bottom, llRect2.top) && between_but_self(llRect1.right, llRect2.left, llRect2.right) ) ||
			 ( between_but_self(llRect1.top, llRect2.bottom, llRect2.top) && between_but_self(llRect1.left, llRect2.left, llRect2.right) ) ||
			 ( between_but_self(llRect1.top, llRect2.bottom, llRect2.top) && between_but_self(llRect1.right, llRect2.left, llRect2.right) ) || 
			 ( between_but_self(llRect2.bottom, llRect1.bottom, llRect1.top) && between_but_self(llRect2.left, llRect1.left, llRect1.right) ) ||
			 ( between_but_self(llRect2.bottom, llRect1.bottom, llRect1.top) && between_but_self(llRect2.right, llRect1.left, llRect1.right) ) ||
			 ( between_but_self(llRect2.top, llRect1.bottom, llRect1.top) && between_but_self(llRect2.left, llRect1.left, llRect1.right) ) ||
			 ( between_but_self(llRect2.top, llRect1.bottom, llRect1.top) && between_but_self(llRect2.right, llRect1.left, llRect1.right) ) );

/*
//////////////////////////////////////////////////////////////////////////
	BOOL bRect1Bottom= between_but_self(llRect1.bottom, llRect2.bottom, llRect2.top);
	BOOL bRect1Top	 = between_but_self(llRect1.top, llRect2.bottom, llRect2.top);
	BOOL bRect1Left	 = between_but_self(llRect1.left, llRect2.left, llRect2.right);
	BOOL bRect1Right = between_but_self(llRect1.right, llRect2.left, llRect2.right);
	BOOL bRect2Bottom= between_but_self(llRect2.bottom, llRect1.bottom, llRect1.top);
	BOOL bRect2Top	 = between_but_self(llRect2.top, llRect1.bottom, llRect1.top);
	BOOL bRect2Left	 = between_but_self(llRect2.left, llRect1.left, llRect1.right);
	BOOL bRect2Right = between_but_self(llRect2.right, llRect1.left, llRect1.right);
	return ( ( bRect1Bottom && bRect1Left ) ||
			 ( bRect1Bottom && bRect1Right ) ||
			 ( bRect1Top && bRect1Left ) ||
			 ( bRect1Top && bRect1Right ) || 
			 ( bRect2Bottom && bRect2Left ) ||
			 ( bRect2Bottom && bRect2Right ) ||
			 ( bRect2Top && bRect2Left ) ||
			 ( bRect2Top && bRect2Right ) );
*/
	
}

/*
*   return value:
*       -1  --- para error;
*       0   --- at least one points of the line out of the rect;
*       1   --- in.
*/
int IsLineInRect(LLPOINT * pLine, int nPtsCount, LLRECT &llRect)
{
    return IsFeatureInRect(pLine, nPtsCount, llRect);
}

/*
*   return value:
*       -1  --- para error;
*       0   --- at least one points of the Feature out of the rect;
*       1   --- in.
*/
int IsFeatureInRect(const LLPOINT * pFeature, int nPtsCount, const LLRECT &llRect)
{

    if (nPtsCount == 0 || pFeature == NULL)
    {
        return - 1;
    }

    for (int i = 0; i < nPtsCount; i++)
    {
        if (IsPointInRect(pFeature[i], llRect) == 0)
        {
            return 0;
        }
    }

    return 1;
}

int IsFeatureInRect(const FLOAT_LONLAT * pFeature, int nPtsCount, const FLOAT_LLRECT &llRect)
{

    if (nPtsCount == 0 || pFeature == NULL)
    {
        return - 1;
    }

    for (int i = 0; i < nPtsCount; i++)
    {
        if (IsPointInRect(pFeature[i], llRect) == 0)
        {
            return 0;
        }
    }

    return 1;
}

/*
*   return value:
*       -1  --- para error;
*       0   --- at least one points of the Polygon out of the rect;
*       1   --- in.
*/
int IsPolygonInRect(const LLPOINT * pPolygon, int nPtsCount, const LLRECT &llRect)
{
    return IsFeatureInRect(pPolygon, nPtsCount, llRect);
}


void GetMBR(const LONLAT * pPts, int nPtsCount, LLRECT &llMBR)
{
    if (nPtsCount == 0 || pPts == NULL)
        return;

    llMBR.left = 181.0;
    llMBR.right = -181.0;
    llMBR.bottom = 91.0;
    llMBR.top = -91.0;

    for (int i = 0; i < nPtsCount; i++)
    {
        if (pPts[i].lon > llMBR.right)  llMBR.right = pPts[i].lon;
        if (pPts[i].lon < llMBR.left)   llMBR.left  = pPts[i].lon;
        if (pPts[i].lat > llMBR.top)    llMBR.top   = pPts[i].lat;
        if (pPts[i].lat < llMBR.bottom) llMBR.bottom= pPts[i].lat;
    }
}

void GetMBR(const FLOAT_LONLAT * pPts, int nPtsCount, FLOAT_LLRECT &llMBR)
{
    if (nPtsCount == 0 || pPts == NULL)
        return;

    llMBR.left = 181.0;
    llMBR.right = -181.0;
    llMBR.bottom = 91.0;
    llMBR.top = -91.0;

    for (int i = 0; i < nPtsCount; i++)
    {
        if (pPts[i].lon > llMBR.right)  llMBR.right = pPts[i].lon;
        if (pPts[i].lon < llMBR.left)   llMBR.left  = pPts[i].lon;
        if (pPts[i].lat > llMBR.top)    llMBR.top   = pPts[i].lat;
        if (pPts[i].lat < llMBR.bottom) llMBR.bottom= pPts[i].lat;
    }
}


//////////////////////////////////////////////////////////////////////
//未实现
void DenseLine(const LONLAT * pLine, int nPtsCount, double dSpace, 
               LONLAT * pNewLine, int &nNewPtsCount)
{
    
    return ;
}

//////////////////////////////////////////////////////////////////////
//angle
int GetAngleDif(int nAngle1, int nAngle2)
{
	if (nAngle1 < 0) 
		nAngle1 = 360 - nAngle1;
	if (nAngle2 < 0) 
		nAngle2 = 360 - nAngle2;

	nAngle1 = nAngle1 % 360;
	nAngle2 = nAngle2 % 360;

	int nAngleMin = min(nAngle1, nAngle2);
	int nAngleMax = max(nAngle1, nAngle2);

	return min(nAngleMax - nAngleMin, nAngleMin + 360 - nAngleMax);
}

float GetAngle(float lon1, float lat1, float lon2, float lat2)
{
	LONLAT point1, point2;
	point1 = LONLAT(lon1, lat1);
	point2 = LONLAT(lon2, lat2);
	return (float)GetAngle(point1, point2);
}

//return value:radian
double GetAngle(const LONLAT &point1, const LONLAT &point2)
{
    LONLAT pointAngle = point2 - point1;
    double dAngle;
    if (pointAngle.lon == .0)
    {
        dAngle = pointAngle.lat > .0 ? (PI / 2) : 0 - (PI / 2);
    }
    else
    {
        dAngle = atan(pointAngle.lat / pointAngle.lon);
        
        if (pointAngle.lon < .0)
        {
            dAngle = PI + dAngle;
        }
    }

	dAngle = PI / 2 - dAngle;
	if (dAngle < 0)
		dAngle = 2 * PI + dAngle;

    return dAngle ;	//2006-6-23
}

double GetAngle(const LONLAT &point1, const LONLAT &point2, const LONLAT &point3, const LONLAT &point4)
{
    double dAngle1 = GetAngle(point1, point2) * DEGREES;
    double dAngle2 = GetAngle(point3, point4) * DEGREES;
    double dAngleDif = GetAngleDif((int)dAngle1, (int)dAngle2);
    
    return dAngleDif * RADIANS;
}

double GetAngle(const LLSEGMENT &segment1, const LLSEGMENT &segment2)
{
    return GetAngle(segment1.llHead, segment1.llTail, segment2.llHead, segment2.llTail);
}

//////////////////////////////////////////////////////////////////////
//clip
int Inside(LONLAT& pt, Edge b, LONLAT& wMin, LONLAT& wMax)
{
	switch(b)
	{
	case Left:
		if(pt.lon < wMin.lon) return (FALSE);
		break;
	case Right:
		if(pt.lon > wMax.lon) return (FALSE);
		break;
	case Bottom:
		if(pt.lat < wMin.lat) return (FALSE);
		break;
	case Top:
		if(pt.lat > wMax.lat) return (FALSE);
		break;
	}

	return TRUE;
}

int Cross(LONLAT& pt1,LONLAT& pt2,Edge b,LONLAT& wMin,LONLAT& wMax)
{
	if(Inside(pt1,b,wMin,wMax) == Inside(pt2,b,wMin,wMax))
		return (FALSE);
	else
		return (TRUE);
}

LONLAT Intersect(LONLAT& pt1,LONLAT& pt2,Edge b,LONLAT& wMin,LONLAT& wMax)
{
	LONLAT ptTemp;
	float Temp;
	if(pt1.lon != pt2.lon ) 
		Temp = (float)(pt1.lat - pt2.lat) / (float)(pt1.lon - pt2.lon);
	
	switch(b)
	{
	case Left:
		ptTemp.lon = wMin.lon;
		ptTemp.lat = (int)(pt2.lat + (wMin.lon - pt2.lon ) * Temp);
		break;
	case Right:
		ptTemp.lon = wMax.lon;
		ptTemp.lat = (int)(pt2.lat + (wMax.lon - pt2.lon ) * Temp); 
		break;
	case Bottom:
		ptTemp.lat = wMin.lat;
		if(pt1.lon != pt2.lon)
			ptTemp.lon = (int)(pt2.lon + (wMin.lat - pt2.lat ) / Temp);
		else
			ptTemp.lon = pt2.lon;
		break;
	case Top:
		ptTemp.lat = wMax.lat;
		if(pt1.lon != pt2.lon) 
			ptTemp.lon = (int)(pt2.lon + (wMax.lat - pt2.lat ) / Temp);
		else
			ptTemp.lon = pt2.lon;
		break;	
	}

	return (ptTemp);
}

/*
*	pIn	--- feature which will be clipped;
*	pOut--- new feature after clipped
*/
int ClipPolygon(LONLAT wMin,LONLAT wMax,int n,LONLAT *pIn,LONLAT *pOut)
{
	LONLAT Head,Tail;
	LONLAT *pTemp = new LONLAT[n * 2], *pMid = pIn;

	int out;
	for (int i = 0; i < 4; i++)
	{
		out = 0;
		for(int cnt = 0;cnt < n;cnt++)
		{
			Head = pMid[cnt];
			if (cnt != (n - 1))
				Tail = pMid[cnt+1];
			else
				Tail = pMid[0];
		
			BOOL bh = Inside(Head, (Edge)i, wMin, wMax);
			BOOL bt = Inside(Tail, (Edge)i, wMin, wMax);
			if (bh == bt && bh)
				pTemp[out++] = Tail;
			else if (bh != bt)
			{
				pTemp[out++] = Intersect(Head, Tail, (Edge)i, wMin, wMax);
				if (bt)
					pTemp[out++] = Tail;
			}
		}
		if (i == 0)
			pMid = pOut;

		LONLAT* p = pMid;
		pMid = pTemp;
		pTemp = p;

		n = out;
	}

	delete [] pTemp;
	return (out);
}




//////////////////////////////////////////////////////////////////////

/*
*				90
*				^
*				|
*				|
*				|
*	180	--------+-------->0
*				|
*				|
*				|
*	  225 	   -90		-45
*/
void test()
{

	LONLAT ll1, ll2;
	ll1.lon = 0;
	ll1.lat = 0;
	ll2.lon = 1;
	ll2.lat = -1;

	
//	GetAngle(ll1, ll2);
	int nDegree1 = (int)radian2degree(GetAngle(LONLAT(0,0), LONLAT(1,0)));
	int nDegree2 = (int)radian2degree(GetAngle(LONLAT(0,0), LONLAT(0,1)));
	int nDegree3 = (int)radian2degree(GetAngle(LONLAT(0,0), LONLAT(-1,0)));
	int nDegree4 = (int)radian2degree(GetAngle(LONLAT(0,0), LONLAT(0,-1)));
	int nDegree11 = (int)radian2degree(GetAngle(LONLAT(0,0), LONLAT(1,1)));
	int nDegree21 = (int)radian2degree(GetAngle(LONLAT(0,0), LONLAT(-1,1)));
	int nDegree31 = (int)radian2degree(GetAngle(LONLAT(0,0), LONLAT(-1,-1)));
	int nDegree41 = (int)radian2degree(GetAngle(LONLAT(0,0), LONLAT(1,-1)));


}

