//#include "stdafx.h"
//#include "SDELib.h"
#include "../MemoryMngLib/IMemoryMng.h"
#include "../MemoryMngLib/IMemoryMng2.h"
#include "GIS_FeatureLine.h"

CGIS_FeatureLine::CGIS_FeatureLine(EnGeoObjType enOType): CGIS_Feature( enOType ){
	m_nPtNum = 0;
	m_pPart = NULL;
	m_nPartNum = 0;
}

CGIS_FeatureLine::~CGIS_FeatureLine(){
		//0-从系统堆上分配 1-从空间数据堆上分配 2-从查询堆上分配 3.从路网堆上分配
  switch(m_nMemmoryType){
  case 0:
		if( m_pPart ){
			free(m_pPart);
     }   
    break;
  case 1:
		if( m_pPart ){
			m_pMemMngOfBuddyData->newfree(m_pPart);        
		}         
    break;
  case 2:
		if( m_pPart ){
			m_pForQueryMemMngOfBuddyData->newfree(m_pPart);
    }         
    break;
  case 3:
		if( m_pPart ){
			m_pForNetworkMemMngOfBuddyData->newfree(m_pPart);
    }
    break;
  default:
      ASSERT(FALSE);
      break;
  }
}

int  CGIS_FeatureLine::GetPartNum( ){
	return m_nPartNum;
}

int* CGIS_FeatureLine::GetPart( ){
	return m_pPart;
}

void CGIS_FeatureLine::SetRectObj( ST_GEO_RECT rtObj ){
	//memcpy(&m_rtObj, &rtObj, sizeof(ST_GEO_RECT));
	m_rtObj = rtObj;
}

ST_GEO_RECT CGIS_FeatureLine::GetRectObj( ){
	return m_rtObj;
}


BOOL CGIS_FeatureLine::SetPartInfo(int nLength,int* pList){
    switch(m_nMemmoryType){
    case 0:{
			SetPartInfoAt(NULL,nLength,pList);
        }
        break;
    case 1:
        {
            SetPartInfoAt(m_pMemMngOfBuddyData,nLength,pList);
        }
        break;
    case 2:
        {
            SetPartInfoAt(m_pForQueryMemMngOfBuddyData,nLength,pList);
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    return TRUE;
}

BOOL CGIS_FeatureLine::SetPartInfoAt(IMemoryMng* pMemoryMng,int nLength,int* pList){
    //旧的数据空间一定不存在
    ASSERT(!m_pPart);
    
    //分配新的数据空间
    int nByteLength = sizeof(int)*nLength;
    char* pBuf = NULL;
    if(nByteLength){
        if(!pMemoryMng){
            pBuf = (char*)malloc(nByteLength);
        }
        else{
          pBuf = (char*)pMemoryMng->newmalloc(nByteLength);
        }
        memcpy(pBuf,pList,nByteLength);   
    }
    //////////////////////////////////////////////////////////////////////////
    m_pPart = (int*)pBuf;
    m_nPartNum = nLength;
    return TRUE;
}

void CGIS_FeatureLine::SetPartInfoDirect(short nLength,int* pList){
    m_pPart = pList;
    m_nPartNum = nLength;
}
//系统堆上分配
CGIS_Feature* CGIS_FeatureLine::Clone(){
  ASSERT(m_pForQueryMemMngOfFeatureLine);
  CGIS_FeatureLine* pFtrAddr = (CGIS_FeatureLine*)malloc(sizeof(CGIS_FeatureLine));
  if(!pFtrAddr)
      return NULL;
	CGIS_FeatureLine* pFtr = new (pFtrAddr) CGIS_FeatureLine(m_enOType); //堆上分配
  pFtr->m_nMemmoryType = 0;

  pFtr->SetPartInfo(m_nPartNum,m_pPart);
  pFtr->SetPtInfo(m_nPtNum,m_pPtList);
  pFtr->SetAnnotationInfo(m_nAnno,m_bstrAnno);

	pFtr->m_nBreadthID = m_nBreadthID;
	pFtr->SetRectObj( m_rtObj );
	return pFtr;
}
//查询 缓冲上分配
CGIS_Feature* CGIS_FeatureLine::CloneV2(){
  ASSERT(m_pForQueryMemMngOfFeatureLine);
  CGIS_FeatureLine* pFtrAddr = (CGIS_FeatureLine*)m_pForQueryMemMngOfFeatureLine->newmalloc(sizeof(CGIS_FeatureLine));
	CGIS_FeatureLine* pFtr = new (pFtrAddr) CGIS_FeatureLine(m_enOType);
    pFtr->m_nMemmoryType = 2;

	pFtr->SetPartInfo(m_nPartNum,m_pPart);
  pFtr->SetPtInfo(m_nPtNum,m_pPtList);
  pFtr->SetAnnotationInfo(m_nAnno,m_bstrAnno);

	pFtr->m_nBreadthID = m_nBreadthID;
	pFtr->SetRectObj( m_rtObj );
	return pFtr;
}

//start
#ifdef MEMORY_SELF_MANAGE
//请求缓冲数据
void* CGIS_FeatureLine::operator new(size_t size){	
  void* pRtn = m_pMemMngOfFeatureLine->newmalloc(size);	
  return pRtn;
}

//在已分配的堆上构造对象pObjAddr
void* CGIS_FeatureLine::operator new(size_t size,CGIS_FeatureLine* pObjAddr){
  return pObjAddr;
}

void* CGIS_FeatureLine::operator new(size_t size,IMemoryMng2* pMemoryMng){
    
  void* pRtn = pMemoryMng->newmalloc();
    
	if(!pRtn) //分配不出来便从系统堆上分配
	{ /*
        if(CGIS_Feature::m_dwOverflowTickCount != 0xffffffff)
        {
            if((GetTickCount() - CGIS_Feature::m_dwOverflowTickCount) > 300000)
            {
                CGIS_Feature::m_dwOverflowTickCount = 0xffffffff;
							THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }

            pRtn = new char[size];
        }
        else
        {
            CGIS_Feature::m_dwOverflowTickCount = GetTickCount();
            pRtn = new char[size];
        }
				*/
		pRtn = new char[size];
	}
	return pRtn;
}

void CGIS_FeatureLine::operator delete (void* p,size_t size){
	CGIS_Feature* pFtr = (CGIS_Feature*)p;
    switch(pFtr->m_nMemmoryType)
    {
    case 0:
        {
            free(p);
        }
        break;
    case 1:
        {
            m_pMemMngOfFeatureLine->newfree(p);
        }
        break;
    case 2:
        {
            m_pForQueryMemMngOfFeatureLine->newfree(p);
        }
        break;
    case 3:
        {
            m_pForNetworkMemMngOfFeatureLine->newfree(p);
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }
}
#endif
