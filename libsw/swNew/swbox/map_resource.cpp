
#include "map_resource.h"
#include "sysconfig.h"


SysResMgr::SysResMgr(){
}

struct _poi_sign_map_t{
	int poi;
	wxString name;
};

bool SysResMgr::init(shared_ptr<wxSQLite3Database> db){
	_db = db;
	//////////////////////////////////////////////////////////////////////////
	/* 初始化 poi名称
	*/
	_poi_sign_map_t pois[]={
		{1,wxT("旅店")},
		{2,wxT("旅游景点")},
		{-1,wxT("")},		
	};
	wxSQLite3Statement stm ;
	_poi_sign_map_t * ppoi = pois;
	try{
		while( ppoi->poi !=-1){
			stm = _db->PrepareStatement( wxString::Format(wxT("update icon_data set display=? where name='poi_sign_%d';"),ppoi->poi));
			stm.Bind(1,ppoi->name.ToUTF8().data());
			stm.ExecuteUpdate();
			stm.Reset();
			ppoi++;
		}
	}catch(...){
		wxLogMessage(wxT("update icon_data failed!"));
	}
	//////////////////////////////////////////////////////////////////////////
	reload();
	return true;
}

void	SysResMgr::reload(unsigned int resMask){
	try{
		if( resMask & Res_ICON ){
			loadIcons();
		}
	}catch(...){}
}

//
void SysResMgr::loadIcons(){      
	_icons_lock;
	//wxCriticalSectionLocker(_icons_lock);
	_id_icons.clear();
	_named_icons.clear();
	
	try{
		wxSQLite3ResultSet rs = _db->ExecuteQuery("select *  from icon_data;");	
		///////////////
		wxString s;
		while (rs.NextRow()){
			shared_ptr<SysResIcon> icon ( new SysResIcon);
			icon->id = rs.GetInt(wxT("SID"));
			icon->name = rs.GetString(wxT("NAME"));
			icon->file = rs.GetString(wxT("FILE"));
			icon->display = rs.GetString(wxT("DISPLAY"));
			
	//		wxLogMessage(wxString::Format(wxT("icon res: %d,%s,%s"),icon->id,icon->name,icon->display));
			long tc ;
			rs.GetString(wxT("TCOLOR")).ToLong(&tc,16);
			/*
			if( icon->name == wxT("vechile_hide")){
				tc = tc;
			}*/
			//掩码，透明色
			unsigned char r,g,b;
			r = (unsigned char)(tc >> 16 );
			g = (unsigned char)(tc >> 8 );
			b = (unsigned char)(tc);
			//icon->tcolor = shared_ptr<wxColour>(new wxColour(r,g,b) );
			s = getPath() + wxT("icons/")+icon->file;
			icon->bitmap  = wxBitmap(wxImage(s));
			if( !icon->bitmap.IsOk()){
				//wxMessageBox(wxT("icon资源加载失败: ")+s);
				wxLogMessage(wxT("icon资源加载失败: ")+s);
			}
			wxMask* mask;
			mask = new wxMask( icon->bitmap,wxColour(r,g,b) );
			icon->bitmap.SetMask(mask);
			icon->size = wxSize( icon->bitmap.GetWidth(),icon->bitmap.GetHeight());
			_id_icons[icon->id] =  icon ;
			_named_icons[icon->name] = icon;
		}		
		///初始化车辆图标
		 
		shared_ptr<SysResIcon> vehiceArrows = this->getIcon(wxT("icons_vehicle_arrow"));
		int w,h;
		h = vehiceArrows->bitmap.GetHeight();
		w = vehiceArrows->bitmap.GetWidth()/24 ; // per 15 degree		
		for(int n=0;n<24;n++){
			shared_ptr<SysResIcon> icon (new SysResIcon);
			icon->bitmap =	vehiceArrows->bitmap.GetSubBitmap(wxRect(n*w,0,w,h));
			icon->size = wxSize(w,h);
			_vehicleArrows[wxT("normal")][n*15] = icon;	//每15度一个icon
		}
		vehiceArrows = this->getIcon(wxT("icons_vehicle_arrow_blue"));
		h = vehiceArrows->bitmap.GetHeight();
		w = vehiceArrows->bitmap.GetWidth()/24 ; // per 15 degree		
		for(int n=0;n<24;n++){
			shared_ptr<SysResIcon> icon (new SysResIcon);
			icon->bitmap =	vehiceArrows->bitmap.GetSubBitmap(wxRect(n*w,0,w,h));
			icon->size = wxSize(w,h);
			_vehicleArrows[wxT("blue")][n*15] = icon;	//每15度一个icon
		}
		vehiceArrows = this->getIcon(wxT("icons_vehicle_arrow_green"));
		h = vehiceArrows->bitmap.GetHeight();
		w = vehiceArrows->bitmap.GetWidth()/24 ; // per 15 degree		
		for(int n=0;n<24;n++){
			shared_ptr<SysResIcon> icon (new SysResIcon);
			icon->bitmap =	vehiceArrows->bitmap.GetSubBitmap(wxRect(n*w,0,w,h));
			icon->size = wxSize(w,h);
			_vehicleArrows[wxT("green")][n*15] = icon;	//每15度一个icon
		}
		vehiceArrows = this->getIcon(wxT("icons_vehicle_arrow_red"));
		h = vehiceArrows->bitmap.GetHeight();
		w = vehiceArrows->bitmap.GetWidth()/24 ; // per 15 degree		
		for(int n=0;n<24;n++){
			shared_ptr<SysResIcon> icon (new SysResIcon);
			icon->bitmap =	vehiceArrows->bitmap.GetSubBitmap(wxRect(n*w,0,w,h));
			icon->size = wxSize(w,h);
			_vehicleArrows[wxT("red")][n*15] = icon;	//每15度一个icon
		}

		/* 
		///针对与不同的车辆状态有多种车辆颜色
		wxString sv = gvmsSysConfig::instance().getLocalGlobalSettingValue(wxT("vehicleType.count"),wxT("1"))
		int vehTypeCount = util_str2Int(sv,1);
		for(int n=1;n<=vehTypeCount;n++){
			VehicleIconT vicon;
			wxSQLite3ResultSet rs = _db->ExecuteQuery("select *  from icon_data;");	
			while (rs.NextRow()){
				//shared_ptr<SysResIcon> icon ( new SysResIcon);
				//icon->id = rs.GetInt(wxT("SID"));
				//icon->name = rs.GetString(wxT("NAME"));

				icon->file = rs.GetString(wxT("FILE"));
				icon->display = rs.GetString(wxT("DISPLAY"));
				if( rs.GetString(wxT("NAME") == wxString::Format(wxT("vehicleType.%d.icon.normal"),n)){
					
				}
			}
		}
		*/
		


	}catch(...){
		wxLogMessage(wxT("resource::load_icons() failed!"));
	}
}

shared_ptr<SysResIcon> SysResMgr::getVehicleArrowIcon(GVMS_ANGLE angle,const wxString& kind /*="normal"*/){
	shared_ptr<SysResIcon> icon;
	icon = _vehicleArrows[kind][ (angle/15)*15];
	return icon;
}

shared_ptr< SysResIcon> SysResMgr::getIcon(int id){
	if(_id_icons.find(id) == _id_icons.end()){
		wxMessageBox(wxString::Format(wxT("未能匹配poi:%d的资源"),id));
	}
	return _id_icons[id];
}

shared_ptr< SysResIcon> SysResMgr::getIcon(const wxString& name){
	//shared_ptr< SysResIcon> icon( new SysResIcon); 
	//*icon.get() =  *_named_icons[name].get();
	//return icon;
	if( _named_icons.find(name) == _named_icons.end()){
		wxMessageBox(wxT("资源加载失败:")+name);
	}
	return _named_icons[name];
	//return shared_ptr<SysResIcon>();
}

wxString SysResMgr::getPath(){
	return gvmsSysConfig::instance().getPath()+ wxT("/resource/");
	/*
	wxSQLite3ResultSet rs = _db->ExecuteQuery("select value from global_settings where key = 'resource.path';");	
	///////////////
	wxString path;
	if( rs.NextRow()){
		path = rs.GetString(wxT("VALUE"))+wxT("/");
	}
	return path;
	*/
}

SysResMgr	SysResMgr::_handle;
SysResMgr&	SysResMgr::instance(){
	return _handle;
}