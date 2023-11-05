// TempLayerInfo.h: interface for the CTempLayerInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPLAYERINFO_H__1B19ACA3_7550_423F_B2C1_D05F5A96FAD8__INCLUDED_)
#define AFX_TEMPLAYERINFO_H__1B19ACA3_7550_423F_B2C1_D05F5A96FAD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTempLayerInfo  
{
public:
	void SetMinGRScale( double dbMinGR );
	void SetMaxGRScale( double dbMaxGR );
	void SetMinARScale( double dbMinGR );
	void SetMaxARScale( double dbMaxGR );
	void SetLayerState( BOOL bState );
	void SetIconID(int nIconID);
	BOOL GetIconID(int& nIconID);
	unsigned short GetLayerID( );
	void SetLayerID(unsigned short nLayerID);
	

	CTempLayerInfo();
	CTempLayerInfo(int nID);
	virtual ~CTempLayerInfo();

	//////////////////////////////////////////////////////////////////////////
		unsigned short  m_nLayerID;				//Õº≤„ID
		int m_nIconID;
};

#endif // !defined(AFX_TEMPLAYERINFO_H__1B19ACA3_7550_423F_B2C1_D05F5A96FAD8__INCLUDED_)
