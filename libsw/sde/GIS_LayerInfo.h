
#ifndef _GIS_LAYERINFO_H
#define _GIS_LAYERINFO_H

#include "swBase.h"
#include "GeoGlobe.h"

class swEXPORT CGIS_LayerInfo {
public:
	CGIS_LayerInfo( );
	CGIS_LayerInfo( UINT nLID );
	virtual ~CGIS_LayerInfo();
public:
	swString			m_bstrLName;
	swString		GetLFName( );
	unsigned short GetLayerID( );
	void SetLayerID( unsigned short nLID );
	double GetMinGRScale( );
	void SetMinGRScale( double dbMinGR );
	double GetMaxGRScale( );
	void SetMaxGRScale( double dbMaxGR );
	double GetMinARScale( );
	void SetMinARScale( double dbMinAR );
	double GetMaxARScale( );
	void SetMaxARScale( double dbMaxAR );
	ST_GEO_PEN GetPen( );
	void SetPen( ST_GEO_PEN pen );
	ST_GEO_BRUSH GetBrush( );
	void SetBrush( ST_GEO_BRUSH brush );
	short GetLayerType( );
	void SetLayerType( short nLType );
	short GetIconID();
	void SetIconID( short sIconID );
	ST_GEO_RECT GetLayerRect( );
	void SetLayerRect( ST_GEO_RECT rtLayer );

	BOOL GetLayerState();
	void SetLayerState( BOOL bState );

private:
	short			m_nLType;				//ͼ������
	ST_GEO_PEN		m_pen;
	ST_GEO_BRUSH	m_brush;
	short				m_sIconID;				// ͼ��
	double			m_dbMinGR, m_dbMaxGR,	//ͼ����ʾ����
							m_dbMinAR, m_dbMaxAR;	//��ע��ʾ����
	swString			m_bstrLFName;
	ST_GEO_RECT		m_layerRect;			// ͼ�㷶Χ
	unsigned short  m_nLayerID;			//ͼ��ID
	BOOL				m_bState;						//�Ƿ���ʾ	
};

#endif 
