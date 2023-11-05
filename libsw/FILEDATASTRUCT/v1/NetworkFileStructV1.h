/**
 * @file   F:\WorkStation-3.5\WorkStation-Assistant\BanConvertTools\FileDataStruct.h
 * @brief 地图数据文件的类型和结构
 *
 * @purpose 
 * 
 *
 * @author  
 * @date    2006/10/24
 */
#ifndef _NETWORKFILESTRUCT_V1_H_
#define _NETWORKFILESTRUCT_V1_H_

	
struct StuNetworkFileStructV1
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
		unsigned long nBreadthMapID;
		unsigned long nBreadthMapOffset;
		unsigned long nBreadthMapSize;
	}StuIndexInfoOfBreadth;

	typedef struct
	{
		unsigned long nCountOfLevel1RoadSection;
		unsigned long nCountOfLevel2RoadSection;
		unsigned long nCountOfLevel3RoadSection;
		
		unsigned long nVertexOffset;
		unsigned long nCountOfLevel1Vertex;
		unsigned long nCountOfLevel2Vertex;
		unsigned long nCountOfLevel3Vertex;
	}StuBreadthHeaderInfo;

};
#endif //_NETWORKFILESTRUCT_V1_H_