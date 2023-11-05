/**
 * @brief ID数据文件结构
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
		unsigned long nID;              //地图的ID
		unsigned long nOffset;          //地图的文件偏移
		unsigned long nBreadthCount;    //地图内的图幅的数量
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