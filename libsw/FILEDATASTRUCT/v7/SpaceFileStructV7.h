/**
 * @file   SpaceFileStruceV7.h
 * @brief ���ṹ���ڶ�����������·���еĸ��ֽṹ
 *
 * @purpose 
 * 
 *
 * @author  
 * @date    2006/10/24
 */
#ifndef _SPACEFILESTRUCT_V7_H_
#define _SPACEFILESTRUCT_V7_H_

	
struct StuSpaceFileStructV7
{	
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
	
	
	struct StuIndexInfoOfBreadth
	{
		unsigned long nID;
		unsigned long nOffset;
		unsigned long nBeforeCompressSize;
		unsigned long nAfterCompressSize;


        void DumpState()
        {
            TRACE1("Breadth ID:%d\n",nID);
        };
        void AssertValid()
        {
            ASSERT( nID < 999999);
            //ASSERT( nOffset > 0);
            ASSERT( nOffset < 1024*1024*512);
            ASSERT( nBeforeCompressSize < 1024*1024);
            ASSERT( nAfterCompressSize < 1024*1024);
        };
		
	};
		
	struct StuBreadthHeaderInfo
	{
		unsigned long nFeatureCount;
		unsigned long nCharDataBuddyLength;
		unsigned long nShortDataBuddyLength;
		unsigned long nDwordDataBuddyLength;
        
        void DumpState()
        {
            TRACE1("nFeatureCount           :%d\n",nFeatureCount);
            TRACE1("nCharDataBuddyLength    :%d\n",nCharDataBuddyLength);
            TRACE1("nShortDataBuddyLength   :%d\n",nShortDataBuddyLength);
            TRACE1("nDwordDataBuddyLength   :%d\n",nDwordDataBuddyLength);
        };	
        void AssertValid()
        {
            ASSERT( nFeatureCount < 10000);
            ASSERT( nCharDataBuddyLength    < 1024*1024);
            ASSERT( nShortDataBuddyLength   < 1024*1024);
            ASSERT( nDwordDataBuddyLength   < 1024*1024);
        };
	};
		
	struct StuGisFeatureRoadSection
	{
		unsigned long nID;
		unsigned short nPointCount;
		unsigned char nDirection;
		unsigned char nAnnoNum;
	};

	struct StuGisFeatureRoad
	{
		unsigned long nID;
		unsigned short nPointCount;
        unsigned short nSectionCount;
		unsigned short nAnnotateBlockSize;
		unsigned char nDirection;
	};
};

#endif //_SPACEFILESTRUCT_V7_H_
