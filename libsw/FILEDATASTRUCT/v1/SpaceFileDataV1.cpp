// SpaceFileDataV1.cpp: implementation of the CSpaceFileDataV1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TempMapInfo.h"
#include "TempGeoMap.h"
#include "TempMapLayer.h"
#include "TempLayerManager.h"
#include "TempLayerInfo.h"
#include "GIS_BreadthMap.h"

#include "./FileDataStruct/V1/GIS_FeaturePoint.h"
#include "./FileDataStruct/V1/GIS_FeatureRut.h"
#include "./FileDataStruct/V1/GIS_FeatureLine.h"
#include "./FileDataStruct/V1/GIS_FeaturePoly.h"

#include "./Zlib/zlib.h"
#include "./DebugFunctionLib/Watch.h"


#include "SpaceFileDataV1.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BOOL ReadPointFeature   ( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, short sIconID, CPtrList* pOList );
BOOL ReadLineFeature    ( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, CPtrList* pOList );
BOOL ReadPolyFeature    ( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, CPtrList* pOList );
BOOL ReadRutFeature     ( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, CPtrList* pOList );

BOOL CompareFtrPoint(CGIS_FeaturePoint* pFtr1,CGIS_FeaturePoint* pFtr2)
{
    if(strcmp(pFtr1->GetAnnotation(),pFtr2->GetAnnotation()))
        return FALSE;

    if(pFtr1->GetPtList()->dx != pFtr2->GetPtList()->dx)
        return FALSE;

    if(pFtr1->GetPtList()->dy != pFtr2->GetPtList()->dy)
        return FALSE;

    return TRUE;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpaceFileDataV1::CSpaceFileDataV1()
{
    m_pMapContainer = NULL;
}

CSpaceFileDataV1::~CSpaceFileDataV1()
{

}

/**
 * @brief 加载并完成分析工作.分析的代码可拆离
 *
 * @param 
 * @return
*/
BOOL CSpaceFileDataV1::Load(CStringArray& arFilePath)
{

    ASSERT(arFilePath.GetSize() == 2);
    CString strFilePath = arFilePath[0];
    CString strIn2FilePath = arFilePath[1];

#ifdef  ANALYSE_DATA
    m_DataAnalyse.Initial();
#endif
	//加载in2文件到国家图对象和城市图对象.
	LoadIn2File(strIn2FilePath);
    m_strFilePath = strFilePath;
	

	CFile aFile(strFilePath,CFile::modeRead);
	CFile* pFile = &aFile;
    ReadSpaceData(pFile);
    DUMPSTATE(this);
    //ReadCityPosData(pFile);
    return FALSE;
}

BOOL CSpaceFileDataV1::ReadSpaceData(CFile* pFile)
{
    ASSERT(!m_pMapContainer);
    m_pMapContainer = new MapOfGeoMap;
	//读头
    m_pFileHeadInfo = new StuSpaceFileStructV1 :: StuHeaderInfo;
    memset(m_pFileHeadInfo,0,sizeof(StuSpaceFileStructV1 :: StuHeaderInfo));

    pFile->SeekToBegin();
    pFile->Read( m_pFileHeadInfo->chSign, 9 );
		
    int nMNum = 0;
    int nMID = 0;
    int nMOff = 0;
    pFile->Read( &nMNum, sizeof(int) );//读地图的数量
    m_pFileHeadInfo->nMapCount = nMNum;
     

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////
    

	int nCount = m_pFileHeadInfo->nMapCount;

	//---------------------------
	//读入地图索引
	StuSpaceFileStructV1::StuIndexInfoOfMap* pIndexInfoOfMap = new StuSpaceFileStructV1::StuIndexInfoOfMap[nCount];
	memset(pIndexInfoOfMap,0,sizeof(StuSpaceFileStructV1::StuIndexInfoOfMap)*nCount);
	int i;
    for( i=0;	i<nMNum;	i++ )
    {
        pFile->Read( &nMID, sizeof(int) );
        pFile->Read( &nMOff, sizeof(int) );
        pIndexInfoOfMap[i].nID = nMID;
        pIndexInfoOfMap[i].nOffset = nMOff;
        
        CTempGeoMap* pMap = (CTempGeoMap*)m_ListOfMap.GetAt(m_ListOfMap.FindIndex(i));
        ASSERT(pMap);
        pIndexInfoOfMap[i].nBreadthCount		= pMap->GetBreadthMapNum();
     }	
	//------------------------------------------------------------------------
	//读入地图
	for( i = 1; i < nCount; i++)
	{
        stuMapItem* pItem = new stuMapItem;
        
        pItem->pBreadthContainer = new MapOfBreadth;
        pItem->pIndex = &pIndexInfoOfMap[i];
        pItem->nMapID = i;

		ReadMap(pFile,pItem);
        (*m_pMapContainer)[pItem->pIndex->nID] = pItem;
	}
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	//清理工作
	delete pIndexInfoOfMap;
	pIndexInfoOfMap = NULL;

	
	return TRUE;
}
//加载in2文件
BOOL CSpaceFileDataV1::LoadIn2File( CString& strFName)
{
	
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
	CTempMapLayer *pLList[25];
	
	//循环读出所有层的信息
	CTempLayerManager *pLManager = new CTempLayerManager();//搞出图层管理器
	int i;
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
	CTempLayerInfo LIList[25];
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
		for( j=0;	j<nLNum;	j++ )
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

BOOL CSpaceFileDataV1::ReadLayerAllInfoInBreadth(CFile* pFile,int nMapN,int nBreadthN)
{
    CTempGeoMap* pMap = (CTempGeoMap*)m_ListOfMap.GetAt(m_ListOfMap.FindIndex(nMapN));
    TRY
    {  
        //读头
        pFile->SeekToBegin();
        char szInfo[32];
        pFile->Read( szInfo, 9 );
        
        
        int nMNum = 0;
        int nMID = 0;
        int nMOff = 0;
        pFile->Read( &nMNum, sizeof(int) );//读地图的数量
		int i;
        for( i=0;	i<nMNum;	i++ )
        {
            pFile->Read( &nMID, sizeof(int) );
            pFile->Read( &nMOff, sizeof(int) );
            if( nMID == pMap->GetMapInfo()->GetMapID() )
                break;
        }
        
        int nLayerTableCount = 0;
        switch(nMapN)
        {
        case 0:
            nLayerTableCount = 6;
            break;
        case 1:
            nLayerTableCount = 25;
            break;
        default:
            ASSERT(FALSE);
            break;
        }
        
        CGIS_BreadthMap* pBMap = pMap->GetBreadthMapByID2(nBreadthN);
        int nBreadthID = pBMap->m_nBreadthID;
        
        
        int nBID = 0;
        int nBOff = nMOff + 13 + (nBreadthID)*8;
        pFile->Seek( nBOff, CFile::begin );//定位文件到图幅索引偏移
        pFile->Read( &nBID, sizeof(int) );
        pFile->Read( &nBOff, sizeof(int) );
        pFile->Seek( nMOff+nBOff, CFile::begin );//定位文件到图幅数据
        
        CTempMapLayer *pLayer = NULL;
        DWORD dwStation = 0;
        
        int nActualLayerCount = 0;
        {
            //读出图层索引
            dwStation = nMOff+nBOff;
            int* pLayerTable =  new int[4*nLayerTableCount];
            pFile->Seek( dwStation, CFile::begin );
            pFile->Read( pLayerTable, sizeof(int)*4*nLayerTableCount );
            
            for (int i = 0; i < nLayerTableCount; i++)
            {
                int* nLInfo = &pLayerTable[i*4];
                if( nLInfo[0] != i ) 
                    ASSERT(FALSE);
                
                if( nLInfo[2] == 0 ||
                    nLInfo[3] == 0 ||
                    (nLInfo[2] == 8 && nLInfo[3] == 8 ) ||
                    (nLInfo[2] == 8 && nLInfo[3] == 11)) 
                {
                    //TRACE0("层索引数据非法!");
                    continue;
                }
                
                
                
                //---------------------------------------------
                //生成解压前与解压后缓冲区
                char *pFBuf = new char[nLInfo[2]];
                
                
                char *pBuf = new char[nLInfo[3]];
                //读出数据解压
                dwStation = nMOff+nBOff + nLInfo[1];
                pFile->Seek( dwStation, CFile::begin );
                pFile->Read( pBuf, nLInfo[3] );
                DWORD dwSize = nLInfo[2];
                uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nLInfo[3] );
                delete [] pBuf;
                
                
                dwStation = 0;
                BOOL bRutType = FALSE;
                short sIconID = -1;
                CPtrList *pOList = NULL;
                
                
                //取出id
                int nLID = 0;
                Fread( pFBuf, dwSize, dwStation, &nLID, sizeof(int), 1 );
                if( nLID > 4 && nLID < 12 )
					bRutType = TRUE;
				else
					bRutType = FALSE;
                
                //读出所有ftr
                int nCount;
                Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
                if(nCount == 0)
                {
                    //TRACE0("本层ftr个数为0!!.\n");
                    delete[] pFBuf;
                    continue;
                }
                BYTE bType = 0;		
                int nType = -1;
                Fread( pFBuf, dwSize, dwStation, &bType, sizeof(BYTE), 1 );
                switch(bType)
                {
                case 1:
                    nType = CGIS_Feature::EN_GOTYPE_POINT;
                    break;
                case 2:
                    if(bRutType)
                    {
                        nType = CGIS_Feature::EN_GOTYPE_RUT;
                    }
                    else
                    {
                        nType = CGIS_Feature::EN_GOTYPE_LINE;
                    }
                    break;
                case 3:
                    nType = CGIS_Feature::EN_GOTYPE_POLY;
                    break;
                default:
                    ASSERT(FALSE);
                    break;
                }
                
                delete[] pFBuf;
                               
                int nIconID = -1;
                {
                    CTempGeoMap* pMap = (CTempGeoMap*)m_ListOfMap.GetAt(m_ListOfMap.FindIndex(nMapN));
                    CTempLayerManager* pLayerMng = pMap->m_pLayerMng;
                    CTempMapLayer* pLayer = (*pLayerMng->m_pLayListMap)[i];
                    ASSERT(pLayer);
                    pLayer->GetLayerInfo()->GetIconID(nIconID);
                }
                ASSERT(nIconID != -1);
                m_LayerNewN2OldN    [nBreadthN][nActualLayerCount]  = i;
                m_LayerNewN2ID      [nBreadthN][nActualLayerCount]  = nLID;
                m_LayerNewN2Type    [nBreadthN][nActualLayerCount]  = nType;
                m_LayerNewN2IconID  [nBreadthN][nActualLayerCount]  = nIconID;
                nActualLayerCount++;

            }
            
            delete pLayerTable;
            pLayerTable = NULL;

		}

        m_LayerCount[nBreadthN] = nActualLayerCount;

        if(!nActualLayerCount)
            return FALSE;
		return TRUE;
	}
	CATCH( CMemoryException, e )
	{
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{

		return FALSE;
	}
	AND_CATCH( CException, e )
	{
		return FALSE;
	}
	END_CATCH

		
	return FALSE;
}

BOOL CSpaceFileDataV1::ReadMap(CFile* pFile,stuMapItem* pMapItem)
{
    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////

    StuSpaceFileStructV1 :: StuIndexInfoOfMap* pIndexInfoOfMap = pMapItem->pIndex;
	ASSERT(pIndexInfoOfMap);

	//读入图幅索引
	StuSpaceFileStructV1::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = NULL;	//图幅索引

	int nCount = pIndexInfoOfMap->nBreadthCount;
	{
		pIndexInfoOfBreadth = new StuSpaceFileStructV1::StuIndexInfoOfBreadth[nCount];
		memset(pIndexInfoOfBreadth,0,sizeof(StuSpaceFileStructV1::StuIndexInfoOfBreadth)*nCount);
//		pFile->Read(pIndexInfoOfBreadth,sizeof(StuSpaceFileStructV1::StuIndexInfoOfBreadth) * nCount);
	}

	//读入图幅数据
	//------------------------------------------------------------------------
	int i;
	for(i = 0; i < nCount; i++)
	{
		if(!(i%0Xff))
		{
			static DWORD dwtickstart = 0;
			TRACE2("----ReadBreadthV1(%d):%d ms--------\n",i,GetTickCount() - dwtickstart);
			dwtickstart = GetTickCount();
		}

        stuBreadthItem* pBreadthItem = new stuBreadthItem;
        pBreadthItem->pLayerContainer = new MapOfLayer;
        pBreadthItem->pIndex = &pIndexInfoOfBreadth[i];
        pBreadthItem->nMapID        = pMapItem->nMapID;
        pBreadthItem->nBreadthID    = i;
        
        ReadLayerAllInfoInBreadth(pFile,pMapItem->nMapID,i);
        pBreadthItem->pIndex->nLayerCount = m_LayerCount[i];
      
        pFile->Seek(pBreadthItem->pIndex->nOffset+dwOffsetBase,CFile::begin);
        ReadBreadth(pFile,pBreadthItem);//读入图幅
        if(pBreadthItem->pLayerContainer->size())
        {
            m_BreadthContainer[pBreadthItem->pIndex->nID] = pBreadthItem;
        }
        else
        {
            delete pBreadthItem->pLayerContainer;
            //TRACE1("图幅[%d]没有路段数据.\n",i);
        }
	}
    
	//清理
	delete pIndexInfoOfBreadth;
	pIndexInfoOfBreadth = NULL;

	return TRUE;
}

BOOL CSpaceFileDataV1::ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem)
{
    ASSERT(pBreadthItem);


    StuSpaceFileStructV1::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = 
        pBreadthItem->pIndex;
    ASSERT(pIndexInfoOfBreadth);

    DWORD dwOffsetBase = pFile->GetPosition();

	//////////////////////////////////////////////////////////////////////////
	ASSERT(pIndexInfoOfBreadth);

	int nCount = pIndexInfoOfBreadth->nLayerCount;
	//////////////////////////////////////////////////////////////////////////
	StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData* pLayerIndex = NULL;//层索引区
	//读入图层索引
	{
		pLayerIndex = new StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData[nCount];
		memset(pLayerIndex,0,sizeof(StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData)*nCount);
		//pFile->Read(pLayerIndex,sizeof(StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData) * nCount );
	}
	//////////////////////////////////////////////////////////////////////////

	//读入图层数据
#ifdef ANALYSE_DATA
    m_DataAnalyse.m_nEmptyPackageCount += 25;

    m_BreadthRoadSectionCount = 0;
#endif
	int i;
	for(i = 0 ; i < 25; i++)
	{
        stuLayerItem* pLayerItem = new stuLayerItem;
        pLayerItem->pFtrContainer = new MapOfFeature;
        pLayerItem->pIndex = &pLayerIndex[i];
        pLayerItem->nMapID      = pBreadthItem->nMapID;
        pLayerItem->nBreadthID  = pBreadthItem->nBreadthID;
        pLayerItem->nLayerID    = i;
        //if(pIndexOfLayer->nID == 10 || pIndexOfLayer->nID == 11)
        {
            ReadLayer( pFile,pLayerItem );									
        }
	}
#ifdef ANALYSE_DATA
    if( m_BreadthRoadSectionCount < m_DataAnalyse.m_nMinRoadSectionCountOfBreadth )
    {
        m_DataAnalyse.m_nMinRoadSectionCountOfBreadth = m_BreadthRoadSectionCount;
    }
    if( m_BreadthRoadSectionCount > m_DataAnalyse.m_nMaxRoadSectionCountOfBreadth)
    {
        m_DataAnalyse.m_nMaxRoadSectionCountOfBreadth = m_BreadthRoadSectionCount;
    }
    m_BreadthRoadSectionCount = 0;
#endif    

	delete pLayerIndex;
	pLayerIndex = NULL;

	return FALSE;
}

BOOL CSpaceFileDataV1::ReadLayer(CFile* pFile,stuLayerItem* pItem)
{
    ASSERT(pFile);
    ASSERT(pItem);

    unsigned long nMapID = pItem->nMapID;
    unsigned long nBreadthID = pItem->nBreadthID;
    unsigned long nLayerID = pItem->nLayerID;
//////////////////////////////////////////////////////////////////////////
    int nMapN = nMapID;
    int nBreadthN = nBreadthID;
    int nLayerN = nLayerID;

    CPtrList aList;
    ReadLayerV1(pFile,nMapN,nBreadthN,nLayerN,&aList);


    //////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_DATA
    if(nLayerN >= 5 && nLayerN <= 11)
    {
        m_BreadthRoadSectionCount += aList.GetCount();
    }
#endif       
    //////////////////////////////////////////////////////////////////////////
    
    POSITION pos = aList.GetHeadPosition();
    while(pos)
    {
        CGIS_Feature* pFtr = (CGIS_Feature*)aList.GetNext(pos);
        delete pFtr;
    }

	return FALSE;
}

/**
 * @brief 把指定的层的数据读到一个ptlist中.
 *
 * @param 
 * @return
*/

static char pBufBeforeCompress  [5000000];
static char pBufAfterCompress   [3000000];
BOOL CSpaceFileDataV1::ReadLayerV1(CFile* pFile,int nMapN,int nBreadthN,int nLayerN,CPtrList* pFtrList)
{
	CTempGeoMap* pMap = (CTempGeoMap*)m_ListOfMap.GetAt(m_ListOfMap.FindIndex(nMapN));
	char *pFBuf = NULL;
	TRY
	{

		//读头
		pFile->SeekToBegin();
		char szInfo[32];
		pFile->Read( szInfo, 9 );

		
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		pFile->Read( &nMNum, sizeof(int) );//读地图的数量
		int i;
		for( i=0;	i<nMNum;	i++ )
		{
			pFile->Read( &nMID, sizeof(int) );
			pFile->Read( &nMOff, sizeof(int) );
			if( nMID == pMap->GetMapInfo()->GetMapID() )
				break;
		}

		CGIS_BreadthMap* pBMap = pMap->GetBreadthMapByID2(nBreadthN);
		int nBreadthID = pBMap->m_nBreadthID;


		int nBID = 0;
		int nBOff = nMOff + 13 + (nBreadthID)*8;
		pFile->Seek( nBOff, CFile::begin );//定位文件到图幅索引偏移
		pFile->Read( &nBID, sizeof(int) );
		pFile->Read( &nBOff, sizeof(int) );
		pFile->Seek( nMOff+nBOff, CFile::begin );//定位文件到图幅数据
		
		CTempMapLayer *pLayer = NULL;
		DWORD dwStation = 0;

	
		{
			//读出图层索引
			dwStation = nMOff+nBOff + nLayerN*16;
			int nLInfo[4];
			pFile->Seek( dwStation, CFile::begin );
			pFile->Read( nLInfo, sizeof(int)*4 );
			
			if( nLInfo[0] != nLayerN ) 
				return FALSE;//id不等的话说明数据有误



                if( nLInfo[2] == 0 ||
                    nLInfo[3] == 0 ||
                    (nLInfo[2] == 8 && nLInfo[3] == 8 ) ||
                    (nLInfo[2] == 8 && nLInfo[3] == 11)) 
                {
#ifdef ANALYSE_DATA
                    m_DataAnalyse.m_nEmptyPackageCount++;
#endif
                    return FALSE;
                }
#ifdef ANALYSE_DATA          
            //nLInfo[2]-压缩前size
            //nLInfo[3]-压缩后size

                DWORD dwBeforeCompressSize = nLInfo[2];
                DWORD dwAfterCompressSize = nLInfo[3];

                if(dwBeforeCompressSize > m_DataAnalyse.m_dwMaxBeforeCompressSize)
                {
                    m_DataAnalyse.m_dwMaxBeforeCompressSize = dwBeforeCompressSize;
                }
                if(dwBeforeCompressSize < m_DataAnalyse.m_dwMinBeforeCompressSize)
                {
                    m_DataAnalyse.m_dwMinBeforeCompressSize = dwBeforeCompressSize;
                }
                if(dwAfterCompressSize > m_DataAnalyse.m_dwMaxAfterCompressSize)
                {
                    m_DataAnalyse.m_dwMaxAfterCompressSize = dwAfterCompressSize;
                }
                if(dwAfterCompressSize < m_DataAnalyse.m_dwMinAfterCompressSize)
                {
                    m_DataAnalyse.m_dwMinAfterCompressSize = dwAfterCompressSize;
                }
				int i;
                for(i = 0; i< 10; i++)
                {
                    DWORD dwUpperLimit = i*CDataAnalyse::CELL_SIZE;
                    DWORD dwLowerLimit = (i+1)*CDataAnalyse::CELL_SIZE;
                    if( dwUpperLimit <= dwBeforeCompressSize && dwBeforeCompressSize < dwLowerLimit)
                    {
                        m_DataAnalyse.nCountOfSpecialSizePackageBeforeCompress[i]++;
                    }

                    if(dwUpperLimit <= dwAfterCompressSize && dwAfterCompressSize < dwLowerLimit)
                    {
                        m_DataAnalyse.nCountOfSpecialSizePackageAfterCompress[i]++;
                    }
                }
                if(dwBeforeCompressSize >= 10*CDataAnalyse::CELL_SIZE)
                {
                    m_DataAnalyse.nCountOfSpecialSizePackageBeforeCompress[10]++;
                }
                if(dwAfterCompressSize >= 10*CDataAnalyse::CELL_SIZE)
                {
                    m_DataAnalyse.nCountOfSpecialSizePackageAfterCompress[10]++;
                }
                


#endif
			//---------------------------------------------
			//生成解压前与解压后缓冲区            

            pFBuf = pBufBeforeCompress;
            char* pBuf = pBufAfterCompress;

			//读出数据解压
			dwStation = nMOff+nBOff + nLInfo[1];
			pFile->Seek( dwStation, CFile::begin );
			pFile->Read( pBufAfterCompress, nLInfo[3] );
			DWORD dwSize = nLInfo[2];
			int nUnComRtn = uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBufAfterCompress, nLInfo[3] );
            ASSERT(Z_OK == nUnComRtn);
			
			
			dwStation = 0;
			BOOL bRutType = FALSE;
			short sIconID = -1;
			CPtrList *pOList = NULL;
			
			while( dwSize > dwStation )
			{
				//取出id
				int nLID = 0;
				Fread( pFBuf, dwSize, dwStation, &nLID, sizeof(int), 1 );
				if( nLID > 4 && nLID < 12 )
					bRutType = TRUE;
				else
					bRutType = FALSE;


				pOList = pFtrList;
                ASSERT(pOList);
				//读出所有ftr
				int nCount;
				Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
                
				BYTE bType = 0;
				int i;
				for( i=0;	i< nCount;	i++ )
				{
					Fread( pFBuf, dwSize, dwStation, &bType, sizeof(BYTE), 1 );
					switch( bType )
					{
					case 1:
						ReadPointFeature( pFBuf, dwSize, dwStation, nBID, sIconID, pOList );
						break;
					case 2:
						{
							if( bRutType )
							{
								ReadRutFeature( pFBuf, dwSize, dwStation, nBID, pOList );
							}
							else
							{
								ReadLineFeature( pFBuf, dwSize, dwStation, nBID, pOList );
							}
						}
						break;
					case 3:
						{
							ReadPolyFeature( pFBuf, dwSize, dwStation, nBID, pOList );
						}
						break;
					default:
						{
//							CString str;
//							str.Format( _T("BAN-MID:%d,BID:%d,LID:%d--类型异常"), pMap->GetMapInfo()->GetMapID(), 
//									pBMap->m_nBMID, pLayer->GetLayerInfo()->GetLayerID() );
//							AfxMessageBox( str );
							ASSERT( FALSE );
							delete[] pFBuf;
							return FALSE;
						}
						break;
					}
				}
			}
			//delete[] pFBuf;
		}
	}
	CATCH( CMemoryException, e )
	{

		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
		return FALSE;
	}
	AND_CATCH( CException, e )
	{

		return FALSE;
	}
	END_CATCH

		
	return FALSE;
}

#ifdef _DEBUG
void CSpaceFileDataV1::DumpState()
{
    TRACE0("********************************************************************\n");
    TRACE1("BreadthCount                : %d.\n",m_BreadthContainer.size());
    

    TRACE0("********************************************************************\n");
    

#ifdef ANALYSE_DATA
    m_DataAnalyse.DumpState();
#endif
	TRACE0("*********************************************************\n\n");


    TRACE0("**********CSpaceFileDataV1 infomation********************\n");
    TRACE1("BreadthCount: %d.\n",m_BreadthContainer.size());
    TRACE1("FtrCount    : %d.\n",GetFtrCount());
    TRACE0("*********************************************************\n\n");
}
void CSpaceFileDataV1::AssertValid() const
{
    
}
#endif

//////////////////////////////////////////////////////////////////////////
unsigned long CSpaceFileDataV1::GetFtrCount()
{

    unsigned long nCount = 0;    
    return nCount;
}

BOOL CSpaceFileDataV1::ClearOperation()
{
    MapOfGeoMap* pMapContainer = m_pMapContainer;
    MapOfGeoMap::iterator iteratorOfGeoMap;
    for(iteratorOfGeoMap = pMapContainer->begin();
    iteratorOfGeoMap != pMapContainer->end();iteratorOfGeoMap++)
    {
        pair <unsigned long ,stuMapItem* > aPair = *iteratorOfGeoMap;
        stuMapItem* pMapItem = aPair.second;
        
        MapOfBreadth* pBreadthContainer = pMapItem->pBreadthContainer;
        MapOfBreadth::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = m_BreadthContainer.begin();
        iteratorOfBreadth != m_BreadthContainer.end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
            stuBreadthItem* pBreadthItem = aPair.second;
            
            MapOfLayer* pLayerContainer = pBreadthItem->pLayerContainer;
            MapOfLayer::iterator iteratorOfLayer;
            for(iteratorOfLayer = pLayerContainer->begin();
            iteratorOfLayer != pLayerContainer->end();iteratorOfLayer++)
            {
                pair <unsigned long ,stuLayerItem* > aPair = *iteratorOfLayer;
                stuLayerItem* pLayerItem = aPair.second;
                
                MapOfFeature* pFtrContainer = pLayerItem->pFtrContainer;
                MapOfFeature::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair <unsigned long ,CGIS_Feature* > aPair = *iteratorOfFtr;
                    CGIS_Feature* pFtr = aPair.second;
                    ASSERT(pFtr);
                    delete pFtr; 
                    pFtr = NULL;
                }
                pFtrContainer->clear();
                delete pFtrContainer;
            }
            pLayerContainer->clear();
            delete pLayerContainer;
        }
        m_BreadthContainer.clear();
        delete pBreadthContainer;
    }
    pMapContainer->clear();
    delete pMapContainer;

    m_pMapContainer = NULL;

    return FALSE;
}

BOOL CSpaceFileDataV1::write(LPCTSTR strFileName)
{
	//写文件
	CMemFile aMemFile;
    WriteSpaceData(&aMemFile);
  
    //////////////////////////////////////////////////////////////////////////
    //写入真实文件
    CFile aFile(strFileName,CFile::modeCreate|CFile::modeReadWrite);
 	unsigned long nSrcSize = aMemFile.GetLength();
	unsigned char* pSrcData = aMemFile.Detach();
	aMemFile.Write(pSrcData,nSrcSize);
	delete []pSrcData;
	pSrcData = NULL;

    return FALSE;
}

BOOL CSpaceFileDataV1::WriteSpaceData(CFile* pFile)
{
    ASSERT(!m_pFileHeadInfo);
    ASSERT(m_pMapContainer);
    MapOfGeoMap* pMapContainer = m_pMapContainer;
    
	//---------------------------
	//构造头,写头
    m_pFileHeadInfo = new StuSpaceFileStructV1 :: StuHeaderInfo;
    StuSpaceFileStructV1 :: StuHeaderInfo* pFileHeadInfo = m_pFileHeadInfo;
    memset(pFileHeadInfo,0,sizeof(StuSpaceFileStructV1 :: StuHeaderInfo));
    strcpy(pFileHeadInfo->chSign,"BAN UNI_1");
    pFileHeadInfo->nMapCount = pMapContainer->size();//(全国图+详图)
    pFile->Write(pFileHeadInfo,sizeof(StuSpaceFileStructV1 :: StuHeaderInfo));
    
    //---------------------------
    int nCount = pFileHeadInfo->nMapCount;
    DWORD dwOffsetBase = pFile->GetPosition();
    //写入地图索引
    StuSpaceFileStructV1::StuIndexInfoOfMap* pIndexInfoOfMap = 
        new StuSpaceFileStructV1::StuIndexInfoOfMap[nCount];
    memset(pIndexInfoOfMap,0,sizeof(StuSpaceFileStructV1::StuIndexInfoOfMap)*nCount);
    
    DWORD IndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfMap,sizeof(StuSpaceFileStructV1::StuIndexInfoOfMap) * nCount);
    
    int nIndexN = 0;
    //////////////////////////////////////////////////////////////////////////
    MapOfGeoMap::iterator iteratorOfGeoMap;
    for(iteratorOfGeoMap = pMapContainer->begin();
    iteratorOfGeoMap != pMapContainer->end();iteratorOfGeoMap++)
    {
        pair <unsigned long ,stuMapItem* > aPair = *iteratorOfGeoMap;
        stuMapItem* pMapItem = aPair.second;

        pMapItem->pIndex->nOffset = pFile->GetPosition() - dwOffsetBase;
        WriteMap(pFile,pMapItem);
        memcpy(&pIndexInfoOfMap[nIndexN],pMapItem->pIndex,sizeof(StuSpaceFileStructV1::StuIndexInfoOfMap));
        nIndexN++;
    }

      
    pFile->Seek(IndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfMap,sizeof(StuSpaceFileStructV1::StuIndexInfoOfMap) * nCount);

    delete pIndexInfoOfMap;
    pIndexInfoOfMap = NULL;
    return FALSE;
}
BOOL CSpaceFileDataV1::WriteMap(
        CFile* pFile,
        stuMapItem* pItem)
{
    ASSERT(pFile);
    ASSERT(pItem);
    StuSpaceFileStructV1 :: StuIndexInfoOfMap* pIndexInfoOfMap = pItem->pIndex;
    ASSERT(pIndexInfoOfMap);
    ASSERTVALID(pIndexInfoOfMap);
    MapOfBreadth* pContainer = pItem->pBreadthContainer;
    ASSERT(pContainer);
//////////////////////////////////////////////////////////////////////////


    DWORD dwOffsetBase = pFile->GetPosition();

    int nCount = pIndexInfoOfMap->nBreadthCount;
    StuSpaceFileStructV1::StuIndexInfoOfBreadth* pIndexInfoOfBreadth 
        = new StuSpaceFileStructV1::StuIndexInfoOfBreadth[nCount];
    memset(pIndexInfoOfBreadth,0,sizeof(StuSpaceFileStructV1::StuIndexInfoOfBreadth)*nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwIndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuSpaceFileStructV1::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////
    
    int nIndexN = 0;
    MapOfBreadth::iterator iteratorOfContainer;
    for(iteratorOfContainer = pContainer->begin();
        iteratorOfContainer != pContainer->end(); iteratorOfContainer++)
        {
            //WATCH(_T("write breadth"));
            pair < unsigned long,stuBreadthItem*> aPair = *iteratorOfContainer;
            stuBreadthItem* pBreadthItem = aPair.second;
          
            pBreadthItem->pIndex->nOffset = pFile->GetPosition() - dwOffsetBase;
            WriteBreadth(pFile,pBreadthItem);
            memcpy(&pIndexInfoOfBreadth[nIndexN],pBreadthItem->pIndex,sizeof(StuSpaceFileStructV1::StuIndexInfoOfBreadth));
            ASSERT(pBreadthItem);
           
            //pIndexInfoOfBreadth[nIndex].nID         = nID;
            //pIndexInfoOfBreadth[nIndex].nOffset     = pFile->GetPosition();
            //pIndexInfoOfBreadth[nIndex].nLayerCount = 1;

            nIndexN++;
            if(!(nIndexN&0xff))
            {
                static DWORD dwTickStart = GetTickCount();
                TRACE2("write breadth %d..%d ms\n",nIndexN,GetTickCount() - dwTickStart);
                dwTickStart = GetTickCount();
            }
        }

    //////////////////////////////////////////////////////////////////////////
    pFile->Seek(dwIndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuSpaceFileStructV1::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////
    
    delete pIndexInfoOfBreadth;
    pIndexInfoOfMap = NULL;
    return TRUE;
    
}
BOOL CSpaceFileDataV1::WriteBreadth(
    CFile* pFile,
    stuBreadthItem* pItem)
{
    ASSERT(pFile);
    ASSERT(pItem);
    ASSERT(pItem->pIndex);
    ASSERTVALID(pItem->pIndex);
    ASSERT(pItem->pLayerContainer);
//////////////////////////////////////////////////////////////////////////
    MapOfLayer* pContainer = pItem->pLayerContainer;
    ASSERT(pContainer);
    StuSpaceFileStructV1 :: StuIndexInfoOfBreadth* pIndexInfoOfBreadth = pItem->pIndex;

    DWORD dwOffsetBase = pFile->GetPosition();
  	//////////////////////////////////////////////////////////////////////////
	//写图幅中的所有层的索引
    int nCount = pIndexInfoOfBreadth->nLayerCount;
	StuSpaceFileStructV1 :: StuIndexInfoOfLayerInBreadthData* pLayerIndex = 
        new StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData[nCount];
    memset(pLayerIndex,0,sizeof(StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData)*nCount);
    //////////////////////////////////////////////////////////////////////////
	DWORD dwIndexPos = pFile->GetPosition();
	pFile->Write(pLayerIndex,sizeof(StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData) * nCount );
	//////////////////////////////////////////////////////////////////////////

    //写图幅中的所有层的数据
    int nIndexN = 0;
    MapOfLayer::iterator iteratorOfContainer;
    for(iteratorOfContainer = pContainer->begin();
    iteratorOfContainer != pContainer->end(); iteratorOfContainer++)
    {
        pair < unsigned long,stuLayerItem*> aPair = *iteratorOfContainer;
        stuLayerItem* pLayerItem = aPair.second;
              
        pLayerItem->pIndex->nOffset = pFile->GetPosition() - dwOffsetBase;
        WriteLayer( pFile, pLayerItem);		
        memcpy(&pLayerIndex[nIndexN],pLayerItem->pIndex,sizeof(StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData));

        
        nIndexN++;
    } 
    //////////////////////////////////////////////////////////////////////////
	pFile->Seek(dwIndexPos,CFile::begin);
	pFile->Write(pLayerIndex,sizeof(StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData) * nCount);
    //////////////////////////////////////////////////////////////////////////
    
	pFile->SeekToEnd();//重新定位到尾部

	delete pLayerIndex;
	pLayerIndex = NULL;

    return FALSE;

 

}
static char s_BuddyBuf [10000000];
static char s_DestBuf2 [10000000];
BOOL CSpaceFileDataV1::WriteLayer(
    CFile* pFile,
    stuLayerItem* pItem)
{
    ASSERT(pFile);
    ASSERT(pItem);
    ASSERT(pItem->pIndex);
    ASSERTVALID(pItem->pIndex);
    ASSERT(pItem->pFtrContainer);
//////////////////////////////////////////////////////////////////////////
    MapOfFeature* pFtrContainer = pItem->pFtrContainer;
    ASSERT(pFtrContainer);
    StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData* pLayerIndex = pItem->pIndex;
      
    StuSpaceFileStructV1::StuHeaderInfoOfLayerInBreadthData* pHeader = 
        new StuSpaceFileStructV1::StuHeaderInfoOfLayerInBreadthData;
    memset(pHeader,0,sizeof(StuSpaceFileStructV1::StuHeaderInfoOfLayerInBreadthData));
    pHeader->nFeatureCount = pFtrContainer->size();
    

    CMemFile aMemFile(1000000);
    {
        CFile* pFile = &aMemFile;        
        //注入层头
        DWORD dwHeaderPos = pFile->GetPosition();
		pFile->Write(pHeader,		sizeof(StuSpaceFileStructV1::StuHeaderInfoOfLayerInBreadthData));

        switch(pLayerIndex->nType)
        {
        case CGIS_Feature::EN_GOTYPE_POINT:
            {
                MapOfRoadSection::iterator iteratorOfRoadSection;
                for(iteratorOfRoadSection = pFtrContainer->begin();
                iteratorOfRoadSection != pFtrContainer->end(); iteratorOfRoadSection++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfRoadSection;
                    CGIS_FeaturePoint* pFtr = (CGIS_FeaturePoint*)aPair.second;
                    
                }
            }
            break;
        case CGIS_Feature::EN_GOTYPE_RUT:
            {
                MapOfRoadSection::iterator iteratorOfRoadSection;
                for(iteratorOfRoadSection = pFtrContainer->begin();
                iteratorOfRoadSection != pFtrContainer->end(); iteratorOfRoadSection++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfRoadSection;
                    CGIS_FeatureRut* pFtr = (CGIS_FeatureRut*)aPair.second;
                    
                }
            }
            break;
        case CGIS_Feature::EN_GOTYPE_LINE:
            {
                MapOfRoadSection::iterator iteratorOfRoadSection;
                for(iteratorOfRoadSection = pFtrContainer->begin();
                iteratorOfRoadSection != pFtrContainer->end(); iteratorOfRoadSection++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfRoadSection;
                    CGIS_FeatureLine* pFtr = (CGIS_FeatureLine*)aPair.second;
                    
                }
            }
            break;
        case CGIS_Feature::EN_GOTYPE_POLY:
            {
                MapOfRoadSection::iterator iteratorOfRoadSection;
                for(iteratorOfRoadSection = pFtrContainer->begin();
                iteratorOfRoadSection != pFtrContainer->end(); iteratorOfRoadSection++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfRoadSection;
                    CGIS_FeaturePoly* pFtr = (CGIS_FeaturePoly*)aPair.second;
                    
                }
            }
            break;
        default:
            ASSERT(FALSE);
            break;
        }
        //////////////////////////////////////////////////////////////////////////
        //在这里要转换所有Ftr的Buddy数据
        {
            char* pBuddy = s_BuddyBuf;
            unsigned long nCharDataBuddyLength;
            unsigned long nShortDataBuddyLength;
            unsigned long nDwordDataBuddyLength;

            unsigned long nBuddyDataSize;
            VERIFY(GetBuddyDataSize(pFtrContainer,pLayerIndex->nType,nBuddyDataSize));
            ASSERT(nBuddyDataSize <= sizeof(s_BuddyBuf));
            
            memset(s_BuddyBuf,0,nBuddyDataSize);
            CollectBuddyData(
                pFtrContainer,
                pBuddy,
                pLayerIndex->nType,
                nCharDataBuddyLength,
                nShortDataBuddyLength,
                nDwordDataBuddyLength);

            ASSERT(nBuddyDataSize = nCharDataBuddyLength+ nShortDataBuddyLength + nDwordDataBuddyLength);
            pFile->Write(s_BuddyBuf,nBuddyDataSize);
                
            pHeader->nCharDataBuddyLength = nCharDataBuddyLength;
            pHeader->nShortDataBuddyLength = nShortDataBuddyLength;
            pHeader->nDwordDataBuddyLength = nDwordDataBuddyLength;

            pFile->Seek(dwHeaderPos,CFile::begin);
            pFile->Write(pHeader,sizeof(StuSpaceFileStructV1::StuHeaderInfoOfLayerInBreadthData));

        }
        //紧跟着写入    
    }

    ASSERTVALID(pHeader);
    //////////////////////////////////////////////////////////////////////////
    

    //////////////////////////////////////////////////////////////////////////
    //压缩数据
	unsigned long nSrcSize = aMemFile.GetLength();
	unsigned char* pSrcData = aMemFile.Detach();
	unsigned char* pDestBuf =(unsigned char*) s_DestBuf2;//new unsigned char[0x1000000];
	unsigned long nDestSize = compressBound(nSrcSize);
	compress2(pDestBuf,&nDestSize,pSrcData,nSrcSize,Z_DEFAULT_COMPRESSION);
	ASSERT(nDestSize > 0);

	pLayerIndex->nBeforeCompressSize  = nSrcSize;
	pLayerIndex->nAfterCompressSize   = nDestSize;

   	delete pHeader;
	pHeader = NULL;
    //////////////////////////////////////////////////////////////////////////
    //真正写入数据
    pFile->Write(pDestBuf,nDestSize);
    delete [] pSrcData;
    pSrcData = NULL;
    return TRUE;

}


BOOL CSpaceFileDataV1::CollectBuddyData(
     MapOfFeature* pFtrContainer,
     char* pBuddy,
     int nType,
     unsigned long& nCharDataBuddyLength,
     unsigned long& nShortDataBuddyLength,
     unsigned long& nDwordDataBuddyLength)
{
    ASSERT(pFtrContainer);
     
    ASSERT(pBuddy);
    nCharDataBuddyLength    = 0;
    nShortDataBuddyLength   = 0;
    nDwordDataBuddyLength   = 0;
        
	int nNum;
	int nSize;
	char* pBuddyData = pBuddy;
	ASSERT(pBuddyData);
	*( (DWORD*)pBuddyData ) = 'y'<<24|'l'<<16|'d'<<8|' '<<0;
	char* pCurrent = pBuddyData+4;


	int nCount = pFtrContainer->size();
	//根据类型处理
	switch(nType)
	{
	case CGIS_Feature::EN_GOTYPE_POINT:
		{
            
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                CGIS_FeaturePoint* pFtr = (CGIS_FeaturePoint*)aPair.second;
                ASSERT(pFtr);
                				
				nNum = pFtr->GetAnnoNum();
				ASSERT(nNum <= 65);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					memcpy(pCurrent,pFtr->GetAnnotation(),nSize);
					pCurrent += nSize;
				}


				nNum = pFtr->GetStrAddressLength();
				ASSERT(nNum <= 255);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					memcpy(pCurrent,pFtr->GetAddress(),nSize);
					pCurrent += nSize;
                }


				nNum = pFtr->GetStrPhoneLength();
				ASSERT(nNum <= 255);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					memcpy(pCurrent,pFtr->GetPhoneNum(),nSize);
					pCurrent += nSize;
				}
									
			}
			//--------------------------------------

			int nCountOfBuddyData = pCurrent - pBuddyData;
			ASSERT(nCountOfBuddyData >= 4);
			nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
			pCurrent = pBuddyData + nCountOfBuddyData;			
			nCharDataBuddyLength = nCountOfBuddyData;

		}

		break;
    case CGIS_Feature::EN_GOTYPE_RUT:
        {
            {
				//由于字符串单项对齐开销太大,所以整合在一起进行对齐

                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRut* pFtr = (CGIS_FeatureRut*)aPair.second;
                    ASSERT(pFtr);
                    //ASSERTVALID(pFtr);
                    
                    nNum = pFtr->GetAnnoNum();
                    ASSERT(nNum <= 64);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(char) * nNum;
                        memcpy(pCurrent,pFtr->GetAnnotation(),nSize);
                        pCurrent += nSize;
                    }             
                }
           	
				int nCountOfBuddyData = pCurrent - pBuddyData;
				ASSERT(nCountOfBuddyData >= 4);
				nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
				pCurrent = pBuddyData + nCountOfBuddyData;
		
				//写长度
				nCharDataBuddyLength = nCountOfBuddyData;

			}

            //////////////////////////////////////////////////////////////////////////
            
			{				
				//把坐标点整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRut* pFtr = (CGIS_FeatureRut*)aPair.second;
                    ASSERT(pFtr);
					
					nNum = pFtr->GetPtNum();
					//ASSERT(nNum < 50);
					if( nNum > 0 )
					{
						nSize = sizeof(float) * 2 * nNum;
						memcpy(pCurrent,pFtr->GetPtList(),nSize);
						pCurrent += nSize;
					}

				}

				int nCountOfBuddyData = pCurrent - pBuddyData;
				ASSERT(nCountOfBuddyData >= 4);
				nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
				pCurrent = pBuddyData + nCountOfBuddyData;

				nDwordDataBuddyLength = nCountOfBuddyData - nCharDataBuddyLength;

			}


			//--------------------------------------
		}
		break;
	case CGIS_Feature::EN_GOTYPE_LINE:
		{
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                CGIS_FeatureLine* pFtr = (CGIS_FeatureLine*)aPair.second;
                ASSERT(pFtr);
                
                nNum = pFtr->GetAnnoNum();
                ASSERT(nNum <= 64);
                if( nNum > 0 )
                {
                    nSize = sizeof(char) * nNum;
                    memcpy(pCurrent,pFtr->GetAnnotation(),nSize);
                    pCurrent += nSize;
                }
                
            }
            {
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
                //写偏移
                nCharDataBuddyLength = nCountOfBuddyData;
                
            }

			
			//////////////////////////////////////////////////////////////////////////

			{			
				//把dword数据整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                    CGIS_FeatureLine* pFtr = (CGIS_FeatureLine*)aPair.second;
                    ASSERT(pFtr);
                    
					nNum = pFtr->GetPtNum();
					//ASSERT(nNum < 11);
					if( nNum > 0 )
					{
						nSize = sizeof(float) * 2 * nNum;
						memcpy(pCurrent,pFtr->GetPtList(),nSize);
						pCurrent += nSize;
					}

					
					nNum = pFtr->GetPartNum();
					//ASSERT(nNum < 11);
					if( nNum > 0 )
					{
						nSize = sizeof(int) * nNum;
						memcpy(pCurrent,pFtr->GetPart(),nSize);
						pCurrent += nSize;
					}

				}

				int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
                nDwordDataBuddyLength = nCountOfBuddyData - nCharDataBuddyLength;
                
            }
            
            //--------------------------------------
        }
        break;
    case CGIS_Feature::EN_GOTYPE_POLY:
        {
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                CGIS_FeaturePoly* pFtr = (CGIS_FeaturePoly*)aPair.second;
                ASSERT(pFtr);
                
                nNum = pFtr->GetAnnoNum();
                ASSERT(nNum <= 64);
                if( nNum > 0 )
                {
                    nSize = sizeof(char) * nNum;
                    memcpy(pCurrent,pFtr->GetAnnotation(),nSize);
                    pCurrent += nSize;
                    
                }
            }
            {
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
                //写偏移
                nCharDataBuddyLength = nCountOfBuddyData;
                
            }
            
            
            //////////////////////////////////////////////////////////////////////////
            
            {
                
                //把dword数据整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                    CGIS_FeaturePoly* pFtr = (CGIS_FeaturePoly*)aPair.second;
                    ASSERT(pFtr);
                    
                    nNum = pFtr->GetPtNum();
                    //ASSERT(nNum < 11);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(float) * 2 * nNum;
                        memcpy(pCurrent,pFtr->GetPtList(),nSize);
                        pCurrent += nSize;
                    }
                    else
                        
                        
                        nNum = pFtr->GetPartNum();
                    //ASSERT(nNum < 11);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(int) * nNum;
                        memcpy(pCurrent,pFtr->GetPart(),nSize);
                        pCurrent += nSize;
                    }
                    
                }
                
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
                nDwordDataBuddyLength = nCountOfBuddyData - nCharDataBuddyLength;
                
            }
            
            
        }
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	int nCountOfBuddyData = pCurrent - pBuddyData;
	ASSERT(nCountOfBuddyData >= 4);
	ASSERT( nCountOfBuddyData == ((nCountOfBuddyData + 0x03)&0xfffffffc)	);//以4为边界对齐
	ASSERT( nCountOfBuddyData == nCharDataBuddyLength + nShortDataBuddyLength + nDwordDataBuddyLength);

	return TRUE;
}

BOOL CSpaceFileDataV1::GetBuddyDataSize( 
                      MapOfFeature* pFtrContainer,
                      int nType,
                      unsigned long& nBuddyDataSize)
{
    ASSERT(pFtrContainer);
     
        
	int nNum;
	int nSize;
	char* pBuddyData = NULL;
	char* pCurrent = pBuddyData+4;


	int nCount = pFtrContainer->size();
	//根据类型处理
	switch(nType)
	{
	case CGIS_Feature::EN_GOTYPE_POINT:
		{
            
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                CGIS_FeaturePoint* pFtr = (CGIS_FeaturePoint*)aPair.second;
                ASSERT(pFtr);
                				
				nNum = pFtr->GetAnnoNum();
				ASSERT(nNum <= 65);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					pCurrent += nSize;
				}


				nNum = pFtr->GetStrAddressLength();
				ASSERT(nNum <= 255);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					pCurrent += nSize;
                }


				nNum = pFtr->GetStrPhoneLength();
				ASSERT(nNum <= 255);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					pCurrent += nSize;
				}
									
			}
			//--------------------------------------

			int nCountOfBuddyData = pCurrent - pBuddyData;
			ASSERT(nCountOfBuddyData >= 4);
			nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
			pCurrent = pBuddyData + nCountOfBuddyData;			

		}

		break;
    case CGIS_Feature::EN_GOTYPE_RUT:
        {
            {
				//由于字符串单项对齐开销太大,所以整合在一起进行对齐

                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRut* pFtr = (CGIS_FeatureRut*)aPair.second;
                    ASSERT(pFtr);
                    
                    nNum = pFtr->GetAnnoNum();
                    ASSERT(nNum <= 100);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(char) * nNum;
                        pCurrent += nSize;
                    }             
                }
           	
				int nCountOfBuddyData = pCurrent - pBuddyData;
				ASSERT(nCountOfBuddyData >= 4);
				nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
				pCurrent = pBuddyData + nCountOfBuddyData;
		
				//写长度

			}

            //////////////////////////////////////////////////////////////////////////
            
			{				
				//把坐标点整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRut* pFtr = (CGIS_FeatureRut*)aPair.second;
                    ASSERT(pFtr);
					
					nNum = pFtr->GetPtNum();
					//ASSERT(nNum < 50);
					if( nNum > 0 )
					{
						nSize = sizeof(float) * 2 * nNum;
						pCurrent += nSize;
					}

				}

				int nCountOfBuddyData = pCurrent - pBuddyData;
				ASSERT(nCountOfBuddyData >= 4);
				nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
				pCurrent = pBuddyData + nCountOfBuddyData;


			}


			//--------------------------------------
		}
		break;
	case CGIS_Feature::EN_GOTYPE_LINE:
		{
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                CGIS_FeatureLine* pFtr = (CGIS_FeatureLine*)aPair.second;
                ASSERT(pFtr);
                
                nNum = pFtr->GetAnnoNum();
                ASSERT(nNum <= 64);
                if( nNum > 0 )
                {
                    nSize = sizeof(char) * nNum;
                    pCurrent += nSize;
                }
                
            }

            {
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                                
            }

			
			//////////////////////////////////////////////////////////////////////////

			{			
				//把dword数据整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                    CGIS_FeatureLine* pFtr = (CGIS_FeatureLine*)aPair.second;
                    ASSERT(pFtr);
                    
					nNum = pFtr->GetPtNum();
					//ASSERT(nNum < 11);
					if( nNum > 0 )
					{
						nSize = sizeof(float) * 2 * nNum;
						pCurrent += nSize;
					}

					
					nNum = pFtr->GetPartNum();
					//ASSERT(nNum < 11);
					if( nNum > 0 )
					{
						nSize = sizeof(int) * nNum;
						pCurrent += nSize;
					}

				}

				int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                                
            }
            
            //--------------------------------------
        }
        break;
    case CGIS_Feature::EN_GOTYPE_POLY:
        {
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                CGIS_FeaturePoly* pFtr = (CGIS_FeaturePoly*)aPair.second;
                ASSERT(pFtr);
                
                nNum = pFtr->GetAnnoNum();
                ASSERT(nNum <= 64);
                if( nNum > 0 )
                {
                    nSize = sizeof(char) * nNum;
                    pCurrent += nSize;
                    
                }
            }
            
            {
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
            }
            
                
                //////////////////////////////////////////////////////////////////////////
            
            {
                
                //把dword数据整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_Feature*> aPair = *iteratorOfFtr;
                    CGIS_FeaturePoly* pFtr = (CGIS_FeaturePoly*)aPair.second;
                    ASSERT(pFtr);
                    
                    nNum = pFtr->GetPtNum();
                    //ASSERT(nNum < 11);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(float) * 2 * nNum;
                        pCurrent += nSize;
                    }
                    else
                        
                        
                        nNum = pFtr->GetPartNum();
                    //ASSERT(nNum < 11);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(int) * nNum;
                        pCurrent += nSize;
                    }
                    
                }
                
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
            }
                
            
        }
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	int nCountOfBuddyData = pCurrent - pBuddyData;
	ASSERT(nCountOfBuddyData >= 4);
	ASSERT( nCountOfBuddyData == ((nCountOfBuddyData + 0x03)&0xfffffffc)	);//以4为边界对齐

    nBuddyDataSize = nCountOfBuddyData;

	return TRUE;
}

BOOL  ReadPointFeature( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, short sIconID, CPtrList* pOList )
{
	unsigned short nPOIType = 0;
	ST_GEO_PTXY *pPt = NULL;
	BYTE nAddrLen = 0;
	char *szAddr = NULL;
	BYTE nPhoneLen = 0;
	char *szPhone = NULL;
	int nAnno = 0;
	char *szAnno = NULL;
	CGIS_FeaturePoint *pPoint = NULL;
	TRY
	{
		Fread( pFBuf, dwSize, dwStation, &nPOIType, sizeof(unsigned short), 1 );

		Fread( pFBuf, dwSize, dwStation, &nAddrLen, sizeof(BYTE), 1 );
		if( nAddrLen > dwSize-dwStation )
			return FALSE;
		if( nAddrLen > 0 )
		{
			szAddr = new char[nAddrLen+1];
			Fread( pFBuf, dwSize, dwStation, szAddr, sizeof(char)*nAddrLen, 1 );
			szAddr[nAddrLen] = '\0';
			nAddrLen += 1;
		}

		Fread( pFBuf, dwSize, dwStation, &nPhoneLen, sizeof(BYTE), 1 );
		if( nPhoneLen > dwSize-dwStation )
		{
			if( szAddr )
				delete[] szAddr;
			return FALSE;
		}
		if( nPhoneLen > 0 )
		{
			szPhone = new char[nPhoneLen+1];
			Fread( pFBuf, dwSize, dwStation, szPhone, sizeof(char)*nPhoneLen, 1 );
			szPhone[nPhoneLen] = '\0';
			nPhoneLen += 1;
		}

		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
		if( nAnno > dwSize-dwStation )
		{
			if( szAddr )
				delete[] szAddr;
			if( szPhone )
				delete[] szPhone;
			return FALSE;
		}
		if( nAnno > 0 )
		{
			szAnno = new char[nAnno+1];
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
			szAnno[nAnno] = '\0';
			nAnno += 1;
		}
		pPt = new ST_GEO_PTXY;
//		if( m_bNation )
			Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY), 1 );
//		else
//		{
//			unsigned short nVal;
//			Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//			pPt->dx = BMAP_ORIGN_POS.dx + nVal/100000.0;
//			Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//			pPt->dy = BMAP_ORIGN_POS.dy + nVal/100000.0;
//		}

			pPoint = new CGIS_FeaturePoint( CGIS_Feature::EN_GOTYPE_POINT ); 
		if( szAnno )
			pPoint->SetAnnotation( szAnno );
		pPoint->SetPtList( pPt );
		pPoint->SetIconID( sIconID );
		pPoint->SetAnnoNum( nAnno );
		pPoint->SetAddress( szAddr );
		pPoint->SetStrAddressLength( nAddrLen );
		pPoint->SetPhoneNum( szPhone );
		pPoint->SetStrPhoneLength( nPhoneLen );
		pPoint->SetPointObjectType( nPOIType );
		pPoint->m_nBreadthID = nBreadthID;
	}
	CATCH( CMemoryException, e )
	{
		if( pPoint )
			delete pPoint;
		else
		{
			if( pPt )
				delete pPt;
			if( szAnno )
				delete[] szAnno;
			if( szAddr )
				delete[] szAddr;
			if( szPhone )
				delete[] szPhone;
		}
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	pOList->AddTail( pPoint );
	return TRUE;
}

BOOL  ReadLineFeature( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, CPtrList* pOList )
{
	int nPtNum = 0, nPartNum = 0, nAnno = 0;

	ST_GEO_PTXY *pPt = NULL;
	int *pPart = NULL;
	char *szAnno = NULL;
	CGIS_FeatureLine *pLine = NULL;
	TRY
	{
		Fread( pFBuf, dwSize, dwStation, &nPtNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nPartNum, sizeof(int), 1 );
		if( nPartNum > (dwSize-dwStation)/4 )
			return FALSE;
		if( nPartNum > 0 )
		{
			pPart = new int[nPartNum];
			if( nPartNum == 1 )
			{
				Fread( pFBuf, dwSize, dwStation, pPart, sizeof(int)*nPartNum, 1 );
				pPart[0] = nPtNum;
			}
			else
			{
				int *pTemp = new int[nPartNum];
				Fread( pFBuf, dwSize, dwStation, pTemp, sizeof(int)*nPartNum, 1 );
				int i;
				for(  i=0;	i< nPartNum-1;	i++ )
					pPart[i] = pTemp[i+1]-pTemp[i];
				pPart[i] = nPtNum - pTemp[i];
				delete[] pTemp;
			}
		}

		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
		if( nAnno > dwSize-dwStation )
		{
			if( pPart )
				delete[] pPart;
			return FALSE;
		}
		if( nAnno > 0 )
		{
			szAnno = new char[nAnno+1];
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
			szAnno[nAnno] = '\0';
			nAnno += 1;
		}
		if( nPtNum <= 0 )
		{
			if( szAnno )
				delete[] szAnno;
			if( nPartNum >= 1 )
				delete[] pPart;
			return FALSE;
		}
		if( nPtNum > (dwSize-dwStation)/4 )
		{
			if( pPart )
				delete[] pPart;
			if( szAnno )
				delete[] szAnno;
			return FALSE;
		}
		pPt = new ST_GEO_PTXY[nPtNum];
//		if( m_bNation )
			Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nPtNum, 1 );
//		else
//		{
//			unsigned short nVal;
//			for( int i=0;	i<nPtNum;	i++ )
//			{
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dx = BMAP_ORIGN_POS.dx + nVal/100000.0;
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dy = BMAP_ORIGN_POS.dy + nVal/100000.0;
//			}
//		}

		if( nPtNum <= 1 )
		{
			if( szAnno )
				delete[] szAnno;
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			return FALSE;
		}
		pLine = new CGIS_FeatureLine( CGIS_Feature::EN_GOTYPE_LINE );
		if( szAnno )
			pLine->SetAnnotation( szAnno );
		pLine->SetPart( pPart );
		pLine->SetPartNum( nPartNum );
		pLine->SetPtList( pPt );
		pLine->SetPtNum( nPtNum );
		ST_GEO_RECT rtObj;
		CalcRect( pPt, nPtNum, rtObj );
		pLine->SetRectObj( rtObj );
		pLine->SetAnnoNum( nAnno );
		pLine->m_nBreadthID = nBreadthID;
	//	pLayer->GetObjectList()->AddTail( pLine );
	}
	CATCH( CMemoryException, e )
	{
		if( pLine )
			delete pLine;
		else
		{
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			if( szAnno )
				delete[] szAnno;
		}
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	pOList->AddTail( pLine );
	return TRUE;
}

BOOL  ReadPolyFeature( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, CPtrList* pOList )
{
	int nPtNum = 0, nPartNum = 0, nAnno = 0;
	ST_GEO_PTXY *pPt = NULL;
	int *pPart = NULL;
	char *szAnno = NULL;
	CGIS_FeaturePoly *pPoly = NULL;
	TRY
	{
		Fread( pFBuf, dwSize, dwStation, &nPtNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nPartNum, sizeof(int), 1 );

		if( nPartNum > (dwSize-dwStation)/4 )
			return FALSE;
		if( nPartNum > 0 )
		{
			pPart = new int[nPartNum];
			if( nPartNum == 1 )
			{
				Fread( pFBuf, dwSize, dwStation, pPart, sizeof(int)*nPartNum, 1 );
				pPart[0] = nPtNum;
			}
			else
			{
				int *pTemp = new int[nPartNum];
				Fread( pFBuf, dwSize, dwStation, pTemp, sizeof(int)*nPartNum, 1 );
				int i;
				for( i=0;	i< nPartNum-1;	i++ )
					pPart[i] = pTemp[i+1]-pTemp[i];
				pPart[i] = nPtNum - pTemp[i];
				delete[] pTemp;
			}
		}

		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
		if( nAnno > dwSize-dwStation )
		{
			if( pPart )
				delete[] pPart;
			return FALSE;
		}
		if( nAnno > 0 )
		{
			szAnno = new char[nAnno+1];
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
			szAnno[nAnno] = '\0';
			nAnno += 1;
		}

		BYTE nShowClass = 0;
		Fread( pFBuf, dwSize, dwStation, &nShowClass, sizeof(BYTE), 1 );
//		if( m_bNation )
//			nShowClass = 9;

		if( nPtNum <= 0 )
		{
			if( szAnno )
				delete[] szAnno;
			if( nPartNum >= 1 )
				delete[] pPart;
			return FALSE;
		}
		if( nPtNum > (dwSize-dwStation)/4 )
		{
			if( pPart )
				delete[] pPart;
			if( szAnno )
				delete[] szAnno;
			return FALSE;
		}
		pPt = new ST_GEO_PTXY[nPtNum];
//		if( m_bNation )
			Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nPtNum, 1 );
//		else
//		{
//			unsigned short nVal;
//			for( int i=0;	i<nPtNum;	i++ )
//			{
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dx = BMAP_ORIGN_POS.dx + nVal/100000.0;
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dy = BMAP_ORIGN_POS.dy + nVal/100000.0;
//			}
//		}
		if( nPtNum <= 2 )
		{
			if( szAnno )
				delete[] szAnno;
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			return FALSE;
		}
		pPoly = new CGIS_FeaturePoly( CGIS_Feature::EN_GOTYPE_POLY );
		if( szAnno )
			pPoly->SetAnnotation( szAnno );
		pPoly->SetPart( pPart );
		pPoly->SetPartNum( nPartNum );
		pPoly->SetPtList( pPt );
		pPoly->SetPtNum( nPtNum );
		ST_GEO_RECT rtObj;
		CalcRect( pPt, nPtNum, rtObj );
		pPoly->SetRectObj( rtObj );
		pPoly->SetCenterPt( float((rtObj.dLeft+rtObj.dRight)/2.0), float((rtObj.dTop+rtObj.dBottom)/2.0) );
		pPoly->SetAnnoNum( nAnno );
		pPoly->m_nBreadthID = nBreadthID;
		pPoly->SetPolyShowClass( nShowClass );
	//	pLayer->GetObjectList()->AddTail( pPoly );
	}
	CATCH( CMemoryException, e )
	{
		if( pPoly )
			delete pPoly;
		else
		{
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			if( szAnno )
				delete[] szAnno;
		}
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	pOList->AddTail( pPoly );
	return TRUE;
}
BOOL  ReadRutFeature( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, CPtrList* pOList )
{
	int nPtNum = 0, nPartNum = 0, nAnno = 0;
	int nID = 0;
	BYTE byDirection = 0;
	ST_GEO_PTXY *pPt = NULL;
	int *pPart = NULL;
	char *szAnno = NULL;
	CGIS_FeatureRut *pRut = NULL;
	TRY
	{
		Fread( pFBuf, dwSize, dwStation, &nPtNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nPartNum, sizeof(int), 1 );

		if( nPartNum > (dwSize-dwStation)/4 )
			return FALSE;
		if( nPartNum > 0 )
		{
			pPart = new int[nPartNum];
			if( nPartNum == 1 )
			{
				Fread( pFBuf, dwSize, dwStation, pPart, sizeof(int)*nPartNum, 1 );
				pPart[0] = nPtNum;
			}
			else
			{
				int *pTemp = new int[nPartNum];
				Fread( pFBuf, dwSize, dwStation, pTemp, sizeof(int)*nPartNum, 1 );
				int i;
				for( i=0;	i< nPartNum-1;	i++ )
					pPart[i] = pTemp[i+1]-pTemp[i];
				pPart[i] = nPtNum - pTemp[i];
				delete[] pTemp;
			}
		}
		Fread( pFBuf, dwSize, dwStation, &nID, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &byDirection, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
		if( nAnno > dwSize-dwStation )
		{
			if( pPart )
				delete[] pPart;
			return FALSE;
		}
		if( nAnno > 0 )
		{
			szAnno = new char[nAnno+1];
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
			szAnno[nAnno] = '\0';
			nAnno += 1;
		}
		if( nPtNum <= 0 )
		{
			if( szAnno )
				delete[] szAnno;
			if( nPartNum >= 1 )
				delete[] pPart;
			return FALSE;
		}
		if( nPtNum > (dwSize-dwStation)/4 )
		{
			if( pPart )
				delete[] pPart;
			if( szAnno )
				delete[] szAnno;
			return FALSE;
		}
		pPt = new ST_GEO_PTXY[nPtNum];
//		if( m_bNation )
			Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nPtNum, 1 );
//		else
//		{
//			unsigned short nVal;
//			for( int i=0;	i<nPtNum;	i++ )
//			{
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dx = BMAP_ORIGN_POS.dx + nVal/100000.0;
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dy = BMAP_ORIGN_POS.dy + nVal/100000.0;
//			}
//		}
		if( nPtNum <= 1 )
		{
			if( szAnno )
				delete[] szAnno;
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			return FALSE;
		}
		pRut = new CGIS_FeatureRut( CGIS_Feature::EN_GOTYPE_RUT );
		if( szAnno )
			pRut->SetAnnotation( szAnno );
		if( pPart )
			delete[] pPart;
		pPart = NULL;
		nPartNum = 0;
		pRut->SetPtList( pPt );
		pRut->SetPtNum( nPtNum );
		ST_GEO_RECT rtObj;
		CalcRect( pPt, nPtNum, rtObj );
		pRut->SetRectObj( rtObj );
		pRut->SetAnnoNum( nAnno );
		pRut->SetRutID( nID );
        ASSERT(nID < 20000);
		pRut->SetRutDirection( byDirection );
		pRut->m_nBreadthID = nBreadthID;
	}
	CATCH( CMemoryException, e )
	{
		if( pRut )
			delete pRut;
		else
		{
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			if( szAnno )
				delete[] szAnno;
		}
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	pOList->AddTail( pRut );
	return TRUE;
}

BOOL CSpaceFileDataV1::ReadCityPosData(CFile* pFile)
{
    ASSERT(!m_pMapContainer);
    m_pMapContainer = new MapOfGeoMap;

    CPtrList aListOfShengHui;
    CPtrList aListOfDiJiShi;



	//读头
    StuSpaceFileStructV1 :: StuHeaderInfo aFileHeadInfo;
    StuSpaceFileStructV1 :: StuHeaderInfo* pFileHead = &aFileHeadInfo;
    memset(pFileHead,0,sizeof(StuSpaceFileStructV1 :: StuHeaderInfo));

    pFile->SeekToBegin();
    pFile->Read( pFileHead->chSign, 9 );
		
    int nMNum = 0;
    int nMID = 0;
    int nMOff = 0;
    pFile->Read( &nMNum, sizeof(int) );//读地图的数量
    pFileHead->nMapCount = nMNum;
     

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////
    

	int nCount = pFileHead->nMapCount;

	//---------------------------
	//读入地图索引
	StuSpaceFileStructV1::StuIndexInfoOfMap* pIndexInfoOfMap = new StuSpaceFileStructV1::StuIndexInfoOfMap[nCount];
	memset(pIndexInfoOfMap,0,sizeof(StuSpaceFileStructV1::StuIndexInfoOfMap)*nCount);
	int i;
    for( i=0;	i<nMNum;	i++ )
    {
        pFile->Read( &nMID, sizeof(int) );
        pFile->Read( &nMOff, sizeof(int) );
        pIndexInfoOfMap[i].nID = nMID;
        pIndexInfoOfMap[i].nOffset = nMOff;
        
        CTempGeoMap* pMap = (CTempGeoMap*)m_ListOfMap.GetAt(m_ListOfMap.FindIndex(i));
        ASSERT(pMap);
        pIndexInfoOfMap[i].nBreadthCount		= pMap->GetBreadthMapNum();
     }	
	//------------------------------------------------------------------------
	//读入地图
	for( i = 1; i < nCount; i++)
	{
        stuMapItem* pMapItem = new stuMapItem;
        
        pMapItem->pBreadthContainer = new MapOfBreadth;
        pMapItem->pIndex = &pIndexInfoOfMap[i];
        pMapItem->nMapID = i;

        {
            int nCount = pMapItem->pIndex->nBreadthCount;
            for(int i = 0; i < nCount; i++)
            {                
                stuBreadthItem* pBreadthItem = new stuBreadthItem;
                pBreadthItem->pLayerContainer = new MapOfLayer;
                pBreadthItem->pIndex = NULL;
                pBreadthItem->nMapID        = pMapItem->nMapID;
                pBreadthItem->nBreadthID    = i;
                
                ReadLayerAllInfoInBreadth(pFile,pMapItem->nMapID,i);
                
                {
                    //读入图层数据
                    for(int i = 24 ; i < 25; i++)
                    {
                        stuLayerItem* pLayerItem = new stuLayerItem;
                        pLayerItem->pFtrContainer = new MapOfFeature;
                        pLayerItem->pIndex = NULL;
                        pLayerItem->nMapID      = pBreadthItem->nMapID;
                        pLayerItem->nBreadthID  = pBreadthItem->nBreadthID;
                        pLayerItem->nLayerID    = i;
                        //if(pIndexOfLayer->nID == 10 || pIndexOfLayer->nID == 11)
                        {
                            CPtrList aList;
                            ReadLayerV1(pFile,pLayerItem->nMapID,pLayerItem->nBreadthID,pLayerItem->nLayerID,&aList);
                            
                            
                            //////////////////////////////////////////////////////////////////////////
                            {
                                POSITION pos = aList.GetHeadPosition();
                                while(pos)
                                {
                                    POSITION posOfCurrentFtr = pos;
                                    CGIS_Feature* pFtr = (CGIS_Feature*)aList.GetNext(pos);
                                    CGIS_FeaturePoint* pPoint = (CGIS_FeaturePoint*)pFtr;
                                    switch(pPoint->GetPointObjectType())
                                    {
                                    case 1:
                                        {
                                            BOOL bAdd = TRUE;
                                            POSITION posSub = aListOfShengHui.GetHeadPosition();
                                            while(posSub)
                                            {
                                                 CGIS_FeaturePoint* pPointSub = (CGIS_FeaturePoint*)aListOfShengHui.GetNext(posSub);
                                                 if(CompareFtrPoint(pPoint,pPointSub))
                                                 {
                                                     bAdd = FALSE;
                                                 }

                                            }
                                            if(bAdd)
                                            {                                               
                                                aList.RemoveAt(posOfCurrentFtr);
                                                aListOfShengHui.AddTail(pPoint);
                                            }
                                            //CString strText(pPoint->GetAnnotation(),pPoint->GetAnnoNum());
                                            //TRACE3("省会:%s[%f-%f]\n", strText,pPoint->GetPtList()->dx,pPoint->GetPtList()->dy);    
                                        }

                                        break;
                                    case 2:
                                        {
                                            BOOL bAdd = TRUE;
                                            POSITION posSub = aListOfDiJiShi.GetHeadPosition();
                                            while(posSub)
                                            {
                                                 CGIS_FeaturePoint* pPointSub = (CGIS_FeaturePoint*)aListOfDiJiShi.GetNext(posSub);
                                                 if(CompareFtrPoint(pPoint,pPointSub))
                                                 {
                                                     bAdd = FALSE;
                                                 }

                                            }
                                            if(bAdd)
                                            {
                                                aList.RemoveAt(posOfCurrentFtr);
                                                aListOfDiJiShi. AddTail(pPoint);
                                            }
//                                            CString strText(pPoint->GetAnnotation(),pPoint->GetAnnoNum());
//                                            TRACE3("地级市:%s[%f-%f]\n",strText,pPoint->GetPtList()->dx,pPoint->GetPtList()->dy);      
                                        }
                                        break;
                                    default:
                                        break;
                                    }
                                }   
                            }
                            //////////////////////////////////////////////////////////////////////////
                            
                            POSITION pos = aList.GetHeadPosition();
                            while(pos)
                            {
                                CGIS_Feature* pFtr = (CGIS_Feature*)aList.GetNext(pos);
                                delete pFtr;
                            }
                            
                        }
                    }
                }
                if(pBreadthItem->pLayerContainer->size())
                {
                    m_BreadthContainer[pBreadthItem->pIndex->nID] = pBreadthItem;
                }
                else
                {
                    delete pBreadthItem->pLayerContainer;
                    //TRACE1("图幅[%d]没有路段数据.\n",i);
                }
            }


        }
        (*m_pMapContainer)[pMapItem->nMapID] = pMapItem;
	}
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	//清理工作
	delete pIndexInfoOfMap;
	pIndexInfoOfMap = NULL;

   {
//        POSITION pos = aListOfShengHui.GetHeadPosition();
//        while(pos)
//        {
//            CGIS_FeaturePoint* pPoint = (CGIS_FeaturePoint*)aListOfShengHui.GetNext(pos);
//            CString strText(pPoint->GetAnnotation(),pPoint->GetAnnoNum());
//            TRACE3("省会:%s[%f-%f]\n", strText,pPoint->GetPtList()->dx,pPoint->GetPtList()->dy);    
//            delete pPoint;
//        }
    }
    {
        POSITION pos = aListOfDiJiShi.GetHeadPosition();
        while(pos)
        {
            CGIS_FeaturePoint* pPoint = (CGIS_FeaturePoint*)aListOfDiJiShi.GetNext(pos);
            CString strText(pPoint->GetAnnotation(),pPoint->GetAnnoNum());
            TRACE3("地级市:%s[%f-%f]\n", strText,pPoint->GetPtList()->dx,pPoint->GetPtList()->dy);    
            delete pPoint;
        }
    }

	
	return TRUE;
}

#ifdef ANALYSE_DATA
void CSpaceFileDataV1::CDataAnalyse::Initial()
{
    //关于图幅的
    m_nBreadthCount = 0;
    m_nEmptyBreadthCount = 0;
    m_nPackageCount = 0;
    m_nEmptyPackageCount = 0;
    
    m_nMinRoadSectionCountOfBreadth = 0xffffffff;
    m_nMaxRoadSectionCountOfBreadth = 0;

    m_dwMinBeforeCompressSize   = 0xffffffff;
    m_dwMaxBeforeCompressSize   = 0;
    m_dwMinAfterCompressSize    = 0xffffffff;
    m_dwMaxAfterCompressSize    = 0;
      
    memset(nCountOfSpecialSizePackageBeforeCompress,0,sizeof(nCountOfSpecialSizePackageBeforeCompress));
    memset(nCountOfSpecialSizePackageAfterCompress, 0,sizeof(nCountOfSpecialSizePackageAfterCompress));

}
void CSpaceFileDataV1::CDataAnalyse::DumpState()
{
	TRACE0("********************************************************************\n");
	TRACE0("CSpaceFileDataV1----\n");
	TRACE1("图幅                            :%d.\n",m_nBreadthCount);
	TRACE1("空图幅                          :%d.\n",m_nEmptyBreadthCount);
    TRACE1("MinRoadSectionCountOfBreadth    :%d.\n",m_nMinRoadSectionCountOfBreadth);
    TRACE1("MaxRoadSectionCountOfBreadth    :%d.\n",m_nMaxRoadSectionCountOfBreadth);
    TRACE1("MinBeforeCompressSize           :%d.\n",m_dwMinBeforeCompressSize);
    TRACE1("MaxBeforeCompressSize           :%d.\n",m_dwMaxBeforeCompressSize);
    TRACE1("MinAfterCompressSize            :%d.\n",m_dwMinAfterCompressSize);
    TRACE1("MaxAfterCompressSize            :%d.\n",m_dwMaxAfterCompressSize);

    {
        TRACE0("nCountOfSpecialSizePackageBeforeCompress:\n");
        for(int i = 0; i < 10; i++)
        {
            TRACE3("[%.6dk--%.6dk)          :%d .\n",i*10,(i+1)*10,nCountOfSpecialSizePackageBeforeCompress[i]);
        }
        TRACE1(">100k:          :%d .\n",nCountOfSpecialSizePackageBeforeCompress[10]);
    }
    {
        
        TRACE0("nCountOfSpecialSizePackageAfterCompress:\n");
        for(int i = 0; i < 10; i++)
        {
            TRACE3("[%.6dk--%.6dk)          :%d .\n",i*10,(i+1)*10,nCountOfSpecialSizePackageAfterCompress[i]);
        }
        TRACE1(">100k:          :%d .\n",nCountOfSpecialSizePackageAfterCompress[10]);
    }

    TRACE0("********************************************************************\n");
    

}

#endif