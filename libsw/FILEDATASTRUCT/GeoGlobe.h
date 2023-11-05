///////////////////////////////////////////////////////////////////////////////
// GeoGlobe.h: interface for the Geo Globals
//
// Copyright (c) 2004，上海合众思壮科技有限责任公司GIS部
//
// All rights reserved
//
// 文件名称：GeoGlobe.h
//
// 摘要    ：GEO公共函数头文件
//
// 作者    ：Hansom
//
// 当前版本：1.1
//
// 完成日期：2004年05月15日
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_GEOGLOBE_H__D3FCF0B0_E7A5_4917_83E4_D2FDDE447597__INCLUDED_)
#define AFX_GEOGLOBE_H__D3FCF0B0_E7A5_4917_83E4_D2FDDE447597__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxtempl.h>
///////////////////////////////////////////////////////////////////////////////
const int MAPID_UM_NAVI		= 1000;
const int MAPID_MAP_NATION	= 1;
const int MAPID_MAP_DETAIL = 2;
const int MAPID_MAP_BEIJING	= 336;
const int MAX_LINE_LENGTH	= 500;
const int RUT_TYPE_NUM		= 7;
const double FN_LEN			= 30.;
const double MAPSECLUDE_SCALE = 450.0;
const BYTE MOVE_CLIP_SCREEN = 6;
const unsigned short MAX_BRUT_NUM = 10570;
const BYTE MAX_RLAY_NUM	= 7;
const unsigned short CITY_LIST_ZHIXIA = 1;
const unsigned short CITY_LIST_ALL = 0;
const int MATCH_CITYROAD_NORMAL = 60;
const int MATCH_CITYROAD_VTX   = 20;
const int MATCH_HIGHWAY_NOMAL = 200;
const int MATCH_HIGHWAY_VTX = 60;
const int PASS_VTX = 20;
const int MATCH_HIGHWAY_LEAVE = 500;
const int MATCH_CITYROAD_LEAVE = 100;
const int BUFFER_QUERY_STEP = 5000;
const int BUFFER_QUERY_MAX = 100000;
const int LAYER_PT_START = 12;
const int LAYER_PT_END = 24;
class CGIS_Feature;
class CGIS_FeatureRut;
//自定义宏

#define FLOAT_ZERO    0.0000082
#define FLOAT_DETA_XY 0.0000120
#define QUERY_INDEX_POI 1
#define QUERY_INDEX_ROAD 2
//消息宏
#define MSG_CHANGE_MAP = 0
#define MSG_CHANGE_MAPINFO = 1
#define WM_MSG_REDRAW  (WM_USER + 5001)
#define MSG_READFILE_EXCEPTION (WM_USER+1024)

///地物类型
typedef enum tagENGEOOBJECTTYPE{
	EN_GOTYPE_NONE,			//地图点地物
	EN_GOTYPE_FIRST,

	EN_GOTYPE_POINT = EN_GOTYPE_FIRST,	//地图点地物
	EN_GOTYPE_UPOINT,		//用户点地物
	EN_GOTYPE_LINE,			//地图线地物
	EN_GOTYPE_ULINE,		//用户线地物
	EN_GOTYPE_POLY,			//地图面地物
	EN_GOTYPE_UPOLY,		//用户面地物
	EN_GOTYPE_RUT,			//地图道路地物
	EN_GOTYPE_URUT,			//用户道路地物
	EN_GOTYPE_ROUTE,		//航线地物类型
	EN_GOTYPE_TRACK,		//航迹地物类型
	EN_GOTYPE_ADDRESS,		//地址薄类型
	EN_GOTYPE_LAST,
}EnGeoObjType;
///图层类型
typedef enum tagENLAYERTYPE{
	EN_LAYTYPE_USER = 0,	//用户图层
	EN_LAYTYPE_MAP			//地图图层
}EnLayType;
///地图文件类型
typedef enum tagENMAPFILETYPE{
	EN_MFTYPE_BAND = 0,		//分幅/桶地图文件
	EN_MFTYPE_GUIDE30		//导航30
}EnMapFileType;
///图层文件类型
typedef enum tagENLAYFILETYPE{
	EN_LFTYPE_BAND = 0,		//分幅/桶地图图层文件
	EN_LFTYPE_GUIDE30,		//导航30
	EN_LFTYPE_ELF3,
	EN_LFTYPE_MIF
}EnLayFileType;
///相互间的剪切关系
typedef enum tagENCLIPTYPE{ 
	CLIP_NULL = 0, //no common area
	CLIP_IN,	//A complete in B
	CLIP_OUT,	//B complete in A
	CLIP_CROSS	//A and B have crossing part
} EnClipType;
///定义计算方法
typedef enum tagENCALPATHMETHOD
{
	EN_CPMETHOD_QUICKESTCAL = 0,//最快计算
	EN_CPMETHOD_QUICKCAL,		//快速计算
	EN_CPMETHOD_BETTERROUTE,		//较好航线
	EN_CPMETHOD_BESTROUTE			//最佳航线
}EnCalPathMethod;
///定义选路方式
typedef enum tagENROUTEPREFERENCE
{
	EN_RUTPREF_FASTERTIME = 0,	//最短时间(较快路线)
	EN_RUTPREF_SHORTDIS,		//最短距离(较短路线)
	EN_RUTPREF_LITTLECOST,		//较少收费路线
	EN_RUTPREF_LITTLETURN		//较少转弯
}EnRoutePreference;
///define the vehicle type
typedef enum tagENVEHICLETYPE
{
	EN_VEHICLETYPE_CAR = 0,		//小汽车
	EN_VEHICLETYPE_TRUCK,		//大卡车
	EN_VEHICLETYPE_BUS,			//公交车
	EN_VEHICLETYPE_EMERGENCY,	//救护车/消防车
	EN_VEHICLETYPE_TAXI,		//出租车
	EN_VEHICLETYPE_DELIVERY,	//邮政车
	EN_VEHICLETYPE_BICYCLE,		//自行车
	EN_VEHICLETYPE_PEDESTRIAN	//步行
}EnVehicleType;
///定义航点类型
typedef enum tagENWAYPOINTTYPE
{
	EN_WAYPOINTTYPE_ORIGIN = 0,		//起始点
	EN_WAYPOINTTYPE_DESTINATION,	//目的点
	EN_WAYPOINTTYPE_INTERMEDIATE,	//中间点/途经点
	EN_WAYPOINTTYPE_CITYBAND		//城市边缘
}EnWaypointType;
typedef enum tagENWAYPOINTMARKTYPE
{
	EN_WAYPOINTMARK_INCITY = 0,
	EN_WAYPOINTMARK_OUTCITY,
}EnWaypointMarkType;
///定义航线上点的类型
typedef enum tagENPATHPOINTTYPE
{
	EN_PATHPOINTTYPE_START = 0,		//起点
	EN_PATHPOINTTYPE_INFLEXION,		//拐点
	EN_PATHPOINTTYPE_NODE,			//节点
	EN_PATHPOINTTYPE_INTERMEDIATE,	//途经点
	EN_PATHPOINTTYPE_END			//终点
}EnPathpointType;
///定义SDE工具类型
typedef enum tagENSDETOOLTYPE{
	EN_SDETOOLTYPE_NETWORK = 0,		///路网分析工具
	EN_SDETOOLTYPE_SYMBOL			///图标编辑管理工具
}EnSDEToolType;

///定义缓冲池类型
typedef enum tagENBUFSTORTYPE{
	EN_BUFSTORTYPE_NET = 0,		///用于路网
	EN_BUFSTORTYPE_VIEW			///用于显示
}EnBufStorType;
///定义导航数据图层类型
typedef enum tagENUSERMAPLAYERTYPE
{
	EN_UMLAYERTYPE_ROUTE = 0,
	EN_UMLAYERTYPE_TRACK,
	EN_UMLAYERTYPE_ADDRESSBOOK
}EnUserMapLayerType;

typedef enum tagENLAYEROBJECTTYPE{
	LOTYPE_POINT = 0,
	LOTYPE_LINE,
	LOTYPE_POLY
}EnLObjectType;
typedef enum tagENLINELEVEL
{
	EN_LEVEL_DEGREE = 0,
	EN_LEVEL_MINUTE,
	EN_LEVEL_SECOND
}EnLinelevel;
typedef enum tagMATCHROAD
{
	EN_MATCH_NORMAL = 0,
	EN_MATCH_SIMULATENAV,
	EN_MATCH_REALNAV
}EnMathState;
struct  ST_LonLatLinelevel
{
	EnLinelevel EnLevel;
	UINT nDecimal;
};
typedef enum tagENMAPTYPE
{
	EN_MAPTYPE_CITY = 0,
	EN_MAPTYPE_NATION
}EnMapType;

//TOOL类型
typedef enum tagENTOOLTYPE
{
	QUERY_BYSHARP = 0,
	QUERY_BYATT,
	QUERY_POI,
	EDIT_FEATURE,
	CONFIG_MAP,
	CONFIG_LAYER,
	LAYER_TOOL
}EnTooType;

//查找时返回的地物类型
typedef enum tagENOBJTYPE
{
	EN_OBJ_ALL = 0,
	EN_OBJ_PT,
	EN_OBJ_LINE,
	EN_OBJ_POLY,
	EN_OBJ_RUT
}EnQueryObjType;
//道路匹配返回值
typedef enum tagENMATCHROAD
{
	EN_MATCH_NULL = 0, //附近无可匹配的道路。
	EN_MATCH_WELL,	   //正常匹配
	EN_MATCH_OUT,	   //在导航状态下，离开道路匹配的阀值，但是还未到偏离航线的阀值
	EN_NAV_OUT,		   //导航状态下，车辆偏离航线但附近还有可匹配道路。
}EnMatchRoad;
//定义高速公路上服务区的类型
typedef enum tagENHWSERVERTYPE
{
	EN_HWST_NONE = 0,
	EN_HWST_REST = 1,	//餐饮
	EN_HWST_GAS			//加油站
//	EN_HWST_STOP		
}EnHWServerType;

//定义错误的返回值类型
typedef enum tagENEXCEPTYPE
{
	EN_ET_NOERR = 0,	//没有错误
	EN_ET_MEMERR,	//内存访问错误
	EN_ET_MEMOVER,	//内存溢出
	EN_ET_FILEERR,	//文件访问错误
	EN_ET_RESERR,	//window资源访问错误
	EN_ET_ARCERR,	//串行化错误
	EN_ET_OTHERR	//其他错误
}EnExceptType;

/////define the map of CGIS_Field by FieldID
class CGIS_Field;
typedef CMap< UINT, UINT, CGIS_Field*, CGIS_Field* > CGIS_FieldMap;
class IGIS_Layer;
typedef CMap< int, int, CPtrList *, CPtrList* > CListMap;
typedef CMap< short, short, IGIS_Layer*, IGIS_Layer* > CGIS_LayerMap;
///////////////////////////////////////////////////////////////////////////////
// 画笔结构
struct ST_GEO_PEN
{
	COLORREF	nColor;		// 颜色
	BYTE		nWidth;		// 线宽
	BYTE		nStyle;		// 线型
};

///////////////////////////////////////////////////////////////////////////////
// 刷子
struct ST_GEO_BRUSH
{
	COLORREF	nForeColor;	// 前景颜色
	COLORREF	nBackColor;	// 背景颜色
	BYTE		nWidth;		// 线宽
	BYTE		nStyle;		// 线型
	BYTE		nFillStyle;	// 填充样式
};

///////////////////////////////////////////////////////////////////////////////
// 字体
struct ST_GEO_FONT
{
	COLORREF	nColor;		// 字体颜色
	int			nSize;		// 尺寸
	int			nCharID;	// 默认字符id
	int			nStyle;
};
///////////////////////////////////////////////////////////////////////////////
//Name索引结构体(名称，图幅号)
struct ST_INDEX_NAME
{
	char  *Name;
	int  BandID;
	BYTE  LayerID;
	BYTE  ClassID;
};
///城市地图信息
struct ST_CCITY_INFO
{
	int  nCityID;	//城市ID
	char strCityName[32];	//城市的名称
//	CString strMName;		//地图的名称
//	int nBNum;				//包含的图幅个数
//	int *pBIDList;			//图幅ID列表
};
//省信息
struct ST_CPRO_INFO
{
	unsigned short nProvinceID; //省的ID
	char strPName[32];		    //省的名称
};
//国家信息
struct ST_CCOUNTRY_INFO
{
	unsigned short nCountryID;  //国家的ID
	char strCName[32];		    //国家的名称
	CPtrList *PList;			//该国所有省列表
	CPtrList *CityList;			//该国所有城市列表
};
//POI类型信息
struct ST_POITYPE_INFO
{
	BYTE nTypeID; //POI类型ID
	char strPOIName[32];    //POI名称
	CPtrList *pSubList;		//小类列表
};
//POI小类信息
struct ST_SUBPOITYPE_INFO
{
	char strSubPOIName[32];// 小类名称
	BYTE nTypeID ; // 小类ID
};
///////////////////////////////////////////////////////////////////////////////
// 坐标结构体(DX、DY)
struct ST_GEO_PTXY
{
	float		dx, dy;
};
//定义边缘节点信息
struct ST_GEO_VEDGE
{
	BYTE			byEdge;		//是否是图幅边缘:0-不是,1-是
	int				nABID;		//邻接图幅号
	unsigned short	nARID;		//连接Rut_id
};
//定义道路虚拟图信息
struct ST_GEO_RVDRAFT
{
	BYTE	byVirtual;		//是否存在虚拟图:0- 没有虚拟图;非0- 有虚拟图
	unsigned short *pVORList;	//出去的道路ID数组
	int		*pVIDs;			//虚拟图编号数组
};
//定义道路的禁行信息
struct ST_GEO_RLIMIT
{
	BYTE	byLimit;			//是否存在交通限制:0-没有交通限制;非0- 有交通限制
	unsigned short *pLRList;	//禁止通行的道路id
	BYTE	byRLimit;			//是否反向存在交通限制:0-没有交通限制;非0- 有交通限制
	unsigned short *pRLRList;	//反向禁止通行的道路id
};
//定义节点遍历的信息
struct ST_GEO_VSORT
{
	POSITION posOHead;
	BYTE bySHead;	//Vertex status:0 1 2 ,	0 代表未曾计算过,1 OPEN, 2 CLOSE
	POSITION posOEnd;
	BYTE bySEnd;	//Vertex status:0 1 2 ,0 代表 未曾计算过,1 OPEN,2 CLOSE
};
//交叉路口结构体
struct ST_ROAD_PT
{
	ST_GEO_PTXY  pt;
	CString      Name;
};
//带路段数据的交叉路口结构
struct ST_ROAD_INNAME
{
	CGIS_FeatureRut *pRoute;
	CString InName;
};
struct ST_ROAD_CHOOSE
{
	POSITION pos;  
	int nMin; //最短距离
	ST_GEO_PTXY pt; //匹配后的位置
};

///////////////////////////////////////////////////////////////////////////////
// 地理范围结构(左、右、顶、底)
struct ST_GEO_RECT
{
	float		dLeft, dTop, dRight, dBottom;
};

///////////////////////////////////////////////////////////////////////////////
// 道路信息结构体
struct ST_GEO_RUTINFO
{
	int			nID;
	BYTE		byDirection;			// 单双行标号
	BYTE		byType;					// 道路类别，1-3，用颜色表示 ,只是用来显示而已
    BYTE		wWidth;					// 路宽.米
	BYTE		bySaturated;			// 路段饱和度
	BYTE		byLaneNum;				// 车道数
	BYTE		bySlope;				// 坡度
	BYTE		byMaterial;				// 路面材质
	BYTE		byRteLevel;				// 道路等级
	DWORD		dwVehicle;				// 车辆通行限制
	DWORD		dwInfo;					// 其他通行限制
	float		fLength;				// 从nStartPtID到nEndPtID方向权值。初始0，不通为-1
	ST_GEO_RECT	rectObj;

	int			nVtxStartID;
	int			nVtxEndID;

	int			nPtNum;
	DWORD		dwNotePos;
};

///////////////////////////////////////////////////////////////////////////////
// 注释信息结构体
struct ST_GEO_ANNO
{
	CString	szAnno;
	int		nRectNum;
	RECT	*pRect;
};

struct ST_LINE_INFO
{
	int			nPartNum;
	DWORD		dwNotePos;
	ST_GEO_RECT	rectObj;
};
struct ST_RTV_HEAD
{
	char chHeader[4];
	int nRutNum;
	int nVtxNum;
	int nPtPos;
};
struct ST_VOICE_INDEX
{
	CString strLong;
	CString strShort;
};
struct ST_VOICE_NODE
{
	BYTE byAngle;		//拐弯角度类型（1.直行 2.缓拐3.拐弯4.急拐5.U型拐）
	BYTE bySameName;	//出入道路路名是否一致（11．是12．否)
	BYTE byConfusion;	//是否存在混乱路口（15．是16．否)
	BYTE byConfusion2;	//表示转向同类路口的状态(15左侧,16不存在,37右侧)
	BYTE byOtherStreet;	//是否需要除出入口道路外的其他道路（21．是22．否)
	BYTE byIMI;			//是否存在IMI(23．是24．否)
	BYTE byDestination;	//是否导到目的地（25．是26．否)
	BYTE byTurn;		//拐弯方向(27．左28．右)
	BYTE byRound;		//入口道路与出口道路之间是否有环岛(29．是30．否)
	BYTE bySameRoad;	//入口道路和出口道路是否为同一条道路（31．是32．否)
	BYTE byExName;		//是否存在与出口道路同名的道路（除入口道路外）(RUT)17． 是18． 否
	BYTE byEnName;		//是否存在与入口道路同名的道路（除出口道路外）(RUT)19． 是20． 否
};
struct ST_VOICE_RUT
{
	BYTE byType;		//道路类型(RUT)(6． 高速路7． 匝道8． 普通道路33．隧道34．渡口)
	BYTE byAdd;			//道路类型附加项(9． 是环岛10． 不是环岛)
	BYTE byDrivable;	//道路是否可以驾车行驶(RUT)13． 是14． 否
	BYTE byCharge;		//是否为收费路段（RUT）35． 是36． 否
};
struct ST_FEATURE_POINT
{
	CGIS_Feature *pFeature;
	double dbDis;
};
//避开路段信息
struct ST_AVOID_RUT
{
	unsigned short nMID;	//路段所在map的ID
		int nBID;				//路段所在图幅的编号ID
	int nRID;				//路段的ID
};
//机动信息
struct ST_TURN_INFO
{
	ST_TURN_INFO()
	{
		psPos = NULL;
		psEMPos = NULL;
		byTDir = byTAng = 0;
		dbD2Pre = 0.0;
		strRName = strLVoice = strSVoice = _T("");
		bSeriMobile = FALSE;
		byRMobile = 0;
		byRMType = 0;
		byConfusion2 = 16;
	}
	POSITION psPos;		//机动点在航线上的位置
	POSITION psEMPos;	//机动结束的位置(环岛/连续机动)
	BYTE	 byTDir;	//该机动的转弯方向(1、左转 2、右转 3、直行 4.环岛 5.目的地)
	BYTE	 byTAng;	//1.直行 2.缓拐3.拐弯4.急拐5.U型拐
	BYTE	 byConfusion;	//表示混乱路口的状态(15左侧,16不存在,37右侧)
	BYTE	 byConfusion2;	//表示转向同类路口的状态(15左侧,16不存在,37右侧)
	CString	 strRName;	//下条道路名称
	double	 dbD2Pre;	//距离前一个机动点的距离
	CString	 strLVoice; //长距离语音信息
	CString  strSVoice;	//短距离语音信息
	BOOL	 bSeriMobile;//是否为连续机动的后续点
	BYTE	 byRMobile;	//环岛的个数
	BYTE	 byRMType;	//环岛的类型（0.不是环岛，1.输出的参数是第几个出口，2.距离与出口信息）
};
//绘制POI图标
struct ST_PT_DRAW 
{
	int nx;
	int ny;
	int nIconID;
};

///定义原始位置信息
extern int MAP_ORIGIN_BID;
extern int MAP_ORIGIN_XNUM;
extern int MAP_ORIGIN_YNUM;
extern float MAP_ORIGIN_CENTERX;
extern float MAP_ORIGIN_CENTERY;
//static int MAP_ORIGIN_BID = 444624;
//static int MAP_ORIGIN_XNUM = 372;
//static int MAP_ORIGIN_YNUM = 354;
//static float MAP_ORIGIN_CENTERX = 106.563;
//static float MAP_ORIGIN_CENTERY = 29.5417;

//把图幅的ID转换为横向纵向的的个数
void ParseBMapID( int nBMID, int &nBMXNum, int &nBMYNum );
//根据偏移得到图幅的ID
int CalculateBMapID( int nBMXID, int nBMYID );
//根据点位置得到图幅的ID
int CalculateBMapID( float dx, float dy );
//根据点位置得到图幅的ID
void CalculateBMapIDInfo( float dx, float dy, int &nXNum, int &nYNum );
//根据点位置得到图幅的ID
void CalculateBMapIDInRect( ST_GEO_RECT rt, int nBMIDs[3000], int &nBNum );
//得到图幅的矩形框
ST_GEO_RECT CalculateBMapRect( int nBMID );

//计算线的buffer
void CreateRect(double d,ST_GEO_PTXY& pa,ST_GEO_PTXY& pb,ST_GEO_PTXY& pc,ST_GEO_PTXY& pd,ST_GEO_PTXY& pe,ST_GEO_PTXY& pf);
double AntiClock(ST_GEO_PTXY& center,ST_GEO_PTXY& outer);
double Clockwise(ST_GEO_PTXY& center,ST_GEO_PTXY& outer);
BOOL CalcLineBuff( double dbWidth, ST_GEO_PTXY *pPtList, int nPtNum, ST_GEO_PTXY pPtBuf[], int &nPtBuf );
double CalcArea( ST_GEO_PTXY pt1, ST_GEO_PTXY pt2, ST_GEO_PTXY pt3 );
// 在矩形列表中选取一个最大的矩形区域返回
void	__declspec(dllexport) CalcRect(ST_GEO_PTXY* pPt, int nNum, ST_GEO_RECT& rect);
///从内存块中读取部分内容
size_t Fread(char *pFileBuf,DWORD dwSize,DWORD &dwStation,void *buffer, size_t size, size_t count);

/*/判断点和矩形框的关系(0~9)
	 1----2-----3
	 |			|
	 4	  5		6		0
	 |			|
	 7----8-----9
 */
BYTE	__declspec(dllexport) GetRelationPt2Rect( double dx, double dy, double dx1, double dy1, double dx2, double dy2);
void ParseBreadthMapInfo( char* szInfo, int &nIndex, int &nParent, ST_GEO_RECT &rtBreadth );
BOOL ExtractFilePath(char* szFileName,char* szPath);
char* getStrItemEx(char* szBuf,char szLim,int nPos,int &nLen);

// 规范化指定的矩形
void	NormalizeRect(ST_GEO_RECT &rect);
// 判断矩形交叉情况：0无交集，1有交集，2、3包含关系
int		__declspec(dllexport) GeoRectIntersect(ST_GEO_RECT *sourceRect, ST_GEO_RECT *refRect);
// 判断已知一点是否在另外两点的区域内，如果在其内部返回TRUE
BOOL	__declspec(dllexport) IsPtInRect(double dx, double dy, double dx1, double dy1, double dx2, double dy2);
BOOL __declspec(dllexport) IsPtInScreen(int x,int y,int x1,int x2,int y1,int y2);

// 获得折线段上的离散点链表
BOOL	__declspec(dllexport) GetPointsOnLine(ST_GEO_PTXY* pLine, int nPtNum, int nPart, ST_GEO_PTXY* pPtList);
// 计算一条线段的长度
double	__declspec(dllexport) GetLength(double dx1, double dy1, double dx2, double dy2);
// 获得折线链表的总长度
double	__declspec(dllexport) GetTotalLength(ST_GEO_PTXY *pPt, int nNum);
// 根据斜率在弧上求得一点坐标
void	__declspec(dllexport) GetPointOnArc(double dx1, double dy1, double dx2, double dy2, 
					  double dRatio, double& xx, double& yy);
// 得到剪切矩形
ST_GEO_PTXY* GetClipRgn(ST_GEO_PTXY* pPt, int nNum, ST_GEO_RECT &rect, 
						int *pnPtNum, int nObjType);
// 判断两个存在的矩形是否有交集，非零表示有交集
BOOL	__declspec(dllexport) IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2);
// 如果计算出来的交点位于两个矩形的公共区域，则为真交点，返回TRUE
BOOL	__declspec(dllexport) GetIntersectedPoint(double X1, double Y1, double X2, double Y2, double X3, 
							double Y3, double X4, double Y4, double &XX, double &YY, int Flag);
//得到POI的小类别
unsigned short  GetPoiID(unsigned short nId, UINT nLayerID);
//计算字符的内码
int     GetCharCode(CString strIn);

///////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_GEOGLOBE_H__D3FCF0B0_E7A5_4917_83E4_D2FDDE447597__INCLUDED_)
