
#ifndef _GIS_NETWORK_VTX_H
#define _GIS_NETWORK_VTX_H

#include "swBase.h"
class IMemoryMng;
class IMemoryMng2;

class CGIS_NetworkVtx  
{
public:
	CGIS_NetworkVtx( );
	~CGIS_NetworkVtx();
    CGIS_NetworkVtx* CreateCloneObject();
    void DestoryCloneObject();
    friend class CGIS_NetworkVtx;
public:
	void SetVtxInfo( unsigned short nVID, ST_GEO_PTXY ptPostion, int nVBID = 0 );
	void SetVRutInfo( BYTE byRNum = 0, unsigned short *pVRList = NULL, unsigned short *pRTList = NULL, BYTE *pRLList = NULL );
	void SetVEInfo( BYTE byEdge = 0, int nABID = 0, unsigned short nARID = 0 );

	unsigned short GetVtxID( );
	ST_GEO_PTXY GetVtxPosition( );
	BYTE GetVRutNum( );
	unsigned short* GetVRutList( );
	unsigned short* GetVRTurnList( );
	BYTE* GetVRLList( );
	BYTE GetVtxEdge( );
	int GetVAbutBID( );
	unsigned short GetVAbutRID( );
	int GetVBreadthID( );

	ST_GEO_VSORT* GetVtxSInfo( );
    void ResetSortInfo( )
    {
        //ASSERT(m_pVSInfo);
        if(m_pVSInfo)
            memset( m_pVSInfo, 0, sizeof(ST_GEO_VSORT) );
    }
//	POSITION m_posOHead;
//	BYTE m_bySHead;				//Vertex status:0 1 2 ,
//											//   0 代表 未曾计算过,
//											//   1 OPEN,
//											//   2 CLOSE
//	POSITION m_posOEnd;
//	BYTE m_bySEnd;				//Vertex status:0 1 2 ,
//											//   0 代表 未曾计算过,
//											//   1 OPEN,
//											//   2 CLOSE
private:
	unsigned short	m_nVtxID;		//节点ID
	ST_GEO_PTXY		m_ptPosition;	//节点坐标
	ST_GEO_VEDGE	*m_pVEInfo;		//边缘节点信息
//	BYTE			m_byEdge;		//是否是图幅边缘:0-不是,1-是
//	int				m_nABID;		//邻接图幅号
//	unsigned short	m_nARID;		//连接Rut_id
	BYTE			m_byRNum;		//相连接的路段数
	unsigned short	*m_pVRList;		//相连接的路段ID
	unsigned short	*m_pRTList;		//与水平方向的夹角数组
	BYTE			*m_pRLList;		//路段所在图层ID
	int				m_nVBID;		//节点所在图幅ID
	ST_GEO_VSORT	*m_pVSInfo;		//节点遍历时的信息
    
#ifdef MEMORY_SELF_MANAGE
public:
	void* operator new(size_t size);
	void operator delete (void*,size_t);

    static IMemoryMng* m_pMemMngOfSelf;
    static IMemoryMng* m_pMemMngOfBuddyData;
    static IMemoryMng* m_pMemMngOfSortInfo;
    static IMemoryMng* m_pMemMngOfEdgeInfo;
#endif	
//////////////////////////////////////////////////////////////////////////

};

#endif 
