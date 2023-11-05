// swWin32.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <wx/wx.h>

#include "server.h"
#include "view_end.h"


void test_endian(){
	swFile file;
	swUInt32 u32 = 0x3f9e0419;
	float f1 ;
	memcpy(&f1,&u32,4);
	file.Open(swT("c:/t.dat"));
	swByte buff[120];
	file.Read(buff,4);
	swByteStream stream(buff,4);
	float vf;
	stream.readValue(vf);
}

void test_jpeg(){
	wxImage image;
	image.LoadFile(swT("c:/out.jpg"));
	wxFile file;
	file.Create("c:/dump.txt",true);
	file.Write( image.GetData(),image.GetWidth()*image.GetHeight()*3);	
	wxImage image2;
	image2.Create(image.GetWidth(),image.GetHeight());
	image2.SetData(image.GetData(),false);
	if(image2.IsOk()){
		wxBitmap bitmap(image2);
		bitmap.SaveFile("c:/dump.jpg",wxBITMAP_TYPE_JPEG)	;
	}
	
	file.Close();
}

#if SW_RPC_SUPPORT==0
int _tmain(int argc, _TCHAR* argv[]){
	wxInitAllImageHandlers();
	//test_endian();
//	test_jpeg();
//	
//	swStringProps props;
	swMapIdentT mapid;
	mapid =0;
	swMapView * view;
//	swMemHeapConfigT heapcfg;
	swStringProps serverprops;
	
	serverprops["confile"] = "../etc/mapserv.xml";
	if( !swMapServer::instance()->init(serverprops)){
		return false;
	}
	
	view = swMapServer::instance()->createView(mapid);
	
	swMapViewEnd vend;
	
	vend.create(view);
	vend.setScaleLevel(8);
	vend.setCenterPoint(swGeoPoint(121.41f,31.138f))	;
	vend.sizeChanged(swViewSize(800,600));	
	//vend.rotate(30);
//	swMemHeapEngine::instance()->takeMemProfile(memprof);
	vend.render();
	vend.getMemBitmap()->SaveFile(swT("c:/out.jpg"),wxBITMAP_TYPE_JPEG);
//	swMemHeapEngine::instance()->takeMemProfile(memprof);
	//vend.setCenterPoint(swGeoPoint(121.41+0.02,31.138f))	;
	//vend.setCenterPoint(swGeoPoint(121.5,31.06666))	;
	swMemHeapEngine::instance()->dumpMemUsingProfile();
	vend.rotate(60);
	vend.render();
	vend.getMemBitmap()->SaveFile(swT("c:/out2.jpg"),wxBITMAP_TYPE_JPEG);
	
	
//	swMemHeapEngine::instance()->takeMemProfile(memprof);
	return 0;
}
#endif
