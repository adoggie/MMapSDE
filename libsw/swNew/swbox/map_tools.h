
#ifndef _MAP_TOOLS_H
#define _MAP_TOOLS_H

#include "map_map.h"
#include "map_symbol.h"

#include <wx/wx.h>
#include <vector>
#include <list>
#include <string>
#include <map>

#include <wx/wxsqlite3.h>
#include <boost/shared_ptr.hpp>
using namespace boost;

enum GeoMapToolTypeT{
	GMTT_NULL = -1,
	GMTT_PAN=0,
	GMTT_ZOOMIN,				//�Ŵ�
	GMTT_ZOOMOUT,				//��С
	GMTT_MEASURE_LINE,	//���
	GMTT_SELECT,				//��ѡ��������ѡ�񹤾�
	GMTT_EDITOR,				//��ͼ����༭����,poi�ر�֧���϶�,barrier֧���϶��͸ı�ڵ���״
	GMTT_NEW_POI,				//���POI
	GMTT_NEW_BARRIER,		//���Χ��
};

/*
measure_line: 
����: 
	DIST_POINTS	: stringline �ĳ���
	DIST_FREE;		���һ�����뵱ǰ����ľ���
*/
class GeoMapToolBase;
struct GeoMapTool_Event{
	GeoMapToolTypeT	src;	//�¼�Դ����
	std::map<wxString,wxString>		msg;	//�¼�������Ϣ����
	wxMouseEvent * evt;
	GeoMapToolBase* tool;
};

class GeoMapToolClient{
public:
	GeoMapToolClient(){}
	virtual void OnMapToolEvent( GeoMapTool_Event& evt){}
};

//// GeoMapToolBase //////////////////

class GeoMapCanvas ;
class GeoMapCanvas;
class GeoMapToolBase{
public:
	GeoMapToolBase();
	virtual void	set(GeoMapCanvas*) ;	
	virtual void  onMouseEvent(wxMouseEvent & evt);
	GeoMapToolTypeT	getType();
	void	addClient( GeoMapToolClient*);
	void  delClient(GeoMapToolClient*);
	virtual void onMapRedraw(const ViewRectT& rect=ViewRectT());//����ͼ�����ػ��ʱ��֪ͨtool,����tool����ʱ���ڵ�ͼˢ�µ���tool������Ϊʧ��
protected:
	void	init();
	virtual void	drawRect(const ViewRectT& rect,wxDC * dc,bool restore=false);			//���ƾ�������
	virtual void	restoreRect(const ViewRectT& rect);		//�ظ�֮ǰcanvas���򳡾�
	GeoMapCanvas*		_canvas;
	shared_ptr<wxCursor>			_cursor;
	wxString				_cursorName;
	shared_ptr<ViewPointT>	_startPt;
	GeoMapToolTypeT				_type;

	std::list<GeoMapToolClient*>			_clients;
	wxCriticalSection								_cs_clients;
};

inline GeoMapToolTypeT	GeoMapToolBase::getType(){
	return _type;
}

//ƽ��
class GeoMapTool_Pan:public GeoMapToolBase{
public:
	GeoMapTool_Pan();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
private:
	
};
//�Ŵ�
class GeoMapTool_ZoomIn:public GeoMapToolBase{
public:
	GeoMapTool_ZoomIn();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
};
//��С
class GeoMapTool_ZoomOut:public GeoMapToolBase{
public:
	GeoMapTool_ZoomOut();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
};

enum GeoMapToolSelectTypeT{
	GMTST_NULL = 0x00,
//	GMTST_MAP =0x01,
	GMTST_POI = 0x02,
	GMTST_VEHICLE =0x04,
	GMTST_BARRIER = 0x08
};

//ָ������ѡ���������
class GeoMapTool_Select:public GeoMapToolBase{
public:
	GeoMapTool_Select();
	void	set(GeoMapCanvas*);
	void	setSelectTypes(unsigned int );			//���ÿ�ѡ������,���������
	unsigned int getSelectTypes();
	void  onMouseEvent(wxMouseEvent & evt);
private:
	unsigned int	selectTypes;		//ѡ��������ͼ���
	GeoMapSymbolList			_cachedlist;
};

//���߾�,������ stringline
class GeoMapTool_MeasureLine:public GeoMapToolBase{
public:
	GeoMapTool_MeasureLine();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
private:
	void	clearScene();
private:
	std::list<shared_ptr<ViewPointT> >	_ptlist;		//�����߶���
	shared_ptr<ViewPointT>		_lastpt;
	
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////





#endif
