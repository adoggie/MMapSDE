
#ifndef _MAP_RESOURCE_H
#define _MAP_RESOURCE_H

#include "map_symbol.h"

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include <wx/thread.h>
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
using namespace boost;

enum ResourceType{
	Res_ICON = 0x01,
};

struct SysResIcon{
	int		id;
	wxString	name;	
	wxSize		size;	// width and height
	shared_ptr<wxColour>	tcolor;	// transparent color 
	//wxImage		image;
	wxBitmap	bitmap;
	
	wxString	file;			//�ⲿ�ļ�����
	wxString	display;	//��ʾ���� 
};

//////////////////////////////////////////////////////////////////////////
//����ͼ����Ϣ
struct VehicleIconT{
	wxBitmap	normal;
	wxBitmap	alarm;
	wxBitmap  stop;
};

typedef std::map<int,VehicleIconT> VehicleIconListT;	//����������Ϣ

//////////////////////////////////////////////////////////////////////////



typedef std::map< int, shared_ptr<SysResIcon> > GeoMapIconListId;
typedef std::map<wxString,shared_ptr<SysResIcon> > GeoMapIconListName;

typedef int GVMS_ANGLE;	//�г��Ƕ�
typedef std::map<GVMS_ANGLE,shared_ptr<SysResIcon> > GeoMapIcon_VehicleArrowListT;
typedef std::map<wxString ,GeoMapIcon_VehicleArrowListT> GeoMapIcon_VehicleArrowMapNameListT;

class SysResMgr{
public:
	SysResMgr();
	bool init(shared_ptr<wxSQLite3Database> );
	void	reload(unsigned int  resMask = Res_ICON);	//ˢ����Դ����
	shared_ptr< SysResIcon> getIcon(int id);
	shared_ptr< SysResIcon> getIcon(const wxString& name);
	wxString getPath();
	shared_ptr<SysResIcon> getVehicleArrowIcon(GVMS_ANGLE angle,const wxString& kind=wxT("normal"));	//��������ͼ��
public:
	static SysResMgr	_handle;
	static SysResMgr&	instance();
private:
	void	loadIcons();
private:
	shared_ptr<wxSQLite3Database>	_db;
	GeoMapIconListId			_id_icons;
	GeoMapIconListName		_named_icons;
	wxCriticalSection			_icons_lock;
	GeoMapIcon_VehicleArrowMapNameListT	_vehicleArrows;
	
	VehicleIconListT			_vehIcons;	///<����ͼ����Ϣ
};


#endif

