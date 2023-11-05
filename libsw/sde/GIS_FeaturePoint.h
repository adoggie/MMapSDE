
#ifndef _GIS_FEATURE_POINT_H
#define _GIS_FEATURE_POINT_H

#include "swBase.h"

#include "GIS_Feature.h"

class swEXPORT CGIS_FeaturePoint : public CGIS_Feature{
public:
	CGIS_FeaturePoint(EnGeoObjType enOType);
	virtual ~CGIS_FeaturePoint();
public:
	virtual CGIS_Feature* Clone();
	virtual CGIS_Feature* CloneV2();
	void SetIconID( UINT nIconID );
	UINT GetIConID( );
	unsigned short GetPointObjectType( );
	void SetPointObjectType( unsigned short nPOIType );
	BYTE GetStrAddressLength( );
	void SetStrAddressLength( BYTE nAddLen );
	char* GetAddress( );
	void SetAddress( char *szAddress );
	BYTE GetStrPhoneLength( );
	void SetStrPhoneLength( BYTE nPhoneLen );
	char* GetPhoneNum( );
	void SetPhoneNum( char *szPhone );
  BOOL SetAddressInfo(int nLength,char* strText);
  BOOL SetPhoneInfo(int nLength,char* strText);
  void SetAddressInfoDirect(int nLength,char* strText);
  void SetPhoneInfoDirect(int nLength,char* strText);

#ifdef MEMORY_SELF_MANAGE
public:
	void* operator new(size_t size);
	void* operator new(size_t size,CGIS_FeaturePoint* pObjAddr);
    void* operator new(size_t size,IMemoryMng2* pMemoryMng);
	void operator delete (void*,size_t);
#endif	

protected:  
    BOOL SetAddressInfoAt(IMemoryMng* pMemoryMng,int nLength,char* strText);
    BOOL SetPhoneInfoAt(IMemoryMng* pMemoryMng,int nLength,char* strText);
//////////////////////////////////////////////////////////////////////////

private:
	UINT	m_nIconID;
	unsigned short m_nPOIType;	//点类型
	BYTE	m_nAddLen;			//地址串长度
	char	*m_szAddress;		//地址
	BYTE	m_nPhoneLen;		//电话串长度
	char	*m_szPhone;			//电话号码
};

//////////////////////////////////////////////////////////////////////////
//inline 方法
inline void CGIS_FeaturePoint::SetIconID( UINT nIconID )
{
	m_nIconID = nIconID;
}
inline UINT CGIS_FeaturePoint::GetIConID( )
{
	return m_nIconID;
}
inline unsigned short CGIS_FeaturePoint::GetPointObjectType( )
{
	return m_nPOIType;
}
inline void CGIS_FeaturePoint::SetPointObjectType( unsigned short nPOIType )
{
	m_nPOIType = nPOIType;
}
inline BYTE CGIS_FeaturePoint::GetStrAddressLength( )
{
	return m_nAddLen;
}
inline void CGIS_FeaturePoint::SetStrAddressLength( BYTE nAddLen )
{
	m_nAddLen = nAddLen;
}
inline char* CGIS_FeaturePoint::GetAddress( )
{
	return m_szAddress;
}
inline void CGIS_FeaturePoint::SetAddress( char* strText)
{
    m_szAddress = strText;
}

inline BYTE CGIS_FeaturePoint::GetStrPhoneLength( )
{
	return m_nPhoneLen;
}
inline void CGIS_FeaturePoint::SetStrPhoneLength( BYTE nPhoneLen )
{
	m_nPhoneLen = nPhoneLen;
}
inline char* CGIS_FeaturePoint::GetPhoneNum( )
{
	return m_szPhone;
}
inline void CGIS_FeaturePoint::SetPhoneNum( char* strText)
{
	m_szPhone = strText;
}
#endif 
