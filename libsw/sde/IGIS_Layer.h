
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

	//��ȡͼ������
	EnLayType GetLayerType( );
	//��ȡ������ͼ�����Ϣ
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
	CGIS_LayerInfo *m_pInfo;	//ͼ����Ϣ
	EnLayType m_enLType;			//ͼ������
	//CListMap *m_pObjListMap;	//���ͼ����������,һ�����ɶ��ͼ����ɣ�ͼ��������n�����󹹳�������������Ƕ�����map<ͼ����,ͼ�����*>�ṹ
	//list ռ�ÿռ��vectorҪ�󣬿��Կ���ֱ��ʹ��vector
	BreadthObjectListT m_pObjListMap;
};

#endif 
