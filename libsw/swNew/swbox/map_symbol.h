
#ifndef _MAP_SYMBOL_H
#define _MAP_SYMBOL_H

//#include "map_resource.h"
#include "gvms_cmc.h"
#include "map_map.h"
#include <vector>
#include <map>
#include <list>
#include <wx/wx.h>

#include <geos.h>
using namespace geos;//::geom;
//using namespace geos::io;
#include "ice/sts.h"

#include <boost/shared_ptr.hpp>
using namespace boost;

enum GeoMapSymbolType{
	GEOMAP_SYMBOL_NULL =0x00,
	GEOMAP_SYMBOL_VEHICLE = 0x01,		//����
	GEOMAP_SYMBOL_USER_POI,					//�û���Ȥ��
	GEOMAP_SYMBOL_BARRIER,					//����Χ��
	GEOMAP_SYMBOL_ROUTE,						//���ٹ켣
	GEOMAP_SYMBOL_LAST = 0xffff			
};
class GeoMapLayerBase;
class GeoMapLayer_Vehicle;
class GeoMapLayer_UserPoi;
class GeoMapLayer_Barrier;

struct SysResIcon;
class GeoMapSymbolBase{
public:
	GeoMapSymbolBase();
	GeoMapSymbolType	getType();
	virtual void	setIcon(shared_ptr<SysResIcon>& icon);
	//virtual void	setText(wxString & text);
	virtual void	setLayer(GeoMapLayerBase* layer);
	virtual void	setGeoXY(double x,double y,bool & viewPtChanged);	//���þ�γ�� 
	virtual void  setViewXY(int x, int y);			//�ƶ�ͼ��
	virtual void	draw();						//�򵥵ĵ��ӻ���
	virtual void	drawCompound();		//����ͼ��������ƣ��ϳ����¼�ͼ�㲢����,֧������ͼ���symbol���ƶ�
	virtual geom::Envelope		getEnvelope();			//ͼ�ϱ߽����
	virtual geom::Coordinate	getViewXY();		//ͼ�����ĵ�
	virtual geom::Coordinate	getGeoXY();				//��γ������
	
	
	friend class GeoMapLayerBase;
	friend class GeoMapLayer_Vehicle;
	friend class GeoMapLayer_UserPoi;
	friend class GeoMapLayer_Barrier;
		
	virtual bool	isIntersected(const ViewRectT & rc);		//�Ƿ���ָ�������н���
	virtual bool	isGeoIntersected(const GeoRectT & rc);		//���������Ƿ��ཻ
	GeoPointT		getGeoPoint();
	virtual void	setFocus(bool focus = true);		//ͼ�걻ѡ��
	virtual bool focused();												//�Ƿ�ǰ�Ѿ��۽�
	
	virtual wxString getText();
	virtual int		getObjectId(){	return 0;}
	virtual void init();		//����symbol��ִ�г�ʼ��
	GeoMapLayerBase* getLayer();
	virtual bool	isIntersected(const ViewPointT& pt); //�����Ƿ��ڷ�����
	virtual void	setVisible(bool visible=true);
	bool	getVisible();
	void  setTimestamp(int t){ _timestamp = t;}
	int		getTimestamp(){ return _timestamp;}
	void	setFont(const wxFont& font);
	wxFont& getFont();
	void setTextColor(const wxColour& color);
	wxColour getTextColor();
protected:
	
	virtual void	drawIcon();
	virtual void	drawText();
	void	drawFocus();
	virtual ViewRectT getIconRect();
	ViewRectT	getTextRect();
protected:
	GeoMapSymbolType	_type;
	GeoMapLayerBase*	_layer;	//��Ⱦͼ��
	shared_ptr<SysResIcon>	_icon;
/*
	wxString					_name;		//����
	unsigned int			_sid;
	unsigned int			_clientid;	//�ͻ����
	
	bool							_visible;
	geom::Coordinate	_geo_xy;
	bool							_focused;		//�۽�
	ViewPointT				_viewxy;
	*/
	geom::Coordinate	_geo_xy;
	bool		_visible;
	int			_timestamp;
	wxFont	_font;
	wxColour _textColor; //�ı���ɫ
};

typedef std::list< shared_ptr<GeoMapSymbolBase> > GeoMapSymbolList;

class GeoMapSymbol_Vehicle:public GeoMapSymbolBase{
public:
	GeoMapSymbol_Vehicle();
	void	draw();						
	void	drawCompound();		
	void	drawText();
	geom::Envelope		getEnvelope();			//ͼ�ϱ߽����
	friend class GeoMapLayer_Vehicle;
	wxString getText();
	int		getObjectId();
	void init();	
	void	setDirection(float angle);		//�����г�����

	void	showTrackPath(bool show= true);	
	void  setTrackPathColor(wxColour& color);
	void	setTrackPathWidth(int width);		///<�켣�߿��
	void	drawTrackPath();
	void	resetTrackPoints();							///<����켣��
	void  addTrackPoint(const TrackGeoPointT& gpt);	///<��ӹ켣��
	void	addTrackPoints(const std::list<TrackGeoPointT>& gpts); ///<��Ӷ���켣�㼯
	void	setStatus(eVehicleStatusT vs);
	void	setCenter();	///<��ͼ����
private:
	bool	setIcon();
public:					
	gvms::clientVehicleT	_vechile;
	bool		_showTrackPath;					///<�Ƿ���ʾ�켣��
	wxColour	_trackPathColor;			///<�켣����ɫ
	int			_trackPathWidth;				///<�켣�߿��
	std::list<TrackGeoPointT>	_geoPoints;	///<���ٹ켣�㼯��
	wxCriticalSection		_points_mutex;
	float			_direction;	///<����
	eVehicleStatusT	_status;
};

class GeoMapSymbol_UserPoi:public GeoMapSymbolBase{
public:
	GeoMapSymbol_UserPoi();
	void	draw();						
	void	drawCompound();			
	geom::Envelope		getEnvelope();
	friend class GeoMapLayer_UserPoi;
	void	setGeoXY(double x,double y,bool & viewPtChanged);	//���þ�γ�� 
	void  setViewXY(int x, int y);
	geom::Coordinate	getGeoXY();				//��γ������
	int		getObjectId();
	void init();	
	wxString getText();
	void	drawIcon();		//ǿ�����¼���icon�������޸�icon֮���update
public:
	unsigned int			_mask_bit;
	shared_ptr<gvms::clientPoiDataT>	_userpoi;
};

class GeoMapSymbol_Barrier:public GeoMapSymbolBase{
public:
	GeoMapSymbol_Barrier();
	void	draw();						
	void	drawCompound();		
	bool	isIntersected(const ViewRectT & rc);		//�Ƿ���ָ�������н���
	bool	isIntersected(const ViewPointT& pt);		//�Ƿ�����ڶ������
	bool	isGeoIntersected(const GeoRectT & rc);		//���������Ƿ��ཻ
	geom::Envelope		getEnvelope();			//ͼ�ϱ߽����
	geom::Coordinate	getGeoXY();				//��γ������
	int		getObjectId();
	void	init();	
	void	setGeoXY(double x,double y,bool & viewPtChanged);	//���þ�γ�� 
	void  setViewXY(int x, int y);
	ViewRectT getIconRect();
	shared_ptr<geom::Polygon>	getViewPolygon();
public:
	shared_ptr<geom::Polygon>		_geo_polygon;		//����Χ��
	//shared_ptr<geom::Polygon>		_view_polygon;		//��ͼ����� 
	shared_ptr<gvms::clientBarrierT>	_barrier;
};

class GeoMapSymbol_Route:public GeoMapSymbolBase{
public:
	GeoMapSymbol_Route();
	void	draw();						
	void	drawCompound();		
	bool	isIntersected(const ViewRectT & rc);		//�Ƿ���ָ�������н���
	bool	isGeoIntersected(const GeoRectT & rc);	
	void init();	
	
};







#endif

