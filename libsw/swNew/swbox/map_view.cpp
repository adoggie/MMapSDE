
#include "map_view.h"
#include "sysconfig.h"
#include <wx/wx.h>
#include <wx/tokenzr.h>
#include "util.h"
//IMPLEMENT_DYNCREATE(GeoMapViewImpl, CGeoView)

BEGIN_MESSAGE_MAP(GeoMapViewImpl, CGeoView)
	ON_WM_PAINT( )
	ON_WM_SIZE( )
END_MESSAGE_MAP()

GeoMapViewImpl::GeoMapViewImpl(GeoMapLib*	maplib):GeoMapView(maplib){		
	m_OldScaleLevel = 0;
	m_nAngleOld = -1;
}

GeoMapViewImpl::~GeoMapViewImpl(){

}

void GeoMapViewImpl::draw( ){
	wxCriticalSectionLocker l(_drawMutex); //防止多个canvas并发问题

	RECT TempPlotRect = GetViewPlot();
	CBitmap* TemppBackBMP = GetBackBMP();
	CDC* TemppBackDC = GetBackDC();
	if( !TemppBackDC )	return;
	//CBitmap* pOldBMP = (CBitmap*)(TemppBackDC->SelectObject(TemppBackBMP));
	ST_GEO_PTXY xyCenter;
	ST_GEO_RECT rtOld = GetOldDrawRect();
	xyCenter.dx = (rtOld.dRight + rtOld.dLeft) / 2;
	xyCenter.dy = (rtOld.dBottom + rtOld.dTop) / 2;
	
	int Oldnx,Oldny;
	XyMapToView( xyCenter.dx,xyCenter.dy,Oldnx,Oldny );

	ST_GEO_PTXY xyCurrent;
	xyCurrent.dx = (NewDrawRect.dRight + NewDrawRect.dLeft) / 2;
	xyCurrent.dy = (NewDrawRect.dBottom + NewDrawRect.dTop) / 2;
	int Curnx, Curny;
	XyMapToView( xyCurrent.dx,xyCurrent.dy,Curnx,Curny );
	
 	SetbReDrawCar(FALSE);
	if( abs(Oldnx-Curnx)>=5 || abs(Oldny-Curny)>=5 || m_OldScaleLevel != GetCurrentScaleLevel() || m_nAngleOld != m_Angle)
	{
		DrawMap();
		m_OldScaleLevel = GetCurrentScaleLevel();
		m_nAngleOld = m_Angle;
	}
	else
	{
		//SetbReDrawCar(TRUE);
		DrawMap();
	}
	//dc.BitBlt(0,0, (TempPlotRect.right - TempPlotRect.left)-4, (TempPlotRect.bottom - TempPlotRect.top)-4, 
	//		  TemppBackDC, 0, 0, SRCCOPY);	
	//HDC dcCanvas = ::GetDC((HWND) NULL);
//	HDC dcCanvas = ::GetDC((HWND) _conf.hWnd );
//	::BitBlt( (HDC)dcCanvas,0,0,600,600,TemppBackDC->GetSafeHdc(),0,0,SRCCOPY);
}

bool GeoMapViewImpl::init(GeoMapViewConfigT& conf){
	GeoMapView::init(conf);
//	SubclassWindow((HWND)conf.hWnd);
	Attach((HWND)conf.hWnd );
	SetScale(&_conf.map_scale_list[0],_conf.map_scale_list.size());
	SetScale( _conf.centerXY.x, _conf.centerXY.y, _conf.scale );
	return true;
}

void	GeoMapViewImpl::getBitmap(GeoMapBitmapT& bitmap){
	//draw();
	bitmap.hdc = (void*)GetBackDC()->GetSafeHdc();
	return;
	CSize sz = GetBackBMP()->GetBitmapDimension();
	bitmap.width = sz.cx;
	bitmap.height = sz.cy;
	BITMAP hdr;
	DWORD bmpSize ;
	GetBackBMP()->GetBitmap(&hdr);
	bmpSize =  hdr.bmHeight * hdr.bmWidthBytes;
	bitmap.width = hdr.bmWidth;
	bitmap.height = hdr.bmHeight;
	bitmap.data.resize(bmpSize);
	GetBackBMP()->GetBitmapBits( bmpSize, &bitmap.data[0]);	
}

void	GeoMapViewImpl::zoomIn(int cx,int cy,int level ){
	ZoomOutAt( CPoint(cx,cy));
	_conf.scale = (int)GetCurrentScaleLevel();
}
void	GeoMapViewImpl::zoomInRect(ViewRectT rect){
}
void	GeoMapViewImpl::zoomOut(int cx,int cy,int level ){
	ZoomInAt( CPoint(cx,cy));
	_conf.scale = (int)GetCurrentScaleLevel();
}

void	GeoMapViewImpl::zoomOutRect(ViewRectT rect){
}

void	GeoMapViewImpl::xyV2M(int x,int y,GeoPointT& point){
	XyViewToMap(x,y,point.x,point.y);
}


//void	GeoMapViewImpl::xyM2V(const GeoPointT& point,int &x,int &y){
//}

void	GeoMapViewImpl::xyM2V(double x,double y,ViewPointT& xy){
	this->XyMapToView(x,y,xy.x,xy.y);
}

void	GeoMapViewImpl::changeSize(int cx,int cy){
	GeoMapView::changeSize(cx,cy);
	IGIS_MapManager* m_pMapManager = CSDEControler::GetSDEControl()->GetMapManager();    
    IGIS_Map* pMap;
    pMap = (IGIS_Map*)(m_pMapManager->GetFirstMap());
    while(pMap != NULL){
        pMap->Attach(this);
        pMap = (IGIS_Map*)(m_pMapManager->GetNext(pMap));
    }
	AdjustBuf();
	ST_GEO_PTXY ptxy;
	 ptxy.dx = _conf.centerXY.x;
	ptxy.dy = _conf.centerXY.y;
//	SetCurPt( ptxy);
//	SetScale(&_conf.map_scale_list[0],_conf.map_scale_list.size());
	//SetScale( _conf.centerXY.x, _conf.centerXY.y, _conf.scale );
	SetScale( _conf.centerXY.x, _conf.centerXY.y,_conf.scale );
	SetDrawStatic();
	SetMapParticular();
//	draw();
}

/*
	double dx = 121.395139;
	double dy = 31.160859;
	dx = 117.107574;
	dy = 39.117363;
	dx = 121.462959;
	dy = 31.241604;


	double arScale[15] = {
                0.200000f,
                0.600000f,
                1.000000f,
                2.000000f,
                4.000000f,
                8.000000f,
                15.000000f,
                30.000000f,
                60.000000f,
                120.000000f,
                300.000000f,
                600.000000f,
                1200.000000f,
                2000.000000f,
                5000.000000f,       
    };
*/

/*
bool GeoMapViewImpl::Init(ViewInitConfigT& conf){
	_conf  = conf;
	SubclassWindow(conf.hWnd);

	return true;
}
*/


void GeoMapViewImpl::OnSize( UINT, int, int ){
	changeSize(0,0);
}

void GeoMapViewImpl::OnPaint( ){
	draw();
}



GeoRectT GeoMapViewImpl::getGeoRect(){
	ST_GEO_RECT rc = GetCurrentDrawrect();
	GeoRectT r;
	r.x = rc.dLeft ;
	r.y = rc.dTop;
	r.w = rc.dRight - rc.dLeft;
	r.h = rc.dBottom - rc.dTop;
	return r;
}

void			GeoMapViewImpl::moveGeoCenterXY(GeoPointT xy){
	if( xy.x == 0 || xy.y == 0){
		return;
	}
	MoveToCenter(xy.x,xy.y);
	_conf.centerXY.x = xy.x;
	_conf.centerXY.y = xy.y;

}


void GeoMapViewImpl::SetDrawStatic()
{
	std::vector<COLORREF> colors;
	IGIS_Map *pMap = NULL;
	{
		colors = util_ParseRGBColors( gvmsSysConfig::instance().getGlobalSettingValue(wxT("cmc.map.backcolor"),wxT("240,240,208")) );		
		SetBackColor(colors[0]);		
		IGIS_MapManager* m_pMapManager = CSDEControler::GetSDEControl()->GetMapManager();  
		pMap= (IGIS_Map*)(m_pMapManager->GetFirstMap());
		while(pMap)
		{
			if(	pMap->GetMapInfo()->GetMapID() == MAPID_UM_NAVI )
			{
				pMap = (IGIS_Map*)m_pMapManager->GetNext( pMap );
				continue;
			}
			if( pMap->GetMapInfo()->GetMapID() == MAPID_MAP_NATION )
			{
				for( int j = 0 ; j < 2 ; j++ ) //设置0，1层的绘制颜色
				{
					CGIS_LayerInfo *pLayerInfo = pMap->GetLayerInfo(j);
					switch(j)
					{
					case 0:
						colors = util_ParseRGBColors( gvmsSysConfig::instance().getGlobalSettingValue(wxT("cmc.map.nation.layer0.color"),wxT("0,0,0/0,0,0")) );		
						if( colors.size() ==2)
							SetMapColor(pLayerInfo,colors[0],colors[1]);//行政区边界线颜色
						break;
					case 1:
						colors = util_ParseRGBColors( gvmsSysConfig::instance().getGlobalSettingValue(wxT("cmc.map.nation.layer1.color"),wxT("48,48,248/48,48,248")) );		
						if( colors.size() ==2)
							SetMapColor(pLayerInfo,colors[0],colors[1]);
							//SetMapColor(pLayerInfo,RGB(48,48,248),RGB(48,48,248));
						break;
					default:
						break;
					}
				}
				pMap = (IGIS_Map*)m_pMapManager->GetNext( pMap );
				continue;
			}
			// = MAPID_MAP_DETAIL
			
			for(int j=0; j<12; j++) //设置 0 ~11层的绘制颜色
			{
				CGIS_LayerInfo *pLayerInfo = pMap->GetLayerInfo(j);
				colors = util_ParseRGBColors( gvmsSysConfig::instance().getGlobalSettingValue(
								wxString::Format(wxT("cmc.map.detail.layer%d.color"),j)) );
				if( colors.size()==2){
					 SetMapColor(pLayerInfo,colors[0],colors[1]);
				}		
			}
			pMap = (IGIS_Map*)(m_pMapManager->GetNext(pMap));
		}
	}

	return ;
}


void GeoMapViewImpl::SetMapColor(CGIS_LayerInfo *layer, COLORREF brCol, COLORREF penCol)
{
	ST_GEO_BRUSH brush = layer->GetBrush();
	brush.nForeColor = brCol;
	ST_GEO_PEN   pen = layer->GetPen();
	if (penCol != -1)
	{
		pen.nColor = penCol;
	}
	layer->SetBrush(brush);
	layer->SetPen(pen);
}


void GeoMapViewImpl::SetMapParticular()
{
	int level[LAYER_PT_END+1];
	GetDefaultLevel(level);

	IGIS_MapManager* m_pMapManager = CSDEControler::GetSDEControl()->GetMapManager();  
	if (m_pMapManager == NULL)
	{
		return;
	}
	IGIS_Map *pMap = (IGIS_Map*)(m_pMapManager->GetFirstMap());
	while(pMap)
	{
		if(pMap->GetMapInfo()->GetMapID() == MAPID_MAP_NATION	||
			pMap->GetMapInfo()->GetMapID() == MAPID_UM_NAVI )
		{
			pMap = (IGIS_Map*)m_pMapManager->GetNext( pMap );
			continue;
		}
		for(int j = 0 ; j <= LAYER_PT_END ; j++)
		{
			CGIS_LayerInfo *pLayerInfo = pMap->GetLayerInfo(j);
			double scale = GetScale(level[j]);
			pLayerInfo->SetMaxARScale( 1000000 );
			pLayerInfo->SetMaxGRScale( 1000000 );
			if(fabs(scale-0)<ZERO)
			{
				pLayerInfo->SetMinGRScale(0);
				pLayerInfo->SetMinARScale(0);
			}
			else if( fabs(scale-(-1))<ZERO )
			{
				pLayerInfo->SetMinGRScale(1000000000);
				pLayerInfo->SetMinARScale(1000000000);
			}
			else
			{
				pLayerInfo->SetMinGRScale(1852*60 / scale -10);
				pLayerInfo->SetMinARScale(1852*60 / scale -10);
			}
		}
		pMap = (IGIS_Map*)(m_pMapManager->GetNext(pMap));
	}
}

void GeoMapViewImpl::GetDefaultLevel(int *nLevel)
{
 
	wxString layers = gvmsSysConfig::instance().getGlobalSettingValue(wxT("cmc.map.scalelayer.defs"));
	if(layers!=wxT("")){
		wxArrayString paris = wxStringTokenize(layers,wxT(","));
		if( paris.Count() == 26){
			for(int n=0;n<paris.Count();n++){
				wxArrayString pair =wxStringTokenize(paris[n],wxT(":"));
			 
				if( pair.Count()==2){
					nLevel[n] = atoi(pair[1].ToUTF8().data());	
				}
			}	
			return;			
		}		
	}
 
	if(nLevel)
	{
		nLevel[0] = 100;
		nLevel[1] = 2;
		nLevel[2] = 5;
		nLevel[3] = 100;
		 
// 		nLevel[4] = 3;
// 		nLevel[5] = 3;
// 		nLevel[6] = 5;
// 		nLevel[7] = 5;
// 		nLevel[8] = 6;
// 		nLevel[9] = 7;
// 		nLevel[10] = 8;
// 		nLevel[11] = 100;
		 
		nLevel[4] = 3;
		nLevel[5] = 8;
		nLevel[6] = 8;
		nLevel[7] = 8;
		nLevel[8] = 9;
		nLevel[9] = 10;
		nLevel[10] = 10;
		nLevel[11] = 100;

		nLevel[12] = 2;
		nLevel[13] = 2;
		nLevel[14] = 1;
		nLevel[15] = 2;
		nLevel[16] = 3;
		nLevel[17] = 3;
		nLevel[18] = 1;
		nLevel[19] = 1;
		nLevel[20] = 1;
		nLevel[21] = 2;
		nLevel[22] = 2;
		nLevel[23] = 2;
		nLevel[24] = 100;
		nLevel[25] = 1;
	}
}
/*
void GeoMapViewImpl::GetDefaultLevel(int *nLevel)
{
	if(nLevel)
	{
		nLevel[0] = 100;
		nLevel[1] = 2;
		nLevel[2] = 5;
		nLevel[3] = 100;

		// 		nLevel[4] = 3;
		// 		nLevel[5] = 3;
		// 		nLevel[6] = 5;
		// 		nLevel[7] = 5;
		// 		nLevel[8] = 6;
		// 		nLevel[9] = 7;
		// 		nLevel[10] = 8;
		// 		nLevel[11] = 100;

		nLevel[4] = 3;
		nLevel[5] = 3;
		nLevel[6] = 7;
		nLevel[7] = 8;
		nLevel[8] = 8;
		nLevel[9] = 8;
		nLevel[10] = 8;
		nLevel[11] = 100;

		nLevel[12] = 2;
		nLevel[13] = 2;
		nLevel[14] = 1;
		nLevel[15] = 2;
		nLevel[16] = 3;
		nLevel[17] = 3;
		nLevel[18] = 1;
		nLevel[19] = 1;
		nLevel[20] = 1;
		nLevel[21] = 2;
		nLevel[22] = 2;
		nLevel[23] = 2;
		nLevel[24] = 100;
		nLevel[25] = 1;
	}
}*/

void GeoMapViewImpl::setScale(double dx, double dy,int scalelevel){
	SetScale(dx,dy,(UINT)scalelevel);
	_conf.scale = scalelevel;
	_conf.centerXY.x = dx;
	_conf.centerXY.y = dy;
}

void GeoMapViewImpl::getOffset(float &x, float &y){
	x = nAdjustx;
	y =nAdjusty;
}


wxCriticalSection	GeoMapViewImpl::_drawMutex; 