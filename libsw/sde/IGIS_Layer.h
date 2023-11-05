
#ifndef _IGIS_LAYER_H
#define _IGIS_LAYER_H

#include "swBase.h"

#include "IGIS_MapView.h"
#include "GIS_LayerInfo.h"
#include "GIS_Feature.h"
#include "GIS_FeaturePoint.h"
#include "GIS_FeatureLine.h"
#include "GIS_FeaturePoly.h"
#include "GIS_FeatureRut.h"

class IGIS_LayerFile;
class swEXPORT IGIS_Layer  
{
public:
	IGIS_Layer();
	virtual ~IGIS_Layer();
	typedef std::map<MapBreadthID_T ,std::list<CGIS_Feature*> > BreadthObjectListT;
public:
	virtual void DrawLayer( IGIS_LayerFile *pLFile,  IGIS_MapView *pView, swRect rtPolt, ST_GEO_RECT rtDraw, double dbScale ) = 0;
	virtual void DrawLayer( IGIS_MapView *pView, swRect rtPolt, ST_GEO_RECT rtDraw, double dbScale ) = 0;

	//获取图层类型
	EnLayType GetLayerType( );
	//获取和设置图层的信息
	CGIS_LayerInfo* GetLayerInfo( );
	//CListMap* GetObjectListMap( );
	BreadthObjectListT& GetObjectListMap( );
	void InitObjectListMap( int nBreadthID );
	
protected:
	/*
	void DrawPoint( IGIS_MapView *pView, CGIS_FeaturePoint *pPoint, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
	void DrawLine( IGIS_MapView *pView, CGIS_FeatureLine *pLine, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
	void DrawPoly( IGIS_MapView *pView, CGIS_FeaturePoly *pPoly, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
	void DrawRut( IGIS_MapView *pView, CGIS_FeatureRut *pRut, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
*/
	CGIS_LayerInfo *m_pInfo;	//图层信息
	EnLayType m_enLType;			//图层类型
	//CListMap *m_pObjListMap;	//存放图层对象的链表,一个层由多个图块组成，图块里面有n个对象构成链表，在这里就是定义了map<图块编号,图块对象*>结构
	//list 占用空间比vector要大，可以考虑直接使用vector
	BreadthObjectListT m_pObjListMap;
};

#endif 
