#include "map_canvas.h"
#include "map_scalebar.h"


/*
    400米
|___________|
*/
void MapCanvasPlugin_ScaleBar::OnDraw(GeoMapCanvas* canvas,wxDC* dc){
	wxPen pen;
	int x,y;
	x=20,y = canvas->GetSize().GetHeight()-35; 
	int w,h;
	w = 100;
	h=15;
	pen.SetColour(*wxWHITE);
	pen.SetWidth(2);
	dc->SetPen(pen);
	dc->DrawLine(wxPoint(x,y),wxPoint(x,y+h));
	dc->DrawLine(wxPoint(x,y+h),wxPoint(x+w,y+h));
	dc->DrawLine(wxPoint(x+w,y+h),wxPoint(x+w,y));
	pen.SetColour(*wxBLACK);
	dc->SetPen(pen);
	//////////////////////////////////////////////////////////////////////////
	wxString text;
	double len = canvas->getScale()* w;
	if( len <1000){
		text = wxString::Format(wxT("%d 米"),(int)len);
	}else{
		text= wxString::Format(wxT("%0.3f 公里"),len/1000.0);
	}
	long text_w,text_h;
	wxPoint text_pos;
	dc->GetTextExtent(text,&text_w,&text_h);
	text_pos.x = (w-text_w)/2 + x;
	text_pos.y = (y+h-text_h-5);
	dc->DrawText(text,text_pos);
	//////////////////////////////////////////////////////////////////////////

	x-=1;y--;
	dc->DrawLine(wxPoint(x,y),wxPoint(x,y+h));
	dc->DrawLine(wxPoint(x,y+h),wxPoint(x+w,y+h));
	dc->DrawLine(wxPoint(x+w,y+h),wxPoint(x+w,y));

	
}
