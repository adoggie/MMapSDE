#ifndef _SDE_CONTROLER_H
#define _SDE_CONTROLER_H

#include "swBase.h"

#include "GeoGlobe.h"
#include "Globe.h"
#include "IGIS_MapManager.h"
#include "IGIS_NetWork.h"

class IMemoryMngSet;
class CMemoryDispatchForQueryData;
class CMemoryDispatchForSpaceDataV2;
class CMemoryDispatchForNetworkV2;
    
//////////////////////////////////////////////////////////////////////////

class swEXPORT CSDEControler  
{

public:
	///版本信息结构
	typedef struct
	{
		DWORD dwVersionInfoSize;
			//The size, in bytes, of this data structure. Set this member to sizeof(VERSIONINFOA) before calling the GetVersionEx function. 
		DWORD dwMajorVersion;
			//This identifies the major version number. For example, for 2.10, the major version number is 2. 
		DWORD dwMinorVersion;
			//This identifies the minor version number. For example, for 2.10, the minor version number is 1. 
		DWORD dwBuildNumber;
			//This identifies the build number or is set to 0. 
		DWORD dwPlatformId;
			//not use.
		CHAR   szCSDVersion[ 128 ];  
			//not use
	} VERSIONINFO, *PVERSIONINFO, *LPVERSIONINFO;

public:
    //SDE错误码
    enum enErrorCode
    {
        ERROR_NONE,
        ERROR_UNDEFINED,
        ERROR_OUT_OF_MEMORY,
        ERROR_UNSUPPORTED_MAP_VERSION,
        ERROR_LOAD_MAP_FAILED,
        ERROR_LOAD_USER_MAP_FAILED,
        ERROR_ENCRYPT
    };

public:
	static CSDEControler* GetSDEControl();
	virtual ~CSDEControler();
	BOOL InitSDE(swString& strMapPath, swString& strUserMapPath ,enErrorCode* pErrorCode);
	BOOL GetVersionEx( LPVERSIONINFO lpVersionInformation );
	BOOL GetMapVersionEx( LPVERSIONINFO lpVersionInformation );

	IGIS_MapManager* GetMapManager();
	IMemoryMngSet* GetMemoryMngSet();
	IGIS_SDETool* GetSDETool( EnSDEToolType enSDEToolType );

	CMemoryDispatchForQueryData*	GetMemoryDispatchForQuery(){return m_pMemoryDispatchForQuery;};
	CMemoryDispatchForSpaceDataV2*	GetMemoryDispatchForSpace(){return m_pMemoryDispatchForSpace;};
	CMemoryDispatchForNetworkV2*	GetMemoryDispatchForNetwork(){return m_pMemoryDispatchForNetwork;};

	BOOL IsMemOver();
protected:
	BOOL UninitSDE();
protected:
	shared_ptr<IGIS_MapManager>  m_MapManager;
	//IGIS_MapManager* m_MapManager;
  IMemoryMngSet* m_pMemoryMngSet;

	CMemoryDispatchForQueryData*	m_pMemoryDispatchForQuery;
	CMemoryDispatchForSpaceDataV2*	m_pMemoryDispatchForSpace;
	CMemoryDispatchForNetworkV2*	m_pMemoryDispatchForNetwork;
	//CPtrList *m_pSDEToolList;
	std::list< shared_ptr<IGIS_SDETool> > m_pSDEToolList;
	CSDEControler();
	//static CSDEControler *m_pControl;
	static shared_ptr<CSDEControler> m_pControl; 
#ifdef _DEBUG
public:
    BOOL ResetMemoryForSpace();
#endif
    VERSIONINFO m_pMapVersionInfo;
};

#endif
