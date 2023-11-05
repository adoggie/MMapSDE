// RichPOIView.cpp: implementation of the CRichPOIView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sdelib.h"
#include "RichPOIView.h"
#include "GeoGlobe.h"
#include "./Zlib/zlib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static HANDLE gRichPOIViewFile = INVALID_HANDLE_VALUE;
CRichPOIView::CRichPOIView()
{
	m_pListBandPOI = new CPtrList;
	m_pSpaceIndex = new CPtrList;
}

CRichPOIView::~CRichPOIView()
{
	if( m_pListBandPOI )
	{
		ST_RICHPOI_BANDSET* pRichPOIBand = NULL ;
		RICHPOI_INFO* pRichPOIInfo = NULL;
		POSITION pos = m_pListBandPOI->GetHeadPosition();
		while( pos )
		{
			pRichPOIBand = (ST_RICHPOI_BANDSET*)(m_pListBandPOI->GetNext( pos ));
			if( pRichPOIBand )
			{
				POSITION pos1 = pRichPOIBand->listRichPOI.GetHeadPosition();
				while( pos1 )
				{
					pRichPOIInfo = (RICHPOI_INFO*)( pRichPOIBand->listRichPOI.GetNext( pos1 ) );
					if( pRichPOIInfo )
					{
						delete pRichPOIInfo;
						pRichPOIInfo = NULL;
					}
				}
				pRichPOIBand->listRichPOI.RemoveAll();
				delete pRichPOIBand;
				pRichPOIBand = NULL;
			}
		}
		delete m_pListBandPOI;
		m_pListBandPOI = NULL;
	}
	if( m_pSpaceIndex )
	{
		ST_SPACE_RICHPOI* pSpace = NULL;
		POSITION pos = m_pSpaceIndex->GetHeadPosition();
		while( pos )
		{
			pSpace = (ST_SPACE_RICHPOI*)m_pSpaceIndex->GetNext( pos );
			if( pSpace )
			{
				delete pSpace;
				pSpace = NULL;
			}
		}
		m_pSpaceIndex->RemoveAll();
		delete m_pSpaceIndex;
		m_pSpaceIndex = NULL;
	}
}

CPtrList* CRichPOIView::GetRichPOISet( int nBandID[500] , int nBandNum )
{
	if( !InitRichPOIView() )
		return NULL;

	POSITION pos = m_pListBandPOI->GetHeadPosition();
	ST_RICHPOI_BANDSET* pBandPOI = NULL;
	RICHPOI_INFO* pRichPOI = NULL;
	while( pos )
	{
		POSITION posOld = pos;
		pBandPOI = (ST_RICHPOI_BANDSET*)m_pListBandPOI->GetNext( pos );
		if( pBandPOI )
		{
			BOOL bFind = FALSE;
			for( int i = 0 ; i<nBandNum ; i++ )
			{
				if( pBandPOI->nBandID == nBandID[i] )
				{
					bFind = TRUE;
					break;
				}
			}
			//卸载不需要的图幅数据
			if( !bFind )
			{
				POSITION posPOI = pBandPOI->listRichPOI.GetHeadPosition();
				while( posPOI )
				{
					pRichPOI = ( RICHPOI_INFO* )(pBandPOI->listRichPOI.GetNext( posPOI ));
					if( pRichPOI )
					{
						delete pRichPOI;
						pRichPOI = NULL;
					}
				}
				delete pBandPOI;
				pBandPOI = NULL;
				m_pListBandPOI->RemoveAt( posOld );
			}
		}
	}
	
	//加载新的数据
	for( int i = 0 ; i<nBandNum ; i++ )
	{
		POSITION pos = m_pListBandPOI->GetHeadPosition();
		BOOL bFind = FALSE;
		while( pos )
		{
			pBandPOI = (ST_RICHPOI_BANDSET*)m_pListBandPOI->GetNext( pos );
			if( pBandPOI->nBandID == nBandID[i] )
			{
				bFind = TRUE;
				break;
			}
			
		}
		if( !bFind )
		{
			//进行加载
			LoadRichPOIByBandID( nBandID[i]);
		}
	}
	
	return m_pListBandPOI;
}
BOOL CRichPOIView::InitRichPOIView()
{
	if( m_pSpaceIndex->GetCount() > 0 )
		return TRUE;
	if( gRichPOIViewFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sautomap.chr", m_strFilePath );
		gRichPOIViewFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gRichPOIViewFile)
			return FALSE;
	}
	DWORD dwSpaceIdxPos;
	int  nLevel1Cnt = 0;
	TRY
	{
		char szInfo[60];
		int nBandCount;
		//获得空间索引的位置
		CFile file( gRichPOIViewFile );
		SetFilePointer(gRichPOIViewFile,sizeof(RICHPOI_FILEHEAD),NULL,FILE_BEGIN);
		file.Read(&dwSpaceIdxPos,sizeof(DWORD));

		//跳过最大包围矩形，获取图幅数量
		SetFilePointer(gRichPOIViewFile,dwSpaceIdxPos+16,NULL,FILE_BEGIN);
		file.Read(&nBandCount,sizeof(DWORD));

		
		for (int BandNum=0; BandNum < nBandCount; BandNum++)
		{
			ST_SPACE_RICHPOI* pSpaceInfo = new ST_SPACE_RICHPOI;

			file.Read(&pSpaceInfo->nBandID,sizeof(DWORD));
			file.Seek( 16 , FILE_CURRENT );
			file.Read(&pSpaceInfo->nOffset , sizeof(DWORD));
			file.Read(&pSpaceInfo->nLevel3Num, sizeof(DWORD));

			m_pSpaceIndex->AddTail( pSpaceInfo );
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
	return TRUE;
}
void CRichPOIView::SetRichPOIFilePath( swString strFilePath )
{
	m_strFilePath = strFilePath;
}
BOOL CRichPOIView::LoadRichPOIByBandID( int nBandID )
{
	if( !InitRichPOIView() )
		return FALSE;
	if( gRichPOIViewFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sautomap.chr", m_strFilePath );
		gRichPOIViewFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gRichPOIViewFile)
			return FALSE;
	}
	TRY
	{
		POSITION pos = m_pSpaceIndex->GetHeadPosition();
		ST_SPACE_RICHPOI* pSpaceRichPOI = NULL;
		while( pos )
		{
			pSpaceRichPOI = (ST_SPACE_RICHPOI*)m_pSpaceIndex->GetNext( pos );
			if( pSpaceRichPOI->nBandID == nBandID )
			{
				ST_RICHPOI_BANDSET* pRichPOIBand = new ST_RICHPOI_BANDSET;
				pRichPOIBand->nBandID = nBandID;
				m_pListBandPOI->AddTail( pRichPOIBand );
				//////////////////////////////////////////////////////////////////////////
				char szInfo[60];
				int nLevel3Count;
				//获得空间索引的位置
				CFile file( gRichPOIViewFile );
				SetFilePointer(gRichPOIViewFile,pSpaceRichPOI->nOffset ,NULL,FILE_BEGIN);
//				file.Read(&nLevel3Count,sizeof(DWORD));
				BYTE *pbCAddr_Name = NULL;
				BYTE *pbUCAddr_Name = NULL;
				BYTE *pbCAddr_Data = NULL;
				BYTE *pbUCAddr_Data = NULL;
				DWORD dwRichPoiCnt,dwRichPoiNameOffset,dwRichPoiNameLenUZip,dwRichPoiDataOffset
					,dwRichPoiDataLen,dwRichPoiDataLenUZip,dwRichPoiNameLen;
				for( int i = 0 ; i<pSpaceRichPOI->nLevel3Num; i++ )
				{
					SetFilePointer(gRichPOIViewFile,(pSpaceRichPOI->nOffset+64*i)+36,
						NULL,FILE_BEGIN);

					file.Read(&dwRichPoiCnt,sizeof(DWORD));
					file.Read(&dwRichPoiNameOffset,sizeof(DWORD));
					file.Read(&dwRichPoiNameLen, sizeof(DWORD));
					file.Read(&dwRichPoiNameLenUZip, sizeof(DWORD));
					file.Read(&dwRichPoiDataOffset, sizeof(DWORD));
					file.Read(&dwRichPoiDataLen, sizeof(DWORD));
					file.Read(&dwRichPoiDataLenUZip, sizeof(DWORD));

					int iNameLen = dwRichPoiNameLen;
					int iUZipNameLen = dwRichPoiNameLenUZip;
					int iDataLen = dwRichPoiDataLen;
					int iUZipDataLen = dwRichPoiDataLenUZip;
					pbCAddr_Name = new BYTE[iNameLen];
					pbUCAddr_Name = new BYTE[iUZipNameLen];
					pbCAddr_Data = new BYTE[iDataLen];
					pbUCAddr_Data = new BYTE[iUZipDataLen];
					memset(pbCAddr_Name,0,iNameLen);
					memset(pbUCAddr_Name,0,iUZipNameLen);
					memset(pbCAddr_Data,0,iDataLen);
					memset(pbUCAddr_Data,0,iUZipDataLen);

					SetFilePointer(gRichPOIViewFile,dwRichPoiNameOffset,NULL,FILE_BEGIN);
					file.Read(pbCAddr_Name, iNameLen);
					if (Z_OK != uncompress(pbUCAddr_Name, (unsigned long*)&iUZipNameLen, pbCAddr_Name, iNameLen))
					{
						delete []pbCAddr_Data;
						pbCAddr_Data = NULL;
						delete []pbUCAddr_Data;
						pbUCAddr_Data = NULL;
						delete []pbCAddr_Name;
						pbCAddr_Name = NULL;
						delete []pbUCAddr_Name;
						pbUCAddr_Name = NULL;
						return FALSE;
					}

					SetFilePointer(gRichPOIViewFile,dwRichPoiDataOffset,NULL,FILE_BEGIN);
					file.Read(pbCAddr_Data, iDataLen);
					if (Z_OK != uncompress(pbUCAddr_Data, (unsigned long*)&iUZipDataLen, pbCAddr_Data, iDataLen))
					{
						delete []pbCAddr_Data;
						pbCAddr_Data = NULL;
						delete []pbUCAddr_Data;
						pbUCAddr_Data = NULL;
						delete []pbCAddr_Name;
						pbCAddr_Name = NULL;
						delete []pbUCAddr_Name;
						pbUCAddr_Name = NULL;
						return FALSE;
					}

					BYTE* pPos_name = pbUCAddr_Name;
					BYTE* pPos_data = pbUCAddr_Data;

					for(int j=0;j<(int)dwRichPoiCnt;j++)
					{
						RICHPOI_INFO* info = new RICHPOI_INFO;
						//name
						int nameLen  = *pPos_name;
						pPos_name+=sizeof(BYTE);
						if( nameLen > 0 )
						{
							memcpy(szInfo,pPos_name,nameLen*sizeof(char));
							szInfo[nameLen] = '\0';
							CharToUnicode( szInfo , &info->strRichPoiName );
							pPos_name+=nameLen*sizeof(char);
						}
						else
							info->strRichPoiName.Empty();
						//data
						memcpy(&info->dwRichPOIID,pPos_data,sizeof(DWORD));
						pPos_data+=sizeof(DWORD);
						memcpy(&info->fLon,pPos_data,sizeof(float));
						pPos_data+=sizeof(float);
						memcpy(&info->fLat,pPos_data,sizeof(float));
						pPos_data+=sizeof(float);
						memcpy(&info->dwContentInfoOffset,pPos_data,sizeof(DWORD));
						pPos_data+=sizeof(DWORD);
						memcpy(&info->dwContentTxtLen,pPos_data,sizeof(DWORD));
						pPos_data+=sizeof(DWORD);
						memcpy(&info->dwContentTxtLenUZip,pPos_data,sizeof(DWORD));
						pPos_data+=sizeof(DWORD);
						memcpy(&info->iContentMediaCnt,pPos_data,sizeof(int));
						pPos_data+=sizeof(int);
						memcpy(&info->dwAlarmSpeed,pPos_data,sizeof(BYTE));
						pPos_data+=sizeof(BYTE);
						memcpy(&nameLen,pPos_data,sizeof(BYTE));
						pPos_data+=sizeof(BYTE);
						if( nameLen >0 )
						{
							memcpy(&szInfo,pPos_data,nameLen*sizeof(char));
							szInfo[nameLen] = '\0';
							CharToUnicode( szInfo , &(info->strPhoneCode) );
							pPos_data+=nameLen*sizeof(char);
						}
						else
							info->strPhoneCode.Empty();

						pRichPOIBand->listRichPOI.AddTail( info );
					}

					delete []pbCAddr_Data;
					pbCAddr_Data = NULL;
					delete []pbUCAddr_Data;
					pbUCAddr_Data = NULL;
					delete []pbCAddr_Name;
					pbCAddr_Name = NULL;
					delete []pbUCAddr_Name;
					pbUCAddr_Name = NULL;
}					



				//////////////////////////////////////////////////////////////////////////
				
				
			}
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
	return TRUE;
}
