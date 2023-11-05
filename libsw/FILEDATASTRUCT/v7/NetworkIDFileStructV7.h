/**
 * @brief ID�����ļ��ṹ
 *
 * @param 
 * @return
*/

#ifndef _NETWORKIDFILESTRUCTV7_H_
#define _NETWORKIDFILESTRUCTV7_H_


struct StuNetworkIDFileStructV7
{	
	typedef struct
	{
		char chSign[12];
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
     		
        StuBreadthHeaderInfo()
        {
            memset(this,0,sizeof(StuBreadthHeaderInfo));
        };
	};
    

	//////////////////////////////////////////////////////////////////////////	
	typedef struct
	{
        unsigned long nDetailID;
        unsigned long nMergeID;
		
	} stuRoadSection;

	typedef struct  
    {
        unsigned long nDetailID;
        unsigned long nMergeID;
	}stuVertex;
};


#endif //_NETWORKIDFILESTRUCTV7_H_