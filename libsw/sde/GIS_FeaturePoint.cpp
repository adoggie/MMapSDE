
#include "GIS_FeaturePoint.h"

CGIS_FeaturePoint::CGIS_FeaturePoint(EnGeoObjType enOType)
	: CGIS_Feature( enOType ){
	m_szAddress = NULL;
	m_szPhone = NULL;
	m_nAddLen = 0;
	m_nPhoneLen = 0;
	m_nPOIType = 0;
	m_nIconID = 0;
	m_nMemmoryType = 0;
}

CGIS_FeaturePoint::~CGIS_FeaturePoint(){
    switch(m_nMemmoryType){
    case 0:{
            if( m_szAddress ){
                free(m_szAddress);
            }
            if( m_szPhone ){
                free(m_szPhone);
            }    
        }
        break;
    case 1:
        {
            if( m_szAddress ){
                m_pMemMngOfBuddyData->newfree(m_szAddress);                
            }
            if( m_szPhone ){
                m_pMemMngOfBuddyData->newfree(m_szPhone);
            }    
        }
        break;
    case 2:{
            if( m_szAddress ){
                m_pForQueryMemMngOfBuddyData->newfree(m_szAddress);
            }
            if( m_szPhone ){
                m_pForQueryMemMngOfBuddyData->newfree(m_szPhone);
            }    
        }
        break;
    case 3:
        {
            if( m_szAddress ){
                m_pForNetworkMemMngOfBuddyData->newfree(m_szAddress);
            }
            if( m_szPhone ){
                m_pForNetworkMemMngOfBuddyData->newfree(m_szPhone);
            }    
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }
}


BOOL CGIS_FeaturePoint::SetAddressInfo(int nLength,char* strText){
    switch(m_nMemmoryType){
    case 0:
        SetAddressInfoAt(NULL,nLength,strText);
        break;
    case 1:
				SetAddressInfoAt(m_pMemMngOfBuddyData,nLength,strText);        
        break;
    case 2:
				SetAddressInfoAt(m_pForQueryMemMngOfBuddyData,nLength,strText);        
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    return TRUE;
}

BOOL CGIS_FeaturePoint::SetAddressInfoAt(IMemoryMng* pMemoryMng,int nLength,char* strText){
    //分配新的数据空间
    int nByteLength = sizeof(char)*nLength;
    char* pBuf = NULL;
    if(nByteLength){
        //ASSERT(nLength-1 == strlen(strText));
        if(!pMemoryMng){
            pBuf = (char*)malloc(nByteLength);
        }else{
            pBuf = (char*)pMemoryMng->newmalloc(nByteLength); //缓冲分配
        }
      memcpy(pBuf,strText,nByteLength);
			pBuf[nByteLength-1]='\0';			
    }
		m_szAddress = pBuf;
		m_nAddLen = nLength;
    return TRUE;
}

BOOL CGIS_FeaturePoint::SetPhoneInfo(int nLength,char* strText){
    switch(m_nMemmoryType){
		case 0:
				SetPhoneInfoAt(NULL,nLength,strText);       
        break;
    case 1:
			SetPhoneInfoAt(m_pMemMngOfBuddyData,nLength,strText);      
        break;
    case 2:      
				SetPhoneInfoAt(m_pForQueryMemMngOfBuddyData,nLength,strText);     
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    
    return TRUE;
}

BOOL CGIS_FeaturePoint::SetPhoneInfoAt(IMemoryMng* pMemoryMng,int nLength,char* strText){
    //分配新的数据空间
    int nByteLength = sizeof(char)*nLength;
    char* pBuf = NULL;
    if(nByteLength){
        ASSERT(nLength-1 == strlen(strText));
        if(!pMemoryMng){
            pBuf = (char*)malloc(nByteLength);
        }else{
            pBuf = (char*)pMemoryMng->newmalloc(nByteLength);
        }
        memcpy(pBuf,strText,nByteLength);
        pBuf[nByteLength-1] = 0;
    }
    m_szPhone = pBuf;
    m_nPhoneLen = nLength;
    return TRUE;
}

void CGIS_FeaturePoint::SetAddressInfoDirect(int nLength,char* strText){
    m_szAddress = strText;
    m_nAddLen = nLength;
}

void CGIS_FeaturePoint::SetPhoneInfoDirect(int nLength,char* strText){ 
    m_szPhone = strText;
    m_nPhoneLen = nLength;
}

//系统堆上复制
CGIS_Feature* CGIS_FeaturePoint::Clone(){
    //ASSERT(m_pForQueryMemMngOfFeaturePoint);
  CGIS_FeaturePoint* pFtrAddr = (CGIS_FeaturePoint*)malloc(sizeof(CGIS_FeaturePoint));
  if(!pFtrAddr)
      return NULL;
  CGIS_FeaturePoint* pFtr = new (pFtrAddr) CGIS_FeaturePoint(m_enOType);
  pFtr->m_nMemmoryType = 0;
  pFtr->SetPtInfo(1,m_pPtList);
  pFtr->SetAnnotationInfo(m_nAnno,m_bstrAnno);
  pFtr->SetAddressInfo(m_nAddLen,m_szAddress);
  pFtr->SetPhoneInfo(m_nPhoneLen,m_szPhone);
	pFtr->SetPointObjectType(m_nPOIType);
	pFtr->m_nBreadthID = m_nBreadthID;
	pFtr->SetIconID( m_nIconID );
  
	return pFtr;
}


//查询堆上分配
CGIS_Feature* CGIS_FeaturePoint::CloneV2(){
	// 或者直接调用 new CGIS_FeaturePoint来实现从缓冲堆上分配
  CGIS_FeaturePoint* pFtrAddr = (CGIS_FeaturePoint*)m_pForQueryMemMngOfFeaturePoint->newmalloc(sizeof(CGIS_FeaturePoint));
	CGIS_FeaturePoint* pFtr = new (pFtrAddr) CGIS_FeaturePoint(m_enOType);
  pFtr->m_nMemmoryType = 2; //查询堆
	pFtr->SetPtInfo(1,m_pPtList);
  pFtr->SetAnnotationInfo(m_nAnno,m_bstrAnno);
  pFtr->SetAddressInfo(m_nAddLen,m_szAddress);
  pFtr->SetPhoneInfo(m_nPhoneLen,m_szPhone);
	pFtr->SetPointObjectType(m_nPOIType);
	pFtr->m_nBreadthID = m_nBreadthID;
	pFtr->SetIconID( m_nIconID );
	return pFtr;
}

#ifdef MEMORY_SELF_MANAGE

//--------------------------------------
//构造CGIS_FeaturePoint对象
void* CGIS_FeaturePoint::operator new(size_t size){	
	void* pRtn = m_pMemMngOfFeaturePoint->newmalloc(size);
  return pRtn;
}

void* CGIS_FeaturePoint::operator new(size_t size,CGIS_FeaturePoint* pObjAddr){
  return pObjAddr;
}

void* CGIS_FeaturePoint::operator new(size_t size,IMemoryMng2* pMemoryMng){
  void* pRtn = pMemoryMng->newmalloc();  
	if(!pRtn)
	{  /*
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
        }*/
		pRtn = new char[size];
	}
	return pRtn;
}

void CGIS_FeaturePoint::operator delete (void* p,size_t size){
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
            m_pMemMngOfFeaturePoint->newfree(p);
        }
        break;
    case 2:
        {
            m_pForQueryMemMngOfFeaturePoint->newfree(p);
        }
        break;
    case 3:
        {
            m_pForNetworkMemMngOfFeaturePoint->newfree(p);
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    
}
#endif
