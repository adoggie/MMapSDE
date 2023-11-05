// GIS_NetworkPathInfo.cpp: implementation of the CGIS_NetworkPathInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkPathInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
FILE * CGIS_NetworkPathInfo::m_pVFile = NULL;

CGIS_NetworkPathInfo::CGIS_NetworkPathInfo( swString strVFile )
{
	m_enCalMethod = EN_CPMETHOD_QUICKESTCAL;
	m_enRoutePref = EN_RUTPREF_FASTERTIME;
	m_enVehicleType = EN_VEHICLETYPE_CAR;
	m_pARMapIDs = NULL;
	m_pARBIDs = NULL;
	m_pARRutIDs = NULL;
	m_nARNum = 0;
	m_pLimitRMapIDs = NULL;
	m_pLimitRBIDs = NULL;
	m_pLimitRRutIDs = NULL;
	m_nLimitRNum = 0;
	m_bTollEvite = m_bHWEvite = m_bSelEvite = m_bUnlayEvite = FALSE;
	if (NULL == (m_pVFile = _wfopen(strVFile,L"rt")) )
		m_pVFile = NULL;
	m_pFRut = NULL;
	m_nDType = 3;

    m_fInterSectionClipRectangleExtendX =  0.00004f;
    m_fInterSectionClipRectangleExtendY =  0.00004f;

    m_nStartCalculateTime = 0xff;
}

CGIS_NetworkPathInfo::~CGIS_NetworkPathInfo()
{
	delete []m_pARMapIDs;
	delete []m_pARRutIDs;
	fclose( m_pVFile );
	m_pARMapIDs = NULL;
	m_pARRutIDs = NULL;
	m_nARNum = 0;

	if (m_pLimitRMapIDs != NULL)
	{
		delete []m_pLimitRMapIDs;
		m_pLimitRMapIDs = NULL;
	}
	if (m_pLimitRBIDs != NULL)
	{
		delete []m_pLimitRBIDs;
		m_pLimitRBIDs = NULL;
	}
	if (m_pLimitRRutIDs != NULL)
	{
		delete []m_pLimitRRutIDs;
		m_pLimitRRutIDs = NULL;
	}
}

EnCalPathMethod CGIS_NetworkPathInfo::GetCalPathMethod( )
{
	return m_enCalMethod;
}
void CGIS_NetworkPathInfo::SetCalPathMethod( EnCalPathMethod enCalMethod )
{
	m_enCalMethod = enCalMethod;
}
EnRoutePreference CGIS_NetworkPathInfo::GetRoutePreference( )
{
	return m_enRoutePref;
}
void CGIS_NetworkPathInfo::SetRoutePreference( EnRoutePreference enRoutePref )
{
	m_enRoutePref = enRoutePref;
}
EnVehicleType CGIS_NetworkPathInfo::GetVehicleType( )
{
	return m_enVehicleType;
}
void CGIS_NetworkPathInfo::SetVehicleType( EnVehicleType enVehicleType )
{
	m_enVehicleType = enVehicleType;
}
void CGIS_NetworkPathInfo::SetEviteInfo( BOOL bTollEvite, BOOL bHWEvite, BOOL bSelEvite, BOOL bUnlayEvite )
{
	m_bTollEvite = bTollEvite;
	m_bHWEvite = bHWEvite;
	m_bSelEvite = bSelEvite;
	m_bUnlayEvite = bUnlayEvite;
}
BOOL CGIS_NetworkPathInfo::IsTollEvite( )
{
	return m_bTollEvite;
}
BOOL CGIS_NetworkPathInfo::IsHWayEvite( )
{
	return m_bHWEvite;
}
BOOL CGIS_NetworkPathInfo::IsSelEvite( )
{
	return m_bSelEvite;
}
BOOL CGIS_NetworkPathInfo::IsUnlayEvite( )
{
	return m_bUnlayEvite;
}

void CGIS_NetworkPathInfo::SetAvoidRutInfo( UINT nARNum, int *pARMapIDs, int *pARBIDs, int *pARRutIDs )
{
	m_nARNum = nARNum;
	m_pARMapIDs = pARMapIDs;
	m_pARBIDs = pARBIDs;
	m_pARRutIDs = pARRutIDs;
}
void CGIS_NetworkPathInfo::GetAvoidRutInfo( UINT &nARNum, int **pARMapIDs, int **pARBIDs, int **pARRutIDs )
{
	nARNum = m_nARNum;
	*pARMapIDs = m_pARMapIDs;
	*pARBIDs = m_pARBIDs; 
	*pARRutIDs = m_pARRutIDs;
}

void CGIS_NetworkPathInfo::SetLimitRutInfo( UINT nARNum, int *pARMapIDs, int *pARBIDs, int *pARRutIDs )
{
	if (m_pLimitRMapIDs != NULL)
	{
		delete []m_pLimitRMapIDs;
		m_pLimitRMapIDs = NULL;
	}
	if (m_pLimitRBIDs != NULL)
	{
		delete []m_pLimitRBIDs;
		m_pLimitRBIDs = NULL;
	}
	if (m_pLimitRRutIDs != NULL)
	{
		delete []m_pLimitRRutIDs;
		m_pLimitRRutIDs = NULL;
	}
	if (nARNum > 0)
	{
		m_nLimitRNum = nARNum;
		m_pLimitRMapIDs = new int[nARNum];
		m_pLimitRBIDs = new int[nARNum];
		m_pLimitRRutIDs = new int[nARNum];
		*m_pLimitRMapIDs = *pARMapIDs;
		*m_pLimitRBIDs = *pARBIDs;
		*m_pLimitRRutIDs = *pARRutIDs;
	}
}
void CGIS_NetworkPathInfo::GetLimitRutInfo( UINT &nARNum, int **pARMapIDs, int **pARBIDs, int **pARRutIDs )
{
	nARNum = m_nLimitRNum;
	*pARMapIDs = m_pLimitRMapIDs;
	*pARBIDs = m_pLimitRBIDs; 
	*pARRutIDs = m_pLimitRRutIDs;
}

//判断节点处转弯方向和类型
void CGIS_NetworkPathInfo::JudgeNodeAngleAndTurn( int nAngle, BYTE &byAngle, BYTE &byTurn )
{
	int nAng = abs( abs(nAngle)-180 );
	//直行
	if( nAng <= 30 )
		byAngle = 1;
	else if( nAng <= 65 )
		byAngle = 2;
	else if( nAng <= 135 )
		byAngle = 3;
	else if( nAng < 150 )
		byAngle = 4;
	else 
		byAngle = 5;
	//拐弯方向(27．左28．右)
	if( nAngle < 0 )
	{
		if( abs(nAngle) > 180 )
			byTurn = 27;
		else
			byTurn = 28;
	}
	else
	{
		if( nAngle > 180 )
			byTurn = 28;
		else
			byTurn = 27;
	}
	if( nAngle < -1000 )
		byAngle = 6;
}


//========================================================================
// 根据提供的索引，从语音文件中找到相应的语音信息，总共存在长距离 短距离
//
// 接近路口3种。同时去掉语音文本中的换行符。
//
// vInd(in)：索引结构
//
// strLVoice(out)：长距离信息，strSVoice(out)：短距离信息，
//
// strIVoice(out)：接近路口信息
//========================================================================
BOOL CGIS_NetworkPathInfo::GetVoiceString(ST_VOICE_INDEX vInd,
				swString &strLVoice, swString &strSVoice, swString &strIVoice)
{
	if(m_pVFile == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	if(vInd.strLong.IsEmpty() || vInd.strShort.IsEmpty() ||
		vInd.strIntersec.IsEmpty())
	{
		return FALSE;
	}

	fseek( m_pVFile, 0, SEEK_SET );
	char szLine[500];
	swString strLine;
    int nCount = 0;
    while ( !feof(m_pVFile) )
    {
        if ( fgets( szLine, MAX_LINE_LENGTH, m_pVFile ) != NULL)
        {
            CharToUnicode( szLine , &strLine );
            if ( strLine.Find(vInd.strLong) == 0 )
            {
                int nPos = strLine.Find(L':');
                int nIndexLength = vInd.strLong.GetLength();
                if (nPos == nIndexLength)
                {
					int nOffSet = 1;
					if (strLine.Find(_T("\n")) != -1)
						nOffSet++;
                    strLVoice = strLine.Mid(nPos+1, strLine.GetLength()-nPos-nOffSet);
                    nCount++;
                    if( nCount == 3 )
                        return TRUE;   
                }
            }
            if( strLine.Find(vInd.strShort) == 0 )
            {
                int nPos = strLine.Find(L':');
                int nIndexLength = vInd.strShort.GetLength();
                if(nPos == nIndexLength)
                {
					int nOffSet = 1;
					if (strLine.Find(_T("\n")) != -1)
						nOffSet++;
                    strSVoice = strLine.Mid(nPos+1, strLine.GetLength()-nPos-nOffSet);
                    nCount++;
                    if(nCount == 3)
                        return TRUE;                    
                }
            }
			if (strLine.Find(vInd.strIntersec) == 0)
			{
				int nPos = strLine.Find(L':');
				int nIndexLength = vInd.strIntersec.GetLength();
				if (nPos == nIndexLength)
				{
					int nOffSet = 1;
					if (strLine.Find(_T("\n")) != -1)
						nOffSet++;
					strIVoice = strLine.Mid(nPos+1, strLine.GetLength()-nPos-nOffSet);
					nCount++;
					if (nCount == 3)
						return TRUE;
				}
			}
        }
    }
	return FALSE;
}

//========================================================================
// 根据语音条件得到语音索引
// 
// 修改： 1 增加看板信息的判断 20070809
//========================================================================
BOOL CGIS_NetworkPathInfo::GetMoibleVoiceIndex(
							ST_VOICE_NODE *pVNode,      // [in]
							ST_VOICE_RUT *pVRStart,     // [in]
							ST_VOICE_RUT *pVREnd,       // [in]
							ST_VOICE_INDEX &indVoice)	// [out]
{
	if (0 == pVREnd->bMakeVoice)
		return FALSE;

	indVoice.strLong = _T("");
	indVoice.strShort = _T("");
#ifdef _ANALYSE_TURN_VOICE
    int nMathcIndex = -1;
#endif
	indVoice.strIntersec = _T("");

	// 规则1：出入道路名相同 直行 普通道路 存在混乱路口
	if (pVNode->byAngle == 1 &&
		pVNode->bySameName == 11 &&
		(pVNode->byConfusion2==15 || pVNode->byConfusion2==37 ||
		 pVNode->byConfusion2==38) &&
		pVNode->byOtherStreet == 21 &&
		pVRStart->byType == 8 &&
		pVRStart->byAdd == 10 &&
		pVRStart->byDrivable == 13 && 
		pVREnd->byType == 8 &&
		pVREnd->byAdd == 10 &&
		pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 1;
#endif
		if (pVNode->byConfusion2 == 15)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-1";
				indVoice.strShort = L"1-2-1";
				indVoice.strIntersec = L"1-3-1";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-4";
				indVoice.strShort = L"1-1-4";
				indVoice.strIntersec = L"1-3-4";
			}
		}
		else if(pVNode->byConfusion2 == 37)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-2";
				indVoice.strShort = L"1-2-2";
				indVoice.strIntersec = L"1-3-2";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-5";
				indVoice.strShort = L"1-1-5";
				indVoice.strIntersec = L"1-3-5";
			}
		}
		else if(pVNode->byConfusion2 == 38)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-3";
				indVoice.strShort = L"1-2-3";
				indVoice.strIntersec = L"1-3-3";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-6";
				indVoice.strShort = L"1-1-6";
				indVoice.strIntersec = L"1-3-6";
			}
		}
	}	
	//规则2：出入道路名不相同 直行 存在混乱路口
	else if (
        pVNode->byAngle == 1 && 
        pVNode->bySameName == 12 && 
        pVNode->byOtherStreet == 21 && 
		(pVRStart->byType==6 || pVRStart->byType==7 || pVRStart->byType==8) && 
        pVRStart->byAdd == 10 &&
		pVRStart->byDrivable == 13 &&
        pVREnd->byType == 8 && 
		pVREnd->byAdd == 10 && 
        pVREnd->byDrivable == 13)
	{		
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 2;
#endif
		if(pVNode->byConfusion2 == 15 )
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"2-1-1";
				indVoice.strShort = L"2-2-1";
				indVoice.strIntersec = L"2-3-1";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-4";
				indVoice.strShort = L"1-1-4";
				indVoice.strIntersec = L"1-3-4";
			}
		}
		else if(pVNode->byConfusion2 == 37 )
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"2-1-2";
				indVoice.strShort = L"2-2-2";
				indVoice.strIntersec = L"2-3-2";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-5";
				indVoice.strShort = L"1-1-5";
				indVoice.strIntersec = L"1-3-5";
			}
		}
		else if(pVNode->byConfusion2 == 38 )
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"2-1-3";
				indVoice.strShort = L"2-2-3";
				indVoice.strIntersec = L"2-3-3";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-6";
				indVoice.strShort = L"1-1-6";
				indVoice.strIntersec = L"1-3-6";
			}
		}
	}
	// 规则3：出入口道路名相同 缓拐或拐弯 普通道路或匝道
	else if ((pVNode->byAngle==2 || pVNode->byAngle==3) &&
			 pVNode->bySameName == 11 &&
			 pVRStart->byType == 8 &&
			 pVRStart->byAdd == 10 &&
			 pVRStart->byDrivable == 13 &&
			 pVREnd->byType == 8 && 
			 pVREnd->byAdd==10 &&
			 pVREnd->byDrivable == 13)
	{	
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 3;
#endif
        if (pVNode->byConfusion2 == 15)
		{
			if (pVNode->byTurn == 27)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-1";
					indVoice.strShort = L"3-2-1";
					indVoice.strIntersec = L"3-3-1";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-9";
					indVoice.strShort = L"3-1-9";
					indVoice.strIntersec = L"3-3-9";
				}
			}
			else if (pVNode->byTurn == 28)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-3";
					indVoice.strShort = L"3-2-3";
					indVoice.strIntersec = L"3-3-3";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-11";
					indVoice.strShort = L"3-1-11";
					indVoice.strIntersec = L"3-3-11";
				}
			}
		}
		else if (pVNode->byConfusion2 == 37)
		{
			if (pVNode->byTurn == 27)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-2";
					indVoice.strShort = L"3-2-2";
					indVoice.strIntersec = L"3-3-2";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-10";
					indVoice.strShort = L"3-1-10";
					indVoice.strIntersec = L"3-3-10";					
				}
			}
			else if (pVNode->byTurn == 28)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-4";
					indVoice.strShort = L"3-2-4";
					indVoice.strIntersec = L"3-3-4";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-12";
					indVoice.strShort = L"3-1-12";
					indVoice.strIntersec = L"3-3-12";
				}
			}
		}
		else if( pVNode->byConfusion2 == 38 )
		{
			if (pVNode->byTurn == 27)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-5";
					indVoice.strShort = L"3-2-5";
					indVoice.strIntersec = L"3-3-5";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-13";
					indVoice.strShort = L"3-1-13";
					indVoice.strIntersec = L"3-3-13";
				}
			}
			else if (pVNode->byTurn == 28)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-6";
					indVoice.strShort = L"3-2-6";
					indVoice.strIntersec = L"3-3-6";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-14";
					indVoice.strShort = L"3-1-14";
					indVoice.strIntersec = L"3-3-14";
				}
			}
		}
		else if (pVNode->byConfusion2 == 16)
		{
			if (pVNode->byTurn == 27)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-7";
					indVoice.strShort = L"3-2-7";
					indVoice.strIntersec = L"3-3-7";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-15";
					indVoice.strShort = L"3-1-15";
					indVoice.strIntersec = L"3-3-15";
				}
			}
			else if (pVNode->byTurn == 28)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-8";
					indVoice.strShort = L"3-2-8";
					indVoice.strIntersec = L"3-3-8";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"3-1-16";
					indVoice.strShort = L"3-1-16";
					indVoice.strIntersec = L"3-3-16";
				}
			}
		}
	}
	// 规则4：出入口道路名相同 急拐 普通道路或匝道
	else if (pVNode->byAngle == 4 &&
			 pVNode->bySameName == 11 &&
			 pVNode->byOtherStreet == 21 &&
			 pVNode->byIMI == 24 && 
			 pVRStart->byType == 8 &&
			 pVRStart->byAdd == 10 &&
			 pVRStart->byDrivable == 13 &&
			 pVREnd->byType == 8 && 
			 pVREnd->byAdd == 10 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 4;
#endif	
		if (pVNode->byConfusion2 == 15)
		{
			if (pVNode->byTurn == 27)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-1";
					indVoice.strShort = L"4-2-1";
					indVoice.strIntersec = L"4-3-1";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-9";
					indVoice.strShort = L"4-1-9";
					indVoice.strIntersec = L"4-3-9";
				}
			}
			else if (pVNode->byTurn == 28)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-2";
					indVoice.strShort = L"4-2-2";
					indVoice.strIntersec = L"4-3-2";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-10";
					indVoice.strShort = L"4-1-10";
					indVoice.strIntersec = L"4-3-10";
				}
			}
		}
		else if (pVNode->byConfusion2 == 37)
		{
			if (pVNode->byTurn == 27)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-3";
					indVoice.strShort = L"4-2-3";
					indVoice.strIntersec = L"4-3-3";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-11";
					indVoice.strShort = L"4-1-11";
					indVoice.strIntersec = L"4-3-11";
				}
			}
			else if (pVNode->byTurn == 28)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-4";
					indVoice.strShort = L"4-2-4";
					indVoice.strIntersec = L"4-3-4";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-12";
					indVoice.strShort = L"4-1-12";
					indVoice.strIntersec = L"4-3-12";
				}
			}
		}
		else if( pVNode->byConfusion2 == 38 )
		{
			if (pVNode->byTurn == 27)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-5";
					indVoice.strShort = L"4-2-5";
					indVoice.strIntersec = L"4-3-5";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-13";
					indVoice.strShort = L"4-1-13";
					indVoice.strIntersec = L"4-3-13";
				}
			}
			else if (pVNode->byTurn == 28)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-6";
					indVoice.strShort = L"4-2-6";
					indVoice.strIntersec = L"4-3-6";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-14";
					indVoice.strShort = L"4-1-14";
					indVoice.strIntersec = L"4-3-14";
				}
			}
		}
		else if (pVNode->byConfusion2 == 16)
		{
			if (pVNode->byTurn == 27)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-7";
					indVoice.strShort = L"4-2-7";
					indVoice.strIntersec = L"4-3-7";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-15";
					indVoice.strShort = L"4-1-15";
					indVoice.strIntersec = L"4-3-15";
				}
			}
			else if (pVNode->byTurn == 28)
			{
				if (0 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-8";
					indVoice.strShort = L"4-2-8";
					indVoice.strIntersec = L"4-3-8";
				}
				else if (1 == pVREnd->bHaveSign)
				{
					indVoice.strLong = L"4-1-16";
					indVoice.strShort = L"4-1-16";
					indVoice.strIntersec = L"4-3-16";
				}
			}
		}
	}
	// 规则5：出入口道路不同名 缓拐/拐弯/急拐
	else if ((pVNode->byAngle==2 || pVNode->byAngle==3 || pVNode->byAngle==4)&&
			 pVNode->bySameName == 12 && 
			 (pVRStart->byType==6||pVRStart->byType==7||pVRStart->byType==8)&&
			 pVRStart->byAdd == 10 && 
			 pVRStart->byDrivable == 13 &&
			 (pVREnd->byType==8 || pVREnd->byType==7) &&
			 pVREnd->byAdd == 10 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 5;
#endif
        if (pVNode->byConfusion2 == 16)
		{
			if (pVNode->byAngle==2 || pVNode->byAngle==3)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-1";
						indVoice.strShort = L"5-2-1";
						indVoice.strIntersec = L"5-3-1";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-15";
						indVoice.strShort = L"3-1-15";
						indVoice.strIntersec = L"3-3-15";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-2";
						indVoice.strShort = L"5-2-2";
						indVoice.strIntersec = L"5-3-2";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-16";
						indVoice.strShort = L"3-1-16";
						indVoice.strIntersec = L"3-3-16";
					}
				} 
			}
			else if (pVNode->byAngle == 4)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-3";
						indVoice.strShort = L"5-2-3";
						indVoice.strIntersec = L"5-3-3";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-15";
						indVoice.strShort = L"4-1-15";
						indVoice.strIntersec = L"4-3-15";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-4";
						indVoice.strShort = L"5-2-4";
						indVoice.strIntersec = L"5-3-4";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-16";
						indVoice.strShort = L"4-1-16";
						indVoice.strIntersec = L"4-3-16";
					}
				}
			}
		}
		else if (pVNode->byConfusion2 == 15)
		{
			if (pVNode->byAngle==2 || pVNode->byAngle==3)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-5";
						indVoice.strShort = L"5-2-5";
						indVoice.strIntersec = L"5-3-5";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-9";
						indVoice.strShort = L"3-1-9";
						indVoice.strIntersec = L"3-3-9";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-6";
						indVoice.strShort = L"5-2-6";
						indVoice.strIntersec = L"5-3-6";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-11";
						indVoice.strShort = L"3-1-11";
						indVoice.strIntersec = L"3-3-11";
					}
				} 
			}
			else if (pVNode->byAngle == 4)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-7";
						indVoice.strShort = L"5-2-7";
						indVoice.strIntersec = L"5-3-7";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-9";
						indVoice.strShort = L"4-1-9";
						indVoice.strIntersec = L"4-3-9";						
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-8";
						indVoice.strShort = L"5-2-8";
						indVoice.strIntersec = L"5-3-8";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-10";
						indVoice.strShort = L"4-1-10";
						indVoice.strIntersec = L"4-3-10";
					}
				} 
			}
		}
		else if (pVNode->byConfusion2 == 37)
		{
			if (pVNode->byAngle==2 || pVNode->byAngle==3)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-9";
						indVoice.strShort = L"5-2-9";
						indVoice.strIntersec = L"5-3-9";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-10";
						indVoice.strShort = L"3-1-10";
						indVoice.strIntersec = L"3-3-10";						
					}
				}
				if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-10";
						indVoice.strShort = L"5-2-10";
						indVoice.strIntersec = L"5-3-10";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-12";
						indVoice.strShort = L"3-1-12";
						indVoice.strIntersec = L"3-3-12";						
					}
				} 
			}
			else if (pVNode->byAngle == 4)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-11";
						indVoice.strShort = L"5-2-11";
						indVoice.strIntersec = L"5-3-11";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-11";
						indVoice.strShort = L"4-1-11";
						indVoice.strIntersec = L"4-3-11";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-12";
						indVoice.strShort = L"5-2-12";
						indVoice.strIntersec = L"5-3-12";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-12";
						indVoice.strShort = L"4-1-12";
						indVoice.strIntersec = L"4-3-12";						
					}
				} 
			}
		}
		else if (pVNode->byConfusion2 == 38)
		{
			if (pVNode->byAngle==2 || pVNode->byAngle==3)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-13";
						indVoice.strShort = L"5-2-13";
						indVoice.strIntersec = L"5-3-13";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-13";
						indVoice.strShort = L"3-1-13";
						indVoice.strIntersec = L"3-3-13";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-14";
						indVoice.strShort = L"5-2-14";
						indVoice.strIntersec = L"5-3-14";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-14";
						indVoice.strShort = L"3-1-14";
						indVoice.strIntersec = L"3-3-14";
					}
				} 
			}
			else if (pVNode->byAngle == 4)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-15";
						indVoice.strShort = L"5-2-15";
						indVoice.strIntersec = L"5-3-15";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-13";
						indVoice.strShort = L"4-1-13";
						indVoice.strIntersec = L"4-3-13";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"5-1-16";
						indVoice.strShort = L"5-2-16";
						indVoice.strIntersec = L"5-3-16";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-14";
						indVoice.strShort = L"4-1-14";
						indVoice.strIntersec = L"4-3-14";
					}
				} 
			}
		}
	}
	// 规则6：在同一条道路上掉头
	else if (pVNode->byAngle==5 &&
			 pVNode->bySameName==11 &&
			 pVNode->byIMI==24 &&
			 pVRStart->byType==8 && 
			 pVRStart->byDrivable==13 &&
			 pVREnd->byType==8 &&
			 pVREnd->byDrivable==13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 6;
#endif
		indVoice.strLong = L"6-1-1";
		indVoice.strShort = L"6-2-1";
		indVoice.strIntersec = L"6-3-1";
	}
	// 规则7：U型拐，出入道路同名
	else if (pVNode->byAngle==5 &&
			 pVNode->bySameName==11 &&
			 pVNode->byIMI==23 &&
			 pVRStart->byDrivable==13 &&
			 pVREnd->byDrivable==13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 7;
#endif
		indVoice.strLong = L"7-1-1";
		indVoice.strShort = L"7-2-1";
		indVoice.strIntersec = L"7-3-1";
	}
	// 规则8：驶入匝道
	else if ((pVNode->byAngle==2||pVNode->byAngle==3||pVNode->byAngle==4) &&
			 pVNode->byOtherStreet == 21 && 
			 (pVRStart->byType==6||pVRStart->byType==7||pVRStart->byType==8)&&
			 pVRStart->byDrivable == 13 && 
			 pVREnd->byType == 7 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 8;
#endif
		if (pVNode->byConfusion2 == 15)
		{
			if (pVNode->byAngle==2 || pVNode->byAngle==3)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-1";
						indVoice.strShort = L"8-2-1";
						indVoice.strIntersec = L"8-3-1";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-9";
						indVoice.strShort = L"3-1-9";
						indVoice.strIntersec = L"3-3-9";												
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-2";
						indVoice.strShort = L"8-2-2";
						indVoice.strIntersec = L"8-3-2";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-11";
						indVoice.strShort = L"3-1-11";
						indVoice.strIntersec = L"3-3-11";												
					}
				}
			}
			else if (pVNode->byAngle == 4)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-3";
						indVoice.strShort = L"8-2-3";
						indVoice.strIntersec = L"8-3-3";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-9";
						indVoice.strShort = L"4-1-9";
						indVoice.strIntersec = L"4-3-9";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-4";
						indVoice.strShort = L"8-2-4";
						indVoice.strIntersec = L"8-3-4";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-10";
						indVoice.strShort = L"4-1-10";
						indVoice.strIntersec = L"4-3-10";
					}
				}
			}
		}
		else if (pVNode->byConfusion2 == 37)
		{
			if (pVNode->byAngle==2 || pVNode->byAngle==3)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-5";
						indVoice.strShort = L"8-2-5";
						indVoice.strIntersec = L"8-3-5";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-10";
						indVoice.strShort = L"3-1-10";
						indVoice.strIntersec = L"3-3-10";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-6";
						indVoice.strShort = L"8-2-6";
						indVoice.strIntersec = L"8-3-6";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-12";
						indVoice.strShort = L"3-1-12";
						indVoice.strIntersec = L"3-3-12";
					}
				}
			}
			else if (pVNode->byAngle == 4)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-7";
						indVoice.strShort = L"8-2-7";
						indVoice.strIntersec = L"8-3-7";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-11";
						indVoice.strShort = L"4-1-11";
						indVoice.strIntersec = L"4-3-11";
					}
				}
				if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-8";
						indVoice.strShort = L"8-2-8";
						indVoice.strIntersec = L"8-3-8";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-12";
						indVoice.strShort = L"4-1-12";
						indVoice.strIntersec = L"4-3-12";
					}
				}
			}
		}
		else if (pVNode->byConfusion2 == 38)
		{
			if (pVNode->byAngle==2 || pVNode->byAngle==3)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-9";
						indVoice.strShort = L"8-2-9";
						indVoice.strIntersec = L"8-3-9";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-13";
						indVoice.strShort = L"3-1-13";
						indVoice.strIntersec = L"3-3-13";						
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-10";
						indVoice.strShort = L"8-2-10";
						indVoice.strIntersec = L"8-3-10";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-14";
						indVoice.strShort = L"3-1-14";
						indVoice.strIntersec = L"3-3-14";
					}
				}
			}
			else if (pVNode->byAngle == 4)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-11";
						indVoice.strShort = L"8-2-11";
						indVoice.strIntersec = L"8-3-11";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-13";
						indVoice.strShort = L"4-1-13";
						indVoice.strIntersec = L"4-3-13";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-12";
						indVoice.strShort = L"8-2-12";
						indVoice.strIntersec = L"8-3-12";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-14";
						indVoice.strShort = L"4-1-14";
						indVoice.strIntersec = L"4-3-14";
					}
				}
			}
		}
		else if (pVNode->byConfusion2 == 16)
		{
			if (pVNode->byAngle==2 || pVNode->byAngle==3)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-13";
						indVoice.strShort = L"8-2-13";
						indVoice.strIntersec = L"8-3-13";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-15";
						indVoice.strShort = L"3-1-15";
						indVoice.strIntersec = L"3-3-15";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-14";
						indVoice.strShort = L"8-2-14";
						indVoice.strIntersec = L"8-3-14";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"3-1-16";
						indVoice.strShort = L"3-1-16";
						indVoice.strIntersec = L"3-3-16";
					}
				}
			}
			else if (pVNode->byAngle == 4)
			{
				if (pVNode->byTurn == 27)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-15";
						indVoice.strShort = L"8-2-15";
						indVoice.strIntersec = L"8-3-15";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-15";
						indVoice.strShort = L"4-1-15";
						indVoice.strIntersec = L"4-3-15";
					}
				}
				else if (pVNode->byTurn == 28)
				{
					if (0 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"8-1-16";
						indVoice.strShort = L"8-2-16";
						indVoice.strIntersec = L"8-3-16";
					}
					else if (1 == pVREnd->bHaveSign)
					{
						indVoice.strLong = L"4-1-16";
						indVoice.strShort = L"4-1-16";
						indVoice.strIntersec = L"4-3-16";
					}
				}
			}
		}
	}
	// 规则9：直行 高速
	else if (pVNode->byAngle == 1 &&
			 pVNode->byOtherStreet == 21 &&
			 (pVRStart->byType==6 || pVRStart->byType==7 || pVRStart->byType==8) &&
			 pVNode->byDestination == 26 &&
			 pVRStart->byDrivable == 13 &&
			 (pVREnd->byType==6 || pVREnd->byType==7) &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 9;
#endif

        if (pVNode->byConfusion2 == 15)
		{
			if (0 == pVREnd->bHaveSign)
			{
				if(pVNode->bySameName == 11)
				{
					indVoice.strLong = L"9-1-1";
					indVoice.strShort = L"9-2-1";
					indVoice.strIntersec = L"9-3-1";
				}
				else
				{
					indVoice.strLong = L"9-1-2";
					indVoice.strShort = L"9-2-2";
					indVoice.strIntersec = L"9-3-2";
				}
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-4";
				indVoice.strShort = L"1-1-4";
				indVoice.strIntersec = L"1-3-4";
			}
		}
		else if (pVNode->byConfusion2 == 37)
		{
			if (0 == pVREnd->bHaveSign)
			{
				if(pVNode->bySameName == 11)
				{
					indVoice.strLong = L"9-1-3";
					indVoice.strShort = L"9-2-3";
					indVoice.strIntersec = L"9-3-3";
				}
				else
				{
					indVoice.strLong = L"9-1-4";
					indVoice.strShort = L"9-2-4";
					indVoice.strIntersec = L"9-3-4";
				}
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-5";
				indVoice.strShort = L"1-1-5";
				indVoice.strIntersec = L"1-3-5";
			}
		}
		else if (pVNode->byConfusion2 == 38)
		{
			if (0 == pVREnd->bHaveSign)
			{
				if(pVNode->bySameName == 11)
				{
					indVoice.strLong = L"9-1-5";
					indVoice.strShort = L"9-2-5";
					indVoice.strIntersec = L"9-3-5";
				}
				else
				{
					indVoice.strLong = L"9-1-6";
					indVoice.strShort = L"9-2-6";
					indVoice.strIntersec = L"9-3-6";
				}
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-6";
				indVoice.strShort = L"1-1-6";
				indVoice.strIntersec = L"1-3-6";
			}
		}
	}
	// 规则10：出入道路名相同 环拐/拐弯 高速
	else if ((pVNode->byAngle==2 || pVNode->byAngle==3) &&
			 pVNode->bySameName == 11 && 
			 pVNode->byConfusion2 == 16 &&
			 pVNode->byOtherStreet == 21 &&
			 pVRStart->byType==6 && 
			 pVRStart->byDrivable == 13 &&
			 pVREnd->byType == 6 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 10;
#endif

        if (pVNode->byTurn == 27)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"3-1-7";
				indVoice.strShort = L"3-2-7";
				indVoice.strIntersec = L"3-3-7";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"3-1-15";
				indVoice.strShort = L"3-1-15";
				indVoice.strIntersec = L"3-3-15";
			}
		}
		else if (pVNode->byTurn == 28)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"3-1-8";
				indVoice.strShort = L"3-2-8";
				indVoice.strIntersec = L"3-3-8";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"3-1-16";
				indVoice.strShort = L"3-1-16";
				indVoice.strIntersec = L"3-3-16";
			}
		}
	}
	// 规则11：出入道路名相同 直行 高速
	else if (pVNode->byAngle == 1 &&
			 pVNode->bySameName == 11 &&
			 (pVNode->byConfusion2 == 15 || pVNode->byConfusion2 == 37 ||
			  pVNode->byConfusion2 == 38) && 
			 pVRStart->byType == 6 &&
			 pVRStart->byDrivable == 13 &&
			 pVNode->byEnName == 20 &&
			 pVREnd->byType == 6 && 
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 11;
#endif
        if (pVNode->byConfusion2 == 15)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"11-1-1";
				indVoice.strShort = L"11-2-1";
				indVoice.strIntersec = L"11-3-1";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-4";
				indVoice.strShort = L"1-1-4";
				indVoice.strIntersec = L"1-3-4";
			}
		}
		else if(pVNode->byConfusion2 == 37)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"11-1-2";
				indVoice.strShort = L"11-2-2";
				indVoice.strIntersec = L"11-3-2";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-5";
				indVoice.strShort = L"1-1-5";
				indVoice.strIntersec = L"1-3-5";
			}
		}
		else if(pVNode->byConfusion2 == 38)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"11-1-3";
				indVoice.strShort = L"11-2-3";
				indVoice.strIntersec = L"11-3-3";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"1-1-6";
				indVoice.strShort = L"1-1-6";
				indVoice.strIntersec = L"1-3-6";
			}
		}
	}
	// 规则12：出入道路不同名 环拐/拐弯 高速
	else if ((pVNode->byAngle==2 || pVNode->byAngle==3) &&
			 pVNode->bySameName == 12 && 
			 pVNode->byOtherStreet == 21 &&
			 pVRStart->byType == 6 &&
			 pVRStart->byDrivable == 13 && 
			 pVREnd->byType == 6 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 12;
#endif
		if (pVNode->byTurn == 27)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"5-1-1";
				indVoice.strShort = L"5-2-1";
				indVoice.strIntersec = L"5-3-1";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"3-1-15";
				indVoice.strShort = L"3-1-15";
				indVoice.strIntersec = L"3-3-15";
			}
		}
		else if (pVNode->byTurn == 28)
		{
			if (0 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"5-1-2";
				indVoice.strShort = L"5-2-2";
				indVoice.strIntersec = L"5-3-2";
			}
			else if (1 == pVREnd->bHaveSign)
			{
				indVoice.strLong = L"3-1-16";
				indVoice.strShort = L"3-1-16";
				indVoice.strIntersec = L"3-3-16";
			}
		} 
	}
	// 规则13：驶入收费路段（规则不用）
	// 规则14：驶出收费路段（规则不用）
	// 规则15：目的地和起点在环岛上
	else if (pVNode->byDestination == 25 &&
			 pVRStart->byAdd == 9 &&
			 pVRStart->byDrivable == 13 && 
			 pVREnd->byAdd == 9 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 13;
#endif
		indVoice.strLong = L"15-1-1"; 
		indVoice.strShort = L"15-2-1";
		indVoice.strIntersec = L"15-3-1";
	}
	// 规则16：起点在环岛上
	else if (pVRStart->byAdd == 9 &&
			 pVRStart->byDrivable == 13 &&
			 pVREnd->byAdd == 10 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 16;
#endif
		indVoice.strLong = L"16-1-1"; 
		indVoice.strShort = L"16-2-1";
		indVoice.strIntersec = L"16-3-1";
	}
	// 规则17：目的地在环岛上
	else if (pVNode->byDestination == 25 &&
			 pVRStart->byAdd == 10 &&
			 pVREnd->byAdd==9 &&
			 (pVRStart->byDrivable==13 || pVRStart->byDrivable==14) &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 17;
#endif
		indVoice.strLong = L"17-1-1"; 
		indVoice.strShort = L"17-2-1";
		indVoice.strIntersec = L"17-3-1";
	}
	// 规则18：路段之间有环岛
	else if (pVNode->byRound == 29 &&
			 pVRStart->byAdd == 10 &&
			 (pVRStart->byDrivable==13 || pVRStart->byDrivable==14) && 
			 pVREnd->byAdd == 10 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 18;
#endif
		indVoice.strLong = L"18-1-1"; 
		indVoice.strShort = L"18-2-1";
		indVoice.strIntersec = L"18-3-1";
	}
	// 规则19：驶入隧道
	else if ((pVRStart->byType==6 || pVRStart->byType==7 || pVRStart->byType==8) &&  
              pVRStart->byDrivable==13 &&                 
		      pVREnd->byType == 33 &&
              pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 19;
#endif
		if (pVNode->byAngle == 1)
		{
			indVoice.strLong = L"19-1-5";
			indVoice.strShort = L"19-2-5";
			indVoice.strIntersec = L"19-3-5";
		}
		else if (pVNode->byAngle==2 || pVNode->byAngle==3)
		{
			if (pVNode->byTurn == 27)
			{
				indVoice.strLong = L"19-1-1";
				indVoice.strShort = L"19-2-1";
				indVoice.strIntersec = L"19-3-1";
			}
			if (pVNode->byTurn == 28)
			{
				indVoice.strLong = L"19-1-2";
				indVoice.strShort = L"19-2-2";
				indVoice.strIntersec = L"19-3-2";
			}
		}
		else if (pVNode->byAngle == 4)
		{
			if (pVNode->byTurn == 27)
			{
				indVoice.strLong = L"19-1-3";
				indVoice.strShort = L"19-2-3";
				indVoice.strIntersec = L"19-3-3";
			}
			if (pVNode->byTurn == 28)
			{
				indVoice.strLong = L"19-1-4";
				indVoice.strShort = L"19-2-4";
				indVoice.strIntersec = L"19-3-4";
			}
		}
	}
	// 规则20：驶出隧道
	else if (pVRStart->byType == 33 &&
			 pVRStart->byDrivable == 13 &&
			 pVREnd->byDrivable == 13 && 
			 (pVREnd->byType == 6 || pVREnd->byType == 7 || pVREnd->byType == 8))
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 20;
#endif
		if (pVNode->byAngle == 1)
		{
			indVoice.strLong = L"20-1-5";
			indVoice.strShort = L"20-2-5";
			indVoice.strIntersec = L"20-3-5";
		}
		else if (pVNode->byAngle==2 || pVNode->byAngle==3)
		{
			if (pVNode->byTurn == 27)
			{
				indVoice.strLong = L"20-1-1";
				indVoice.strShort = L"20-2-1";
				indVoice.strIntersec = L"20-3-1";
			}
			if (pVNode->byTurn == 28)
			{
				indVoice.strLong = L"20-1-2";
				indVoice.strShort = L"20-2-2";
				indVoice.strIntersec = L"20-3-2";
			} 
		}
		else if (pVNode->byAngle == 4)
		{
			if (pVNode->byTurn == 27)
			{
				indVoice.strLong = L"20-1-3";
				indVoice.strShort = L"20-2-3";
				indVoice.strIntersec = L"20-3-3";
			}
			if (pVNode->byTurn == 28)
			{
				indVoice.strLong = L"20-1-4";
				indVoice.strShort = L"20-2-4";
				indVoice.strIntersec = L"20-3-4";
			}
		}
	}
	// 规则21：驶入渡口
	else if ((pVRStart->byType==6 || pVRStart->byType==7 || pVRStart->byType==8) && 
			 pVRStart->byDrivable == 13 &&
			 pVREnd->byType == 34 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 21;
#endif
		if (pVNode->byAngle == 1)
		{
			indVoice.strLong = L"21-1-5";
			indVoice.strShort = L"21-2-5";
			indVoice.strIntersec = L"21-3-5";
		}
		else if (pVNode->byAngle==2 || pVNode->byAngle==3)
		{
			if (pVNode->byTurn == 27)
			{
				indVoice.strLong = L"21-1-1";
				indVoice.strShort = L"21-2-1";
				indVoice.strIntersec = L"21-3-1";
			}
			if (pVNode->byTurn == 28)
			{
				indVoice.strLong = L"21-1-2";
				indVoice.strShort = L"21-2-2";
				indVoice.strIntersec = L"21-3-2";
			} 
		}
		else if (pVNode->byAngle == 4)
		{
			if (pVNode->byTurn == 27)
			{
				indVoice.strLong = L"21-1-3";
				indVoice.strShort = L"21-2-3";
				indVoice.strIntersec = L"21-3-3";
			}
			if (pVNode->byTurn == 28)
			{
				indVoice.strLong = L"21-1-4";
				indVoice.strShort = L"21-2-4";
				indVoice.strIntersec = L"21-3-4";
			} 
		}
	}
	// 规则22：驶出渡口
	else if (pVRStart->byType == 34 &&
			 pVRStart->byDrivable == 13 &&
			 pVREnd->byDrivable==13 && 
			 (pVREnd->byType==6 || pVREnd->byType==7 || pVREnd->byType==8))
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 22;
#endif
		if (pVNode->byAngle == 1)
		{
			indVoice.strLong = L"22-1-5";
			indVoice.strShort = L"22-2-5";
			indVoice.strIntersec = L"22-3-5";
		}
		else if (pVNode->byAngle==2 || pVNode->byAngle==3)
		{
			if (pVNode->byTurn == 27)
			{
				indVoice.strLong = L"22-1-1";
				indVoice.strShort = L"22-2-1";
				indVoice.strIntersec = L"22-3-1";
			}
			if (pVNode->byTurn == 28)
			{
				indVoice.strLong = L"22-1-2";
				indVoice.strShort = L"22-2-2";
				indVoice.strIntersec = L"22-3-2";
			}
		}
		else if (pVNode->byAngle == 4)
		{
			if (pVNode->byTurn == 27)
			{
				indVoice.strLong = L"22-1-3";
				indVoice.strShort = L"22-2-3";
				indVoice.strIntersec = L"22-3-3";
			}
			if (pVNode->byTurn == 28)
			{
				indVoice.strLong = L"22-1-4";
				indVoice.strShort = L"22-2-4";
				indVoice.strIntersec = L"22-3-4";
			}
		}
	}
	// 规则23：U型拐
	else if (pVNode->byAngle == 5 &&
			 pVNode->byIMI == 23 &&
			 pVNode->bySameName == 12 &&
			 pVRStart->byDrivable == 13 &&
			 pVREnd->byDrivable == 13)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 23;
#endif
        indVoice.strLong = L"7-1-1";
		indVoice.strShort = L"7-2-1";
		indVoice.strIntersec = L"7-3-1";
	}
	// 规则24：到达目的地
	else if(pVNode->byDestination == 25)
	{
#ifdef _ANALYSE_TURN_VOICE
        nMathcIndex = 24;
#endif
		if(pVNode->byTurn == 27)
		{
			indVoice.strLong = L"24-1-1";
			indVoice.strShort = L"24-2-1";
			indVoice.strIntersec = L"24-3-1";
		}
		else
		{
			indVoice.strLong = L"24-1-2";
			indVoice.strShort = L"24-2-2";
			indVoice.strIntersec = L"24-3-2";
		}
	}

	//////////////////////////////////////////////////////////////////////////
#ifdef _ANALYSE_TURN_VOICE
    if(nMathcIndex != -1)
    {
        TRACE1("匹配到语音规则%d\n",nMathcIndex);
    }
#endif
#ifdef _ANALYSE_TURN_VOICE
	if( indVoice.strLong.IsEmpty() || indVoice.strShort.IsEmpty() )
    {
        TRACE0("----------------\n");
        TRACE0("语音规则匹配失败:\n");
        DUMPSTATE(pVRStart);
        DUMPSTATE(pVNode);
        DUMPSTATE(pVREnd);
        TRACE0("----------------\n");
    }
    else
    {
        TRACE0("----------------\n");
        TRACE0("语音规则匹配成功:\n");
        DUMPSTATE(pVRStart);
        DUMPSTATE(pVNode);
        DUMPSTATE(pVREnd);
        TRACE0("----------------\n");
    }
#endif
    //////////////////////////////////////////////////////////////////////////

    
    if( indVoice.strLong.IsEmpty() || indVoice.strShort.IsEmpty() ||
		indVoice.strIntersec.IsEmpty())
	{
        return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//lijun 20070718添加
//设置路口扩大图的裁剪矩形
void CGIS_NetworkPathInfo::SetInterSectionClipRectangle(float fExtendX,float fExtendY)
{
    m_fInterSectionClipRectangleExtendX = fExtendX;
    m_fInterSectionClipRectangleExtendY = fExtendY;

}
void CGIS_NetworkPathInfo::GetInterSectionClipRectangle(float& fExtendX,float& fExtendY)
{
    fExtendX = m_fInterSectionClipRectangleExtendX;
    fExtendY = m_fInterSectionClipRectangleExtendY;
}

//////////////////////////////////////////////////////////////////////////
void CGIS_NetworkPathInfo::SetStartCalculateTime(SYSTEMTIME *plocTime)
{
	ASSERT(plocTime != NULL);
	m_nStartCalculateTime = plocTime->wHour * 6 + plocTime->wMinute / 10;
}
BYTE CGIS_NetworkPathInfo::GetStartCalculateTime()
{
    return m_nStartCalculateTime;
}
