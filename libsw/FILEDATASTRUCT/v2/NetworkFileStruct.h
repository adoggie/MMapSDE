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
#ifndef _NETWORKFILESTRUCT_H_
#define _NETWORKFILESTRUCT_H_

	
struct StuNetworkFileStructV2
{	
	typedef struct
	{
		char chSign[0x10];
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
		unsigned long BreadthMapID;
		unsigned long BreadthMapOffset;
		unsigned long nAfterCompressSize;
		unsigned long nBeforeCompressSize;
		unsigned long nLevel1DataAfterCompressSize;
		unsigned long nLevel1DataBeforeCompressSize;
		unsigned long nLevel2DataAfterCompressSize;
		unsigned long nLevel2DataBeforeCompressSize;


//		unsigned short nRoadSectionFlag;
//		unsigned short nVertexFlag;
	}StuIndexInfoOfBreadth;

	typedef struct
	{
		
		unsigned long nRoadSectionOffset;
		unsigned long nCountOfLevelAllRoadSection;
		unsigned long nCountOfLevel1RoadSection;
		unsigned long nCountOfLevel2RoadSection;
		
		unsigned long nVertexOffset;
		unsigned long nCountOfLevelAllVertex;
		unsigned long nCountOfLevel1Vertex;
		unsigned long nCountOfLevel2Vertex;


		unsigned long nOffsetOfRoadSectionBuddy;
		unsigned long nSizeOfLevelAllRoadSectionBuddy;
		unsigned long nSizeOfLevel1RoadSectionBuddy;
		unsigned long nSizeOfLevel2RoadSectionBuddy;

		unsigned long nOffsetOfVertexBuddy;
		unsigned long nSizeOfLevelAllVertexBuddy;
		unsigned long nSizeOfLevel1VertexBuddy;
		unsigned long nSizeOfLevel2VertexBuddy;
		
		unsigned long nRoadSectionCharDataBuddyOffset;
		unsigned long nRoadSectionCharDataBuddyLength;
		unsigned long nRoadSectionShortDataBuddyOffset;
		unsigned long nRoadSectionShortDataBuddyLength;
		unsigned long nRoadSectionDwordDataBuddyOffset;
		unsigned long nRoadSectionDwordDataBuddyLength;

		unsigned long nVertexCharDataBuddyOffset;
		unsigned long nVertexCharDataBuddyLength;
		unsigned long nVertexShortDataBuddyOffset;
		unsigned long nVertexShortDataBuddyLength;
		unsigned long nVertexDwordDataBuddyOffset;
		unsigned long nVertexDwordDataBuddyLength;

	}StuBreadthHeaderInfo;


	//////////////////////////////////////////////////////////////////////////
	//���������ṹ����·������
	
	typedef struct
	{
		unsigned short	m_nRutID;			//·��ID	
		unsigned char	m_byDirection;		//·�ε�˫�б�־(0��ʾ˫�У�1��ʾend��start,2��ʾstart��end,3��ʾ˫�����)
		unsigned char	m_byType;			//��·���:(0--���٣�1--���и���·��2--������3--��Ҫ�ط���&�ֶ�
		//4--һ���ص���5--��������6--������·��
		
		unsigned char	m_byCharge;			//�շ�:0--���շ�1--˫���շ�2--�����շ�3--�����շ�
		unsigned char	m_byLaneNum;			//·�γ�����:�����ݵĸ��ݵ�·�����ȱʡֵ
		unsigned char	m_bySpeed;			//�ٶȼ���:���ݵ�·�����ȱʡ�ٶ�
		unsigned char	m_byRutLevel;		//��·���ܼ���:
		
		float	m_dRLength;			//·�γ���
		
		unsigned short m_nVtxStartID;//·����ʼ�ڵ�ID
		unsigned short m_nVtxEndID;	//·��β�ڵ�ID
		
		unsigned char	m_byNormalNum;	//�Ƿ���ڽ�ͨ����		:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
		unsigned char	m_byReverseNum;	//�Ƿ�����ڽ�ͨ����	:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
		unsigned char	m_byVirtualPicNum;	//����ͼ����
		unsigned char	m_byRutType;	//��·����:0--����1--δ�ɼ�����2--�����߷���3--��������·֮��ĵ�·
		
		unsigned long	m_nRBID;			//·������ͼ��ID
		
	} stuRoadSection;

	typedef struct  
	{
		unsigned short	m_nVtxID;				//�ڵ�ID
		unsigned char	m_byRoadSectionNum;		//�����ӵ�·����
		unsigned char	unUsed;		
		
		float m_fLat;//γ������
		float m_fLog;//��������
		unsigned long	m_nVBID;							//�ڵ�����ͼ��ID
	} stuVertex;
};
#endif //_NETWORKFILESTRUCT_H_