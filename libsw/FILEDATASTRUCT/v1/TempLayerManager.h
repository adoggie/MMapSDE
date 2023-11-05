// TempLayerManager.h: interface for the CTempLayerManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPLAYERMANAGER_H__7F23F3F9_6A1B_4626_AD5C_3D555443F328__INCLUDED_)
#define AFX_TEMPLAYERMANAGER_H__7F23F3F9_6A1B_4626_AD5C_3D555443F328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
class CTempMapLayer;
typedef CMap< short, short, CTempMapLayer*, CTempMapLayer* > CTempLayerMap;
//////////////////////////////////////////////////////////////////////////

class CTempLayerManager  
{
public:
	CTempLayerManager();
	virtual ~CTempLayerManager();
	CTempLayerMap* GetLayerListMap();

	CTempLayerMap *m_pLayListMap;
};

#endif // !defined(AFX_TEMPLAYERMANAGER_H__7F23F3F9_6A1B_4626_AD5C_3D555443F328__INCLUDED_)
