
#include "GIS_MapDataManager.h"
#include "GIS_GeoMap.h"
#include "GIS_UserMap.h"
#include "GIS_MFBand.h"   //scott  包含此文件编译错误
#include "SDEControler.h"
#include "MemoryDispatchForSpaceDataV2.h"
#include "GeoView.h"
static UINT CALLBACK UpdateProc(CMemoryDispatchForSpaceDataV2::enUpdateAspect eAspect, DWORD dwPara);

CGIS_MapDataManager::CGIS_MapDataManager(){
	
	//m_pCountrySet = new CPtrList;
	//m_pPOITypeSet = new CPtrList;
	//g_pSpaceMemEvent = new CEvent;
	//g_pSpaceMemEvent->SetEvent();
	//g_pQueryMemEvent = new CEvent;
	//g_pQueryMemEvent->SetEvent();

    CSDEControler* pSDEControl = CSDEControler::GetSDEControl();
    //ASSERT(pSDEControl);
    CMemoryDispatchForSpaceDataV2 *pMemoryMngDispatch = pSDEControl->GetMemoryDispatchForSpace();
    //ASSERT( pMemoryMngDispatch );
    pMemoryMngDispatch->Observe(UpdateProc);
}

CGIS_MapDataManager::~CGIS_MapDataManager(){
	/*
	if( m_pCountrySet )
	{
		ST_CCOUNTRY_INFO *pCountryInfo = NULL;
		POSITION pos = m_pCountrySet->GetHeadPosition();
		while( pos )
		{
			pCountryInfo = (ST_CCOUNTRY_INFO*)m_pCountrySet->GetNext( pos );
			if( !pCountryInfo )
				continue;
			ST_CPRO_INFO *pProInfo = NULL;
			ST_CCITY_INFO *pCityInfo = NULL;
			if( pCountryInfo->PList )
			{
				POSITION posPro = pCountryInfo->PList->GetHeadPosition();
				while( posPro )
				{
					pProInfo = (ST_CPRO_INFO*)pCountryInfo->PList->GetNext( posPro );
					if( pProInfo )
					{
						delete pProInfo;
						pProInfo = NULL;
					}
				}
				delete pCountryInfo->PList;
				pCountryInfo->PList = NULL;
			}
			if( pCountryInfo->CityList )
			{
				POSITION posCity = pCountryInfo->CityList->GetHeadPosition();
				while( posCity )
				{
					pCityInfo = (ST_CCITY_INFO*)pCountryInfo->CityList->GetNext( posCity );
					if( pCityInfo )
					{
						delete pCityInfo;
						pCityInfo = NULL;
					}
				}
				delete pCountryInfo->CityList;
				pCountryInfo->CityList = NULL;
			}
			delete pCountryInfo;
			pCountryInfo = NULL;
		}
		delete m_pCountrySet;
		m_pCountrySet = NULL;
	}
	*/
	/*
	if( m_pPOITypeSet )
	{
		POSITION pos = m_pPOITypeSet->GetHeadPosition();
		ST_POITYPE_INFO *pPOIType = NULL;
		while( pos )
		{
			pPOIType = (ST_POITYPE_INFO*)m_pPOITypeSet->GetNext( pos );
			if( !pPOIType )
				continue;
			ST_SUBPOITYPE_INFO *pSubType = NULL;
			if( pPOIType->pSubList )
			{
				POSITION posSub = pPOIType->pSubList->GetHeadPosition();
				while( posSub )
				{
					pSubType = (ST_SUBPOITYPE_INFO*)pPOIType->pSubList->GetNext( posSub );
					if( pSubType )
					{
						delete pSubType;
						pSubType = NULL;
					}
				}
				delete pPOIType->pSubList;
				pPOIType->pSubList = NULL;
			}
			delete pPOIType;
			pPOIType =NULL;

		}
		delete m_pPOITypeSet;
		m_pPOITypeSet = NULL;
	}*/
/*
	if( g_pQueryMemEvent )
	{
		delete g_pQueryMemEvent;
		g_pQueryMemEvent = NULL;
	}*/
	/*
	if( g_pSpaceMemEvent )
	{
		delete g_pSpaceMemEvent;
		g_pSpaceMemEvent = NULL;
	}*/
}

IGIS_Map* CGIS_MapDataManager::GetMapByName(swString szMapName){
	std::vector<shared_ptr<IGIS_Map> >::iterator itr;
	IGIS_Map * map;
	for(itr = _mapset.begin();itr!=_mapset.end();itr++){
		if ( (*itr)->GetMapInfo()->GetMapName() ==  szMapName ){
			return (*itr).get();
		}
	}
	return NULL;
}
/*
IGIS_Map* CGIS_MapDataManager::GetFirstMap(){
	if( _mapset.size()){
		return _mapset[0].get();
	}
	return NULL;
}*/

IGIS_Map* CGIS_MapDataManager::GetMapByID(UINT m_mapID){
	std::vector<shared_ptr<IGIS_Map> >::iterator itr;
	for( itr = _mapset.begin();itr!=_mapset.end();itr++){
		if( (*itr)->GetMapInfo()->GetMapID() == m_mapID){
			return itr->get();
		}
	}
	return NULL;
	/*
	IGIS_Map* m_pTempMap;
	m_pTempMap = NULL;
	POSITION pos;
	if(m_MapSet != NULL)
	{
		pos = m_MapSet->GetHeadPosition();
		while (pos != NULL) 
		{
			m_pTempMap = (IGIS_Map*)(m_MapSet->GetNext(pos));
			if(m_pTempMap != NULL)
			{
				if( m_pTempMap->GetMapInfo()->GetMapID() == m_mapID )
				{
					return m_pTempMap;
				}
			}
		}
	}
	return NULL;*/
}

 	/*
IGIS_Map* CGIS_MapDataManager::GetLastMap(){
	 

	if(m_MapSet != NULL)
	{
		if(m_MapSet->GetCount() > 0)
		{
			return (IGIS_Map*)(m_MapSet->GetTail());
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}*/

//未曾使用
BOOL CGIS_MapDataManager::ClearMapData( ){
	/*
	int n;
//	IGIS_Map* map = NULL;
	std::vector<shared_ptr<IGIS_Map> > itrmap;
	IGIS_MapView* view = NULL;
	for(itrmap=_mapset.begin();itrmap!=_mapset.end();itrmap++){
		if( (*itrmap)->GetMapInfo()->GetMapID() != MAPID_UM_NAVI){
			CGIS_GeoMap *map = (CGIS_GeoMap*) (*itrmap).get();
			std::list<IGIS_MapView*>::iterator itr;
			for(itr = map->_mapViewSet.begin();itr!=map->_mapViewSet.end();itr++){
				map->Detache(*itr); //删除所有与地图关联的视图
			}
		//	_mapset[n]
		}

	}
	*/
		/*
		POSITION pss = NULL;
		POSITION pos = m_MapSet->GetHeadPosition();
		while( pos ) 
		{
			pss = pos;
			pTMap = (IGIS_Map*)m_MapSet->GetNext(pos);
			if( pTMap )
			{
				if( pTMap->GetMapInfo()->GetMapID() != MAPID_UM_NAVI )
				{
					CGIS_GeoMap *pMap = (CGIS_GeoMap*)pTMap;
					POSITION ps = pMap->m_pMapViewSet->GetHeadPosition();
					IGIS_MapView *pView = NULL;
					while( ps )
					{
						pView = (IGIS_MapView*)pMap->m_pMapViewSet->GetNext(ps);
						if( pView )
							pMap->Detache( pView );
					}
					m_MapSet->RemoveAt( pss );
					delete pTMap;
					pTMap = NULL;
				}
			}
		}
	}*/

	return TRUE;
}

//初始化地图信息
int CGIS_MapDataManager::OpenMap(swString szFilePath){
	FindMapFile( szFilePath );
	SetMainPath( szFilePath );
	InitSet(); //读取set.dat一些常数信息poi,country,city
	return 0;
	
}

//打开地图文件，创建地图对象，并加入数组内
/* 
int CGIS_MapDataManager::FindMapFile( swString bstrPath){
	HANDLE handle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FD;
	BOOL bFileExist=TRUE;
	swString strPath = bstrPath + L"\\*.*";
	handle = FindFirstFile(strPath,&FD);
	while (handle!=INVALID_HANDLE_VALUE && bFileExist)
	{
		swString strPathTem;
		if(FD.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
		{
#ifdef _WIN32_WCE
			strPathTem = bstrPath + L"\\" + FD.cFileName;
			FindMapFile( strPathTem );
#else
		//to be add	
#endif
		}
		else
		{
			strPathTem = FD.cFileName;
			strPathTem.MakeUpper();
			CGIS_GeoMap* pMap = NULL;
			if( strPathTem.Find(L".MP2") > 0 )
			{
				strPathTem = bstrPath + L"\\" + FD.cFileName;
				strPathTem.MakeUpper();
				//pMap = new CGIS_GeoMap;
				shared_ptr<IGIS_Map> pMap(new CGIS_GeoMap);
				if( pMap->Open(strPathTem ) )
				{
					//m_MapSet->AddTail( pMap );
					_mapset.push_back(pMap);
				}
			 
			}
			else if( strPathTem.Find(L".IN2") > 0 ) //此文件包含了导航引擎设计的所有地图、图层和图块信息的定义
			{
				strPathTem = bstrPath + L"\\" + FD.cFileName;
				strPathTem.MakeUpper();
				CGIS_MFBand fMBand( strPathTem );
				int nReturn = fMBand.LoadMaps( strPathTem, _mapset );
				if( nReturn != 0 )
					return nReturn;
				pMap = NULL;
			}
			else if( strPathTem.Find(L".MAP") > 0 )
			{
				strPathTem = bstrPath + L"\\" + FD.cFileName;
				strPathTem.MakeUpper();
				//CGIS_UserMap *pUMap = new CGIS_UserMap;
				shared_ptr<IGIS_Map> pUMap(new CGIS_UserMap);
				if( pUMap->OpenUserMap( strPathTem ) ){
					//m_MapSet->AddTail( pUMap );
					_mapset.push_back(pUMap);
				}
				else
				{
					// 操作有误，应该删除pUMap
					delete pUMap;
					pUMap = NULL;
				}
			}
		}
		bFileExist=FindNextFile(handle,&FD);
	}
	FindClose(handle);
	return 0;
}*/

int CGIS_MapDataManager::FindMapFile( swString bstrPath){
	wxArrayString files;
	wxDir::GetAllFiles(bstrPath,&files);
	int n;
	for(n=files.Count();n++){
		if( files[n].UpperCase().Right(4) == wxT(".MP2") ){
			shared_ptr<IGIS_Map> pMap(new CGIS_GeoMap);
			if( pMap->Open(bstrPath+wxT("/")+files[n]) ){
				_mapset.push_back(pMap);
			}
		}
		if( files[n].UpperCase().Right(4) == wxT(".IN2") ){
			shared_ptr<IGIS_Map> pMap(new CGIS_GeoMap);
			CGIS_MFBand fMBand( bstrPath+wxT("/")+files[n] );
			fMBand.LoadMaps( bstrPath+wxT("/")+files[n], _mapset );
		}
		if( files[n].UpperCase().Right(4) == wxT(".MAP") ){
			shared_ptr<IGIS_Map> pUMap(new CGIS_UserMap);
			if( pUMap->OpenUserMap( bstrPath+wxT("/")+files[n] ) ){
				_mapset.push_back(pUMap);
			}
		}
	}

	return 0;
}


#include "GIS_LayerManager.h"
#include "GIS_UserMap.h"
IGIS_Map* CGIS_MapDataManager::CreateMap( CGIS_MapInfo *pMapInfo ){
	CGIS_UserMap *pMap = new CGIS_UserMap;
	pMap->InitUserMap();
//	pMap->OpenUserMap( L"map\\navi.map" );
	pMap->SaveUserMap( L"map\\navi.map" );
	return pMap;
//	return pMap;
//	CGIS_GeoMap *pMap = new CGIS_GeoMap();
////	pMap->GetMapInfo() = pMapInfo;
//	CGIS_LayerManager *pLManager = new CGIS_LayerManager();
//	if( pLManager )
//		pMap->m_LayerManagerSet->AddTail( pLManager );
//	m_MapSet->AddTail( pMap );
//	return pMap;

//	CGIS_LayerManager *pLManager = new CGIS_LayerManager();
//	if( pLManager )
//	{
//		CGIS_LayerInfo *pInfo = new CGIS_LayerInfo;
//		if( pInfo )
//		{
//			pInfo->SetLayerType( 0 );
//			pInfo->m_bstrLName = BSTR("GeoUser");
//			ST_GEO_PEN pen;
//			ST_GEO_BRUSH brush;
//			pen.nColor		= 9692023;
//			brush.nForeColor	= 9692023;
//			pen.nWidth		= 1;
//			pen.nStyle		= 0;
//			brush.nStyle		= 0;
//			pInfo->SetPen( pen );
//			pInfo->SetBrush( brush );
//			pInfo->SetIconID( 64 );
//			pInfo->SetMinGRScale( 40000.0 );
//			pInfo->SetMaxGRScale( 2000000 );
//			pInfo->SetMinARScale( 80000.0 );
//			pInfo->SetMaxARScale( 4000000 );
//			ST_GEO_RECT rectLayer;
//			rectLayer.dLeft	= (float)121.327;
//			rectLayer.dTop	= (float)31.1279;
//			rectLayer.dRight	= (float)121.663;
//			rectLayer.dBottom = (float)31.4078;
//			pInfo->SetLayerRect( rectLayer );
//		}
//		CGIS_LayerAttInfo *pLAttInfo = new CGIS_LayerAttInfo();
//		CGIS_Field *pField = new CGIS_Field;
//		pField->SetFieldDefVal(L"chengming");
//		pField->SetFieldName( L"Name" );
//		pField->SetFieldSize(32);
//		pField->SetFieldType( 0 );
//		pLAttInfo->AddFieldInfo( pField );
//		pField = new CGIS_Field;
//		pField->SetFieldDefVal(L"255");
//		pField->SetFieldName( L"Color" );
//		pField->SetFieldSize(4);
//		pField->SetFieldType( 1 );
//		pLAttInfo->AddFieldInfo( pField );
//		pField = new CGIS_Field;
//		pField->SetFieldDefVal(L"1");
//		pField->SetFieldName( L"width" );
//		pField->SetFieldSize(4);
//		pField->SetFieldType( 1 );
//		pLAttInfo->AddFieldInfo( pField );
//		
//		pLManager->CreateUserLayer(pInfo, pLAttInfo);
//		pMap->m_MapInfo->m_nLayerNum +=1;
//	}
//	pMap->m_LayerManagerSet->AddTail( pLManager );
//	pMap->SaveToFile();
//	return pMap;
}

/*
IGIS_Map* CGIS_MapDataManager::GetNext(IGIS_Map* pMap)
{
	if(pMap != NULL)
	{
		POSITION pos = m_MapSet->GetHeadPosition();
		IGIS_Map* pTempMap;
		while(pos)
		{
			pTempMap = (IGIS_Map*)m_MapSet->GetNext(pos);
			if(pTempMap != NULL && pos)
			{
				if(pMap == pTempMap)
				{
					return (IGIS_Map*)m_MapSet->GetNext(pos);
				}
			}
			else
			{
				return NULL;
			}
		}
	}
	else
	{
		return NULL;
	}
	return NULL;
}

*/
CPtrList* CGIS_MapDataManager::GetProvinceList( unsigned short nCountryID )
{
	if( m_pCountrySet )
	{
		POSITION pos = m_pCountrySet->GetHeadPosition();
		ST_CCOUNTRY_INFO *pCountryInfo = NULL;
		while( pos )
		{
			pCountryInfo = (ST_CCOUNTRY_INFO*)m_pCountrySet->GetNext(pos);
			if( pCountryInfo && pCountryInfo->nCountryID == nCountryID )
				return pCountryInfo->PList;
		}
	}
	return NULL;
}
//此函数暂时没有使用
/*
CPtrList* CGIS_MapDataManager::GetCityList( unsigned short nCountryID,unsigned short nProID )
{
	if( m_pCountrySet )
	{
		POSITION pos = m_pCountrySet->GetHeadPosition();
		ST_CCOUNTRY_INFO *pCountryInfo = NULL;
		while( pos )
		{
			pCountryInfo = (ST_CCOUNTRY_INFO*)m_pCountrySet->GetNext(pos);
			if( pCountryInfo && pCountryInfo->nCountryID == nCountryID )
			{
				if( nProID == CITY_LIST_ALL )
					return pCountryInfo->CityList;
				if( nProID == CITY_LIST_ZHIXIA ) //判别是否为直辖市
				{
					CPtrList *pZhiSet = new CPtrList;
					POSITION ps = pCountryInfo->CityList->GetHeadPosition();
					ST_CCITY_INFO *pZhiCityInfo ;
					while( ps )
					{
						pZhiCityInfo = (ST_CCITY_INFO*)pCountryInfo->CityList->GetNext(ps);
						int nID = pZhiCityInfo->nCityID;
						if(  nID == 110000 || nID == 120000 || nID == 310000 || nID == 500000 )
							pZhiSet->AddTail( pZhiCityInfo );
					}
					return pZhiSet;
				}
				else
				{
					CPtrList *CitySet = new CPtrList;
					POSITION psCity = pCountryInfo->CityList->GetHeadPosition();
					ST_CCITY_INFO *pCityInfo ;
					while( psCity )
					{
						pCityInfo = ( ST_CCITY_INFO *)pCountryInfo->CityList->GetNext(psCity);
						int nID =( pCityInfo->nCityID ) / 10000;
						if( nID == nProID )
							CitySet->AddTail( pCityInfo );
					}
					return CitySet;
				}
			}
		}
	}
	return NULL;
}*/

//省市编号规则 , 省编号 = idx * 10000,0~10000是这个省下的市县
std::vector<ST_CCITY_INFO>& CGIS_MapDataManager::GetCityList( unsigned short nCountryID,unsigned short nProID ){
	int n,m;
	std::vector<ST_CCITY_INFO> citylist;
	for(n=0;n<_countrySet.size();n++){
		if(_countrySet[n].nCountryID == nCountryID){
			if( nProID == CITY_LIST_ALL ){
				return _countrySet[n].CityList; 
			}
			//判别是否为直辖市
			if( nProID == CITY_LIST_ZHIXIA ) {
				for(m=0;m<_countrySet[n].CityList.size();m++){
					int nID  = _countrySet[n].CityList[m].nCityID;
					if(  nID == 110000 || nID == 120000 || nID == 310000 || nID == 500000 ){
						citylist.push_back(_countrySet[n].CityList[m]);
					}
				}
			}else{ //非直辖市
				for(m=0;m<_countrySet[n].CityList.size();m++){
					int nID =_countrySet[n].CityList[m].nCityID / 10000;
					if( nID == nProID ){
						citylist.push_back(_countrySet[n].CityList[m]);
					}					
				}
			}
		}
	}
	return citylist;
}

ST_CCITY_INFO* CGIS_MapDataManager::GetCityByID( unsigned short nCountryID,int nCityID ){
	/*
	if( m_pCountrySet )
	{
		POSITION pos = m_pCountrySet->GetHeadPosition();
		ST_CCOUNTRY_INFO *pCountryInfo = NULL;
		while( pos )
		{
			pCountryInfo = (ST_CCOUNTRY_INFO*)m_pCountrySet->GetNext(pos);
			if( pCountryInfo && pCountryInfo->nCountryID == nCountryID )
			{
				POSITION psCity = pCountryInfo->CityList->GetHeadPosition();
				ST_CCITY_INFO *pCityInfo ;
				while( psCity )
				{
					pCityInfo = ( ST_CCITY_INFO *)pCountryInfo->CityList->GetNext(psCity);
					if( pCityInfo->nCityID == nCityID )
						return pCityInfo;
				}
			}
		}
	}
	return NULL;*/
	int n,m;
	for(n=0;n<_countrySet.size();n++){
		if( _countrySet[n].nCountryID == nCountryID){
			for(m=0;m<_countrySet[n].CityList.size();m++){
				if( _countrySet[n].CityList[m].nCityID ==nCityID ){
					return &_countrySet[n].CityList[m];
				}
			}
		}
	}
	return NULL;
}

std::vector<ST_POITYPE_INFO>& CGIS_MapDataManager::GetPOIClassList( ){
	return _poiTypeSet;
}
std::vector<ST_SUBPOITYPE_INFO>& CGIS_MapDataManager::GetPOISubClassList( unsigned short nSubID ) {
	for(int n=0;n<_poiTypeSet.size();n++){
		if( _poiTypeSet[n].nTypeID == nSubID){
			return _poiTypeSet[n].pSubList;
		}
	}
}

std::vector<ST_CCOUNTRY_INFO>& CGIS_MapDataManager::GetCountryList( ){
	return _countrySet;
}
//初始化操作，将一些常量信息读出来存储
BOOL CGIS_MapDataManager::InitSet(){
	swString strFName = m_strMainPath + wxT("/set.dat");
/*	HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;
*/
	swFile file;
	if( !file.Open(strFName)){
		return FALSE;
	}
	//DWORD dwSize = GetFileSize( hFile, NULL );
	DWORD dwSize = file.Length();
	DWORD dwCount = 0;
	std::vector<swbyte_t> stream;
	stream.resize(dwSize);
	char *pFBuf = (char*)&stream[0];
	dwCount = file.Read(pFBuf,dwSize)
	if( dwSize > dwCount ){		
		return FALSE;
	}
	DWORD dwStation = 0;
	int nCount = 0;
	//POI种类数量
	Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
	if( nCount == 0 ){
		return FALSE;
	}
	int i;
	for( i = 0 ; i < nCount ; i++ ){
		//ST_POITYPE_INFO *pPOIType = new ST_POITYPE_INFO;
		//pPOIType->pSubList = new CPtrList;
		ST_POITYPE_INFO pPOIType;
		//POI的名称	
		Fread( pFBuf, dwSize, dwStation, pPOIType.strPOIName, sizeof(char), 32 );
		//POI的ID
		Fread( pFBuf, dwSize, dwStation, &pPOIType.nTypeID , sizeof(BYTE), 1 );

		int nSubNum ;
		//POI的小类数量
		Fread( pFBuf, dwSize, dwStation, &nSubNum , sizeof(int), 1 );
		int j;
		for( j = 0 ; j < nSubNum ; j++ ){
			//ST_SUBPOITYPE_INFO *pSubPOI = new ST_SUBPOITYPE_INFO;
			ST_SUBPOITYPE_INFO pSubPOI ;
			Fread( pFBuf, dwSize, dwStation, &pSubPOI , sizeof(ST_SUBPOITYPE_INFO), 1 );
			//pPOIType->pSubList->AddTail( pSubPOI );
			pPOIType.pSubList.push_back(pSubPOI);
		}
		//if( m_pPOITypeSet )
		//	m_pPOITypeSet->AddTail( pPOIType );
		_poiTypeSet.push_back(pPOIType);
	}
	//国家数量
	Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
	if( nCount == 0 ){
		return FALSE;
	}
	for( i = 0 ; i < nCount ; i++){
		//ST_CCOUNTRY_INFO *pCountryInfo = new ST_CCOUNTRY_INFO;
		//pCountryInfo->CityList = new CPtrList;
		//pCountryInfo->PList = new CPtrList;
		ST_CCOUNTRY_INFO pCountryInfo;
		//国家ID
		Fread( pFBuf, dwSize, dwStation, &pCountryInfo.nCountryID, sizeof(unsigned short), 1 );
		//国家名称
		Fread( pFBuf, dwSize, dwStation, pCountryInfo.strCName, sizeof(char), 32 );
		int nPCount ;
		//省个数
		Fread( pFBuf, dwSize, dwStation, &nPCount, sizeof(int), 1 );
		for( int j = 0 ; j < nPCount ; j++ ){
			//ST_CPRO_INFO *pProInfo = new ST_CPRO_INFO;
			ST_CPRO_INFO pProInfo;
			Fread( pFBuf, dwSize, dwStation, &pProInfo, sizeof(ST_CPRO_INFO), 1 );
			//pCountryInfo->PList->AddTail( pProInfo );
			pCountryInfo.PList.push_back( pProInfo );
		}
		int nCCount;
		Fread( pFBuf, dwSize, dwStation, &nCCount, sizeof(int), 1 );//城市个数
		for( int k = 0 ; k < nCCount ; k++ ){
			//ST_CCITY_INFO *pCityInfo  = new ST_CCITY_INFO;
			ST_CCITY_INFO pCityInfo;
			Fread( pFBuf, dwSize, dwStation, &pCityInfo, sizeof(ST_CCITY_INFO), 1 );
			pCountryInfo.CityList.push_back( pCityInfo );
		}
		//if( m_pCountrySet )
		//	m_pCountrySet->AddTail( pCountryInfo );
		_countrySet.push_back(pCountryInfo);
	}
	return TRUE;
}
swString CGIS_MapDataManager::GetMainPath()
{
	return m_strMainPath;
}
void CGIS_MapDataManager::SetMainPath( swString szFilePath )
{
	m_strMainPath = szFilePath;
}
/*
void CGIS_MapDataManager::ClearSpaceMem( ){

	POSITION pos = m_MapSet->GetHeadPosition();
    IGIS_Map* pTempMap;
    while(pos)
    {
        pTempMap = (IGIS_Map*)m_MapSet->GetNext(pos);
        if( pTempMap && pTempMap->GetMapInfo()->GetMapID() != MAPID_UM_NAVI )
        {
            CGIS_GeoMap* pMap = (CGIS_GeoMap*)pTempMap;
			pMap->ResetMapState();
			CPtrList *pLayerMngSet = pMap->m_LayerManagerSet;
			ASSERT(pLayerMngSet != NULL);

			POSITION posLayer = pLayerMngSet->GetHeadPosition();
			CGIS_LayerManager* pLayerMng = NULL;
			while( posLayer )
			{
				pLayerMng = (CGIS_LayerManager*)(pLayerMngSet->GetNext(posLayer));
				if( pLayerMng )
				{
					CGIS_LayerMap *pLayerListMap = pLayerMng->GetLayerListMap();
					ASSERT(pLayerListMap);
					POSITION posLayerList = pLayerListMap->GetStartPosition();
					short    nKey;
					IGIS_Layer* pLayer = NULL;
					while( posLayerList  )
					{
						pLayerListMap->GetNextAssoc( posLayerList, nKey, pLayer );
						if( pLayer )
						{
							//清空记数器
							( (CGIS_MapLayer*)pLayer )->ClearLCount();
							//解除空间数据关系属性
							CListMap* pObjListMap = pLayer->GetObjectListMap();
							CPtrList *pObjList = NULL;
							POSITION posObjList = pObjListMap->GetStartPosition();
							
							// 为了避免混淆，变量尽量在重叠区域不要重名，更名变量
							// 原来：int nKey 改为：nInKey
							int nInKey;
							while( posObjList  )
							{
								pObjListMap->GetNextAssoc( posObjList, nInKey, pObjList );
								if( pObjList )
									pObjList->RemoveAll();
							}
						}
					}
				}
			}
        }
    }
   {
		// 避免变量重名混淆，更改变量名称
		// 原来：POSITION pos 改为：posView
        POSITION posView = m_ListOfView.GetHeadPosition();
        while(posView)
        {
            CGeoView* pView = (CGeoView*)m_ListOfView.GetNext(posView);
            ASSERT(pView);
            pView->ClearBufData( TRUE );
        }
    }
}*/


void CGIS_MapDataManager::ClearSpaceMem( ){
	int n;
	for(n=0;n<_mapset.size();n++){
		if( _mapset[n]->GetMapInfo()->GetMapID() != MAPID_UM_NAVI ){
			CGIS_GeoMap* pMap = (CGIS_GeoMap*)_mapset[n].get();
			pMap->ResetMapState();
			_layerMgnSet
			std::list<CGIS_LayerManager*>::iterator itrLayerMng;

			CPtrList *pLayerMngSet = pMap->m_LayerManagerSet;

			POSITION posLayer = pLayerMngSet->GetHeadPosition();
			CGIS_LayerManager* pLayerMng = NULL;
			while( posLayer )
			{
				pLayerMng = (CGIS_LayerManager*)(pLayerMngSet->GetNext(posLayer));
				if( pLayerMng )
				{
					CGIS_LayerMap *pLayerListMap = pLayerMng->GetLayerListMap();
					ASSERT(pLayerListMap);
					POSITION posLayerList = pLayerListMap->GetStartPosition();
					short    nKey;
					IGIS_Layer* pLayer = NULL;
					while( posLayerList  )
					{
						pLayerListMap->GetNextAssoc( posLayerList, nKey, pLayer );
						if( pLayer )
						{
							//清空记数器
							( (CGIS_MapLayer*)pLayer )->ClearLCount();
							//解除空间数据关系属性
							CListMap* pObjListMap = pLayer->GetObjectListMap();
							CPtrList *pObjList = NULL;
							POSITION posObjList = pObjListMap->GetStartPosition();

							// 为了避免混淆，变量尽量在重叠区域不要重名，更名变量
							// 原来：int nKey 改为：nInKey
							int nInKey;
							while( posObjList  )
							{
								pObjListMap->GetNextAssoc( posObjList, nInKey, pObjList );
								if( pObjList )
									pObjList->RemoveAll();
							}
						}
					}
				}
			}
		}
	}
	{
		// 避免变量重名混淆，更改变量名称
		// 原来：POSITION pos 改为：posView
		POSITION posView = m_ListOfView.GetHeadPosition();
		while(posView)
		{
			CGeoView* pView = (CGeoView*)m_ListOfView.GetNext(posView);
			ASSERT(pView);
			pView->ClearBufData( TRUE );
		}
	}
}


void CGIS_MapDataManager::ClearQueryMem( )
{

}
/*
CEvent* CGIS_MapDataManager::GetSpaceMemEvent()
{
	return g_pSpaceMemEvent;
}*/
/*
CEvent* CGIS_MapDataManager::GetQueryMemEvent()
{
	return g_pQueryMemEvent;
}*/

static UINT CALLBACK UpdateProc(CMemoryDispatchForSpaceDataV2::enUpdateAspect eAspect, DWORD dwPara)
{
    CSDEControler* pSDEControl = CSDEControler::GetSDEControl();
    ASSERT(pSDEControl);
    //////////////////////////////////////////////////////////////////////////
    switch(eAspect)
    {
    case CMemoryDispatchForSpaceDataV2::UPDATE_RELEASE:
        {
            CGIS_MapDataManager *pMapDataManager = (CGIS_MapDataManager*)pSDEControl->GetMapManager();
            ASSERT( pMapDataManager );
            pMapDataManager->ClearSpaceMem();
        }
        break;
    case CMemoryDispatchForSpaceDataV2::UPDATE_RESET:
        {
            CGIS_MapDataManager *pMapDataManager = (CGIS_MapDataManager*)pSDEControl->GetMapManager();
            ASSERT( pMapDataManager );
            pMapDataManager->ClearSpaceMem();
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }

    return 1;
}

