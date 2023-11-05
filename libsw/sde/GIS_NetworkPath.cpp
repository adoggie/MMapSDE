
#include "stdafx.h"
#include "GIS_NetworkPath.h"

#include "SDEControler.h"
#include "GIS_Pathpoint.h"
#include "GIS_PathpointNode.h"
#include "GIS_PathpointMark.h"
#include "GIS_NetworkPathInfo.h"
#include "GIS_FeatureRut.h"
#include "IGIS_MapManager.h"
#include "GIS_QueryByShape.h"
#include "GIS_PathpointNodeV2.h"
#include "GIS_NetworkException.h"
#include "GIS_NetworkStatus.h"
//////////////////////////////////////////////////////////////////////////
#include "DebugFunctionLib/DebugFun.h"
/**
 * @brief   调试函数库(只在调试版本中被定义)
 *
 * 日志文件功能
 * 调试输出到屏幕功能
 * 函数调用跟踪功能
 *
 *    
 * @note    如果不想在调试版本中使能这些宏中的一项或几项,应该将这些宏在local文
 *          件中重新定义为空,即打开下面相应的注释即可.       
*/

//#define DBG2SCR0(sz)              
//#define DBG2SCR1(sz, p1)          
//#define DBG2SCR2(sz, p1, p2)      
//#define DBG2SCR3(sz, p1, p2, p3) 
//#define TRACK_FUNCTION(sz) 
//#define DBG2LOG0(sz)                
//#define DBG2LOG1(sz, p1)            
//#define DBG2LOG2(sz, p1, p2)        
//#define DBG2LOG3(sz, p1, p2, p3)    
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//定义在本文件内部的宏
#ifdef _DEBUG

#define TRACE_TURN_LIST_SCRIPT

#endif

#define DISTANCE_TIMESLICE_RECAL	3000 // 距离禁行路段3000米重新计算
#define DISTANCE_TIMESLICE_NOCAL	1000 // 开始导航后1公里内不做时间片禁行
#define SPEED_MOTOR_WAY	120*5/18.0	// 在高等级道路上的评估速度，单位 km/h
#define SPEED_MAJOR_WAY	60*5/18.0
#define SPEED_MINOR_WAY 40*5/18.0

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int FindIndexByPostionInPtrList(CPtrList* pList,POSITION posFind);

static BOOL ExitJudge()
{
	//////////////////////////////////////////////////////////////////////////
	//退出判定
	//TRACE0("航线规划线程进行退出判定.\n");
    if(CGIS_NetworkStatus::m_bCalculateStopSign)
	{
		TRACE0("退出标志置位,函数返回.\n");	
	}
	//////////////////////////////////////////////////////////////////////////

	return CGIS_NetworkStatus::m_bCalculateStopSign;
}

#define ROAD_NONAME_DEL		_T("驶入 %s") // 没有路名时截去部分

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkPath::CGIS_NetworkPath()
{
	m_pPathpointList = new CPtrList;
	m_pTurnList = new CPtrList;
	m_pTRList = new CPtrList;
	m_pVoiceExitRut = NULL;
	m_pVoiceEnterRut = NULL;
	m_pVoiceNode = NULL;
	m_posCurrent = NULL;
	m_posVoice = NULL;
	m_posVEPre = NULL;
	m_posVEnd = NULL;
	m_strLVoice = _T("");
	m_strSVoice = _T("");
	m_strIVoice = _T("");
	m_bSRut = FALSE;
	m_dbDis = 100000.0;
	m_byMType = 0;
	m_byRMNum = 0;
	m_bCTFlag = FALSE;
    m_pNodeList = NULL;
	m_bTipCal = false;
	memset(m_posLimit, 0, sizeof(m_posLimit));
	m_dbDisRoute = 0.0;
	m_posNodeCur = NULL;
}

CGIS_NetworkPath::~CGIS_NetworkPath()
{
	if( m_pPathpointList )
	{
		CGIS_Pathpoint *pPPoint;
		POSITION pos = m_pPathpointList->GetHeadPosition();
		while( pos )
		{
			pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetNext( pos );
			if( pPPoint )
				delete pPPoint;
		}
		m_pPathpointList->RemoveAll();
		delete m_pPathpointList;
	}
	if( m_pTurnList )
	{
		POSITION pos = m_pTurnList->GetHeadPosition();
		ST_TURN_INFO *pTurnInfo = NULL;
		while( pos )
		{
			pTurnInfo = (ST_TURN_INFO*)m_pTurnList->GetNext(pos);
			if( pTurnInfo )
				delete pTurnInfo;
		}
		m_pTurnList->RemoveAll();
		delete m_pTurnList;
	}
	if( m_pTRList )
	{
//		POSITION pos = m_pTRList->GetHeadPosition();
//		CGIS_FeatureRut *pRut = NULL;
//		while( pos )
//		{
//			pRut = (CGIS_FeatureRut*)m_pTRList->GetNext(pos);
//			if( pRut )
//				delete pRut;
//		}
		m_pTRList->RemoveAll();
		delete m_pTRList;
	}

    if(m_pNodeList)
    {
		POSITION pos = m_pNodeList->GetHeadPosition();
		CGIS_PathpointNodeV2* pNode = NULL;
		while( pos )
		{
			pNode = (CGIS_PathpointNodeV2*)m_pNodeList->GetNext(pos);
			ASSERT(pNode);
            delete pNode;
            pNode = NULL;
		}
		m_pNodeList->RemoveAll();
		delete m_pNodeList;
    }
	m_pPathpointList = NULL;
	m_pTurnList = NULL;
	m_pTRList = NULL;
	m_pVoiceExitRut = NULL;
	m_pVoiceEnterRut = NULL;
	m_pVoiceNode = NULL;
	m_posCurrent = NULL;
	m_posVoice = NULL;
	m_strLVoice = _T("");
	m_strSVoice = _T("");
	m_strIVoice = _T("");
}
CPtrList* CGIS_NetworkPath::GetPathpointList( )
{
	return m_pPathpointList;
}
void CGIS_NetworkPath::GetVoicePosition( POSITION &posCurr, POSITION &posVoice )
{
	posVoice = m_posVoice;
	posCurr = m_posCurrent;
}
BOOL CGIS_NetworkPath::IsInMobile( )
{
	return m_bInMobile;
}
void CGIS_NetworkPath::SetCTFlag( BOOL bCTFlag )
{
	m_bCTFlag = bCTFlag;
}
BOOL CGIS_NetworkPath::InitNavigate( )
{
	m_posCurrent = m_pPathpointList->GetHeadPosition();
	m_posVoice = m_posVEnd = m_posVEPre = m_pPathpointList->GetHeadPosition();
	m_pVoiceExitRut = ((CGIS_PathpointMark*)m_pPathpointList->GetHead())->GetVRutInfo();
	m_pVoiceEnterRut = NULL;
	m_pVoiceNode = NULL;
	m_strLVoice = _T("");
	m_strSVoice = _T("");
	m_strIVoice = _T("");
	m_bSRut = FALSE;
	m_dbDis = 100000.0;
	m_byMType = 0;
	m_byRMNum = 0;
	if( m_bCTFlag == FALSE )
    {
        int nErrorCode = CGIS_NetworkException::ERROR_NONE;
        CalcuatePathTurnList(&nErrorCode);
    }

	if( m_pTurnList->GetCount() == 0 )
		m_posTurn = NULL;
	else
		m_posTurn = m_pTurnList->GetHeadPosition();
	m_bInMobile = FALSE;
	m_byInMType = 0;
	m_dbDis2Dest = 0.0;

	m_bTipCal = false;
	memset(m_posLimit, 0, sizeof(m_posLimit));

	POSITION pos = m_pPathpointList->GetHeadPosition();
	m_dbDisRoute = 0.0;
	while (pos)
	{
		CGIS_Pathpoint *pPathPoint = NULL;
		pPathPoint = (CGIS_Pathpoint*)m_pPathpointList->GetNext(pos);
		m_dbDisRoute += pPathPoint->GetDis2Next();
	}
	if (m_pNodeList != NULL)
		m_posNodeCur = m_pNodeList->GetHeadPosition();
	else
		m_posNodeCur = NULL;
	return TRUE;
}

//========================================================================
// 处理连续机动，将两个机动信息合并为一个
//
// nOrd(in)：第几次进入，表示当前要合成的第几个机动，目前只允许连续两个机动
//
// pTurnInfo(in)：机动点信息，ntype(in)：语音类型，1-长距离 2-短距离 3-路口
//
// 返回值：部分文本语音
//
// 修改：1 连续机动前后两句的连接，用“、然后”分开 20070723
//
//		 2 如果没有路名，没有混乱则截去“驶入 %s”，有则取名“道路” 20070724
//========================================================================
swString CGIS_NetworkPath::RecombineTInfo(int nOrd, ST_TURN_INFO *pTurnInfo,
										 int ntype)
{
	ASSERT(pTurnInfo != NULL);
	swString strSeri = _T(""); // 保存连续机动文本，用于函数返回值
	swString strFormat; // 保存需要格式化的文本

	if(nOrd == 1)
	{
		FormatVoiceRoadName(pTurnInfo, ntype, strFormat);
	}
	else if(nOrd == 2)
	{
		strSeri = _T("、然后");
		FormatVoiceRoadName(pTurnInfo, 3, strFormat);
	}

	strSeri += strFormat;
	return strSeri;
}

const double MAX_SMMW_DISTANCE = 190.0;	//普通道路的连续机动距离
const double MAX_SMHW_DISTANCE = 500.0;	//告诉道路的连续机动距离
//处理TurnList里存在的连续机动
BOOL CGIS_NetworkPath::CheckTLSeriateMobile( CPtrList *pPPList, CPtrList *pTList )
{
	if( pTList->GetCount() < 1 )
		return FALSE;
	POSITION pos = pTList->GetHeadPosition();
	ST_TURN_INFO *pTurnInfo = NULL;
	ST_TURN_INFO *pTNext = NULL;
	double dbDis = 0.0;

//
////	CFile file;
//	swString strPath = _T("");
//	strPath.Format( _T("%s\\%s"), GetProPathName(_T("\\map")), _T("MInfo.txt") );
////	file.Open( _T("\\Storage Card\\MobileNavigator\\), CFile::modeCreate|CFile::modeWrite );
////	file.Open( strPath, CFile::modeCreate|CFile::modeWrite );
//	swString str;
//	char szInfo[128];
////	str = _T("/////////////////////////////////\x0d\x0a");
//	str = _T("/////////////////////////////////\n");
//	int nNum = UnicodeToChar( str, szInfo, 128 );
//	TRACE( str );
////	file.Write( szInfo, nNum );
////	str = _T("转弯方向(1、左转 2、右转 3、直行 4.环岛 5.目的地);\x0d\x0a");
//	str = _T("转弯方向(1、左转 2、右转 3、直行 4.环岛 5.目的地);\n");
//	nNum = UnicodeToChar( str, szInfo, 128 );
//	TRACE( str );
////	file.Write( szInfo, nNum );
////	str = _T("转弯角度(1.直行 2.缓拐3.拐弯4.急拐5.U型拐);\x0d\x0a");
//	str = _T("转弯角度(1.直行 2.缓拐3.拐弯4.急拐5.U型拐);\n");
//	nNum = UnicodeToChar( str, szInfo, 128 );
//	TRACE( str );
////	file.Write( szInfo, nNum );	
////	str.Format( _T("机动个数:%d;\x0d\x0a"), pTList->GetCount() );
//	str.Format( _T("机动个数:%d;\n"), pTList->GetCount() );
//	nNum = UnicodeToChar( str, szInfo, 128 );
//	TRACE( str );
////	file.Write( szInfo, nNum );
	while( pos )
	{
		pTurnInfo = (ST_TURN_INFO*)pTList->GetNext(pos);
		if( pTurnInfo )
		{
//			str.Format( _T("机动信息:%.2f米,%d,%d,%s\x0d\x0a"), pTurnInfo->dbD2Pre, pTurnInfo->byTDir, pTurnInfo->byTAng, pTurnInfo->strRName );
//			str.Format( _T("机动信息:%.2f米,%d,%d,%s\n"), pTurnInfo->dbD2Pre, pTurnInfo->byTDir, pTurnInfo->byTAng, pTurnInfo->strRName );
//			nNum = UnicodeToChar( str, szInfo, 128 );
//			TRACE( str );
//			file.Write( szInfo, nNum );	
			dbDis = MAX_SMMW_DISTANCE;
			CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPPList->GetAt(pTurnInfo->psPos);
			if( pPPoint )
			{
				if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
				{
					if( ((CGIS_PathpointNode*)pPPoint)->GetRType() == 0 )
						dbDis = MAX_SMHW_DISTANCE;
				}
			}
//			if( (pTurnInfo->dbD2Pre < dbDis  && pTurnInfo->byRMobile == 0) )
			if( pTurnInfo->dbD2Pre < dbDis+10.0 )
			{
				pTurnInfo->bSeriMobile = TRUE;
				pTNext = pTurnInfo;
			}
		}
	}
// 	file.Close();
	m_byAFlag = 0;
	return TRUE;
}
/**
 * @brief 计算机动列表
 *
 * @param 
 * @return
*/
BOOL CGIS_NetworkPath::CalcuatePathTurnList(int* pErrorCode )
{
    if(ExitJudge())
    {
        *pErrorCode = CGIS_NetworkException::ERROR_STOP_BY_OUTSIDE;
        return FALSE;
    }
    
    ASSERTVALID(this);

#ifdef _ANALYSE_PATH_TURN
    TRACK_FUNCTION("CalcuatePathTurnList()************");
    TRACE1("PathPointListCount[%d]\n",m_pPathpointList->GetCount());
#endif
    //////////////////////////////////////////////////////////////////////////
    {
        POSITION pos = m_pTurnList->GetHeadPosition();
        ST_TURN_INFO *pTurnInfo = NULL;
        while( pos )
        {
            pTurnInfo = (ST_TURN_INFO*)m_pTurnList->GetNext(pos);
            if( pTurnInfo )
                delete pTurnInfo;
        }
        m_pTurnList->RemoveAll();
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef _ANALYSE_PATH_TURN
    {

    }
#endif
    
    CGIS_Pathpoint *pPPoint = NULL;
	CGIS_Pathpoint *pPNext = NULL;

    POSITION pos = m_pPathpointList->GetHeadPosition();
	ST_TURN_INFO* pTurnInfo = new ST_TURN_INFO;
	if( !CalculateVoicePosition2( m_pPathpointList, pos, &pTurnInfo ) )
	{
		delete pTurnInfo;
		pTurnInfo = NULL;


		if( pos )
			pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetNext(pos);
		ASSERT( pPPoint );
		while( pos )
		{
			pPNext = (CGIS_Pathpoint*)m_pPathpointList->GetNext(pos);
            ASSERT(pPNext);
			if( pPNext )
			{
				pPPoint->CalcDis2Next( pPNext->GetPathpointPosition() );
				pPPoint = pPNext;
			}
		}
		if( pPNext )
			pPNext->CalcDis2Next( pPNext->GetPathpointPosition() );
		return TRUE;
	}
#ifdef _ANALYSE_PATH_TURN
    DUMPSTATE(pTurnInfo);
    TRACE_SLEEP_BY_TIMES(10);
#endif

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    int nIndexN = 0;
#endif
	BOOL bRMobile = FALSE;
	POSITION psStartVoice   = NULL;//使连续机动内的距离不作累加
	POSITION psEndVoice     = pTurnInfo->psPos;
	if( pTurnInfo->byRMobile > 0 )
	{
		psStartVoice = pTurnInfo->psPos;
		psEndVoice = pTurnInfo->psEndMobilePos;
	}

    pos = m_pPathpointList->GetHeadPosition();
	POSITION ps = NULL;
	if( pos )
		pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetNext(pos);
	ASSERT( pPPoint );
    while( pos )
    {
        ps = pos;

        pPNext = (CGIS_Pathpoint*)m_pPathpointList->GetNext(pos);
        ASSERT(pPNext);        
        pPPoint->CalcDis2Next( pPNext->GetPathpointPosition() );
       
        if( bRMobile == FALSE )
        {   
            //当前机动的距离为本机动与上机动间的点点距离之和
            pTurnInfo->dbD2Pre += pPPoint->GetDis2Next();
        }
        
        if( ps == psStartVoice )
            bRMobile = TRUE;

        if( ps == psEndVoice )
        {
            //当前点已经是语音点就追入机动
            m_pTurnList->AddTail(pTurnInfo);
            pTurnInfo = new ST_TURN_INFO;

            //-------------------------------------------------------------
            //下一个path点存在,则要继续计算机动,更新语音点pos.并重新开始类加机动距离
            if( pos )
            {
                //TRACE1("从第%d个path点处开始计算turn.\n",nIndexN);
                VERIFY(CalculateVoicePosition2( m_pPathpointList, pos, &pTurnInfo ));
#ifdef _ANALYSE_PATH_TURN
                DUMPSTATE(pTurnInfo);
                TRACE_SLEEP_BY_TIMES(10);
#endif
            }
            psEndVoice = pTurnInfo->psPos;
            bRMobile = FALSE;

            
            if( pTurnInfo->byRMobile > 0 )
            {
                //是环岛机动
                psStartVoice = pTurnInfo->psPos;
                psEndVoice = pTurnInfo->psEndMobilePos;//
            }
        }
        pPPoint = pPNext;
       
        
#ifdef _DEBUG
        nIndexN++;
#endif
        
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
	if( psEndVoice != m_pPathpointList->GetTailPosition() )
		delete pTurnInfo;
	pTurnInfo = NULL;
	if( pPNext )
		pPNext->CalcDis2Next( pPNext->GetPathpointPosition() );
	ASSERT( m_pTurnList->GetCount() >= 0 );

    //DUMPSTATE(this);
	CheckTLSeriateMobile( m_pPathpointList, m_pTurnList );
    //DUMPSTATE(this);
	m_bCTFlag = TRUE;

	return TRUE;
}
CPtrList* CGIS_NetworkPath::GetPathTurnList( )
{
	return m_pTurnList;
}
CPtrList *CGIS_NetworkPath::GetNavigateVTRInfo( ST_GEO_RECT &rtBuff )
{
	rtBuff = m_rtTRBuf;
	return m_pTRList;
}
BOOL CGIS_NetworkPath::GetNavigateVoiceInfo( BYTE &byMType,			//机动类型
							swString &strCRName,		//当前路名
							swString &strNRName,		//下一路名
							double &dbDisNav,		//离机动点距离
							double &dbAngleNav,		//与机动点方向
							double &dbDisDes,		//离终点距离
							double &dbAngleDes,		//与终点方向
							swString &strVLong,		// 远距离语音信息
							swString &strVShort,		// 近距离语音信息
							swString &strVInter,		// 路口语音信息
							swString &strRName,		//下个路名
							BYTE &byRMNum )		//环岛路口的个数
{
	byMType = m_byMType;
	strCRName = m_strCRName;
	strNRName = m_strNRName;
	dbDisNav = m_dbDisNav;
	dbAngleNav = m_dbAngleNav;
	dbDisDes = m_dbDisDes;
	dbAngleDes = m_dbAngleDes;
	strVLong = m_strVLong;
	strVShort = m_strVShort;
	strVInter = m_strVInter;
	strRName = m_strInfo;
	if( m_byRMNum >0 )
		byRMNum = m_byRMNum-1;
	else
		byRMNum = m_byRMNum;
	return TRUE;
}

//========================================================================
// 取得到下个航线节点的距离
//========================================================================
double CGIS_NetworkPath::GetDisToNextNode()
{
	return m_dbDisToNode;
}

BOOL CGIS_NetworkPath::GetNavigateVoice( ST_GEO_PTXY pt, BYTE byType )
{
	m_strVLong = _T("");
	m_strVShort = _T("");
	m_strVInter = _T("");
	m_strRName = _T("");
	m_dbDisNav = m_dbDisDes = m_dbAngleNav = 0.0;
	m_dbDisToNode = m_dbAngleDes = 0.0;
	if( m_pPathpointList->GetCount() <= 0 )
		return FALSE;
    
	// 判断即将到达终点
	CGIS_Pathpoint *pPPoint;
	POSITION pss = m_pPathpointList->GetTailPosition();
	POSITION psTail = pss;
	m_pPathpointList->GetPrev( pss );
	if( m_posCurrent == pss || m_posCurrent == psTail )
	{
		pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetTail();
		double Tempx = fabs(pt.dx-pPPoint->GetPathpointPosition().dx);
		double Tempy = fabs(pt.dy-pPPoint->GetPathpointPosition().dy);
		
		if( Tempx<FLOAT_DETA_XY && Tempy<FLOAT_DETA_XY )
			return TRUE;
		
		TRACE( L"最后一个点的位置：%f %f \n" , Tempx*1852*60 , Tempy*1852*60 );
	}
	
	BOOL bVoice = TRUE;
	BOOL bRCD2Dest = FALSE;
	POSITION posCurrent = m_posCurrent;

	// 计算当前位置，并判断是否需要计算新的语音
	if(CalculateCurrentPosition(pt, m_pPathpointList, m_posVoice, posCurrent, bVoice))
	{
		// 车辆在环岛内时，语音信息文本清空
		if (m_byInMType == 2)
		{
			m_strLVoice = _T("");
			m_strSVoice = _T("");
			m_strIVoice = _T("");
		}

		m_posCurrent = posCurrent;
		if( bVoice == FALSE )
		{
			m_dbDis2Dest = 0.0;
			bRCD2Dest = TRUE;
			if(CalculateVoicePosition3(m_pPathpointList, m_pTurnList, posCurrent, m_posTurn, m_posVoice))
				bVoice = TRUE;
			else
				bVoice = FALSE;
		}
	}
	else
		return FALSE;
	
	if( posCurrent == NULL || posCurrent == m_pPathpointList->GetTailPosition() || m_posVoice == NULL )
		return FALSE;

	POSITION pos = posCurrent;
	// 计算当前路名和下条路段名称
	CalculateRutNames( m_pPathpointList, pos, m_strCRName, m_strNRName );
	if( m_byInMType == 2 && m_strCRName.IsEmpty() )
		m_strCRName = _T("环岛");
	if( m_byAFlag == 1 && m_strNRName.IsEmpty() )
		m_strNRName = _T("目的地");

	pos = posCurrent;
	double dbDisToNextPath = 0.0; // 记录到下个路径点的距离
	// 两种情况下进入条件判断：1 普通状况下，未到达语音点，2 在环岛内
	if( (pos!=m_posVoice && m_posVoice!=NULL && m_posVEnd==NULL) ||
		(pos!=m_posVEnd && m_posVEnd!=NULL) )
	{
		bool bStopCal = false; // 停止计算到下个航线节点距离
		pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt( pos );
		
		if (!bStopCal && pPPoint->GetPathpointType()==EN_PATHPOINTTYPE_NODE)
		{
			if (fabs(pt.dx - pPPoint->GetPathpointPosition().dx) < FLOAT_ZERO &&
				fabs(pt.dy - pPPoint->GetPathpointPosition().dy) < FLOAT_ZERO)
				bStopCal = true;
		}
		
		m_pPathpointList->GetNext( pos );
		pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt( pos );
		if( pPPoint != NULL)
			dbDisToNextPath += CalcDist( pt.dx, pt.dy, pPPoint->GetPathpointPosition().dx,
									pPPoint->GetPathpointPosition().dy );
		m_dbDisNav += dbDisToNextPath;
		while( pos )
		{
			pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt( pos );
			// 计算到下个航线节点的距离
			if (!bStopCal && (pPPoint->GetPathpointType()==EN_PATHPOINTTYPE_NODE))
			{
				bStopCal = true;
				m_dbDisToNode = m_dbDisNav;
			}	

			if( pos == m_posVoice || pos == m_posVEnd )
				break;
			if( pPPoint != NULL)
				m_dbDisNav += pPPoint->GetDis2Next();
						
			m_pPathpointList->GetNext( pos );
		}
	}
	else
	{
		m_pPathpointList->GetNext( pos );
		pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt( pos );
		if(pPPoint != NULL)
			m_dbDis2Dest += CalcDist( pt.dx, pt.dy, pPPoint->GetPathpointPosition().dx,
										pPPoint->GetPathpointPosition().dy );
	}
	if( bRCD2Dest )
	{
		while( pos )
		{
			pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetNext( pos );
			if(pPPoint != NULL)
				m_dbDis2Dest += pPPoint->GetDis2Next();
		}
	}
	m_dbDisDes = m_dbDisNav + m_dbDis2Dest;
	pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetTail();
	m_dbAngleDes = CalcAngle( pPPoint->GetPathpointPosition().dx,
								pPPoint->GetPathpointPosition().dy, pt.dx, pt.dy );
	if( bVoice )
	{
		pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt(m_posVoice);
		m_dbAngleNav = CalcAngle( pPPoint->GetPathpointPosition().dx,
									pPPoint->GetPathpointPosition().dy, pt.dx, pt.dy );
		m_strVLong = m_strLVoice;
		m_strVShort = m_strSVoice;
		m_strVInter = m_strIVoice;
		m_strRName = m_strInfo;
	}
	else
		m_dbDisNav = -1;

	BOOL bRes = TRUE;
	if (m_dbDisRoute - m_dbDisDes > 1000)
	{
		CGIS_Pathpoint *pPoint = NULL;
		POSITION posPathList = m_posCurrent;
		pPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt(posPathList);
		if (fabs(pt.dx - pPoint->GetPathpointPosition().dx) > FLOAT_ZERO ||
			fabs(pt.dy - pPoint->GetPathpointPosition().dy) > FLOAT_ZERO)
		{
			m_pPathpointList->GetNext(posPathList);
		}
		else
		{
			dbDisToNextPath = 0.0;
		}
		double dbDisToCurNode = CalCurPosInNodeList(posPathList);
		if (-1 == dbDisToCurNode)
			return TRUE;
		dbDisToCurNode += dbDisToNextPath;
		
		bRes = TimeSliceLimit(m_pNodeList, dbDisToCurNode, m_posNodeCur);
	}
	return bRes;
}

//========================================================================
// 计算当前位置
//
// 前提：当前的车辆位置匹配到航线上
//
// 当前位置：指的是车辆在航线上的已经经过的上个路径点位置，目前认为，超过
//
//		路径点而未到下个路径点，或者在路径点上，认为当前位置在这个路径点上
//
// 语音：当经过一个语音点，需要计算语音
//
// 返回：TRUE为在点列表中能够找到当前位置
//
// 缺陷：由于算法只回溯了一个节点，因此当车辆退后两个节点以上时，功能失效
//
// 修改：1 注释掉清空语音信息的地方，因为可能由于缺陷而导致本函数失败
//========================================================================
BOOL CGIS_NetworkPath::CalculateCurrentPosition(
		ST_GEO_PTXY pt,				// [in]当前车辆经纬度
		CPtrList *pPathPointList,	// [in]路径点列表
		POSITION posVoice,			// [in]语音点在列表中的位置
		POSITION &posCurrent,		// [out]车辆在列表中的位置
		BOOL &bVoice )				// [out]是否需要计算下个语音点
{
	if( posCurrent==pPathPointList->GetTailPosition() || posCurrent==NULL )
		return FALSE;

	POSITION pos = posCurrent;
	CGIS_Pathpoint *pPathPointPrevious = NULL;
	CGIS_Pathpoint *pPathPointNext = NULL;
    //////////////////////////////////////////////////////////////////////////
    // 找到当前位置的前个node点
	if(pos != pPathPointList->GetHeadPosition())
    {
        CGIS_Pathpoint *pPathPoint = NULL;
        while( pos )
        {
			pPathPoint = (CGIS_Pathpoint*)pPathPointList->GetAt(pos);
			if( pPathPoint )
			{
				if( pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE ||
					pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_START || 
					pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE || 
					pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_END )
					break;
			}
			pPathPointList->GetPrev( pos );
		}
	}

	// 找到当前位置的后一个node点
	POSITION posNodeAfterCur = posCurrent;
	if (posNodeAfterCur != pPathPointList->GetTailPosition())
	{
		CGIS_Pathpoint *pPathPoint = NULL;
        while(posNodeAfterCur)
        {
			pPathPoint = (CGIS_Pathpoint*)pPathPointList->GetAt(posNodeAfterCur);
			if(pPathPoint != NULL)
			{
				if( pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE ||
					pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_START || 
					pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE || 
					pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_END )
					break;
			}
			pPathPointList->GetNext(posNodeAfterCur);
		}
	}
    //////////////////////////////////////////////////////////////////////////

	int nMaxNum = 102400;
	POSITION ps = posCurrent;
    CGIS_Pathpoint *pPathPoint = NULL;
	POSITION posTemp = NULL; // 保存临时值，可能的当前位置
	bool bOverCurpos = false; // 是否经过了上一次的当前位置

	for( int i = 0;	i < nMaxNum;	i++ )
	{
		if( pos == NULL )
			return FALSE;

		if( pos == posVoice || pos == m_posVEnd )
		{
			if( pos == m_posVEnd )
			{
				m_bInMobile = FALSE;
				m_byInMType = 0;
				bVoice = FALSE;
			}
			else if( pos == posVoice )
			{
				m_bInMobile = TRUE;
				//m_strLVoice = m_strSVoice = m_strIVoice = m_strInfo = _T("");
				m_byMType = 0;
				m_byRMNum = 0;
				if( m_byInMType == 3 )
					m_byInMType = 2;
				if( m_posVEnd == posVoice || m_posVEnd == NULL )
				{
					m_bInMobile = FALSE;
					m_byInMType = 0;
					bVoice = FALSE;
				}
			}
		}
        
		ps = pos;
		pPathPointPrevious = (CGIS_Pathpoint*)pPathPointList->GetNext( pos );
		if( pos == NULL)
			return FALSE;
		pPathPointNext = (CGIS_Pathpoint*)pPathPointList->GetAt( pos );
		if( pPathPointPrevious == NULL	||	pPathPointNext == NULL )
		{
			posCurrent = pPathPointList->GetTailPosition();
			return FALSE;
		}
		if (ps == posCurrent)
		{
			bOverCurpos = true;
		}
		if (ps == posNodeAfterCur)
		{
			bOverCurpos = true;
			if (posTemp != NULL)
			{
				posCurrent = posTemp;
				return TRUE;
			}
		}
		if( PointOnLine( pt, pPathPointPrevious->GetPathpointPosition(), pPathPointNext->GetPathpointPosition() ) )
		{		
			if( fabs(pt.dx-pPathPointNext->GetPathpointPosition().dx) < FLOAT_ZERO &&
				fabs(pt.dy-pPathPointNext->GetPathpointPosition().dy) < FLOAT_ZERO )
				posTemp = pos;
			else
				posTemp = ps;
			if (bOverCurpos)
			{
				posCurrent = posTemp;
				return TRUE;
			}
		}
	}
	return FALSE;
}

////计算下一个需要播放语音的位置
BOOL CGIS_NetworkPath::CalculateVoicePosition2( 
    CPtrList*       pPathPointList,
    POSITION        posCurrent, 
    ST_TURN_INFO**  pTurnInfo )
{
	ASSERT( pPathPointList->GetCount() > 1 );

#ifdef _ANALYSE_PATH_TURN   
    TRACE1("\n从第%d点进入",FindIndexByPostionInPtrList(pPathPointList,posCurrent));
    switch( ((CGIS_Pathpoint*)pPathPointList->GetAt(posCurrent))->GetPathpointType())
    {
    case EN_PATHPOINTTYPE_START:
        TRACE0("(起点)\n");
        break;
    case EN_PATHPOINTTYPE_INFLEXION:		
        TRACE0("(拐点)\n");
        break;
    case EN_PATHPOINTTYPE_NODE:		
        TRACE0("(NODE点)\n");
        break;
    case EN_PATHPOINTTYPE_INTERMEDIATE:
        TRACE0("(途经点)\n");
        break;
    case EN_PATHPOINTTYPE_END:			
        TRACE0("(终点)\n");
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    TRACE_SLEEP_BY_TIMES(10);
#endif	
    
    
    swString strRName = _T("");
	POSITION pos = posCurrent;
	ST_VOICE_INDEX viInfo;
	CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPathPointList->GetAt( pos );

    
	if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_START )
	{
        m_pVoiceNode = NULL;
		m_pVoiceEnterRut = NULL;
		m_pVoiceExitRut = ((CGIS_PathpointMark*)pPPoint)->GetVRutInfo();
		if( pPathPointList->GetCount() <= 2 )
			m_pVoiceEnterRut = ((CGIS_PathpointMark*)pPPoint)->GetVRutInfo();
	}
	else if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END )
	{
		m_pVoiceEnterRut = m_pVoiceExitRut;
		m_pVoiceNode = ((CGIS_PathpointMark*)pPPoint)->GetVNodeInfo();
		m_pVoiceExitRut = ((CGIS_PathpointMark*)pPPoint)->GetVRutInfo();
		
        if( CGIS_NetworkPathInfo::GetMoibleVoiceIndex( m_pVoiceNode, m_pVoiceEnterRut, m_pVoiceExitRut, viInfo ) )
		{
			CGIS_NetworkPathInfo::GetVoiceString(viInfo, (*pTurnInfo)->strLVoice,
						(*pTurnInfo)->strSVoice, (*pTurnInfo)->strIVoice);

			(*pTurnInfo)->byTAng = m_pVoiceNode->byAngle;
			if( m_pVoiceNode->byDestination == 25 && 
				(viInfo.strLong == _T("24-1-1") ||
				 viInfo.strLong == _T("24-1-2")) && 
				(viInfo.strShort == _T("24-2-1") ||
				 viInfo.strShort == _T("24-2-2")) &&
				(viInfo.strIntersec == _T("24-3-1") ||
				 viInfo.strIntersec == _T("24-3-2")))
			{
				(*pTurnInfo)->byTDir = 5;
			}
			else
				(*pTurnInfo)->byTDir = m_pVoiceNode->byTurn-26;
			CharToUnicode( ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation() , &((*pTurnInfo)->strRName) );
			(*pTurnInfo)->psPos = pPathPointList->GetTailPosition();
			(*pTurnInfo)->byConfusion = m_pVoiceNode->byConfusion;
			(*pTurnInfo)->byConfusion2 = m_pVoiceNode->byConfusion2;
			return TRUE;
		}
		return FALSE;
	}
    else
    {
        
    }
	

    //////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    int nIndexN = 0;
#endif
	int nCount = 0;
	POSITION ps = pos;
	while( pos )
	{
		short nRMOrd = 0;
		ps = pos;
		pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt( pos );
        ASSERT(pPPoint);
        
        if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
        {
#ifdef _ANALYSE_PATH_TURN
            TRACE2("遍历到NODE点(%d),是当前的第%d个Path点.\n"\
                ,FindIndexByPostionInPtrList(pPathPointList,pos),nIndexN);
            TRACE2("\t%f,%f\n",pPPoint->GetPathpointPosition().dx,pPPoint->GetPathpointPosition().dy );
#endif

            m_pVoiceEnterRut = m_pVoiceExitRut;
            m_pVoiceNode = ((CGIS_PathpointNode*)pPPoint)->GetVNodeInfo();
            m_pVoiceExitRut = ((CGIS_PathpointNode*)pPPoint)->GetVRutInfo();
    
            
            //环岛处理
            if( m_pVoiceExitRut->byAdd == 9 || m_pVoiceNode->byRound == 29 )
            {
                nRMOrd = ((CGIS_PathpointNode*)pPPoint)->GetRMOrder();
#ifdef _ANALYSE_PATH_TURN
                TRACE1("当前语音机动是一个环岛.出口为%d.\n",nRMOrd);
#endif

                POSITION pss = pos;
                m_pPathpointList->GetNext(pss);
                while( pss )
                {
                    pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt( pss );
                    ASSERT(pPPoint);
                    if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
                    {
                        nCount++;
                        
                        m_pVoiceNode = ((CGIS_PathpointNode*)pPPoint)->GetVNodeInfo();
                        m_pVoiceExitRut = ((CGIS_PathpointNode*)pPPoint)->GetVRutInfo();
                        
                        //取大的环岛出口
                        if( ((CGIS_PathpointNode*)pPPoint)->GetRMOrder() > nRMOrd )
                        {
#ifdef _ANALYSE_PATH_TURN
                            TRACE2("发现更大的环岛出口(%d > %d)\n",((CGIS_PathpointNode*)pPPoint)->GetRMOrder(),nRMOrd);
                            DUMPSTATE(pPPoint);
#endif
                            nRMOrd = ((CGIS_PathpointNode*)pPPoint)->GetRMOrder();
                        }
                        
                        //如果下一条路段不是环岛路段或者已到了目的地,那么本次机动结束
                        if( m_pVoiceExitRut->byAdd == 10 || m_pVoiceNode->byDestination == 25 )
                        {
                            (*pTurnInfo)->psEndMobilePos = pss;
                            
                            pos = pss;
                            if( ((CGIS_PathpointNode*)pPPoint)->GetRutAnno( ) )
                                CharToUnicode( ((CGIS_PathpointNode*)pPPoint)->GetRutAnno( ) , &(*pTurnInfo)->strRName );
                            //										(*pTurnInfo)->strRName = ((CGIS_PathpointNode*)pPPoint)->GetRutAnno( );
                            else
                                (*pTurnInfo)->strRName = L"";
                            break;
                        }
                    }
                    else if( 
                        pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE
                        || 
                        pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END )
                    {
                        nCount++;
                        m_pVoiceNode = ((CGIS_PathpointMark*)pPPoint)->GetVNodeInfo();
                        m_pVoiceExitRut = ((CGIS_PathpointMark*)pPPoint)->GetVRutInfo();
                        //								if( ((CGIS_PathpointMark*)pPPoint)->GetRMOrder() > nRMOrd )
                        //									nRMOrd = ((CGIS_PathpointMark*)pPPoint)->GetRMOrder();
                        if( m_pVoiceExitRut->byAdd == 10 || m_pVoiceNode->byDestination == 25 )
                        {
                            (*pTurnInfo)->psEndMobilePos = pss;
                            CharToUnicode( ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation() , &((*pTurnInfo)->strRName) );
                            //									(*pTurnInfo)->strRName = ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation();
                            pos = pss;
                            break;
                        }
                    }                    
                    m_pPathpointList->GetNext(pss);
                }
            }
            
            if( nCount > 0 )
            {
                ST_VOICE_NODE *pVNode = new ST_VOICE_NODE;
                memcpy( pVNode, m_pVoiceNode, sizeof(ST_VOICE_NODE) );
                pVNode->byAngle = 6;	//环岛
                pVNode->byRound = 29;
                m_pVoiceNode = pVNode;
            }
            else
            {
                if( ((CGIS_PathpointNode*)pPPoint)->GetRutAnno( ) )
                    CharToUnicode( ((CGIS_PathpointNode*)pPPoint)->GetRutAnno( ) , &(*pTurnInfo)->strRName );
                //						(*pTurnInfo)->strRName = ((CGIS_PathpointNode*)pPPoint)->GetRutAnno( );
                else
                    (*pTurnInfo)->strRName = L"";
            }

            BOOL bMatch = CGIS_NetworkPathInfo::GetMoibleVoiceIndex( m_pVoiceNode, m_pVoiceEnterRut, m_pVoiceExitRut, viInfo );

            if( bMatch )
            {
                CGIS_NetworkPathInfo::GetVoiceString(viInfo,
										(*pTurnInfo)->strLVoice,
										(*pTurnInfo)->strSVoice,
										(*pTurnInfo)->strIVoice);

                if(m_pVoiceExitRut->bHaveSign)
                {
                    CGIS_PathpointNode* pNode = (CGIS_PathpointNode*)pPPoint;
                    CGIS_FeatureRut::SIGNINFO* pSign = pNode->GetSignInfo();
                    ASSERT(pSign);
                    ASSERT(pSign->byNameLen > 0);
                    ASSERT(pSign->pszName);
					CharToUnicode(pSign->pszName,&(*pTurnInfo)->strSign);

                }

                //填充转弯方向
                (*pTurnInfo)->byTAng = m_pVoiceNode->byAngle;
                if( (*pTurnInfo)->byTAng == 1 )
                    (*pTurnInfo)->byTDir = 3;
                else
                    (*pTurnInfo)->byTDir = m_pVoiceNode->byTurn-26;

                //机动点在pathlist中的位置
                (*pTurnInfo)->psPos = pos;
                (*pTurnInfo)->byConfusion = m_pVoiceNode->byConfusion;
                (*pTurnInfo)->byConfusion2 = m_pVoiceNode->byConfusion2;
                
                BYTE byRMType = 0;
                if( viInfo.strLong		== _T("15-1-1") ||
					viInfo.strShort		== _T("15-2-1") ||
					viInfo.strIntersec	== _T("15-3-1") ||
                    viInfo.strLong		== _T("16-1-1") ||
					viInfo.strShort		== _T("16-2-1") ||
					viInfo.strIntersec	== _T("16-3-1") )
				{
                    byRMType = 1;
				}
                else if( viInfo.strLong		== _T("17-1-1") ||
						 viInfo.strShort	== _T("17-2-1") ||
						 viInfo.strIntersec	== _T("17-3-1") ||
						 viInfo.strLong		== _T("18-1-1") ||
						 viInfo.strShort	== _T("18-2-1") ||
						 viInfo.strIntersec	== _T("18-3-1") )
				{
                    byRMType = 2;
				}
                if( nCount>0 || byRMType>0 )
                {
                    (*pTurnInfo)->byTDir = 4;
                    (*pTurnInfo)->byRMType = byRMType;
                    (*pTurnInfo)->byRMobile = nRMOrd+1;
                    (*pTurnInfo)->psPos = ps;
                    if( nCount > 0 )
                    {
                        delete m_pVoiceNode;
                        m_pVoiceNode = NULL;
                    }
                    else
                        (*pTurnInfo)->psEndMobilePos = ps;
                    nCount = 0;
                }
				
                if(	(viInfo.strLong == _T("24-1-1") ||
					 viInfo.strLong == _T("24-1-2")) && 
                    (viInfo.strShort == _T("24-2-1") ||
					 viInfo.strShort == _T("24-2-2")) &&
					(viInfo.strIntersec == _T("24-3-1") ||
					 viInfo.strIntersec == _T("24-3-2")))
                {
                    CGIS_Pathpoint *pPCur = NULL;
                    POSITION posCur = pos;
                    while( posCur )
                    {
                        pPCur = (CGIS_Pathpoint*)m_pPathpointList->GetAt(posCur);
                        if( pPCur )
                        {
                            if( pPCur->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE || 
                                pPCur->GetPathpointType() == EN_PATHPOINTTYPE_END )
                            {
                                (*pTurnInfo)->psPos = posCur;
                                (*pTurnInfo)->byTDir = 5;
                                break;
                            }
                        }
                        m_pPathpointList->GetNext(posCur);
                    }
                }
                return TRUE;
            }
            else
            {
                
                //ASSERT(FALSE);//不可能找不到匹配的语音规则
                if( nCount > 0 )
                {
                    nCount = 0;
                    delete m_pVoiceNode;
                    m_pVoiceNode = NULL;
                }
            }
        }
        else if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE 
            || pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END )
        {
#ifdef _DEBUG
//            TRACE2("遍历到遍历到途中点/终点(%d),是当前的第%d个Path点.\n"\
//                ,FindIndexByPostionInPtrList(pPathPointList,pos),nIndexN);
//            TRACE2("\t%f,%f\n",pPPoint->GetPathpointPosition().dx,pPPoint->GetPathpointPosition().dy );
#endif
            m_pVoiceEnterRut = m_pVoiceExitRut;
            m_pVoiceNode = ((CGIS_PathpointMark*)pPPoint)->GetVNodeInfo();
            m_pVoiceExitRut = ((CGIS_PathpointMark*)pPPoint)->GetVRutInfo();
            if( m_pVoiceExitRut->byAdd == 9 || m_pVoiceNode->byRound == 29 )
            {
                POSITION pss = pos;
                m_pPathpointList->GetNext(pss);
                while( pss )
                {
                    pPPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt( pss );
                    if( pPPoint )
                    {
                        if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
                        {
                            nCount++;
                            m_pVoiceNode = ((CGIS_PathpointNode*)pPPoint)->GetVNodeInfo();
                            m_pVoiceExitRut = ((CGIS_PathpointNode*)pPPoint)->GetVRutInfo();
                            if( ((CGIS_PathpointNode*)pPPoint)->GetRMOrder() > nRMOrd )
                                nRMOrd = ((CGIS_PathpointNode*)pPPoint)->GetRMOrder();
                            if( m_pVoiceExitRut->byAdd == 10 || m_pVoiceNode->byDestination == 25 )
                            {
                                (*pTurnInfo)->psEndMobilePos = pss;
                                if( ((CGIS_PathpointNode*)pPPoint)->GetRutAnno( ) )
                                    CharToUnicode( ((CGIS_PathpointNode*)pPPoint)->GetRutAnno( ) , &(*pTurnInfo)->strRName );
                                //										(*pTurnInfo)->strRName = ((CGIS_PathpointNode*)pPPoint)->GetRutAnno( );
                                else
                                    (*pTurnInfo)->strRName = L"";
                                pos = pss;
                                break;
                            }
                        }
                        else if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE || pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END )
                        {
                            nCount++;
                            m_pVoiceNode = ((CGIS_PathpointMark*)pPPoint)->GetVNodeInfo();
                            m_pVoiceExitRut = ((CGIS_PathpointMark*)pPPoint)->GetVRutInfo();
                            //								if( ((CGIS_PathpointNode*)pPPoint)->GetRMOrder() > nRMOrd )
                            //									nRMOrd = ((CGIS_PathpointNode*)pPPoint)->GetRMOrder();
                            if( m_pVoiceExitRut->byAdd == 10 || m_pVoiceNode->byDestination == 25 )
                            {
                                (*pTurnInfo)->psEndMobilePos = pss;
                                CharToUnicode( ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation() , &(*pTurnInfo)->strRName );
                                //									(*pTurnInfo)->strRName = ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation();
                                pos = pss;
                                break;
                            }
                        }
                    }
                    m_pPathpointList->GetNext(pss);
                }
            }
            if( nCount > 0 )
            {
                ST_VOICE_NODE *pVNode = new ST_VOICE_NODE;
                memcpy( pVNode, m_pVoiceNode, sizeof(ST_VOICE_NODE) );
                pVNode->byAngle = 6;	//环岛
                pVNode->byRound = 29;
                m_pVoiceNode = pVNode;
            }
            else
                CharToUnicode( ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation() , &(*pTurnInfo)->strRName );

            if( CGIS_NetworkPathInfo::GetMoibleVoiceIndex( m_pVoiceNode, m_pVoiceEnterRut, m_pVoiceExitRut, viInfo ) )
            {
                CGIS_NetworkPathInfo::GetVoiceString(viInfo,
										(*pTurnInfo)->strLVoice,
										(*pTurnInfo)->strSVoice,
										(*pTurnInfo)->strIVoice);
				
                (*pTurnInfo)->byTAng = m_pVoiceNode->byAngle;
                if( (*pTurnInfo)->byTAng == 1 )
                    (*pTurnInfo)->byTDir = 3;
                else
                    (*pTurnInfo)->byTDir = m_pVoiceNode->byTurn-26;
                (*pTurnInfo)->psPos = pos;
                (*pTurnInfo)->byConfusion = m_pVoiceNode->byConfusion;
                (*pTurnInfo)->byConfusion2 = m_pVoiceNode->byConfusion2;
                BYTE byRMType = 0;

                if( viInfo.strLong		== _T("15-1-1") ||
					viInfo.strShort		== _T("15-2-1") ||
					viInfo.strIntersec	== _T("15-3-1") ||
                    viInfo.strLong		== _T("16-1-1") ||
					viInfo.strShort		== _T("16-2-1") ||
					viInfo.strIntersec	== _T("16-3-1") )
				{
                    byRMType = 1;
				}
                else if( viInfo.strLong		== _T("17-1-1") ||
						 viInfo.strShort	== _T("17-2-1") ||
						 viInfo.strIntersec	== _T("17-3-1") ||
						 viInfo.strLong		== _T("18-1-1") ||
						 viInfo.strShort	== _T("18-2-1") ||
						 viInfo.strIntersec	== _T("18-3-1") )
				{
                    byRMType = 2;
				}
                if( nCount > 0 || byRMType>0 )
                {
                    (*pTurnInfo)->byTDir = 4;
                    (*pTurnInfo)->byRMType = byRMType;
                    (*pTurnInfo)->byRMobile = nRMOrd+1;
                    (*pTurnInfo)->psPos = ps;
                    if( nCount > 0 )
                    {
                        delete m_pVoiceNode;
                        m_pVoiceNode = NULL;
                    }
                    else
                        (*pTurnInfo)->psEndMobilePos = ps;
                    nCount = 0;
                }
                if( (viInfo.strLong == _T("24-1-1") ||
					 viInfo.strLong == _T("24-1-2")) && 
                    (viInfo.strShort == _T("24-2-1") ||
					 viInfo.strShort == _T("24-2-2")) &&
					(viInfo.strIntersec == _T("24-3-1") ||
					 viInfo.strIntersec == _T("24-3-2")))
                {
                    CGIS_Pathpoint *pPCur = NULL;
                    POSITION posCur = pos;
                    while( posCur )
                    {
                        pPCur = (CGIS_Pathpoint*)m_pPathpointList->GetAt(posCur);
                        if( pPCur )
                        {
                            if( pPCur->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE || 
                                pPCur->GetPathpointType() == EN_PATHPOINTTYPE_END )
                            {
                                (*pTurnInfo)->psPos = posCur;
                                (*pTurnInfo)->byTDir = 5;
                                break;
                            }
                        }
                        m_pPathpointList->GetNext(posCur);
                    }
                }
                return TRUE;
            }
            else
            {
                if( nCount > 0 )
                {
                    nCount = 0;
                    delete m_pVoiceNode;
                    m_pVoiceNode = NULL;
                }
            }
        }


#ifdef _DEBUG
        nIndexN++;//这个变量只是为了统计序数
#endif        
        m_pPathpointList->GetNext( pos );
            
    }
    return FALSE;
}

//========================================================================
// 计算下一个需要播放语音的位置
//========================================================================
BOOL CGIS_NetworkPath::CalculateVoicePosition3(CPtrList *pPPList,
											   CPtrList *pTList,
											   POSITION posCurrent,
											   POSITION &posTurn,
											   POSITION &posVoice)
{
	m_byAFlag = 0;
	m_byInMType = 0;
	if(posTurn == NULL)
	{
		m_posVoice = pPPList->GetTailPosition();
		m_posVEnd = NULL;
		return FALSE;
	}
	ST_TURN_INFO *pTurnInfo = NULL;
	if( pTList->GetCount() < 2 )
	{
		pTurnInfo = (ST_TURN_INFO*)pTList->GetAt(posTurn);
		if( pTurnInfo )
		{
			m_strInfo = pTurnInfo->strRName;
			m_strLVoice = pTurnInfo->strLVoice;
			m_strSVoice = pTurnInfo->strSVoice;
			m_strIVoice = pTurnInfo->strIVoice;
			m_byMType = pTurnInfo->byRMType;
			m_byRMNum = pTurnInfo->byRMobile;
			posVoice = pTurnInfo->psPos;
			m_posVEnd = pTurnInfo->psEndMobilePos;
			if( pTurnInfo->strLVoice.Find( _T("目的地") ) >= 0 )
				m_byAFlag = 1;
			if( pTurnInfo->byRMobile > 0 )
				m_byInMType = 3;
			CalculateVTRList( m_pPathpointList, pTurnInfo );
		}
		return TRUE;
	}
	
	ASSERT( pTList->GetCount() >= 2 );
	ST_TURN_INFO *pTNext = NULL;
	pTurnInfo = (ST_TURN_INFO*)pTList->GetAt(posTurn);
	POSITION pos = m_posVEPre;
	while( pos )
	{
		if( pos == pTurnInfo->psPos )
		{
			m_posVEPre = pos;
			pTList->GetNext(posTurn);
			if( posTurn )
				pTurnInfo = (ST_TURN_INFO*)pTList->GetAt(posTurn);
		}
		if( pos == posCurrent )
		{
			int nCount = 0;
			POSITION psTurn = posTurn;
			if( psTurn == NULL )
			{
				m_strLVoice = m_strSVoice = m_strIVoice = m_strInfo = _T("");
				m_byMType = 0;
				m_byRMNum = 0;
				m_bInMobile = FALSE;
				return TRUE;
			}
			ST_TURN_INFO *pTStart = (ST_TURN_INFO*)pTList->GetAt(psTurn);
			if( pTStart->bSeriMobile )
			{
				m_bInMobile = TRUE;
				m_byInMType = 1;
				if( pTStart->byRMobile > 0 )
					m_byInMType = 3;
			}
			else
			{
				m_byInMType = 0;
				m_bInMobile = FALSE;
				if( pTStart->byRMobile > 0 )
					m_byInMType = 3;
			}

			BOOL bDest = FALSE;
			pTNext = (ST_TURN_INFO*)pTList->GetNext(psTurn);
			posVoice = pTNext->psPos;
			m_posVEnd = pTNext->psEndMobilePos;
			if( pTNext->byTDir == 5 )
				bDest = TRUE;
			nCount++;
			
			m_strLVoice = RecombineTInfo(nCount, pTNext, 1);
			m_strSVoice = RecombineTInfo(nCount, pTNext, 2);
			m_strIVoice = RecombineTInfo(nCount, pTNext, 3);

			if( psTurn )
			{
				ST_TURN_INFO *pTTemp = (ST_TURN_INFO*)pTList->GetAt(psTurn);
				if( pTTemp->bSeriMobile )
				{
					while( psTurn )
					{
						pTNext = (ST_TURN_INFO*)pTList->GetAt(psTurn);
						if( pTNext )
						{
							if( pTNext->bSeriMobile )
							{
								if( pTNext->byTDir == 5 )
									bDest = TRUE;
								nCount++;
								swString str = RecombineTInfo(nCount, pTNext, 3);
								m_strLVoice += str;
								m_strSVoice += str;
								m_strIVoice += str;
								if( nCount >= 2 )
									break;
							}
							else
								break;
						}
						pTList->GetNext(psTurn);
					}
				}
			}
			ASSERT( pTNext );
			if( nCount > 1 )
			{
				m_strInfo = pTNext->strRName;
				m_byMType = 0;
				m_byRMNum = 0;
				if( pTNext->strLVoice.Find( _T("目的地") ) >= 0 )
					m_byAFlag = 1;
				CalculateVTRList( m_pPathpointList, pTStart );
			}
			else
			{
				m_strInfo = pTNext->strRName;
				FormatVoiceRoadName(pTNext, 1, m_strLVoice);
				FormatVoiceRoadName(pTNext, 2, m_strSVoice);
				FormatVoiceRoadName(pTNext, 3, m_strIVoice);
				m_byMType = pTNext->byRMType;
				m_byRMNum = pTNext->byRMobile;
				if( pTNext->strLVoice.Find( _T("目的地") ) >= 0 )
					m_byAFlag = 1;
				CalculateVTRList( m_pPathpointList, pTStart );
			}
			return TRUE;
		}
		pPPList->GetNext(pos);
	}
	return TRUE;
}

//计算相关路名（当前、下一）
BOOL CGIS_NetworkPath::CalculateRutNames( CPtrList *pPathPointList, POSITION posCurrent, swString &strCRName, swString &strNRName )
{
	strCRName = "";
	strNRName = "";
	POSITION pos = posCurrent;
	CGIS_Pathpoint *pPPoint = NULL;
	while( pos )
	{
		pPPoint = (CGIS_Pathpoint*)pPathPointList->GetPrev(pos);
		if( pPPoint )
		{
			if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
			{
				CharToUnicode( ((CGIS_PathpointNode*)pPPoint)->GetRutAnno() , &strCRName );
//				strCRName = ((CGIS_PathpointNode*)pPPoint)->GetRutAnno();
				break;
			}
			else if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END || 
				pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE )
			{
				CharToUnicode( ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation() , &strCRName );
//				strCRName = ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation();
				break;
			}
			else if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_START )
			{
				strCRName = ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPRutName();
				break;
			}
		}
	}
	if( pos == NULL && strCRName.IsEmpty() )
	{
		pos = posCurrent;
		pPPoint = (CGIS_Pathpoint*)pPathPointList->GetAt(pos);
		if( pPPoint )
		{
			if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
				CharToUnicode( ((CGIS_PathpointNode*)pPPoint)->GetRutAnno() , &strCRName );
//				strCRName = ((CGIS_PathpointNode*)pPPoint)->GetRutAnno();
		}
	}
//	if( m_posVEnd )
//		pos = m_posVEnd;
//	else
//		pos = m_posVoice;
	pos = m_posVoice;
	if( pos )
	{
		pPPoint = (CGIS_Pathpoint*)pPathPointList->GetAt(pos);
		if( pPPoint )
		{
			if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
				CharToUnicode( ((CGIS_PathpointNode*)pPPoint)->GetRutAnno() , &strNRName );
//				strNRName = ((CGIS_PathpointNode*)pPPoint)->GetRutAnno();
			else if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END || 
				pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE )
				CharToUnicode( ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation() , &strNRName );
//				strNRName = ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPAnnoation();
			else if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_START )
				strNRName = ((CGIS_PathpointMark*)pPPoint)->GetWayPoint()->GetWPRutName();
		}
	}
	if( pos == NULL && strNRName.IsEmpty() )
		strNRName = L"目的地";
	return TRUE;
}
BOOL CGIS_NetworkPath::CalculateVTRList( CPtrList *pPPList, ST_TURN_INFO *pTurnInfo )
{
	ASSERT( pTurnInfo );
	m_pTRList->RemoveAll();
	CGIS_Pathpoint *pPPStart = NULL;
	CGIS_Pathpoint *pPPEnd = NULL;
	if( pTurnInfo->psPos )
		pPPStart = (CGIS_Pathpoint*)pPPList->GetAt(pTurnInfo->psPos);
	pPPEnd = pPPStart;
	if( pTurnInfo->psEndMobilePos )
		pPPEnd = (CGIS_Pathpoint*)pPPList->GetAt(pTurnInfo->psEndMobilePos);
	CGIS_Pathpoint *pPPoint = NULL;
	POSITION pos = pTurnInfo->psPos;
	while( pos )
	{
		pPPoint = (CGIS_Pathpoint*)pPPList->GetAt( pos );
		if( pPPoint )
		{
			if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
			{
				m_rtTRBuf = ((CGIS_PathpointNode*)pPPoint)->GetCRect( );
				m_pTRList->AddTail( ((CGIS_PathpointNode*)pPPoint)->GetNodeRutList() );
			}
		}
		if( pos == pTurnInfo->psPos || pTurnInfo->psEndMobilePos == NULL )
			break;
		pPPList->GetNext( pos );
	}
	return TRUE;
}
void CGIS_NetworkPath::GetTLPosition( POSITION &posTurn )
{
	posTurn = m_posTurn;
}


//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_NetworkPath::DumpState() const	
{  
    TRACE0("********************************************************************\n");
    TRACE0("CGIS_NetworkPath::DumpState()\n");
    //dump the pathpointlist
    {
        CPtrList *pPathPointList = m_pPathpointList;		///路线上点的列表
        ASSERT(pPathPointList);
        
        int nTypeCount[5];
        for(int i = 0; i < 5; i++)
        {
            nTypeCount[i] = 0;
        }
        int nCount = pPathPointList->GetCount();
        POSITION pos = pPathPointList->GetHeadPosition();
        int nIndexN = 0;
        while(pos)
        {
            CGIS_Pathpoint* pPathPoint = (CGIS_Pathpoint*)pPathPointList->GetNext(pos);
            EnPathpointType eType = pPathPoint->GetPathpointType();
            switch(eType)
            {
            case EN_PATHPOINTTYPE_START:
                {
                    nTypeCount[0]++;                    
                    CGIS_PathpointMark* pFtr = (CGIS_PathpointMark*)pPathPoint;
                }
                break;
            case EN_PATHPOINTTYPE_INFLEXION:
                {
                    nTypeCount[1]++;
                }
                break;
            case EN_PATHPOINTTYPE_NODE:
                {
                    nTypeCount[2]++;
                    CGIS_PathpointNode* pFtr = (CGIS_PathpointNode*)pPathPoint;
                    //DUMPSTATE(pFtr);
                    //TRACE_SLEEP_BY_TIMES(20);
                }
                break;
            case EN_PATHPOINTTYPE_INTERMEDIATE:
                {
                    nTypeCount[3]++;
                    CGIS_PathpointMark* pFtr = (CGIS_PathpointMark*)pPathPoint;
                }
                break;
            case EN_PATHPOINTTYPE_END:
                {
                    nTypeCount[4]++;
                    CGIS_PathpointMark* pFtr = (CGIS_PathpointMark*)pPathPoint;
                }
                break;
            default:
                ASSERT(FALSE);
                break;
            }
            nIndexN++;
        }
        
        
        TRACE1("--PathPointList[cnt = %d]\n",pPathPointList->GetCount());
        TRACE1("起始点  (START)             [cnt = %d]\n",nTypeCount[0]);
        TRACE1("拐点    (INFLEXION)         [cnt = %d]\n",nTypeCount[1]);
        TRACE1("NODE点  (NODE)              [cnt = %d]\n",nTypeCount[2]);
        TRACE1("途中点  (INTERMEDIATE       [cnt = %d]\n",nTypeCount[3]);
        TRACE1("目标点  (END                [cnt = %d]\n",nTypeCount[4]);
        TRACE0("--------\n");

    }

    //////////////////////////////////////////////////////////////////////////
    //dump the turnlist
    TRACE1("Turnlist[cnt = %d]\n",m_pTurnList->GetCount());
    {
        POSITION pos = m_pTurnList->GetHeadPosition();
        ST_TURN_INFO *pTurnInfo = NULL;
        while( pos )
        {
            pTurnInfo = (ST_TURN_INFO*)m_pTurnList->GetNext(pos);
#ifdef _DEBUG
            DUMPSTATE(pTurnInfo);
            CGIS_Pathpoint* pPathPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt(pTurnInfo->psPos);
            ASSERT(pPathPoint);
            ST_GEO_PTXY pt =  pPathPoint->GetPathpointPosition();
            TRACE2("\t%f,%f\n",pt.dx,pt.dy );
            TRACE_SLEEP_BY_TIMES(10);
#endif
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //write the turnlist script
#ifdef TRACE_TURN_LIST_SCRIPT
    TRACE0("\n####################################################################\n");
    TRACE1("#TurnlistScript[cnt = %d]\n",m_pTurnList->GetCount());
    {
        POSITION pos = m_pTurnList->GetHeadPosition();
        ST_TURN_INFO *pTurnInfo = NULL;
        while( pos )
        {
            pTurnInfo = (ST_TURN_INFO*)m_pTurnList->GetNext(pos);
            {

                if(pTurnInfo->byRMType == 0)
                {
                    TRACE3("#%.1f km(%.1f m)路名[%s] .\n",pTurnInfo->dbD2Pre/1000,pTurnInfo->dbD2Pre,pTurnInfo->strRName);
                    TRACE2("#\t%s\t%s\n",pTurnInfo->strLVoice,pTurnInfo->strSVoice);
                }
                else
                {
                    TRACE3("#%.1f km(%.1f m)出口序[%d] .\n",pTurnInfo->dbD2Pre/1000,pTurnInfo->dbD2Pre,pTurnInfo->byRMobile);
                    TRACE2("#\t%s\t%s\n",pTurnInfo->strLVoice,pTurnInfo->strSVoice);
                    
                }
            }

            CGIS_Pathpoint* pPathPoint = (CGIS_Pathpoint*)m_pPathpointList->GetAt(pTurnInfo->psPos);
            ASSERT(pPathPoint);
            ST_GEO_PTXY pt =  pPathPoint->GetPathpointPosition();
            TRACE2("\t%f\n\t%f\n同步调用:显示中心定位\n",pt.dx,pt.dy );
            TRACE_SLEEP_BY_TIMES(10);

        }
    }
    TRACE0("####################################################################\n\n");
#endif

    TRACE0("********************************************************************\n");
}

void CGIS_NetworkPath::AssertValid() const	
{   
    if(m_pNodeList)
    {
        POSITION pos = m_pNodeList->GetHeadPosition();
        while(pos)
        {
            CGIS_PathpointNodeV2* pNode = (CGIS_PathpointNodeV2*)m_pNodeList->GetNext(pos);
            ASSERT(pNode);
            ASSERTVALID(pNode);
        }
    }
        
}
#endif
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
int FindIndexByPostionInPtrList(CPtrList* pList,POSITION posFind)
{
    int nRtn = -1;

    int nIndex = 0;
    POSITION pos = pList->GetHeadPosition();
    while(pos)
    {
        if(pos == posFind)
        {
            nRtn = nIndex;
            break;
        }

        pList->GetNext(pos);
        nIndex++;
    }

    return nRtn;

}
#endif

//========================================================================
// 是否因为时间片禁行而重新计算航线，状态只能提取一次
//========================================================================
bool CGIS_NetworkPath::GetbTipReCal()
{
	bool bCal = m_bTipCal;
	m_bTipCal = false;
	return bCal;
}

//========================================================================
// 处理时间片禁行
//
// 基本处理思想：从当前节点向后不断搜索
//
// 1 阀值3公里内如果有禁行路段，评估不能通过，提示重新计算，停止搜索
//
// 2 阀值3公里内没有禁行路段，或有但评估通过，继续搜索
//
// 3 在3公里外，若遇到不是唯一可通行路段，停止搜索
//
// 4 遇到禁行路段，评估不能通过，提示重新计算，停止搜索
//
// 5 遇到禁行路段，评估通过，接着步骤3处理
//
// 注意点：1 对评估不通过的禁行路段只提示一次
//
//		   2 对评估通过的禁行路段会不断评估，防止堵车等事件发生
//
//		   3 目前禁行只考虑转向时刻，即进入路段的时刻，并不是整条路段禁行
//
//		   4 开始导航后，在1公里内不做时间片禁行处理，防止无休止的计算
//
//		   5 一个节点上可能有几条禁行路段信息，一条禁行信息中的路段可能有多条
//
// 缺陷：如果出现很多连续唯一通行路段，可能会评估不是很准
//
// 返回：TRUE正常行驶，FALSE重新计算
//========================================================================
BOOL CGIS_NetworkPath::TimeSliceLimit(CPtrList *pList, // 航线节点列表
									  double dbDisToNode, // 到当前节点距离
									  POSITION posCurNode) // 当前节点
{
	ASSERT(pList != NULL && posCurNode != NULL);

	double dbSpeed = SPEED_MINOR_WAY; // 评估的速度，默认为最低等级

	// 计算出到当前节点的偏移时间
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	int nTime = systime.wHour*60 + systime.wMinute; // 相对于0点的时间偏移
	POSITION posPre = posCurNode;
	pList->GetPrev(posPre);
	if (posPre != NULL)
	{
		CGIS_PathpointNodeV2 *pTemp = NULL;
		pTemp = (CGIS_PathpointNodeV2*)pList->GetAt(posPre);
		if (pTemp != NULL)
		{
			CGIS_PathpointNodeV2::stuRoadSectionInfo *pRoadSec = NULL;
			pRoadSec = pTemp->GetNextRoadSectionInfo();
			if (pRoadSec->nType == 0)
				dbSpeed = SPEED_MOTOR_WAY;
			else if (pRoadSec->nType==1 || pRoadSec->nType==2 || pRoadSec->nType==3)
				dbSpeed = SPEED_MAJOR_WAY;
		}
	}
	nTime += (int)((dbDisToNode/dbSpeed) / 60);

	double dbDis = dbDisToNode;
	POSITION pos = posCurNode;
	CGIS_PathpointNodeV2 *pPathpointNodeV2 = NULL;
	
	while (pos)
	{
		POSITION posSave = pos;
		pPathpointNodeV2 = (CGIS_PathpointNodeV2*)pList->GetNext(pos);

		if (dbDis > DISTANCE_TIMESLICE_RECAL)
		{
			if (!pPathpointNodeV2->IfOnlyPassRoadSection())
				return TRUE;
		}

		CGIS_NetworkRut::stuRoadSectionLimitInfoGroup *pLimitInfo = NULL;
		pLimitInfo = pPathpointNodeV2->GetLimitInfo();	
		CGIS_PathpointNodeV2::stuRoadSectionInfo *pRoadSec = NULL;
		pRoadSec = pPathpointNodeV2->GetNextRoadSectionInfo();
		int i;
		for (i=0; i<20; i++)
		{
			if (m_posLimit[i] == posSave)
				break;
		}

		// 若指针为空，表示没有禁行路段
		if((pLimitInfo != NULL) && (i == 20))
		{
			CGIS_NetworkRut::TURNINFO *pTurnInfo = NULL;
			pTurnInfo = pLimitInfo->pTurnInfoArray;

			for (int m=0; m<pLimitInfo->byTurnCount; m++)
			{
				int nRutId = pPathpointNodeV2->GetNextRutID();
				int k;
				for (k=0; k<pTurnInfo[m].byToRouteIDCount; k++)
				{
					if (pTurnInfo[m].pwToRutID[k] == nRutId)
						break;
				}
				if (k == pTurnInfo[m].byToRouteIDCount)
					break;
				// 如果到达路段的时刻点在禁行时段范围内，认为符合禁行
				if ((nTime > pTurnInfo[m].byStartTime*10 &&
					 nTime < pTurnInfo[m].byEndTime*10) ||
					(pTurnInfo[m].byStartTime==255 && pTurnInfo[m].byEndTime==255))
				{
					m_bTipCal = true;
					for (int j=0; j<19; j++)
						m_posLimit[j] = m_posLimit[j+1];
					
					m_posLimit[19] = posSave;
					return FALSE;
				}
			}
		}

		if (pRoadSec->nType == 0)
			dbSpeed = SPEED_MOTOR_WAY;
		else if (pRoadSec->nType==1 || pRoadSec->nType==2 || pRoadSec->nType==3)
			dbSpeed = SPEED_MAJOR_WAY;
		else
			dbSpeed = SPEED_MINOR_WAY;
		
		dbDis += pRoadSec->nLength;
		nTime += (int)((pRoadSec->nLength/dbSpeed) / 60);
	}
	return TRUE;
}


//========================================================================
// 计算在NODE点列表中的当前位置
//
// 实施策略：在路径点列表中，选取当前点开始，从NODE点列表中的当前点开始
//
// 遍历，直到路径点列表中的点能找到对应，此时，可以得到NODE点列表中对应的
//
// 点为当前点，路径点列表中当前点到对应点的距离为到当前NODE点的距离
//
// 参数为路径点列表的当前位置，返回：到当前节点的距离，失败为-1
//========================================================================
double CGIS_NetworkPath::CalCurPosInNodeList(POSITION posPathpoint)
{
	if (m_pPathpointList==NULL || m_pNodeList==NULL)
		return -1;
	
	POSITION posNodeList = m_posNodeCur;
	double dbDisToNode = 0.0;

	while (posPathpoint)
	{
		posNodeList = m_posNodeCur;
		while (posNodeList)
		{
			CGIS_PathpointNodeV2 *pNodeV2 = NULL;
			pNodeV2 = (CGIS_PathpointNodeV2*)m_pNodeList->GetAt(posNodeList);
			if (pNodeV2 != NULL)
			{
				if (posPathpoint == pNodeV2->GetPosOfPathPointList())
				{
					m_posNodeCur = posNodeList;
					return dbDisToNode;
				}
			}
			m_pNodeList->GetNext(posNodeList);
		}
		CGIS_Pathpoint *pPath = NULL;
		pPath = (CGIS_Pathpoint*)m_pPathpointList->GetNext(posPathpoint);
		if (pPath != NULL)
			dbDisToNode += pPath->GetDis2Next();
	}

	return -1;
}

//========================================================================
// 格式化语音路名信息，将语音中需要填充路名的地方都填好
//========================================================================
void CGIS_NetworkPath::FormatVoiceRoadName(ST_TURN_INFO *pInfo, // 机动点
										   int nOrd, // 1 远距离 2 中距离 3 近距离
										   swString &strVoice) // 语音文本
{
	if (nOrd == 1)
		strVoice = pInfo->strLVoice;
	else if (nOrd == 2)
		strVoice = pInfo->strSVoice;
	else if (nOrd == 3)
		strVoice = pInfo->strIVoice;

	// 在此格式化路名，如果存在看板信息，则使用它，没有就使用路名
	swString strRoadName;
	if (pInfo->strSign.IsEmpty())
	{	
		strRoadName = pInfo->strRName;
		if (strRoadName.IsEmpty())
		{
			if (pInfo->byConfusion2 != 16)
			{
				strRoadName = _T("道路");
			}
			else
			{
				int nFind = strVoice.Find(ROAD_NONAME_DEL);
				if (-1 != nFind)
					strVoice = strVoice.Left(nFind);
			}
		}
	}
	else
		strRoadName = pInfo->strSign;

	if (nOrd == 1 || nOrd == 2)
	{
		switch(pInfo->byRMType)
		{
		case 0:
			strVoice.Format(strVoice, _T("%s"), strRoadName);
			break;
		case 1:
			strVoice.Format(strVoice, pInfo->byRMobile-1);
			break;
		case 2:
			strVoice.Format(strVoice, _T("%s"), pInfo->byRMobile-1);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	else if (nOrd == 3)
	{
		switch(pInfo->byRMType)
		{
		case 0:
			strVoice.Format(strVoice, strRoadName);
			break;
		case 1:
			strVoice.Format(strVoice, pInfo->byRMobile-1);
			break;
		case 2:
			strVoice.Format(strVoice, pInfo->byRMobile-1);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
}