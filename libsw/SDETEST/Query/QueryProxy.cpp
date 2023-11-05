// QueryProxy.cpp: implementation of the CQueryProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "QueryProxy.h"
#include "../SDE/GIS_Address.h"
#include "../SDE/GIS_QueryByAtt.h"
#include "../SDE/GIS_QueryByShape.h"
#include "../SDE/SDEControler.h"
#include "../SDE/IGIS_Map.h"
#include "IGIS_Query2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//POI��ѯ�߳�
void	POIQueryThread(LPVOID lParam)
{
//	if( lParam == NULL )
//		return;
//
//	CQueryProxy * pQuery = (CQueryProxy*)lParam;
//	if( pQuery->m_nRange == 1 )//��ѯ����POI
//	{
//		pQuery->QuerySubPOIByType();
//	}
//	else if( pQuery->m_nRange == 2 )
//	{
//		//����POI
//		if( pQuery->m_nType == 1 )
//		{
//			if( pQuery->m_strName.IsEmpty() )
//				pQuery->QueryAllPOI();
//			else
//				pQuery->QueryPOIByName(pQuery->m_strName);
//		}
//		else//ָ������
//		{
//			pQuery->QuerySubPOIByType();
//		}
//	}

//	pQuery->m_strName.Empty();
//	::PostMessage(HWND_BROADCAST,WM_QUERYOVER,NULL,NULL);
//	TRACE0("��ѯ����\n\n");
}

//��ַ(��������·��)��ѯ�߳�
void AddrQueryThread(LPVOID lParam)
{
	if( lParam == NULL )
		return;
	
	CQueryProxy * pQuery = (CQueryProxy*)lParam;
	if( pQuery->m_nType == 1 )
		pQuery->QueryRoadSection();
	else
		pQuery->QueryRoadCross(pQuery->m_strCross);

	::PostMessage(HWND_BROADCAST,WM_QUERYOVER,NULL,NULL);
}

//��·��ѯ�߳�
void RoadQueryThread(LPVOID lParam)
{
	if( lParam == NULL )
		return;
	
	CQueryProxy * pQuery = (CQueryProxy*)lParam;
	
	pQuery->QueryRoad(pQuery->m_strInputRoad);
	
	::PostMessage(HWND_BROADCAST,WM_QUERYOVER,NULL,NULL);
}

CQueryProxy::CQueryProxy()
{
	m_pSearchData	=	NULL;
	m_pCountryList	=	NULL;
	m_pRoadList		=	new CPtrList;
	m_pAllPOIList	=	new CPtrList;
	m_pSubPOIList	=	new CPtrList;
	m_pRSecList		=	new CPtrList;
	m_pCrossRoadList =  new CPtrList;
	m_pMapList	=		new CPtrList;
	m_pPOIList	=		new CPtrList;

	m_pMap = NULL;

	m_posRoad	=	NULL;
	m_pMapMng	=	NULL;

	m_strRoad		=	"";
	m_strPOIName	=	_T("");

	m_nCityID = -1;
	m_nRange  = 0;
	m_nQueryStyle = -1;
	m_nCountryID = 86;
	m_nProvinceID = -1;
	m_nPOIType = 0;

	m_dwMainThreadID = GetCurrentThreadId();

//	InitMapList();

	m_strNearSelPath = _T("");
	m_strHistoryFile = "\\history.dat";

	m_nQueryBaseOnType = 0;
	m_ptBaseOn.dx = 0.0f;
	m_ptBaseOn.dy = 0.0f;
}

CQueryProxy::~CQueryProxy()
{
	if ( m_pSearchData != NULL )
	{
		delete m_pSearchData;
		m_pSearchData = NULL;
	}
	
	POSITION pos = m_pIndexList.GetHeadPosition();
	while (pos)
	{
		ST_INDEX_NAME * pIn = (ST_INDEX_NAME*)m_pIndexList.GetNext(pos);
		if (pIn)
		{
			if (pIn->Name)
			{
				delete pIn->Name;
				pIn->Name = NULL;
			}
			
			delete pIn;
			pIn = NULL;
		}
	}
	
	m_pIndexList.RemoveAll();

	RemoveAllPOI();
	RemoveSubPOI();
	RemoveAllRoad();
	RemoveRSecList();
}

//////////////////////////////////////////////////////
//Purpose:	ȡ�ò�ѯ���
//Return:	��ѯ���
//
SEARCHDATA* CQueryProxy::GetSearchData()
{
	return m_pSearchData;
}

/////////////////////////////////////////////////////
//Purpose:	���ò�ѯ���
//Param:	pData-��ѯ���(����pData�Ŀ���)
//Return:	void
//
void CQueryProxy::SetSearchData(SEARCHDATA *pData)
{
	//����Ѿ������ѯ����������µĽ��ǰ��ɾ��ԭ���Ľ��
	if ( NULL != m_pSearchData )
	{
		delete m_pSearchData;
		m_pSearchData = NULL;
	}

	//�����µĲ�ѯ���
	m_pSearchData = pData;
}

//////////////////////////////////////////////////////
//Purpose:	��������
//Para:		searchData-Ҫ����������
//
void ST_SEARCH_DATA::CopyData(const ST_SEARCH_DATA&searchData)
{
	enType = searchData.enType;
	
	if (enType==EN_SEARCHTYPE_FEATURE)
	{
		CGIS_FeaturePoint * pPoint = (CGIS_FeaturePoint*)searchData.pData;
		pData = pPoint->Clone();
	}
	else if (enType==EN_SEARCHTYPE_ADDRESS)
	{
		CGIS_FeatureAddress * pAddr = (CGIS_FeatureAddress*)searchData.pData;
		pData = pAddr->Clone();
	}
	else //if(enType==EN_SEARCHTYPE_PTXY)
	{
		pData = new ST_CONSID_DATA;
		memcpy( pData, searchData.pData,sizeof(ST_CONSID_DATA) );
	}
}

//////////////////////////////////////////////////////
//Purpose:	����=������
//
ST_SEARCH_DATA & ST_SEARCH_DATA::operator =(const ST_SEARCH_DATA&searchData)
{
	CopyData(searchData);
	return *this;
}

//////////////////////////////////////////////////////
//Purpose:	��������
//
ST_SEARCH_DATA::ST_SEARCH_DATA( const ST_SEARCH_DATA&searchData)
{
	CopyData(searchData);
}

//////////////////////////////////////////////////////
//Purpose:	���ù���ID
//Para:		����ID
//
void CQueryProxy::SetCountryID(int nCountryID)
{
	m_nCountryID = nCountryID;
}


//////////////////////////////////////////////////////
//PurPose:	����ѡ���ʡ(ֱϽ��)����
//Para:		ʡ(ֱϽ��)����ID
//
void CQueryProxy::SetProvinceID(int nProID)
{
	m_nProvinceID = nProID;
}

//////////////////////////////////////////////////////
//Purpose:	���ó���ID
//Para:		����ID
//
void CQueryProxy::SetCityID(int nCityID)
{
	m_nCityID = nCityID;
}

//////////////////////////////////////////////////////
//Purpose:	���õ�·
//Para:		��·����
//
void CQueryProxy::SetRoad(CString strRoad)
{
	m_strRoad = strRoad;
}

///////////////////////////////////////////////////////
// Purpose:	��ѯ��·
// Para:	strRoad		- ��·����
//
void CQueryProxy::QueryRoad(CString strRoad)
{
	if ( m_pMap )
	{
		//ȡ�ò�ѯ��������(������)
		CGIS_QueryByAtt * pQuery = (CGIS_QueryByAtt*)m_pMap->GetTool(QUERY_BYATT);
		if (!pQuery)
		{
			return;
		}

		//��ѯ����
		CPtrList List;
		
		POSITION pos = m_pIndexList.GetHeadPosition();
		while (pos)
		{
			ST_INDEX_NAME * pIn = (ST_INDEX_NAME*)m_pIndexList.GetNext(pos);
			if (pIn)
			{
				if (pIn->Name)
				{
					delete pIn->Name;
					pIn->Name = NULL;
				}
				
				delete pIn;
				pIn = NULL;
			}
		}

		m_pIndexList.RemoveAll();
		m_arRoadName.RemoveAll();
		pQuery->GetNameNIN(m_pIndexList,m_pMap->GetMapInfo()->GetMapFilePath(),m_nCityID,strRoad,2);

		ST_INDEX_NAME	* pIndex = NULL;
		CString			strRoadName;	//��char*���͵ĵ�·��ת��ΪCString���͵ĵ�·��
		BOOL			bIndentical = TRUE;		//����ͬ���ĵ�·��

		//���˲�ѯ���ظ��ĵ�·��
		POSITION p = m_pIndexList.GetHeadPosition();
		while ( p )
		{
			bIndentical = FALSE;
			pIndex = (ST_INDEX_NAME*)m_pIndexList.GetNext(p);
			CharToUnicode( pIndex->Name,&strRoadName);
			for( int i = 0; i < m_arRoadName.GetSize(); i++ )
			{
				CString strTmp = m_arRoadName.GetAt(i);
				if( strRoadName == strTmp )
				{
					bIndentical = TRUE;
					break;
				}
			}

			//���治ͬ�ĵ�·��
			if ( !bIndentical )
				m_arRoadName.Add( strRoadName );
		}
	}
}

//////////////////////////////////////////////////////
// Purpose:	ɾ����·�б������еĵ�·
//
void CQueryProxy::RemoveAllRoad()
{
	POSITION pos = m_pRoadList->GetHeadPosition();
	while ( pos )
	{
		CGIS_Feature* pFeature = (CGIS_Feature*)m_pRoadList->GetNext(pos);
		if ( NULL != pFeature )
		{
			delete pFeature;
			pFeature = NULL;
		}
	}

	m_pRoadList->RemoveAll();
}

////////////////////////////////////////////////////
// Purpose:	ȡ�õ�·�б��е�һ����·
// Return:	ȡ�õĵ�·
//
CGIS_Feature* CQueryProxy::GetFirstRoad()
{
	ASSERT( NULL != m_pRoadList );
	
	if ( m_pRoadList->GetCount() <= 0 )
	{
		return NULL;
	}

	m_posRoad = m_pRoadList->GetHeadPosition();
	if (NULL==m_posRoad)
	{
		return NULL;
	}
	else
		return (CGIS_Feature*)m_pRoadList->GetNext(m_posRoad);
}

///////////////////////////////////////////////////
// Purpose:	ȡ�õ�·�б�����һ����·
//
CGIS_Feature* CQueryProxy::GetNextRoad()
{
	ASSERT(NULL!=m_posRoad);

	return (CGIS_Feature*)m_pRoadList->GetNext(m_posRoad);
}

//////////////////////////////////////////////////
// Purpose:	ȡ�õ�·�б������һ����·
// Para:	ȡ�õĵ�·
//
CGIS_Feature* CQueryProxy::GetLastRoad()
{
	ASSERT( NULL != m_pRoadList );
	
	if ( m_pRoadList->GetCount() <= 0 )
	{
		return NULL;
	}
	
	return (CGIS_Feature*)m_pRoadList->GetTail();
}

///////////////////////////////////////////////////
// Purpose:	��ѯ����·��
// Para:	�����·����
// Remark:	���strCrossΪ�գ��������к�ѡ���·�����·��
//
void CQueryProxy::QueryRoadCross(CString strCross)
{
	if ( m_strRoad.IsEmpty() )
	{
		return;
	}

	if( !m_pCrossRoadList )
		return;
	if( m_pCrossRoadList->GetCount() > 0 )
		m_pCrossRoadList->RemoveAll();

	//��ѯ����·��
	if(m_pMap)
	{
		CGIS_QueryByShape *pQuerySh = (CGIS_QueryByShape*)m_pMap->GetTool(QUERY_BYSHARP);
		if ( !pQuerySh )
		{
			return;
		}
		
		CPtrList List;
		pQuerySh->QueryByIntersect( List, m_nCityID, m_strRoad, strCross );
		m_pCrossRoadList->AddTail(&List);
	}
}

///////////////////////////////////////////////////
// Purpose:	��ѯ���ƺ���
// Para:	���ƺ���
// Remark:	���strDoorplateΪ�գ�����ѡ���·���е����ƺ�
//
void CQueryProxy::QueryRoadDoorplate(CString strDoorplate)
{
	if ( m_strRoad.IsEmpty() )
	{
		return;
	}

	//��ѯ���ƺ�
}

////////////////////////////////////////////////////
// Purpose: ȡ�õ�·�����б�
//
CStringArray& CQueryProxy::GetRoadArray()
{
	return m_arRoadName;
}

///////////////////////////////////////////////////
// Purpose:	��ѯ��··��
//
void CQueryProxy::QueryRoadSection()
{
	ASSERT(NULL!=m_pRSecList);
	//��ѯǰ��ɾ��ԭ��������
	RemoveAllRoad();
	RemoveRSecList();
	
	//��ѯ����·��
	if(m_pMap)
	{
		CGIS_QueryByShape *pQuerySh = (CGIS_QueryByShape*)m_pMap->GetTool(QUERY_BYSHARP);
		if ( !pQuerySh )
		{
			return;
		}

		//ɸѡ����������Index
		CPtrList ListIndex;
		for( int i = 0; i < m_pIndexList.GetCount(); i++ )
		{
			ST_INDEX_NAME * pIndex = (ST_INDEX_NAME*)m_pIndexList.GetAt(m_pIndexList.FindIndex(i));
			if( pIndex )
			{
				CString strName = _T("");
				CharToUnicode(pIndex->Name,&strName);
				if( strName == m_strRoad )
				{
					ListIndex.AddTail(pIndex);
				}
			}
		}

		pQuerySh->QueryByIntersect(*m_pRSecList,&ListIndex, EN_ROUTE);
/*		
		//Step 1:����������ѯ���е�·��(CGIS_Feature)
		CGIS_QueryByAtt * pQueryAtt = (CGIS_QueryByAtt*)m_pMap->GetTool(QUERY_BYATT);
		if ( !pQueryAtt )
		{
			return;
		}
		
		ST_INDEX_NAME * pIndex = NULL;
		CPtrList FeatureList;
		
		POSITION pos = m_pIndexList.GetHeadPosition();
		while(pos)
		{
			pIndex = (ST_INDEX_NAME*)m_pIndexList.GetNext(pos);
			if( pIndex )
			{
				CString strTmp;
				CharToUnicode(pIndex->Name,&strTmp);
				if( strTmp == m_strRoad )
				{
					pQueryAtt->QueryByAnnotation( FeatureList,
												pIndex->BandID,
												pIndex->LayerID,
												pIndex->ClassID,
												pIndex->Name);

					m_pRoadList->AddTail(&FeatureList);
				}
				if ( pIndex->Name != NULL )
				{
					delete pIndex->Name;
					pIndex->Name = NULL;
				}
				delete pIndex;
				pIndex = NULL;
			}
		}
		
		m_pIndexList.RemoveAll();
//		m_pRoadList->AddTail(&FeatureList);
		
		//Step 2:����·�β�ѯ�������·���ཻ��·��
		CGIS_QueryByShape *pQuerySh = (CGIS_QueryByShape*)m_pMap->GetTool(QUERY_BYSHARP);
		if ( !pQuerySh )
		{
			return;
		}
		
		CPtrList List;
		pos = m_pRoadList->GetHeadPosition();
		while ( pos )
		{
			CGIS_Feature * pFeature = (CGIS_Feature*)m_pRoadList->GetNext( pos );
			DWORD dw = GetTickCount();
			pQuerySh->QueryByIntersect( List, pFeature, EN_OBJ_RUT );
			TRACE1("==============%d================\n\n",GetTickCount()-dw);
			CString strRoadSec = (CString)pFeature->GetAnnotation();
			POSITION p = List.GetHeadPosition();
			while (p)
			{
				CGIS_Feature * pFea = (CGIS_Feature*)List.GetNext(p);
				if (!pFea || (CString)pFea->GetAnnotation() == m_strRoad)
				{
					continue;
				}
				//����·�νṹ
				CString strCross = (CString)pFea->GetAnnotation();
				ROADSECTION * Sec = new ROADSECTION;
				Sec->strName = strRoadSec + "��" + strCross + "��";
				Sec->pFea = pFeature;

				//����б�Ϊ�գ������
				if (m_pRSecList->GetCount()<=0)
				{
					m_pRSecList->AddTail(Sec);
					continue;
				}

				//�����Ѿ����ڵ�·��
				BOOL	bFlag = FALSE;	//��ʶSec.strName�Ѿ�������·���б�,FALSE:������
				POSITION pp = m_pRSecList->GetHeadPosition();
				while (pp)
				{
					ROADSECTION * pSec = (ROADSECTION*)m_pRSecList->GetNext(pp);
					if (!pSec)
					{
						continue;
					}
					//�Ѿ�����
					if (pSec->strName==Sec->strName)
					{
						bFlag = TRUE;
					}
				}

				//�����ڣ������
				if (!bFlag)
				{
					m_pRSecList->AddTail(Sec);
				}
			}
		}
//*/
	}
}

///////////////////////////////////////////////////////
// Purpose:	��ѯ����POI
//
void CQueryProxy::QueryAllPOI( CString strPOIName )
{
	CPtrList List;
	CGIS_QueryByShape * pQuerySh = (CGIS_QueryByShape*)m_pMap->GetTool(QUERY_BYSHARP);
	DWORD dwTime = GetTickCount();
	pQuerySh->QueryBufferByName(List,&m_ptCurPos,NUM_RECORD_COUNT,strPOIName);
	TRACE( L"QUERYTIME: %d \n" , GetTickCount() - dwTime );
	
	ST_FEATURE_POINT *	pPoint = NULL;
	POSITION p = List.GetHeadPosition();
	while(p)
	{
		pPoint = (ST_FEATURE_POINT*)List.GetNext(p);
		m_pAllPOIList->AddTail(pPoint->pFeature);

		delete pPoint;
		pPoint = NULL;
	}
	
	List.RemoveAll();
	m_pPOIList->RemoveAll();

	m_pPOIList->AddTail(m_pAllPOIList);
}

///////////////////////////////////////////////////////
// Purpose:	ɾ����Ȥ���б������е���Ȥ��
//
void CQueryProxy::RemoveAllPOI()
{
	if (!m_pAllPOIList)
	{
		return;
	}

	POSITION pos = m_pAllPOIList->GetHeadPosition();
	if ( 1 == m_nRange )
	{
//		ASSERT( FALSE );
		while ( pos )
		{
			ST_INDEX_NAME * pIndex = (ST_INDEX_NAME*)m_pAllPOIList->GetNext(pos);
			if (!pIndex)
			{
				if ( !pIndex->Name )
				{
					delete pIndex->Name;
					pIndex->Name = NULL;
				}

				delete pIndex;
				pIndex = NULL;
			}
		} 
	}
	else if ( 2 == m_nRange )
	{
		while ( pos )
		{
			CGIS_Feature * pFeature = (CGIS_Feature*)m_pAllPOIList->GetNext(pos);
			if (pFeature) 
			{
				delete pFeature;
				pFeature = NULL;
			}
		}
	}

	m_pAllPOIList->RemoveAll();
}

////////////////////////////////////////////////////////
// Purpose:	��ѯ����POI
// Para:	


///////////////////////////////////////////////////////
// Purpose:	ɾ��������Ȥ���б������е���Ȥ��
//
void CQueryProxy::RemoveSubPOI()
{
	if ( NULL == m_pSubPOIList )
	{
		return;
	}

	POSITION	pos = m_pSubPOIList->GetHeadPosition();
	if( 1 == m_nRange )
	{
		while(pos)
		{
			ST_INDEX_NAME * pIndex = (ST_INDEX_NAME*)m_pSubPOIList->GetNext(pos);
			if ( pIndex != NULL )
			{
				delete pIndex;
				pIndex = NULL;
			}
		}
	}
	else if ( 2 == m_nRange )
	{
		while(pos)
		{
			CGIS_Feature * pFeature = (CGIS_Feature*)m_pSubPOIList->GetNext(pos);
			if ( NULL != pFeature )
			{
				delete pFeature;
				pFeature = NULL;
			}
		}
	}
	
	m_pSubPOIList->RemoveAll();
}

///////////////////////////////////////////////////////////
// Purpose: ���õ�ǰ�Ĳ�ѯ��ʽ
// Para:	��ѯ��ʽ
//				EN_QUERYADDR = 0,	//��ѯ��ַ
//				EN_QUERYPOI,	//��ѯPOI
//				EN_QUERYNPOI,	//��ѯ����POI
//				EN_QUERYPOSTCODE	//��ѯ��������
//
void CQueryProxy::SetQueryStyle(int nStyle)
{
	m_nQueryStyle = nStyle;
}

///////////////////////////////////////////////////////////
// Purpose:	ȡ�ò�ѯ��ʽ
//
int CQueryProxy::GetQueryStyle()
{
	return m_nQueryStyle;
}

///////////////////////////////////////////////////////////
// Purpose:	��ʼ����ͼ�б�
// 
void CQueryProxy::InitMapList()
{
	CSDEControler	*	pControl	= CSDEControler::GetSDEControl();
	ASSERT(pControl);
	VERIFY(m_pMapMng = pControl->GetMapManager());

	m_pMapList->RemoveAll();

	IGIS_Map *pTempMap = (IGIS_Map*)m_pMapMng->GetMapByID( 2 );
	m_pMapList->AddTail( pTempMap );
	m_pMap = (IGIS_Map*)m_pMapList->GetHead( );
}

//////////////////////////////////////////////////////
// Purpose:	���·���б�
void CQueryProxy::RemoveRSecList()
{
	if( NULL==m_pRSecList )
		return;

	POSITION pos = m_pRSecList->GetHeadPosition();
	while (pos)
	{
		ROADSECTION * pSec = (ROADSECTION*)m_pRSecList->GetNext(pos);
		if (pSec)
		{
			delete pSec;
			pSec = NULL;
		}
	}

	m_pRSecList->RemoveAll();
}

///////////////////////////////////////////////////////////
// Purpose:	����POI����
void CQueryProxy::SetPOIType(int nType)
{
	m_nPOIType = nType;
}

///////////////////////////////////////////////////////////
// Purpose:	����POI����
void CQueryProxy::SetSubPOIType(int nType)
{
	m_nSubPOIType = nType;
}

ST_SEARCH_DATA * ST_SEARCH_DATA::Clone()
{
	ST_SEARCH_DATA * pData = new ST_SEARCH_DATA;
	ZeroMemory(pData,sizeof(ST_SEARCH_DATA));

	pData->enType = enType;
	
	if (pData->enType==EN_SEARCHTYPE_FEATURE)
	{
		CGIS_FeaturePoint * pPoint = (CGIS_FeaturePoint*)this->pData;
		pData->pData = pPoint->Clone();
	}
	else if (enType==EN_SEARCHTYPE_ADDRESS)
	{
		CGIS_FeatureAddress * pAddr = (CGIS_FeatureAddress*)this->pData;
		pData->pData = pAddr->Clone();
	}
	else //if(enType==EN_SEARCHTYPE_PTXY)
	{
		pData->pData = new ST_CONSID_DATA;
		memcpy( pData->pData, this->pData,sizeof(ST_CONSID_DATA) );
	}

	return pData;
}

////////////////////////////////////////////////////////
// Purpose:	ȡ�ù����б��е�һ������
// Return:	���ҽṹ
//
ST_CCOUNTRY_INFO* CQueryProxy::GetFirstCountry()
{
	if ( m_pCountryList && m_pCountryList->GetCount() > 0 )
		m_pCountryList->RemoveAll();

	m_pCountryList = m_pMapMng->GetCountryList();
	if( !m_pCountryList || m_pCountryList->GetCount() <= 0 )
		return NULL;

	m_posCountry = m_pCountryList->GetHeadPosition();
	if( !m_posCountry )
		return NULL;
	else
		return (ST_CCOUNTRY_INFO*)m_pCountryList->GetNext(m_posCountry);
}

////////////////////////////////////////////////////////
// Purpose:	ȡ�ù����б�����һ������
// Return:	���ҽṹ
//
ST_CCOUNTRY_INFO* CQueryProxy::GetNextCountry()
{
	if( !m_pCountryList || m_pCountryList->GetCount() <= 0 || !m_posCountry )
		return NULL;

	return (ST_CCOUNTRY_INFO*)m_pCountryList->GetNext(m_posCountry);
}

////////////////////////////////////////////////////////
// Purpose:	ȡ�ù����б������һ������
// Return:	���ҽṹ
//
ST_CCOUNTRY_INFO* CQueryProxy::GetLastCountry()
{
	if ( !m_pCountryList )
		VERIFY( m_pCountryList=m_pMapMng->GetCountryList() );

	return (ST_CCOUNTRY_INFO*)m_pCountryList->GetTail();
}

////////////////////////////////////////////////////////
// Purpose:	ȡ��ʡ�б��е�һ������
// Return:	ʡ�ṹ
//
ST_CPRO_INFO* CQueryProxy::GetFirstProvince()
{
	//���²�ѯ
	m_pProvinceList = m_pMapMng->GetProvinceList(m_nCountryID);
	if( !m_pProvinceList || m_pProvinceList->GetCount() <= 0 )
		return NULL;
	
	m_posProvince = m_pProvinceList->GetHeadPosition();
	if( !m_posProvince )
		return NULL;
	else
		return (ST_CPRO_INFO*)m_pProvinceList->GetNext(m_posProvince);
}

////////////////////////////////////////////////////////
// Purpose:	ȡ��ʡ�б�����һ������
// Return:	ʡ�ṹ
//
ST_CPRO_INFO* CQueryProxy::GetNextProvince()
{
	if( !m_pProvinceList || m_pProvinceList->GetCount() <= 0 || !m_pProvinceList )
		return NULL;

	if( !m_posProvince )
		return NULL;
	
	return (ST_CPRO_INFO*)m_pProvinceList->GetNext(m_posProvince);
}

////////////////////////////////////////////////////////
// Purpose:	ȡ��ʡ�б������һ������
// Return:	ʡ�ṹ
//
ST_CPRO_INFO* CQueryProxy::GetLastProvince()
{
	if ( !m_pProvinceList )
		VERIFY(m_pProvinceList = m_pMapMng->GetProvinceList(m_nCountryID));
	
	return (ST_CPRO_INFO*)m_pProvinceList->GetTail();
}

//ȡ�ù����б�
CPtrList* CQueryProxy::GetCountryList()
{
	return m_pMapMng->GetCountryList();
}

//ȡ�ó����б�
CPtrList* CQueryProxy::GetCityList()
{
	if( m_nProvinceID < 0 || m_nCountryID <0 )
		return NULL;
	else
		return m_pMapMng->GetCityList(m_nCountryID,m_nProvinceID);
}

//ȡ�ó����б�
CPtrList* CQueryProxy::GetCityList( CString strCityName )
{
	if( m_nCountryID <0 )
		return NULL;
	
	CPtrList * pListCity = m_pMapMng->GetCityList(m_nCountryID);
	if( !pListCity )
		return NULL;

	CPtrList * pList = new CPtrList;
	ASSERT(pList);

	POSITION pos = pListCity->GetHeadPosition();
	while (pos)
	{
		ST_CCITY_INFO * pCity = (ST_CCITY_INFO*)pListCity->GetNext(pos);
		if( pCity )
		{
			CString strTmp;
			CharToUnicode(pCity->strCityName,&strTmp);
			if( strTmp.Find(strCityName,0) >= 0 )
			{
				pList->AddTail(pCity);
			}
		}
	}

	if( pList->GetCount() <= 0 )
	{
		delete pList;
		pList = NULL;
	}

	return pList;
}

//ȡ��·���б�
CPtrList* CQueryProxy::GetRoadSecList()
{
	return m_pRSecList;
}

//ȡ�ý���·���б�
CPtrList* CQueryProxy::GetCrossRoadList()
{
	return m_pCrossRoadList;
}

//ȡ��POI�����б�
CPtrList* CQueryProxy::GetPOITypeList()
{
	return m_pMapMng->GetPOIClassList();
}

//ȡ��POI�����б�
CPtrList* CQueryProxy::GetPOISubTypeList()
{
	return m_pMapMng->GetPOISubClassList(m_nPOIType);
}

CPtrList * CQueryProxy::GetPOIList()
{
	return m_pPOIList;
}

CGIS_Feature* CQueryProxy::GetFeatureByIndex(ST_INDEX_NAME *pIndex)
{
	if( !pIndex )
		return NULL;

	CGIS_QueryByAtt * pQueryAtt = (CGIS_QueryByAtt*)m_pMap->GetTool(QUERY_BYATT);
	if( !pQueryAtt )
		return NULL;

	CPtrList List;
	TRY
	{
		pQueryAtt->QueryByAnnotation(List,pIndex->BandID,pIndex->LayerID,pIndex->ClassID,pIndex->Name);
	}
	CATCH_ALL(e)
	{
		TRACE0("error\n\n");
		return NULL;
	}
	END_CATCH_ALL

	if( List.GetCount() <= 0 )
		return NULL;
	else
		return (CGIS_Feature*)List.GetHead();
}

void CQueryProxy::SetCurPos(ST_GEO_PTXY ptCurPos)
{
	m_ptCurPos = ptCurPos;
}

int CQueryProxy::GetPOIType()
{
	return m_nPOIType;
}

void CQueryProxy::QueryPOIByName(CString strName)
{
	CGIS_QueryByAtt * pQuery = (CGIS_QueryByAtt*)m_pMap->GetTool(QUERY_BYATT);
	if( !pQuery )
		return;

//	QueryAllPOI();

	m_pPOIList->RemoveAll();
	pQuery->QueryByAnnotation( *m_pPOIList, m_pAllPOIList, strName );
}

void CQueryProxy::POIQuery(int nRange, int nType, CString strName)
{
	ASSERT( nRange == 1 || nRange == 2 );
	ASSERT( nType == 1 || nType == 2 );

	if( nRange == 2 && nType == 1 )
		RemoveAllPOI();

	RemoveSubPOI();
	
	m_nRange	= nRange;
	m_nType		= nType;
	m_strName	= strName;

	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)POIQueryThread,this,0,0);
}

//ͨ�������ѯPOI
void CQueryProxy::QueryPOIByType( int nRange )
{
	if ( !m_pAllPOIList )
	{
		return;
	}

	RemoveAllPOI();

	RemovePOIList();
	
	//��ѯ���е���Ȥ��
	if ( 1 == nRange )
	{
		if (!m_pMap)
		{
			return;
		}
		
		m_pPOIList->RemoveAll();
		CGIS_QueryByAtt * pQueryAtt = (CGIS_QueryByAtt*)m_pMap->GetTool(QUERY_BYATT);
		CPtrList List;
		pQueryAtt->GetLayerNIN(List,m_pMap->GetMapInfo()->GetMapFilePath(),m_nCityID, m_nPOIType);
		m_pAllPOIList->AddTail(&List);
		m_pPOIList->AddTail( m_pAllPOIList );
	}
	//��ѯNEARRANGE(5km)��Χ�ڵ���Ȥ��
	else if( 2 == nRange )
	{
		CPtrList List;
		CGIS_QueryByShape * pQuerySh = (CGIS_QueryByShape*)m_pMap->GetTool(QUERY_BYSHARP);

//		pQuerySh->QueryByBuffer(List,&m_ptCurPos,NEARRANGE,m_nPOIType,0, EN_OBJ_PT);
		if( m_strPOIName.IsEmpty() )
		{
			pQuerySh->QueryByBuffer(List,&m_ptCurPos,NUM_RECORD_COUNT,m_nPOIType,m_nSubPOIType);
		}
		else
		{
			pQuerySh->QueryBufferByName(List,&m_ptCurPos,NUM_RECORD_COUNT,m_nPOIType,m_nSubPOIType,m_strPOIName );
			m_strPOIName.Empty();
		}

		


		ST_FEATURE_POINT *	pPoint = NULL;
		POSITION p = List.GetHeadPosition();
		while(p)
		{
			pPoint = (ST_FEATURE_POINT*)List.GetNext(p);
//			m_pAllPOIList->AddTail( pPoint->pFeature->Clone() );
			m_pPOIList->AddTail( pPoint->pFeature );

			delete pPoint;
			pPoint = NULL;
		}
		
		List.RemoveAll();
	}

	m_nRange = nRange;
}

void CQueryProxy::QuerySubPOIByType()
{
	CGIS_QueryByAtt * pQuery = (CGIS_QueryByAtt*)m_pMap->GetTool(QUERY_BYATT);
	if( !pQuery )
		return;
	
	CPtrList List,List1; 
	//��ѯ������Ȥ��
	if( 1 == m_nRange )
	{
		pQuery->GetClassNIN( List, m_pAllPOIList, m_pMap->GetMapInfo()->GetMapFilePath(), m_nSubPOIType);
	}
	//��ѯ��������Ȥ��
	else if ( 2 == m_nRange )
	{
		pQuery->QueryByType( List, m_pAllPOIList, m_nPOIType, m_nSubPOIType);
	}
	
	m_pSubPOIList->AddTail(&List);
	
	//m_pPOIList��m_pSubPOIList����m_pAllPOIList��һ���Ӽ�,��ѯǰ�Ѿ��������
	m_pPOIList->RemoveAll();
	
	if( m_strName.IsEmpty() )
		m_pPOIList->AddTail(m_pSubPOIList);
	else
	{
		if( 1 == m_nRange )
		{
			pQuery->GetNINByName( *m_pPOIList, m_pSubPOIList, m_strName );
		}
		if( 2 == m_nRange )
			pQuery->QueryByAnnotation( *m_pPOIList, m_pSubPOIList, m_strName );

		m_strName.Empty();
	}
}

void CQueryProxy::AddrQuery(int nType, CString strCross)
{
	ASSERT(nType==1||nType==2);

	m_nType = nType;
	m_strCross = strCross;

	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AddrQueryThread,this,0,0);
}

void CQueryProxy::RoadQuery(CString strInputRoad)
{
	ASSERT(!strInputRoad.IsEmpty());
	m_strInputRoad = strInputRoad;
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RoadQueryThread,this,0,0);
}

void CQueryProxy::SetStopSearch()
{
	CGIS_QueryByShape * pQuerySh = (CGIS_QueryByShape*)m_pMap->GetTool(QUERY_BYSHARP);
	pQuerySh->SetbStopSearch( TRUE );
}

void CQueryProxy::SetPOIName(CString strName)
{
	m_strPOIName = strName;
}

void CQueryProxy::SetNetWorkRoute(CGIS_NetworkRoute *pNetWork)
{
	ASSERT( pNetWork );

	CGIS_QueryByShape * pQuerySh = (CGIS_QueryByShape*)m_pMap->GetTool(QUERY_BYSHARP);
	ASSERT( pQuerySh );

	pQuerySh->SetNetWorkRoute( pNetWork );
}

void CQueryProxy::SetBNetwork(BOOL bNetwork)
{
	CGIS_QueryByShape * pQuerySh = (CGIS_QueryByShape*)m_pMap->GetTool(QUERY_BYSHARP);
	ASSERT( pQuerySh );

	pQuerySh->SetBNearNetWork( bNetwork );
}

//���ò�ѯ��׼���͡����ڵ�ǰ��0�����ڲ�ͬ�ĳ���1�����ڵ�ǰ����2������Ŀ���3
void CQueryProxy::SetQueryBaseOnType( int nType )
{
	m_nQueryBaseOnType = nType;
}

int CQueryProxy::GetQueryBaseOnType()
{
	return m_nQueryBaseOnType;
}

void CQueryProxy::SetQueryBaseOnPt(ST_GEO_PTXY ptBaseOn)
{
	m_ptBaseOn = ptBaseOn;
}

ST_GEO_PTXY CQueryProxy::GetQueryBaseOnPt()
{
	return m_ptBaseOn;
}

void CQueryProxy::RemovePOIList()
{
	if( m_nRange == 1 )
	{
		//���ԭ���Ľ��
		if( m_pPOIList )
		{
			POSITION pos = m_pPOIList->GetHeadPosition();
			while (pos)
			{
				ST_INDEX_NAME * pIndex = (ST_INDEX_NAME*)m_pPOIList->GetNext(pos);
				if( pIndex )
				{
					delete pIndex;
					pIndex = NULL;
				}
			}
		}
	}
	else if( m_nRange == 2 )
	{
		//���ԭ���Ľ��
		if( m_pPOIList )
		{
			POSITION pos = m_pPOIList->GetHeadPosition();
			while (pos)
			{
				CGIS_Feature * pFea = (CGIS_Feature*)m_pPOIList->GetNext(pos);
				if( pFea )
				{
					delete pFea;
					pFea = NULL;
				}
			}
		}
	}
	
	m_pPOIList->RemoveAll(); 
}
