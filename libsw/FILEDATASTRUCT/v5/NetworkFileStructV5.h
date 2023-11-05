/**
 * @file   
 * @brief ����·���ṹ����
 *
 * @purpose 
 * 
 *
 * @author  
 * @date    2006/11/21
 */
#ifndef _NETWORKFILESTRUCT_V5_H_
#define _NETWORKFILESTRUCT_V5_H_

	
struct StuNetworkFileStructV5
{	
	typedef struct
	{
		char chSign[0x10];
		unsigned long nMapCount;
	}StuHeaderInfo;

	typedef struct
	{
		unsigned long nID;              //��ͼ��ID
		unsigned long nOffset;          //��ͼ���ļ�ƫ��
		unsigned long nBreadthCount;    //��ͼ�ڵ�ͼ��������
	}StuIndexInfoOfMap;

	typedef struct
	{
		unsigned long nID;
		unsigned long nOffset;
		unsigned long nAfterCompressSize;
		unsigned long nBeforeCompressSize;
 
	}StuIndexInfoOfBreadth;

	struct StuBreadthHeaderInfo
	{
		unsigned long nCountOfLevelAllRoadSection;		
		unsigned long nCountOfLevelAllVertex;

        
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
        //���������λ����CGis_NetworkRoadSectionV5���־���һ��,����CGis_NetworkRoadSectionV5��������Ϊʵ.
        unsigned long	m_nRutID;			//·��ID	
        unsigned long	m_nRBID;			//·������ͼ��ID
        
        unsigned char	m_byDirection;		//·�ε�˫�б�־(0��ʾ˫�У�1��ʾend��start,2��ʾstart��end,3��ʾ˫�����)
        unsigned char	m_byCharge;			//�շ�:0--���շ�1--˫���շ�2--�����շ�3--�����շ�
        unsigned char	m_byRutLevel;		//��·���ܼ���:
        unsigned char	m_byRutType;	//��·����:0--����1--δ�ɼ�����2--�����߷���3--��������·֮��ĵ�·
        unsigned char	m_byType;	        //��·�ȼ�
        unsigned char	m_byunuse[3];	        
        
        
        unsigned long  m_nStartVertexID;//·����ʼ�ڵ�ID
        unsigned long  m_nEndVertexID;	//·��β�ڵ�ID
        
        unsigned char	m_byNormalNum;	//�Ƿ���ڽ�ͨ����		:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
        unsigned char	m_byReverseNum;	//�Ƿ�����ڽ�ͨ����	:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
             
        float	m_dRLength;			//·�γ���
		
	} stuRoadSection;

	typedef struct  
    {
        unsigned long	m_nID;		            //�ڵ�ID
        unsigned long	m_nBreadthID;			//�ڵ�����ͼ��ID
        
        unsigned char	m_nRoadSectionNum;		//�����ӵ�·����
        unsigned char	m_nEdge;				//bit0 �Ƿ��Ǳ߽���
                                                //bit1-bit6 δ����
                                                //bit7 ��ʾ����ı䳤�����Ƿ��Ǵӱ�����ڴ�������Ϸ������.
        float m_fLat;   //γ������
        float m_fLog;   //��������
	}stuVertex;
};
#endif //_NETWORKFILESTRUCT_V5_H_