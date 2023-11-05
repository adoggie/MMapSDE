
#include "stdafx.h"
#include "SDEControler.h"
#include "GIS_MapDataManager.h"
#include "GIS_Network.h"

#include "MemoryMngSetV2.h"
#include "MemoryDispatchForQueryData.h"
#include "MemoryDispatchForSpaceDataV2.h"
#include "MemoryDispatchForNetworkV2.h"
//////////////////////////////////////////////////////////////////////////
#include "DebugFunctionLib/DebugFun.h"

#ifdef _DEBUG

#define DBG2LOGMEMSTATUS(sz) {\
	MEMORYSTATUS mst;\
	::GlobalMemoryStatus( &mst );\
\
    DBG2LOG0("****************************************\n");\
	DBG2LOG0(sz);	\
	DBG2LOG0("\nGlobalMemoryStatus:\n");	\
	DBG2LOG1("\tCurrent Load      [%.2d%].\n",mst.dwMemoryLoad);\
	DBG2LOG1("\tTotal Physical    [%dm].\n",mst.dwTotalPhys>>20);\
	DBG2LOG1("\tAvailable	Physical[%dm].\n",mst.dwAvailPhys>>20);\
	DBG2LOG1("\tTotal Visual      [%dm].\n",mst.dwTotalVirtual>>20);\
	DBG2LOG1("\tAvailable Visual  [%dm].\n",mst.dwAvailVirtual>>20);\
    DBG2LOG0("****************************************\n");\
    }

#else //_DEBUG

#define DBG2LOGMEMSTATUS(sz)

#endif //_DEBUG


#define WRITE2LOGMEMSTATUS(sz) {\
	MEMORYSTATUS mst;\
	::GlobalMemoryStatus( &mst );\
\
    WRITE2LOG0("****************************************\n");\
	WRITE2LOG0(sz);	\
	WRITE2LOG0("\nGlobalMemoryStatus:\n");	\
	WRITE2LOG1("\tCurrent Load      [%.2d%].\n",mst.dwMemoryLoad);\
	WRITE2LOG1("\tTotal Physical    [%dm].\n",mst.dwTotalPhys>>20);\
	WRITE2LOG1("\tAvailable	Physical[%dm].\n",mst.dwAvailPhys>>20);\
	WRITE2LOG1("\tTotal Visual      [%dm].\n",mst.dwTotalVirtual>>20);\
	WRITE2LOG1("\tAvailable Visual  [%dm].\n",mst.dwAvailVirtual>>20);\
    WRITE2LOG0("****************************************\n");\
    }


shared_ptr<CSDEControler> CSDEControler::m_pControl;// = NULL;
CSDEControler::CSDEControler(){
	//m_MapManager = NULL;
	//m_pSDEToolList = new CPtrList;
    m_pMemoryMngSet = NULL;
    //m_pMapVersionInfo = NULL;
}

CSDEControler::~CSDEControler()
{
    VERIFY(UninitSDE());
}
CSDEControler* CSDEControler::GetSDEControl(){
	if( m_pControl.get() == NULL){
		m_pControl = shared_ptr<CSDEControler>(new CSDEControler);
	}
	return m_pControl;
}
//////////////////////////////////////////////////////////////////////////
//这个CRITICAL_SECTION用来同步解压缩动作.
extern "C"
{
    CRITICAL_SECTION g_stCriticalSection;
}
//////////////////////////////////////////////////////////////////////////
//strUmPath用户地图数据并未使用
BOOL CSDEControler::InitSDE(swString& strMPath, swString& strUMPath ,enErrorCode* pErrorCode){
    //ASSERT(pErrorCode);
    
    //InitializeCriticalSection(&g_stCriticalSection);
    //InitializeCriticalSection(&g_stCriticalSectionForGetSDETool);
    WRITE2LOGMEMSTATUS("enter InitSde");

    {
        if(m_pMemoryMngSet)
        {
            delete m_pMemoryMngSet;
            m_pMemoryMngSet = NULL;
        }
        
        m_pMemoryMngSet = new CMemoryMngSetV2;
        TRACE0("************Before InitSDE()****************************\n");
        MEMMNG_DUMPSTATE(m_pMemoryMngSet);
        TRACE0("****************************************\n");
        
        if(!m_pMemoryMngSet->InitialOperation())
        {
            TRACE0("内存管理器集合初始化失败!\n");
            *pErrorCode = ERROR_OUT_OF_MEMORY;

            WRITE2LOG1("InitSde Error: %d\n",*pErrorCode);
            return FALSE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DBG2LOGMEMSTATUS("after create CMemoryMngSetV2");
    //////////////////////////////////////////////////////////////////////////
    {
        m_pMemoryDispatchForQuery = new CMemoryDispatchForQueryData;
        ASSERT(m_pMemoryDispatchForQuery);
        if(!m_pMemoryDispatchForQuery->InitialOperation())
        {
            TRACE0("查询数据管理器初始化失败!\n");

            *pErrorCode = ERROR_OUT_OF_MEMORY;
            WRITE2LOG1("InitSde Error: %d\n",*pErrorCode);
            return FALSE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DBG2LOGMEMSTATUS("after create CMemoryDispatchForQueryData");
    //////////////////////////////////////////////////////////////////////////
    
    {
        m_pMemoryDispatchForSpace = new CMemoryDispatchForSpaceDataV2;
        ASSERT(m_pMemoryDispatchForSpace);
        if(!m_pMemoryDispatchForSpace->InitialOperation())
        {
            TRACE0("空间数据管理器初始化失败!\n");
            *pErrorCode = ERROR_OUT_OF_MEMORY;
            WRITE2LOG1("InitSde Error: %d\n",*pErrorCode);
            return FALSE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DBG2LOGMEMSTATUS("after create CMemoryDispatchForSpaceDataV2");
    //////////////////////////////////////////////////////////////////////////
    {
        m_pMemoryDispatchForNetwork = new CMemoryDispatchForNetworkV2;
        ASSERT(m_pMemoryDispatchForNetwork);
        if(!m_pMemoryDispatchForNetwork->InitialOperation())
        {
            TRACE0("路网数据管理器初始化失败!\n");
            *pErrorCode = ERROR_OUT_OF_MEMORY;
            WRITE2LOG1("InitSde Error: %d\n",*pErrorCode);
            return FALSE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DBG2LOGMEMSTATUS("after create CMemoryDispatchForNetworkV2");
    //////////////////////////////////////////////////////////////////////////
	//ASSERT(!m_MapManager);
	//m_MapManager = new CGIS_MapDataManager;
	if( !strMPath.IsEmpty() ) // 0 成功
	{
		int nOpenReturn = m_MapManager->OpenMap(strMPath);
		if( nOpenReturn == 2 )
        {
            *pErrorCode = ERROR_LOAD_MAP_FAILED ;            
            WRITE2LOG1("InitSde Error: %d\n",*pErrorCode);
            return FALSE;//初始化时就打开该路径下的所有地图
            
        }
		else if( nOpenReturn == 1 )
		{
			*pErrorCode = ERROR_ENCRYPT ;            
			return FALSE;//初始化时就打开该路径下的所有地图
		}
	}
	else
    {
        *pErrorCode = ERROR_LOAD_MAP_FAILED;            
        return FALSE;
    }

    //////////////////////////////////////////////////////////////////////////
    DBG2LOGMEMSTATUS("after OpenMap");
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    DBG2LOGMEMSTATUS("after OpenUserMap");
    //////////////////////////////////////////////////////////////////////////

   // ASSERT(!m_pMapVersionInfo);
    //m_pMapVersionInfo = new VERSIONINFO;
		/*
    swString strFilePath = strMPath + wxT("/AutoMap.ver");
    FILE* fp = _tfopen(strFilePath,wxT("rb"));
    if(!fp)
    {
        *pErrorCode = ERROR_UNSUPPORTED_MAP_VERSION;
        WRITE2LOG1("InitSde Error: %d\n",*pErrorCode);
        return FALSE;
    }
    int numwritten = fread( &m_pMapVersionInfo->dwMajorVersion, sizeof( DWORD ), 2, fp );
    if(numwritten != 2)
    {
        VERIFY(!fclose(fp));
        *pErrorCode = ERROR_UNSUPPORTED_MAP_VERSION;
        WRITE2LOG1("InitSde Error: %d\n",*pErrorCode);
        return FALSE;
    }

    VERIFY(!fclose(fp));
   */     
	TRACE0("************After InitSDE()****************************\n");
	MEMMNG_DUMPSTATE(m_pMemoryMngSet);
	TRACE0("****************************************\n");

    //////////////////////////////////////////////////////////////////////////
    WRITE2LOGMEMSTATUS("exit initsde");
    //////////////////////////////////////////////////////////////////////////
    *pErrorCode = ERROR_NONE;            
	return TRUE;
}

BOOL CSDEControler::UninitSDE(){
	/*
    if(m_pMapVersionInfo)
    {
        delete m_pMapVersionInfo;
        m_pMapVersionInfo = NULL;
    }*/
	/*
    if( m_MapManager )
    {
        delete m_MapManager;
        m_MapManager = NULL;
    }*/
    /*
    POSITION pos = m_pSDEToolList->GetHeadPosition();
    IGIS_SDETool *pSDETool = NULL;
    while( pos )
    {
        pSDETool = (IGIS_SDETool*)m_pSDEToolList->GetNext( pos );
        if( pSDETool )
            delete pSDETool;
    }
    m_pSDEToolList->RemoveAll();
    delete m_pSDEToolList;
*/
    {
        if(m_pMemoryDispatchForQuery)
        {
            delete m_pMemoryDispatchForQuery;
            m_pMemoryDispatchForQuery = NULL;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    {
        if(m_pMemoryDispatchForSpace)
        {
            delete m_pMemoryDispatchForSpace;
            m_pMemoryDispatchForSpace = NULL;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    {
        if(m_pMemoryDispatchForNetwork)
        {
            delete m_pMemoryDispatchForNetwork;
            m_pMemoryDispatchForNetwork = NULL;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //内存管理器要最后析构
    {
        if(m_pMemoryMngSet)
        {
            delete m_pMemoryMngSet;
            m_pMemoryMngSet = NULL;
        }        
    }
    //////////////////////////////////////////////////////////////////////////

    m_pControl = NULL;
    return TRUE;
}

IGIS_MapManager* CSDEControler::GetMapManager(){
	return m_MapManager.get();
}

IMemoryMngSet* CSDEControler::GetMemoryMngSet()
{
	return m_pMemoryMngSet;
}

IGIS_SDETool* CSDEControler::GetSDETool( EnSDEToolType enSDEToolType ){
  //CGetSDEToolLock aLock;
	std::list< shared_ptr<IGIS_SDETool> >::iterator itr;
	for(itr=m_pSDEToolList.begin();itr!=m_pSDEToolList.end();itr++){
		if(　(*itr)->GetSDEToolType( ) == enSDEToolType){
			return (*itr).get();
		}
	}
	IGIS_SDETool *pSDETool = NULL;
	/*
	POSITION pos = m_pSDEToolList->GetHeadPosition();
	IGIS_SDETool *pSDETool = NULL;
	while( pos )
	{
		pSDETool = (IGIS_SDETool*)m_pSDEToolList->GetNext( pos );
		if( pSDETool )
		{
			if( pSDETool->GetSDEToolType( ) == enSDEToolType )
				return pSDETool;
		}
	}*/
	switch( enSDEToolType )
	{
	case EN_SDETOOLTYPE_NETWORK:
		/*
			CGIS_NetWork *pNetwork = new CGIS_NetWork;
			pNetwork->SetMapManager( m_MapManager );
			pSDETool = pNetwork;
			*/
		shared_ptr<IGIS_SDETool> tool(new CGIS_NetWork);
		tool->SetMapManager( m_MapManager.get() );
		m_pSDEToolList.push_back(tool);
		pSDETool = tool.get();
		break;
	case EN_SDETOOLTYPE_SYMBOL:
		break;
	default:
		ASSERT( FALSE );
		break;
	}
	//m_pSDEToolList->AddTail( pSDETool );
	return pSDETool;
}
BOOL CSDEControler::IsMemOver(){
	//ASSERT(m_pMemoryMngSet);
	return m_pMemoryMngSet->IsMemoryOver();
}

BOOL CSDEControler::GetVersionEx ( LPVERSIONINFO lpVersionInformation )
{
    AFX_MODULE_STATE* pState = AfxGetStaticModuleState ();
    if(!pState)
        return FALSE;

	if(lpVersionInformation->dwVersionInfoSize < sizeof(VERSIONINFO))
	{
		TRACE0("VersionInformation buffer size is small.\n");
		return FALSE;
	}

	TCHAR pModuleFileName[MAX_PATH];
	int nLength = GetModuleFileName(pState->m_hCurrentInstanceHandle, pModuleFileName, MAX_PATH);
	
	if(nLength == 0)
	{
		TRACE0("GetModuleFileName Failed!\n");
		return FALSE;
	}
	
	DWORD dwHandle = 0;
	DWORD dwFileVersionInfoSize = GetFileVersionInfoSize(pModuleFileName,&dwHandle);
	if(dwFileVersionInfoSize == 0)
	{
		TRACE0("GetFileVersionInfoSize Failed!\n");
		return FALSE;
	}
	
	char* pVersionInfoData = new char[dwFileVersionInfoSize];
	if(!pVersionInfoData)
	{
		TRACE0("Can't alloc the mem of the version info.\n");
		return FALSE;
	}
	
	BOOL bSuccess = GetFileVersionInfo(pModuleFileName,0,dwFileVersionInfoSize,pVersionInfoData);
	if(!bSuccess)
	{
		TRACE0("GetFileVersionInfo Failed!\n");
		delete []pVersionInfoData;
		return FALSE;
	}
	
	UINT uiInfoSize = 0;
	VS_FIXEDFILEINFO* pFixedFileInfo = NULL;
	bSuccess = VerQueryValue(pVersionInfoData, _T("\\"),(void**)&pFixedFileInfo,&uiInfoSize);
	if(!bSuccess)
	{
		TRACE0("VerQueryValue Failed!\n");
		delete []pVersionInfoData;
		return FALSE;
	}
	
	lpVersionInformation->dwMajorVersion = pFixedFileInfo->dwFileVersionMS;
	lpVersionInformation->dwMinorVersion = pFixedFileInfo->dwFileVersionLS;
	lpVersionInformation->dwBuildNumber = 0;
	
	delete []pVersionInfoData;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
BOOL CSDEControler::ResetMemoryForSpace()
{
    ASSERT(m_pMemoryDispatchForSpace);
    m_pMemoryDispatchForSpace->Lock();
    TRACE0("CSDEControler::ResetMemoryForSpace()\n");
    m_pMemoryDispatchForSpace->RequireReset();
    m_pMemoryDispatchForSpace->Unlock();

    return TRUE;
}
#endif
//////////////////////////////////////////////////////////////////////////
BOOL CSDEControler::GetMapVersionEx( LPVERSIONINFO lpVersionInformation ){
	/*
    ASSERT(m_pMapVersionInfo);
    ASSERT(lpVersionInformation);
	if(lpVersionInformation->dwVersionInfoSize < sizeof(VERSIONINFO))
	{
		TRACE0("VersionInformation buffer size is small.\n");
		return FALSE;
	}

    lpVersionInformation->dwMajorVersion = m_pMapVersionInfo->dwMajorVersion;
    lpVersionInformation->dwMinorVersion = m_pMapVersionInfo->dwMinorVersion;
    */
	* lpVersionInformation = m_pMapVersionInfo;
		return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//对预编译宏的检查
#ifdef _ENCRYPT_CHECK_ENABLE
#pragma message( "本次编译使能了加密校验功能!" )
#else
//#pragma message( "本次编译未使能加密校验功能!" )
#endif
//////////////////////////////////////////////////////////////////////////