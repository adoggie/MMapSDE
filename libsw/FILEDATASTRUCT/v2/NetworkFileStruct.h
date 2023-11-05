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
	//下面两个结构用于路网数据
	
	typedef struct
	{
		unsigned short	m_nRutID;			//路段ID	
		unsigned char	m_byDirection;		//路段单双行标志(0表示双行，1表示end到start,2表示start到end,3表示双向禁行)
		unsigned char	m_byType;			//道路类别:(0--高速，1--都市高速路，2--国道，3--主要地方道&轮渡
		//4--一般县道，5--市镇村道，6--其他道路）
		
		unsigned char	m_byCharge;			//收费:0--不收费1--双向收费2--正向收费3--反向收费
		unsigned char	m_byLaneNum;			//路段车道数:无数据的根据道路类别有缺省值
		unsigned char	m_bySpeed;			//速度级别:根据道路类别有缺省速度
		unsigned char	m_byRutLevel;		//道路功能级别:
		
		float	m_dRLength;			//路段长度
		
		unsigned short m_nVtxStartID;//路段起始节点ID
		unsigned short m_nVtxEndID;	//路段尾节点ID
		
		unsigned char	m_byNormalNum;	//是否存在交通限制		:0-没有交通限制;非0- 有交通限制(且表示路段个数)
		unsigned char	m_byReverseNum;	//是否反向存在交通限制	:0-没有交通限制;非0- 有交通限制(且表示路段个数)
		unsigned char	m_byVirtualPicNum;	//虚拟图数量
		unsigned char	m_byRutType;	//道路类型:0--环岛1--未采集属性2--上下线分离3--两条高速路之间的道路
		
		unsigned long	m_nRBID;			//路段所在图幅ID
		
	} stuRoadSection;

	typedef struct  
	{
		unsigned short	m_nVtxID;				//节点ID
		unsigned char	m_byRoadSectionNum;		//相连接的路段数
		unsigned char	unUsed;		
		
		float m_fLat;//纬度坐标
		float m_fLog;//经度坐标
		unsigned long	m_nVBID;							//节点所在图幅ID
	} stuVertex;
};
#endif //_NETWORKFILESTRUCT_H_