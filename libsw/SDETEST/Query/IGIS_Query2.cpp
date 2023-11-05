// IGIS_Query2.cpp: implementation of the IGIS_Query2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../sde/SDEControler.h"
#include "IGIS_Query2.h"


#if defined(VERSION_NATIVE)
UINT ConditionData[IGIS_Query2::OBJECT_LAST - IGIS_Query2::OBJECT_FIRST][2] = 
{
	{IGIS_Query2::OBJECT_COUNTRY,		IGIS_Query2::CONDITION_NONE			},
	{IGIS_Query2::OBJECT_PROVINCE,		IGIS_Query2::CONDITION_COUNTRY		},
	{IGIS_Query2::OBJECT_CITY,			IGIS_Query2::CONDITION_PROVINCE		},
	{IGIS_Query2::OBJECT_POSTCODE,		IGIS_Query2::CONDITION_PROVINCE		},
	
	{IGIS_Query2::OBJECT_ROAD,			IGIS_Query2::CONDITION_CITY | IGIS_Query2::CONDITION_MATCH_TEXT},
	{IGIS_Query2::OBJECT_CROSSROAD,		IGIS_Query2::CONDITION_ROAD			},
	{IGIS_Query2::OBJECT_HOUSENUMBER,	IGIS_Query2::CONDITION_ROAD			},
	
	{IGIS_Query2::OBJECT_BIGCLASS,		IGIS_Query2::CONDITION_CITY			},
	{IGIS_Query2::OBJECT_SMALLCLASS,	IGIS_Query2::CONDITION_BIGCLASS		},
	{IGIS_Query2::OBJECT_POI,			IGIS_Query2::CONDITION_BIGCLASS	| IGIS_Query2::CONDITION_BASE_ON | IGIS_Query2::CONDITION_LATLOG},
	{IGIS_Query2::OBJECT_POI_BY_INDEX,	IGIS_Query2::CONDITION_INDEX_OF_POI},
	{IGIS_Query2::OBJECT_INDEX_OF_POI,	IGIS_Query2::CONDITION_CITY	| IGIS_Query2::CONDITION_BIGCLASS},
	
	{IGIS_Query2::OBJECT_NEAREST,		IGIS_Query2::CONDITION_LATLOG		},
	{IGIS_Query2::OBJECT_ROADSECTION,		IGIS_Query2::CONDITION_CITY | IGIS_Query2::CONDITION_ROAD		},
};
#else
UINT ConditionData[IGIS_Query2::OBJECT_LAST - IGIS_Query2::OBJECT_FIRST][2] = 
{
	{IGIS_Query2::OBJECT_COUNTRY,		IGIS_Query2::CONDITION_NONE			},
	{IGIS_Query2::OBJECT_PROVINCE,		IGIS_Query2::CONDITION_NONE			},
	{IGIS_Query2::OBJECT_CITY,			IGIS_Query2::CONDITION_COUNTRY		},
	{IGIS_Query2::OBJECT_POSTCODE,		IGIS_Query2::CONDITION_COUNTRY		},
	
	{IGIS_Query2::OBJECT_ROAD,			IGIS_Query2::CONDITION_CITY | IGIS_Query2::CONDITION_MATCH_TEXT},
	{IGIS_Query2::OBJECT_CROSSROAD,		IGIS_Query2::CONDITION_ROAD			},
	{IGIS_Query2::OBJECT_HOUSENUMBER,	IGIS_Query2::CONDITION_ROAD			},
	
	{IGIS_Query2::OBJECT_BIGCLASS,		IGIS_Query2::CONDITION_CITY			},
	{IGIS_Query2::OBJECT_SMALLCLASS,	IGIS_Query2::CONDITION_BIGCLASS		},
	{IGIS_Query2::OBJECT_POI,			IGIS_Query2::CONDITION_BIGCLASS		},
	
	{IGIS_Query2::OBJECT_NEAREST,		IGIS_Query2::CONDITION_LATLOG		},
	{IGIS_Query2::OBJECT_ROADSECTION,		IGIS_Query2::CONDITION_CITY | IGIS_Query2::CONDITION_ROAD		},
};
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CQueryCommonObject,CObject,VERSIONABLE_SCHEMA|1)
void CQueryCommonObject::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CObject::Serialize( archive );

	TRY
	{
		// now do the stuff for our specific class
		if( archive.IsStoring() )
		{
			archive << m_strName;
			archive << m_nId;
			archive << m_dy;
			archive << m_dx;
			
		}
		else
		{
			archive >> m_strName;
			archive >> m_nId;
			archive >> m_dy;
			archive >> m_dx;
			
		}
	}
	CATCH_ALL(e)
	{
		THROW(e);
	}
	END_CATCH_ALL

}

IMPLEMENT_SERIAL(CCountryObject,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CCountryObject::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );

	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
//		archive << m_strName << m_nId;
	}
	else
	{
//		archive >> m_strName >> m_nId;
	}
}
IMPLEMENT_SERIAL(CProvinceObject,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CProvinceObject::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );

	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
//		archive << m_strName << m_nId;
	}
	else
	{
//		archive >> m_strName >> m_nId;
	}
}

IMPLEMENT_SERIAL(CCityObject,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CCityObject::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );

	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
//		archive << m_strName << m_nId;
	}
	else
	{
//		archive >> m_strName >> m_nId;
	}
}

IMPLEMENT_SERIAL(CRoadObject,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CRoadObject::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );

	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
//		archive << m_strName;
	}
	else
	{
//		archive >> m_strName;
	}
}

IMPLEMENT_SERIAL(CRoadPointObject,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CRoadPointObject::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );

	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
//		archive << m_strName << m_dx << m_dy;
	}
	else
	{
//		archive >> m_strName >> m_dx >> m_dy;
	}
}

IMPLEMENT_SERIAL(CStIndexOfPOI,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CStIndexOfPOI::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );

	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
		archive << m_BandID << m_LayerID << m_ClassID;
	}
	else
	{
		archive >> m_BandID >> m_LayerID >> m_ClassID;
	}
}

IMPLEMENT_SERIAL(CStFavoritePoint,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CStFavoritePoint::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );
	
	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
		ASSERT(m_pObj);
		CGIS_FeatureAddress* pFAddress = m_pObj;
		archive<<pFAddress->GetAvoid()<<pFAddress->GetVoice()<<pFAddress->GetAdminID();
		archive<<pFAddress->GetCityID()<<pFAddress->GetAddressID()<<pFAddress->GetMapID();
		archive<<pFAddress->GetAddressName()<<pFAddress->GetAdminName()<<pFAddress->GetPhoneNum();
		archive<<pFAddress->GetStreetName()<<pFAddress->GetZipCode();
		archive<<pFAddress->GetLocation().dx<<pFAddress->GetLocation().dy;
	}
	else
	{
		int			nID = 0;
		CString		strAddName = "";
		UINT		nAdminID = 0;
		CString		strAdminName = "";
		CString		strStreetName = "";
		CString		strZipCode = "";
		UINT		nCityID = 0;
		UINT		nMapID = 0;
		CString		strPhoneNum = "";//�绰����
		BOOL		bVoice = FALSE;//����������õ�ַ50��ʱ��ʾ����
		BOOL		bAvoid = FALSE;//����·��ʱ�ܿ�
		ST_GEO_PTXY pt;
		float		fTmp = 0.0f;
		
		
		archive>>bAvoid>>bVoice>>nAdminID>>nCityID>>nID>>nMapID>>strAddName>>strAdminName;
		archive>>strPhoneNum>>strStreetName>>strZipCode;
		archive>>fTmp;
		pt.dx = fTmp;
		archive>>fTmp;
		pt.dy = fTmp;
		
		CGIS_FeatureAddress * pTmp = new CGIS_FeatureAddress(nID);
		
		pTmp->SetAvoid( bAvoid );
		pTmp->SetVoice( bVoice );
		pTmp->SetAdminID( nAdminID );
		pTmp->SetCityID( nCityID );
		pTmp->SetMapID( nMapID );
		pTmp->SetAddressName( strAddName );
		pTmp->SetAdminName( strAdminName );
		pTmp->SetPhoneNum( strPhoneNum );
		pTmp->SetStreetName( strStreetName );
		pTmp->SetZipCode( strZipCode );
		pTmp->SetLocation( pt );
	
		m_pObj = pTmp;
	}
}


IMPLEMENT_SERIAL(CStGISFeature,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CStGISFeature::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );
	
	int nLen;
	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
		CGIS_FeaturePoint * pObj = (CGIS_FeaturePoint*)m_pObj;

		int nLength = 0;
		char* pStr = NULL;
		CString strData = _T("");

		nLength = pObj->GetAnnoNum();
		pStr = pObj->GetAnnotation();
		strData = CString(pStr,nLength);
		archive << strData << nLength;

		nLength = pObj->GetStrAddressLength();
		pStr = pObj->GetAddress();
		strData = CString(pStr,nLength);
		archive << strData << nLength;

		nLength = pObj->GetStrPhoneLength();
		pStr = pObj->GetPhoneNum();
		strData = CString(pStr,nLength);
		archive << strData << nLength;

		ST_GEO_PTXY * ptXY = pObj->GetPtList();
		if( ptXY )
		{
			nLen = 1;//��ǵ�ĸ���
			archive << nLen;

			archive << ptXY->dx;
			archive << ptXY->dy;
		}
		else
		{
			nLen = 0;
			archive << nLen;
		}
	}
	else
	{
        CGIS_FeaturePoint* pFtrAddr = (CGIS_FeaturePoint*)malloc(sizeof(CGIS_FeaturePoint));
        CGIS_FeaturePoint* pPoint = new (pFtrAddr) CGIS_FeaturePoint(EN_GOTYPE_POINT);
        pPoint->m_nMemmoryType = 0;
            
		m_pObj = pPoint;
		CGIS_FeaturePoint * pObj = (CGIS_FeaturePoint*)m_pObj;

		int nLength = 0;
		char* pStr = NULL;
		CString strData = _T("");
		char* pszTmp = NULL;

		archive >> strData >> nLength;
		if(nLength > 0)
		{
			pszTmp = new char[nLength];
			UnicodeToChar( strData, pszTmp, nLength );	
			pObj->SetAnnotationInfo( nLength, pszTmp );
			delete []pszTmp;
			pszTmp = NULL;
		}


		archive >> strData >> nLength;
		if(nLength > 0)
		{
			pszTmp = new char[nLength];
			UnicodeToChar( strData, pszTmp, nLength );	
			pObj->SetAddressInfo(nLength, pszTmp );
			delete []pszTmp;
			pszTmp = NULL;
		}


		archive >> strData >> nLength;
		if(nLength > 0)
		{
			pszTmp = new char[nLength];
			UnicodeToChar( strData, pszTmp, nLength );	
			pObj->SetPhoneInfo(nLength, pszTmp );
			delete []pszTmp;
			pszTmp = NULL;
		}

		archive >> nLen;
		if( nLen == 1 )
		{
			ST_GEO_PTXY aPtXY;
			float fTmp = 0.0f;
			archive >> fTmp;
			aPtXY.dx = fTmp;
			archive >> fTmp;
			aPtXY.dy = fTmp;

			pObj->SetPtInfo(nLen, &aPtXY);
		}	
	}
}

IMPLEMENT_SERIAL(CSmallClass,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CSmallClass::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );

	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
//		archive << m_strName;
//		archive << m_nId;
	}
	else
	{
//		archive >> m_strName;
//		archive >> m_nId;
	}
}

IMPLEMENT_SERIAL(CRoadSection,CQueryCommonObject,VERSIONABLE_SCHEMA|1)
void CRoadSection::Serialize( CArchive& archive )
{
	// call base class function first
	// base class is CObject in this case
	CQueryCommonObject::Serialize( archive );
	
	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
	}
	else
	{
	}
}
//////////////////////////////////////////////////////////////////////////
TCHAR* CountryData[] =
{
	_T("����"),		_T("����Ⱥ��"),		_T("����"),			_T("Ų��"),
	_T("���"),		_T("����"),			_T("����˹"),		_T("�ڿ���"),
	_T("�׶���˹"),	_T("Ħ������"),		_T("������"),		_T("��ɳ����"),
	_T("����ά��"),	_T("����"),			_T("�ݿ�"),			_T("������"),
	_T("����־"),	_T("�µ���"),		_T("��֧��ʿ��"),	_T("��ʿ"),
	_T("����"),		_T("����ʱ"),		_T("¬ɭ��"),		_T("���е�"),
	_T("ֱ������"),	_T("������"),		_T("������"),		_T("Ħ�ɸ�"),
	_T("������"),	_T("������"),		_T("������"),		_T("�����"),
	_T("��ٸ�"),	_T("ʥ����ŵ"),		_T("�����"),		_T("���޵���"),
	_T("˹�工��"),	_T("˹��������"),	_T("����"),			_T("�����"),
	_T("��˹����"),	_T("��������"),		_T("��������"),		_T("����������"),
	_T("ϣ��")
};

//////////////////////////////////////////////////////////////////////////
TCHAR* ProvinceRawData[] =
{
	_T("ֱϽ��"),	_T("����"),		_T("����"),		_T("����"),
	_T("�㶫"),		_T("����"),		_T("����"),		_T("����"),
	_T("�ӱ�"),		_T("����"),		_T("������"),	_T("����"),
	_T("����"),		_T("����"),		_T("����"),		_T("����"),
	_T("����"),		_T("���ɹ�"),	_T("����"),		_T("�ຣ"),
	_T("ɽ��"),		_T("ɽ��"),		_T("����"),		_T("�Ĵ�"),
	_T("����"),		_T("�½�"),		_T("����"),		_T("�㽭"),
};
//////////////////////////////////////////////////////////////////////////
struct stSmallClassDataItem
{
	TCHAR* pStrName;
	int nSmallClassID;
	int nBigClassID;
};
stSmallClassDataItem SmallClassDataInCombinedBigClass[] =
{
	{_T("����ż�"),9,22},
	{_T("���߿���"),4,18},
	{_T("�����˶�"),6,22},
	{_T("���ξ���"),8,22},
	{_T("�ڽ�"),	4,19},
	{_T("סլ��"),	5,15},
};
//////////////////////////////////////////////////////////////////////////

IGIS_Query2::IGIS_Query2()
	:m_ExitEvent(FALSE,TRUE,NULL,NULL)
{
	m_pSDEControl = NULL;
	m_pThread = NULL;
	Initial();
}

IGIS_Query2::~IGIS_Query2()
{
	CObject* pObj = NULL;
	int nCount = 0;
	int i = 0;

	DeleteAllList();
//////////////////////////////////////////////////////////////////////////
//lijun add 2006-7-21
	{
		POSITION pos = m_lstDirectlyCityRawData.GetHeadPosition();
		while(pos)
		{
			CObject* pObj = m_lstDirectlyCityRawData.GetNext(pos);
			if (pObj)
			{
				ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CObject)));
				delete pObj;
				pObj = NULL;
			}
		}
	}
//////////////////////////////////////////////////////////////////////////

}

BOOL IGIS_Query2::SetCondition(tagSearchCondition& pData)
{
	if (NULL != m_pThread)
	{
		DWORD dwErr = ::WaitForSingleObject(m_pThread->m_hThread,0);
		
		if(WAIT_FAILED == dwErr)
		{
			//TRACE0("SetCondition �ȴ��̶߳���ʧ��,ԭ��������̶߳������˳�.\n");
		}
		else if(WAIT_OBJECT_0 == dwErr)
		{
			//TRACE0("SetCondition ��ѯ�߳����˳�...\n");
		}
		else
		{
			//TRACE0("SetCondition ��ѯ�߳�������,�����Խ����µĲ�ѯ��������.\n");
			return FALSE;
		}

	}

	m_Condition = pData;
	m_CurrentPos = NULL;
	
	return TRUE;
}

void IGIS_Query2::GetCondition(tagSearchCondition& pData)
{
	pData = m_Condition;
}

BOOL IGIS_Query2::SearchStart()
{
	//�����ѯ�߳���������,֪ͨ��ѯ�߳��˳�.
	WaitSearchThreadExit();
	
	//////////////////////////////////////////////////////////////////////////
	//��������Ƿ��㹻
	if(!IfConditionEnough())
	{
		return FALSE;
	}
	DeleteAllList();
	
	//////////////////////////////////////////////////////////////////////////
	
	m_SearchThreadData.hExitEvent = m_ExitEvent.m_hObject;
	m_SearchThreadData.hWnd = NULL;
	m_SearchThreadData.pGis_Query2 = this;
	TRACE0("������ѯ�߳�\n");
	//	m_pThread = AfxBeginThread(SearchThread,(LPVOID)&m_SearchThreadData,THREAD_PRIORITY_NORMAL,0);
	m_pThread = AfxBeginThread(SearchThread,(LPVOID)&m_SearchThreadData,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	m_pThread->m_bAutoDelete = FALSE;
	m_pThread->ResumeThread();
	
	return TRUE;
}

BOOL IGIS_Query2::IfConditionEnough()
{
	for (int i = 0; i < OBJECT_LAST - OBJECT_FIRST; i++)
	{
		if(ConditionData[i][0] == m_Condition.nResult )
		{
			if ( (ConditionData[i][1]&m_Condition.nConditionMask) == ConditionData[i][1])
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	return FALSE;
}

BOOL IGIS_Query2::Initial()
{
	ASSERT(m_pSDEControl == NULL);

	CSDEControler *pControl = CSDEControler::GetSDEControl();
//	CString temp = L"\\Storage Card\\MobileNavigator\\MAP";
//	pControl->InitSDE(temp);	//����ֵ��ʾ�Ƿ��һ�ε��ã��������

	m_pSDEControl = pControl;
	m_QueryTool.InitMapList();

//////////////////////////////////////////////////////////////////////////
//lijun add 2006-7-21
/*�ڳ�ʼ��ʱ��ֱϽ�г����б���ؽ����Ա�ʹ��.*/
	
	IGIS_MapManager *pMapManager = m_pSDEControl->GetMapManager();
	CPtrList *pLstOfObject = pMapManager->GetCityList(86,CITY_LIST_ZHIXIA);
	ASSERT(pLstOfObject != NULL);
	if (pLstOfObject->IsEmpty())
	{
		return FALSE;
	}
	
	int nCount = pLstOfObject->GetCount();
	POSITION pos = pLstOfObject->GetHeadPosition();
	
	int nID = 0;
	int nItem = -1;
	while( pos )
	{
		ST_CCITY_INFO* pObj = (ST_CCITY_INFO*)pLstOfObject->GetNext(pos);
		if( pObj )
		{
			//CString strText(pObj->strCityName);	
			CString strText = _T("");
			CharToUnicode(pObj->strCityName, &strText);
			int nId = pObj->nCityID;

			CObject* pAddObj = new CProvinceObject(strText,nId);
			m_lstDirectlyCityRawData.AddTail(pAddObj);
		}
	}
//////////////////////////////////////////////////////////////////////////	
	return TRUE;
}

void IGIS_Query2::DeleteAllList()
{
	{
		POSITION pos = m_lstObject.GetHeadPosition();
		while(pos)
		{
			CObject* pObj = m_lstObject.GetNext(pos);
			if (pObj)
			{
				ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CObject)));
				delete pObj;
				pObj = NULL;
			}
		}
	}
	m_lstObject.RemoveAll();
	
	m_QueryTool.RemoveAllPOI();
	m_QueryTool.RemoveSubPOI();
	m_QueryTool.RemoveAllRoad();
	m_QueryTool.RemoveRSecList();
	m_QueryTool.RemovePOIList();
}

/**
 * @brief ��ѯ�߳�
 *
 * @param 
 * @return
*/
//##ModelId=444C9A9E02B0
UINT IGIS_Query2::SearchThread(void* pParam)
{
	tagSearchThreadData* pData = (tagSearchThreadData*)pParam;
	HANDLE hExit = pData->hExitEvent;
	
	IGIS_Query2* pGIS_Query2 = pData->pGis_Query2;
	ASSERT(pGIS_Query2 != NULL);
	
	
	//���ɴ���,��һ�δ�Ĳ��ҹ���
	{
		switch(pGIS_Query2->m_Condition.nResult)
		{
		case OBJECT_COUNTRY:
			{
				{
					IGIS_MapManager *pMapManager = pGIS_Query2->m_pSDEControl->GetMapManager();
					CPtrList *pLstOfObject = pMapManager->GetCountryList();
					int nCount = pLstOfObject->GetCount();
					POSITION pos = pLstOfObject->GetHeadPosition();
					
					int nID = 0;
					int nItem = -1;
					while( pos )
					{
						ST_CCOUNTRY_INFO* pObj = (ST_CCOUNTRY_INFO*)pLstOfObject->GetNext(pos);
						if( pObj )
						{
							CString strText = _T("");
							CharToUnicode(pObj->strCName, &strText);
							int nId = pObj->nCountryID;
							CObject* pAddObj = new CCountryObject(strText,nId);
							pGIS_Query2->m_lstObject.AddTail(pAddObj);
						}
					}
				}
			}
			break;
		case OBJECT_PROVINCE:
			{
				{
					//////////////////////////////////////////////////////////////////////////
					//LIJUN ADD 2006-7-21
					POSITION pos = pGIS_Query2->m_lstDirectlyCityRawData.GetHeadPosition();
					while(pos)
					{
						CObject* pObj = pGIS_Query2->m_lstDirectlyCityRawData.GetNext(pos);
						ASSERT(pObj && pObj->IsKindOf(RUNTIME_CLASS(CObject)));
						ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CProvinceObject)));
						CProvinceObject* pObject = (CProvinceObject*)pObj;
						
						if(pGIS_Query2->m_Condition.nConditionMask&IGIS_Query2::CONDITION_MATCH_TEXT)
						{
							CString strText = pObject->m_strName;
							CString strMatchText = pGIS_Query2->m_Condition.strMatchText;
							CString strSource = strText.Left(strMatchText.GetLength());
							
							if(!_tcscmp(strSource,strMatchText))
							{
								CObject* pObj = new CProvinceObject(*pObject);
								pGIS_Query2->m_lstObject.AddTail(pObj);
							}
						}
						else
						{
							CObject* pObj = new CProvinceObject(*pObject);
							pGIS_Query2->m_lstObject.AddTail(pObj);
						}
					}
				}
				//////////////////////////////////////////////////////////////////////////
				

				
				int nCountryId = pGIS_Query2->m_Condition.nCountryId; 
				//_stscanf(pGIS_Query2->m_Condition.strCountry,_T("%d"),&nCountryId);
				{
					IGIS_MapManager *pMapManager = pGIS_Query2->m_pSDEControl->GetMapManager();
					CPtrList *pLstOfObject = pMapManager->GetProvinceList(nCountryId);
					int nCount = pLstOfObject->GetCount();
					POSITION pos = pLstOfObject->GetHeadPosition();
					
					int nID = 0;
					int nItem = -1;
					while( pos )
					{
						ST_CPRO_INFO* pObj = (ST_CPRO_INFO*)pLstOfObject->GetNext(pos);
						if( pObj )
						{
							CString strText = _T("");
							CharToUnicode(pObj->strPName, &strText);
							if(pGIS_Query2->m_Condition.nConditionMask&IGIS_Query2::CONDITION_MATCH_TEXT)
							{
								CString strMatchText = pGIS_Query2->m_Condition.strMatchText;
								CString strSource = strText.Left(strMatchText.GetLength());
								if(_tcscmp(strSource,strMatchText))
									continue;
							}
							
							int nId = pObj->nProvinceID;
							CObject* pAddObj = new CProvinceObject(strText,nId);
							pGIS_Query2->m_lstObject.AddTail(pAddObj);
						}
					}
				}
				
			}
			break;
		case OBJECT_CITY:
			{
				//TRACE0("Query City Start\n");
				int nCountryId;
				int nProvinceId; 
//				_stscanf(pGIS_Query2->m_Condition.strCountry,_T("%d"),&nCountryId);
//				_stscanf(pGIS_Query2->m_Condition.strProvince,_T("%d"),&nProvinceId);
				nCountryId = pGIS_Query2->m_Condition.nCountryId;
				nProvinceId = pGIS_Query2->m_Condition.nProvinceId;
				{
					IGIS_MapManager *pMapManager = pGIS_Query2->m_pSDEControl->GetMapManager();
					CPtrList *pLstOfObject = pMapManager->GetCityList(nCountryId,nProvinceId);
					ASSERT(pLstOfObject);
					
					int nCount = pLstOfObject->GetCount();
					if(nCount == 0)
					{
						ST_CCITY_INFO* pObj = (ST_CCITY_INFO*)pMapManager->GetCityByID(nCountryId,nProvinceId);

						if( pObj )
						{
							//CString strText(pObj->strCityName);
							CString strText = _T("");
							CharToUnicode(pObj->strCityName, &strText);
							
							if(pGIS_Query2->m_Condition.nConditionMask & IGIS_Query2::CONDITION_MATCH_TEXT)
							{
								CString strMatchText = pGIS_Query2->m_Condition.strMatchText;
								CString strSource = strText.Left(strMatchText.GetLength());
								if(!_tcscmp(strSource,strMatchText))
								{
									int nId = pObj->nCityID;
									CObject* pAddObj = new CCityObject(strText,nId);
									pGIS_Query2->m_lstObject.AddTail(pAddObj);
								}
								
							}
							else
							{
								int nId = pObj->nCityID;
								CObject* pAddObj = new CCityObject(strText,nId);
								pGIS_Query2->m_lstObject.AddTail(pAddObj);
							}
						
						}
					}
					else
					{
						POSITION pos = pLstOfObject->GetHeadPosition();
						int nID = 0;
						int nItem = -1;
						while( pos )
						{
							ST_CCITY_INFO* pObj = (ST_CCITY_INFO*)pLstOfObject->GetNext(pos);
							if( pObj )
							{
								//CString strText(pObj->strCityName);
								CString strText = _T("");
								CharToUnicode(pObj->strCityName, &strText);
								
								if(pGIS_Query2->m_Condition.nConditionMask & IGIS_Query2::CONDITION_MATCH_TEXT)
								{
									CString strMatchText = pGIS_Query2->m_Condition.strMatchText;
									CString strSource = strText.Left(strMatchText.GetLength());
									if(_tcscmp(strSource,strMatchText))
										continue;
								}
								
								int nId = pObj->nCityID;
								CObject* pAddObj = new CCityObject(strText,nId);
								pGIS_Query2->m_lstObject.AddTail(pAddObj);
							}
						}
					}
				}	
			}
			break;
		case OBJECT_POSTCODE:
			{
				ASSERT(FALSE);
			}
			break;
		case OBJECT_ROAD:
			{
				int nCityId; 
//				_stscanf(pGIS_Query2->m_Condition.strCity,_T("%d"),&nCityId);
				nCityId = pGIS_Query2->m_Condition.nCityId;
				{
					//��·����������,�˲����ܿ�,��ʱ����
					pGIS_Query2->m_QueryTool.SetCityID(nCityId);
					pGIS_Query2->m_QueryTool.QueryRoad(pGIS_Query2->m_Condition.strMatchText);
					CStringArray& arRoad = pGIS_Query2->m_QueryTool.GetRoadArray();
					
					int nCount = arRoad.GetSize();
					for (int i = 0; i < nCount; i++)
					{
						CObject* pAddObj = new CRoadObject(arRoad[i]);
						pGIS_Query2->m_lstObject.AddTail(pAddObj);
					}
				}	
			}
			break;
		case OBJECT_CROSSROAD:
			{
				pGIS_Query2->m_QueryTool.SetRoad(pGIS_Query2->m_Condition.strRoad);
				pGIS_Query2->m_QueryTool.QueryRoadCross(pGIS_Query2->m_Condition.strRoad2);
				CPtrList* pLstOfObject = pGIS_Query2->m_QueryTool.GetCrossRoadList();
					
				int nCount = pLstOfObject->GetCount();
				POSITION pos = pLstOfObject->GetHeadPosition();
				
				int nID = 0;
				int nItem = -1;
				while( pos )
				{
					ST_ROAD_PT* pObj = (ST_ROAD_PT*)pLstOfObject->GetNext(pos);
					if( pObj )
					{
						CString strText = pObj->Name;
						CObject* pAddObj = new CRoadPointObject(strText,pObj->pt.dx,pObj->pt.dy);
						pGIS_Query2->m_lstObject.AddTail(pAddObj);
					}
				}
			}
			break;
		case OBJECT_HOUSENUMBER:
			{
				ASSERT(FALSE);
			}
			break;
		case OBJECT_ROADSECTION:
			{
				//pGIS_Query2->m_QueryTool.QueryRoad(pGIS_Query2->m_Condition.strRoad);
				pGIS_Query2->m_QueryTool.SetRoad(pGIS_Query2->m_Condition.strRoad);
				pGIS_Query2->m_QueryTool.QueryRoadSection();
				CPtrList* pLstOfObject = pGIS_Query2->m_QueryTool.GetRoadSecList();	
					
				int nCount = pLstOfObject->GetCount();
				POSITION pos = pLstOfObject->GetHeadPosition();
				
				int nID = 0;
				int nItem = -1;
				while( pos )
				{
					ST_ROAD_INNAME* pObj = (ST_ROAD_INNAME*)pLstOfObject->GetNext(pos);
					if( pObj )
					{
						CObject* pAddObj = new CRoadSection(pObj);
						{
							//this code for the better view name of the road
							ASSERT(pAddObj->IsKindOf(RUNTIME_CLASS(CRoadSection)));
							CRoadSection* pObject = (CRoadSection*)pAddObj;
							CString strText = pObject->m_strName;
							strText = pGIS_Query2->m_Condition.strRoad + _T("(") + strText + _T(")") ;
							pObject->m_strName = strText;
						}
						pGIS_Query2->m_lstObject.AddTail(pAddObj);
					}
				}
			}
			break;
		case OBJECT_POI:
			{
				int nRealBigClass = pGIS_Query2->m_Condition.nBigClass;
				int nRealSmallClass = pGIS_Query2->m_Condition.nSmallClass;
				
				int nBigClass = pGIS_Query2->m_Condition.nBigClass;
				if(nBigClass == 1)
				{
					//�˴��಻�ṩ������С��Ĺ���,����һ��Ҫ����С��
					ASSERT(pGIS_Query2->m_Condition.nConditionMask&IGIS_Query2::CONDITION_SMALLCLASS);
					int nIndex = pGIS_Query2->m_Condition.nSmallClass;
					
					nRealBigClass = SmallClassDataInCombinedBigClass[nIndex].nBigClassID;
					nRealSmallClass = SmallClassDataInCombinedBigClass[nIndex].nSmallClassID;
				}

				

				pGIS_Query2->m_QueryTool.SetPOIType(nRealBigClass);
				
				if(pGIS_Query2->m_Condition.nConditionMask&IGIS_Query2::CONDITION_SMALLCLASS)
				{
					pGIS_Query2->m_QueryTool.SetSubPOIType(nRealSmallClass);
				}
				else
				{
					pGIS_Query2->m_QueryTool.SetSubPOIType(0);
				}

				if(pGIS_Query2->m_Condition.nConditionMask&IGIS_Query2::CONDITION_MATCH_TEXT )
				{
					CString strText = pGIS_Query2->m_Condition.strMatchText;
					pGIS_Query2->m_QueryTool.SetPOIName( strText );
				}
				
				switch(pGIS_Query2->m_Condition.eSearchRangeBaseon)
				{
				case IGIS_Query2::RANGE_BASE_ON_POINT:
					{
						pGIS_Query2->m_QueryTool.SetBNetwork(FALSE);
					}
					break;
				case IGIS_Query2::RANGE_BASE_ON_LINE:
					{
						pGIS_Query2->m_QueryTool.SetBNetwork(TRUE);
						ASSERT(pGIS_Query2->m_Condition.pLineBaseOn);
						pGIS_Query2->m_QueryTool.SetNetWorkRoute((CGIS_NetworkRoute*)pGIS_Query2->m_Condition.pLineBaseOn);
					}
					break;
				case IGIS_Query2::RANGE_BASE_ON_CITY:
					{
						ASSERT(FALSE);
						pGIS_Query2->m_QueryTool.QueryPOIByType(1);
					}
					break;
				default:
					ASSERT(FALSE);
					break;
				}
				
				ST_GEO_PTXY aPt;
				aPt.dx = pGIS_Query2->m_Condition.fLog;
				aPt.dy = pGIS_Query2->m_Condition.fLat;
				
				pGIS_Query2->m_QueryTool.SetCurPos(aPt);
				pGIS_Query2->m_QueryTool.QueryPOIByType(2);
				//	pGIS_Query2->m_QueryTool.QuerySubPOIByType();
				
				CPtrList* pLstOfObject = pGIS_Query2->m_QueryTool.GetPOIList();//�����������
				
				int nCount = pLstOfObject->GetCount();
				TRACE1("OBJECT_POI:%d\n",nCount);
				POSITION pos = pLstOfObject->GetHeadPosition();
				
				int nID = 0;
				int nItem = -1;
				while( pos )
				{
					CGIS_Feature* pObj = (CGIS_Feature*)pLstOfObject->GetNext(pos);
					//TRACE1("ST_FEATURE_POINT: %s\n",pObj->pFeature->GetAnnotation());
					if( pObj )
					{
						CObject* pAddObj = new CStGISFeature(pObj->Clone());
						pGIS_Query2->m_lstObject.AddTail(pAddObj);
					}
				}
				
				
				//pGIS_Query2->m_QueryTool.GetPOIList();//�����������
			}
			break;
		case OBJECT_NEAREST://Ŀǰ�ɲ�5���������еĻ�ִ���С���
			{
				{
					ST_GEO_PTXY aPt;
					aPt.dx = pGIS_Query2->m_Condition.fLog;
					aPt.dy = pGIS_Query2->m_Condition.fLat;
					
					pGIS_Query2->m_QueryTool.SetCurPos(aPt);
					
					IGIS_Query2::tagSearchCondition aSearchCondition;
					pGIS_Query2->GetCondition(aSearchCondition);
					if (!(aSearchCondition.nConditionMask&IGIS_Query2::CONDITION_BIGCLASS))
					{
						//���δ���ô���
					}
					else
					{
						pGIS_Query2->m_QueryTool.SetPOIType(aSearchCondition.nBigClass);

						if(!(aSearchCondition.nConditionMask&IGIS_Query2::CONDITION_SMALLCLASS))
						{
							//���δ����С��
						}
						else	
						{
							pGIS_Query2->m_QueryTool.SetSubPOIType(aSearchCondition.nSmallClass);
						}
					}
					
					CString strMatchText = pGIS_Query2->m_Condition.strMatchText;
					
					switch(pGIS_Query2->m_Condition.eSearchRangeBaseon)
					{
					case IGIS_Query2::RANGE_BASE_ON_POINT:
						{
							pGIS_Query2->m_QueryTool.SetBNetwork(FALSE);
						}
						break;
					case IGIS_Query2::RANGE_BASE_ON_LINE:
						{
							pGIS_Query2->m_QueryTool.SetBNetwork(TRUE);
							ASSERT(pGIS_Query2->m_Condition.pLineBaseOn);
							pGIS_Query2->m_QueryTool.SetNetWorkRoute((CGIS_NetworkRoute*)pGIS_Query2->m_Condition.pLineBaseOn);
						}
						break;
					default:
						ASSERT(FALSE);
						break;
					}

					pGIS_Query2->m_QueryTool.QueryAllPOI( strMatchText );


					CPtrList* pLstOfObject = pGIS_Query2->m_QueryTool.GetPOIList();//�����������
					POSITION pos = pLstOfObject->GetHeadPosition();
					while (pos)
					{
						CGIS_Feature * pObj = (CGIS_Feature*)pLstOfObject->GetNext(pos);
						if( pObj )
						{
							CObject* pAddObj = new CStGISFeature(pObj->Clone());
							pGIS_Query2->m_lstObject.AddTail(pAddObj);
						}
					}
					
//					int nCount = pLstOfObject->GetCount();
//					TRACE1("OBJECT_NEAREST:%d\n",nCount);
//					POSITION pos = pLstOfObject->GetHeadPosition();
//					
//					int nID = 0;
//					int nItem = -1;
//					while( pos )
//					{
//						CGIS_Feature* pObj = (CGIS_Feature*)pLstOfObject->GetNext(pos);
//						//TRACE1("ST_FEATURE_POINT: %s\n",pObj->pFeature->GetAnnotation());
//						if( pObj )
//						{
//							CString strText(pObj->GetAnnotation());
//
//							if(pGIS_Query2->m_Condition.nConditionMask & IGIS_Query2::CONDITION_MATCH_TEXT)
//							{
//								CString strMatchText = pGIS_Query2->m_Condition.strMatchText;
//								CString strSource = strText.Left(strMatchText.GetLength());
//								if(_tcscmp(strSource,strMatchText))
//									continue;
//							}
//
//							CObject* pAddObj = new CStGISFeature(pObj->Clone());
//							pGIS_Query2->m_lstObject.AddTail(pAddObj);
//						}
//					}				
				}
				
				//////////////////////////////////////////////////////////////////////////
				//			m_QueryTool.SetPOIType(m_Condition.nBigClass);
				//			m_QueryTool.QueryPOIByType(2);
				//			m_QueryTool.SetSubPOIType(m_Condition.nSmallClass);
				//			m_QueryTool.QuerySubPOIByType();
				//			m_QueryTool.GetPOIList();//�����������
				
				//////////////////////////////////////////////////////////////////////////
			}
			break;
		case OBJECT_SMALLCLASS://��ѯС���б�
			{
				int nBigClass = pGIS_Query2->m_Condition.nBigClass;
				if(nBigClass == 1)
				{
					int nCount = sizeof(SmallClassDataInCombinedBigClass)/sizeof(stSmallClassDataItem);
					//�Զ��������Ĵ���IDΪ1
					for(int i = 0; i < nCount; i++)
					{
						CString strText(SmallClassDataInCombinedBigClass[i].pStrName);
						int nId = i;
						CObject* pAddObj = new CSmallClass(strText,nId);
						pGIS_Query2->m_lstObject.AddTail(pAddObj);	
					}
				}
				else
				{
					pGIS_Query2->m_QueryTool.SetPOIType(pGIS_Query2->m_Condition.nBigClass);//���ô���
					CPtrList* pLstOfObject = pGIS_Query2->m_QueryTool.GetPOISubTypeList();
					
					int nCount = pLstOfObject->GetCount();
					TRACE1("OBJECT_SMALLCLASS:%d\n",nCount);
					POSITION pos = pLstOfObject->GetHeadPosition();
					
					int nID = 0;
					int nItem = -1;
					while( pos )
					{
						ST_SUBPOITYPE_INFO* pObj = (ST_SUBPOITYPE_INFO*)pLstOfObject->GetNext(pos);
						if( pObj )
						{
							//CString strText(pObj->strSubPOIName);
							CString strText = _T("");
							CharToUnicode(pObj->strSubPOIName, &strText);
							int nId = pObj->nTypeID;
							CObject* pAddObj = new CSmallClass(strText,nId);
							pGIS_Query2->m_lstObject.AddTail(pAddObj);
						}
					}
				}	
			}
			break;
		case OBJECT_INDEX_OF_POI://���ݳ���ID��С��id��POI�б������.
			{
				pGIS_Query2->m_QueryTool.SetCityID( pGIS_Query2->m_Condition.nCityId );
				pGIS_Query2->m_QueryTool.SetPOIType(pGIS_Query2->m_Condition.nBigClass);
				pGIS_Query2->m_QueryTool.QueryPOIByType(1);
				
				CPtrList* pLstOfObject = pGIS_Query2->m_QueryTool.GetPOIList();//�����������
				
				int nCount = pLstOfObject->GetCount();
				TRACE1("OBJECT_INDEX_OF_POI:%d\n",nCount);
				POSITION pos = pLstOfObject->GetHeadPosition();
				
				int nID = 0;
				int nItem = -1;
				while( pos )
				{
					ST_INDEX_NAME* pObj = (ST_INDEX_NAME*)pLstOfObject->GetNext(pos);
					//TRACE1("ST_FEATURE_POINT: %s\n",pObj->pFeature->GetAnnotation());
					if( pObj )
                    {
                        CString strText = pObj->Name;
                        if(pGIS_Query2->m_Condition.nConditionMask & IGIS_Query2::CONDITION_MATCH_TEXT)
                        {
                            CString strMatchText = pGIS_Query2->m_Condition.strMatchText;
                            CString strSource = strText.Left(strMatchText.GetLength());
                            if(_tcscmp(strSource,strMatchText))
                                continue;
                        }


						CObject* pAddObj = new CStIndexOfPOI(pObj);
						pGIS_Query2->m_lstObject.AddTail(pAddObj);
					}
				}
			}
			break;
		case OBJECT_POI_BY_INDEX://�����������poi(Ŀǰֻ��һ��)
			{
				ST_INDEX_NAME aStIndex;

				char szName[128] = {0}; 
				CString strName = pGIS_Query2->m_Condition.strNameOfIndex;
				ASSERT( strName.GetLength() < 64 );
				UnicodeToChar( strName, szName, 128 );

				aStIndex.Name = szName;
				aStIndex.BandID = pGIS_Query2->m_Condition.BandIDOfIndex;
				aStIndex.ClassID = pGIS_Query2->m_Condition.ClassIDOfIndex;
				aStIndex.LayerID = pGIS_Query2->m_Condition.LayerIDOfIndex;

				CGIS_Feature* pObj = pGIS_Query2->m_QueryTool.GetFeatureByIndex(&aStIndex);
				if( pObj )
				{
					CObject* pAddObj = new CStGISFeature(pObj);
					pGIS_Query2->m_lstObject.AddTail(pAddObj);
				}
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	

	//////////////////////////////////////////////////////////////////////////
	//�Ѳ鵽�Ķ���֪ͨ��Ӧ��	
	UPDATEPROC UpdateProc =  pData->pGis_Query2->m_Condition.lpfnUpdateProc;
	POSITION pos = pGIS_Query2->m_lstObject.GetHeadPosition();
	while(pos)
	{
		//�鿴һ�²�ѯ�߳��˳��ź���,����Ϊ֪ͨ״̬��ȡ��SDE�Ĳ�ѯ,�����Խ����߳�
		if(WAIT_OBJECT_0 == ::WaitForSingleObject(hExit,0))
		{
			TRACE0("��ѯ�߳��յ��˳�֪ͨ.\n");
			break;
		}

		POSITION aPos = pos;
		CObject* pObj =  pGIS_Query2->m_lstObject.GetNext(pos);
		pGIS_Query2->m_lstObject.RemoveAt(aPos);
		ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CObject)));
		(*UpdateProc)(UPDATE_NEW_OBJECT, (DWORD)pObj);//֪ͨ���µĶ���
	}

	(*UpdateProc)(UPDATE_STOP, (DWORD)0);//֪ͨ��ѯ����
	TRACE0("��ѯ�̷߳���.\n");
	return 1;
}

BOOL IGIS_Query2::SearchStop()
{
	TRACE0("�Բ�ѯ�߳̽�����ֹ...\n");
	m_QueryTool.SetStopSearch();
	return	WaitSearchThreadExit();
}

BOOL IGIS_Query2::WaitSearchThreadExit()
{
	if (NULL != m_pThread)
	{
		//TRACE0("��ѯ�̶߳���Ϊ��.\n");
		m_ExitEvent.SetEvent(); //�����˳��ź���
		Sleep(200);
		DWORD dwErr = ::WaitForSingleObject(m_pThread->m_hThread,0);
		
		if(WAIT_FAILED == dwErr)
		{
			TRACE0("�ȴ��̶߳���ʧ��,ԭ��������̶߳������˳�.\n");
		}
		else if(WAIT_OBJECT_0 == dwErr)
		{
			TRACE0("��ѯ�߳����˳�...\n");
		}
		else
		{
			TRACE0("��ѯ�߳�������,����֪ͨ�䷵��...\n");

			while(WAIT_OBJECT_0 != ::WaitForSingleObject(m_pThread->m_hThread,0))
			{
				Sleep(200);
			}
			TRACE0("��ѯ�߳����˳�.\n");

		}

		m_ExitEvent.ResetEvent();
		
		TRACE0("ɾ����ѯ�̶߳���.\n");
		delete m_pThread;
		m_pThread = NULL;
	}

	return TRUE;
}
