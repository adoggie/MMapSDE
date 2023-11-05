
#ifndef _GIS_MAPLAYER_H
#define _GIS_MAPLAYER_H

#include "swBase.h"
#include "IGIS_Layer.h"

class CGIS_MapLayer : public IGIS_Layer  {
public:
	CGIS_MapLayer( CGIS_LayerInfo *pInfo );
	virtual ~CGIS_MapLayer();
public:
	//virtual void DrawLayer( IGIS_LayerFile *pLFile, IGIS_MapView *pView, swRect rtPolt, ST_GEO_RECT rtDraw, double dbScale );
	//virtual void DrawLayer( IGIS_MapView *pView, swRect rtPolt, ST_GEO_RECT rtDraw, double dbScale );
	//BOOL EstimateLoadState( double dbScale );
	//BOOL IsLoad( );
	void InitLBCount( int nMaxNum );
	void IncreaseLCount( int nBID );
	void DecreaseLCount( int nBID );
	unsigned short GetBLCount( int nBID );
	void InitLBOList( int nBID );
	int GetBandMaxID( );
	void ClearLCount();
private:
	//BOOL EstimateDrawCondition( double dbScale, double dbMinGRScale, double dbMaxGRScale );
	unsigned char *m_pBLCount; //单个图层涉及的breadth的引用计数数组
	int m_nBandMaxID;
};

#endif 
