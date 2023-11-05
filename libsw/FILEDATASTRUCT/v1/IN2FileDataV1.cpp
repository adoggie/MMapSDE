// IN2FileDataV1.cpp: implementation of the CIN2FileDataV1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IN2FileDataV1.h"

#include "TempMapInfo.h"
#include "TempGeoMap.h"
#include "TempMapLayer.h"
#include "TempLayerManager.h"
#include "TempLayerInfo.h"
#include "GIS_BreadthMap.h"

int MAP_ORIGIN_BID = 444624;
int MAP_ORIGIN_XNUM = 372;
int MAP_ORIGIN_YNUM = 354;
float MAP_ORIGIN_CENTERX = 106.563;
float MAP_ORIGIN_CENTERY = 29.5417;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIN2FileDataV1::CIN2FileDataV1()
{
    m_pArrayOfBreadthMap = NULL;
    m_nBreadthCount = 0;

}

CIN2FileDataV1::~CIN2FileDataV1()
{
	POSITION pos = m_ListOfMap.GetHeadPosition();
	while(pos)
	{
		CTempGeoMap*pMap = (CTempGeoMap*)m_ListOfMap.GetNext(pos);
		ASSERT(pMap);
		delete pMap;
		pMap = NULL;
	}
	m_ListOfMap.RemoveAll();
}

BOOL CIN2FileDataV1::Load(LPCTSTR strFilePath)
{
    CString strFName = strFilePath;
 	HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwSize = GetFileSize( hFile, NULL );
	DWORD dwCount = 0;
	char *pFBuf = new char[dwSize];
	ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
	if( dwSize > dwCount )
		return FALSE;

		DWORD dwStation = 0;
		char szInfo[32];

		//解密加密过程
		DWORD dwReturnBytes = 0;
		char MyUUID[16];
		Fread( pFBuf, dwSize, dwStation, szInfo, 32, 1 );
#ifdef _WIN32_WCE
#ifdef _ENCRYPT_CHECK_ENABLE
		BOOL bIsOK = KernelIoControl(IOCTL_HAL_GET_UUID, 0, 0, MyUUID, sizeof(UUID), &dwReturnBytes );
		if( !bIsOK )
			return 1;
		if( !EnCode( szInfo , MyUUID ,  hFile) )
		{
			if( !UnCode(szInfo , MyUUID) )
				return 1;
		}
#endif
#endif

        
        

	CTempGeoMap *pMap = new CTempGeoMap;
	int n0 = strFName.ReverseFind(L'\\');
	CString strPath = strFName.Left( n0+1 );
	pMap->GetMapInfo()->SetMapFilePath( strFName.Left( n0+1 ) );

	
	//////////////////////////////////////////////////////////////////////////
	//读出所有的层信息
	//
	int nNum = 0;
	int nLNum = 0;
	//读图层个数(全国图为6,详图为25)
	Fread( pFBuf, dwSize, dwStation, &nLNum, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapLayNum(nLNum);
	BYTE byNum = 0;
	float dbNum = 0.0;
	CTempMapLayer *pLList[30];
	
	//循环读出所有层的信息
	int i;
	CTempLayerManager *pLManager = new CTempLayerManager();//搞出图层管理器
	for( i=0;	i< nLNum;	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		CTempLayerInfo *pLInfo = new CTempLayerInfo( byNum );
		ST_GEO_PEN pen;
		ST_GEO_BRUSH brush;
		Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pen.nStyle = nNum;
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		brush.nStyle = nNum;
		//pLInfo->SetPen( pen );
		//pLInfo->SetBrush( brush );
		///图层的显示
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		pLInfo->SetLayerState( byNum );
		//图层的scale范围
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		dbNum = 0.1;
		//首都：5 直辖市：4 省会：3 地级市：2
		if( i == 2 )
			dbNum = 70;
		if( i == 4 || i == 3 || i == 5 )
			dbNum = 20;
		pLInfo->SetMinGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		dbNum = 10000000.1;
		pLInfo->SetMaxGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		dbNum = 0.1;
		//首都：5 直辖市：4 省会：3 地级市：2
		if( i == 2 )
			dbNum = 70;
		if( i == 4 || i == 3 || i == 5 )
			dbNum = 20;
		pLInfo->SetMinARScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		pLInfo->SetMaxARScale(dbNum);
		///
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pLInfo->SetIconID(nNum);
		
		CTempMapLayer *pLayer = new CTempMapLayer( pLInfo );
		pLList[i] = pLayer;
		pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
	}
	//////////////////////////////////////////////////////////////////////////
	
	
	//////////////////////////////////////////////////////////////////////////
	//读地图名做一下验证
	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
	if( nNum )
		szInfo[nNum] = '\0';
	//pMap->GetMapInfo()->SetMapName( szInfo );
	//if( CString(szInfo).Find(L"NAV_2005_SChina") >= 0 )
	//	pMap->GetMapInfo()->SetMapType( EN_MAPTYPE_NATION );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//读地图id(全国图为0)
	int nMID = 0;
	Fread( pFBuf, dwSize, dwStation, &nMID, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapID(nMID);

	//读边界点(全国图没有)
	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapBndPtNum( nNum );
	if( nNum != 0 )
	{
		ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
		Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
		pMap->GetMapInfo()->SetMapBndList( pBndPtList );
	}
	//////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////////////
	int nBMID = 0;
	int nBMapID = 0;
	ST_GEO_RECT rtBreadth;
	Fread( pFBuf, dwSize, dwStation, &nBMID, sizeof(int), 1 );

	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );
	pMap->GetMapInfo()->SetMapRect( rtBreadth );
	//读出全国图图幅数(只有28个,0-27)
	int nBMNum = 0;
	Fread( pFBuf, dwSize, dwStation, &nBMNum, sizeof(int), 1 );

	int nBMapMaxID = 0;
	CGIS_BreadthMap *pBMapList = NULL;

	pBMapList = new CGIS_BreadthMap[nBMNum];
	memset( pBMapList, 0, sizeof(CGIS_BreadthMap)*nBMNum );
	ST_GEO_RECT pRTList[128];
	for( i=0;	i<nBMNum;	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &pBMapList[i].m_nBMID, sizeof(int), 1 );

		Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &pRTList[i].dTop, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &pRTList[i].dBottom, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &pRTList[i].dLeft, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &pRTList[i].dRight, sizeof(float), 1 );

		pBMapList[i].m_nBreadthID = nBMapID;

		if( pBMapList[i].m_nBreadthID > nBMapMaxID )
			nBMapMaxID = pBMapList[i].m_nBreadthID;
	}
	pMap->SetBreadthMapNum( nBMNum );
	pMap->SetBreadthMapList( pBMapList );
	pMap->SetBreadthMapRects( pRTList, nBMNum );

	//从图层管理器中遍历所有图层,对每个图层初始化
//	{
//		POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
//		short    nKey;
//		CTempMapLayer* pLay = NULL;
//		while( pos != NULL )
//		{
//			pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
//			if( pLay )
//			{
//				((CTempMapLayer*)pLay)->InitLBCount( nBMapMaxID );
//			}
//		}
// 	}
	//pMap->m_LayerManagerSet->AddTail( pLManager );
	pMap->m_pLayerMng = pLManager;

	//pMap->Open( "" );
	//pMapList->AddTail( pMap );

	m_ListOfMap.AddTail(pMap);

	pBMapList = NULL;
	//以上是全国图
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//以下是城市图
	CTempLayerInfo LIList[30];
	Fread( pFBuf, dwSize, dwStation, &nLNum, sizeof(int), 1 );
	for( i=0;	i<nLNum;	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		LIList[i].SetLayerID( byNum );
		ST_GEO_PEN pen;
		ST_GEO_BRUSH brush;
		Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pen.nStyle = nNum;
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		brush.nStyle = nNum;
//		LIList[i].SetPen( pen );
//		LIList[i].SetBrush( brush );
		///图层的显示
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		LIList[i].SetLayerState( byNum );
		//图层的scale范围
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		LIList[i].SetMinGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		LIList[i].SetMaxGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		LIList[i].SetMinARScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		LIList[i].SetMaxARScale(dbNum);
		///
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		if( i>10 )
			LIList[i].SetIconID(nNum);
		else
			LIList[i].SetIconID(0);
	}
	int nMNum = 0;
	Fread( pFBuf, dwSize, dwStation, &nMNum, sizeof(int), 1 );
	for( i=0;	i<nMNum;	i++ )
	{
		pMap = new CTempGeoMap;
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
		if( nNum )
			szInfo[nNum] = '\0';
		//pMap->GetMapInfo()->SetMapName( szInfo );
		//pMap->GetMapInfo()->SetMapFilePath( strFName.Left( n0+1 ) );

		Fread( pFBuf, dwSize, dwStation, &nMID, sizeof(int), 1 );
		pMap->GetMapInfo()->SetMapID(nMID);
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pMap->GetMapInfo()->SetMapBndPtNum( nNum );
		if( nNum != 0 )
		{
			ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
			Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
			pMap->GetMapInfo()->SetMapBndList( pBndPtList );
		}
		pLManager = new CTempLayerManager();
		int j;
		for(  j=0;	j<nLNum;	j++ )
		{
			CTempLayerInfo *pLInfo = new CTempLayerInfo();
			memcpy( pLInfo, &LIList[j], sizeof(CTempLayerInfo) );
			CTempMapLayer *pLayer = new CTempMapLayer( pLInfo );
			pLList[j] = pLayer;
			pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
		}

		Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );

		pMap->GetMapInfo()->SetMapRect( rtBreadth );


		//读出详图的图幅个数
		Fread( pFBuf, dwSize, dwStation, &nBMNum, sizeof(int), 1 );

		//-------------------------------------------------------------
		//读出基本图幅号,基本图幅号的X轴,基本图幅号的Y轴,基本图幅的中心点
		Fread( pFBuf, dwSize, dwStation, &MAP_ORIGIN_BID, sizeof(int), 1 );
		int nn3 = 0;
		Fread( pFBuf, dwSize, dwStation, &nn3, sizeof(int), 1 );
		MAP_ORIGIN_XNUM = nn3;
		Fread( pFBuf, dwSize, dwStation, &nn3, sizeof(int), 1 );
		MAP_ORIGIN_YNUM = nn3;
		float dValue = 0.0;
		Fread( pFBuf, dwSize, dwStation, &dValue, sizeof(float), 1 );
		MAP_ORIGIN_CENTERY = dValue;
		Fread( pFBuf, dwSize, dwStation, &dValue, sizeof(float), 1 );
		MAP_ORIGIN_CENTERX = dValue;
		//-------------------------------------------------------------


		//////////////////////////////////////////////////////////////////////////
		//构造出图幅个数的BreadthMap,
		int nBMapNum = 0;
		nBMapMaxID = 0;
		pBMapList = new CGIS_BreadthMap[nBMNum];
		memset( pBMapList, 0, sizeof(CGIS_BreadthMap)*nBMNum );
		for( j=0;	j<nBMNum;	j++ )
		{

			Fread( pFBuf, dwSize, dwStation, &pBMapList[nBMapNum].m_nBMID, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
			pBMapList[nBMapNum].m_nBreadthID = nBMapID;

			if( pBMapList[nBMapNum].m_nBreadthID > nBMapMaxID )
				nBMapMaxID = pBMapList[nBMapNum].m_nBreadthID;
			nBMapNum++;
			/////init breadthmap layer object list;
		}
		//-----------------------------------------------------------------------

		
		pMap->SetBreadthMapNum( nBMapNum );
		pMap->SetBreadthMapList( pBMapList );

        m_nBreadthCount = nBMapNum;
        m_pArrayOfBreadthMap = pBMapList;
//        {
            TRACE1("nBMapNum = %d\n",nBMapNum);
//            CFile aFile(_T("e:\\CGIS_BreadthMap.dat"),CFile::modeCreate|CFile::modeReadWrite);
//            aFile.Write(pBMapList,sizeof(CGIS_BreadthMap)*nBMapNum);
//         }


//		{
//			POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
//			short    nKey;
//			CTempMapLayer* pLay = NULL;
//			
//			while( pos != NULL )
//			{
//				pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
//				if( pLay )
//				{
//					((CTempMapLayer*)pLay)->InitLBCount( nBMapMaxID );
//				}
//			}
//		}

//		pMap->m_LayerManagerSet->AddTail( pLManager );
		pMap->m_pLayerMng = pLManager;

//		pMap->Open( "" );
//		pMapList->AddTail( pMap );
		m_ListOfMap.AddTail(pMap);

		pBMapList = NULL;
	}

	delete[] pFBuf;
	CloseHandle( hFile );
	return TRUE;
   
}

CGIS_BreadthMap* CIN2FileDataV1::GetBreadthMapByID( int nRawBreadthID )
{
	CGIS_BreadthMap *pBNet = NULL;
	// 定位区域：
	int nStart = 0;
	int nEnd = m_nBreadthCount;
	int nMid = 0;
	while( nEnd > nStart )
	{
		nMid = (nStart+nEnd)/2;
		if( m_pArrayOfBreadthMap[nMid].m_nBMID == nRawBreadthID )
			return &m_pArrayOfBreadthMap[nMid];
		else if( m_pArrayOfBreadthMap[nMid].m_nBMID > nRawBreadthID )
			nEnd = nMid;
		else
			nStart = nMid;
		if( nEnd-nStart <= 1 )
        {
            if ( m_pArrayOfBreadthMap[nStart].m_nBMID == nRawBreadthID )
                return &m_pArrayOfBreadthMap[nStart];
            else if ( m_pArrayOfBreadthMap[nEnd].m_nBMID == nRawBreadthID )
                return &m_pArrayOfBreadthMap[nEnd];
            return NULL;
            

        }
	}
	return NULL;
}
CGIS_BreadthMap* CIN2FileDataV1::GetBreadthMapByIndex( int nBreadthID )
{
    ASSERT(nBreadthID < GetBreadthCount());
    return &m_pArrayOfBreadthMap[nBreadthID];
}
CTempGeoMap* CIN2FileDataV1::GetMapByIndex(int nIndex)
{
    POSITION pos = m_ListOfMap.FindIndex(nIndex);
    if(pos)
    {
        return (CTempGeoMap*)m_ListOfMap.GetAt(pos);
    }

    return NULL;
}
