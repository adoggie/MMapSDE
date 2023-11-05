// TempMapLayer.h: interface for the CTempMapLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPMAPLAYER_H__783CF6C3_BC6A_46CE_8F9E_3AB4CCE85807__INCLUDED_)
#define AFX_TEMPMAPLAYER_H__783CF6C3_BC6A_46CE_8F9E_3AB4CCE85807__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTempLayerInfo;
//////////////////////////////////////////////////////////////////////////

class CTempMapLayer  
{
public:
	CTempLayerInfo* GetLayerInfo( )
	{
		return m_pInfo;
	}
	CTempMapLayer(CTempLayerInfo* pInfo);
	virtual ~CTempMapLayer();
	void InitLBCount( int nMaxNum );
//////////////////////////////////////////////////////////////////////////
	unsigned char *m_pBLCount;

	CTempLayerInfo* m_pInfo;

};

#endif // !defined(AFX_TEMPMAPLAYER_H__783CF6C3_BC6A_46CE_8F9E_3AB4CCE85807__INCLUDED_)
