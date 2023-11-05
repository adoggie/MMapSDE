
///////////////////////////////////////////////////////////////////////////////
#ifndef _GEO_GLOBE_H
#define _GEO_GLOBE_H
#include "swBase.h"

//#include <Afxtempl.h>
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
const unsigned short MAX_BRUT_NUM = 12000;
const BYTE MAX_RLAY_NUM	= 7;	//��·����
const unsigned short CITY_LIST_ZHIXIA = 1;
const unsigned short CITY_LIST_ALL = 0;
const int MATCH_CITYROAD_NORMAL = 60;
const int MATCH_CITYROAD_VTX   = 30;
const int MATCH_HIGHWAY_NOMAL = 150;
const int MATCH_HIGHWAY_VTX = 80;
const int PASS_VTX = 20;
const int MATCH_HIGHWAY_LEAVE = 300;
const int MATCH_CITYROAD_LEAVE = 100;
const int BUFFER_QUERY_STEP = 5000;
const int BUFFER_QUERY_MAX = 100000;
const int LAYER_PT_START = 12;
const int LAYER_PT_END = 25;
class CGIS_Feature;
class CGIS_FeatureRut;
//�Զ����

#define FLOAT_ZERO    0.0000082
#define FLOAT_DETA_XY 0.0000120
#define QUERY_INDEX_POI 1
#define QUERY_INDEX_ROAD 2
#define CHINESE_CODEPAGE 936
//��Ϣ��
//#define MSG_CHANGE_MAP   0
//#define MSG_CHANGE_MAPINFO   1
#define WM_MSG_REDRAW  (WM_USER + 5001)
#define MSG_READFILE_EXCEPTION (WM_USER+1024)

///��������
typedef enum tagENGEOOBJECTTYPE{
	EN_GOTYPE_NONE,			//��ͼ�����
	EN_GOTYPE_FIRST,

	EN_GOTYPE_POINT = EN_GOTYPE_FIRST,	//��ͼ�����
	EN_GOTYPE_UPOINT,		//�û������
	EN_GOTYPE_LINE,			//��ͼ�ߵ���
	EN_GOTYPE_ULINE,		//�û��ߵ���
	EN_GOTYPE_POLY,			//��ͼ�����
	EN_GOTYPE_UPOLY,		//�û������
	EN_GOTYPE_RUT,			//��ͼ��·����
	EN_GOTYPE_URUT,			//�û���·����
	EN_GOTYPE_ROUTE,		//���ߵ�������
	EN_GOTYPE_TRACK,		//������������
	EN_GOTYPE_ADDRESS,		//��ַ������
	EN_GOTYPE_LAST,
}EnGeoObjType;
///ͼ������
typedef enum tagENLAYERTYPE{
	EN_LAYTYPE_USER = 0,	//�û�ͼ��
	EN_LAYTYPE_MAP			//��ͼͼ��
}EnLayType;
///��ͼ�ļ�����
typedef enum tagENMAPFILETYPE{
	EN_MFTYPE_BAND = 0,		//�ַ�/Ͱ��ͼ�ļ�
	EN_MFTYPE_GUIDE30		//����30
}EnMapFileType;
///ͼ���ļ�����
typedef enum tagENLAYFILETYPE{
	EN_LFTYPE_BAND = 0,		//�ַ�/Ͱ��ͼͼ���ļ�
	EN_LFTYPE_GUIDE30,		//����30
	EN_LFTYPE_ELF3,
	EN_LFTYPE_MIF
}EnLayFileType;
///�໥��ļ��й�ϵ
typedef enum tagENCLIPTYPE{ 
	CLIP_NULL = 0, //no common area
	CLIP_IN,	//A complete in B
	CLIP_OUT,	//B complete in A
	CLIP_CROSS	//A and B have crossing part
} EnClipType;
///������㷽��
typedef enum tagENCALPATHMETHOD
{
	EN_CPMETHOD_QUICKESTCAL = 0,//������
	EN_CPMETHOD_QUICKCAL,		//���ټ���
	EN_CPMETHOD_BETTERROUTE,		//�Ϻú���
	EN_CPMETHOD_BESTROUTE			//��Ѻ���
}EnCalPathMethod;
///����ѡ·��ʽ
typedef enum tagENROUTEPREFERENCE
{
	EN_RUTPREF_FASTERTIME = 0,	//���ʱ��(�Ͽ�·��)
	EN_RUTPREF_SHORTDIS,		//��̾���(�϶�·��)
	EN_RUTPREF_LITTLECOST,		//�����շ�·��
	EN_RUTPREF_LITTLETURN		//����ת��
}EnRoutePreference;
///define the vehicle type
typedef enum tagENVEHICLETYPE
{
	EN_VEHICLETYPE_CAR = 0,		//С����
	EN_VEHICLETYPE_TRUCK,		//�󿨳�
	EN_VEHICLETYPE_BUS,			//������
	EN_VEHICLETYPE_EMERGENCY,	//�Ȼ���/������
	EN_VEHICLETYPE_TAXI,		//���⳵
	EN_VEHICLETYPE_DELIVERY,	//������
	EN_VEHICLETYPE_BICYCLE,		//���г�
	EN_VEHICLETYPE_PEDESTRIAN	//����
}EnVehicleType;
///���庽������
typedef enum tagENWAYPOINTTYPE
{
	EN_WAYPOINTTYPE_ORIGIN = 0,		//��ʼ��
	EN_WAYPOINTTYPE_DESTINATION,	//Ŀ�ĵ�
	EN_WAYPOINTTYPE_INTERMEDIATE,	//�м��/;����
	EN_WAYPOINTTYPE_CITYBAND		//���б�Ե
}EnWaypointType;
typedef enum tagENWAYPOINTMARKTYPE
{
	EN_WAYPOINTMARK_INCITY = 0,
	EN_WAYPOINTMARK_OUTCITY,
}EnWaypointMarkType;
///���庽���ϵ������
typedef enum tagENPATHPOINTTYPE
{
	EN_PATHPOINTTYPE_START = 0,		//���
	EN_PATHPOINTTYPE_INFLEXION,		//�յ�
	EN_PATHPOINTTYPE_NODE,			//�ڵ�
	EN_PATHPOINTTYPE_INTERMEDIATE,	//;����
	EN_PATHPOINTTYPE_END			//�յ�
}EnPathpointType;
///����SDE��������
typedef enum tagENSDETOOLTYPE{
	EN_SDETOOLTYPE_NETWORK = 0,		///·����������
	EN_SDETOOLTYPE_SYMBOL			///ͼ��༭������
}EnSDEToolType;

///���建�������
typedef enum tagENBUFSTORTYPE{
	EN_BUFSTORTYPE_NET = 0,		///����·��
	EN_BUFSTORTYPE_VIEW			///������ʾ
}EnBufStorType;
///���嵼������ͼ������
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

//TOOL����
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
//����·�ڲ�ѯ�ķ�������
typedef enum tagINTERSECT{
	EN_ROUTE = 0,
	EN_INTERSECT
}EnIntersectType;
//����ʱ���صĵ�������
typedef enum tagENOBJTYPE
{
	EN_OBJ_ALL = 0,
	EN_OBJ_PT,
	EN_OBJ_LINE,
	EN_OBJ_POLY,
	EN_OBJ_RUT
}EnQueryObjType;
//��·ƥ�䷵��ֵ
typedef enum tagENMATCHROAD
{
	EN_MATCH_NULL = 0, //�����޿�ƥ��ĵ�·��
	EN_MATCH_WELL,	   //����ƥ��
	EN_MATCH_OUT,	   //�ڵ���״̬�£��뿪��·ƥ��ķ�ֵ�����ǻ�δ��ƫ�뺽�ߵķ�ֵ
	EN_NAV_OUT,		   //����״̬�£�����ƫ�뺽�ߵ��������п�ƥ���·��
}EnMatchRoad;
//������ٹ�·�Ϸ�����������
typedef enum tagENHWSERVERTYPE
{
	EN_HWST_NONE = 0,
	EN_HWST_REST = 1,	//����
	EN_HWST_GAS			//����վ
//	EN_HWST_STOP		
}EnHWServerType;

//�������ķ���ֵ����
typedef enum tagENEXCEPTYPE
{
	EN_ET_NOERR = 0,	//û�д���
	EN_ET_MEMERR,	//�ڴ���ʴ���
	EN_ET_MEMOVER,	//�ڴ����
	EN_ET_FILEERR,	//�ļ����ʴ���
	EN_ET_RESERR,	//window��Դ���ʴ���
	EN_ET_ARCERR,	//���л�����
	EN_ET_OTHERR	//��������
}EnExceptType;
/////define the map of CGIS_Field by FieldID
class CGIS_Field;
//typedef CMap< UINT, UINT, CGIS_Field*, CGIS_Field* > CGIS_FieldMap;
typedef std::map<UINT,CGIS_Field> CGIS_FieldMap;
class IGIS_Layer;
//typedef CMap< int, int, CPtrList *, CPtrList* > CListMap;
typedef std::map< int,std::list<void*> > CListMap;
//typedef CMap< short, short, IGIS_Layer*, IGIS_Layer* > CGIS_LayerMap;
typedef std::map< short ,IGIS_Layer*> CGIS_LayerMap;
typedef int MapBreadthID_T;
///////////////////////////////////////////////////////////////////////////////
// ���ʽṹ
struct ST_GEO_PEN
{
	COLORREF	nColor;		// ��ɫ
	BYTE		nWidth;		// �߿�
	BYTE		nStyle;		// ����
};

///////////////////////////////////////////////////////////////////////////////
// ˢ��
struct ST_GEO_BRUSH
{
	COLORREF	nForeColor;	// ǰ����ɫ
	COLORREF	nBackColor;	// ������ɫ
	BYTE		nWidth;		// �߿�
	BYTE		nStyle;		// ����
	BYTE		nFillStyle;	// �����ʽ
};

///////////////////////////////////////////////////////////////////////////////
// ����
struct ST_GEO_FONT
{
	COLORREF	nColor;		// ������ɫ
	int			nSize;		// �ߴ�
	int			nCharID;	// Ĭ���ַ�id
	int			nStyle;
};
///////////////////////////////////////////////////////////////////////////////
//Name�����ṹ��(���ƣ�ͼ����)
struct ST_INDEX_NAME
{
	char  *Name;
	int  BandID;
	BYTE  LayerID;
	BYTE  ClassID;
};
//��ƴ����Name�����ṹ��(ƴ�����ơ����ƣ�ͼ����)
struct ST_PYINDEX_NAME
{
	swString strPYName;
	char*  Name;
	int  BandID;
	BYTE  LayerID;
	BYTE  ClassID;
};
///���е�ͼ��Ϣ
struct ST_CCITY_INFO
{
	int  nCityID;	//����ID
	char strCityName[32];	//���е�����
//	swString strMName;		//��ͼ������
//	int nBNum;				//������ͼ������
//	int *pBIDList;			//ͼ��ID�б�
};
//ʡ��Ϣ
struct ST_CPRO_INFO
{
	unsigned short nProvinceID; //ʡ��ID
	char strPName[32];		    //ʡ������
};
//������Ϣ
struct ST_CCOUNTRY_INFO
{
	unsigned short nCountryID;  //���ҵ�ID
	char strCName[32];		    //���ҵ�����
	//CPtrList *PList;			//�ù�����ʡ�б�
	std::vector<ST_CPRO_INFO> PList;
	//CPtrList *CityList;			//�ù����г����б�
	std::vector<ST_CCITY_INFO> CityList;
};
struct ST_SUBPOITYPE_INFO;
//POI������Ϣ
struct ST_POITYPE_INFO
{
	BYTE nTypeID; //POI����ID
	char strPOIName[32];    //POI����
	//CPtrList *pSubList;		//С���б�
	std::vector<ST_SUBPOITYPE_INFO> pSubList;
};
//POIС����Ϣ
struct ST_SUBPOITYPE_INFO
{
	char strSubPOIName[32];// С������
	BYTE nTypeID ; // С��ID
};
///////////////////////////////////////////////////////////////////////////////
// ����ṹ��(DX��DY)
typedef tagPointT<float> ST_GEO_PTXY;
/*
struct ST_GEO_PTXY
{
	float		dx, dy;
};*/
//�����Ե�ڵ���Ϣ
struct ST_GEO_VEDGE
{
	BYTE			byEdge;		//�Ƿ���ͼ����Ե:0-����,1-��
	unsigned short	nARID;		//����Rut_id
	int				nABID;		//�ڽ�ͼ����
};
//�����·����ͼ��Ϣ
struct ST_GEO_RVDRAFT
{
	BYTE	byVirtual;		//�Ƿ��������ͼ:0- û������ͼ;��0- ������ͼ
	unsigned short *pVORList;	//��ȥ�ĵ�·ID����
	int		*pVIDs;			//����ͼ�������
};
//�����·�Ľ�����Ϣ
struct ST_GEO_RLIMIT
{
	BYTE	byLimit;			//�Ƿ���ڽ�ͨ����:0-û�н�ͨ����;��0- �н�ͨ����
	BYTE	byRLimit;			//�Ƿ�����ڽ�ͨ����:0-û�н�ͨ����;��0- �н�ͨ����
	unsigned short *pLRList;	//��ֹͨ�еĵ�·id
	unsigned short *pRLRList;	//�����ֹͨ�еĵ�·id
};
//����ڵ��������Ϣ
struct ST_GEO_VSORT
{
	BYTE byStatusOfNormalSearch;	//������Ľڵ�ʹ��״̬.Vertex status:0 1 2 ,	0 ����δ�������,1 OPEN, 2 CLOSE
	BYTE byStatusOfReverseSearch;	//������Ľڵ�ʹ��״̬.Vertex status:0 1 2 ,0 ���� δ�������,1 OPEN,2 CLOSE
	POSITION posOHead;
	POSITION posOEnd;
};
//����·�ڽṹ��
struct ST_ROAD_PT
{
	ST_GEO_PTXY  pt;
	swString      Name;
};
//��·�����ݵĽ���·�ڽṹ
struct ST_ROAD_INNAME
{
	CGIS_FeatureRut *pRoute;
	swString InName;
};
struct ST_ROAD_CHOOSE
{
	POSITION pos;  
	int nMin; //��̾���
	ST_GEO_PTXY pt; //ƥ����λ��
};

///////////////////////////////////////////////////////////////////////////////
// ����Χ�ṹ(���ҡ�������)
typedef tagRect2T<float> ST_GEO_RECT;
/*struct ST_GEO_RECT{
	float		dLeft, dTop, dRight, dBottom;
};*/

///////////////////////////////////////////////////////////////////////////////
// ��·��Ϣ�ṹ��
struct ST_GEO_RUTINFO
{
	int			nID;
	BYTE		byDirection;			// ��˫�б��
	BYTE		byType;					// ��·���1-3������ɫ��ʾ ,ֻ��������ʾ����
    BYTE		wWidth;					// ·��.��
	BYTE		bySaturated;			// ·�α��Ͷ�
	BYTE		byLaneNum;				// ������
	BYTE		bySlope;				// �¶�
	BYTE		byMaterial;				// ·�����
	BYTE		byRteLevel;				// ��·�ȼ�
	DWORD		dwVehicle;				// ����ͨ������
	DWORD		dwInfo;					// ����ͨ������
	float		fLength;				// ��nStartPtID��nEndPtID����Ȩֵ����ʼ0����ͨΪ-1
	ST_GEO_RECT	rectObj;

	int			nVtxStartID;
	int			nVtxEndID;

	int			nPtNum;
	DWORD		dwNotePos;
};

///////////////////////////////////////////////////////////////////////////////
// ע����Ϣ�ṹ��
struct ST_GEO_ANNO
{
	swString	szAnno;
	int		nRectNum;
	RECT	*pRect;
	BYTE	byLevel; //��ʾ���ȼ�	
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
	swString strLong; // ��������������
	swString strShort; // �̾�����������
	swString strIntersec; // �ӽ�·��ʱ��������
};

struct ST_VOICE_NODE
{
	BYTE byAngle;		//����Ƕ����ͣ�1.ֱ�� 2.����3.����4.����5.U�͹գ�
	BYTE bySameName;	//�����··���Ƿ�һ�£�11����12����)
	BYTE byConfusion;	//�Ƿ���������·�λ��ҵ�·��(ֻ����ͬ��·��)(15�� 16�� 37�� 38��)
	BYTE byConfusion2;	//�Ƿ���������·�λ��ҵ�·��(15���,16������,37�Ҳ�)
	BYTE byOtherStreet;	//�Ƿ���Ҫ������ڵ�·���������·��21����22����)
	BYTE byIMI;			//�Ƿ����IMI(23����24����)
	BYTE byDestination;	//�Ƿ񵼵�Ŀ�ĵأ�25����26����)
	BYTE byTurn;		//���䷽��(27����28����)
	BYTE byRound;		//��ڵ�·����ڵ�·֮���Ƿ��л���(29����30����)
	BYTE bySameRoad;	//��ڵ�·�ͳ��ڵ�·�Ƿ�Ϊͬһ����·��31����32����)
	BYTE byExName;		//�Ƿ��������ڵ�·ͬ���ĵ�·������ڵ�·�⣩(RUT)17�� ��18�� ��
	BYTE byEnName;		//�Ƿ��������ڵ�·ͬ���ĵ�·�������ڵ�·�⣩(RUT)19�� ��20�� ��

    ST_VOICE_NODE()
    {
        Init();
    }
    ~ST_VOICE_NODE()
    {
    }
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////

public:
    BOOL Init()
    {
        byAngle             = 1;    //����Ƕ����ͣ�1.ֱ�� 2.����3.����4.����5.U�͹գ�
        bySameName          = 11;   //�����··���Ƿ�һ�£�11����12����)
        byConfusion         = 16;   //�Ƿ���ڻ���·��(15.��16.��37.��38.��)
        byConfusion2        = 16;   //�Ƿ���ڻ���·��(15.��16.��37.��38.��)
        byOtherStreet       = 21;   //�Ƿ���Ҫ������ڵ�·���������·��21����22����)
        byIMI               = 24;   //�Ƿ����IMI(23����24����)
        byDestination       = 25;   //�Ƿ񵼵�Ŀ�ĵأ�25����26����)
        byTurn              = 0;    //���䷽��(27����28����)
        byRound             = 30;   //��ڵ�·����ڵ�·֮���Ƿ��л���(29����30����)
        bySameRoad          = 31;   //��ڵ�·�ͳ��ڵ�·�Ƿ�Ϊͬһ����·��31����32����)
        byExName            = 18;   //�Ƿ��������ڵ�·ͬ���ĵ�·������ڵ�·�⣩(RUT)17�� ��18�� ��
        byEnName            = 20;   //�Ƿ��������ڵ�·ͬ���ĵ�·�������ڵ�·�⣩(RUT)19�� ��20�� ��

        return TRUE;
    }
};
struct ST_VOICE_RUT
{
	BYTE byType;		//��·����(RUT)(6�� ����·7�� �ѵ�8�� ��ͨ��·33�����34���ɿ�)
	BYTE byAdd;			//��·���͸�����(9�� �ǻ���10�� ���ǻ���)
	BYTE byDrivable;	//��·�Ƿ���Լݳ���ʻ(RUT)13�� ��14�� ��
	BYTE byCharge;		//�Ƿ�Ϊ�շ�·�Σ�RUT��35�� ��36�� ��
    BYTE bMakeVoice:1;  //��ǰ��node�Ƿ�Ҫ��������
    BYTE bHaveSign:1;     //�Ƿ��з��򿴰���Ϣ.

    ST_VOICE_RUT()
    {
        Init();
    }
	~ST_VOICE_RUT()
	{

	}
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////
public:
    BOOL Init()
    {
        memset(this,0,sizeof(ST_VOICE_RUT));
        bMakeVoice = 1;
        return TRUE;
    }
};
struct ST_FEATURE_POINT
{
	CGIS_Feature *pFeature;
	double dbDis;
};
//�ܿ�·����Ϣ
struct ST_AVOID_RUT
{
	unsigned short nMID;	//·������map��ID
		int nBID;				//·������ͼ���ı��ID
	int nRID;				//·�ε�ID
};
//������Ϣ
struct ST_TURN_INFO
{
	ST_TURN_INFO()
	{
		psPos = NULL;
		psEndMobilePos = NULL;
		byTDir = byTAng = 0;
		dbD2Pre = 0.0;
		strRName = strLVoice = strSVoice = _T("");
		strSign.Empty();
		bSeriMobile = FALSE;
		byRMobile = 0;
		byRMType = 0;
		byConfusion = 16;
		byConfusion2 = 16;
	}

//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////	

	POSITION psPos;		        //�������ں����ϵ�λ��
	POSITION psEndMobilePos;	//����������λ��(����/��������)
	BYTE	 byTDir;	        //�û�����ת�䷽��(1����ת 2����ת 3��ֱ�� 4.���� 5.Ŀ�ĵ�)
	BYTE	 byTAng;	        //1.ֱ�� 2.����3.����4.����5.U�͹�
	BYTE	 byConfusion;	    //��ʾ����·�ڵ�״̬(15���,16������,37�Ҳ�)
	BYTE	 byConfusion2;	    //��ʾת��ͬ��·�ڵ�״̬(15���,16������,37�Ҳ�)
	swString	 strRName;	        //������·����
	double	 dbD2Pre;	        //����ǰһ��������ľ���
	swString	 strLVoice;         // ������������Ϣ
	swString  strSVoice;	        // �̾���������Ϣ
	swString	 strIVoice;         // �ӽ�·��������Ϣ
    swString  strSign;           //���򿴰���Ϣ
	BOOL	 bSeriMobile;       //�Ƿ�Ϊ���������ĺ�����
	BYTE	 byRMobile;	        //�����ĸ���
	BYTE	 byRMType;	        //���������ͣ�0.���ǻ�����1.����Ĳ����ǵڼ������ڣ�2.�����������Ϣ��
};
//����POIͼ��
struct ST_PT_DRAW 
{
	int nx;
	int ny;
	int nIconID;
};
//��ͼ��ѡ�е�·�ķ���ֵ
struct ST_FOCUS_RUT
{
	swString strName;
	int nMapID;
	int nBandID;
	int nRutID;
};
//POI��ѯ�����ʾ�ڵ�ͼ�ϣ����⽹����ʾ
struct ST_FOCUS_PT
{
	swString strName;
	ST_GEO_PTXY ptXy;
	int nSymbalID;
};

struct RICHPOI_LEVEL2;
struct RICHPOI_LEVEL3;
//RICHPOIһ���б�����
struct RICHPOI_LEVEL1
{
	DWORD dwLevel1ID;			//һ�����ID
	swString strLevel1Name;		//һ���������
	//CPtrList pLevel2List;		//�������
	std::list<RICHPOI_LEVEL2*> pLevel2List;
	~RICHPOI_LEVEL1(){
		std::list<RICHPOI_LEVEL2*>::iterator itr;
		for(itr = pLevel2List.begin();itr!=pLevel2List.end();itr++){
			RICHPOI_LEVEL2* p = *itr;
			delete p;
		}
	}
};

//RICHPOI��������б�
struct RICHPOI_LEVEL2
{
	DWORD dwLevel2ID;			//�������ID
	swString strLevel2Name;		//�����������
	//CPtrList pLevel3List;		//�������
	std::list<RICHPOI_LEVEL3*> pLevel3List;
	~RICHPOI_LEVEL2(){
		std::list<RICHPOI_LEVEL3*>::iterator itr;
		for(itr = pLevel3List.begin();itr!=pLevel3List.end();itr++){
			RICHPOI_LEVEL3* p = *itr;
			delete p;
		}
	}
};

//RICHPOI��������б�
struct RICHPOI_LEVEL3
{
	DWORD dwLevel3ID;					//�������ID
	swString strLevel3Name;				//�����������
	DWORD dwDataPackageOffset;			//���ݰ�λ��ƫ��
	DWORD dwDataPackageCnt;				//���ݰ�����
};
//RICHPOI���ݽṹ��
struct RICHPOI_INFO
{
	swString strRichPoiName;				//RichPoi����
	DWORD dwRichPOIID;					//RichPoiID
	float fLon;							//Lon
	float fLat;							//Lat
	DWORD dwContentInfoOffset;			//����������Ϣƫ��λ��
	DWORD dwContentTxtLen;				//�����������ֳ���n
	DWORD dwContentTxtLenUZip;			//������������ѹ��ǰ����
	int iContentMediaCnt;				//Media�ļ�����
	BYTE dwAlarmSpeed;					//����ʱ��
	swString strPhoneCode;				//�绰����
};
//RICHPOI�ļ�ͷ
struct RICHPOI_FILEHEAD
{
	char chEcompassRichPOI[15];
	BYTE Type;
	BYTE FileCreateDate[3];
	BYTE DataUpdateVer;
	BYTE DataVer;
	char DataSource[32];
	BYTE ZipType[4];
	DWORD dwFileLen;
	BYTE EncryptField[4];
};///����ԭʼλ����Ϣ

struct ST_RICHPOI_BANDSET
{
	int nBandID;
	//CPtrList listRichPOI;
	std::list<RICHPOI_INFO> listRichPOI;
};

struct ST_SPACE_RICHPOI
{
	int nBandID;	//ͼ��ID
	int nOffset;	//�����������ƫ��λ��
	int nLevel3Num;	//�����������
};
//·������ͼ�Ĳ����ṹ
struct ST_EXPAND_MAPSET
{
	int nWidth;			//ͼƬ���	
	int nLength;		//ͼƬ����
	COLORREF colBack;	//ͼƬ������ɫ
	COLORREF colRoad;	//��·��ɫ
	COLORREF colRut;	//������ɫ
	COLORREF colMidLine;//��·��������ɫ
};

extern int MAP_ORIGIN_BID;
extern int MAP_ORIGIN_XNUM;
extern int MAP_ORIGIN_YNUM;
extern double MAP_ORIGIN_CENTERX;
extern double MAP_ORIGIN_CENTERY;
//static int MAP_ORIGIN_BID = 444624;
//static int MAP_ORIGIN_XNUM = 372;
//static int MAP_ORIGIN_YNUM = 354;
//static float MAP_ORIGIN_CENTERX = 106.563;
//static float MAP_ORIGIN_CENTERY = 29.5417;

//��ͼ����IDת��Ϊ��������ĵĸ���
void ParseBMapID( int nBMID, int &nBMXNum, int &nBMYNum );
//����ƫ�Ƶõ�ͼ����ID
int CalculateBMapID( int nBMXID, int nBMYID );
//���ݵ�λ�õõ�ͼ����ID
int CalculateBMapID( float dx, float dy );
//���ݵ�λ�õõ�ͼ����ID
void CalculateBMapIDInfo( float dx, float dy, int &nXNum, int &nYNum );
//���ݵ�λ�õõ�ͼ����ID
void CalculateBMapIDInRect( ST_GEO_RECT rt, int nBMIDs[3000], int &nBNum );
//�õ�ͼ���ľ��ο�
ST_GEO_RECT CalculateBMapRect( int nBMID );

//�����ߵ�buffer
void CreateRect(double d,ST_GEO_PTXY& pa,ST_GEO_PTXY& pb,ST_GEO_PTXY& pc,ST_GEO_PTXY& pd,ST_GEO_PTXY& pe,ST_GEO_PTXY& pf);
double AntiClock(ST_GEO_PTXY& center,ST_GEO_PTXY& outer);
double Clockwise(ST_GEO_PTXY& center,ST_GEO_PTXY& outer);
BOOL CalcLineBuff( double dbWidth, ST_GEO_PTXY *pPtList, int nPtNum, ST_GEO_PTXY pPtBuf[], int &nPtBuf );
double CalcArea( ST_GEO_PTXY pt1, ST_GEO_PTXY pt2, ST_GEO_PTXY pt3 );
// �ھ����б���ѡȡһ�����ľ������򷵻�
void	swEXPORT CalcRect(ST_GEO_PTXY* pPt, int nNum, ST_GEO_RECT& rect);
///���ڴ���ж�ȡ��������
size_t Fread(char *pFileBuf,DWORD dwSize,DWORD &dwStation,void *buffer, size_t size, size_t count);

/*/�жϵ�;��ο�Ĺ�ϵ(0~9)
	 1----2-----3
	 |			|
	 4	  5		6		0
	 |			|
	 7----8-----9
 */
BYTE	swEXPORT GetRelationPt2Rect( double dx, double dy, double dx1, double dy1, double dx2, double dy2);
void ParseBreadthMapInfo( char* szInfo, int &nIndex, int &nParent, ST_GEO_RECT &rtBreadth );
BOOL ExtractFilePath(char* szFileName,char* szPath);
char* getStrItemEx(char* szBuf,char szLim,int nPos,int &nLen);

// �淶��ָ���ľ���
void	NormalizeRect(ST_GEO_RECT &rect);
// �жϾ��ν��������0�޽�����1�н�����2��3������ϵ
int		swEXPORT GeoRectIntersect(ST_GEO_RECT *sourceRect, ST_GEO_RECT *refRect);
// �ж���֪һ���Ƿ�����������������ڣ���������ڲ�����TRUE
BOOL	swEXPORT IsPtInRect(double dx, double dy, double dx1, double dy1, double dx2, double dy2);
BOOL swEXPORT IsPtInScreen(int x,int y,int x1,int x2,int y1,int y2);

// ������߶��ϵ���ɢ������
BOOL	swEXPORT GetPointsOnLine(ST_GEO_PTXY* pLine, int nPtNum, int nPart, ST_GEO_PTXY* pPtList);
// ����һ���߶εĳ���
double	swEXPORT GetLength(double dx1, double dy1, double dx2, double dy2);
// �������������ܳ���
double	swEXPORT GetTotalLength(ST_GEO_PTXY *pPt, int nNum);
// ����б���ڻ������һ������
void	swEXPORT GetPointOnArc(double dx1, double dy1, double dx2, double dy2, 
					  double dRatio, double& xx, double& yy);
// �õ����о���
ST_GEO_PTXY* GetClipRgn(ST_GEO_PTXY* pPt, int nNum, ST_GEO_RECT &rect, 
						int *pnPtNum, int nObjType);
// �ж��������ڵľ����Ƿ��н����������ʾ�н���
BOOL	swEXPORT IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2);
// �����������Ľ���λ���������εĹ���������Ϊ�潻�㣬����TRUE
BOOL	swEXPORT GetIntersectedPoint(double X1, double Y1, double X2, double Y2, double X3, 
							double Y3, double X4, double Y4, double &XX, double &YY, int Flag);
//�õ�POI��С���
unsigned short  GetPoiID(unsigned short nId, UINT nLayerID);
//�����ַ�������
int     GetCharCode(swString strIn);

//////////////////////////////////////////////////////////////////////////
//�˽ṹΪ�����еĳ�����Ϣ
typedef struct 
{
	int  nLanNum;
	BYTE*  pbyLaneDir;
	BYTE*	pbyOutLaneDir;
}ST_LANDINFO;

#endif 
