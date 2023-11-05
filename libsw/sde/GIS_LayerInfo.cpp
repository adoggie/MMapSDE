
//#include "stdafx.h"
#include "GIS_LayerInfo.h"

CGIS_LayerInfo::CGIS_LayerInfo( ){
	m_bState = TRUE;
	m_nLayerID = 0;

	// 增加成员变量的初始化 20070329
	m_nLType = 0;
	m_pen.nColor = RGB(0, 0, 0);
	m_pen.nStyle = PS_SOLID;
	m_pen.nWidth = 1;
	m_brush.nBackColor = RGB(255, 255, 255);
	m_brush.nFillStyle = BS_SOLID;
	m_brush.nForeColor = RGB(0, 0, 0);
	m_brush.nStyle = PS_SOLID;
	m_brush.nWidth = 1;
	m_sIconID = 0;
	m_dbMinGR = 0;
	m_dbMaxGR = 0;
	m_dbMinAR = 0;
	m_dbMaxAR = 0;
	::memset(&m_layerRect, 0, sizeof(ST_GEO_RECT));
}

CGIS_LayerInfo::CGIS_LayerInfo(UINT nLID)
{
	m_bState = TRUE;
	m_nLayerID = nLID;

	// 增加成员变量的初始化 20070329
	m_nLType = 0;
	m_pen.nColor = RGB(0, 0, 0);
	m_pen.nStyle = PS_SOLID;
	m_pen.nWidth = 1;
	m_brush.nBackColor = RGB(255, 255, 255);
	m_brush.nFillStyle = BS_SOLID;
	m_brush.nForeColor = RGB(0, 0, 0);
	m_brush.nStyle = PS_SOLID;
	m_brush.nWidth = 1;
	m_sIconID = 0;
	m_dbMinGR = 0;
	m_dbMaxGR = 0;
	m_dbMinAR = 0;
	m_dbMaxAR = 0;
	::memset(&m_layerRect, 0, sizeof(ST_GEO_RECT));

}

CGIS_LayerInfo::~CGIS_LayerInfo()
{
	m_bState = FALSE;
	m_nLayerID = 0;
}
swString CGIS_LayerInfo::GetLFName( )
{
	return m_bstrLFName;
}
unsigned short CGIS_LayerInfo::GetLayerID( )
{
	return m_nLayerID;
}
void CGIS_LayerInfo::SetLayerID( unsigned short nLID )
{
	m_nLayerID = nLID;
}
short CGIS_LayerInfo::GetLayerType()
{
	return m_nLType;
}
void CGIS_LayerInfo::SetLayerType( short nLType )
{
	m_nLType = nLType;
}
short CGIS_LayerInfo::GetIconID()
{
	return m_sIconID;
}
void CGIS_LayerInfo::SetIconID( short sIconID )
{
	m_sIconID = sIconID;
}
ST_GEO_RECT CGIS_LayerInfo::GetLayerRect( )
{
	return m_layerRect;
}
void CGIS_LayerInfo::SetLayerRect( ST_GEO_RECT rtLayer )
{
	memcpy(&m_layerRect, &rtLayer, sizeof(ST_GEO_RECT));
}

double CGIS_LayerInfo::GetMinGRScale( )
{
	return m_dbMinGR;
}
void CGIS_LayerInfo::SetMinGRScale( double dbMinGR )
{
	m_dbMinGR = dbMinGR;
}
double CGIS_LayerInfo::GetMaxGRScale( )
{
	return m_dbMaxGR;
}
void CGIS_LayerInfo::SetMaxGRScale( double dbMaxGR )
{
	m_dbMaxGR = dbMaxGR;
}
double CGIS_LayerInfo::GetMinARScale( )
{
	return m_dbMinAR;
}
void CGIS_LayerInfo::SetMinARScale( double dbMinAR )
{
	m_dbMinAR = dbMinAR;
}
double CGIS_LayerInfo::GetMaxARScale( )
{
	return m_dbMaxAR;
}
void CGIS_LayerInfo::SetMaxARScale( double dbMaxAR )
{
	m_dbMaxAR = dbMaxAR;
}
ST_GEO_PEN CGIS_LayerInfo::GetPen( )
{
	return m_pen;
}
void CGIS_LayerInfo::SetPen( ST_GEO_PEN pen )
{
	// 如果结构里没有指针类型，可以直接进行赋值，简化操作
	m_pen = pen;
}
ST_GEO_BRUSH CGIS_LayerInfo::GetBrush( )
{
	return m_brush;
}
void CGIS_LayerInfo::SetBrush( ST_GEO_BRUSH brush )
{
	m_brush = brush;
}
BOOL CGIS_LayerInfo::GetLayerState()
{
	return m_bState;
}
void CGIS_LayerInfo::SetLayerState( BOOL bState )
{
	m_bState = bState;
}
