/**
 * @file   
 * @brief 主干路网结构定义
 *
 * @purpose 
 * 
 *
 * @author  
 * @date    2006/11/21
 */
#ifndef _NETWORKFILESTRUCT_V7_H_
#define _NETWORKFILESTRUCT_V7_H_

	
struct StuNetworkFileStructV7
{	
	typedef struct
	{
		char chSign[0x10];
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
	//下面两个结构用于路网数据
	
	typedef struct
	{
        //这里的数据位置与CGis_NetworkRoadSectionV7保持绝对一致,并且CGis_NetworkRoadSectionV7析构函数为实.
        unsigned short	m_nRutID;			//路段ID	
        unsigned short	m_nRBID;		//路段所在图幅ID
        
        unsigned char	m_byDirection;		//路段单双行标志(0表示双行，1表示end到start,2表示start到end,3表示双向禁行)
        unsigned char	m_byCharge;			//收费:0--不收费1--双向收费2--正向收费3--反向收费
        unsigned char	m_byRutLevel;		//道路功能级别:
        unsigned char	m_byRutType;	//道路类型:0--环岛1--未采集属性2--上下线分离3--两条高速路之间的道路
        unsigned char	m_byType;	        //道路等级
        unsigned char	m_byunuse[3];	        
               
        
        unsigned short  m_nStartVertexID;//路段起始节点ID
        unsigned short  m_nEndVertexID;	//路段尾节点ID
        
        unsigned char	m_byNormalNum;	//是否存在交通限制		:0-没有交通限制;非0- 有交通限制(且表示路段个数)
        unsigned char	m_byReverseNum;	//是否反向存在交通限制	:0-没有交通限制;非0- 有交通限制(且表示路段个数)
             
        float	m_dRLength;			//路段长度
		
	} stuRoadSection;

	typedef struct  
    {
        unsigned short	m_nID;		            //节点ID
        unsigned short	m_nBreadthID;			//节点所在图幅ID
        
        unsigned char	m_nRoadSectionNum;		//相连接的路段数
        unsigned char	m_nEdge;				//是否边界结点
        unsigned char	m_nLowLayerRoadSectionNum;		//低级路段个数
        unsigned char	unuse;				//是否边界结点

        float m_fLat;   //纬度坐标
        float m_fLog;   //经度坐标
	}stuVertex;
};
#endif //_NETWORKFILESTRUCT_V7_H_
