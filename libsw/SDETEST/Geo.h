/********************************************************************
	created:	2006/05/11
	created:	11:5:2006   11:22
	filename: 	F:\project\DataTransfer\MapDataCompile\Geo.h
	file path:	F:\project\DataTransfer\MapDataCompile
	file base:	Geo
	file ext:	h
	author:		HomerLu
	
	purpose:	对二维（三维）世界的描述
	coordsys:	WGS84
	definition:		
				point		---	二维（三维）世界的一个坐标对；
				segment		---	包含两个point的有向线段
				straightline--- 两点确定的一条直线
				line		--- 多条线段连成的折线
				rect		---	摆放端正的矩形框，
				polygon		---	多边形
				MBR			---	最小包围矩形(rect)，
								minimal boundary rectangle
*********************************************************************/

#ifndef _GEO_H_
#define _GEO_H_

#include "math.h"


//////////////////////////////////////////////////////////////////////
//math

#ifndef PI
#define PI                  3.14159265358979323846
#endif

#define DOUBLE_PI              (PI * 2)

#ifndef DEGREES
#define DEGREES             (180 / PI)
#endif

#ifndef RADIANS
#define RADIANS             (PI / 180)
#endif

#ifndef EARTH_PERIMETER
//length unit:meters
#define EARTH_PERIMETER		( 40000 * 1000 )
#endif

#ifndef EARTH_RADIUS
#define EARTH_RADIUS		( EARTH_PERIMETER / 2 / PI)
#endif

#ifndef DIS_PER_DEGREE
//#define DIS_PER_DEGREE		(1852 * 60)
#define DIS_PER_DEGREE		(EARTH_PERIMETER / 360)
#endif

#define radian2degree(radian)	(radian * DEGREES)
#define degree2radian(degree)	(degree * RADIANS)

#ifndef square
#define square(x)           (x) * (x)
#endif 

#ifndef between
#define between(val,val1,val2) ((val)>=(val1)&&(val)<=(val2))
#endif

#ifndef between_but_self
#define between_but_self(val,val1,val2) ((val)>(val1)&&(val)<(val2))
#endif

#define get_module(val1, val2) ((val1) - (val2) * ((int)((val1) / (val2))));

int Power(int n, int nIndex);

//end math
//////////////////////////////////////////////////////////////////////

#define MAX_LON				180.0
#define MIN_LON				-180.0
#define MAX_LAT				90.0
#define MIN_LAT				-90.0
#define INVALID_LON          MAX_LON
#define INVALID_LAT          MAX_LAT
#define INVALID_LONLAT       {INVALID_LON, INVALID_LAT}
#define INVALID_RECT		 {MAX_LON, MIN_LAT, MIN_LON, MAX_LAT}

#ifndef IsValidLon
#define IsValidLon(lon) ((lon) > MIN_LON && (lon) < MAX_LON)
#endif

#ifndef IsValidLat
#define IsValidLat(lat) ((lat) > MIN_LAT && (lat) < MAX_LAT)
#endif

#ifndef IsValidLL
#define IsValidLL(ll)       ( IsValidLon( (ll).lon ) && IsValidLat( (ll).lat ) )
#endif

//////////////////////////////////////////////////////////////////////
//clip
typedef enum 
{
	Left = 0,
	Right,
	Bottom,
	Top
} Edge;

#define N_EDGE 4


//////////////////////////////////////////////////////////////////////
//_tagFloatLonLat
typedef struct _tagFloatLonLat
{
	float lon;
	float lat;
}FLOAT_LONLAT,*LPFLOAT_LONLAT;

//////////////////////////////////////////////////////////////////////
//_tagBoundBox
typedef struct _tagBoundBox
{
    float   left;
    float   top;
    float   right;
    float   bottom;

    _tagBoundBox();
    _tagBoundBox(float fLeft, float fTop, float fRight, float fBottom);
    _tagBoundBox(double dLeft, double dTop, double dRight, double dBottom);
    _tagBoundBox(_tagBoundBox & box);
    _tagBoundBox operator = (const _tagBoundBox & box);

	float Height();
	float Width();

}BOUNDBOX,*LPBOUNDBOX,FLOAT_LLRECT,*LPFLOAT_LLRECT;



//////////////////////////////////////////////////////////////////////
//_tagLonLat
#ifndef _LON_LAT_
#define _LON_LAT_
typedef struct _tagLonLat
{
	double lon;	//in degree
	double lat;	//in degree

    _tagLonLat(double longitude = .0, double latitude = .0);

    _tagLonLat(const _tagLonLat & ll);
    
    BOOL operator == (const _tagLonLat & ll) const;
    
    BOOL operator != (const _tagLonLat & ll) const;
    
    _tagLonLat operator = (const _tagLonLat & ll);
    
    _tagLonLat operator + (const _tagLonLat & ll) const;
    
    _tagLonLat operator - (const _tagLonLat & ll) const;
	
    _tagLonLat operator += (const _tagLonLat & ll);
    
    _tagLonLat operator -= (const _tagLonLat & ll);

    double operator * (const _tagLonLat & ll) const; //cross product

    double CrossProduct(const _tagLonLat & ll);
    
    BOOL IsValid();
    static BOOL IsValid(const _tagLonLat & ll);
    
}LONLAT,*LPLONLAT,LLPOINT,*LPLLPOINT;
#endif  //_LON_LAT_

//////////////////////////////////////////////////////////////////////
//_tagLonLatAlt
#ifndef _LON_LAT_ALT
#define _LON_LAT_ALT
typedef struct _tagLonLatAlt : public _tagLonLat
{
    double alt;
}LONLATALT,*LPLONLATALT;
#endif  //_LON_LAT_ALT

//////////////////////////////////////////////////////////////////////
//_tagLLSegment
#ifndef _LL_SEGMENT_
#define _LL_SEGMENT_
typedef struct _tagLLSegment
{
    LLPOINT llHead;
    LLPOINT llTail;

    _tagLLSegment();
    _tagLLSegment(LLPOINT & head, LLPOINT & llTail);
    _tagLLSegment(_tagLLSegment & llSegment);
	
	BOOL operator == (const _tagLLSegment &seg) const; 

    _tagLLSegment operator + (const _tagLLSegment &seg) const;
    
    _tagLLSegment operator - (const _tagLLSegment &seg) const;

    _tagLLSegment operator += (const _tagLLSegment &seg);
    
    _tagLLSegment operator -= (const _tagLLSegment &seg);

    void Reserve();
    static void Reserve(_tagLLSegment & llSegment);

    double CrossProduct();	//*
    static double CrossProduct(_tagLLSegment & llSegment);	//*

}LLSEGMENT,*LPLLSEGMENT;
#endif  //_LL_SEGMENT_

//////////////////////////////////////////////////////////////////////
//_tagLLRect
#ifndef _LL_RECT_
#define _LL_RECT_
typedef struct _tagLLRect
{
    double left;
    double top;
    double right;
    double bottom;
    
    _tagLLRect();

    _tagLLRect(double Left, double Top, double Right, double Bottom);

    _tagLLRect(const _tagLLRect &llRect);

    _tagLLRect operator = (const _tagLLRect &llRect);

    BOOL operator == (const _tagLLRect &llRect) const;

    BOOL IsPointInRect(LONLAT & point);

    double Height();

    double Width();

}LLRECT,*LPLLRECT;
#endif  //_LL_RECT_

//////////////////////////////////////////////////////////////////////
//CEdge

class CEdge
{
public:
	CEdge();
	virtual ~CEdge();
    
public:
	long		m_nID;
	long		m_nHeadNodeID;
	long		m_nTailNodeID;
	CEdge *		m_lpHLinked;
	CEdge *		m_lpTLinked;
	long		m_nPtsNum;
	LONLAT *	m_pllPts;
};



//////////////////////////////////////////////////////////////////////
//function

double GetDistance(const LONLAT &point1, const LONLAT &point2);
double GetSegmentLen(const LLSEGMENT &segment);
double GetLineLen(const LONLAT * pPoints, int nCount);

void Swap(LONLAT &ll1, LONLAT &ll2);
void ReverseLine(LONLAT * pLine, int nCount);

double GetDisPtFromStraightLine(const LLPOINT & point, 
                                const LLPOINT & point1, 
                                const LLPOINT & point2, 
                                LLPOINT & pointX);

double GetDisPtFromLine(const LLPOINT & point, const LLPOINT * pLine, int nPtsCount, LLPOINT & pointX);

inline double  GetPtDisFromSegment
        (const LONLAT &point,
         const LONLAT &point1,
         const LONLAT &point2,
               LONLAT &intersection);

inline double  GetPtDisFromSegment
        (const LONLAT &point,
         const LLSEGMENT &segment,
               LONLAT &intersection);

inline BOOL IsPointInStraightLine(const LLPOINT &point, 
                                  const LLPOINT &point1, 
                                  const LLPOINT &point2);
inline BOOL IsPointInSegment(const LLPOINT &point, 
                             const LLPOINT &head, 
                             const LLPOINT &tail);
inline BOOL IsPointInSegment(const LLPOINT &point, 
                             const LLSEGMENT &segment);


BOOL IsIntersectWith(const LLSEGMENT segment1, 
                     const LLSEGMENT segment2,
                     LLPOINT & pointX);


BOOL IsIntersectWith(const LLPOINT &point1, 
                     const LLPOINT &point2, 
                     const LLPOINT &point3, 
                     const LLPOINT &point4, 
                     LLPOINT &pointX);

void GetMBR(const LONLAT * pPts, int nPtsCount, LLRECT &llMBR);
void GetMBR(const FLOAT_LONLAT * pPts, int nPtsCount, FLOAT_LLRECT &llMBR);

int IsPointInRect(const LLPOINT & point, const LLRECT &llRect);
int IsPointInRect(const FLOAT_LONLAT & point, const FLOAT_LLRECT &llRect);
int IsLineInRect(const LLPOINT * pLine, int nPtsCount, const LLRECT &llRect);
int IsPolygonInRect(const LLPOINT * pPolygon, int nPtsCount, const LLRECT &llRect);
int IsFeatureInRect(const LLPOINT * pFeature, int nPtsCount, const LLRECT &llRect);
int IsFeatureInRect(const FLOAT_LONLAT * pFeature, int nPtsCount, const FLOAT_LLRECT &llRect);
BOOL IsIntersectWithRect(const LLRECT &llRect1, const LLRECT &llRect2);
BOOL IsIntersectWithRect(const FLOAT_LLRECT &llRect1, const FLOAT_LLRECT &llRect2);

/*
* function: IsPtInPolygon
*    ascertain that a point in Polygon or not.
* parameter: 
*	 llPt[in] 		--- coordinate of point;
*	 plls[in]  		--- climax of Polygon;
*	 nPtsCount[in] 	--- count of climax.
* return value:
*  -1 --- parameter error;
*   0 --- out of the Polygon;
*   1 --- in;
*   2 --- is climax;
*   3 --- in edge.
*/
int IsPointInPolygon(const LONLAT &llPt, const LONLAT * pPolygon, int nPtsCount);



//////////////////////////////////////////////////////////////////////
float GetAngle(float lon1, float lat1, float lon2, float lat2);

//return value:radian
double GetAngle(const LONLAT &point1, const LONLAT &point2);

//return value:radian
double GetAngle(const LLSEGMENT &segment1, 
                const LLSEGMENT &segment2);

//return value:radian
double GetAngle(const LONLAT &point1, 
                const LONLAT &point2, 
                const LONLAT &point3, 
                const LONLAT &point4);


//////////////////////////////////////////////////////////////////////
//clip
int ClipPolygon(LONLAT wMin,LONLAT wMax,int n,LONLAT *pIn,LONLAT *pOut);



//////////////////////////////////////////////////////////////////////
//test
void test();


#endif  //_GEO_H_
