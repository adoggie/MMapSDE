
#ifndef _MAP_LAYER_H
#define _MAP_LAYER_H

#include "map_symbol.h"
#include "map_map.h"

#include <wx/wx.h>

#include <vector>
#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
using namespace boost;

enum GeoMapLayerType{
	GEOMAP_LAYER_NULL =0x00,
	GEOMAP_LAYER_USER_POI =0x01,
	GEOMAP_LAYER_BARRIER = 0x02,
	//GEOMAP_LAYER_ROUTE = 0x04,
	GEOMAP_LAYER_VEHICLE = 0x04,
	//GEOMAP_LAYER_TRACK = 0x10,		//gps����ʵʱ����ͼ�㣬ʵ�ֳ����͹켣����
	GEOMAP_LAYER_LOWEST = GEOMAP_LAYER_USER_POI,
	GEOMAP_LAYER_TOPMOST = GEOMAP_LAYER_VEHICLE,
	GEOMAP_LAYER_LAST = 0xff
};

struct GeoMapLayerFontT{
	wxString	font;
	int				size;
};

enum GeoMapLayerTextAlignT{
	GEOMAP_TEXTALIGN_LEFT = 0x1,
	GEOMAP_TEXTALIGN_BOTTOM ,
	GEOMAP_TEXTALIGN_RIGHT ,
	GEOMAP_TEXTALIGN_TOP 
};

class GeoMapCanvas;
class GeoMapLayerBase{
public: 
	GeoMapLayerBase();
	//void   chainLayer(GeoMapLayerType top ,GeoMapLayerType bottom );
	GeoMapLayerBase* getTopLayer();
	GeoMapLayerBase* getBottomLayer();
	void		setCanvas( GeoMapCanvas*  canvas);
	GeoMapCanvas*		getCanvas();
	inline GeoMapLayerType		getType();
	virtual void draw();									//���»���
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
	virtual bool	textVisible();	//�����ı��Ƿ���ʾ
	void	setFont(GeoMapLayerFontT& font);
	GeoMapLayerFontT& getFont();
	GeoMapSymbolList	getSymbols();
	GeoMapLayerTextAlignT	getTextAlign();
	void setTextAlign(GeoMapLayerTextAlignT align);
	virtual GeoMapSymbolList findSymbols(const ViewRectT& rc,GeoMapSymbolBase* ignore=NULL);//ѡȡ���ο��ڶ���
	virtual void lockSymbols(bool lock=true);
	virtual void	changeSize(int cx,int cy);
	wxDC*		getDrawDC();
	wxBitmap*	getDrawBitmap();
	virtual void  invalidViewRect(const ViewRectT& rect,GeoMapSymbolBase* ignoreDraw=NULL/*���Ի���*/);
	virtual void setSymbolGeoXY(const GeoPointT& xy,int sid,bool& viewXyChanged/*�Ƿ�ı�����ͼλ��*/,int time=0);	//���ö����������,���������꼴������ͼ�����ˢ��
	virtual void clearSymbols();		//���ͼ������
	virtual void redraw();
	virtual shared_ptr<GeoMapSymbolBase>	getSymbol(const ViewPointT& pt); //��ȡָ��xy�ϵķ��Ŷ���
	void		setSymbolVisible(int sid,bool visible = true);	//������ʾ������ͼ��ͼ��
	virtual void append(shared_ptr<GeoMapSymbolBase>& symbol);	//��Ӷ���ָ����ͼ��
	virtual shared_ptr<GeoMapSymbolBase>	getSymbol(int sid); //��ȡָ�����Ŷ���
	virtual shared_ptr<GeoMapSymbolBase> detachSymbol(int sid);	//ɾ������
	virtual void attachSymbol(const shared_ptr<GeoMapSymbolBase> symbol); //��ӷ���
	
protected:
	//virtual GeoMapSymbolList getSymbolsInViewRect
	virtual void	before_drawSelf();
	virtual void  after_drawSelf();
protected:
	GeoMapSymbolList		_symbols;
	wxCriticalSection		_cs_symbols;

	GeoMapCanvas*				_canvas;
	bool						_visible;
	bool						_textVisible;
	GeoMapLayerType	_type;
	//GeoMapLayerType	_topLayer;
	//GeoMapLayerType	_bottomLayer;
	GeoMapLayerFontT	_font;
	GeoMapLayerTextAlignT	_textAlign;
	wxMemoryDC			_drawDC;
	shared_ptr<wxBitmap>	_drawBitmap;
};

class GeoMapLayer_Vehicle:public GeoMapLayerBase{
public:
	GeoMapLayer_Vehicle();
	void draw();
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
	GeoMapSymbolList findSymbols(const ViewRectT& rc); //����鳵
	void appendSymbols( std::list< shared_ptr<gvms::clientVehicleT> > & vehicles); //����µĳ�������ͼ
	void	setDirection(float angle,int vid);		//�����г�����
	bool	textVisible();

	void	showTrackPath(int vid,bool show);
	void	hideTrackPath();	///ȫ������
	void	setTrackPathColor(int vid,wxColour& color);
	void	setTrackPathWidth(int vid,int width);
	void	before_drawSelf();

	void	setReplayMode(bool mode);
	bool  isReplayMode();
	void  setFocus(int vid) ; //���ó���Ϊ����
private:
	bool		_replaying; //�Ƿ���ʷ�ط�״̬ 
	wxFont	_fntFouse;	//���㳵���ı���ʾ����
	int			_focuseVehicle;	//���㳵��
};

class GeoMapLayer_UserPoi:public GeoMapLayerBase{
public: 
	GeoMapLayer_UserPoi();
	void draw();
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
	GeoMapSymbolList findSymbols(const ViewRectT& rc); 
	bool	textVisible();
private:
};

class GeoMapLayer_Barrier:public GeoMapLayerBase{
public:
	GeoMapLayer_Barrier();
	void draw();
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
	GeoMapSymbolList findSymbols(const ViewRectT& rc); 
	void	deleteBarriersByGroup(int bgId);		//ɾ��ָ��Χ�����Χ��
	void	before_drawSelf();
	void  after_drawSelf();
private:
	wxMemoryDC			_tempDC;
	wxBitmap				_tempBitmap;	
};

class GeoMapLayer_Route:public GeoMapLayerBase{
public:
	GeoMapLayer_Route();
	void draw();
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
private:
};

//////////////////////////////////////////////////////////////////////////
//��������ͼ��Ҳ��������ʷ��¼�ط�
//������������ʱ�����ڳ����㽫���ٳ�������
//�켣����������ʾ����
struct GeoMapTrackPathAttrT{
	wxPen	pen;
	GeoMapTrackPathAttrT(){
		pen.SetColour(wxT("RED"));
		pen.SetWidth(3);
	}
};

struct GeoMapTrackPointT{
	GeoPointT geopt;	//λ��
	int				time;		//ʱ�� 1970 
};


typedef std::map< GeoMapLayerType, shared_ptr<GeoMapLayerBase> > GeoMapLayerList;



#endif
