// GIS_FeaturePointV2.h: interface for the CGIS_FeaturePointV2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATUREPOINTV2_H__F75F24CF_3EF5_4C7D_BF00_1EEEF0D05189__INCLUDED_)
#define AFX_GIS_FEATUREPOINTV2_H__F75F24CF_3EF5_4C7D_BF00_1EEEF0D05189__INCLUDED_

#include "GIS_FeatureV2.h"
#include "SpaceFileStructV2.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CGIS_FeaturePointV2  : public CGIS_FeatureV2
{
public:
	CGIS_FeaturePointV2();
	virtual ~CGIS_FeaturePointV2();

    StuSpaceFileStructV2::StuGisFeaturePoint* GetDataAddr(){return &m_stData;};
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
	void MakeBuddyIndependent();    
//////////////////////////////////////////////////////////////////////////
public:
    char* GetAnnotation(){	return m_pAnno;};
    int GetAnnoNum(){return m_stData.nAnnoNum;};
    void SetIconID( UINT nIconID );
	UINT GetIConID();
	unsigned short GetPointObjectType();
    void SetPointObjectType( unsigned short nPOIType );
    BYTE GetStrAddressLength(){return m_stData.nAddressNum;};
	void SetStrAddressLength( BYTE nAddLen );
    char* GetAddress(){return m_pAddress;};
	void SetAddress( char *szAddress );
    BYTE    GetStrPhoneLength(){return m_stData.nPhoneNum;};
    char*   GetPhoneNum(){return m_pPhone;};
			
//////////////////////////////////////////////////////////////////////////
//protected:
public:
    StuSpaceFileStructV2::StuGisFeaturePoint m_stData;
    char* m_pAnno;
    char* m_pAddress;
    char* m_pPhone;
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	virtual void DumpState() const;		
	virtual void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////
};

inline void CGIS_FeaturePointV2::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
{
	if(m_stData.nAnnoNum)
	{
		m_pAnno	= (char*)(pCharAddr);
		pCharAddr += m_stData.nAnnoNum*sizeof(char);
	}
	if(m_stData.nAddressNum)
	{
		m_pAddress	= (char*)(pCharAddr);
		pCharAddr += m_stData.nAddressNum*sizeof(char);
	}
	if(m_stData.nPhoneNum)
	{
		m_pPhone	= (char*)(pCharAddr);
		pCharAddr += (m_stData.nPhoneNum)*sizeof(char);
	}
}
inline void CGIS_FeaturePointV2::MakeBuddyIndependent()
{
    ASSERT(!m_bBuddyIndependent);
    //////////////////////////////////////////////////////////////////////////
    int nSize = 0;
    void* pDest = NULL;
    void* pSrc = NULL;
    
	if(m_stData.nAnnoNum)
	{
        ASSERT(m_pAnno);
        nSize = m_stData.nAnnoNum;
        pSrc = m_pAnno;
        pDest = malloc(nSize);
        memcpy(pDest,pSrc,nSize);
        m_pAnno = (char*)pDest;
	}
	if(m_stData.nAddressNum)
	{
        ASSERT(m_pAddress);
        nSize = m_stData.nAddressNum;
        pSrc = m_pAddress;
        pDest = malloc(nSize);
        memcpy(pDest,pSrc,nSize);
        m_pAddress = (char*)pDest;
	}
	if(m_stData.nPhoneNum)
	{

        ASSERT(m_pPhone);
        nSize = m_stData.nPhoneNum;
        pSrc = m_pPhone;
        pDest = malloc(nSize);
        memcpy(pDest,pSrc,nSize);
        m_pPhone = (char*)pDest;
	}
    //////////////////////////////////////////////////////////////////////////
    m_bBuddyIndependent = TRUE;
}
#endif // !defined(AFX_GIS_FEATUREPOINTV2_H__F75F24CF_3EF5_4C7D_BF00_1EEEF0D05189__INCLUDED_)
