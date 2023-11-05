// GIS_MBBNetwork.cpp: implementation of the CGIS_MBBNetwork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_MBBNetwork.h"
#include "./Zlib/zlib.h"
#include "GeoView.h"

#include "FileDataStruct/V7/GIS_NetworkRoadSectionV7.h"
#include "FileDataStruct/V7/GIS_NetworkVertexV7.h"
#include "FileDataStruct/V7/GIS_FeatureRoadSectionV7.h"
#include "filedatastruct/V7/GIS_FeatureRoadV7.h"
#include "filedatastruct/V7/NetworkIDFileStructV7.h"

#include "./DebugFunctionLib/Watch.h"
//////////////////////////////////////////////////////////////////////////
#include "./DebugFunctionLib/DebugFun.h"
/**
 * @brief   调试函数库(只在调试版本中被定义)
 *
 * 日志文件功能
 * 调试输出到屏幕功能
 * 函数调用跟踪功能
 *
 *    
 * @note    如果不想在调试版本中使能这些宏中的一项或几项,应该将这些宏在local文
 *          件中重新定义为空,即打开下面相应的注释即可.       
*/

//#define DBG2SCR0(sz)              
//#define DBG2SCR1(sz, p1)          
//#define DBG2SCR2(sz, p1, p2)      
//#define DBG2SCR3(sz, p1, p2, p3) 
//#define TRACK_FUNCTION(sz) 
//#define DBG2LOG0(sz)                
//#define DBG2LOG1(sz, p1)            
//#define DBG2LOG2(sz, p1, p2)        
//#define DBG2LOG3(sz, p1, p2, p3)    
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "./MemoryMngLib/IMemoryMng.h"
#include "./MemoryMngLib/IMemoryMng2.h"
//////////////////////////////////////////////////////////////////////////

/*
#include <crtdbg.h>
#define   malloc(s)         _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)

int __cdecl MyAllocHook(
   int      nAllocType,
   void   * pvData,
   size_t   nSize,
   int      nBlockUse,
   long     lRequest,
   const unsigned char * szFileName,
   int      nLine
   )
{
   char *operation[] = { "", "allocating", "re-allocating", "freeing" };
   char *blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client" };

   if ( nBlockUse == _CRT_BLOCK )   // Ignore internal C runtime library allocations
      return( TRUE );

   _ASSERT( ( nAllocType > 0 ) && ( nAllocType < 4 ) );
   _ASSERT( ( nBlockUse >= 0 ) && ( nBlockUse < 5 ) );

//   fprintf( logFile, 
//            "Memory operation in %s, line %d: %s a %d-byte '%s' block (# %ld)\n",
//            szFileName, nLine, operation[nAllocType], nSize, 
//            blockType[nBlockUse], lRequest );
//    if ( pvData != NULL )
//      fprintf( logFile, " at %X", pvData );

   if(pvData == (void*)0x0323c660)
   {
       ASSERT(FALSE);
   }

   return( TRUE );         // Allow the memory operation to proceed
}
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CGIS_MBBNetwork::InsertNode(CGIS_NetworkNodeV7* pNode,CPtrList *pList, BOOL bHead )
{
	if( bHead )
	{
		//顺序插入
		POSITION oldPos,pos = pList->GetHeadPosition();
		
		CGIS_NetworkNodeV7 *pRtnNode = NULL;
		while(pos != NULL)
		{
			oldPos = pos;
			pRtnNode = (CGIS_NetworkNodeV7*)pList->GetNext(pos);
			if(pNode->dFValue < pRtnNode->dFValue)
			{
				pNode->pVertex->GetVtxSInfo()->posOHead = pList->InsertBefore(oldPos,pNode);
				return;
			}
		}

		pNode->pVertex->GetVtxSInfo()->posOHead = pList->AddTail(pNode);
	}
	else
	{
		//顺序插入
		POSITION oldPos,pos = pList->GetHeadPosition();
		
		CGIS_NetworkNodeV7 *pRtnNode = NULL;
		while(pos != NULL)
		{
			oldPos = pos;
			pRtnNode = (CGIS_NetworkNodeV7*)pList->GetNext(pos);
			if(pNode->dFValue < pRtnNode->dFValue)
			{
				pNode->pVertex->GetVtxSInfo()->posOEnd = pList->InsertBefore(oldPos,pNode);
				return;
			}
		}

		pNode->pVertex->GetVtxSInfo()->posOEnd = pList->AddTail(pNode);
	}
}
BOOL CGIS_MBBNetwork::CalculateNetworkNode( CGIS_NetworkNodeV7 *pNode, CPtrList *pOpenList, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead )
{
    
	if( !CalculateRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead ) )
	{
        //TRACE0("计算路段权值失败.删除此node.\n");
		delete pNode;
		return FALSE;
	}
    
    
	CGIS_NetworkNodeV7 *pOldNode = NULL;
	if( bHead )
	{
		if(pNode->pVertex->GetVtxSInfo()->bySHead == 1)//在正向计算的open列表中
		{
			//vtx计算过,就在open中,但因为权值更新了,所以要重新插入.
			if( pNode->pVertex->GetVtxSInfo()->posOHead )
			{
				pOldNode = (CGIS_NetworkNodeV7*)pOpenList->GetAt(pNode->pVertex->GetVtxSInfo()->posOHead);
				if(pNode->dGValue < pOldNode->dGValue)
				{
					pOpenList->RemoveAt(pOldNode->pVertex->GetVtxSInfo()->posOHead);
					delete pOldNode;
                    //TRACE0("插入节点.\n");
					InsertNode(pNode,pOpenList);
				}
                else
				{
                    //计算出来的G开销大的话,再把它销毁.
                    //TRACE0("不插入节点.\n");
					delete pNode;
				}
			}
			else
			{
                //TRACE0("插入节点.\n");
				InsertNode(pNode,pOpenList);
				pNode->pVertex->GetVtxSInfo()->bySHead = 1;
			}
		}
		else if(pNode->pVertex->GetVtxSInfo()->bySHead == 0)//
		{
			//没有计算过,就插入到OPEN中
            //TRACE0("插入节点.\n");
			InsertNode(pNode,pOpenList);
			pNode->pVertex->GetVtxSInfo()->bySHead = 1;

		}
        else
		{   
            //在CLOSE中.直接删掉.
            //TRACE0("不插入节点.\n");
			delete pNode;
		}
	}
	else
	{
		if(pNode->pVertex->GetVtxSInfo()->bySEnd == 1)
		{
			//插入到OPEN中
			if( pNode->pVertex->GetVtxSInfo()->posOEnd )
			{
				pOldNode = (CGIS_NetworkNodeV7*)pOpenList->GetAt(pNode->pVertex->GetVtxSInfo()->posOEnd);
				if(pNode->dGValue < pOldNode->dGValue)
				{
					pOpenList->RemoveAt(pOldNode->pVertex->GetVtxSInfo()->posOEnd);
					delete pOldNode;
                    //TRACE0("插入节点.\n");
					InsertNode(pNode,pOpenList,FALSE);
				}else
				{
                    //TRACE0("不插入节点.\n");
					delete pNode;
				}
			}
			else
			{
				//插入到OPEN中
                //TRACE0("插入节点.\n");
				InsertNode(pNode,pOpenList, FALSE);
				pNode->pVertex->GetVtxSInfo()->bySEnd = 1;
			}
		}
		else	if(pNode->pVertex->GetVtxSInfo()->bySEnd == 0)
		{
			//插入到OPEN中
            //TRACE0("插入节点.\n");
			InsertNode(pNode,pOpenList, FALSE);
			pNode->pVertex->GetVtxSInfo()->bySEnd = 1;

		}else
		{
            //TRACE0("不插入节点.\n");
			delete pNode;
		}
	}
	return TRUE;
}
BOOL CGIS_MBBNetwork::CalculateRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	switch( pPathInfo->GetVehicleType() )
	{
	case EN_VEHICLETYPE_CAR:	        //小汽车
		return CalculateCarRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_TRUCK:		    //大卡车
		return CalculateTruckRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_BUS:		    //公交车
		return CalculateBusRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_EMERGENCY:	    //救护车/消防车
		return CalculateEmergencyRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_TAXI:		    //出租车
		return CalculateTaxiRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_DELIVERY:	    //邮政车
		return CalculateDeliveryRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_BICYCLE:		//自行车
		return CalculateBicycleRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	case EN_VEHICLETYPE_PEDESTRIAN:		//步行
		return CalculatePedestrianRutVal( pNode, pPathInfo, ptEnd, byAngle, bHead );
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return FALSE;
}
const int	dCVel[]	= {20000, 25000, 30000, 40000, 50000, 70000, 110000};//速度
const float dDRate[] = {3.0, 2.5, 2.25, 2.0, 1.5, 1.2};//道路距离权值
const float dTAVal[] = {0.0,0.16,0.68,2.8,2.9,3.0};	//转弯权值
const float	fM[]	= {1.0, 1.25, 1.5, 1.75, 2.0};		//路面权值
//计算车辆类型为小汽车时该节点上最优的路段
BOOL CGIS_MBBNetwork::CalculateCarRutVal(
    CGIS_NetworkNodeV7 *pNode,
    CGIS_NetworkPathInfo *pPathInfo, 
    ST_GEO_PTXY ptEnd,
    BYTE byAngle,
    BOOL bHead )
{
    ////TRACE0("权值计算:小汽车\n");
    
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRoadSectionV7 *pRoadSection = pNode->pRoadSection;
	if( pRoadSection == NULL )
		return FALSE;

    
	if( pRoadSection->GetDirection() == 1 )
	{	
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 2 )
	{
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 3 )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}

    //////////////////////////////////////////////////////////////////////////
    //对规避的权值均取为5.0
    //收费
	if( pPathInfo->IsTollEvite() )
	{
		if( pRoadSection->GetCharge() == 1 || pRoadSection->GetCharge() == 2 )
			gn += 5.0;
	}
    //高速
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRoadSection->GetLevel() == 0 )
			gn += 5.0;
	}
    //回转
	if( pPathInfo->IsSelEvite() )
	{
		if( pRoadSection->GetType() == 23 )
			gn += 5.0;
	}
    //土路
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRoadSection->GetType() == 1 || pRoadSection->GetLevel() == 0 )
			gn += 5.0;
	}

    //////////////////////////////////////////////////////////////////////////
    //根据道路类型加权(道路类型:0--环岛1--未采集属性2--上下线分离3--两条高速路之间的道路)
	if( pRoadSection->GetType() == 1 )
		gn += 0.2;


    //转弯角度加权
	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
    //bsxy
	int nMID = 1;//m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRoadSection->GetBreadthID() )
		{
			if( pRoadSection->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRoadSection->GetLevel()];
	//最短时间(较快路线)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//最短距离(较短路线)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRoadSection->GetLength() / 1000.);
	}
	//较少收费路线
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += (float)(pRoadSection->GetLength()/1000.0)*1.5;
		else 
			gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//较少转弯
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
    
    //距离加权
//	len = (float)CalcDist(pNode->pVertex->GetVtxPosition().dx, pNode->pVertex->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->pVertex->GetLog()-ptEnd.dx) + fabs(pNode->pVertex->GetLat() - ptEnd.dy) )*53.483); ///0.0006
    //bsxy
//	if( len > m_dbMaxDis )
//	{
//		pNode->dFValue = pNode->dGValue = 10000.0;
//		return FALSE;
// 	}
	gn += dbAVal;
	pNode->dGValue = pNode->pParent->dGValue + gn;
	if( m_byCRType == 4 )
		pNode->dFValue = (float)( len*dDRate[pRoadSection->GetLevel()] + pNode->dGValue );
	else
		pNode->dFValue = (float)( len + pNode->dGValue );
	return TRUE;
}
//计算车辆类型为大卡车时该节点上最优的路段
BOOL CGIS_MBBNetwork::CalculateTruckRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRoadSectionV7 *pRoadSection = pNode->pRoadSection;
	if( pRoadSection->GetDirection() == 1 )
	{	
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 2 )
	{
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 3 )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	if( pPathInfo->IsTollEvite() )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRoadSection->GetLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
		if( pRoadSection->GetType() == 23 )
			gn += 5.0;
	}
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRoadSection->GetLevel() == 0 )
			gn += 5.0;
	}
	if( pRoadSection->GetType() == 6 || 
		pRoadSection->GetType() == 7 || pRoadSection->GetType() == 3 )
		gn += 3.0;
	if( pRoadSection->GetLevel() == 0 )
		gn += 2.0;
	if( pRoadSection->GetType() == 1 )
		gn += 1.0;

	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
    //bsxy
	int nMID = 1;//m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRoadSection->GetBreadthID() )
		{
			if( pRoadSection->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRoadSection->GetLevel()];
	//最短时间(较快路线)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//最短距离(较短路线)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRoadSection->GetLength() / 1000.);
	}
	//较少收费路线
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += (float)(pRoadSection->GetLength()/1000.0)*1.5;
		else 
			gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//较少转弯
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->pVertex->GetVtxPosition().dx, pNode->pVertex->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->pVertex->GetLog()-ptEnd.dx) + fabs(pNode->pVertex->GetLat() - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	gn += dbAVal;
	pNode->dGValue = pNode->pParent->dGValue + gn;
	if( m_byCRType == 4 )
		pNode->dFValue = (float)( len*dDRate[pRoadSection->GetLevel()] + pNode->dGValue );
	else
		pNode->dFValue = (float)( len + pNode->dGValue );
	return TRUE;
}
//计算车辆类型为公交车时该节点上最优的路段
BOOL CGIS_MBBNetwork::CalculateBusRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRoadSectionV7 *pRoadSection = pNode->pRoadSection;
	if( pRoadSection->GetDirection() == 1 )
	{	
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 2 )
	{
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 3 )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	if( pPathInfo->IsTollEvite() )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRoadSection->GetLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
		if( pRoadSection->GetType() == 23 )
			gn += 5.0;
	}
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRoadSection->GetLevel() == 0 )
			gn += 5.0;
	}
	if( pRoadSection->GetType() == 6 || 
		pRoadSection->GetType() == 7 || pRoadSection->GetType() == 3 )
		gn += 2.0;
	if( pRoadSection->GetLevel() == 0 )
		gn += 1.5;
	if( pRoadSection->GetType() == 1 )
		gn += 0.2;
	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
    //bsxy
	int nMID = 1;//m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRoadSection->GetBreadthID() )
		{
			if( pRoadSection->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRoadSection->GetLevel()];
	//最短时间(较快路线)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//最短距离(较短路线)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRoadSection->GetLength() / 1000.);
	}
	//较少收费路线
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += (float)(pRoadSection->GetLength()/1000.0)*1.5;
		else 
			gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//较少转弯
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->pVertex->GetVtxPosition().dx, pNode->pVertex->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->pVertex->GetLog()-ptEnd.dx) + fabs(pNode->pVertex->GetLat() - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	gn += dbAVal;
	pNode->dGValue = pNode->pParent->dGValue + gn;
	if( m_byCRType == 4 )
		pNode->dFValue = (float)( len*dDRate[pRoadSection->GetLevel()] + pNode->dGValue );
	else
		pNode->dFValue = (float)( len + pNode->dGValue );
	return TRUE;
}
//计算车辆类型为救护车/消防车时该节点上最优的路段
BOOL CGIS_MBBNetwork::CalculateEmergencyRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRoadSectionV7 *pRoadSection = pNode->pRoadSection;
	if( pPathInfo->IsTollEvite() )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRoadSection->GetLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
		if( pRoadSection->GetType() == 23 )
			gn += 5.0;
	}
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRoadSection->GetLevel() == 0 )
			gn += 5.0;
	}
	if( pRoadSection->GetType() == 6 || 
		pRoadSection->GetType() == 7 || pRoadSection->GetType() == 3 )
		gn += 1.0;
	if( pRoadSection->GetLevel() == 0 )
		gn += 0.5;
	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
    //bsxy
	int nMID = 1;//m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRoadSection->GetBreadthID() )
		{
			if( pRoadSection->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRoadSection->GetLevel()];
	//最短时间(较快路线)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//最短距离(较短路线)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRoadSection->GetLength() / 1000.);
	}
	//较少收费路线
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += (float)(pRoadSection->GetLength()/1000.0)*1.5;
		else 
			gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//较少转弯
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->pVertex->GetVtxPosition().dx, pNode->pVertex->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->pVertex->GetLog()-ptEnd.dx) + fabs(pNode->pVertex->GetLat() - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	gn += dbAVal;
	pNode->dGValue = pNode->pParent->dGValue + gn;
	if( m_byCRType == 4 )
		pNode->dFValue = (float)( len*dDRate[pRoadSection->GetLevel()] + pNode->dGValue );
	else
		pNode->dFValue = (float)( len + pNode->dGValue );
	return TRUE;
}
//计算车辆类型为出租车时该节点上最优的路段
BOOL CGIS_MBBNetwork::CalculateTaxiRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRoadSectionV7 *pRoadSection = pNode->pRoadSection;
	if( pRoadSection->GetDirection() == 1 )
	{	
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 2 )
	{
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 3 )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	if( pPathInfo->IsTollEvite() )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRoadSection->GetLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
		if( pRoadSection->GetType() == 23 )
			gn += 5.0;
	}
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRoadSection->GetLevel() == 0 )
			gn += 5.0;
	}
	if( pRoadSection->GetType() == 6 || 
		pRoadSection->GetType() == 7 || pRoadSection->GetType() == 3 )
		gn += 1.0;
	if( pRoadSection->GetLevel() == 0 )
		gn += 0.5;
	if( pRoadSection->GetType() == 1 )
		gn += 0.5;
	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
    //bsxy
	int nMID = 1;//m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRoadSection->GetBreadthID() )
		{
			if( pRoadSection->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRoadSection->GetLevel()];
	//最短时间(较快路线)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//最短距离(较短路线)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRoadSection->GetLength() / 1000.);
	}
	//较少收费路线
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += (float)(pRoadSection->GetLength()/1000.0)*1.5;
		else 
			gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//较少转弯
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->pVertex->GetVtxPosition().dx, pNode->pVertex->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->pVertex->GetLog()-ptEnd.dx) + fabs(pNode->pVertex->GetLat() - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	gn += dbAVal;
	pNode->dGValue = pNode->pParent->dGValue + gn;
	if( m_byCRType == 4 )
		pNode->dFValue = (float)( len*dDRate[pRoadSection->GetLevel()] + pNode->dGValue );
	else
		pNode->dFValue = (float)( len + pNode->dGValue );
	return TRUE;
}
//计算车辆类型为邮政车时该节点上最优的路段
BOOL CGIS_MBBNetwork::CalculateDeliveryRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRoadSectionV7 *pRoadSection = pNode->pRoadSection;
	if( pRoadSection->GetDirection() == 1 )
	{	
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 2 )
	{
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	else if( pRoadSection->GetDirection() == 3 )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	if( pPathInfo->IsTollEvite() )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRoadSection->GetLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
		if( pRoadSection->GetType() == 23 )
			gn += 5.0;
	}
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRoadSection->GetLevel() == 0 )
			gn += 5.0;
	}
	if( pRoadSection->GetType() == 6 || 
		pRoadSection->GetType() == 7 || pRoadSection->GetType() == 3 )
		gn += 1.0;
	if( pRoadSection->GetLevel() == 0 )
		gn += 0.5;
	if( pRoadSection->GetType() == 1 )
		gn += 0.5;
	float dbAVal = dTAVal[byAngle];
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
    //bsxy
	int nMID = 1;//m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRoadSection->GetBreadthID() )
		{
			if( pRoadSection->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRoadSection->GetLevel()];
	//最短时间(较快路线)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//最短距离(较短路线)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRoadSection->GetLength() / 1000.);
	}
	//较少收费路线
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += (float)(pRoadSection->GetLength()/1000.0)*1.5;
		else 
			gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//较少转弯
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->pVertex->GetVtxPosition().dx, pNode->pVertex->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->pVertex->GetLog()-ptEnd.dx) + fabs(pNode->pVertex->GetLat() - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	gn += dbAVal;
	pNode->dGValue = pNode->pParent->dGValue + gn;
	if( m_byCRType == 4 )
		pNode->dFValue = (float)( len*dDRate[pRoadSection->GetLevel()] + pNode->dGValue );
	else
		pNode->dFValue = (float)( len + pNode->dGValue );
	return TRUE;
}
//计算车辆类型为自行车时该节点上最优的路段
BOOL CGIS_MBBNetwork::CalculateBicycleRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRoadSectionV7 *pRoadSection = pNode->pRoadSection;
	if( pRoadSection->GetDirection() != 0 )
	{
		if( pRoadSection->GetStartVtxID() == pNode->pVertex->GetVtxID() && !bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
		else if( pRoadSection->GetEndVtxID() == pNode->pVertex->GetVtxID() && bHead )
		{
			pNode->dFValue = pNode->dGValue = 10000.0;
			return FALSE;
		}
	}
	float dbAVal = 0.0;
	if( pPathInfo->IsTollEvite() )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRoadSection->GetLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
		if( pRoadSection->GetType() == 23 )
			gn += 5.0;
	}
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRoadSection->GetLevel() == 0 )
			gn += 5.0;
	}
	if( pRoadSection->GetLevel() == 5  || pRoadSection->GetLevel() == 4 )
		gn += 10.0;
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
    //bsxy
	int nMID = 1;//m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRoadSection->GetBreadthID() )
		{
			if( pRoadSection->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRoadSection->GetLevel()];
	//最短时间(较快路线)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//最短距离(较短路线)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRoadSection->GetLength() / 1000.);
	}
	//较少收费路线
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += (float)(pRoadSection->GetLength()/1000.0)*1.5;
		else 
			gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//较少转弯
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->pVertex->GetVtxPosition().dx, pNode->pVertex->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->pVertex->GetLog()-ptEnd.dx) + fabs(pNode->pVertex->GetLat() - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	gn += dbAVal;
	pNode->dGValue = pNode->pParent->dGValue + gn;
	if( m_byCRType == 4 )
		pNode->dFValue = (float)( len*dDRate[pRoadSection->GetLevel()] + pNode->dGValue );
	else
		pNode->dFValue = (float)( len + pNode->dGValue );
	return TRUE;
}
//计算车辆类型为步行时该节点上最优的路段
BOOL CGIS_MBBNetwork::CalculatePedestrianRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  )
{
	float  fMaxVelocity = 0.0;
	float  gn = 0.0;
	float  len = 0.0;
	
	CGIS_NetworkRoadSectionV7 *pRoadSection = pNode->pRoadSection;
	float dbAVal = 0.0;
	if( pPathInfo->IsTollEvite() )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += 5.0;
	}
	if( pPathInfo->IsHWayEvite() )
	{
		if( pRoadSection->GetLevel() == 1 )
			gn += 5.0;
	}
	if( pPathInfo->IsSelEvite() )
	{
		if( pRoadSection->GetType() == 23 )
			gn += 5.0;
	}
	if( pPathInfo->IsUnlayEvite() )
	{
		if( pRoadSection->GetLevel() == 0 )
			gn += 5.0;
	}
	if( pRoadSection->GetLevel() == 5 || pRoadSection->GetLevel() == 4 )
		gn += 10.0;
	UINT nCount = 0;
	int *pMIDs = NULL;
	int *pBIDs = NULL;
	int *pRIDs = NULL;
	BOOL bInAvoid = FALSE;
    //bsxy
	int nMID = 1;//m_pMap->GetMapInfo()->GetMapID();
	pPathInfo->GetAvoidRutInfo( nCount, &pMIDs, &pBIDs, &pRIDs );
	for( int i=0;	i<nCount;	i++ )
	{
		if( pMIDs[i] == nMID && pBIDs[i] == pRoadSection->GetBreadthID() )
		{
			if( pRoadSection->GetRutID() == pRIDs[i] )
			{
				bInAvoid = TRUE;
				break;
			}
		}
	}
	fMaxVelocity = dCVel[pRoadSection->GetLevel()];
	//最短时间(较快路线)
	if( pPathInfo->GetRoutePreference() == EN_RUTPREF_FASTERTIME  )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//最短距离(较短路线)
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_SHORTDIS )
	{
		gn += (float)(pRoadSection->GetLength() / 1000.);
	}
	//较少收费路线
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLECOST )
	{
		if( pRoadSection->GetType() == 24 || pRoadSection->GetType() == 25 || pRoadSection->GetType() == 26 )
			gn += (float)(pRoadSection->GetLength()/1000.0)*1.5;
		else 
			gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
	}
	//较少转弯
	else if( pPathInfo->GetRoutePreference() == EN_RUTPREF_LITTLETURN )
	{
		gn += (float)((pRoadSection->GetLength() /fMaxVelocity) * 60.0 + 0.2);
		if( byAngle == 1 || byAngle == 2 )
			dbAVal *= 1.2;
		else
			dbAVal *= 1.25;
	}
	if( bInAvoid )
		gn += 20.0;
//	len = (float)CalcDist(pNode->pVertex->GetVtxPosition().dx, pNode->pVertex->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy)*0.0006;
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*31.45); ///0.0004
//	len = (float)( (fabs(pNode->pVertex->GetVtxPosition().dx-ptEnd.dx) + fabs(pNode->pVertex->GetVtxPosition().dy - ptEnd.dy) )*47.175); ///0.0006
	len = (float)( (fabs(pNode->pVertex->GetLog()-ptEnd.dx) + fabs(pNode->pVertex->GetLat() - ptEnd.dy) )*53.483); ///0.0006
	if( len > m_dbMaxDis )
	{
		pNode->dFValue = pNode->dGValue = 10000.0;
		return FALSE;
	}
	gn += dbAVal;
	pNode->dGValue = pNode->pParent->dGValue + gn;
	if( m_byCRType == 4 )
		pNode->dFValue = (float)( len*dDRate[pRoadSection->GetLevel()] + pNode->dGValue );
	else
		pNode->dFValue = (float)( len + pNode->dGValue );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
 static BOOL g_bInitialSign = FALSE;
IMemoryMng* CGIS_MBBNetwork::m_pMemMngOfNetworkData = NULL;
//////////////////////////////////////////////////////////////////////////
CGIS_MBBNetwork::CGIS_MBBNetwork()
{       
   //_CrtSetAllocHook( MyAllocHook );

    ASSERT(m_pMemMngOfNetworkData);
    
    memset(m_pRutList,              0, sizeof(CGIS_NetworkRoadSectionV7*)*NAV_MAX_BCBNUM);
    memset(m_pVertexList,           0, sizeof(CGIS_NetworkVertexV7*)*NAV_MAX_BCBNUM);
    memset(m_pRoadSectionBuddyList, 0, sizeof(char*)*NAV_MAX_BCBNUM);
    memset(m_pVertexBuddyList,      0, sizeof(char*)*NAV_MAX_BCBNUM);
    memset(m_nRutNum,               0, sizeof(unsigned short*)*NAV_MAX_BCBNUM);
    memset(m_nVtxNum,               0, sizeof(unsigned short*)*NAV_MAX_BCBNUM);
    
    m_pBreadthMapLoadCount = (char*)m_pMemMngOfNetworkData->newmalloc(NAV_MAX_BCBNUM);
    memset(m_pBreadthMapLoadCount,0,NAV_MAX_BCBNUM);
    

	m_pIIBList = NULL;
	m_pBIDs = NULL;
	m_nBCnt = 0;
	m_nBNum = 0;


    m_bRoundaboutFlag = FALSE;
    m_dbMaxDis = 100000000.0;

    m_pVoiceNode = NULL;
	m_pFRPre = NULL;

    m_pBeforeCompressBuffer = NULL;
    m_pAfterCompressBuffer = NULL;
    m_nSizeOfBeforeCompressBuffer = 0;
    m_nSizeOfAfterCompressBuffer = 0;

    m_nDetailBreadthIDOfCurrentIDArray = 0xffffffff;

}

CGIS_MBBNetwork::~CGIS_MBBNetwork()
{
    ASSERT(m_pMemMngOfNetworkData);

    FreeNetwork();
    FreeNetWorkIDInfo();

    m_pMemMngOfNetworkData->newfree(m_pBreadthMapLoadCount);
    m_pBreadthMapLoadCount = NULL;

    if(g_bInitialSign)
    {
      ClearOperation();
    }
}
BOOL CGIS_MBBNetwork::InitialOperation(enErrorCode* pErrorCode)
{
    ASSERT(!g_bInitialSign);

    BOOL bRtn = LoadNetWorkMapInfo(pErrorCode);
    if(!bRtn)
        return FALSE;
	
    bRtn = LoadSpaceMapInfo(pErrorCode);
    if(!bRtn)
        return FALSE;

    g_bInitialSign = TRUE;
    return TRUE;
}
int CGIS_MBBNetwork::ClearOperation()
{
    ASSERT(g_bInitialSign);
    FreeNetWorkMapInfo();
    FreeSpaceMapInfo();

    g_bInitialSign = FALSE;
    return TRUE;
}

BOOL CGIS_MBBNetwork::IfUpNode( int nDetailBreadthID,int nDetailVertexID,enErrorCode* pErrorCode)
{

    int nDetailID;
    CGIS_MBBNetwork::stuPairIDItem* pMergeIDItem;
    //-------------------------------------------------------------
    
    nDetailID = (nDetailBreadthID<<15)|nDetailVertexID;
    if(!LoadNetWorkIDInfo(nDetailBreadthID,pErrorCode))
    {
        return FALSE;
    }
    pMergeIDItem = GetVertexMergeIDByDetailID(nDetailID);

    return pMergeIDItem == NULL? FALSE:TRUE;
}
//////////////////////////////////////////////////////////////////////////

//BOOL CGIS_MBBNetwork::MemoryInitial()
//{
//    ASSERT(m_pMemoryMng);
//    IMemoryMng* pMemoryMng = m_pMemoryMng;
//    pMemoryMng->ResetHeap();
//
//	
//    CGIS_FeatureRoadSectionV7::M_PM
//    CGIS_NetworkRoadSectionV7::m_pMemMngOfSelf = pMemoryMng;
//    CGIS_NetworkVertexV7::m_pMemMngOfSelf = pMemoryMng;
//
//	CGIS_Feature::m_pMemMngOfFeaturePoint   = pMemoryMng;
//	CGIS_Feature::m_pMemMngOfFeatureLine    = pMemoryMng;
//	CGIS_Feature::m_pMemMngOfFeaturePoly    = pMemoryMng;
//	CGIS_Feature::m_pMemMngOfFeatureRut     = pMemoryMng;
//	CGIS_Feature::m_pMemMngOfBuddyData      = pMemoryMng;
//	//////////////////////////////////////////////////////////////////////////
//#ifdef _DEBUG
//    pMemoryMng ->SetTextInfo(_T("SpaceData"));
//#endif
//
//	return TRUE;
//}
//BOOL CGIS_MBBNetwork::MemoryClear()
//{
//    ASSERT(m_pMemoryMng);
//    //MEMMNG_DUMPSTATE(m_pMemoryMng);
//    return TRUE;
//}


//////////////////////////////////////////////////////////////////////////

#include "./FileDataStruct/V7/SpaceFileStructV7.h"
/**
 * @brief 加载空间地图信息,进行图幅加载必须要先加载这些相关索引信息 
 *
 * @param 
 * @return
*/
static HANDLE g_hSpaceFile = INVALID_HANDLE_VALUE;
static StuSpaceFileStructV7::StuIndexInfoOfBreadth* g_pIndexInfoArrayOfBreadthInSpaceMap = NULL;
static DWORD g_dwOffsetBaseOfMapDataInSpaceMap = 0;
static unsigned long g_nBreadthCountInSpaceMap = 0;

BOOL CGIS_MBBNetwork::LoadSpaceMapInfo(enErrorCode* pErrorCode)
{
    if( g_hSpaceFile == INVALID_HANDLE_VALUE )
	{
//		swString temp = GetProPathName(L"\map\\");
		swString strFName;
		strFName.Format( L"%sBase.img", m_pMap->GetMapInfo()->GetMapFilePath() );
		g_hSpaceFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == g_hSpaceFile)
        {
            if(pErrorCode)
            {
                *pErrorCode = CGIS_MBBNetwork::ERROR_FILE_IO;
            }
			return FALSE;
        }
	}

	CFile aFile(g_hSpaceFile);
    CFile* pFile = &aFile;
    pFile->SeekToBegin();

    TRY
    {        
        //读头
        StuSpaceFileStructV7 :: StuHeaderInfo aHeadInfo;
        pFile->Read(&aHeadInfo,sizeof(StuSpaceFileStructV7 :: StuHeaderInfo));
        //if( strcmp(aHeadInfo.chSign,"RUT_TEST") != 0 )
        //    return 0;
        
        int nCount = aHeadInfo.nMapCount;
        ASSERT(nCount == 1);
        //---------------------------
        //读入地图索引
        StuSpaceFileStructV7::StuIndexInfoOfMap aIndexInfoOfMap;//地图索引信息
        pFile->Read(&aIndexInfoOfMap,sizeof(StuSpaceFileStructV7::StuIndexInfoOfMap) * nCount);
        
        //////////////////////////////////////////////////////////////////////////
        DWORD dwOffsetBase = pFile->GetPosition();
        pFile->Seek( aIndexInfoOfMap.nOffset + dwOffsetBase,CFile::begin);  //地图的偏移相对于文件起始位置的.所以不加头的大小.    
        //////////////////////////////////////////////////////////////////////////
        
        {
            int nCount = aIndexInfoOfMap.nBreadthCount;
            //////////////////////////////////////////////////////////////////////////
            //读索引数据   
            StuSpaceFileStructV7::StuIndexInfoOfBreadth* pIndexInfoArray = (StuSpaceFileStructV7::StuIndexInfoOfBreadth*)
                m_pMemMngOfNetworkData->newmalloc(sizeof(StuSpaceFileStructV7::StuIndexInfoOfBreadth)*nCount);
            if(!pIndexInfoArray)
                AfxThrowUserException();
            
            DWORD dwReadSize = sizeof(StuSpaceFileStructV7::StuIndexInfoOfBreadth) * nCount;
            void* pReadBuffer = pIndexInfoArray;
            DWORD dwReadActualSize = 0;
            dwReadActualSize = aFile.Read(pReadBuffer,dwReadSize);
            if(dwReadActualSize != dwReadSize)
                AfxThrowFileException(CFileException::generic);
            
            DWORD dwOffsetBase = pFile->GetPosition();
            g_pIndexInfoArrayOfBreadthInSpaceMap = pIndexInfoArray;
            g_dwOffsetBaseOfMapDataInSpaceMap = dwOffsetBase;
            g_nBreadthCountInSpaceMap = aIndexInfoOfMap.nBreadthCount;
            //////////////////////////////////////////////////////////////////////////
        }
    }
    CATCH(CFileException,e)
    {
        FreeSpaceMapInfo();
        if(pErrorCode)
        {
            *pErrorCode = CGIS_MBBNetwork::ERROR_FILE_IO;
        }
        return FALSE;
    }
    AND_CATCH(CUserException,e)
    {
        FreeSpaceMapInfo();
        if(pErrorCode)
        {
            *pErrorCode = CGIS_MBBNetwork::ERROR_FILE_IO;
        }
        return FALSE;
    }
    END_CATCH

    return TRUE;
}

BOOL CGIS_MBBNetwork::FreeSpaceMapInfo()
{
    ASSERT(m_pMemMngOfNetworkData);

    if(g_pIndexInfoArrayOfBreadthInSpaceMap);
        m_pMemMngOfNetworkData->newfree(g_pIndexInfoArrayOfBreadthInSpaceMap);
    g_pIndexInfoArrayOfBreadthInSpaceMap = NULL;
    g_nBreadthCountInSpaceMap = 0;
    g_dwOffsetBaseOfMapDataInSpaceMap = 0;

    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
#include "./FileDataStruct/V7/NetworkFileStructV7.h"
/**
 * @brief 加载地图信息,进行图幅加载必须要先加载这些相关索引信息 
 *
 * @param 
 * @return
*/
static HANDLE g_hNetworkFile = INVALID_HANDLE_VALUE;
static StuNetworkFileStructV7::StuIndexInfoOfBreadth* g_pIndexInfoArrayOfBreadth = NULL;
static DWORD g_dwOffsetBaseOfMapData = 0;
static unsigned long g_nBreadthCount = 0;


BOOL CGIS_MBBNetwork::LoadNetWorkMapInfo(enErrorCode* pErrorCode)
{
    if( g_hNetworkFile == INVALID_HANDLE_VALUE )
	{
//		swString temp = GetProPathName(L"\map\\");
		swString strFName;
		strFName.Format( L"%sBase.Rut", m_pMap->GetMapInfo()->GetMapFilePath() );
		g_hNetworkFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == g_hNetworkFile)
        {
            if(pErrorCode)
            {
                *pErrorCode = CGIS_MBBNetwork::ERROR_FILE_IO;
            }
            return FALSE;
        }
	}
	CFile aFile(g_hNetworkFile);
    CFile* pFile = &aFile;

    //////////////////////////////////////////////////////////////////////////

    TRY
    {
        pFile->SeekToBegin();
        
        //读头
        StuNetworkFileStructV7 :: StuHeaderInfo aHeadInfo;
        pFile->Read(&aHeadInfo,sizeof(StuNetworkFileStructV7 :: StuHeaderInfo));
        if( strcmp(aHeadInfo.chSign,"RUT_TEST") != 0 )
            return 0;
        
        int nCount = aHeadInfo.nMapCount;
        ASSERT(nCount == 1);
        //---------------------------
        //读入地图索引
        StuNetworkFileStructV7::StuIndexInfoOfMap aIndexInfoOfMap;//地图索引信息
        pFile->Read(&aIndexInfoOfMap,sizeof(StuNetworkFileStructV7::StuIndexInfoOfMap) * nCount);
        
        //////////////////////////////////////////////////////////////////////////
        DWORD dwOffsetBase = pFile->GetPosition();
        pFile->Seek( aIndexInfoOfMap.nOffset + dwOffsetBase,CFile::begin);  //地图的偏移相对于文件起始位置的.所以不加头的大小.    
        //////////////////////////////////////////////////////////////////////////
        
        {
            int nCount = aIndexInfoOfMap.nBreadthCount;
            //////////////////////////////////////////////////////////////////////////
            //读索引数据   
            g_pIndexInfoArrayOfBreadth = (StuNetworkFileStructV7::StuIndexInfoOfBreadth*)
                m_pMemMngOfNetworkData->newmalloc(sizeof(StuNetworkFileStructV7::StuIndexInfoOfBreadth)*nCount);
            if(!g_pIndexInfoArrayOfBreadth)
                AfxThrowUserException();
            
            DWORD dwReadSize = sizeof(StuNetworkFileStructV7::StuIndexInfoOfBreadth) * nCount;
            DWORD dwReadActualSize = 0;
            dwReadActualSize = pFile->Read(g_pIndexInfoArrayOfBreadth,dwReadSize);
            if(dwReadActualSize != dwReadSize)
                AfxThrowFileException(CFileException::generic);
            
            DWORD dwOffsetBase = pFile->GetPosition();
            g_dwOffsetBaseOfMapData = dwOffsetBase;
            g_nBreadthCount = aIndexInfoOfMap.nBreadthCount;
            //////////////////////////////////////////////////////////////////////////
        }
    }
    CATCH(CFileException,e)
    {
        FreeNetWorkMapInfo();
        if(pErrorCode)
        {
            *pErrorCode = CGIS_MBBNetwork::ERROR_FILE_IO;
        }
        return FALSE;
    }
    AND_CATCH(CUserException,e)
    {
        FreeNetWorkMapInfo();
        if(pErrorCode)
        {
            *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
        }
        return FALSE;
    }
    END_CATCH
        
    return TRUE;
}


BOOL CGIS_MBBNetwork::LoadNetWorkIDInfoActual(
    unsigned long nDetailBreadthID,
    enErrorCode* pErrorCode,
    stuMergeIDArray* pRoadSectionIDInfo,
    stuMergeIDArray* pVertexIDInfo)
{
   if(nDetailBreadthID == m_nDetailBreadthIDOfCurrentIDArray)
       return TRUE;
   
   FreeNetWorkIDInfoActual(pRoadSectionIDInfo,pVertexIDInfo);    
    //////////////////////////////////////////////////////////////////////////
    //加载图幅索引ID数组
    TRY
    {        
        CFile aFile(m_pMap->GetMapInfo()->GetMapFilePath()+_T("NetworkID.dat"),CFile::modeRead);
        CFile* pFile = &aFile;
        pFile->SeekToBegin();
        
        
        StuNetworkIDFileStructV7::StuHeaderInfo aHeadInfo;
        pFile->Read(&aHeadInfo,sizeof(StuNetworkIDFileStructV7::StuHeaderInfo));
        
        if( strcmp(aHeadInfo.chSign,"RUT_TEST") != 0 )
            return 0;
        
        int nCount = aHeadInfo.nMapCount;
        ASSERT(nCount == 1);
        //---------------------------
        //读入地图索引
        StuNetworkIDFileStructV7::StuIndexInfoOfMap aIndexInfoOfMap;//地图索引信息
        pFile->Read(&aIndexInfoOfMap,sizeof(StuNetworkFileStructV7::StuIndexInfoOfMap) * nCount);
        
        //////////////////////////////////////////////////////////////////////////
        DWORD dwOffsetBase = pFile->GetPosition();
        pFile->Seek( aIndexInfoOfMap.nOffset + dwOffsetBase,CFile::begin);  //地图的偏移相对于文件起始位置的.所以不加头的大小.    
        //////////////////////////////////////////////////////////////////////////
             
        StuNetworkIDFileStructV7::StuIndexInfoOfBreadth aIndexInfoOfBreadth;          
        pFile->Seek( sizeof(StuNetworkFileStructV7::StuIndexInfoOfBreadth)*nDetailBreadthID + dwOffsetBase, CFile::begin);  
        pFile->Read(&aIndexInfoOfBreadth,sizeof(StuNetworkFileStructV7::StuIndexInfoOfBreadth));
        
        
        //////////////////////////////////////////////////////////////////////////
        dwOffsetBase = dwOffsetBase + aIndexInfoOfMap.nOffset +
            sizeof(StuNetworkFileStructV7::StuIndexInfoOfBreadth)*aIndexInfoOfMap.nBreadthCount;
        pFile->Seek( aIndexInfoOfBreadth.nOffset + dwOffsetBase, CFile::begin);  
        StuNetworkIDFileStructV7::StuBreadthHeaderInfo aBreadthHeader;
        pFile->Read(&aBreadthHeader,sizeof(StuNetworkFileStructV7::StuBreadthHeaderInfo));
        //////////////////////////////////////////////////////////////////////////
      
        {
            int nCount = aBreadthHeader.nCountOfLevelAllRoadSection;
            stuPairIDItem* pArray = (stuPairIDItem*)m_pMemMngOfNetworkData->newmalloc(sizeof(stuPairIDItem)*nCount);
            if(!pArray)
                AfxThrowUserException();
            
            DWORD dwReadSize = sizeof(stuPairIDItem)*nCount;
            void* pReadBuffer = pArray;
            DWORD dwReadActualSize = 0;
            dwReadActualSize = pFile->Read(pReadBuffer,dwReadSize);
            if(dwReadActualSize != dwReadSize)
                AfxThrowFileException(CFileException::generic);
            
            pRoadSectionIDInfo->nArraySize  = nCount;
            pRoadSectionIDInfo->pArray      = pArray;
            
        }
        {
            int nCount = aBreadthHeader.nCountOfLevelAllVertex;
            stuPairIDItem* pArray = (stuPairIDItem*)m_pMemMngOfNetworkData->newmalloc(sizeof(stuPairIDItem)*nCount);
            if(!pArray)
                AfxThrowUserException();
            
            DWORD dwReadSize = sizeof(stuPairIDItem)*nCount;
            void* pReadBuffer = pArray;
            DWORD dwReadActualSize = 0;
            dwReadActualSize = pFile->Read(pReadBuffer,dwReadSize);
            if(dwReadActualSize != dwReadSize)
                AfxThrowFileException(CFileException::generic);
            
            pVertexIDInfo->nArraySize   = nCount;
            pVertexIDInfo->pArray       = pArray;
        }
    }
    CATCH(CFileException,e)
    {
        FreeNetWorkMapInfo();
        if(pErrorCode)
        {
            *pErrorCode = CGIS_MBBNetwork::ERROR_FILE_IO;
        }
       return FALSE;
    }
    AND_CATCH(CUserException,e)
    {
        FreeNetWorkMapInfo();
        if(pErrorCode)
        {
            *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
        }
        return FALSE;
    }
    END_CATCH
        
    m_nDetailBreadthIDOfCurrentIDArray = nDetailBreadthID;
    return TRUE;

}
BOOL CGIS_MBBNetwork::FreeNetWorkIDInfoActual(    stuMergeIDArray* pRoadSectionIDInfo,stuMergeIDArray* pVertexIDInfo)
{
    if(pRoadSectionIDInfo->pArray)
        m_pMemMngOfNetworkData->newfree(pRoadSectionIDInfo->pArray);
    pRoadSectionIDInfo->pArray = NULL;
    
    if(pVertexIDInfo->pArray)
        m_pMemMngOfNetworkData->newfree(pVertexIDInfo->pArray);
    pVertexIDInfo->pArray = NULL;  

    pRoadSectionIDInfo->nArraySize = 0;
    pVertexIDInfo->nArraySize = 0;

    m_nDetailBreadthIDOfCurrentIDArray = 0xffffffff;

    return TRUE;
}

BOOL CGIS_MBBNetwork::LoadNetWorkIDInfo(unsigned long nDetailBreadthID,enErrorCode* pErrorCode)
{
    return LoadNetWorkIDInfoActual(nDetailBreadthID,pErrorCode,&m_arRoadSectionIDArrayDetailToTrunk,&m_arVertexIDArrayDetailToTrunk);
}

BOOL CGIS_MBBNetwork::FreeNetWorkIDInfo()
{
    return FreeNetWorkIDInfoActual(&m_arRoadSectionIDArrayDetailToTrunk,&m_arVertexIDArrayDetailToTrunk);
}

BOOL CGIS_MBBNetwork::FreeNetWorkMapInfo()
{
    ASSERT(m_pMemMngOfNetworkData);

    if(g_pIndexInfoArrayOfBreadth)
        m_pMemMngOfNetworkData->newfree(g_pIndexInfoArrayOfBreadth);
    g_pIndexInfoArrayOfBreadth = NULL;
    
    
    g_nBreadthCount = 0;
    g_dwOffsetBaseOfMapData = 0;
   
    return TRUE; 
}



StuNetworkFileStructV7::StuIndexInfoOfBreadth* CGIS_MBBNetwork::GetBreadthIndexByBreadthID(unsigned long nBID)
{
	StuNetworkFileStructV7::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = NULL;
	// 定位区域：
	int nStart = 0;
	int nEnd = g_nBreadthCount;
	int nMid = 0;
	while( nEnd > nStart )
	{
		nMid = (nStart+nEnd)/2;
		pIndexInfoOfBreadth = &g_pIndexInfoArrayOfBreadth[nMid];
		if( pIndexInfoOfBreadth->nID == nBID )
			return pIndexInfoOfBreadth;
		else if( pIndexInfoOfBreadth->nID > nBID )
			nEnd = nMid;
		else
			nStart = nMid;
		if( nEnd-nStart <= 1 )
		{
			if ( g_pIndexInfoArrayOfBreadth[nStart].nID == nBID )
				return &g_pIndexInfoArrayOfBreadth[nStart];
			else if ( g_pIndexInfoArrayOfBreadth[nEnd].nID == nBID )
				return &g_pIndexInfoArrayOfBreadth[nEnd];
			return NULL;
		}
	}
	return NULL;
}

CGIS_MBBNetwork::stuPairIDItem* CGIS_MBBNetwork::stuMergeIDArray::GetDestIDBySourceID(unsigned long nDetailID)
{
	stuPairIDItem* pItem = NULL;
	// 定位区域：
	int nStart = 0;
	int nEnd = nArraySize;
	int nMid = 0;
	while( nEnd > nStart )
	{
		nMid = (nStart+nEnd)/2;
		pItem = &pArray[nMid];
		if( pItem->nSourceID == nDetailID )
			return pItem;
		else if( pItem->nSourceID > nDetailID )
			nEnd = nMid;
		else
			nStart = nMid;
		if( nEnd-nStart <= 1 )
		{
			if ( pArray[nStart].nSourceID == nDetailID )
				return &pArray[nStart];
			else if ( pArray[nEnd].nSourceID == nDetailID )
				return &pArray[nEnd];
			return NULL;
		}
	}
	return NULL;
}

#ifndef _WIN32_WCE

CGIS_MBBNetwork::stuPairIDItem* CGIS_MBBNetwork::GetRoadSectionMergeIDByDetailIDV2(unsigned long nDetailID)
{
    enErrorCode eErrorCode  = ERROR_NONE;
    if(!LoadNetWorkIDInfo((nDetailID>>15),&eErrorCode))
    {
        return NULL;
    }

    return m_arRoadSectionIDArrayDetailToTrunk.GetDestIDBySourceID(nDetailID);
}

CGIS_MBBNetwork::stuPairIDItem* CGIS_MBBNetwork::GetVertexMergeIDByDetailIDV2(unsigned long nDetailID)
{
    enErrorCode eErrorCode  = ERROR_NONE;
    if(!LoadNetWorkIDInfo((nDetailID>>15),&eErrorCode))
    {
        return NULL;
    }
    
     return m_arVertexIDArrayDetailToTrunk.GetDestIDBySourceID(nDetailID);
}
#endif
/**
 * @brief 加载指定图幅的数据
 *
 * @param nBID-图幅ID
 * @return 0-加载失败 1-加载成功
*/
BOOL CGIS_MBBNetwork::LoadBreadthData( unsigned long nBreadthID,enErrorCode* pErrorCode)
{
    ASSERT(g_pIndexInfoArrayOfBreadth);
    //TRACE1("LoadBreadthData %d;\n",nBreadthID);
    //WATCH("LoadBreadthData");

    //StuNetworkFileStructV7::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = &g_pIndexInfoArrayOfBreadth[nBID];
    StuNetworkFileStructV7::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = GetBreadthIndexByBreadthID(nBreadthID);
    ASSERT(pIndexInfoOfBreadth->nID == nBreadthID);

    //////////////////////////////////////////////////////////////////////////
    //判断图幅是否已加载过
    ASSERT(m_pBreadthMapLoadCount);
    if(m_pBreadthMapLoadCount[nBreadthID])
        return TRUE;

  	m_pBreadthMapLoadCount[nBreadthID] = 1;

    //////////////////////////////////////////////////////////////////////////
        
	ASSERT(pIndexInfoOfBreadth);
 
    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = g_dwOffsetBaseOfMapData;
    //////////////////////////////////////////////////////////////////////////
	CFile aFile(g_hNetworkFile);
	//CFile aFile((int)g_hNetworkFile);
    CFile* pFile = &aFile;
    
	int nRoadSectionTotalCount = 0;
	int nJunctionTotalCount = 0;
	
	char *pFBuf = NULL;
	{
		//两个size为0表示此图幅是一个空的图幅
		ASSERT(pIndexInfoOfBreadth->nBeforeCompressSize > 0);
		ASSERT(pIndexInfoOfBreadth->nAfterCompressSize > 0);


		int nBeforeCompressSize = pIndexInfoOfBreadth->nBeforeCompressSize;
		int nAfterCompressSize  = pIndexInfoOfBreadth->nAfterCompressSize;
        ASSERT(pIndexInfoOfBreadth->nBeforeCompressSize < 400000);
        ASSERT(pIndexInfoOfBreadth->nAfterCompressSize < 200000);


		DWORD dwLayerOffset     = dwOffsetBase;
        dwLayerOffset = dwOffsetBase + pIndexInfoOfBreadth->nOffset;
		
		//分配压缩前空间
		char *pBufOfBeforeCompress = m_pBeforeCompressBuffer;
		ASSERT(pBufOfBeforeCompress);

		//-------------------------------------------------
		//读取并解压数据
		{
			pFile->Seek( dwLayerOffset,CFile::begin);		
			char *pBufOfAfterCompress = m_pAfterCompressBuffer;//分配压缩后
			ASSERT(pBufOfAfterCompress);
			
			pFile->Read( pBufOfAfterCompress, nAfterCompressSize );
			DWORD dwSize = nBeforeCompressSize;
			int nRet = uncompress( (unsigned char*)pBufOfBeforeCompress, &dwSize, (unsigned char*)pBufOfAfterCompress, nAfterCompressSize  );
			ASSERT(nRet == Z_OK);
		}
		//-----------------------------------------------------

		//-----------------------------------------------------
		//处理图幅头数据
		StuNetworkFileStructV7::StuBreadthHeaderInfo* pBreadthHeader = (StuNetworkFileStructV7::StuBreadthHeaderInfo*)pBufOfBeforeCompress;
		unsigned long nCountOfRoadSection   = pBreadthHeader->nCountOfLevelAllRoadSection;
		unsigned long nCountOfVertex        = pBreadthHeader->nCountOfLevelAllVertex;


		unsigned long nSizeOfRoadSectionBuddy = 
            pBreadthHeader->nRoadSectionCharDataBuddyLength + 
            pBreadthHeader->nRoadSectionShortDataBuddyLength +
            pBreadthHeader->nRoadSectionDwordDataBuddyLength;
		unsigned long nSizeOfVertexBuddy = 
            pBreadthHeader->nVertexCharDataBuddyLength + 
            pBreadthHeader->nVertexShortDataBuddyLength +
            pBreadthHeader->nVertexDwordDataBuddyLength;

        unsigned long nOffsetOfRoadSection      = sizeof(StuNetworkFileStructV7::StuBreadthHeaderInfo);
        unsigned long nOffsetOfVertex           = nOffsetOfRoadSection + sizeof(StuNetworkFileStructV7::stuRoadSection)*nCountOfRoadSection;
        unsigned long nOffsetOfRoadSectionBuddy = nOffsetOfVertex + sizeof(StuNetworkFileStructV7::stuVertex)*nCountOfVertex;
        unsigned long nOffsetOfVertexBuddy      = nOffsetOfRoadSectionBuddy + nSizeOfRoadSectionBuddy;
		//-----------------------------------------------------
        char* pRoadSectionBuddyData            = NULL;
        char* pVertexBuddyData                 = NULL;
		if(nSizeOfRoadSectionBuddy > 0)
		{
			pRoadSectionBuddyData = (char*)m_pMemMngOfNetworkData->newmalloc(nSizeOfRoadSectionBuddy);
            if(!pRoadSectionBuddyData)
            {
                if(pErrorCode)
                {
                    *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
                }
                return FALSE;
            }
			memcpy(
                pRoadSectionBuddyData,
                &pBufOfBeforeCompress[nOffsetOfRoadSectionBuddy],
                nSizeOfRoadSectionBuddy);
            
            
		}
		if(nSizeOfVertexBuddy > 0)
		{
			pVertexBuddyData = (char*)m_pMemMngOfNetworkData->newmalloc(nSizeOfVertexBuddy);
            if(!pVertexBuddyData)
            {
                if(pErrorCode)
                {
                    *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
                }
                return FALSE;
            }

			memcpy(
                pVertexBuddyData,
                &pBufOfBeforeCompress[nOffsetOfVertexBuddy],
                nSizeOfVertexBuddy);
		}
        m_pRoadSectionBuddyList[nBreadthID] = pRoadSectionBuddyData;
        m_pVertexBuddyList[nBreadthID] = pVertexBuddyData;
		
		
		//-----------------------------------------------------
		{
			char* pCharAddr		= NULL;
			char* pShortAddr	= NULL;
			char* pDwordAddr	= NULL;
			
			if(nSizeOfRoadSectionBuddy > 0)
			{
				pCharAddr	= pRoadSectionBuddyData;
				pShortAddr	= pRoadSectionBuddyData+pBreadthHeader->nRoadSectionCharDataBuddyLength;
				pDwordAddr	= pRoadSectionBuddyData+pBreadthHeader->nRoadSectionShortDataBuddyLength;
				
				if((*( (DWORD*)pCharAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pCharAddr	+= 4;
				}
				if((*( (DWORD*)pShortAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pShortAddr	+= 4;
				}
				if((*( (DWORD*)pDwordAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pDwordAddr	+= 4;
				}
			}
			
			int nReadNum = pBreadthHeader->nCountOfLevelAllRoadSection;
            m_pRutList[nBreadthID] = (CGIS_NetworkRoadSectionV7**)m_pMemMngOfNetworkData->newmalloc(sizeof(CGIS_NetworkRoadSectionV7*) * nReadNum);       
            if(!m_pRutList[nBreadthID])
            {
                if(pErrorCode)
                {
                    *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
                }
                return FALSE;
            }
            memset(m_pRutList[nBreadthID],0,sizeof(CGIS_NetworkRoadSectionV7*) * nReadNum);
            m_nRutNum[nBreadthID] = nReadNum;
			StuNetworkFileStructV7::stuRoadSection* pArrayOfStRoadSection = 
                (StuNetworkFileStructV7::stuRoadSection*)(pBufOfBeforeCompress+nOffsetOfRoadSection);
			for( int i = 0;	i < nReadNum;	i++ )
			{
                CGIS_NetworkRoadSectionV7* pObjAddr = (CGIS_NetworkRoadSectionV7*)m_pMemMngOfNetworkData->newmalloc(sizeof(CGIS_NetworkRoadSectionV7));
                if(!pObjAddr)
                {
                    if(pErrorCode)
                    {
                        *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
                    }
                    return FALSE;
                }

				CGIS_NetworkRoadSectionV7 *pRoadSection = new (pObjAddr) CGIS_NetworkRoadSectionV7;
				memcpy(pRoadSection->GetDataAddr(), &pArrayOfStRoadSection[i],sizeof(StuNetworkFileStructV7::stuRoadSection));
				pRoadSection->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
								
				ASSERTVALID(pRoadSection);
				ASSERT(pRoadSection->GetBreadthID() == pIndexInfoOfBreadth->nID);
				ASSERT(pRoadSection->GetRutID() == i);
				m_pRutList[nBreadthID][pRoadSection->GetRutID()] = pRoadSection;
			}

		}
		//--------------------------------------------------------------
		{
			char* pCharAddr		= NULL;
			char* pShortAddr	= NULL;
			char* pDwordAddr	= NULL;
			
			if(nSizeOfVertexBuddy)
			{
				pCharAddr	= pVertexBuddyData;
				pShortAddr	= pVertexBuddyData + pBreadthHeader->nVertexCharDataBuddyLength;
				pDwordAddr	= pVertexBuddyData + pBreadthHeader->nVertexShortDataBuddyLength;
				
				if((*( (DWORD*)pCharAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pCharAddr	+= 4;
				}
				if((*( (DWORD*)pShortAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pShortAddr	+= 4;
				}
				if((*( (DWORD*)pDwordAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pDwordAddr	+= 4;
				}
			}
			int nReadNum = pBreadthHeader->nCountOfLevelAllVertex;
            m_pVertexList[nBreadthID] = (CGIS_NetworkVertexV7**)m_pMemMngOfNetworkData->newmalloc(sizeof(CGIS_NetworkVertexV7*) * nReadNum);
            if(!m_pVertexList[nBreadthID])
            {
                if(pErrorCode)
                {
                    *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
                }
                return FALSE;
            }
            memset(m_pVertexList[nBreadthID],0,sizeof(CGIS_NetworkVertexV7*) * nReadNum);
            m_nVtxNum[nBreadthID] = nReadNum;
            StuNetworkFileStructV7::stuVertex* pArrayOfStVertex = (StuNetworkFileStructV7::stuVertex*)(pBufOfBeforeCompress+nOffsetOfVertex);
			for(int i = 0;	i < nReadNum;	i++ )
			{
                CGIS_NetworkVertexV7* pObjAddr = (CGIS_NetworkVertexV7*)m_pMemMngOfNetworkData->newmalloc(sizeof(CGIS_NetworkVertexV7));
                if(!pObjAddr)
                {
                    if(pErrorCode)
                    {
                        *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
                    }
                    return FALSE;
                }

                ASSERT(pObjAddr);
				CGIS_NetworkVertexV7 *pVertex = new (pObjAddr) CGIS_NetworkVertexV7;
				memcpy(pVertex->GetDataAddr(), &pArrayOfStVertex[i],sizeof(StuNetworkFileStructV7::stuVertex));
				pVertex->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
				ASSERTVALID(pVertex);

				ASSERT(pVertex->GetBreadthID() == pIndexInfoOfBreadth->nID);
                ASSERT(pVertex->GetVtxID() == i);
			    m_pVertexList[nBreadthID][pVertex->GetVtxID()] = pVertex;
			}

		}
		pBufOfBeforeCompress = NULL;
	}
	
	return TRUE;
}
BOOL CGIS_MBBNetwork::ClearUnnecessaryNetwork()
{
    return FALSE;
}

int CGIS_MBBNetwork::FreeBreadthData( unsigned long nBreadthID )
{
    ASSERT(nBreadthID < g_nBreadthCount);
    ASSERT(m_pBreadthMapLoadCount[nBreadthID] == 1);

    {
        CGIS_NetworkRoadSectionV7** pFtrArray = m_pRutList[nBreadthID];
        int nCount = m_nRutNum[nBreadthID];
        for(int i = 0; i < nCount; i++)
        {
            CGIS_NetworkRoadSectionV7* pObj = pFtrArray[i];
            if(pObj)
            {
                pObj->~CGIS_NetworkRoadSectionV7();
                m_pMemMngOfNetworkData->newfree(pObj);
            }
        }
        
        m_pMemMngOfNetworkData->newfree(pFtrArray);
        m_pRutList[nBreadthID] = NULL;
        m_nRutNum[nBreadthID] = 0;
    }
    {
        CGIS_NetworkVertexV7** pFtrArray = m_pVertexList[nBreadthID];
        int nCount = m_nVtxNum[nBreadthID];
        for(int i = 0; i < nCount; i++)
        {
            CGIS_NetworkVertexV7* pObj = pFtrArray[i];
            if(pObj)
            {
                pObj->~CGIS_NetworkVertexV7();
                m_pMemMngOfNetworkData->newfree(pObj);
            }
        }
        
        m_pMemMngOfNetworkData->newfree(pFtrArray);
        m_pVertexList[nBreadthID] = NULL;
        m_nVtxNum[nBreadthID] = 0;
    }

    m_pMemMngOfNetworkData->newfree(m_pRoadSectionBuddyList[nBreadthID]);
    m_pRoadSectionBuddyList[nBreadthID] = NULL;
    m_pMemMngOfNetworkData->newfree(m_pVertexBuddyList[nBreadthID]);
    m_pVertexBuddyList[nBreadthID] = NULL;

    m_pBreadthMapLoadCount[nBreadthID] = 0;
	return TRUE;
}

int CGIS_MBBNetwork::FreeNetwork()
{
    ASSERT(m_pBreadthMapLoadCount);
    for(int i = 0; i < g_nBreadthCount; i++)
    {
        if(m_pBreadthMapLoadCount[i] == 1)
        {
            FreeBreadthData(i);
        }
    }

    return TRUE;
}
//把图幅的ID转换为横向纵向的的个数
void CGIS_MBBNetwork::ParseBMapID( int nBMID, int &nBMXNum, int &nBMYNum )
{
	swString str;
	str.Format(_T("%d"), nBMID );
	nBMXNum = _wtoi(str.Mid(2,2))*8+_wtoi(str.Right(1));
	nBMYNum = _wtoi(str.Left(2))*8 + _wtoi(str.Mid(4,1));
}

/**
 * @brief 由详图的ftr(路段或节点)的id,算出此节点在主干的ftrID
 *
 * @param nDetailBreadthID-详图的排序后的图幅ID
 * @param nDetailFeatureID-详图的ftrid
 * @return
*/

BOOL CGIS_MBBNetwork::DetailFeatureID2TrunkFeatureID(int nDetailBreadthID,unsigned short nDetailFeatureID, unsigned long& nTrunkFeatureID)
{
    ASSERT(nDetailBreadthID < 99999);
    nTrunkFeatureID = (nDetailBreadthID << 15) + nDetailFeatureID;
    return TRUE;
}
StuBreadthMapInfo* CGIS_MBBNetwork::GetNWBInfo( unsigned long nBID )
{
	return NULL;
}

//从network的路段链表中查找路段
CGIS_NetworkRoadSectionV7* CGIS_MBBNetwork::GetNetworkRut( unsigned long nRBID, unsigned long nID )
{
	if( nID >= m_nRutNum[nRBID] )
		return NULL;
	return m_pRutList[nRBID][nID];
}

//从network的节点链表中查找节点
CGIS_NetworkVertexV7* CGIS_MBBNetwork::GetNetworkVtx( unsigned long nVBID, unsigned long nID )
{
	if( nID >= m_nVtxNum[nVBID] )
		return NULL;
	return m_pVertexList[nVBID][nID];
}

//CGIS_NetworkRoadSectionV7* CGIS_MBBNetwork::GetNetworkRut( unsigned long nOGID, unsigned long nRID )
//{
//	stuBreadthItem *pItem = NULL;
//	m_pBreadthContainer->Lookup(nOGID, pItem);
//	if( pItem && pItem->pRoadSectionContainer )
//	{
//		CGIS_NetworkRoadSectionV7 *pNRut = NULL;
//		pItem->pRoadSectionContainer->Lookup(nRID, pNRut);
//		return pNRut;
//	}
//	return NULL;
//}
//CGIS_NetworkVertexV7* CGIS_MBBNetwork::GetNetworkVtx( unsigned long nOGID, unsigned long nVID )
//{
//	stuBreadthItem *pItem = NULL;
//	m_pBreadthContainer->Lookup(nOGID, pItem);
//	if( pItem && pItem->pRoadSectionContainer )
//	{
//		CGIS_NetworkVertexV7 *pNVtx = NULL;
//		pItem->pVertexContainer->Lookup(nVID, pNVtx);
//		return pNVtx;
//	}
//	return NULL;
// }

///仅从单双行考虑是否可以到达路段的另一个节点
BOOL CGIS_MBBNetwork::CouldReachAnotherVertexInRoadSection( CGIS_NetworkVertexV7 *pVtx, CGIS_NetworkRoadSectionV7 *pRut )
{
    ASSERT(pVtx);
    ASSERT(pRut);
    ASSERT(pVtx->GetBreadthID() == pRut->GetBreadthID());
    
    if( pRut->GetDirection() == 0 )
        return TRUE;

    if(pRut->GetStartVtxID() == pVtx->GetVtxID())
    {
        if(pRut->GetDirection() == 1)
            return TRUE;
    }
    
    if(pRut->GetEndVtxID() == pVtx->GetVtxID())
    {
        if(pRut->GetDirection() == 2)
            return TRUE;
    }

    return FALSE;
}

CGIS_NetworkVertexV7* CGIS_MBBNetwork::GetNextRutVtx( CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRoadSection, BOOL bHead )
{
	CGIS_NetworkVertexV7* pRtnVtx = NULL;
	// 起点到终点
	if( pRoadSection->GetStartVtxID() == pVertex->GetVtxID() )
	{
		if( pRoadSection->GetDirection() == 0 )
        {
            pRtnVtx = GetNetworkVtx( pRoadSection->GetBreadthID(),pRoadSection->GetEndVtxID() );
            ASSERT(pRtnVtx);
        }
		else if( pRoadSection->GetDirection() == 1 && bHead )
        {
            pRtnVtx = GetNetworkVtx( pRoadSection->GetBreadthID(),pRoadSection->GetEndVtxID() );
            ASSERT(pRtnVtx);
        }
		else if( pRoadSection->GetDirection() == 2 && !bHead )
        {
            pRtnVtx = GetNetworkVtx( pRoadSection->GetBreadthID(),pRoadSection->GetEndVtxID() );
            ASSERT(pRtnVtx);
        }

	}
	else 
	{
		// 终点到起点
		if( pRoadSection->GetDirection() == 0 )
        {
            pRtnVtx = GetNetworkVtx( pRoadSection->GetBreadthID(),pRoadSection->GetStartVtxID() );
            ASSERT(pRtnVtx);
        }
		else if( pRoadSection->GetDirection() == 1 && !bHead )
        {
            pRtnVtx = GetNetworkVtx( pRoadSection->GetBreadthID(),pRoadSection->GetStartVtxID() );
            ASSERT(pRtnVtx);
        }
		else if( pRoadSection->GetDirection() == 2 && bHead )
        {
            pRtnVtx = GetNetworkVtx( pRoadSection->GetBreadthID(),pRoadSection->GetStartVtxID() );
            ASSERT(pRtnVtx);
        }

	}
	return pRtnVtx;
}
/**
 * @brief 取得路段的另一个节点
 *
 * @param 
 * @return
*/
CGIS_NetworkVertexV7* CGIS_MBBNetwork::GetNextRutVtx2( CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRoadSection, BOOL bHead )
{
	CGIS_NetworkVertexV7 *pSVtx = GetNetworkVtx( pRoadSection->GetBreadthID(), pRoadSection->GetStartVtxID() );
	CGIS_NetworkVertexV7 *pEVtx = GetNetworkVtx( pRoadSection->GetBreadthID(), pRoadSection->GetEndVtxID() );
	if( pSVtx == NULL || pEVtx == NULL )
	{
		ASSERT( pSVtx && pEVtx );
		return NULL;
	}
	if( pSVtx->GetEdge() == 1 && pEVtx->GetEdge() == 1 )
	{
        ASSERT( m_pBreadthMapLoadCount[pRoadSection->GetBreadthID()] == 1);

        int nEdgeRoadSectionID = pSVtx->GetEdgeRoadSectionID()&0X7fff;
        if( pVertex->GetRoadSectionList()[0] == nEdgeRoadSectionID )
            return GetNextRutVtx( pSVtx, pRoadSection, bHead );
        else
            return GetNextRutVtx( pEVtx, pRoadSection, bHead );
	}
	else if( pSVtx->GetEdge() == 1 )
	{
		return GetNextRutVtx( pSVtx, pRoadSection, bHead );
	}
	else if( pEVtx->GetEdge() == 1 )
	{
		return GetNextRutVtx( pEVtx, pRoadSection, bHead );
	}
	else
		ASSERT( FALSE );
	return NULL;
}

CGIS_NetworkVertexV7 *CGIS_MBBNetwork::GetVAbutVtx( CGIS_NetworkVertexV7 *pVertex )
{
	if( pVertex == NULL )
		return NULL;
	CGIS_NetworkVertexV7 *pVAbut = NULL;
	if( pVertex->GetEdge() == 1 )
	{
/*		CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork2( pVertex->GetVAbutBID() );
		if( pBMap == NULL )
			return pVertex;
		if( pBMap->m_nLoadCounter == 0 )
			ReadBreadthNetwork( &pBMap, m_byLFlag );
		if( pBMap->m_nLoadCounter == 0 )
		{
			pVAbut = pVertex;
			return pVAbut;
		}
		CGIS_NetworkRut *pNERut = GetNetworkRut( pBMap->m_nBreadthID, pVertex->GetVAbutRID() );
		if( pNERut )
		{
			CGIS_NetworkVtx *pSVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNERut->GetStartVtxID() );
			CGIS_NetworkVtx *pEVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNERut->GetEndVtxID() );
			if( pSVtx && pEVtx )
			{
				if( pSVtx->GetVtxEdge() == 1 && pEVtx->GetVtxEdge() == 1 )
				{
					CGIS_BreadthMap *pBTemp = GetBreadthMapOfNetwork(pVertex->GetBreadthID());
					if( pBTemp )
					{
						if( pBTemp->m_nBMID == pSVtx->GetVAbutBID() && pVertex->GetRoadSectionList()[0] == pSVtx->GetVAbutRID() )
							pVAbut = pSVtx;
						else
							pVAbut = pEVtx;
					}
					else
						ASSERT( FALSE );
				}
				else if( pSVtx->GetVtxEdge() == 1 )
					pVAbut = pSVtx;
				else if( pEVtx->GetVtxEdge() == 1 )
					pVAbut = pEVtx;
			}
		}
*/	}
	if( pVAbut == NULL )
		pVAbut = pVertex;
	return pVAbut;
}

////计算最优路段
BOOL CGIS_MBBNetwork::CalculateOptimalPathRut( 
    CPtrList *pOpenList, 
    CPtrList *pCloseList, 
    unsigned long *pVIDs,
    int *pVBIDs, 
    unsigned short &nVNum, 
	ST_GEO_PTXY ptEnd, 
    CGIS_NetworkPathInfo *pPathInfo, 
    BYTE byLRNum, 
    unsigned short *pLRList, 
    BYTE nRLevel, 
    BOOL bCFlag, 
    BOOL bSFlag,
    enErrorCode* pErrorCode)
{
    //Open队列不可以为空
	if( pOpenList->GetCount() <= 0 )
    {
        ASSERT(FALSE);
        return FALSE;
    }

//    {
//        //TRACE1("OpenList's count    :%d\n",pOpenList->GetCount());
//        //TRACE1("CloseList's count   :%d\n",pCloseList->GetCount());
//        //TRACE1("目标点个数          :%d\n",nVNum);
//    }



	CGIS_NetworkNodeV7 *pParentNode = (CGIS_NetworkNodeV7*)pOpenList->GetHead();

    
	//////////////////////////////////////////////////////////////////////////
	///判断是否已经找到最优路线,目标节点数组中有本结点则表示找到.
	int i;
	for( i=0;	i<nVNum;	i++ )
	{
		if( pParentNode->pVertex->GetVtxID() == pVIDs[i] && 
				pParentNode->pVertex->GetBreadthID() == pVBIDs[i] )
		{
            TRACE0("find dest in trunk network.\n");
			nVNum = i;
			return TRUE;
		}
	}

//////////////////////////////////////////////////////////////////////////
//debug code 
/*	CDC* pDC = m_pMapView->GetDC();
	//画出当前路段方向
	{
		int nStartID = pParentNode->pRoadSection->GetStartVtxID();
		int nEndID = pParentNode->pRoadSection->GetEndVtxID();
		CGIS_NetworkVertexV7* pStartVtx	= GetNetworkVtx(pParentNode->pRoadSection->GetBreadthID(), nStartID);
		CGIS_NetworkVertexV7* pEndVtx	= GetNetworkVtx(pParentNode->pRoadSection->GetBreadthID(),nEndID);
		ST_GEO_PTXY ptStart;
        ptStart.dx = pStartVtx->GetLog();
        ptStart.dy = pStartVtx->GetLat();
		ST_GEO_PTXY ptEnd;
        ptEnd.dx = pEndVtx->GetLog();
        ptEnd.dy = pEndVtx->GetLat();
		int nStartX;
		int nStartY;

		int nEndX;
		int nEndY;
		m_pMapView->XyMapToView(ptStart.dx,ptStart.dy,nStartX,nStartY);
		m_pMapView->XyMapToView(ptEnd.dx,ptEnd.dy,nEndX,nEndY);

		pDC->MoveTo(nStartX,nStartY);
		pDC->LineTo(nEndX,nEndY);
		CBrush RedBrush(RGB(255,0,0));
		CRect rt(nStartX-2,nStartY-2,nStartX+2,nStartY+2);
		pDC->FillRect(&rt,&RedBrush);

	}
	//画出当前节点
	ST_GEO_PTXY pt;
    pt.dx = pParentNode->pVertex->GetLog();
    pt.dy = pParentNode->pVertex->GetLat();
	int nX,nY;
	m_pMapView->XyMapToView(pt.dx,pt.dy,nX,nY);


	CRect rt(nX-6,nY-6,nX+6,nY+6);
	pDC->Rectangle(&rt);
	swString strVtxID;
	strVtxID.Format(_T("%d"),pParentNode->pVertex->GetVtxID());
	pDC->DrawText(strVtxID,&rt,DT_CENTER|DT_VCENTER);


	m_pMapView->ReleaseDC(pDC);
	//Sleep(100);
*/	
//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//temp code 
	//TRACE2("当前探索node:路段[%d]节点[%d].\n",pParentNode->pRoadSection->GetRutID(),pParentNode->pVertex->GetVtxID());
	//TRACE2("\t 节点坐标[%f][%f].\n",pParentNode->pVertex->GetLog(),pParentNode->pVertex->GetLat());
	//////////////////////////////////////////////////////////////////////////

	pOpenList->RemoveHead( );
	pParentNode->pVertex->GetVtxSInfo()->posOHead = pParentNode->pVertex->GetVtxSInfo()->posOEnd = NULL;
	pParentNode->pVertex->GetVtxSInfo()->bySHead = pParentNode->pVertex->GetVtxSInfo()->bySEnd = 2;
	if( pParentNode->pRoadSection )
	{
		if( pParentNode->pRoadSection->GetType() == 4 )
			pParentNode->pVertex->GetVtxSInfo()->bySHead = pParentNode->pVertex->GetVtxSInfo()->bySEnd = 1;
	}
	pCloseList->AddTail( pParentNode );

	CGIS_NetworkRoadSectionV7 *pRoadSection = NULL;
	CGIS_NetworkVertexV7 *pVertex = NULL;
	CGIS_NetworkNodeV7 *pChildNode = NULL;
	CGIS_NetworkNodeV7 *pOldNode = NULL;

    
	///计算节点为边界节点的情况
	if( pParentNode->pVertex->GetEdge() == 1 )
	{
		int nBID = pParentNode->pVertex->GetEdgeBreadthID();
        unsigned long nEdgeRoadSectionID = pParentNode->pVertex->GetEdgeRoadSectionID()&0x7fff;

        if(!LoadBreadthData(nBID,pErrorCode))
        {
            TRACE0("LoadBreadthData() failed when CalculateOptimalPathRut()\n");
            return FALSE;
        }

//		StuBreadthMapInfo *pBMap = GetNWBInfo(nBID);
//		if( pBMap == NULL )
//        {
//            return FALSE;
//        }
//		if( pBMap->nLCnt == 0 )
//			;//ReadBreadthNetwork( &pBMap, m_byLFlag );
//		if( pBMap->nLCnt == 0 )
// 			return FALSE;
		pRoadSection = GetNetworkRut( nBID, nEdgeRoadSectionID );
		if( pRoadSection == NULL )
        {
            ASSERT(FALSE); 
            return FALSE;
        }
		CGIS_NetworkVertexV7 *pSVtx = GetNetworkVtx( nBID, pRoadSection->GetStartVtxID() );
		CGIS_NetworkVertexV7 *pEVtx = GetNetworkVtx( nBID, pRoadSection->GetEndVtxID() );
//		ASSERT( pSVtx && pEVtx );
		if( pSVtx == NULL || pEVtx == NULL )		
        {
            ASSERT(FALSE);
            return FALSE;
        }
		if( pSVtx->GetEdge() == 1 || pEVtx->GetEdge() == 1 )
		{
			ASSERT( (fabs(pSVtx->GetLog()-pParentNode->pVertex->GetLog())<FLOAT_ZERO &&
				fabs(pSVtx->GetLat()-pParentNode->pVertex->GetLat())<FLOAT_ZERO ) ||
				( fabs(pEVtx->GetLog()-pParentNode->pVertex->GetLog())<FLOAT_ZERO &&
				fabs(pEVtx->GetLat()-pParentNode->pVertex->GetLat())<FLOAT_ZERO ) );
		}
		else
			ASSERT( FALSE );
		pVertex = GetNextRutVtx2( pParentNode->pVertex, pRoadSection, bCFlag );
		if( pVertex == NULL )
		{
			return FALSE;
		}
		if( pRoadSection->GetLevel() < nRLevel )
			return FALSE;

		pChildNode = new CGIS_NetworkNodeV7;
		if( pChildNode == NULL )
			return 0;
		pChildNode->pParent = pParentNode;
		pChildNode->pRoadSection = pRoadSection;
		pChildNode->pVertex = pVertex;
		BOOL bRtn = CalculateNetworkNode( pChildNode, pOpenList, pPathInfo, ptEnd, 1, bCFlag );
        if(bRtn)
        {
            //TRACE3("pChildNode node%d G[%f],F[%f]\n",i,pChildNode->dGValue,pChildNode->dFValue);         
        }
        else
        {
            ASSERT(FALSE);
        }

		if( pSVtx->GetEdge() == 1 )
		{
			if( bCFlag )
				pSVtx->GetVtxSInfo()->bySHead = 1;
			else
				pSVtx->GetVtxSInfo()->bySEnd = 1;
		}
		if( pEVtx->GetEdge() == 1 )
		{
			if( bCFlag )
				pEVtx->GetVtxSInfo()->bySHead = 1;
			else
				pEVtx->GetVtxSInfo()->bySEnd = 1;
		}
		return FALSE;
	}
	////计算其余路段的优先次序
	int nParent = 0;
	for( i=0;	i<pParentNode->pVertex->GetRoadSectionNum();	i++ )
	{
		if( pParentNode->pVertex->GetRoadSectionList()[i] == pParentNode->pRoadSection->GetRutID() )
		{
			nParent++;
			break;
		}
		nParent++;
	}
    int nRoadSectionNum = pParentNode->pVertex->GetRoadSectionNum();
    //TRACE1("根据相邻路段[%d条],扩展出相邻节点\n",nRoadSectionNum);
	int nChild = 0;
	for( i = 0;	i < nRoadSectionNum;	i++ )
	{
        //TRACE1("扩展第%d条\n",i);
		nChild++;
 
        pRoadSection = GetNetworkRut( pParentNode->pVertex->GetBreadthID(), pParentNode->pVertex->GetRoadSectionList()[i] );
		if( pRoadSection == NULL )
        {
            //TRACE0("数据已卸载,不处理\n");
            continue;
        }
		if( pParentNode->pRoadSection->GetRutID() == pRoadSection->GetRutID() && bSFlag == FALSE )
        {
            //TRACE0("来路,不处理\n");
            continue;
        }
		if( pRoadSection->GetLevel() < nRLevel )
        {
            //TRACE0("//等级低于阀值,不处理\n");
            continue;
        }
		pVertex = GetNextRutVtx( pParentNode->pVertex, pRoadSection, bCFlag );
		if( pVertex == NULL )
		{
			//TRACE0("此路段的另一节点限行,不处理\n");
			continue;
		}

        {
            //检查此路段是否被限行
            BOOL bLimit = FALSE;
            for( int j=0;	j < byLRNum; j++ )
            {
                if( pRoadSection->GetRutID() == pLRList[j] )
                {
                    bLimit = TRUE;
                    break;
                }
            }
            if( bLimit )
            {
                //TRACE0("路段被禁行,不处理\n");
                // bsxy continue;
            }
        }



        ////TRACE0("可处理,进行权值计算\n");
		pChildNode = new CGIS_NetworkNodeV7;

		pChildNode->pParent = pParentNode;
		pChildNode->pRoadSection = pRoadSection;
		pChildNode->pVertex = pVertex;
		int nAngle = 0;
		if( bCFlag )
			nAngle = 
            pParentNode->pVertex->GetRoadSectionTurnList()[nParent-1] - 
            pParentNode->pVertex->GetRoadSectionTurnList()[nChild-1];
		else
			nAngle = 
            pParentNode->pVertex->GetRoadSectionTurnList()[nChild-1]  -
            pParentNode->pVertex->GetRoadSectionTurnList()[nParent-1];

        
		BYTE byAngle = 1;
		int nAng = abs( abs(nAngle)-180 );
		//直行
		if( nAng < 8 )
			byAngle = 1;
		else if( nAng < 50 )
			byAngle = 2;
		else if( nAng < 120 )
			byAngle = 3;
		else if( nAng < 160 )
			byAngle = 4;
		else
			byAngle = 5;
			
        BOOL bRtn = CalculateNetworkNode( pChildNode, pOpenList, pPathInfo, ptEnd, byAngle, bCFlag );
//        if(bRtn)
//        {
//            //TRACE3("pChildNode node%d G[%f],F[%f]\n",i,pChildNode->dGValue,pChildNode->dFValue);         
//        }
//        else
//        {
//            ASSERT(FALSE);
//         }
	}



	return FALSE;
}

BOOL CGIS_MBBNetwork::CalculateOptimalPathRutV2( 
    CPtrList *pOpenList, 
    CPtrList *pCloseList, 
    unsigned long *pVIDs,
    int *pVBIDs, 
    unsigned short &nVNum, 
	ST_GEO_PTXY ptEnd, 
    CGIS_NetworkPathInfo *pPathInfo, 
    BYTE byLRNum, 
    unsigned short *pLRList, 
    BYTE nRLevel, 
    BOOL bCFlag, 
    BOOL bSFlag,
    enErrorCode* pErrorCode)
{
    //Open队列不可以为空
    if( pOpenList->GetCount() <= 0 )
    {
        ASSERT(FALSE);
        return FALSE;
    }

    CGIS_NetworkNodeV7 *pParentNode = (CGIS_NetworkNodeV7*)pOpenList->GetHead();
	//////////////////////////////////////////////////////////////////////////
	///判断是否已经找到最优路线,目标节点数组中有本结点则表示找到.
	for( int i = 0;	i < nVNum;	i++ )
	{
		if( pParentNode->pVertex->GetVtxID() == pVIDs[i] && 
			pParentNode->pVertex->GetBreadthID() == pVBIDs[i] )
		{
            TRACE0("find dest in trunk network.\n");
			nVNum = i;
			return TRUE;
		}
	}

	return FALSE; //added by scott
}
//护展当前最优点
BOOL CGIS_MBBNetwork::ExtendNode( 
    CGIS_NetworkNodeV7* pParentNode,
    CPtrList *pOpenList, 
	ST_GEO_PTXY ptEnd, 
    CGIS_NetworkPathInfo *pPathInfo, 
    BYTE byLRNum, 
    unsigned short *pLRList, 
    BYTE nRLevel, 
    BOOL bCFlag, 
    BOOL bSFlag,
    enErrorCode* pErrorCode)
{
    ASSERT(pParentNode);
    ASSERT(pOpenList);


	CGIS_NetworkRoadSectionV7 *pRoadSection = NULL;
	CGIS_NetworkVertexV7 *pVertex = NULL;
	CGIS_NetworkNodeV7 *pChildNode = NULL;
	CGIS_NetworkNodeV7 *pOldNode = NULL;

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
	//扩展边界节点
	if( pParentNode->pVertex->GetEdge() == 1 )
	{
		int nBID = pParentNode->pVertex->GetEdgeBreadthID();
        unsigned long nEdgeRoadSectionID = pParentNode->pVertex->GetEdgeRoadSectionID()&0x7fff;

        if(!LoadBreadthData(nBID,pErrorCode))
        {
            TRACE0("LoadBreadthData() failed when CalculateOptimalPathRut()\n");
            return FALSE;
        }
        
		pRoadSection = GetNetworkRut( nBID, nEdgeRoadSectionID );
		if( pRoadSection == NULL )
        {
            ASSERT(FALSE); 
            return FALSE;
        }
		CGIS_NetworkVertexV7 *pSVtx = GetNetworkVtx( nBID, pRoadSection->GetStartVtxID() );
		CGIS_NetworkVertexV7 *pEVtx = GetNetworkVtx( nBID, pRoadSection->GetEndVtxID() );
//		ASSERT( pSVtx && pEVtx );
		if( pSVtx == NULL || pEVtx == NULL )		
        {
            ASSERT(FALSE);
            return FALSE;
        }
		if( pSVtx->GetEdge() == 1 || pEVtx->GetEdge() == 1 )
		{
			ASSERT( (fabs(pSVtx->GetLog()-pParentNode->pVertex->GetLog())<FLOAT_ZERO &&
				fabs(pSVtx->GetLat()-pParentNode->pVertex->GetLat())<FLOAT_ZERO ) ||
				( fabs(pEVtx->GetLog()-pParentNode->pVertex->GetLog())<FLOAT_ZERO &&
				fabs(pEVtx->GetLat()-pParentNode->pVertex->GetLat())<FLOAT_ZERO ) );
		}
		else
			ASSERT( FALSE );
		pVertex = GetNextRutVtx2( pParentNode->pVertex, pRoadSection, bCFlag );
		if( pVertex == NULL )
		{
			return FALSE;
		}
		if( pRoadSection->GetLevel() < nRLevel )
			return FALSE;

		pChildNode = new CGIS_NetworkNodeV7;
		if( pChildNode == NULL )
			return 0;
		pChildNode->pParent = pParentNode;
		pChildNode->pRoadSection = pRoadSection;
		pChildNode->pVertex = pVertex;
		BOOL bRtn = CalculateNetworkNode( pChildNode, pOpenList, pPathInfo, ptEnd, 1, bCFlag );
        if(bRtn)
        {
            //TRACE3("pChildNode node%d G[%f],F[%f]\n",i,pChildNode->dGValue,pChildNode->dFValue);         
        }
        else
        {
            ASSERT(FALSE);
        }

		if( pSVtx->GetEdge() == 1 )
		{
			if( bCFlag )
				pSVtx->GetVtxSInfo()->bySHead = 1;
			else
				pSVtx->GetVtxSInfo()->bySEnd = 1;
		}
		if( pEVtx->GetEdge() == 1 )
		{
			if( bCFlag )
				pEVtx->GetVtxSInfo()->bySHead = 1;
			else
				pEVtx->GetVtxSInfo()->bySEnd = 1;
		}
        
		return TRUE;
	}


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
	//扩展非边界节点
	int nParent = 0;
	int i;
	for( i=0;	i<pParentNode->pVertex->GetRoadSectionNum();	i++ )
	{
		if( pParentNode->pVertex->GetRoadSectionList()[i] == pParentNode->pRoadSection->GetRutID() )
		{
			nParent++;
			break;
		}
		nParent++;
	}
    int nRoadSectionNum = pParentNode->pVertex->GetRoadSectionNum();
    //TRACE1("根据相邻路段[%d条],扩展出相邻节点\n",nRoadSectionNum);
	int nChild = 0;
	for( i = 0;	i < nRoadSectionNum;	i++ )
	{
        //TRACE1("扩展第%d条\n",i);
		nChild++;
 
        pRoadSection = GetNetworkRut( pParentNode->pVertex->GetBreadthID(), pParentNode->pVertex->GetRoadSectionList()[i] );
		if( pRoadSection == NULL )
        {
            //TRACE0("数据已卸载,不处理\n");
            continue;
        }
		if( pParentNode->pRoadSection->GetRutID() == pRoadSection->GetRutID() && bSFlag == FALSE )
        {
            //TRACE0("来路,不处理\n");
            continue;
        }
		if( pRoadSection->GetLevel() < nRLevel )
        {
            //TRACE0("//等级低于阀值,不处理\n");
            continue;
        }
		pVertex = GetNextRutVtx( pParentNode->pVertex, pRoadSection, bCFlag );
		if( pVertex == NULL )
		{
			//TRACE0("此路段的另一节点限行,不处理\n");
			continue;
		}

        {
            //检查此路段是否被限行
            BOOL bLimit = FALSE;
            for( int j=0;	j < byLRNum; j++ )
            {
                if( pRoadSection->GetRutID() == pLRList[j] )
                {
                    bLimit = TRUE;
                    break;
                }
            }
            if( bLimit )
            {
                //TRACE0("路段被禁行,不处理\n");
                // bsxy continue;
            }
        }



        ////TRACE0("可处理,进行权值计算\n");
		pChildNode = new CGIS_NetworkNodeV7;

		pChildNode->pParent = pParentNode;
		pChildNode->pRoadSection = pRoadSection;
		pChildNode->pVertex = pVertex;
		int nAngle = 0;
		if( bCFlag )
			nAngle = 
            pParentNode->pVertex->GetRoadSectionTurnList()[nParent-1] - 
            pParentNode->pVertex->GetRoadSectionTurnList()[nChild-1];
		else
			nAngle = 
            pParentNode->pVertex->GetRoadSectionTurnList()[nChild-1]  -
            pParentNode->pVertex->GetRoadSectionTurnList()[nParent-1];

        
		BYTE byAngle = 1;
		int nAng = abs( abs(nAngle)-180 );
		//直行
		if( nAng < 8 )
			byAngle = 1;
		else if( nAng < 50 )
			byAngle = 2;
		else if( nAng < 120 )
			byAngle = 3;
		else if( nAng < 160 )
			byAngle = 4;
		else
			byAngle = 5;
			
        BOOL bRtn = CalculateNetworkNode( pChildNode, pOpenList, pPathInfo, ptEnd, byAngle, bCFlag );
//        if(bRtn)
//        {
//            //TRACE3("pChildNode node%d G[%f],F[%f]\n",i,pChildNode->dGValue,pChildNode->dFValue);         
//        }
//        else
//        {
//            ASSERT(FALSE);
//         }
	}



	return TRUE;
}

//判断节点处转弯方向和类型
void CGIS_MBBNetwork::JudgeNodeAngleAndTurn( int nAngle, BYTE &byAngle, BYTE &byTurn )
{
	int nAng = abs( abs(nAngle)-180 );
	//直行
	if( nAng <= 30 )
		byAngle = 1;
	else if( nAng <= 65 )
		byAngle = 2;
	else if( nAng <= 135 )
		byAngle = 3;
	else if( nAng <= 165 )
		byAngle = 4;
	else 
		byAngle = 5;
	//拐弯方向(27．左28．右)
	if( nAngle < 0 )
	{
		if( abs(nAngle) > 180 )
			byTurn = 27;
		else
			byTurn = 28;
	}
	else
	{
		if( nAngle > 180 )
			byTurn = 28;
		else
			byTurn = 27;
	}
	if( nAngle < -1000 )
		byAngle = 6;
}
//判断节点处出入道路路名/入口道路和出口道路是否为同一条道路
	//是否存在与出口道路同名的道路/是否存在与入口道路同名的道路
void CGIS_MBBNetwork::JudgeNodeRutNameInfo(
    CGIS_FeatureRoadSectionV7 *pPreFRut, 
    CGIS_PathpointNode *pPNode, 
    ST_VOICE_NODE *pStVoiceNode )
{
	//路段出入道路路名
//	pVNode->byConfusion = 16;
	pStVoiceNode->bySameName = 12;
	if( pPreFRut == NULL )
	{
		pStVoiceNode->byEnName = 20;
		pStVoiceNode->bySameName = 11;
		pStVoiceNode->bySameRoad = 32;
		pStVoiceNode->byExName = 18;
		return;
	}
	if( pPreFRut->GetAnnotation() && pPNode->GetRutAnno() )
	{
		if( strcmp( pPreFRut->GetAnnotation(), pPNode->GetRutAnno() ) == 0 )
		{
			pStVoiceNode->bySameName = 11;
//			pStVoiceNode->byConfusion = 15;
		}
	}
	else if( pPreFRut->GetAnnotation() == NULL && pPNode->GetRutAnno() == NULL )
		pStVoiceNode->bySameName = 11;
	//入口道路和出口道路是否为同一条道路
	pStVoiceNode->bySameRoad = 32;
	if( pPreFRut->GetRutID() == pPNode->GetNextRutID() )
		pStVoiceNode->bySameRoad = 31;

	///是否存在与出口道路同名的道路
	pStVoiceNode->byExName = 18;
	if( pPreFRut->GetAnnotation() )
	{
		POSITION ps = pPNode->GetNodeRutList()->GetHeadPosition();
		CGIS_NetworkNodeRut *pNNRut;
		while( ps )
		{
			pNNRut = (CGIS_NetworkNodeRut*)pPNode->GetNodeRutList()->GetNext(ps);
			if( pNNRut )
			{
				if( pNNRut->GetRutAnno() )
				{
					if( strcmp( pNNRut->GetRutAnno(), pPreFRut->GetAnnotation() ) == 0 )
					{
						pStVoiceNode->byExName = 17;
//						pStVoiceNode->byConfusion = 15;
						break;
					}
				}
			}
		}
	}
	//是否存在与入口道路同名的道路
	pStVoiceNode->byEnName = 20;
	if( pPNode->GetRutAnno() )
	{
		POSITION ps = pPNode->GetNodeRutList()->GetHeadPosition();
		CGIS_NetworkNodeRut *pNNRut;
		while( ps )
		{
			pNNRut = (CGIS_NetworkNodeRut*)pPNode->GetNodeRutList()->GetNext(ps);
			if( pNNRut )
			{
				if( pNNRut->GetRutAnno() )
				{
					if( strcmp( pNNRut->GetRutAnno(), pPNode->GetRutAnno() ) == 0 )
					{
						pStVoiceNode->byEnName = 19;
//						pStVoiceNode->byConfusion = 15;
						break;
					}
				}
			}
		}
	}
}

BOOL CGIS_MBBNetwork::FullVoiceNodeInfo( 
    CGIS_FeatureRoadSectionV7 *pPreFRut, 
    CGIS_PathpointNode *pPNode, 
    ST_VOICE_NODE *pStVoiceNode )
{
/*	BYTE byOtherStreet;	//是否需要除出入口道路外的其他道路（21．是22．否)
	BYTE byIMI;			//是否存在IMI(23．是24．否)
//	BYTE byDestination;	//是否导到目的地（25．是26．否)
	BYTE byRound;		//入口道路与出口道路之间是否有环岛(29．是30．否)
*/
	pStVoiceNode->byDestination = 26;
	JudgeNodeAngleAndTurn( pPNode->GetTurnAngle(),pStVoiceNode->byAngle, pStVoiceNode->byTurn );
	JudgeNodeRutNameInfo( pPreFRut, pPNode, pStVoiceNode );
	pStVoiceNode->byOtherStreet = 21;
	pStVoiceNode->byIMI = 24;
	pStVoiceNode->byRound = 30;
	return TRUE;
}
BOOL CGIS_MBBNetwork::InitVNodeInfo( ST_VOICE_NODE *pStVoiceNode )
{
	pStVoiceNode->byAngle = 1;		    //拐弯角度类型（1.直行 2.缓拐3.拐弯4.急拐5.U型拐）
	pStVoiceNode->bySameName = 11;	    //出入道路路名是否一致（11．是12．否)
	pStVoiceNode->byConfusion = 16;	    //是否存在混乱路口(15.左16.否37.右38.中)
	pStVoiceNode->byConfusion2 = 16;	//是否存在混乱路口(15.左16.否37.右38.中)
	pStVoiceNode->byOtherStreet = 21;	//是否需要除出入口道路外的其他道路（21．是22．否)
	pStVoiceNode->byIMI = 24;			//是否存在IMI(23．是24．否)
	pStVoiceNode->byDestination = 25;	//是否导到目的地（25．是26．否)
	pStVoiceNode->byTurn = 0;	    	//拐弯方向(27．左28．右)
	pStVoiceNode->byRound = 30;		    //入口道路与出口道路之间是否有环岛(29．是30．否)
	pStVoiceNode->bySameRoad = 31;	    //入口道路和出口道路是否为同一条道路（31．是32．否)
	pStVoiceNode->byExName = 18;		//是否存在与出口道路同名的道路（除入口道路外）(RUT)17． 是18． 否
	pStVoiceNode->byEnName = 20;		//是否存在与入口道路同名的道路（除出口道路外）(RUT)19． 是20． 否
	return TRUE;
}
/**
 * @brief 根据下一路段填充语音信息
 *
 * @param 
 * @return
*/
BOOL CGIS_MBBNetwork::FullVoiceRutInfo( CGIS_NetworkRoadSectionV7 *pRoadSection, ST_VOICE_RUT *pVRut )
{
    ASSERT(pRoadSection);
	if( pRoadSection == NULL )
		return FALSE;
	//设置路段类型
//	if( pRoadSection->GetType() == 0 || pRoadSection->GetType() == 1 )
	if( pRoadSection->GetType() == 0  )
		pVRut->byType = 6;
	else
		pVRut->byType = 8;

    
	if( pRoadSection->GetRutType() == 3 || pRoadSection->GetRutType() == 5 )
		pVRut->byType = 7;
	else if( pRoadSection->GetRutType() == 15 )
		pVRut->byType = 33;
	else if( pRoadSection->GetRutType() == 14 )
		pVRut->byType = 34;
	///路段的附加信息
	if( pRoadSection->GetRutType() == 0 )
		pVRut->byAdd = 9;
	else
		pVRut->byAdd = 10;
	///路段是否可驾车行驶
	pVRut->byDrivable = 13;
	///路段是否为收费路段
	if( pRoadSection->GetRutCharge() == 0 )
		pVRut->byCharge = 36;
	else
		pVRut->byCharge = 35;
	return TRUE;
}

//加载图幅中的空间属性数据
int CGIS_MBBNetwork::LoadSpaceBreadthData(char* pBufOfBeforeCompress,int nBufferSize,int nBreadthID,CGIS_FeatureRoadSectionV7 **pFRList, int &nFNum )
{
    ASSERT(m_pBeforeCompressBuffer);
    ASSERT(m_pAfterCompressBuffer);

//////////////////////////////////////////////////////////////////////////
	CFile aFile(g_hSpaceFile);
    CFile* pFile = &aFile;
    StuSpaceFileStructV7 :: StuIndexInfoOfBreadth* pIndex = &g_pIndexInfoArrayOfBreadthInSpaceMap[nBreadthID];
	ASSERT(pIndex);
    ASSERTVALID(pIndex);
	int nBeforeCompressSize = pIndex->nBeforeCompressSize;
	int nAfterCompressSize = pIndex->nAfterCompressSize;

    //分配压缩前空间
    ASSERT(nBeforeCompressSize <= nBufferSize);
	ASSERT(pBufOfBeforeCompress);
	 
	//-------------------------------------------------
	//读取并解压数据
	{
		char *pBufOfAfterCompress = m_pAfterCompressBuffer;//分配压缩后
		ASSERT(pBufOfAfterCompress);
		
        pFile->Seek(pIndex->nOffset+g_dwOffsetBaseOfMapDataInSpaceMap,CFile::begin);
		pFile->Read( pBufOfAfterCompress, nAfterCompressSize );
		DWORD dwSize = nBeforeCompressSize;
		int nRet = uncompress( (unsigned char*)pBufOfBeforeCompress, &dwSize, (unsigned char*)pBufOfAfterCompress, nAfterCompressSize  );
		ASSERT(nRet == Z_OK);
		ASSERT(nBeforeCompressSize == dwSize);
	}
	//-----------------------------------------------------
	
	{	
		char* pCharAddr		= NULL;
		char* pShortAddr	= NULL;
		char* pDwordAddr	= NULL;
		
		StuSpaceFileStructV7::StuBreadthHeaderInfo* pHeader = (StuSpaceFileStructV7::StuBreadthHeaderInfo*)pBufOfBeforeCompress;
       
        DWORD dwBuddyDataSize = pHeader->nCharDataBuddyLength+pHeader->nShortDataBuddyLength+pHeader->nDwordDataBuddyLength;
        ASSERTVALID(pHeader);

        unsigned long nFtrSize = sizeof(StuSpaceFileStructV7::StuGisFeatureRoadSection);
        unsigned long nBuddyOffset = sizeof(StuSpaceFileStructV7::StuBreadthHeaderInfo) + pHeader->nFeatureCount*nFtrSize;
        char* pBuddyData = pBufOfBeforeCompress+nBuddyOffset;

		if(dwBuddyDataSize > 0)
		{
			pCharAddr	= pBuddyData;
			pShortAddr	= pCharAddr + pHeader->nCharDataBuddyLength;
			pDwordAddr	= pShortAddr+ pHeader->nShortDataBuddyLength;
			if(	*( (DWORD*)pCharAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)	)
			{
				pCharAddr	+= 4;	
			}
			//if( *( (DWORD*)pShortAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0))
			{
				//pShortAddr	+= 4;
			}
			if( *( (DWORD*)pDwordAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0))
			{
				pDwordAddr	+= 4;
			}
		}
		
        StuSpaceFileStructV7::StuGisFeatureRoadSection* pArrayOfStGisFeatureRoadSection = 
            (StuSpaceFileStructV7::StuGisFeatureRoadSection*)(pBufOfBeforeCompress+sizeof(StuSpaceFileStructV7::StuBreadthHeaderInfo));
        
        nFNum = pHeader->nFeatureCount;
       for(int i = 0; i < pHeader->nFeatureCount; i++)
        {
            CGIS_FeatureRoadSectionV7* pFtr = pFRList[i];
            ASSERT(pFtr);
            pFtr->m_pStData = &pArrayOfStGisFeatureRoadSection[i];
            char* pCharAddrOld = pCharAddr;
            char* pShortAddrOld = pShortAddr;
            char* pDwordAddrOld = pDwordAddr;
            pFtr->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
            ASSERTVALID(pFtr);
         
        }
    }
		
    return 0;
}

int CGIS_MBBNetwork::FreeSpaceBreadthData(int nBreadthID,CGIS_FeatureRoadSectionV7 **pFRList, int &nFNum )
{
    return 1;
}


/**
 * @brief 填充指定路段的空间数据到路径点列表中.
 *
 * @param 
 * @return
*/
BOOL CGIS_MBBNetwork::FullPathNodeArc(
    CGIS_Pathpoint *pPPoint, 
    CGIS_FeatureRoadSectionV7 *pFtrRoadSection, //携带空间数据的路段
    ST_GEO_PTXY ptEnd,                          //[nouse]
    BOOL bSequence,                             //[in]TRUE-正向填充 FALSE-反向填充
    CPtrList *pPathpointList, 
    POSITION &pos )
{
	ST_GEO_PTXY pt;
	int nPtNum = pFtrRoadSection->GetPtNum();

	for( int i = 1;	i < nPtNum-1; i++ )
	{
		if( bSequence )
			pt = ((ST_GEO_PTXY*)pFtrRoadSection->GetPtList())[i];
		else
			pt = ((ST_GEO_PTXY*)pFtrRoadSection->GetPtList())[nPtNum-1-i];

        
		CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
		pPathPt->SetPathpointPosition( pt );
		pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
		pos = pPathpointList->InsertAfter( pos, pPathPt );
	}

    //////////////////////////////////////////////////////////////////////////
    //重新修正前点的坐标
	if( bSequence && pPPoint )
    {
        pPPoint->SetPathpointPosition( ((ST_GEO_PTXY*)pFtrRoadSection->GetPtList())[0] );
    }
	else if( pPPoint )
    {
        pPPoint->SetPathpointPosition( ((ST_GEO_PTXY*)pFtrRoadSection->GetPtList())[pFtrRoadSection->GetPtNum()-1] );
    }
	return TRUE;
}
/**
 * @brief 填充非边缘NODE,既要填充空间数据,又要填充语音数据
 *
 * @param 
 * @return
*/
BOOL CGIS_MBBNetwork::FullPathNode(
    CGIS_FeatureRoadSectionV7 **        pFRList, 
    CGIS_NetworkVertexV7 *              pVertex, 
    CGIS_NetworkRoadSectionV7 *         pRoadSectionPrevious,
    CGIS_NetworkRoadSectionV7 *         pRoadSectionNext, 
    CPtrList *                          pPathpointList,
    POSITION &                          pos //[out]
    )
{
    ASSERT(pFRList);
	CGIS_Pathpoint *pPPoint = NULL;
	CGIS_FeatureRoadSectionV7 *pFtrRoadSection = NULL;
	BYTE byRAType = 1;

	m_bTFlay = TRUE;
	ST_GEO_PTXY ptPos;
	ptPos.dx = pVertex->GetLog();
	ptPos.dy = pVertex->GetLat();


    //////////////////////////////////////////////////////////////////////////
    //填充语音信息

    //
    /**
     * @brief 构造机动的条件
     *
     * 1-前一条路段是交叉路口内路段且前一条路段为第一条路段(m_pVoiceNode==NULL)
     * 2-前一条路段是交叉路口内路段且后一条路段为环岛
     * 3-第一条路段不是交叉路口内路段
    */
	if( 
        pRoadSectionPrevious->GetRutType() != 4 
        || 
        (
            pRoadSectionPrevious->GetRutType()==4 
            && 
            (pRoadSectionNext->GetRutType()==0||m_pVoiceNode==NULL)
        ) 
        )
	{
		char* szAnnotationPrevious = NULL;
		char* szAnnotationNext = NULL;
        
        if(pRoadSectionPrevious->GetRutID() != 0xffff)
        {
            if( pFRList[pRoadSectionPrevious->GetRutID()] )
                if( pFRList[pRoadSectionPrevious->GetRutID()]->GetAnnotation() )
                    szAnnotationPrevious = pFRList[pRoadSectionPrevious->GetRutID()]->GetAnnotation();
        }
        if(pRoadSectionNext->GetRutID() != 0xffff)
        {
            if( pFRList[pRoadSectionNext->GetRutID()] )
                if( pFRList[pRoadSectionNext->GetRutID()]->GetAnnotation() )
                    szAnnotationNext = pFRList[pRoadSectionNext->GetRutID()]->GetAnnotation();
        }

        if(!szAnnotationPrevious)szAnnotationPrevious = "";
        if(!szAnnotationNext)szAnnotationNext = "";
        BOOL bSameRoadSectionName = !strcmp(szAnnotationPrevious,szAnnotationNext);

        //此节点的路段数大于2或两条路的名字不同
        int nTotalVertexRoadSectionNum = pVertex->GetRoadSectionNum() + pVertex->GetLowLayerRoadSectionNum();
		if( nTotalVertexRoadSectionNum > 2 || !bSameRoadSectionName  )
		{
			if( m_pVoiceNode )
			{
                //路口内道路直接与环岛路段相连，属于机动,要对上一次做出的机动做一些修正
				if( pRoadSectionPrevious->GetRutType()==4 && pRoadSectionNext->GetRutType()==0 )
				{
                    //根据下一路段填充和路段相关的语音信息
					FullVoiceRutInfo( pRoadSectionNext, m_pVoiceNode->GetVRutInfo() );
					m_pVoiceNode->GetVRutInfo()->byAdd = 10;
					m_pVoiceNode->SetTurnAngle( m_nAPre-m_nANext );
                    //根据前一路段填充和节点相关的语音信息.
					FullVoiceNodeInfo( m_pFRPre, m_pVoiceNode, m_pVoiceNode->GetVNodeInfo() );
					m_pVoiceNode = NULL;
				}
			}


            BOOL bCreateNode = TRUE;
#ifdef _DEBUG
//            if( (pVertex->GetLog() -0 ) < FLOAT_DETA_XY
//                && 
//                (pVertex->GetLat() - 0) <FLOAT_DETA_XY
//                )
//            {
//                ASSERT(FALSE);
//             }
#endif

            //////////////////////////////////////////////////////////////////////////
#if 1         
            //20070615-消除"卜"形路口的NODE
            //此段代码用于对连接三条0层路段的节点的处理(主要是针对城际高速)
            {
                int nRoadSectionNum = pVertex->GetRoadSectionNum();
                CGIS_NetworkRoadSectionV7* pBranchRoadSection = NULL;
                if(
                    nRoadSectionNum == 3 
                    &&
                    pRoadSectionPrevious->GetLevel() == 5 
                    && 
                    pRoadSectionNext->GetLevel() == 5)
                {                         
                    for(int i = 0; i < nRoadSectionNum; i++)
                    {
                        unsigned short nRoadSectionID = pVertex->GetRoadSectionList()[i];
                        if(nRoadSectionID != pRoadSectionPrevious->GetRutID() && nRoadSectionID != pRoadSectionNext->GetRutID())
                        {
                            pBranchRoadSection = GetNetworkRut( pVertex->GetBreadthID(), nRoadSectionID);
                        }                    
                    }
                   
                    ASSERT(pBranchRoadSection);
                    if(pBranchRoadSection)
                    {
#ifdef _DEBUG                    
                        if(pBranchRoadSection->GetLevel() != 5)
                        {
                            TRACE2("Trunk BranchRoadSection's level != 5,log:%.6f,lat:%.6f\n",
                                pVertex->GetLog(),pVertex->GetLat());
                        }
#endif
                        BYTE byNextRoadSectionType = pRoadSectionNext->GetRutType();
                        if(
                            byNextRoadSectionType != 5 
                            &&
                            byNextRoadSectionType != 3
                            &&
                            byNextRoadSectionType != 6
                            &&
                            byNextRoadSectionType != 7
                            )
                        {
                            if(
                                pBranchRoadSection->GetRutType() == 5
                                ||
                                pBranchRoadSection->GetRutType() == 6
                                ||
                                pBranchRoadSection->GetRutType() == 7
                                )
                            {
                                //////////////////////////////////////////////////////////////////////////
                                //070711 只消除驶出路段位于旁支路段的左侧的"卜"形路口
                                short nAngleEnter   = -1;
                                short nAngleExit    = -1;
                                short nAngleBranch  = -1;
                                CGIS_NetworkRut* pRoadSectionBranch = NULL;
                                for(int i = 0; i < 3; i++)
                                {
                                    if(pVertex->GetRoadSectionList()[i] == pRoadSectionPrevious->GetRutID())
                                    { 
                                        nAngleEnter = pVertex->GetRoadSectionTurnList()[i];
                                        continue;
                                    }
                                    
                                    if(pVertex->GetRoadSectionList()[i] == pRoadSectionNext->GetRutID())
                                    {
                                        nAngleExit = pVertex->GetRoadSectionTurnList()[i];
                                        continue;
                                    }    
                                    
                                    nAngleBranch = pVertex->GetRoadSectionTurnList()[i];
                                }
                                ASSERT(nAngleEnter != -1);
                                ASSERT(nAngleExit  != -1);
                                ASSERT(nAngleBranch  != -1);

                                //----注意驶出路段位于旁支路段的左侧,即时不做语音提示.
                                //

                                int nAngleDiff = nAngleExit - nAngleBranch;
                                if( nAngleDiff < 0 )
                                {
                                    if( abs(nAngleDiff) > 180 )
                                    {
                                        //匝道在右侧消除语音
                                        bCreateNode = FALSE;
                                    }
                                    else
                                    {
                                        //匝道在左侧保留语音
                                    }
                                }
                                else
                                {
                                    if( nAngleDiff > 180 )
                                    {
                                        //匝道在左侧保留语音
                                    }
                                    else
                                    {
                                        //匝道在右侧消除语音
                                        bCreateNode = FALSE;
                                    }
                                }



                                //////////////////////////////////////////////////////////////////////////
                            }
                            
                        }
                        
                    }
                }
                
                
            }

#endif 
            //////////////////////////////////////////////////////////////////////////



            //////////////////////////////////////////////////////////////////////////
            
            //////////////////////////////////////////////////////////////////////////
#if 1         
            //20070615-消除"卜"形路口的NODE
            //此段代码用于对两条0层路网路段和一条1层路网路网路段的处理(主要是针对市内高速)
            {
                int nRoadSectionNum = pVertex->GetRoadSectionNum();
                if(
                    nRoadSectionNum == 2 
                    &&
                    pRoadSectionPrevious->GetLevel() == 5 
                    && 
                    pRoadSectionNext->GetLevel() == 5
                    &&
                    pVertex->GetLowLayerRoadSectionNum() == 1
                    )
                {                         

                    //////////////////////////////////////////////////////////////////////////
                    //070711 只消除驶出路段位于旁支路段的左侧的"卜"形路口
                    short nAngleEnter   = -1;
                    short nAngleExit    = -1;
                    short nAngleBranch  = -1;
                        for(int i = 0; i < 2; i++)
                        {
                            if(pVertex->GetRoadSectionList()[i] == pRoadSectionPrevious->GetRutID())
                            { 
                                nAngleEnter = pVertex->GetRoadSectionTurnList()[i];
                                continue;
                            }
                            
                            if(pVertex->GetRoadSectionList()[i] == pRoadSectionNext->GetRutID())
                            {
                                nAngleExit = pVertex->GetRoadSectionTurnList()[i];
                                continue;
                            }    
                            
                        }
                        nAngleBranch = pVertex->GetLowLayerRoadSectionTurnList()[0];
                    
                     
                    ASSERT(nAngleEnter != -1);
                    ASSERT(nAngleExit  != -1);
                    ASSERT(nAngleBranch  != -1);
                    
                    //----注意驶出路段位于旁支路段的左侧,即时不做语音提示.
                    //
                    
                    int nAngleDiff = nAngleExit - nAngleBranch;
                    if( nAngleDiff < 0 )
                    {
                        if( abs(nAngleDiff) > 180 )
                        {
                            //匝道在右侧消除语音
                            bCreateNode = FALSE;
                        }
                        else
                        {
                            //匝道在左侧保留语音
                        }
                    }
                    else
                    {
                        if( nAngleDiff > 180 )
                        {
                            //匝道在左侧保留语音
                        }
                        else
                        {
                            //匝道在右侧消除语音
                            bCreateNode = FALSE;
                        }
                    }
                    
                    
                    
                    //////////////////////////////////////////////////////////////////////////
                }
            }

#endif 
            //////////////////////////////////////////////////////////////////////////

            if(bCreateNode)
            {
            //正常构造一个机动点
			m_pVoiceNode = NULL;
			m_pVoiceNode = new CGIS_PathpointNode;
			pPPoint = m_pVoiceNode;
			m_pVoiceNode->SetPathpointType( EN_PATHPOINTTYPE_NODE );
			m_pVoiceNode->SetPathpointPosition( ptPos );
			m_pVoiceNode->SetNodeVtxID( pVertex->GetVtxID() );
			m_pVoiceNode->SetPBreadthID( 0 );
			pos = pPathpointList->InsertAfter( pos, m_pVoiceNode );

            
			int nVRNum = pVertex->GetRoadSectionNum();
			ASSERT( nVRNum > 0 );
			CGIS_NetworkRoadSectionV7 *pNRut = NULL;

            //判环岛
			if( pRoadSectionPrevious->GetRutType()==0 || pRoadSectionNext->GetRutType()==0 )
			{
				if( m_bRoundaboutFlag == FALSE )
                {
                    m_nRoundaboutOrder = 0;

                }

                //填充方式（用于路口扩大图）1：普通路段　2：十字路口　3：环岛
				byRAType = 3;
			}
            //遍历当前节点的所有路段,
			for( int i=0;	i<nVRNum;	i++ )
			{
				if( pVertex->GetRoadSectionList()[i] == pRoadSectionPrevious->GetRutID() )
                {
                    m_nAPre = pVertex->GetRoadSectionTurnList()[i];
                }
				else
				{
					if( pVertex->GetRoadSectionList()[i] == pRoadSectionNext->GetRutID() )
                    {
                        m_nANext = pVertex->GetRoadSectionTurnList()[i];
                    }
                    
                    
					pNRut = GetNetworkRut( pVertex->GetBreadthID(), pVertex->GetRoadSectionList()[i] );
					if( pNRut )
					{
						if( pNRut->GetRutType() == 4 && byRAType != 3 )
							byRAType = 2;
						else if( pNRut->GetRutType() == 0 )
							byRAType = 3;
                        
						if( byRAType == 3 )
						{
							if( (pNRut!=pRoadSectionNext || pRoadSectionNext->GetRutType()!=0) 
                                && 
                                pNRut->GetRutType() != 0 
                                && 
								GetNextRutVtx(pVertex, pNRut,TRUE) != NULL )
                                
								m_nRoundaboutOrder++;
						}
					}
				}
			}
			if( byRAType == 3 )
            {
                //要说明是第几个环岛出口
                m_pVoiceNode->SetRMOrder( m_nRoundaboutOrder );
            }


			if( !m_bRoundaboutFlag)
			{
                //如果是普通路口,这个地方会进入,进行路口路段的填充
                //如果是环岛路段,这个地方不会进入,直到驶出环岛,也就是环岛接一个普通路段时,才把整个环岛做为一个路口来填充
				FullPathNodeRuts( m_pVoiceNode, pFRList, pVertex, 
                    pRoadSectionPrevious, pRoadSectionNext, byRAType );

                
				if( byRAType == 3 )
					m_nAPre = -1800;
			}


			if( pRoadSectionNext->GetRutType() == 0 )
				m_bRoundaboutFlag = TRUE;
			else 
				m_bRoundaboutFlag = FALSE;

        }
            else
            {
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( ptPos );
			pPPoint = pPathPt;
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathpointList->InsertAfter( pos, pPathPt );

            }

		}
		else//路段数<2且名字相同则放个拐点即可
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( ptPos );
			pPPoint = pPathPt;
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathpointList->InsertAfter( pos, pPathPt );
		}
	}
	else
	{
		CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
		pPPoint = pPathPt;
		pPathPt->SetPathpointPosition( ptPos );
		pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
		pos = pPathpointList->InsertAfter( pos, pPathPt );
	}

    //////////////////////////////////////////////////////////////////////////
    //填充空间部分的数据
    if(pRoadSectionNext->GetRutID() != 0xffff)
    {
        pFtrRoadSection = pFRList[pRoadSectionNext->GetRutID()];
    }
	if( pRoadSectionNext->GetRutType() != 4 
        || 
        (byRAType != 2 && pRoadSectionPrevious->GetRutType() != 4)
        )
	{
		if( m_pVoiceNode )
		{
			int nVRNum = pVertex->GetRoadSectionNum();
			int nAngNextRut = 0;
			ASSERT( nVRNum > 0 );
			for( int i=0;	i<nVRNum;	i++ )
			{
				if( pVertex->GetRoadSectionList()[i] == pRoadSectionNext->GetRutID() )
				{
					nAngNextRut = pVertex->GetRoadSectionTurnList()[i];
					break;
				}
			}
            //当前语音点中写入下一路段ID
			m_pVoiceNode->SetNextRutID( pRoadSectionNext->GetRutID() );
            
			if( pFtrRoadSection )
			{
				if( pFtrRoadSection->GetAnnotation() != NULL )
				{
                    if(m_pVoiceNode->GetRutAnno())
                    {
                        delete m_pVoiceNode->GetRutAnno();
                    }
					char* szAnno = new char[pFtrRoadSection->GetAnnoNum()];
					memcpy( szAnno, pFtrRoadSection->GetAnnotation(), pFtrRoadSection->GetAnnoNum() );
                    //当前语音点中写入下一路段的名字
					m_pVoiceNode->SetRutAnno( szAnno );
					m_pVoiceNode->SetRutAnnoNum( pFtrRoadSection->GetAnnoNum() );
				}
			}
            else
            {
                TRACE0("error.当前路段没有匹配到全国路网中路段造成NODE的名字缺失\n");
            }
            
			m_pVoiceNode->SetRType( pRoadSectionNext->GetType() );
			m_pVoiceNode->SetRutType( pRoadSectionNext->GetRutType() );
			FullVoiceRutInfo( pRoadSectionNext, m_pVoiceNode->GetVRutInfo() );
			if( m_bTFlay )//连续机动
            {
                m_pVoiceNode->SetTurnAngle( m_nAPre-nAngNextRut );
            }
			else
            {
                m_pVoiceNode->SetTurnAngle( 180 );
            }

			FullVoiceNodeInfo( m_pFRPre, m_pVoiceNode, m_pVoiceNode->GetVNodeInfo() );
			if( m_pVoiceNode->GetVNodeInfo()->byAngle == 5 && m_byMRNum > 0 )
            {
                m_pVoiceNode->GetVNodeInfo()->byIMI = 23;
            }

            
            //////////////////////////////////////////////////////////////////////////
            //中间数据重置,路段数据更新为当前
			m_pVoiceNode = NULL;
			m_byMRNum = 0;
			if( m_pFRPre )
            {
                StuSpaceFileStructV7::StuGisFeatureRoadSection* pStData =  m_pFRPre->GetDataAddr();
                ASSERT(pStData);
                delete pStData;
                char* pszAnno = m_pFRPre->GetAnnotation();
                if(pszAnno)
                {
                    delete pszAnno;
                }
                ST_GEO_PTXY* pPtList = m_pFRPre->GetPtList();
                ASSERT(pPtList);
                
                delete []pPtList;
                delete m_pFRPre;

            }

			m_pFRPre = NULL;
			if( pFtrRoadSection )
            {
                m_pFRPre = (CGIS_FeatureRoadSectionV7*)pFtrRoadSection->Clone();
            }
		}
	}
	else
    {
        //当前节点的前一路段为路口内路段,后一路段为路口内路段
        m_byMRNum++;
    }

    ASSERT(pFtrRoadSection);
	if( pFtrRoadSection == NULL )
		return FALSE;

    //////////////////////////////////////////////////////////////////////////
    //正常填充好空间数据.其实填充空间数据只需要下一路段的信息即可
	if( fabs(pFtrRoadSection->GetPtList()[0].dx-pVertex->GetLog())<FLOAT_DETA_XY &&
			fabs(pFtrRoadSection->GetPtList()[0].dy-pVertex->GetLat())<FLOAT_DETA_XY )
    {
            FullPathNodeArc( pPPoint, pFtrRoadSection, ptPos,TRUE, pPathpointList, pos );
    }
	else if( fabs(pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-1].dx-pVertex->GetLog())<FLOAT_DETA_XY &&
			fabs(pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-1].dy-pVertex->GetLat())<FLOAT_DETA_XY )
    {
        FullPathNodeArc( pPPoint, pFtrRoadSection, ptPos, FALSE, pPathpointList, pos );
    }
	else
	{
		double dbDis1 = CalcDist(pFtrRoadSection->GetPtList()[0].dx,pFtrRoadSection->GetPtList()[0].dy,pVertex->GetLog(),pVertex->GetLat() );
		double dbDis2 = CalcDist(pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-1].dx,pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-1].dy,
							pVertex->GetLog(),pVertex->GetLat() );
		if( dbDis1 < dbDis2 )
			FullPathNodeArc( pPPoint, pFtrRoadSection, ptPos, TRUE, pPathpointList, pos );
		else
			FullPathNodeArc( pPPoint, pFtrRoadSection, ptPos, FALSE, pPathpointList, pos );
	}
	return TRUE;
}

//填充路段上的数据
void CGIS_MBBNetwork::GetRutData( 
    CGIS_NetworkVertexV7*          pVertex,
    CGIS_NetworkRoadSectionV7*     pRoadSectionPrevious, 
    CGIS_NetworkRoadSectionV7*     pRoadSectionNext,
    CGIS_FeatureRoadSectionV7**    pFRList,
    CPtrList *pPMid,//[out]
    POSITION &pos )//[out]
{
	///处理数据的收集整理
	if( pVertex->GetEdge() == 1 )
	{
        //////////////////////////////////////////////////////////////////////////
        //对于边界节点构造为一个拐点
		CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
		ST_GEO_PTXY ptPos;
		ptPos.dx = pVertex->GetLog();
		ptPos.dy = pVertex->GetLat();
		pPathPt->SetPathpointPosition( ptPos );
		pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );


        //把点挂到路径点表上(注意,如果pos为空则会挂到尾部)
		pos = pPMid->InsertAfter( pos, pPathPt );

        //////////////////////////////////////////////////////////////////////////
        //把下一路段拉出来构造为拐点

        
        CGIS_FeatureRoadSectionV7 *pFtrRoadSection = NULL;
        if(pRoadSectionNext->GetRutID() != 0xffff)
        {
            pFtrRoadSection = pFRList[pRoadSectionNext->GetRutID()];
        }
        //ASSERT(pFtrRoadSection);

		if( pFtrRoadSection )
		{
			if( pFtrRoadSection->GetPtNum() == 2 )
            {
                return;//两个点无需填充
            }

			if(     fabs(pFtrRoadSection->GetPtList()[0].dx - pVertex->GetLog()) < FLOAT_DETA_XY &&
					fabs(pFtrRoadSection->GetPtList()[0].dy - pVertex->GetLat()) < FLOAT_DETA_XY )
            {
                //正向填充路段到航线.修正作为边缘节点的拐点的数据.(注意路段的首尾两个点是不填的)
                FullPathNodeArc( pPathPt, pFtrRoadSection, ptPos, TRUE, pPMid, pos );
            }
			else if(    fabs(pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-1].dx-pVertex->GetLog())<FLOAT_DETA_XY &&
					    fabs(pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-1].dy-pVertex->GetLat())<FLOAT_DETA_XY )
            {
                //反向填充路段到航线.修正作为边缘节点的拐点的数据.(注意路段的首尾两个点是不填的)
				FullPathNodeArc( pPathPt, pFtrRoadSection, ptPos, FALSE, pPMid, pos );
            }
			else
			{
				double dbDis1 = CalcDist( pVertex->GetLog(), pVertex->GetLat(),
                    pFtrRoadSection->GetPtList()[1].dx, pFtrRoadSection->GetPtList()[1].dy );
				double dbDis2 = CalcDist( pVertex->GetLog(), pVertex->GetLat(), 
					pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-2].dx, pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-2].dy );
                
				if( dbDis1 > dbDis2 )
					FullPathNodeArc( pPathPt, pFtrRoadSection, ptPos, FALSE, pPMid, pos );
				else 
					FullPathNodeArc( pPathPt, pFtrRoadSection, ptPos, TRUE, pPMid, pos );
			}
		}       
	}
    else
    {
        ///非边界节点的处理,则路段就在本数据中.
        FullPathNode( pFRList, pVertex, pRoadSectionPrevious, pRoadSectionNext, pPMid, pos );
    }
}
//填充路段上的数据
void CGIS_MBBNetwork::GetRutDataV2( 
    CGIS_NetworkVertexV7*          pVertex,
    CGIS_NetworkRoadSectionV7*     pRoadSectionPrevious, 
    CGIS_NetworkRoadSectionV7*     pRoadSectionNext,
    CGIS_FeatureRoadSectionV7**    pFRList,
    CPtrList *pPMid,//[out]
    POSITION &pos )//[out]
{
	///处理数据的收集整理
	ASSERT( pVertex->GetEdge() != 1 );
    ///非边界节点的处理,则路段就在本数据中.
    FullPathNodeV2( pFRList, pVertex, pRoadSectionPrevious, pRoadSectionNext, pPMid, pos );   
}


static char g_pSpaceBeforeCompressBuffer[400000]; 

/////填充路径的数据
int CGIS_MBBNetwork::FullPathMData(
    CPtrList *pRutList,     //[in]
    CPtrList *pVertexList,  //[in]
    CPtrList *pPMid)        //[out]传出填充好的数据
{
    TRACK_FUNCTION("CGIS_MBBNetwork::FullPathMData");

	CGIS_NetworkRoadSectionV7*  pHeadRoadSection        = NULL;//尾路段只写机动,不填空间数据
	CGIS_NetworkRoadSectionV7*  pTailRoadSection        = NULL;//尾路段只写机动,不填空间数据
	CGIS_NetworkRoadSectionV7*  pRoadSectionPrevious    = NULL;
	CGIS_NetworkRoadSectionV7*  pRoadSection            = NULL;
	CGIS_NetworkVertexV7*       pVertex                 = NULL;


    pHeadRoadSection = (CGIS_NetworkRoadSectionV7*)pRutList->GetHead();
    pTailRoadSection = (CGIS_NetworkRoadSectionV7*)pRutList->GetTail();
    pRutList->RemoveHead();
    pRutList->RemoveTail();


	POSITION posOfCurrentRoadSection   = pRutList->GetHeadPosition();
	POSITION posOfCurrentVertex        = pVertexList->GetHeadPosition();

    //////////////////////////////////////////////////////////////////////////
    //准备缓冲区    
	CGIS_FeatureRoadSectionV7** pFRList = m_pFRList;
    CGIS_FeatureRoadSectionV7* pFtrRoadSectionArray = m_pFtrRoadSectionArray;

    int nBufferSize     = m_nSizeOfBeforeCompressBuffer;
    char* pBuffer       = m_pBeforeCompressBuffer;

    for(int i = 0; i < 1000; i++)
    {
        pFRList[i] = &pFtrRoadSectionArray[i];
    }
    //////////////////////////////////////////////////////////////////////////


	int nFNum = 0;
	POSITION pos = NULL;
	pVertex = (CGIS_NetworkVertexV7*)pVertexList->GetAt(posOfCurrentVertex);
    //取第一个节点.
	LoadSpaceBreadthData(pBuffer,nBufferSize, pVertex->GetBreadthID(), pFRList, nFNum);
    pRoadSectionPrevious = pHeadRoadSection;



    //遍历路段,加载路段,把路段携带的空间数据填充到航线中去
	while(posOfCurrentRoadSection)
	{
		pRoadSection = (CGIS_NetworkRoadSectionV7*)pRutList->GetNext(posOfCurrentRoadSection);
		pVertex = (CGIS_NetworkVertexV7*)pVertexList->GetNext(posOfCurrentVertex);
		if( pVertex->GetEdge() == 1 )
        {
            FreeSpaceBreadthData(0,pFRList, nFNum);
            memset(pFtrRoadSectionArray,0,sizeof(CGIS_FeatureRoadSectionV7)*1000);
            LoadSpaceBreadthData(pBuffer,nBufferSize,pRoadSection->GetBreadthID(), pFRList, nFNum);
        }

        //TRACE3("Rut(%d)--@(%d)--Rut(%d)\n",pRoadSectionPrevious->GetRutID(),pVertex->GetVtxID(),pRoadSection->GetRutID());
        //TRACE_SLEEP_BY_TIMES(10);
		GetRutData( pVertex, pRoadSectionPrevious, pRoadSection, pFRList, pPMid, pos );
		pRoadSectionPrevious = pRoadSection;
	}

    pRoadSection = pTailRoadSection;
    pVertex = (CGIS_NetworkVertexV7*)pVertexList->GetNext(posOfCurrentVertex);
    if( pVertex->GetEdge() == 1 )
    {
        FreeSpaceBreadthData(0,pFRList, nFNum);
        memset(pFtrRoadSectionArray,0,sizeof(CGIS_FeatureRoadSectionV7)*1000);
        LoadSpaceBreadthData(pBuffer,nBufferSize,pRoadSection->GetBreadthID(), pFRList, nFNum);
    }

    GetRutDataV2( pVertex, pRoadSectionPrevious, pRoadSection, pFRList, pPMid, pos );
    FreeSpaceBreadthData(0,pFRList, nFNum);

    //////////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////////
    
	return 0;
}
BOOL CGIS_MBBNetwork::CheckCalculateCondition(    
    CGIS_NetworkNode *pSNode,           //起始节点
    CGIS_NetworkNode *pENode,           //终止节点
    enErrorCode* pErrorCode
)
{
    ASSERT(pSNode);
    ASSERT(pENode);

    int nDetailID;
    CGIS_MBBNetwork::stuPairIDItem* pMergeIDItem;
    //-------------------------------------------------------------
    
    nDetailID = (pSNode->m_pVtx->GetVBreadthID()<<15)|pSNode->m_pVtx->GetVtxID();
    if(!LoadNetWorkIDInfo(pSNode->m_pVtx->GetVBreadthID(),pErrorCode))
    {
        return FALSE;
    }
    pMergeIDItem = GetVertexMergeIDByDetailID(nDetailID);
    if(!pMergeIDItem)
    {
        *pErrorCode = CGIS_MBBNetwork::ERROR_INVALID_START_VERTEX;
        return FALSE;
    }
    nDetailID = (pENode->m_pVtx->GetVBreadthID()<<15)|pENode->m_pVtx->GetVtxID();
    if(!LoadNetWorkIDInfo(pENode->m_pVtx->GetVBreadthID(),pErrorCode))
    {
        return FALSE;
    }
    pMergeIDItem = GetVertexMergeIDByDetailID(nDetailID);
    if(!pMergeIDItem)
    {
        *pErrorCode = CGIS_MBBNetwork::ERROR_INVALID_END_VERTEX;
        return FALSE;
    }

    return TRUE;
}

int CGIS_MBBNetwork::CalculateOptimalPathMid( 
    CGIS_NetworkNode *pSNode,           //起始节点
    CGIS_NetworkNode *pENode,           //终止节点
    CGIS_NetworkPathInfo *pPathInfo,    //计算参数
    CPtrList*   pPathPointList,
    enErrorCode* pErrorCode)
{
    
    ASSERT(pErrorCode);
    if(!g_bInitialSign)
    {
        if(!InitialOperation(pErrorCode))
        {            
            return FALSE;
        }
    }
    //-------------------------------------------------------------
    BOOL bCheck = CheckCalculateCondition(pSNode,pENode,pErrorCode);
    if(!bCheck)
    {
        return FALSE;
    }
    //-------------------------------------------------------------
    TRACK_FUNCTION("全国路网路径搜索");
    
    TRACE0("--详图NODE:\n");
    DUMPSTATE(pSNode);
    DUMPSTATE(pENode);
    TRACE0("--------\n");

    CGIS_NetworkNodeV7 *pStartNode                  = NULL;
    CGIS_NetworkNodeV7 *pEndNode                    = NULL;
    
    CGIS_NetworkRoadSectionV7 aRoadSectionStart ;
    CGIS_NetworkRoadSectionV7 aRoadSectionEnd ;
    //把传入的路段包装为本地路段
    //////////////////////////////////////////////////////////////////////////   
    {
        unsigned short nRoadSectionAngle = 0xffff;
		int i;
        for(i = 0; i < pSNode->m_pVtx->GetVRutNum(); i++)
        {
            if(pSNode->m_pVtx->GetVRutList()[i] == pSNode->m_pRut->GetRutID())
            {
                nRoadSectionAngle = pSNode->m_pVtx->GetVRTurnList()[i];
            }
        }
        ASSERT(nRoadSectionAngle != 0xffff);

        CGIS_NetworkRoadSectionV7* pRoadSection     = &aRoadSectionStart;
        pRoadSection->m_stData.m_byType             = pSNode->m_pRut->GetType();
        pRoadSection->m_stData.m_nStartVertexID     = pSNode->m_pRut->GetRutStartVtxID();
        pRoadSection->m_stData.m_nEndVertexID       = pSNode->m_pRut->GetRutEndVtxID();
        pRoadSection->m_stData.m_byDirection        = pSNode->m_pRut->GetRutDirection();
        pRoadSection->m_stData.m_byRutLevel         = pSNode->m_pRut->GetRutLevel();
        pRoadSection->m_stData.m_byRutType          = pSNode->m_pRut->GetRutType();
        pRoadSection->m_stData.m_nRutID             = 0xffff;
        pRoadSection->m_stData.m_byCharge           = pSNode->m_pRut->GetRutCharge();

        
        int nDetailID = (pSNode->m_pVtx->GetVBreadthID()<<15)|pSNode->m_pVtx->GetVtxID();
        if(!LoadNetWorkIDInfo(pSNode->m_pVtx->GetVBreadthID(),pErrorCode))
        {
            return FALSE;
        }
        CGIS_MBBNetwork::stuPairIDItem* pMergeIDItem = GetVertexMergeIDByDetailID(nDetailID);       
        ASSERT(pMergeIDItem);
        int nMGID = (pMergeIDItem->nDestID)>>15;

        if(!LoadBreadthData(nMGID,pErrorCode))
        {
            TRACE0("LoadBreadthData() failed!\n");
            
            *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
            return FALSE;
        }

        unsigned long nVertexID = GetVertexMergeIDByDetailID(nDetailID)->nDestID&0x7fff; 



        pRoadSection->m_stData.m_nRBID = nMGID;
        pRoadSection->m_stData.m_nStartVertexID = nVertexID;
        pRoadSection->m_stData.m_nEndVertexID = nVertexID;
        
        CGIS_NetworkVertexV7 *pVertex = GetNetworkVtx(nMGID, nVertexID);
        for( i = 0; i < pVertex->GetRoadSectionNum(); i++)
        {
            if(pVertex->GetRoadSectionTurnList()[i] == nRoadSectionAngle)
            {
                pRoadSection->m_stData.m_nRutID             = pVertex->GetRoadSectionList()[i];
            }
        }
        
        pStartNode = new CGIS_NetworkNodeV7;
        pStartNode->pRoadSection = pRoadSection;
        pStartNode->pVertex = pVertex;
    }
    //////////////////////////////////////////////////////////////////////////  
    {
        unsigned short nRoadSectionAngle = 0xffff;
		int i;
        for(i = 0; i < pENode->m_pVtx->GetVRutNum(); i++)
        {
            if(pENode->m_pVtx->GetVRutList()[i] == pENode->m_pRut->GetRutID())
            {
                nRoadSectionAngle = pENode->m_pVtx->GetVRTurnList()[i];
            }
        }
        ASSERT(nRoadSectionAngle != 0xffff);

        CGIS_NetworkRoadSectionV7* pRoadSection = &aRoadSectionEnd;
        pRoadSection->m_stData.m_byType            = pENode->m_pRut->GetType();
        pRoadSection->m_stData.m_nStartVertexID    = pENode->m_pRut->GetRutStartVtxID();
        pRoadSection->m_stData.m_nEndVertexID      = pENode->m_pRut->GetRutEndVtxID();
        pRoadSection->m_stData.m_byDirection = pENode->m_pRut->GetRutDirection();
        pRoadSection->m_stData.m_byRutLevel = pENode->m_pRut->GetRutLevel();
        pRoadSection->m_stData.m_byRutType = pENode->m_pRut->GetRutType();
        pRoadSection->m_stData.m_nRutID = 0xffff;
        pRoadSection->m_stData.m_byCharge = pENode->m_pRut->GetRutCharge();
        
        int nDetailID = (pENode->m_pVtx->GetVBreadthID()<<15)|pENode->m_pVtx->GetVtxID();
        if(!LoadNetWorkIDInfo(pENode->m_pVtx->GetVBreadthID(),pErrorCode))
        {
            return FALSE;
        }
        CGIS_MBBNetwork::stuPairIDItem* pMergeIDItem = GetVertexMergeIDByDetailID(nDetailID);
        ASSERT(pMergeIDItem);
        int nMGID = (pMergeIDItem->nDestID)>>15;
        if(!LoadBreadthData(nMGID,pErrorCode))
        {
            TRACE0("LoadBreadthData() failed!\n");
            
            *pErrorCode = CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY;
            return FALSE;
        }
        unsigned long nVertexID = GetVertexMergeIDByDetailID(nDetailID)->nDestID&0x7fff; 
        
        pRoadSection->m_stData.m_nRBID = nMGID;
        pRoadSection->m_stData.m_nStartVertexID = nVertexID;
        pRoadSection->m_stData.m_nEndVertexID = nVertexID;

        
        CGIS_NetworkVertexV7 *pVertex = GetNetworkVtx(nMGID, nVertexID);
        for( i = 0; i < pVertex->GetRoadSectionNum(); i++)
        {
            if(pVertex->GetRoadSectionTurnList()[i] == nRoadSectionAngle)
            {
                pRoadSection->m_stData.m_nRutID = pVertex->GetRoadSectionList()[i];
            }
        }
        
        pEndNode = new CGIS_NetworkNodeV7;
        pEndNode->pRoadSection = pRoadSection;
        pEndNode->pVertex = pVertex;
    }
    //////////////////////////////////////////////////////////////////////////

    //把传入的
    return CalculateOptimalPathMidActual(pStartNode,pEndNode,pPathInfo,pPathPointList,pErrorCode);;
}


BOOL CGIS_MBBNetwork::CalculateOptimalPathMidActual( 
    CGIS_NetworkNodeV7 *pSNode,           //起始节点
    CGIS_NetworkNodeV7 *pENode,           //终止节点
    CGIS_NetworkPathInfo *pPathInfo,    //计算参数
    CPtrList *pPMid,
    enErrorCode* pErrorCode)
{   
    WATCH("TrunkNetwork::Calculate");
    WATCH2LOG("TrunkNetwork::Calculate");
	m_pVoiceNode = NULL;
    m_pFRPre = NULL;

	const unsigned short nVNum = 500;

	int pArrayOfObverseBreadthID[nVNum];
	unsigned long pArrayOfObverseVertexID[nVNum];

    int pArrayOfReverseBreadthID[nVNum];
    unsigned long pArrayOfReverseVertexID[nVNum];
	unsigned short nCount = 1;
	unsigned short nNum = 0;
    //////////////////////////////////////////////////////////////////////////
	CPtrList ObverseOpenList;
    CPtrList ObverseCloseList;
    CPtrList ReverseOpenList;
    CPtrList ReverseCloseList;

    //////////////////////////////////////////////////////////////////////////
    TRACE0("--全图NODE:\n");
    DUMPSTATE(pSNode);
    DUMPSTATE(pENode);
    TRACE0("--------\n");
    //////////////////////////////////////////////////////////////////////////
	
    //////////////////////////////////////////////////////////////////////////
    //把开始节点放入ObverseOpenList
	ST_GEO_PTXY ptStart;
	ptStart.dx = pSNode->pVertex->GetLog();
	ptStart.dy = pSNode->pVertex->GetLat();
	pArrayOfObverseVertexID[0] = pSNode->pVertex->GetVtxID();
	pArrayOfObverseBreadthID[0] = pSNode->pVertex->GetBreadthID();
	ObverseOpenList.AddTail(pSNode);

    //////////////////////////////////////////////////////////////////////////
    //把终止节点放入ReverseOpenList
	ST_GEO_PTXY ptEnd;
	ptEnd.dx = pENode->pVertex->GetLog();
	ptEnd.dy = pENode->pVertex->GetLat();
	pArrayOfReverseVertexID[0] = pENode->pVertex->GetVtxID();
	pArrayOfReverseBreadthID[0] = pENode->pVertex->GetBreadthID();
	ReverseOpenList.AddTail(pENode);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //开始计算
    CGIS_NetworkNodeV7 *pNode = NULL;
    CGIS_NetworkRoadSectionV7* pRoadSection = NULL;
    CGIS_NetworkVertexV7* pVertex = NULL;

	unsigned short *pSLRList = NULL;
	BYTE bySLRNum = 0;
	unsigned short *pELRList = NULL;
	BYTE byELRNum = 0;
    
	CGIS_NetworkNodeV7 *pCurrentNodeReverse    = NULL;
	CGIS_NetworkNodeV7 *pCurrentNodeObverse  = NULL;


	BOOL bStart = FALSE;
	BOOL bFind = FALSE;

	unsigned char nRLevel = 4;
	unsigned char bSFlag = FALSE;
	CGIS_NetworkVertexV7 *pMVtx = NULL;

    while(!ObverseOpenList.IsEmpty() && !ReverseOpenList.IsEmpty() &!bFind)
	{
		pCurrentNodeReverse = (CGIS_NetworkNodeV7*)ReverseOpenList.GetHead();
		nNum = nCount>nVNum?nVNum:nCount;

        
        //////////////////////////////////////////////////////////////////////////
        //取限行信息
		if( pCurrentNodeObverse )
		{
			pNode = pCurrentNodeObverse;
			while( pNode )
			{
				if( pNode->pRoadSection )
				{
					if( pNode->pRoadSection->GetType() != 4 )
						break;
				}
				pNode = pNode->pParent;
			}
			if( pNode )
			{
				pSLRList = pNode->pRoadSection->GetLimitNormalList();
				bySLRNum = pNode->pRoadSection->GetLimitNormalNum();
			}
		}
        //TRACE0("起点向终点方向进行一次查找\n");
        BOOL bRtn = CalculateOptimalPathRut(
            &ObverseOpenList,
            &ObverseCloseList, 
            pArrayOfReverseVertexID, 
            pArrayOfReverseBreadthID, 
            nNum,
            ptEnd,
            pPathInfo, 
            bySLRNum, 
            pSLRList, 
            nRLevel, 
            TRUE, 
            bSFlag,
            pErrorCode);

		if( bRtn )
		{
            //TRACE0("start Find road.\n");
			if( ReverseCloseList.GetCount() == 0 )
			{
				bStart = TRUE;
				bFind = TRUE;
				pCurrentNodeReverse = (CGIS_NetworkNodeV7*)ReverseOpenList.GetHead();
				pCurrentNodeObverse = (CGIS_NetworkNodeV7*)ObverseOpenList.GetHead();
				break;
			}
			POSITION ps = ReverseCloseList.GetHeadPosition();
			while(ps)
			{
				pNode = (CGIS_NetworkNodeV7*)ReverseCloseList.GetNext(ps);
				if( pNode == NULL )
                    continue;
				pMVtx = pNode->pVertex;
				if( pMVtx == NULL )
					continue;
				if( pMVtx->GetVtxID() == pArrayOfReverseVertexID[nNum] && pMVtx->GetBreadthID()==pArrayOfReverseBreadthID[nNum] )
				{
					bStart = TRUE;
					bFind = TRUE;
					pCurrentNodeReverse = pNode;
					pCurrentNodeObverse = (CGIS_NetworkNodeV7*)ObverseOpenList.GetHead();
					break;
				}
			}
			break;
		}
		pMVtx = pCurrentNodeReverse->pVertex;
		pMVtx = GetVAbutVtx( pMVtx );
		if( pMVtx == NULL )
        {
            //TRACE0("pMVtx == NULL\n");
            break;
        }
		pArrayOfReverseVertexID[nCount%nVNum] = pMVtx->GetVtxID();
		pArrayOfReverseBreadthID[nCount%nVNum] = pMVtx->GetBreadthID();

		if( ObverseOpenList.IsEmpty() )
        {
            //TRACE0("正方向的OPEN链表已空,路网是断的.\n");
            *pErrorCode = CGIS_MBBNetwork::ERROR_NETWORK_DISCONNECT;
			break;
        }

        
		pCurrentNodeObverse = (CGIS_NetworkNodeV7*)ObverseOpenList.GetHead();
		if( pCurrentNodeReverse )
		{
			pNode = pCurrentNodeReverse;
			while( pNode )
			{
				if( pNode->pRoadSection )
				{
					if( pNode->pRoadSection->GetType() != 4 )
						break;
				}
				pNode = pNode->pParent;
			}
			if( pNode )
			{
				pELRList = pNode->pRoadSection->GetLimitReverseList();
				byELRNum = pNode->pRoadSection->GetLimitReverseNum();
			}
		}
        //TRACE0("终点向起点方向进行一次查找\n");

        bRtn = CalculateOptimalPathRut( 
            &ReverseOpenList, 
            &ReverseCloseList, 
            pArrayOfObverseVertexID,
            pArrayOfObverseBreadthID,
            nNum, 
            ptEnd,
            pPathInfo,
            byELRNum,
            pELRList, 
            nRLevel,
            FALSE, 
            bSFlag,
            pErrorCode);
		if( bRtn )
		{
            //TRACE0("end Find road.\n");
			POSITION ps = ObverseCloseList.GetHeadPosition();
			while(ps)
			{
				pNode = (CGIS_NetworkNodeV7*)ObverseCloseList.GetNext(ps);
				if( pNode == NULL )
                    continue;
				pMVtx = pNode->pVertex;
				if( pMVtx == NULL )
					continue;
				if( pMVtx->GetVtxID() == pArrayOfObverseVertexID[nNum] && pMVtx->GetBreadthID()==pArrayOfObverseBreadthID[nNum] )
				{
					bStart = FALSE;
					bFind = TRUE;
					pCurrentNodeObverse = pNode;
					pCurrentNodeReverse = (CGIS_NetworkNodeV7*)ReverseOpenList.GetHead();
					break;
				}
			}
			break;
		}
		pMVtx = pCurrentNodeObverse->pVertex;
		pMVtx = GetVAbutVtx( pMVtx );
		if( pMVtx == NULL )
        {
            //TRACE0("pMVtx == NULL\n");
            break;
        }
		pArrayOfObverseVertexID[nCount%nVNum] = pMVtx->GetVtxID();
		pArrayOfObverseBreadthID[nCount%nVNum] = pMVtx->GetBreadthID();
		nCount++;
		bSFlag = FALSE;
	}

	if( bFind == TRUE )
	{
		CPtrList RutList, VtxList;
		ASSERT( pCurrentNodeObverse && pCurrentNodeReverse );
		CGIS_NetworkNodeV7 *pNode = pCurrentNodeObverse;//(CGIS_NetworkNode*)ObverseOpenList.GetHead();
		while( pNode )
		{
            //DUMPSTATE(pNode);
			RutList.AddHead( pNode->pRoadSection );
			VtxList.AddHead( pNode->pVertex );
			pNode = pNode->pParent;
		}
		if( VtxList.GetCount() > 0 )
			VtxList.RemoveTail();
//#ifdef _DEBUG
//        {
//            TRACE2("--Start:RutListSize[%d],VtxListSize[%d]\n",RutList.GetCount(),VtxList.GetCount());
//            POSITION posOfRoadSection = RutList.GetHeadPosition();
//            POSITION posOfVertex = VtxList.GetHeadPosition();
//            while(posOfVertex)
//            {
//                CGIS_NetworkRoadSectionV7* pRoadSection = (CGIS_NetworkRoadSectionV7*)RutList.GetNext(posOfRoadSection);
//                CGIS_NetworkVertexV7* pVertex = (CGIS_NetworkVertexV7*)VtxList.GetNext(posOfVertex);
//                TRACE2("\tRut%.4d->@Vtx%.4d\n",pRoadSection->GetRutID(),pVertex->GetVtxID());
//                TRACE_SLEEP_BY_TIMES(10);
//            }
//            TRACE0("--------\n");
//            Sleep(100);
//
//        }
//#endif

        //////////////////////////////////////////////////////////////////////////
		pNode = pCurrentNodeReverse;//(CGIS_NetworkNode*)ReverseOpenList.GetHead();
		while( pNode )
		{
            //DUMPSTATE(pNode);
			if( pNode->pVertex->GetEdge() == 1 )
			{
				int nBID = pNode->pVertex->GetEdgeBreadthID();
			    unsigned long nEdgeRoadSectionID = pNode->pVertex->GetEdgeRoadSectionID()&0x7fff;
				pRoadSection = GetNetworkRut(nBID,nEdgeRoadSectionID);
				if( pRoadSection )
				{
					RutList.AddTail(pNode->pRoadSection);
					CGIS_NetworkVertexV7 *pSVtx = GetNetworkVtx( nBID, pRoadSection->GetStartVtxID() );
					CGIS_NetworkVertexV7 *pEVtx = GetNetworkVtx( nBID, pRoadSection->GetEndVtxID() );
					if( pSVtx == NULL || pEVtx == NULL )
					{
						VtxList.AddTail(pNode->pVertex);
						pNode = pNode->pParent;
						continue;
					}
					if( pSVtx->GetEdge() == 1 || pEVtx->GetEdge() == 1 )
					{
						if( pSVtx->GetEdge() == 1 &&
							(fabs(pSVtx->GetLog()-pNode->pVertex->GetLog())<FLOAT_ZERO &&
								fabs(pSVtx->GetLat()-pNode->pVertex->GetLat())<FLOAT_ZERO ) )
							VtxList.AddTail( pSVtx );
						else if( pEVtx->GetEdge() == 1 &&
							(fabs(pEVtx->GetLog()-pNode->pVertex->GetLog())<FLOAT_ZERO &&
							fabs(pEVtx->GetLat()-pNode->pVertex->GetLat())<FLOAT_ZERO ) )
							VtxList.AddTail( pEVtx );
						else 
							ASSERT( FALSE );
					}
					else
						ASSERT( FALSE );
					pNode = pNode->pParent;
					continue;
				}
			}
			RutList.AddTail( pNode->pRoadSection );
			VtxList.AddTail( pNode->pVertex );
			pNode = pNode->pParent;
		}


//#ifdef _DEBUG
//        {
//            TRACE2("--End  :RutListSize[%d],VtxListSize[%d]\n",RutList.GetCount(),VtxList.GetCount());
//            POSITION posOfRoadSection = RutList.GetHeadPosition();
//            POSITION posOfVertex = VtxList.GetHeadPosition();
//            while(posOfVertex)
//            {
//                CGIS_NetworkRoadSectionV7* pRoadSection = (CGIS_NetworkRoadSectionV7*)RutList.GetNext(posOfRoadSection);
//                CGIS_NetworkVertexV7* pVertex = (CGIS_NetworkVertexV7*)VtxList.GetNext(posOfVertex);
//                TRACE2("\tRut%.4d->@Vtx%.4d\n",pRoadSection->GetRutID(),pVertex->GetVtxID());
//                TRACE_SLEEP_BY_TIMES(10);
//            }
//            TRACE0("--------\n");
//            Sleep(100);
//        }
//#endif

		//////////////////////////////////////////////////////////////////////////		
		FullPathMData(&RutList, &VtxList,pPMid);
        //////////////////////////////////////////////////////////////////////////
        

        ClearNodeList(&ObverseOpenList);
        ClearNodeList(&ObverseCloseList);
        ClearNodeList(&ReverseOpenList);
        ClearNodeList(&ReverseCloseList);
    
		return TRUE;
	}
    else //!bFind
    {
        if(pErrorCode)
        {
            if(ObverseOpenList.IsEmpty() || ReverseOpenList.IsEmpty())
            {
                *pErrorCode = CGIS_MBBNetwork::ERROR_NETWORK_DISCONNECT;
            }
        }

        return FALSE;
    }

    ASSERT(FALSE);
	return FALSE;
}
BOOL CGIS_MBBNetwork::CalculateOptimalPathMidActualV2( 
    CGIS_NetworkNodeV7 *pSNode,           //起始节点
    CGIS_NetworkNodeV7 *pENode,           //终止节点
    CGIS_NetworkPathInfo *pPathInfo,    //计算参数
    CPtrList *pPMid,
    enErrorCode* pErrorCode)
{   
    TRACK_FUNCTION("CGIS_MBBNetwork::Calculate");
    WATCH("CGIS_MBBNetwork::Calculate");
    WATCH2LOG("CGIS_MBBNetwork::Calculate");
	m_pVoiceNode = NULL;
    m_pFRPre = NULL;

	const unsigned short nVNum = 500;
	unsigned long pArrayOfObverseVertexID[nVNum];
    unsigned long pArrayOfReverseVertexID[nVNum];
	int pArrayOfObverseBreadthID[nVNum];
    int pArrayOfReverseBreadthID[nVNum];
	unsigned short nCount = 1;
	unsigned short nNum = 0;
    //////////////////////////////////////////////////////////////////////////
	CPtrList ObverseOpenList;
    CPtrList ObverseCloseList;
    CPtrList ReverseOpenList;
    CPtrList ReverseCloseList;

    //////////////////////////////////////////////////////////////////////////
    TRACE0("--全图NODE:\n");
    DUMPSTATE(pSNode);
    DUMPSTATE(pENode);
    TRACE0("--------\n");
    //////////////////////////////////////////////////////////////////////////
	
    //////////////////////////////////////////////////////////////////////////
    //把开始节点放入ObverseOpenList
	ST_GEO_PTXY ptStart;
	ptStart.dx = pSNode->pVertex->GetLog();
	ptStart.dy = pSNode->pVertex->GetLat();
	pArrayOfObverseVertexID[0] = pSNode->pVertex->GetVtxID();
	pArrayOfObverseBreadthID[0] = pSNode->pVertex->GetBreadthID();
	ObverseOpenList.AddTail(pSNode);

    //////////////////////////////////////////////////////////////////////////
    //把终止节点放入ReverseOpenList
	ST_GEO_PTXY ptEnd;
	ptEnd.dx = pENode->pVertex->GetLog();
	ptEnd.dy = pENode->pVertex->GetLat();
	pArrayOfReverseVertexID[0] = pENode->pVertex->GetVtxID();
	pArrayOfReverseBreadthID[0] = pENode->pVertex->GetBreadthID();
	ReverseOpenList.AddTail(pENode);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //开始计算
    CGIS_NetworkNodeV7 *pNode = NULL;
    CGIS_NetworkRoadSectionV7* pRoadSection = NULL;
    CGIS_NetworkVertexV7* pVertex = NULL;

	unsigned short *pSLRList = NULL;
	BYTE bySLRNum = 0;
	unsigned short *pELRList = NULL;
	BYTE byELRNum = 0;
    
	CGIS_NetworkNodeV7 *pCurrentNodeObverse  = NULL;
	CGIS_NetworkNodeV7 *pCurrentNodeReverse    = NULL;


	BOOL bStart = FALSE;
	BOOL bFind = FALSE;

	unsigned char nRLevel = 4;
	unsigned char bSFlag = FALSE;
	CGIS_NetworkVertexV7 *pMVtx = NULL;

    while(!ObverseOpenList.IsEmpty() && !ReverseOpenList.IsEmpty() &!bFind)
	{
		pCurrentNodeReverse = (CGIS_NetworkNodeV7*)ReverseOpenList.GetHead();
		nNum = nCount>nVNum?nVNum:nCount;

        //////////////////////////////////////////////////////////////////////////
        //取限行信息
		if( pCurrentNodeObverse )
		{
			pNode = pCurrentNodeObverse;
			while( pNode )
			{
				if( pNode->pRoadSection )
				{
					if( pNode->pRoadSection->GetType() != 4 )
						break;
				}
				pNode = pNode->pParent;
			}
			if( pNode )
			{
				pSLRList = pNode->pRoadSection->GetLimitNormalList();
				bySLRNum = pNode->pRoadSection->GetLimitNormalNum();
			}
		}

        //////////////////////////////////////////////////////////////////////////
        
        CPtrList* pOpenList = &ObverseOpenList;
        if( pOpenList->GetCount() <= 0 )
        {
            ASSERT(FALSE);
            return FALSE;
        }
        
        CGIS_NetworkNodeV7 *pParentNode = (CGIS_NetworkNodeV7*)pOpenList->GetHead();
        //-------------------------------------------------------------
        ///判断是否已经找到最优路线,目标节点数组中有本结点则表示找到.
        for( int i = 0;	i < nNum;	i++ )
        {
            if( pParentNode->pVertex->GetVtxID() == pArrayOfReverseVertexID[i] && 
                pParentNode->pVertex->GetBreadthID() == pArrayOfReverseBreadthID[i] )
            {
                TRACE0("find dest in trunk network.\n");
                ASSERT(FALSE);
                return TRUE;
            }
        }
        
        //-------------------------------------------------------------
        pOpenList->RemoveHead( );
        pParentNode->pVertex->GetVtxSInfo()->posOHead = pParentNode->pVertex->GetVtxSInfo()->posOEnd = NULL;
        pParentNode->pVertex->GetVtxSInfo()->bySHead = pParentNode->pVertex->GetVtxSInfo()->bySEnd = 2;
        if( pParentNode->pRoadSection )
        {
            if( pParentNode->pRoadSection->GetType() == 4 )
                pParentNode->pVertex->GetVtxSInfo()->bySHead = pParentNode->pVertex->GetVtxSInfo()->bySEnd = 1;
        }
        ObverseCloseList.AddTail( pParentNode );
        //-------------------------------------------------------------
        
        //////////////////////////////////////////////////////////////////////////
         
        if(!ExtendNode(pParentNode, &ObverseOpenList, ptEnd,pPathInfo,  bySLRNum, 
             pSLRList, nRLevel, TRUE, bSFlag, pErrorCode))
        {
            break;
        }
        //////////////////////////////////////////////////////////////////////////
        


		//if( bRtn )
		{
            //TRACE0("start Find road.\n");
			if( ReverseCloseList.GetCount() == 0 )
			{
				bStart = TRUE;
				bFind = TRUE;
				pCurrentNodeObverse = (CGIS_NetworkNodeV7*)ObverseOpenList.GetHead();
				pCurrentNodeReverse = (CGIS_NetworkNodeV7*)ReverseOpenList.GetHead();
				break;
			}
			POSITION ps = ReverseCloseList.GetHeadPosition();
			while(ps)
			{
				pNode = (CGIS_NetworkNodeV7*)ReverseCloseList.GetNext(ps);
				if( pNode == NULL )
                    continue;
				pMVtx = pNode->pVertex;
				if( pMVtx == NULL )
					continue;


				if( pMVtx->GetVtxID() == pArrayOfReverseVertexID[nNum] && 
                    pMVtx->GetBreadthID()==pArrayOfReverseBreadthID[nNum] )
				{
					bStart = TRUE;
					bFind = TRUE;
					pCurrentNodeReverse = pNode;
					pCurrentNodeObverse = (CGIS_NetworkNodeV7*)ObverseOpenList.GetHead();
					break;
				}
			}
			break;
		}

        //-------------------------------------------------------------
        //
		pMVtx = pCurrentNodeReverse->pVertex;
		pMVtx = GetVAbutVtx( pMVtx );
		if( pMVtx == NULL )
        {
            //TRACE0("pMVtx == NULL\n");
            break;
        }
		pArrayOfReverseVertexID[nCount%nVNum]     = pMVtx->GetVtxID();
		pArrayOfReverseBreadthID[nCount%nVNum]    = pMVtx->GetBreadthID();
        //-------------------------------------------------------------

        
		if( ObverseOpenList.IsEmpty() )
        {
            //TRACE0("正方向的OPEN链表已空,路网是断的.\n");
            *pErrorCode = CGIS_MBBNetwork::ERROR_NETWORK_DISCONNECT;
			break;
        }
		pCurrentNodeObverse = (CGIS_NetworkNodeV7*)ObverseOpenList.GetHead();
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //TRACE0("终点向起点方向进行一次查找\n");
		if( pCurrentNodeReverse )
		{
			pNode = pCurrentNodeReverse;
			while( pNode )
			{
				if( pNode->pRoadSection )
				{
					if( pNode->pRoadSection->GetType() != 4 )
						break;
				}
				pNode = pNode->pParent;
			}
			if( pNode )
			{
				pELRList = pNode->pRoadSection->GetLimitReverseList();
				byELRNum = pNode->pRoadSection->GetLimitReverseNum();
			}
		}

        BOOL bRtn = CalculateOptimalPathRut( 
            &ReverseOpenList, 
            &ReverseCloseList, 
            pArrayOfObverseVertexID,
            pArrayOfObverseBreadthID,
            nNum, 
            ptEnd,
            pPathInfo,
            byELRNum,
            pELRList, 
            nRLevel,
            FALSE, 
            bSFlag,
            pErrorCode);
		if( bRtn )
		{
            //TRACE0("end Find road.\n");
			POSITION ps = ObverseCloseList.GetHeadPosition();
			while(ps)
			{
				pNode = (CGIS_NetworkNodeV7*)ObverseCloseList.GetNext(ps);
				if( pNode == NULL )
                    continue;
				pMVtx = pNode->pVertex;
				if( pMVtx == NULL )
					continue;
				if( pMVtx->GetVtxID() == pArrayOfObverseVertexID[nNum] && pMVtx->GetBreadthID()==pArrayOfObverseBreadthID[nNum] )
				{
					bStart = FALSE;
					bFind = TRUE;
					pCurrentNodeObverse = pNode;
					pCurrentNodeReverse = (CGIS_NetworkNodeV7*)ReverseOpenList.GetHead();
					break;
				}
			}
			break;
		}
		pMVtx = pCurrentNodeObverse->pVertex;
		pMVtx = GetVAbutVtx( pMVtx );
		if( pMVtx == NULL )
        {
            //TRACE0("pMVtx == NULL\n");
            break;
        }
		pArrayOfObverseVertexID[nCount%nVNum] = pMVtx->GetVtxID();
		pArrayOfObverseBreadthID[nCount%nVNum] = pMVtx->GetBreadthID();
		nCount++;
		bSFlag = FALSE;
	}

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //找到路就开始填充
	if( bFind == TRUE )
	{
		CPtrList RutList, VtxList;
		ASSERT( pCurrentNodeObverse && pCurrentNodeReverse );
		CGIS_NetworkNodeV7 *pNode = pCurrentNodeObverse;//(CGIS_NetworkNode*)ObverseOpenList.GetHead();
		while( pNode )
		{
            //DUMPSTATE(pNode);
			RutList.AddHead( pNode->pRoadSection );
			VtxList.AddHead( pNode->pVertex );
			pNode = pNode->pParent;
		}
		if( VtxList.GetCount() > 0 )
			VtxList.RemoveTail();
//#ifdef _DEBUG
//        {
//            TRACE2("--Start:RutListSize[%d],VtxListSize[%d]\n",RutList.GetCount(),VtxList.GetCount());
//            POSITION posOfRoadSection = RutList.GetHeadPosition();
//            POSITION posOfVertex = VtxList.GetHeadPosition();
//            while(posOfVertex)
//            {
//                CGIS_NetworkRoadSectionV7* pRoadSection = (CGIS_NetworkRoadSectionV7*)RutList.GetNext(posOfRoadSection);
//                CGIS_NetworkVertexV7* pVertex = (CGIS_NetworkVertexV7*)VtxList.GetNext(posOfVertex);
//                TRACE2("\tRut%.4d->@Vtx%.4d\n",pRoadSection->GetRutID(),pVertex->GetVtxID());
//                TRACE_SLEEP_BY_TIMES(10);
//            }
//            TRACE0("--------\n");
//            Sleep(100);
//
//        }
//#endif

        //////////////////////////////////////////////////////////////////////////
		pNode = pCurrentNodeReverse;//(CGIS_NetworkNode*)ReverseOpenList.GetHead();
		while( pNode )
		{
            //DUMPSTATE(pNode);
			if( pNode->pVertex->GetEdge() == 1 )
			{
				int nBID = pNode->pVertex->GetEdgeBreadthID();
			    unsigned long nEdgeRoadSectionID = pNode->pVertex->GetEdgeRoadSectionID()&0x7fff;
				pRoadSection = GetNetworkRut(nBID,nEdgeRoadSectionID);
				if( pRoadSection )
				{
					RutList.AddTail(pNode->pRoadSection);
					CGIS_NetworkVertexV7 *pSVtx = GetNetworkVtx( nBID, pRoadSection->GetStartVtxID() );
					CGIS_NetworkVertexV7 *pEVtx = GetNetworkVtx( nBID, pRoadSection->GetEndVtxID() );
					if( pSVtx == NULL || pEVtx == NULL )
					{
						VtxList.AddTail(pNode->pVertex);
						pNode = pNode->pParent;
						continue;
					}
					if( pSVtx->GetEdge() == 1 || pEVtx->GetEdge() == 1 )
					{
						if( pSVtx->GetEdge() == 1 &&
							(fabs(pSVtx->GetLog()-pNode->pVertex->GetLog())<FLOAT_ZERO &&
								fabs(pSVtx->GetLat()-pNode->pVertex->GetLat())<FLOAT_ZERO ) )
							VtxList.AddTail( pSVtx );
						else if( pEVtx->GetEdge() == 1 &&
							(fabs(pEVtx->GetLog()-pNode->pVertex->GetLog())<FLOAT_ZERO &&
							fabs(pEVtx->GetLat()-pNode->pVertex->GetLat())<FLOAT_ZERO ) )
							VtxList.AddTail( pEVtx );
						else 
							ASSERT( FALSE );
					}
					else
						ASSERT( FALSE );
					pNode = pNode->pParent;
					continue;
				}
			}
			RutList.AddTail( pNode->pRoadSection );
			VtxList.AddTail( pNode->pVertex );
			pNode = pNode->pParent;
		}

//#ifdef _DEBUG
//        {
//            TRACE2("--End  :RutListSize[%d],VtxListSize[%d]\n",RutList.GetCount(),VtxList.GetCount());
//            POSITION posOfRoadSection = RutList.GetHeadPosition();
//            POSITION posOfVertex = VtxList.GetHeadPosition();
//            while(posOfVertex)
//            {
//                CGIS_NetworkRoadSectionV7* pRoadSection = (CGIS_NetworkRoadSectionV7*)RutList.GetNext(posOfRoadSection);
//                CGIS_NetworkVertexV7* pVertex = (CGIS_NetworkVertexV7*)VtxList.GetNext(posOfVertex);
//                TRACE2("\tRut%.4d->@Vtx%.4d\n",pRoadSection->GetRutID(),pVertex->GetVtxID());
//                TRACE_SLEEP_BY_TIMES(10);
//            }
//            TRACE0("--------\n");
//            Sleep(100);
//        }
//#endif

		//////////////////////////////////////////////////////////////////////////		
		FullPathMData(&RutList, &VtxList,pPMid);
        //////////////////////////////////////////////////////////////////////////
        

        ClearNodeList(&ObverseOpenList);
        ClearNodeList(&ObverseCloseList);
        ClearNodeList(&ReverseOpenList);
        ClearNodeList(&ReverseCloseList);
    
		return TRUE;
	}

    
	return FALSE;
}
BOOL CGIS_MBBNetwork::ClearNodeList(CPtrList* pNodeList)
{
    POSITION pos = pNodeList->GetHeadPosition();
    CGIS_NetworkNodeV7* pNode = NULL;
	while( pos )
	{
		pNode = (CGIS_NetworkNodeV7*)pNodeList->GetNext( pos );
		if( pNode )
		{
			pNode->pVertex->GetVtxSInfo()->posOHead = pNode->pVertex->GetVtxSInfo()->posOEnd = NULL;
			pNode->pVertex->GetVtxSInfo()->bySHead = pNode->pVertex->GetVtxSInfo()->bySEnd = 0;
			delete pNode;
		}
	}
	pNodeList->RemoveAll();
    
    return TRUE;
}

const unsigned short MAX_PTNUM_ROUND = 32;
BOOL CGIS_MBBNetwork::FullPathNodeRuts(
    CGIS_PathpointNode *pVNode, 
    CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM],
    CGIS_NetworkVertexV7 *pVertex,
    CGIS_NetworkRoadSectionV7 *pRoadSectionPrevious, 
    CGIS_NetworkRoadSectionV7 *pRoadSectionNext, 
    BYTE byRAType )
{
	ST_GEO_PTXY pPtList[MAX_PTNUM_ROUND];
	int nPtNum = 1;
	pPtList[0] = pVNode->GetPathpointPosition();
	pVNode->GetVNodeInfo()->byConfusion = 16;
	pVNode->GetVNodeInfo()->byConfusion2 = 16;
	if( byRAType == 1 )
		return FullPathNodeRutsSVtx( pVNode, pFRList, pVertex, pRoadSectionPrevious, pRoadSectionNext, pPtList, nPtNum );
	else if( byRAType == 2 )
		return FullPathNodeRutsAss( pVNode, pFRList, pVertex, pRoadSectionPrevious, pRoadSectionNext, pPtList, nPtNum );
	else if( byRAType == 3 )
		return FullPathNodeRutsRound( pVNode, pFRList, pVertex, pRoadSectionPrevious, pRoadSectionNext, pPtList, nPtNum );
	return FALSE;
}
//填普通路段
BOOL CGIS_MBBNetwork::FullPathNodeRutsSVtx(
    CGIS_PathpointNode *pVNode, 
    CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], 
    CGIS_NetworkVertexV7 *pVertex, 
    CGIS_NetworkRoadSectionV7 *pRoadSectionPrevious, 
    CGIS_NetworkRoadSectionV7 *pRoadSectionNext, 
    ST_GEO_PTXY pPtList[], 
    int &nPtNum )
{
    
	pVNode->CreateCRect( pPtList, nPtNum );

    //与出口路段同名的路段的角度(包括入口路段)[0中放的是出口路段]
	int nSameNameRoadSectionAngleArray[5];
	memset( nSameNameRoadSectionAngleArray, 0, sizeof(nSameNameRoadSectionAngleArray) );
    //所有其它路段(除去入口路段和出口路段)[0中放的是出口路段]
	int nOtherRoadSectionAngleArray[10];
	memset( nOtherRoadSectionAngleArray, 0, sizeof(nOtherRoadSectionAngleArray));

	int nCountOfSameNameRoadSectionAngleArray = 1;
	int nCountOfOtherRoadSectionAngleArray = 1;
	swString strRut = _T("");
	m_bTFlay = FALSE;

    //////////////////////////////////////////////////////////////////////////
    //取此路段
	CGIS_FeatureRoadSectionV7 *pFtrRoadSection = NULL;
    if(pRoadSectionNext->GetRutID() != 0xffff)
    {
        pFtrRoadSection = pFRList[pRoadSectionNext->GetRutID()];
    }

    //ASSERT(pFtrRoadSection);
	if( pFtrRoadSection )
	{
		if( pFtrRoadSection->GetAnnotation() )
			strRut = pFtrRoadSection->GetAnnotation();
	}
	CGIS_NetworkRoadSectionV7 *pRoadSection = NULL;
	if( pVertex->GetRoadSectionNum() != 3 || pRoadSectionPrevious->GetLength() >= 80.1 )
		m_bTFlay = TRUE;
    int i;
	for( i=0;	i<pVertex->GetRoadSectionNum();	i++ )
	{
        //////////////////////////////////////////////////////////////////////////
        //070606 add for 通行标志
        BOOL bPassSign = FALSE;
        pRoadSection = GetNetworkRut( pVertex->GetBreadthID(), pVertex->GetRoadSectionList()[i] );
		ASSERT(pRoadSection);
        if( pRoadSection )
        {
            bPassSign = CouldReachAnotherVertexInRoadSection(pVertex, pRoadSection);
        }
        //////////////////////////////////////////////////////////////////////////

        if( pVertex->GetRoadSectionNum() == 3 && m_bTFlay == FALSE )
		{
			if( !(pVertex->GetRoadSectionList()[i] == pRoadSectionPrevious->GetRutID()
                || 
                pVertex->GetRoadSectionList()[i] == pRoadSectionNext->GetRutID()) )
			{
				pRoadSection = GetNetworkRut( pVertex->GetBreadthID(), pVertex->GetRoadSectionList()[i] );
				ASSERT(pRoadSection);
				if( pRoadSection )
				{
					if( GetNextRutVtx(pVertex, pRoadSection,TRUE) )
						m_bTFlay = TRUE;
				}
			}
		}

        
		pFtrRoadSection = pFRList[pVertex->GetRoadSectionList()[i]];
		if( pFtrRoadSection )
		{
			if( pFtrRoadSection->GetRutID() == pRoadSectionNext->GetRutID() )
			{
				nSameNameRoadSectionAngleArray[0] = pVertex->GetRoadSectionTurnList()[i];
				nOtherRoadSectionAngleArray[0] = pVertex->GetRoadSectionTurnList()[i];
			}
			else if( pFtrRoadSection->GetRutID() != pRoadSectionPrevious->GetRutID() )
			{
                //不是出口路段,也不是入口路段的话放进来.
                if(bPassSign)
                {
                    nOtherRoadSectionAngleArray[nCountOfOtherRoadSectionAngleArray] = 
                        pVertex->GetRoadSectionTurnList()[i];
                    nCountOfOtherRoadSectionAngleArray += 1;
                }
			}
			CGIS_NetworkNodeRut *pNRut = new CGIS_NetworkNodeRut;

            //////////////////////////////////////////////////////////////////////////
            //由于这些路段加载不到空间数据,所以关掉.
			if( pFtrRoadSection->GetAnnotation() )
			{
				char *szAnno = new char[pFtrRoadSection->GetAnnoNum()];
				memcpy( szAnno, pFtrRoadSection->GetAnnotation(), pFtrRoadSection->GetAnnoNum() );
				pNRut->SetRutAnno( szAnno );
				pNRut->SetRutAnnoNum( pFtrRoadSection->GetAnnoNum() );
                
                //有条路(这条路不是出口路段,但有可能是入口路段)与出口路段同名.混乱角度数组中放入此路段的角度
				if( strRut.Compare(swString(szAnno)) == 0 &&
                    pFtrRoadSection->GetRutID() != pRoadSectionNext->GetRutID() )
				{
                    if(bPassSign)
                    {
                        nSameNameRoadSectionAngleArray[nCountOfSameNameRoadSectionAngleArray] = pVertex->GetRoadSectionTurnList()[i];
                        nCountOfSameNameRoadSectionAngleArray += 1;
                    }
				}
			}

			ST_GEO_PTXY *pPtList = new ST_GEO_PTXY[pFtrRoadSection->GetPtNum()];
			memcpy( pPtList, pFtrRoadSection->GetPtList(), sizeof(ST_GEO_PTXY)*pFtrRoadSection->GetPtNum() );
			pNRut->SetRutPtList( pPtList );
			pNRut->SetRutPtNum( pFtrRoadSection->GetPtNum() );
			pNRut->SetRutID( pFtrRoadSection->GetRutID() );

			pVNode->GetNodeRutList()->AddTail( pNRut );
			if( IsPtInRect(pFtrRoadSection->GetPtList()[0].dx,pFtrRoadSection->GetPtList()[0].dy,pVNode->GetCRect().dLeft,
					pVNode->GetCRect().dTop,pVNode->GetCRect().dRight,pVNode->GetCRect().dBottom) && 
				IsPtInRect(pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-1].dx,pFtrRoadSection->GetPtList()[pFtrRoadSection->GetPtNum()-1].dy,
					pVNode->GetCRect().dLeft,pVNode->GetCRect().dTop,pVNode->GetCRect().dRight,pVNode->GetCRect().dBottom) )
			{
				CGIS_NetworkRoadSectionV7 *pNRut1 = NULL;
				CGIS_NetworkVertexV7 *pNVtx = NULL;
				pNRut1 = GetNetworkRut( pVertex->GetBreadthID(), pFtrRoadSection->GetRutID() );
				if( pNRut1 )
				{
					if( pVertex->GetVtxID() == pNRut1->GetStartVtxID() )
						pNVtx = GetNetworkVtx( pVertex->GetBreadthID(), pNRut1->GetEndVtxID() );
					else
						pNVtx = GetNetworkVtx( pVertex->GetBreadthID(), pNRut1->GetEndVtxID() );
					if( pNVtx )
					{
						for( int j=0;	j<pNVtx->GetRoadSectionNum();	j++ )
						{
							pFtrRoadSection = pFRList[pNVtx->GetRoadSectionList()[j]];
							if( pFtrRoadSection )
							{
								if( pFtrRoadSection->GetRutID() == pNRut1->GetRutID() )
									continue;
								pNRut = new CGIS_NetworkNodeRut;
								if( pFtrRoadSection->GetAnnotation() )
								{
									char *szAnno = new char[pFtrRoadSection->GetAnnoNum()];
									memcpy( szAnno, pFtrRoadSection->GetAnnotation(), pFtrRoadSection->GetAnnoNum() );
									pNRut->SetRutAnno( szAnno );
									pNRut->SetRutAnnoNum( pFtrRoadSection->GetAnnoNum() );
								}
								ST_GEO_PTXY *pPtList = new ST_GEO_PTXY[pFtrRoadSection->GetPtNum()];
								memcpy( pPtList, pFtrRoadSection->GetPtList(), sizeof(ST_GEO_PTXY)*pFtrRoadSection->GetPtNum() );
								pNRut->SetRutPtList( pPtList );
								pNRut->SetRutPtNum( pFtrRoadSection->GetPtNum() );
								pNRut->SetRutID( pFtrRoadSection->GetRutID() );
								pVNode->GetNodeRutList()->AddTail( pNRut );
							}
						}
					}
				}
			}
		}
	}

    for(i = 0; i < pVertex->GetLowLayerRoadSectionNum(); i++)
    {
        nOtherRoadSectionAngleArray[nCountOfOtherRoadSectionAngleArray] = pVertex->GetLowLayerRoadSectionTurnList()[i];
        nCountOfOtherRoadSectionAngleArray++;
		nSameNameRoadSectionAngleArray[nCountOfSameNameRoadSectionAngleArray] = pVertex->GetLowLayerRoadSectionTurnList()[i];
		nCountOfSameNameRoadSectionAngleArray += 1;
    }
    
	if( nCountOfSameNameRoadSectionAngleArray > 1 )//有同名的路口
	{
		for( i=1;	i<nCountOfSameNameRoadSectionAngleArray;	i++ )
		{
			if( abs(nSameNameRoadSectionAngleArray[0]-nSameNameRoadSectionAngleArray[i])<=36 )//这条同名的路段与出口路段的角度差在[-36,+36]内.
			{
                
				if( nSameNameRoadSectionAngleArray[0] > nSameNameRoadSectionAngleArray[i] )
				{
					if( pVNode->GetVNodeInfo()->byConfusion == 37 )
					{
						pVNode->GetVNodeInfo()->byConfusion = 38;
						break;
					}
                    else
                    {
                        //在左
						pVNode->GetVNodeInfo()->byConfusion = 15;
                    }
				}
				else
				{
					if( pVNode->GetVNodeInfo()->byConfusion == 15 )
					{
						pVNode->GetVNodeInfo()->byConfusion = 38;
						break;
					}
					else
                    {
                        //右转
						pVNode->GetVNodeInfo()->byConfusion = 37;
                    }
				}
			}
			else if( (360-abs(nSameNameRoadSectionAngleArray[0]-nSameNameRoadSectionAngleArray[i]))<=36 )//这条同名的路段与出口路段的角度差在[-36,+36]内.
			{
				if( nSameNameRoadSectionAngleArray[0] < nSameNameRoadSectionAngleArray[i] )
				{
					if( pVNode->GetVNodeInfo()->byConfusion == 37 )
					{
						pVNode->GetVNodeInfo()->byConfusion = 38;
						break;
					}
					else
                    {
                        //在左侧
						pVNode->GetVNodeInfo()->byConfusion = 15;

                    }
				}
				else
				{
					if( pVNode->GetVNodeInfo()->byConfusion == 15 )
					{
						pVNode->GetVNodeInfo()->byConfusion = 38;
						break;
					}
					else
                    {
                        //在右
						pVNode->GetVNodeInfo()->byConfusion = 37;
                    }
				}
			}
		}
	}
	if( nCountOfOtherRoadSectionAngleArray > 1 )
	{
		for( i=1;	i<nCountOfOtherRoadSectionAngleArray;	i++ )
		{
			if( abs(nOtherRoadSectionAngleArray[0]-nOtherRoadSectionAngleArray[i])<=50 )
			{
				if( nOtherRoadSectionAngleArray[0] > nOtherRoadSectionAngleArray[i] )
				{
					if( pVNode->GetVNodeInfo()->byConfusion2 == 37 )
					{
						pVNode->GetVNodeInfo()->byConfusion2 = 38;
						break;
					}
					else
						pVNode->GetVNodeInfo()->byConfusion2 = 15;
				}
				else
				{
					if( pVNode->GetVNodeInfo()->byConfusion2 == 15 )
					{
						pVNode->GetVNodeInfo()->byConfusion2 = 38;
						break;
					}
					else
						pVNode->GetVNodeInfo()->byConfusion2 = 37;
				}
			}
			else if( 360-abs(nOtherRoadSectionAngleArray[0]-nOtherRoadSectionAngleArray[i])<=50 )
			{
				if( nOtherRoadSectionAngleArray[0] < nOtherRoadSectionAngleArray[i] )
				{
					if( pVNode->GetVNodeInfo()->byConfusion2 == 37 )
					{
						pVNode->GetVNodeInfo()->byConfusion2 = 38;
						break;
					}
					else
						pVNode->GetVNodeInfo()->byConfusion2 = 15;
				}
				else
				{
					if( pVNode->GetVNodeInfo()->byConfusion2 == 15 )
					{
						pVNode->GetVNodeInfo()->byConfusion2 = 38;
						break;
					}
					else
						pVNode->GetVNodeInfo()->byConfusion2 = 37;
				}
			}
		}
	}
	return TRUE;
}
//填交叉路口内路段
BOOL CGIS_MBBNetwork::FullPathNodeRutsAss( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRoadSectionPrevious, CGIS_NetworkRoadSectionV7 *pRoadSectionNext, ST_GEO_PTXY pPtList[], int &nPtNum )
{
	m_bTFlay = TRUE;
	BOOL bReturn = GetNextAssRut( pVNode, pFRList, pVertex, pRoadSectionPrevious, pPtList, nPtNum );
	pVNode->CreateCRect( pPtList, nPtNum );
	return bReturn;
}
BOOL CGIS_MBBNetwork::GetNextAssRut( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], 
			CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRoadSectionPrevious, ST_GEO_PTXY pPtList[], int &nPtNum )
{
	CGIS_FeatureRoadSectionV7 *pFtrRoadSection = NULL;
	CGIS_NetworkVertexV7 *pVNext = NULL;
	CGIS_NetworkRoadSectionV7 *pRoadSectionNext = NULL;
	for( int i=0;	i<pVertex->GetRoadSectionNum();	i++ )
	{
//		if( pVertex->GetRoadSectionList()[i] == pRoadSectionPrevious->GetRutID() )
//			continue;
		pFtrRoadSection = pFRList[pVertex->GetRoadSectionList()[i]];
		if( pFtrRoadSection )
		{
			CGIS_NetworkNodeRut *pNRut = new CGIS_NetworkNodeRut;
			if( pFtrRoadSection->GetAnnotation() )
			{
				char *szAnno = new char[pFtrRoadSection->GetAnnoNum()];
				memcpy( szAnno, pFtrRoadSection->GetAnnotation(), pFtrRoadSection->GetAnnoNum() );
				pNRut->SetRutAnno( szAnno );
				pNRut->SetRutAnnoNum( pFtrRoadSection->GetAnnoNum() );
			}
			ST_GEO_PTXY *pPtList = new ST_GEO_PTXY[pFtrRoadSection->GetPtNum()];
			memcpy( pPtList, pFtrRoadSection->GetPtList(), sizeof(ST_GEO_PTXY)*pFtrRoadSection->GetPtNum() );
			pNRut->SetRutPtList( pPtList );
			pNRut->SetRutPtNum( pFtrRoadSection->GetPtNum() );
			pNRut->SetRutID( pFtrRoadSection->GetRutID() );
			pVNode->GetNodeRutList()->AddTail( pNRut );
		}
		if( pVertex->GetRoadSectionList()[i] == pRoadSectionPrevious->GetRutID() )
			continue;
		pRoadSectionNext = GetNetworkRut( pVertex->GetBreadthID(), pVertex->GetRoadSectionList()[i] );
		if( pRoadSectionNext )
		{
			if( pRoadSectionNext->GetRutType() == 4 )
			{
				if( pVertex->GetVtxID() == pRoadSectionNext->GetStartVtxID() )
					pVNext = GetNetworkVtx( pVertex->GetBreadthID(), pRoadSectionNext->GetEndVtxID() );
				else
					pVNext = GetNetworkVtx( pVertex->GetBreadthID(), pRoadSectionNext->GetStartVtxID() );
				if( pVNext )
				{
					pVertex->GetVtxSInfo()->bySHead = 3;
					if( pVNext->GetVtxSInfo()->bySHead != 3 )
					{
						pPtList[nPtNum].dx = pVNext->GetLog();
						pPtList[nPtNum].dy = pVNext->GetLat();
						nPtNum++;
						GetNextAssRut( pVNode, pFRList, pVNext, pRoadSectionNext, pPtList, nPtNum );
					}
				}
			}
		}
	}
	return TRUE;
}
//填环岛路段
BOOL CGIS_MBBNetwork::FullPathNodeRutsRound( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRoadSectionPrevious, CGIS_NetworkRoadSectionV7 *pRoadSectionNext, ST_GEO_PTXY pPtList[], int &nPtNum )
{
	CGIS_NetworkRoadSectionV7 *pNRStart = NULL;
	if( pRoadSectionPrevious->GetRutType() == 0 )
		pNRStart = pRoadSectionPrevious;
	else if( pRoadSectionNext->GetRutType() == 0 )
		pNRStart = pRoadSectionNext;
	else
		return FALSE;
	CGIS_NetworkRoadSectionV7 *pNRNext = NULL;
	CGIS_NetworkRoadSectionV7 *pNRut = NULL;
	CGIS_FeatureRoadSectionV7 *pFtrRoadSection = NULL;
	for( int i=0;	i<pVertex->GetRoadSectionNum();	i++ )
	{
		if( pVertex->GetRoadSectionList()[i] != pNRStart->GetRutID() )
		{
			pNRut = GetNetworkRut( pVertex->GetBreadthID(), pVertex->GetRoadSectionList()[i] );
			if( pNRut )
			{
				if( pNRut->GetRutType() == 0 )
					pNRNext = pNRut;
			}
		}
		pFtrRoadSection = pFRList[pVertex->GetRoadSectionList()[i]];
		if( pFtrRoadSection )
		{
			CGIS_NetworkNodeRut *pNNRut = new CGIS_NetworkNodeRut;
			if( pFtrRoadSection->GetAnnotation() )
			{
				char *szAnno = new char[pFtrRoadSection->GetAnnoNum()];
				memcpy( szAnno, pFtrRoadSection->GetAnnotation(), pFtrRoadSection->GetAnnoNum() );
				pNNRut->SetRutAnno( szAnno );
				pNNRut->SetRutAnnoNum( pFtrRoadSection->GetAnnoNum() );
			}
			ST_GEO_PTXY *pPtList = new ST_GEO_PTXY[pFtrRoadSection->GetPtNum()];
			memcpy( pPtList, pFtrRoadSection->GetPtList(), sizeof(ST_GEO_PTXY)*pFtrRoadSection->GetPtNum() );
			pNNRut->SetRutPtList( pPtList );
			pNNRut->SetRutPtNum( pFtrRoadSection->GetPtNum() );
			pNNRut->SetRutID( pFtrRoadSection->GetRutID() );
			pVNode->GetNodeRutList()->AddTail( pNNRut );
		}
	}
	if( pNRNext == NULL )
	{
		pVNode->CreateCRect( pPtList, nPtNum );
		return FALSE;
	}
	if( pVertex->GetVtxID() == pNRNext->GetStartVtxID() )
		pVertex = GetNetworkVtx( pNRNext->GetBreadthID(), pNRNext->GetEndVtxID() );
	else
		pVertex = GetNetworkVtx( pNRNext->GetBreadthID(), pNRNext->GetStartVtxID() );
	if( pVertex )
	{
		pPtList[nPtNum].dx = pVertex->GetLog();
		pPtList[nPtNum].dy = pVertex->GetLat();
		nPtNum++;
		BOOL bReturn = GetNextRoundRut( pVNode, pFRList, pVertex, pNRStart, pNRNext, pPtList, nPtNum );
		pVNode->CreateCRect( pPtList, nPtNum );
		return bReturn;
	}
	pVNode->CreateCRect( pPtList, nPtNum );
	return FALSE;
}
BOOL CGIS_MBBNetwork::GetNextRoundRut( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], 
			CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRStart, CGIS_NetworkRoadSectionV7 *pRoadSectionNext, ST_GEO_PTXY pPtList[], int &nPtNum )
{
	if( pRStart == pRoadSectionNext )
		return TRUE;
//	CGIS_NetworkRoadSectionV7 *pNRNext = NULL;
//	CGIS_NetworkRoadSectionV7 *pNRut = NULL;
//	CGIS_FeatureRoadSectionV7 *pFtrRoadSection = NULL;
//	CGIS_BreadthMap *pBMNet = NULL;
//	CGIS_MapLayer **pLList = NULL;
//	CGIS_FeatureRoadSectionV7 **pFRuts = pFRList;
//	if( pVertex->GetVtxEdge() == 1 )
//	{
//		if( pVertex->GetBreadthID() != m_pBMNet->m_nBMID )
//		{
//			pBMNet = GetBreadthMapByID( pVertex->GetVAbutBID() );
//			if( pBMNet == NULL )
//				return FALSE;
//			pLList = (CGIS_MapLayer**)malloc(sizeof(CGIS_MapLayer*)*MAX_RLAY_NUM);
//			memcpy( pLList, m_pLList, sizeof(CGIS_MapLayer*)*MAX_RLAY_NUM );
//			pFRuts = (CGIS_FeatureRut**)malloc(sizeof(CGIS_FeatureRut*)*MAX_BRUT_NUM);
//			memset( pFRuts, 0, sizeof(CGIS_FeatureRut*)*MAX_BRUT_NUM );
//			CGIS_FeatureRut *pRoadSection = NULL;
//			LoadBLData( pBMNet, pLList, m_nFRLNum );
//			POSITION ps = NULL;
//			for( int i=0;	i<m_nFRLNum;	i++ )
//			{
//				CPtrList *pRList = NULL;
//// 				pLList[i]->IncreaseLCount( pBMNet->m_nBreadthID );
//				pLList[i]->GetObjectListMap()->Lookup( pBMNet->m_nBreadthID, pRList );
//				if( pRList )
//				{
//					ps = pRList->GetHeadPosition();
//					while( ps )
//					{
//						pRoadSection = (CGIS_FeatureRut*)pRList->GetNext(ps);
//						if( pRoadSection )
//							pFRuts[pRoadSection->GetRutID()] = pRoadSection;
//					}
//				}
//			}
//			//////////????????????????????????????????????????????????
//			if( pBMNet->m_nLoadCounter == 0 )
//				ReadBreadthNetwork( &pBMNet );
//		}
//		else
//			pFRuts = m_pFRList;
//		pFtrRoadSection = pFRuts[pVertex->GetVAbutRID()];
//		if( pFtrRoadSection )
//		{
//			CGIS_NetworkNodeRut *pNNRut = new CGIS_NetworkNodeRut;
//			if( pFtrRoadSection->GetAnnotation() )
//			{
//				char *szAnno = new char[pFtrRoadSection->GetAnnoNum()];
//				memcpy( szAnno, pFtrRoadSection->GetAnnotation(), pFtrRoadSection->GetAnnoNum() );
//				pNNRut->SetRutAnno( szAnno );
//				pNNRut->SetRutAnnoNum( pFtrRoadSection->GetAnnoNum() );
//			}
//			ST_GEO_PTXY *pPtList = new ST_GEO_PTXY[pFtrRoadSection->GetPtNum()];
//			memcpy( pPtList, pFtrRoadSection->GetPtList(), sizeof(ST_GEO_PTXY)*pFtrRoadSection->GetPtNum() );
//			pNNRut->SetRutPtList( pPtList );
//			pNNRut->SetRutPtNum( pFtrRoadSection->GetPtNum() );
//			pNNRut->SetRutID( pFtrRoadSection->GetRutID() );
//			pVNode->GetNodeRutList()->AddTail( pNNRut );
//		}
//		if( pBMNet )
//			pNRNext = GetNetworkRut( pBMNet->m_nBreadthID, pVertex->GetVAbutRID() );
//		else
//			pNRNext = GetNetworkRut( m_pBMNet->m_nBreadthID, pVertex->GetVAbutRID() );
//	}
//	else
//	{
//		for( int i=0;	i<pVertex->GetRoadSectionNum();	i++ )
//		{
//			if( pVertex->GetRoadSectionList()[i] == pRoadSectionNext->GetRutID() )
//				continue;
//			pNRut = pBMNetwork->GetNetworkRut( pVertex->GetBreadthID(), pVertex->GetRoadSectionList()[i] );
//			if( pNRut )
//			{
//				if( pNRut->GetRutType() == 0 )
//					pNRNext = pNRut;
//			}
//			pFtrRoadSection = pFRuts[pVertex->GetRoadSectionList()[i]];
//			if( pFtrRoadSection )
//			{
//				CGIS_NetworkNodeRut *pNNRut = new CGIS_NetworkNodeRut;
//				if( pFtrRoadSection->GetAnnotation() )
//				{
//					char *szAnno = new char[pFtrRoadSection->GetAnnoNum()];
//					memcpy( szAnno, pFtrRoadSection->GetAnnotation(), pFtrRoadSection->GetAnnoNum() );
//					pNNRut->SetRutAnno( szAnno );
//					pNNRut->SetRutAnnoNum( pFtrRoadSection->GetAnnoNum() );
//				}
//				ST_GEO_PTXY *pPtList = new ST_GEO_PTXY[pFtrRoadSection->GetPtNum()];
//				memcpy( pPtList, pFtrRoadSection->GetPtList(), sizeof(ST_GEO_PTXY)*pFtrRoadSection->GetPtNum() );
//				pNNRut->SetRutPtList( pPtList );
//				pNNRut->SetRutPtNum( pFtrRoadSection->GetPtNum() );
//				pNNRut->SetRutID( pFtrRoadSection->GetRutID() );
//				pVNode->GetNodeRutList()->AddTail( pNNRut );
//			}
//		}
//	}
//	if( pNRNext == NULL )
//	{
//		if( pBMNet )
//		{
//			CGIS_MapLayer *pLayer = NULL;
//			for( int i=0;	i<MAX_RLAY_NUM;	i++ )
//			{
//				pLayer = pLList[i];
//				if( pLayer )
//					pLayer->DecreaseLCount( pBMNet->m_nBreadthID );
//				pLList[i] = NULL;
//			}
//		}
//		return FALSE;
//	}
//	if( pVertex->GetVtxEdge() == 1 && pBMNet)
//	{
//		CGIS_NetworkVertexV7 *pVTemp = NULL;
//		CGIS_NetworkRoadSectionV7 *pNERut = NULL;
//		CGIS_BreadthMap *pBMap = NULL;
//		if( pBMNet )
//			pBMap = pBMNet;
//		else
//			pBMap = m_pBMNet;
//		pNERut = GetNetworkRut( pBMap->m_nBreadthID, pVertex->GetVAbutRID() );
//		if( pNERut )
//		{
//			CGIS_NetworkVertexV7 *pSVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNERut->GetStartVtxID() );
//			CGIS_NetworkVertexV7 *pEVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNERut->GetEndVtxID() );
//			if( pSVtx && pEVtx )
//			{
//				if( pSVtx->GetVtxEdge() == 1 && pEVtx->GetVtxEdge() == 1 )
//				{
//					CGIS_BreadthMap *pBTemp = GetBreadthMapByID2( pVertex->GetBreadthID() );
//					if( pBTemp )
//					{
//						if( pBTemp->m_nBMID == pSVtx->GetEdgeRoadSectionID() && pVertex->GetRoadSectionList()[0] == pSVtx->GetEdgeRoadSectionID() )
//							pVTemp = pSVtx;
//						else
//							pVTemp = pEVtx;
//					}
//					else
//						ASSERT( FALSE );
//				}
//				else if( pSVtx->GetEdge() == 1 )
//					pVTemp = pSVtx;
//				else if( pEVtx->GetEdge() == 1 )
//					pVTemp = pEVtx;
//			}
//		}
//		if( pVTemp )
//		{
//			if( pVTemp->GetVtxID() == pNRNext->GetStartVtxID() )
//				pVertex = GetNetworkVtx( pNRNext->GetBreadthID(), pNRNext->GetEndVtxID() );
//			else
//				pVertex = GetNetworkVtx( pNRNext->GetBreadthID(), pNRNext->GetStartVtxID() );
//		}
//		else
//			pVertex = NULL;
//	}
//	else
//	{
//		if( pVertex->GetVtxID() == pNRNext->GetStartVtxID() )
//			pVertex = GetNetworkVtx( pNRNext->GetBreadthID(), pNRNext->GetEndVtxID() );
//		else
//			pVertex = GetNetworkVtx( pNRNext->GetBreadthID(), pNRNext->GetStartVtxID() );
//	}
//	if( pVertex == NULL )
//		return FALSE;
//	pPtList[nPtNum].dx = pVertex->GetLog();
//	pPtList[nPtNum].dy = pVertex->GetLat();
//	nPtNum++;
//	if( nPtNum >= MAX_PTNUM_ROUND )
//		return FALSE;
//	GetNextRoundRut( pBMNetwork, pVNode, pFRuts, pVertex, pRStart, pNRNext, pPtList, nPtNum );
//	if( pBMNet )
//	{
//		CGIS_MapLayer *pLayer = NULL;
//		for( int i=0;	i<MAX_RLAY_NUM;	i++ )
//		{
//			pLayer = pLList[i];
//			if( pLayer )
//				pLayer->DecreaseLCount( pBMNet->m_nBreadthID );
//			pLList[i] = NULL;
//		}
//	}
	return TRUE;
}
////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//加载图幅中的空间属性数据
//int CGIS_MBBNetwork::LoadSpaceBreadthDataV2(
//    char* pBufOfBeforeCompress,
//    int nBufferSize,
//    int nBreadthID,
//    CGIS_FeatureRoadV7 **pFRList, 
//    int &nFNum )
//{
//////////////////////////////////////////////////////////////////////////
//	CFile aFile((int)g_hSpaceFile);
//    CFile* pFile = &aFile;
//    StuSpaceFileStructV7 :: StuIndexInfoOfBreadth* pIndex = &g_pIndexInfoArrayOfBreadthInSpaceMap[nBreadthID];
//	ASSERT(pIndex);
//    ASSERTVALID(pIndex);
//	int nBeforeCompressSize = pIndex->nBeforeCompressSize;
//	int nAfterCompressSize = pIndex->nAfterCompressSize;
//    //分配压缩前空间
//    ASSERT(nBeforeCompressSize <= nBufferSize);
//	ASSERT(pBufOfBeforeCompress);
//	 
//	//-------------------------------------------------
//	//读取并解压数据
//	{
//		char *pBufOfAfterCompress = g_pAfterCompressBuffer;//分配压缩后
//		ASSERT(pBufOfAfterCompress);
//		
//        pFile->Seek(pIndex->nOffset+g_dwOffsetBaseOfMapDataInSpaceMap,CFile::begin);
//		pFile->Read( pBufOfAfterCompress, nAfterCompressSize );
//		DWORD dwSize = nBeforeCompressSize;
//		int nRet = uncompress( (unsigned char*)pBufOfBeforeCompress, &dwSize, (unsigned char*)pBufOfAfterCompress, nAfterCompressSize  );
//		ASSERT(nRet == Z_OK);
//		ASSERT(nBeforeCompressSize == dwSize);
//	}
//	//-----------------------------------------------------
//	
//	{	
//		char* pCharAddr		= NULL;
//		char* pShortAddr	= NULL;
//		char* pDwordAddr	= NULL;
//		
//		StuSpaceFileStructV7::StuBreadthHeaderInfo* pHeader = (StuSpaceFileStructV7::StuBreadthHeaderInfo*)pBufOfBeforeCompress;
//       
//        DWORD dwBuddyDataSize = pHeader->nCharDataBuddyLength+pHeader->nShortDataBuddyLength+pHeader->nDwordDataBuddyLength;
//        ASSERTVALID(pHeader);
//        unsigned long nFtrSize = sizeof(StuSpaceFileStructV7::StuGisFeatureRoadSection);
//        unsigned long nBuddyOffset = sizeof(StuSpaceFileStructV7::StuBreadthHeaderInfo) + pHeader->nFeatureCount*nFtrSize;
//        char* pBuddyData = pBufOfBeforeCompress+nBuddyOffset;
//		if(dwBuddyDataSize > 0)
//		{
//			pCharAddr	= pBuddyData;
//			pShortAddr	= pCharAddr + pHeader->nCharDataBuddyLength;
//			pDwordAddr	= pShortAddr+ pHeader->nShortDataBuddyLength;
//			if(	*( (DWORD*)pCharAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)	)
//			{
//				pCharAddr	+= 4;	
//			}
//			if( *( (DWORD*)pShortAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0))
//			{
//				pShortAddr	+= 4;
//			}
//			if( *( (DWORD*)pDwordAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0))
//			{
//				pDwordAddr	+= 4;
//			}
//		}
//		
//        StuSpaceFileStructV7::StuGisFeatureRoad* pArrayOfStGisFeatureRoadSection = 
//            (StuSpaceFileStructV7::StuGisFeatureRoad*)(pBufOfBeforeCompress+sizeof(StuSpaceFileStructV7::StuBreadthHeaderInfo));
//        
//        nFNum = pHeader->nFeatureCount;
//       for(int i = 0; i < pHeader->nFeatureCount; i++)
//        {
//            CGIS_FeatureRoadV7* pFtr = pFRList[i];
//            ASSERT(pFtr);
//            pFtr->m_pStData = &pArrayOfStGisFeatureRoadSection[i];
//            char* pCharAddrOld = pCharAddr;
//            char* pShortAddrOld = pShortAddr;
//            char* pDwordAddrOld = pDwordAddr;
//            pFtr->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
//            ASSERTVALID(pFtr);
//         
//        }
//    }
//		
//    return 0;
//}




/**
 * @brief 填充非边缘NODE,既要填充空间数据,又要填充语音数据
 *
 * @param 
 * @return
*/
BOOL CGIS_MBBNetwork::FullPathNodeV2(
    CGIS_FeatureRoadSectionV7 **        pFRList, 
    CGIS_NetworkVertexV7 *              pVertex, 
    CGIS_NetworkRoadSectionV7 *         pRoadSectionPrevious,
    CGIS_NetworkRoadSectionV7 *         pRoadSectionNext, 
    CPtrList *                          pPathpointList,
    POSITION &                          pos //[out]
    )
{
    ASSERT(pFRList);
	CGIS_Pathpoint *pPPoint = NULL;
	CGIS_FeatureRoadSectionV7 *pFtrRoadSection = NULL;
	BYTE byRAType = 1;

	m_bTFlay = TRUE;
	ST_GEO_PTXY ptPos;
	ptPos.dx = pVertex->GetLog();
	ptPos.dy = pVertex->GetLat();


    //////////////////////////////////////////////////////////////////////////
    //填充语音信息

    //
    /**
     * @brief 构造机动的条件
     *
     * 1-前一条路段是交叉路口内路段且前一条路段为第一条路段(m_pVoiceNode==NULL)
     * 2-前一条路段是交叉路口内路段且后一条路段为环岛
     * 3-第一条路段不是交叉路口内路段
    */
	if( 
        pRoadSectionPrevious->GetRutType() != 4 
        || 
        (
            pRoadSectionPrevious->GetRutType()==4 
            && 
            (pRoadSectionNext->GetRutType()==0||m_pVoiceNode==NULL)
        ) 
        )
	{
		char* szAnnotationPrevious = NULL;
		char* szAnnotationNext = NULL;
        
        if(pRoadSectionPrevious->GetRutID() != 0xffff)
        {
            if( pFRList[pRoadSectionPrevious->GetRutID()] )
                if( pFRList[pRoadSectionPrevious->GetRutID()]->GetAnnotation() )
                    szAnnotationPrevious = pFRList[pRoadSectionPrevious->GetRutID()]->GetAnnotation();
        }
        if(pRoadSectionNext->GetRutID() != 0xffff)
        {
            if( pFRList[pRoadSectionNext->GetRutID()] )
                if( pFRList[pRoadSectionNext->GetRutID()]->GetAnnotation() )
                    szAnnotationNext = pFRList[pRoadSectionNext->GetRutID()]->GetAnnotation();
        }
        
        if(!szAnnotationPrevious)szAnnotationPrevious = "";
        if(!szAnnotationNext)szAnnotationNext = "";
        BOOL bSameRoadSectionName = !strcmp(szAnnotationPrevious,szAnnotationNext);

        //此节点的路段数大于2或两条路的名字不同
        int nTotalVertexRoadSectionNum = pVertex->GetRoadSectionNum() + pVertex->GetLowLayerRoadSectionNum();
		if( nTotalVertexRoadSectionNum > 2 || !bSameRoadSectionName  )
		{
			if( m_pVoiceNode )
			{
                //路口内道路直接与环岛路段相连，属于机动,要对上一次做出的机动做一些修正
				if( pRoadSectionPrevious->GetRutType()==4 && pRoadSectionNext->GetRutType()==0 )
				{
                    //根据下一路段填充和路段相关的语音信息
					FullVoiceRutInfo( pRoadSectionNext, m_pVoiceNode->GetVRutInfo() );
					m_pVoiceNode->GetVRutInfo()->byAdd = 10;
					m_pVoiceNode->SetTurnAngle( m_nAPre-m_nANext );
                    //根据前一路段填充和节点相关的语音信息.
					FullVoiceNodeInfo( m_pFRPre, m_pVoiceNode, m_pVoiceNode->GetVNodeInfo() );
					m_pVoiceNode = NULL;
				}
			}



            //正常构造一个机动点
			m_pVoiceNode = NULL;
			m_pVoiceNode = new CGIS_PathpointNode;
			pPPoint = m_pVoiceNode;
			m_pVoiceNode->SetPathpointType( EN_PATHPOINTTYPE_NODE );
			m_pVoiceNode->SetPathpointPosition( ptPos );
			m_pVoiceNode->SetNodeVtxID( pVertex->GetVtxID() );
			m_pVoiceNode->SetPBreadthID( 0 );
			pos = pPathpointList->InsertAfter( pos, m_pVoiceNode );

            
			int nVRNum = pVertex->GetRoadSectionNum();
			ASSERT( nVRNum > 0 );
			CGIS_NetworkRoadSectionV7 *pNRut = NULL;

            //判环岛
			if( pRoadSectionPrevious->GetRutType()==0 || pRoadSectionNext->GetRutType()==0 )
			{
				if( m_bRoundaboutFlag == FALSE )
                {
                    m_nRoundaboutOrder = 0;

                }

                //填充方式（用于路口扩大图）1：普通路段　2：十字路口　3：环岛
				byRAType = 3;
			}
            //遍历当前节点的所有路段,
			for( int i=0;	i<nVRNum;	i++ )
			{
				if( pVertex->GetRoadSectionList()[i] == pRoadSectionPrevious->GetRutID() )
                {
                    m_nAPre = pVertex->GetRoadSectionTurnList()[i];
                }
				else
				{
					if( pVertex->GetRoadSectionList()[i] == pRoadSectionNext->GetRutID() )
                    {
                        m_nANext = pVertex->GetRoadSectionTurnList()[i];
                    }
                    
                    
					pNRut = GetNetworkRut( pVertex->GetBreadthID(), pVertex->GetRoadSectionList()[i] );
					if( pNRut )
					{
						if( pNRut->GetRutType() == 4 && byRAType != 3 )
							byRAType = 2;
						else if( pNRut->GetRutType() == 0 )
							byRAType = 3;
                        
						if( byRAType == 3 )
						{
							if( (pNRut!=pRoadSectionNext || pRoadSectionNext->GetRutType()!=0) 
                                && 
                                pNRut->GetRutType() != 0 
                                && 
								GetNextRutVtx(pVertex, pNRut,TRUE) != NULL )
                                
								m_nRoundaboutOrder++;
						}
					}
				}
			}
			if( byRAType == 3 )
            {
                //要说明是第几个环岛出口
                m_pVoiceNode->SetRMOrder( m_nRoundaboutOrder );
            }


			if( !m_bRoundaboutFlag)
			{
                //填充路口扩大图
				FullPathNodeRuts( 
                    m_pVoiceNode,
                    pFRList,
                    pVertex, 
                    pRoadSectionPrevious,
                    pRoadSectionNext, 
                    byRAType );

//                stuPairIDItem* pItem = GetVertexDetailIDByMergeID(pVertex->GetVtxID());
//                ASSERT(pItem);
//                unsigned long nDetailVertexID = pItem->nSourceID;
//                short nAngleEnter   = -1;
//                short nAngleExit    = -1;
//                for(int i = 0; i < pVertex->GetRoadSectionNum(); i++)
//                {
//                    if(pVertex->GetRoadSectionList()[i] == pRoadSectionPrevious->GetRutID())
//                    {
//                        nAngleEnter = pVertex->GetRoadSectionTurnList()[i];
//                        continue;
//                    }
//                    
//                    if(pVertex->GetRoadSectionList()[i] == pRoadSectionNext->GetRutID())
//                    {
//                        nAngleExit = pVertex->GetRoadSectionTurnList()[i];
//                        continue;
//                    }    
//                    
//                }
//                ASSERT(nAngleEnter != -1);
//                ASSERT(nAngleExit  != -1);
//                
//                m_pMap->FullPathNodeRuts(
//                    m_pVoiceNode,
//                    nDetailVertexID,
//                    nAngleEnter,
//                    nAngleExit          
//                     )

                
				if( byRAType == 3 )
					m_nAPre = -1800;
			}


			if( pRoadSectionNext->GetRutType() == 0 )
				m_bRoundaboutFlag = TRUE;
			else 
				m_bRoundaboutFlag = FALSE;
		}
		else//路段数<2且名字相同则放个拐点即可
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( ptPos );
			pPPoint = pPathPt;
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathpointList->InsertAfter( pos, pPathPt );
		}
	}
	else
	{
		CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
		pPPoint = pPathPt;
		pPathPt->SetPathpointPosition( ptPos );
		pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
		pos = pPathpointList->InsertAfter( pos, pPathPt );
	}

    //////////////////////////////////////////////////////////////////////////
    //填充空间部分的数据
    if(pRoadSectionNext->GetRutID() != 0xffff)
    {
	    pFtrRoadSection = pFRList[pRoadSectionNext->GetRutID()];
    }
	if( pRoadSectionNext->GetRutType() != 4 
        || 
        (byRAType != 2 && pRoadSectionPrevious->GetRutType() != 4)
        )
	{
		if( m_pVoiceNode )
		{
			int nVRNum = pVertex->GetRoadSectionNum();
			int nAngNextRut = 0;
			ASSERT( nVRNum > 0 );
			for( int i=0;	i<nVRNum;	i++ )
			{
				if( pVertex->GetRoadSectionList()[i] == pRoadSectionNext->GetRutID() )
				{
					nAngNextRut = pVertex->GetRoadSectionTurnList()[i];
					break;
				}
			}
            //当前语音点中写入下一路段ID
			m_pVoiceNode->SetNextRutID( pRoadSectionNext->GetRutID() );
            
			if( pFtrRoadSection )
			{
				if( pFtrRoadSection->GetAnnotation() != NULL )
				{
                    if(m_pVoiceNode->GetRutAnno())
                    {
                        delete m_pVoiceNode->GetRutAnno();
                    }
					char* szAnno = new char[pFtrRoadSection->GetAnnoNum()];
					memcpy( szAnno, pFtrRoadSection->GetAnnotation(), pFtrRoadSection->GetAnnoNum() );
                    //当前语音点中写入下一路段的名字
					m_pVoiceNode->SetRutAnno( szAnno );
					m_pVoiceNode->SetRutAnnoNum( pFtrRoadSection->GetAnnoNum() );
				}
			}
            else
            {
                TRACE0("error.当前路段没有匹配到全国路网中路段造成NODE的名字缺失\n");
            }
            
			m_pVoiceNode->SetRType( pRoadSectionNext->GetType() );
			m_pVoiceNode->SetRutType( pRoadSectionNext->GetRutType() );
			FullVoiceRutInfo( pRoadSectionNext, m_pVoiceNode->GetVRutInfo() );

            if( m_bTFlay )//连续机动
            {
                m_pVoiceNode->SetTurnAngle( m_nAPre-nAngNextRut );
            }
			else
            {
                m_pVoiceNode->SetTurnAngle( 180 );
            }

			FullVoiceNodeInfo( m_pFRPre, m_pVoiceNode, m_pVoiceNode->GetVNodeInfo() );
			if( m_pVoiceNode->GetVNodeInfo()->byAngle == 5 && m_byMRNum > 0 )
            {
                m_pVoiceNode->GetVNodeInfo()->byIMI = 23;
            }

            
            //////////////////////////////////////////////////////////////////////////
            //中间数据重置,路段数据更新为当前
			m_pVoiceNode = NULL;
			m_byMRNum = 0;
			if( m_pFRPre )
            {
                StuSpaceFileStructV7::StuGisFeatureRoadSection* pStData =  m_pFRPre->GetDataAddr();
                ASSERT(pStData);
                delete pStData;
                char* pszAnno = m_pFRPre->GetAnnotation();
                if(pszAnno)
                {
                    delete pszAnno;
                }
                ST_GEO_PTXY* pPtList = m_pFRPre->GetPtList();
                ASSERT(pPtList);
                
                delete []pPtList;
                delete m_pFRPre;

            }
			m_pFRPre = NULL;
			if( pFtrRoadSection )
            {
                m_pFRPre = (CGIS_FeatureRoadSectionV7*)pFtrRoadSection->Clone();
            } 
		}
	}
	else
    {
        //当前节点的前一路段为路口内路段,后一路段为路口内路段
        m_byMRNum++;
    }

	if( pFtrRoadSection == NULL )
		return FALSE;

    //////////////////////////////////////////////////////////////////////////
    //不填空间数据
	return TRUE;
}


int CGIS_MBBNetwork::LoadBreadthDataV2( unsigned long nBreadthID )
{
    CGIS_MBBNetwork::enErrorCode eErrorCode;
    return LoadBreadthData(nBreadthID,&eErrorCode);
}
void CGIS_MBBNetwork::SetGeoMap( CGIS_GeoMap *pMap )
{
	m_pMap = pMap;
}
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_NetworkNodeV7::DumpState() const	
{  
    TRACE2("\tRut%.4d->Vtx%.4d",pRoadSection->GetRutID(),pVertex->GetVtxID());
    switch(pRoadSection->GetDirection())
    {
    case 0:
        TRACE2("(%.4d<->%.4d)\n",pRoadSection->GetStartVtxID() ,pRoadSection->GetEndVtxID());
        break;
    case 1:
        TRACE2("(%.4d ->%.4d)\n",pRoadSection->GetStartVtxID() ,pRoadSection->GetEndVtxID());
        break;
    case 2:
        TRACE2("(%.4d ->%.4d)\n",pRoadSection->GetEndVtxID() ,pRoadSection->GetStartVtxID());
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    TRACE2("\t%f,%f\n",pVertex->GetLog(),pVertex->GetLat());
    Sleep(10);
}

void CGIS_NetworkNodeV7::AssertValid() const	
{   


}
#endif
//////////////////////////////////////////////////////////////////////////