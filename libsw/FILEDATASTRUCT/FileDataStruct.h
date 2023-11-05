/**
 * @file   F:\WorkStation-3.5\WorkStation-Assistant\BanConvertTools\FileDataStruct.h
 * @brief ��ͼ�����ļ������ͺͽṹ
 *
 * @purpose 
 * 
 *
 * @author  
 * @date    2006/10/24
 */
#ifndef _FILEDATASTRUCT_H_
#define _FILEDATASTRUCT_H_

	
struct StuFileData
{	
//	typedef enum tagEnGeoType
//	{
//		EN_TYPE_NONE,					
//			EN_GOTYPE_FIRST,
//			
//			EN_GOTYPE_POINT = EN_GOTYPE_FIRST,	//��ͼ�����
//			EN_GOTYPE_LINE,						//��ͼ�ߵ���
//			EN_GOTYPE_POLY,						//��ͼ����ε���
//			EN_GOTYPE_RUT,						//��ͼ��·����
//			
//			EN_GOTYPE_LAST,
//			EN_GOTYPE_COUNT = EN_GOTYPE_LAST - EN_GOTYPE_FIRST,
//	}EnGeoObjType;
	
	///��������
	typedef enum tagENGEOOBJECTTYPE{
		EN_GOTYPE_NONE,			//��ͼ�յ���
		EN_GOTYPE_FIRST,
		
		EN_GOTYPE_POINT = EN_GOTYPE_FIRST,	//��ͼ�����
		EN_GOTYPE_UPOINT,		//�û������
		EN_GOTYPE_LINE,			//��ͼ�ߵ���
		EN_GOTYPE_ULINE,		//�û��ߵ���
		EN_GOTYPE_POLY,			//��ͼ�����
		EN_GOTYPE_UPOLY,		//�û������
		EN_GOTYPE_RUT,			//��ͼ��·����
		EN_GOTYPE_URUT,			//�û���·����
		EN_GOTYPE_ROUTE,		//���ߵ�������
		EN_GOTYPE_TRACK,		//������������
		EN_GOTYPE_ADDRESS,		//��ַ������
		
		EN_GOTYPE_LAST,
		EN_GOTYPE_COUNT = EN_GOTYPE_LAST - EN_GOTYPE_FIRST
	}EnGeoObjType;
	

	typedef struct
	{
		char chSign[12];
		unsigned long nMapCount;
	}StuHeaderInfo;
	
	typedef struct
	{
		unsigned long nID;
		unsigned long nOffset;
		unsigned long nBreadthCount;
	}StuIndexInfoOfMap;
	
	
	typedef struct
	{
		unsigned long nID;
		unsigned long nOffset;
		unsigned char nLayerCount;
		unsigned char unuse0;
		unsigned char unuse1;
		unsigned char unuse2;
		
	}StuIndexInfoOfBreadth;
	
	typedef struct
	{
		unsigned short nID;
		unsigned char nType;
		unsigned char nIconID;
		
		unsigned long nOffset;
		unsigned long nBeforeCompressSize;
		unsigned long nAfterCompressSize;
	}StuIndexInfoOfLayerInBreadthData;
	
	typedef struct
	{
		unsigned long nFeatureCount;
		unsigned long nCharDataBuddyOffset;
		unsigned long nCharDataBuddyLength;
		unsigned long nShortDataBuddyOffset;
		unsigned long nShortDataBuddyLength;
		unsigned long nDwordDataBuddyOffset;
		unsigned long nDwordDataBuddyLength;
	}StuHeaderInfoOfLayerInBreadthData;
	
	typedef struct
	{
		unsigned char nType;
		unsigned char nAnnoNum;
		unsigned char nAddressNum;
		unsigned char nPhoneNum;
		
		float fLog;
		float fLat;		
//		char* pAnno;
//		char* pAddress;
//		char* pPhone;
		

		
//		void SetBaseAddr(char* pBaseAddr)
//		{
//			if(pAnno)
//				pAnno			= (char*)(	((DWORD)pAnno)		+ pBaseAddr);
//			if(pAddress)
//				pAddress		= (char*)(	((DWORD)pAddress)	+ pBaseAddr);
//			if(pPhone)
//				pPhone			= (char*)(	((DWORD)pPhone)		+ pBaseAddr);
//		}

	}StuGisFeaturePoint;
	
	typedef struct
	{
		unsigned long nPointCount;
//		unsigned long nPartCount;
		unsigned short nID;
		unsigned char nDirection;
		unsigned char nAnnoNum;
		
//		char* pAnno;
//		float* pLoction;
//		int* pPart;
		
//		void SetBaseAddr(char* pBaseAddr)
//		{
//			if(pAnno)
//				pAnno			= (char*)(	((DWORD)pAnno)		+ pBaseAddr);
//			if(pLoction)
//				pLoction		= (float*)(	((DWORD)pLoction)	+ pBaseAddr);
//			if(pPart)
//				pPart			= (int*)(	((DWORD)pPart)		+ pBaseAddr);
//		}
	}StuGisFeatureRoadSection;
	
	typedef struct
	{
		unsigned long nPointCount;
		unsigned long nPartCount;
		
		unsigned char nAnnoNum;
		unsigned char unuse0;
		unsigned char unuse1;
		unsigned char unuse2;
		
//		char* pAnno;
//		float* pLoction;
//		int* pPart;
		
//		void SetBaseAddr(char* pBaseAddr)
//		{
//			if(pAnno)
//				pAnno			= (char*)(	((DWORD)pAnno)		+ pBaseAddr);
//			if(pLoction)
//				pLoction		= (float*)(	((DWORD)pLoction)	+ pBaseAddr);
//			if(pPart)
//				pPart			= (int*)(	((DWORD)pPart)		+ pBaseAddr);
//		}
	}StuGisFeatureLine,StuGisFeaturePoly;


};

#endif //_FILEDATASTRUCT_H_