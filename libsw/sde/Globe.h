
///////////////////////////////////////////////////////////////////////////////
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "swBase.h"
#include "GIS_FeatureLine.h"
#include "GIS_FeaturePoint.h"
#include "GIS_FeaturePoly.h"
#include "GIS_FeatureRut.h"
#include <math.h>

#define	_a84				6378137.0
#define	_e842				0.00669437999013
#define _e84				(sqrt(_e842))
#define	_a54				6378245.0
#define	_e542				0.006693421622965949
#define	_e54				(sqrt(_e542))
#define	_a80				6378140.0
#define	_e802				0.006694384999587952
#define	_e80				(sqrt(_e802))
#define	Minimum				10e-10
#define	ZERO				1e-6
#define	LOOP_NUMBER			5

const long BUFFER_SIZE_04	= 4;
const long BUFFER_SIZE_08	= 8;
const long BUFFER_SIZE_16	= 16;
const long BUFFER_SIZE_32	= 32;
const long BUFFER_SIZE_64	= 64;
const long BUFFER_SIZE_128	= 128;
const long BUFFER_SIZE_KILO	= 1024;
const long SAT_ID_NUMBER	= 12;						// 定义卫星ID数目
const double PI				= 3.14159265358979;

//////////////////////////////////////////////////////////
////add by chengming 2004.11.23
const short ICON_OFFSET = 8;
const short	ANNOTATION_X_OFF = 8;	// x方向的注记偏移
const short ANNOTATION_Y_OFF = 8;	// y方向的注记偏移

const COLORREF POINTOBJECT_FILL_FORECOLOR = RGB(255,0,0);
const COLORREF LINEOBJECT_FILL_FORECOLOR = RGB(0,255,255);
const COLORREF LINEOBJECT_FILL_BACKCOLOR = RGB(255,255,255);
const COLORREF POLYOBJECT_FILL_FORECOLOR = RGB(128,128,128);
const COLORREF POLYOBJECT_FILL_BACKCOLOR = RGB(0,0,0);
const short POINTOBJECT_PEN_WIDTH = 0;
const short LINEOBJECT_PEN_WIDTH = 5;
const short POLYOBJECT_PEN_WIDTH = 1;

//////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 协议类型：NMEA0183，GARMIN协议
enum enumProtocol	{eNMEA, eGarmin};
// 椭球类型：84，54，80
enum enumEllipsolid	{e84, e54, e80};
// 长度单位：米，公里，英尺
enum enumLengthUnit	{eMeter, eKm, eFoot};
// 角度单位：ddd.ddddd，ddd.mm'mmm，ddd.mm'sss
enum enumAngUnit	{eDdD, eDdMM, eDdMS};
// 初始化显示地图方式：默认索引图(可从注册表读取)，关机前显示的地图，不显示地图
enum enumInitDisMap	{eDefault, eLast, eNoMap};
// 航迹保存方式：按时间，按距离，全部保存
enum enumTrkSaveType	{eAll, eTime, eDis};

///////////////////////////////////////////////////////////////////////////////
// 点坐标结构，根据坐标类型确定
struct ST_GEO_PTCOOR
{
	double				dbX;							// B或者X，
	double				dbY;							// L或者Y
	double				dbZ;							// H或者Z
};

///////////////////////////////////////////////////////////////////////////////
// 计算函数
// 系统计算用的是统一的单位值；坐标：WGS84，弧度：度，长度：米
// 显示的时候根据系统设定转换成相应的单位

// 浮点数判断函数
bool swEXPORT EQ(double dbNum1, double dbNum2);		// 等于
bool swEXPORT BIGEQ(double dbNum1, double dbNum2);	// 大于等于
bool swEXPORT SMEQ(double dbNum1, double dbNum2);	// 小于等于

// 将字符串转换为数字
bool StrToNum (short 	&sNum,	 	TCHAR *lptszStr);
bool StrToNum (int 		&iNum, 		TCHAR *lptszStr);
bool StrToNum (long 	&lNum, 		TCHAR *lptszStr);
bool StrToNum (float	&dbNum, 	TCHAR *lptszStr);
bool StrToNum (double	&dbNum, 	TCHAR *lptszStr);

// 将数字转换为字符串，位数由用户定义
bool NumToStr (short 	sNum,	TCHAR *lptszStr);
bool NumToStr (int 		iNum, 	TCHAR *lptszStr);
bool NumToStr (long 	lNum, 	TCHAR *lptszStr);
bool NumToStr (float	dbNum,	TCHAR *lptszStr, short sDigit);
bool NumToStr (double	dbNum,	TCHAR *lptszStr, short sDigit);

// 字符转换
int		swEXPORT UnicodeToChar(swString &strIn, char *pchOut, int nCharLen);
int		swEXPORT CharToUnicode(char *pchIn, swString *pstrOut);

double	swEXPORT GetMin(double TX1, double TX2);
double	swEXPORT GetMax(double TX1, double TX2);

/**
 * @brief 下面三个都是计算两点间距离的函数,但是函数执行性能与计算精度有不同.
 * 
*/
double	swEXPORT CalcDist(double dx1, double dy1, double dx2, double dy2);     //性能最好,精度最差
double	swEXPORT CalcDistV2(double dx1, double dy1, double dx2, double dy2);   //精度最好,性能最差
double	swEXPORT CalcDistV3(double dx1, double dy1, double dx2, double dy2);   //性能与精度适中

double	swEXPORT CalcAngle(double dx1, double dy1, double dx2, double dy2);
void swEXPORT CalcCenter(CGIS_FeaturePoly* pObj);
void	swEXPORT CalcTransit(double dist, double angle, double dx1, double dy1, double& dx, double& dy);

void	swEXPORT Time_dbToSys(double dbTime, SYSTEMTIME *sysTm);
void	swEXPORT Time_SysTodb(SYSTEMTIME &sysTm, double *dbTime);
void	swEXPORT Time_UTCToLocal(SYSTEMTIME const &UtcTm, SYSTEMTIME *pSysTm);

// 系统角度单位与用户设定角度单位转换(空间坐标单位)
double	swEXPORT Ang_SysToUser(double dbIn, enumAngUnit emUserUnit);
double	swEXPORT Ang_UserToSys(double dbIn, enumAngUnit emUserUnit);

// 度、分、秒转换
double	swEXPORT DDDToDMM(double);
double	swEXPORT DDDToDMS(double);
double	swEXPORT DMMToDDD(double);
double	swEXPORT DMSToDDD(double);

double	swEXPORT Len_SysToUser(double dbIn, enumLengthUnit emUserUnit);
double	swEXPORT Len_UserToSys(double dbIn, enumLengthUnit emUserUnit);

///////////////////////////////////////////////////////////////////////////////
// 串口指针
//extern IGpsComm	*pGpsComm;

///////////////////////////////////////////////////////////////////////////////
// 控制输入法的弹出和弹入
void ShowInputPanel(const BOOL bShow);
// 控制全屏显示(任务栏、开始图标、SIP图标)
void ShowTaskbar(const BOOL bShow);
// 控制全屏显示(命令栏)
void ShowCommandbar(const BOOL bShow);
///////////////////////////////////////////////////////////////////////////////
// 得到应用程序路径
swString GetProPathName(swString strPathName);
///////////////////////////////////////////////////////////////////////////////
// 播放声音文件 
// 参数说明：
// 默认播放声音文件0.WAV
// 提供相应的声音ID，自动播放指定的声音文件
BOOL PlayVoice(const DWORD dwVoiceID = 0);

///////////////////////////////////////////////////////////////////////////////
// 计算目标点与两点组成的线段能否作垂线
// 如果可以，则返回垂线长度，如果不可以，则返回目标点与这两点连线的较短值
double UpRightDis(double x, double y, double dx1, double dy1, double dx2, 
				  double dy2, double &ux, double &uy);
void getFontCenter(int& nx,int& ny,int nH,int nW,double dAngle);
double swEXPORT GetDirection(double X1,double Y1,double NX,double NY);
BOOL swEXPORT LineIntersectRound(ST_GEO_PTXY Rpt,double dbRadius, ST_GEO_PTXY pt1,ST_GEO_PTXY pt2);
BOOL swEXPORT PointOnLine(ST_GEO_PTXY pt , ST_GEO_PTXY pt1,ST_GEO_PTXY pt2);
//计算点在线段上的垂足
ST_GEO_PTXY swEXPORT DisPointToLine(ST_GEO_PTXY pt, ST_GEO_PTXY pt1,ST_GEO_PTXY pt2);
BYTE swEXPORT DisPointToLine(ST_GEO_PTXY pt, ST_GEO_PTXY pt1,ST_GEO_PTXY pt2, ST_GEO_PTXY &ptProj );
BOOL swEXPORT LineToLine(ST_GEO_PTXY p1, ST_GEO_PTXY p2,ST_GEO_PTXY q1,ST_GEO_PTXY q2,double &x,double &y);
BOOL swEXPORT LineToLine(ST_GEO_PTXY p1, ST_GEO_PTXY p2,ST_GEO_PTXY q1,ST_GEO_PTXY q2);
BOOL swEXPORT PointInPoly(ST_GEO_PTXY pt , CGIS_FeaturePoly* pPolyFea);
BOOL swEXPORT PointInPoly( ST_GEO_PTXY pt, ST_GEO_PTXY *pPtList, int nPtNum );
BOOL swEXPORT LineInPoly( ST_GEO_PTXY pt1, ST_GEO_PTXY pt2, CGIS_FeaturePoly* pPolyFea );
BOOL swEXPORT LineInPoly( ST_GEO_PTXY pt1, ST_GEO_PTXY pt2, ST_GEO_PTXY *pPtList, int nPtNum );
BOOL swEXPORT MuLineInPoly(CGIS_FeatureLine* pLineFea,CGIS_FeaturePoly* pPolyFea);
BOOL swEXPORT PolyInPoly(CGIS_FeaturePoly* pPolyFea1,CGIS_FeaturePoly* pPolyFea2);
BOOL swEXPORT PolyInPoly(ST_GEO_PTXY *pPtSour, int nNumSour, ST_GEO_PTXY *pPtDest, int nNumDest );
BOOL swEXPORT RectInPoly(ST_GEO_RECT rtSour, ST_GEO_PTXY *pPtDest, int nNumDest );
void GreatCircle(float PointA_Latitude, float PointA_Longitude,
		 float PointB_Latitude, float PointB_Longitude,
		 float *GreatCircleDistance, float *GreatCircleBearing);
void InvertCircle( long fromlat,long fromlog, double dis, double  course , long *outlat, long *outlog );
//简拼查询　返回数据包位置
int GetJPID( char* chName );


///////////////////////////////////////////////////////////////////////////////
// hansom add 200305221300 debugfile
extern void DebugFile(const TCHAR * lpszFormat, ...);

// added by scott
int fabs(int x);
int pow(int x,int y);
float sqrt(int x);


///////////////////////////////////////////////////////////////////////////////
#endif 
