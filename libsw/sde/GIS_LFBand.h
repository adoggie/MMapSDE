
#ifndef _GIS_LFBAND_H
#define _GIS_LFBAND_H

#include "swBase.h"
#include "IGIS_LayerFile.h"
#include "GIS_MapLayer.h"
#include "GIS_GeoMap.h"
#include "GIS_BreadthMap.h"

class CGIS_LFBand : public IGIS_LayerFile  
{
public:
	CGIS_LFBand( );
	CGIS_LFBand( swString bstrFName );
	virtual ~CGIS_LFBand();
public:
	virtual BOOL OpenLayer();
    virtual BOOL LoadLayer();
	BOOL LoadLayerdel( CGIS_GeoMap *pMap, CGIS_BreadthMap *pBMap, CGIS_MapLayer **pLList, int nLNum );
	BOOL LoadLayerV2( CGIS_GeoMap *pMap, CGIS_BreadthMap *pBMap, CGIS_MapLayer **pLList, int nLNum );
	BOOL LoadLaneLayerV2(
        CGIS_GeoMap *pMap,
        CGIS_BreadthMap *pBMap,
        char*&pLaneBlockAddr );
	virtual BOOL SaveToFile( swString bstrFName );

	//BOOL InitLFBand( swString bstrFName, short nBreadthID, CPtrList *pLMList );
private:
	BOOL ReadBreadthLayer( CFile &file, int nOffset, int nBID, CGIS_MapLayer *pLayer );
	CPtrList* GetCurrentObjList( CPtrList *pLMList, UINT nLID, int nBreadthID, short &sIconID );

    BOOL ReadPointFeature( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, short sIconID, std::list<CGIS_Feature*>& pOList );
	BOOL ReadLineFeature( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, std::list<CGIS_Feature*>& pOList );
	BOOL ReadPolyFeature( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID,std::list<CGIS_Feature*>& pOList );
	BOOL ReadRutFeature( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, std::list<CGIS_Feature*>& pOList );

	int m_nBreadthID;
	CPtrList *m_pLMList;
	BOOL	m_bNation;

	BOOL ReadPointFeatureInSpecialSpace( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, short sIconID, CPtrList* pOList,      IMemoryMng* pMemmoryMngOfSelf, IMemoryMng* pMemoryMngOfBuddy);
	BOOL ReadLineFeatureInSpecialSpace( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, CPtrList* pOList ,                     IMemoryMng* pMemmoryMngOfSelf, IMemoryMng* pMemoryMngOfBuddy);
	BOOL ReadPolyFeatureInSpecialSpace( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, CPtrList* pOList ,                     IMemoryMng* pMemmoryMngOfSelf, IMemoryMng* pMemoryMngOfBuddy,BOOL bHaveID=FALSE);
	BOOL ReadRutFeatureInSpecialSpace( char *pFBuf, const DWORD dwSize, DWORD &dwStation, short nBreadthID, CPtrList* pOList ,                      IMemoryMng* pMemmoryMngOfSelf, IMemoryMng* pMemoryMngOfBuddy);

//////////////////////////////////////////////////////////////////////////
protected:
    static int m_nMemoryDest;
//////////////////////////////////////////////////////////////////////////

};

inline BOOL CGIS_LFBand::LoadLayer()
{
    ASSERT(FALSE);
	return FALSE;
}
#endif 
