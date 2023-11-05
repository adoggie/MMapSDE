/**
 * @file   F:\WorkStation-3.5\WorkStation-Assistant\BanConvertTools\FileDataStruct.h
 * @brief ��ͼ�����ļ������ͺͽṹ
 *
 * @purpose 
 * 
 *
 * @author  
 * @date    2006/12/14
 */
#ifndef _NETWORKFILESTRUCT_V2C_H_
#define _NETWORKFILESTRUCT_V2C_H_

	
struct StuNetworkFileStructV2C
{	
	typedef struct
	{
		char chSign[12];
		unsigned long nMapCount;
	}StuHeaderInfo;


	struct StuIndexInfoOfMap
	{
		unsigned long nID;
		unsigned long nOffset;
		unsigned long nBreadthCount;

        StuIndexInfoOfMap()
        {
            memset(this,0,sizeof(StuIndexInfoOfMap));
        };
	};


	struct StuIndexInfoOfBreadth
	{
		unsigned long nBreadthMapID;
		unsigned long nOffset;
		unsigned long nAfterCompressSize;
		unsigned long nBeforeCompressSize;
        
		unsigned short nRoadSectionFlag;
		unsigned short nVertexFlag;

        StuIndexInfoOfBreadth()
        {
            memset(this,0,sizeof(StuIndexInfoOfBreadth));
        };

        void DumpState()
        {
            TRACE1("Breadth ID          :%d\n",nBreadthMapID);
            TRACE1("nOffset   :%d\n",nOffset);
            TRACE1("nAfterCompressSize  :%d\n",nAfterCompressSize);
            TRACE1("nBeforeCompressSize :%d\n",nBeforeCompressSize);

            TRACE1("nRoadSectionFlag    :%d\n",nRoadSectionFlag);
            TRACE1("nVertexFlag         :%d\n",nVertexFlag);
        };
        void AssertValid()
        {
            ASSERT( nBreadthMapID < 99999);
            ASSERT( nOffset > 0 && nOffset < 100*1024*1024);
            ASSERT( nAfterCompressSize  > 0 &&  nAfterCompressSize < 1024*1024);
            ASSERT( nBeforeCompressSize  > 0 &&  nBeforeCompressSize < 1024*1024);
            
            
        };
	};

	struct StuBreadthHeaderInfo
	{
		unsigned long nCountOfLevel1RoadSection;
		unsigned long nCountOfLevel2RoadSection;
		unsigned long nCountOfLevel3RoadSection;
		
		unsigned long nCountOfLevel1Vertex;
		unsigned long nCountOfLevel2Vertex;
		unsigned long nCountOfLevel3Vertex;
        
		unsigned long nRoadSectionCharDataBuddyLength;
		unsigned long nRoadSectionShortDataBuddyLength;
		unsigned long nRoadSectionDwordDataBuddyLength;

		unsigned long nVertexCharDataBuddyLength;
		unsigned long nVertexShortDataBuddyLength;
		unsigned long nVertexDwordDataBuddyLength;

        StuBreadthHeaderInfo()
        {
            memset(this,0,sizeof(StuBreadthHeaderInfo));
        };
	};


	//////////////////////////////////////////////////////////////////////////
	//���������ṹ����·������
	
	typedef struct
	{
		unsigned short	m_nRutID;			//·��ID	
		unsigned char	m_byDirection;		//·�ε�˫�б�־(0��ʾ˫�У�1��ʾend��start,2��ʾstart��end,3��ʾ˫�����)
		unsigned char	m_byType;			//��·���:(0--���٣�1--���и���·��2--������3--��Ҫ�ط���&�ֶ�
		                                    //4--һ���ص���5--��������6--������·��
		
		unsigned char	m_byCharge;			//�շ�:0--���շ�1--˫���շ�2--�����շ�3--�����շ�
		unsigned char	m_byLaneNum;		//·�γ�����:�����ݵĸ��ݵ�·�����ȱʡֵ
		unsigned char	m_bySpeed;			//�ٶȼ���:���ݵ�·�����ȱʡ�ٶ�
		unsigned char	m_byRutLevel;		//��·���ܼ���:
		
		float	m_dRLength;                 //·�γ���
		
		unsigned short m_nVtxStartID;       //·����ʼ�ڵ�ID
		unsigned short m_nVtxEndID;	        //·��β�ڵ�ID
		
		unsigned char	m_byNormalNum;	    //�Ƿ���ڽ�ͨ����		:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
		unsigned char	m_byReverseNum;	    //�Ƿ�����ڽ�ͨ����	:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
		unsigned char	m_byVirtualPicNum;	//����ͼ����
		unsigned char	m_byRutType;	    //��·����:0--����1--δ�ɼ�����2--�����߷���3--��������·֮��ĵ�·
		
		unsigned long	m_nRBID;			//·������ͼ��ID
		
	} stuRoadSection;

	typedef struct  
	{
		unsigned short	m_nVtxID;				//�ڵ�ID
		unsigned char	m_byRoadSectionNum;		//�����ӵ�·����
		unsigned char	unUsed;		
		
		float m_fLat;                           //γ������
		float m_fLog;                           //��������
		unsigned long	m_nVBID;                //�ڵ�����ͼ��ID
	} stuVertex;
};
#endif //_NETWORKFILESTRUCT_V2C_H_