
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
const long SAT_ID_NUMBER	= 12;						// ��������ID��Ŀ
const double PI				= 3.14159265358979;

//////////////////////////////////////////////////////////
////add by chengming 2004.11.23
const short ICON_OFFSET = 8;
const short	ANNOTATION_X_OFF = 8;	// x�����ע��ƫ��
const short ANNOTATION_Y_OFF = 8;	// y�����ע��ƫ��

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
// Э�����ͣ�NMEA0183��GARMINЭ��
enum enumProtocol	{eNMEA, eGarmin};
// �������ͣ�84��54��80
enum enumEllipsolid	{e84, e54, e80};
// ���ȵ�λ���ף����Ӣ��
enum enumLengthUnit	{eMeter, eKm, eFoot};
// �Ƕȵ�λ��ddd.ddddd��ddd.mm'mmm��ddd.mm'sss
enum enumAngUnit	{eDdD, eDdMM, eDdMS};
// ��ʼ����ʾ��ͼ��ʽ��Ĭ������ͼ(�ɴ�ע����ȡ)���ػ�ǰ��ʾ�ĵ�ͼ������ʾ��ͼ
enum enumInitDisMap	{eDefault, eLast, eNoMap};
// �������淽ʽ����ʱ�䣬�����룬ȫ������
enum enumTrkSaveType	{eAll, eTime, eDis};

///////////////////////////////////////////////////////////////////////////////
// ������ṹ��������������ȷ��
struct ST_GEO_PTCOOR
{
	double				dbX;							// B����X��
	double				dbY;							// L����Y
	double				dbZ;							// H����Z
};

///////////////////////////////////////////////////////////////////////////////
// ���㺯��
// ϵͳ�����õ���ͳһ�ĵ�λֵ�����꣺WGS84�����ȣ��ȣ����ȣ���
// ��ʾ��ʱ�����ϵͳ�趨ת������Ӧ�ĵ�λ

// �������жϺ���
bool swEXPORT EQ(double dbNum1, double dbNum2);		// ����
bool swEXPORT BIGEQ(double dbNum1, double dbNum2);	// ���ڵ���
bool swEXPORT SMEQ(double dbNum1, double dbNum2);	// С�ڵ���

// ���ַ���ת��Ϊ����
bool StrToNum (short 	&sNum,	 	TCHAR *lptszStr);
bool StrToNum (int 		&iNum, 		TCHAR *lptszStr);
bool StrToNum (long 	&lNum, 		TCHAR *lptszStr);
bool StrToNum (float	&dbNum, 	TCHAR *lptszStr);
bool StrToNum (double	&dbNum, 	TCHAR *lptszStr);

// ������ת��Ϊ�ַ�����λ�����û�����
bool NumToStr (short 	sNum,	TCHAR *lptszStr);
bool NumToStr (int 		iNum, 	TCHAR *lptszStr);
bool NumToStr (long 	lNum, 	TCHAR *lptszStr);
bool NumToStr (float	dbNum,	TCHAR *lptszStr, short sDigit);
bool NumToStr (double	dbNum,	TCHAR *lptszStr, short sDigit);

// �ַ�ת��
int		swEXPORT UnicodeToChar(swString &strIn, char *pchOut, int nCharLen);
int		swEXPORT CharToUnicode(char *pchIn, swString *pstrOut);

double	swEXPORT GetMin(double TX1, double TX2);
double	swEXPORT GetMax(double TX1, double TX2);

/**
 * @brief �����������Ǽ�����������ĺ���,���Ǻ���ִ����������㾫���в�ͬ.
 * 
*/
double	swEXPORT CalcDist(double dx1, double dy1, double dx2, double dy2);     //�������,�������
double	swEXPORT CalcDistV2(double dx1, double dy1, double dx2, double dy2);   //�������,�������
double	swEXPORT CalcDistV3(double dx1, double dy1, double dx2, double dy2);   //�����뾫������

double	swEXPORT CalcAngle(double dx1, double dy1, double dx2, double dy2);
void swEXPORT CalcCenter(CGIS_FeaturePoly* pObj);
void	swEXPORT CalcTransit(double dist, double angle, double dx1, double dy1, double& dx, double& dy);

void	swEXPORT Time_dbToSys(double dbTime, SYSTEMTIME *sysTm);
void	swEXPORT Time_SysTodb(SYSTEMTIME &sysTm, double *dbTime);
void	swEXPORT Time_UTCToLocal(SYSTEMTIME const &UtcTm, SYSTEMTIME *pSysTm);

// ϵͳ�Ƕȵ�λ���û��趨�Ƕȵ�λת��(�ռ����굥λ)
double	swEXPORT Ang_SysToUser(double dbIn, enumAngUnit emUserUnit);
double	swEXPORT Ang_UserToSys(double dbIn, enumAngUnit emUserUnit);

// �ȡ��֡���ת��
double	swEXPORT DDDToDMM(double);
double	swEXPORT DDDToDMS(double);
double	swEXPORT DMMToDDD(double);
double	swEXPORT DMSToDDD(double);

double	swEXPORT Len_SysToUser(double dbIn, enumLengthUnit emUserUnit);
double	swEXPORT Len_UserToSys(double dbIn, enumLengthUnit emUserUnit);

///////////////////////////////////////////////////////////////////////////////
// ����ָ��
//extern IGpsComm	*pGpsComm;

///////////////////////////////////////////////////////////////////////////////
// �������뷨�ĵ����͵���
void ShowInputPanel(const BOOL bShow);
// ����ȫ����ʾ(����������ʼͼ�ꡢSIPͼ��)
void ShowTaskbar(const BOOL bShow);
// ����ȫ����ʾ(������)
void ShowCommandbar(const BOOL bShow);
///////////////////////////////////////////////////////////////////////////////
// �õ�Ӧ�ó���·��
swString GetProPathName(swString strPathName);
///////////////////////////////////////////////////////////////////////////////
// ���������ļ� 
// ����˵����
// Ĭ�ϲ��������ļ�0.WAV
// �ṩ��Ӧ������ID���Զ�����ָ���������ļ�
BOOL PlayVoice(const DWORD dwVoiceID = 0);

///////////////////////////////////////////////////////////////////////////////
// ����Ŀ�����������ɵ��߶��ܷ�������
// ������ԣ��򷵻ش��߳��ȣ���������ԣ��򷵻�Ŀ��������������ߵĽ϶�ֵ
double UpRightDis(double x, double y, double dx1, double dy1, double dx2, 
				  double dy2, double &ux, double &uy);
void getFontCenter(int& nx,int& ny,int nH,int nW,double dAngle);
double swEXPORT GetDirection(double X1,double Y1,double NX,double NY);
BOOL swEXPORT LineIntersectRound(ST_GEO_PTXY Rpt,double dbRadius, ST_GEO_PTXY pt1,ST_GEO_PTXY pt2);
BOOL swEXPORT PointOnLine(ST_GEO_PTXY pt , ST_GEO_PTXY pt1,ST_GEO_PTXY pt2);
//��������߶��ϵĴ���
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
//��ƴ��ѯ���������ݰ�λ��
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
