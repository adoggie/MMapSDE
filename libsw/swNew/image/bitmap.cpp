#include <windows.h>
#include <stdio.h>

void SaveImage(const char * bmp_file,void *rgb_data,int rgb_len,BITMAPINFOHEADER * bi){
	BITMAPFILEHEADER hdr;
    FILE *fsave;
	memset(&hdr,0,sizeof(hdr));
    hdr.bfType = ((WORD) ('M' << 8) | 'B');
	hdr.bfSize = sizeof(hdr)+sizeof(BITMAPINFOHEADER)+rgb_len;
	hdr.bfOffBits = (DWORD) (sizeof(hdr) + sizeof(BITMAPINFOHEADER) );
    //--
	fsave = fopen(bmp_file,"wb");
    fwrite( &hdr,sizeof(hdr),1,fsave);
    fwrite(bi,sizeof(BITMAPINFOHEADER),1,fsave);
    fwrite(rgb_data,rgb_len,1,fsave);
    fflush(fsave);
    fclose(fsave);
}


void FillBitmapInfoHdr(BITMAPINFOHEADER * bi,int width,int height,int deep){
    memset(bi,0,sizeof(BITMAPINFOHEADER));
	bi->biSize		= sizeof(BITMAPINFOHEADER);
	bi->biWidth		= width;
	bi->biHeight 		= height;
	bi->biPlanes 		= 1;
	bi->biBitCount		= deep;			//bm.bmPlanes * bm.bmBitsPixel;
	bi->biCompression	= BI_RGB;
	bi->biSizeImage		= 0;
	bi->biXPelsPerMeter	= 0;
	bi->biYPelsPerMeter	= 0;
	bi->biClrUsed		= 0;
	bi->biClrImportant	= 0;
}

bool GetImageData2( HWND target,RECT rect,void * data,int * len){
 	HDC entireDC;
    RECT rc;
    HBITMAP bitmap;
    HDC CompDC;    
	entireDC = GetDC(target);
    //GetWindowRect(target,&rc);
    rc = rect;   
	
	if( rc.right-rc.left<=0 || rc.bottom-rc.top <=0){
		return false;
	}
	bitmap = CreateCompatibleBitmap( entireDC,rc.right-rc.left,rc.bottom-rc.top);
	if(bitmap ==NULL){
		ReleaseDC(target,entireDC);
		return false;
	}
    CompDC = CreateCompatibleDC(entireDC);
    SelectObject(CompDC,bitmap);    
    BitBlt(CompDC,0,0,rc.right-rc.left,rc.bottom-rc.top,entireDC,rect.left,rect.top,SRCCOPY);

	BITMAPINFO bmi;
  	BITMAPINFOHEADER   *bi;
  	int rgb_len ;
    int lines;
    bi = &bmi.bmiHeader;
  	char *rgb_buff ;
    FillBitmapInfoHdr(bi,rc.right-rc.left,rc.bottom-rc.top,24);

    rgb_len = bi->biWidth * bi->biHeight * 3;
	
    rgb_buff =(char*) data;// new char[rgb_len];
    if( rgb_buff == NULL ){
        return false;
    }
    lines = GetDIBits( CompDC,bitmap,0,rc.bottom-rc.top,rgb_buff,
    					(LPBITMAPINFO)bi,DIB_RGB_COLORS);
    DeleteObject(bitmap);
    ReleaseDC(target,CompDC);
    ReleaseDC(target,entireDC);
    DeleteDC(CompDC);
    if( lines == NULL){
      //  delete[] rgb_buff;
        return false;
    }
   // *data = rgb_buff;
    *len = rgb_len;
    return true;
}


bool GetImageData( HWND target,RECT rect,void ** data,int * len){
 	HDC entireDC;
    RECT rc;
    HBITMAP bitmap;
    HDC CompDC;    
	entireDC = GetDC(target);
    //GetWindowRect(target,&rc);
    rc = rect;    
	bitmap = CreateCompatibleBitmap( entireDC,rc.right-rc.left,rc.bottom-rc.top);
    CompDC = CreateCompatibleDC(entireDC);
    SelectObject(CompDC,bitmap);    
    BitBlt(CompDC,0,0,rc.right-rc.left,rc.bottom-rc.top,entireDC,0,0,SRCCOPY);

	BITMAPINFO bmi;
  	BITMAPINFOHEADER   *bi;
  	int rgb_len ;
    int lines;
    bi = &bmi.bmiHeader;
  	char *rgb_buff ;
    FillBitmapInfoHdr(bi,rc.right-rc.left,rc.bottom-rc.top,24);

    rgb_len = bi->biWidth * bi->biHeight * 3;
    rgb_buff = new char[rgb_len];
    if( rgb_buff == NULL ){
        return false;
    }
    lines = GetDIBits( CompDC,bitmap,0,rc.bottom-rc.top,rgb_buff,
    					(LPBITMAPINFO)bi,DIB_RGB_COLORS);
    DeleteObject(bitmap);
    ReleaseDC(target,CompDC);
    ReleaseDC(target,entireDC);
    DeleteDC(CompDC);
    if( lines == NULL){
        delete[] rgb_buff;
        return false;
    }
    *data = rgb_buff;
    *len = rgb_len;
    return true;
}


void DrawBitmap(HWND hwnd,int width,int height,char *data,int deep);
void CaptureImage(HWND target,const char * destfile){
    RECT rc;
    GetWindowRect(target,&rc);
    void *data;
    int len;
    GetImageData(target,rc,&data,&len);

    BITMAPINFO bmi;
  	BITMAPINFOHEADER   *bi;  	
    bi = &bmi.bmiHeader;
    FillBitmapInfoHdr(bi,rc.right-rc.left,rc.bottom-rc.top,24);

    SaveImage(destfile,data,len,bi);
    
    delete[] data;
}

void DrawBitmap2(HWND hwnd,RECT rect,char *data,int deep){

	BITMAPINFOHEADER   *bi;
    BITMAPINFO bitmap;
    bi = &bitmap.bmiHeader;
	memset(&bitmap,0,sizeof(bitmap));
	bi->biSize		= sizeof(BITMAPINFOHEADER);
	bi->biWidth		= rect.right - rect.left;
	bi->biHeight 		= rect.bottom - rect.top;
	bi->biPlanes 		= 1;
	bi->biBitCount		= deep;			//bm.bmPlanes * bm.bmBitsPixel;
	bi->biCompression	= BI_RGB;
	bi->biSizeImage		= bi->biWidth*bi->biHeight*deep/3;
	bi->biXPelsPerMeter	= 0;
	bi->biYPelsPerMeter	= 0;
	bi->biClrUsed		= 0;
	bi->biClrImportant	= 0;

	HDC hdc =   GetDC(hwnd);

	StretchDIBits( hdc,
		rect.left,
		rect.top,
		rect.right-rect.left,
		rect.bottom - rect.top,
		//rc.right-1,
		//rc.bottom-1,
		0,
		0,
		rect.right-rect.left,
		rect.bottom - rect.top,
		(CONST VOID *)data,
		&bitmap,
		DIB_RGB_COLORS,
		SRCCOPY );

	//ReleaseDC(hwnd,hdc);
	ReleaseDC(hwnd,hdc);
}

void StretchDrawBitmap(HWND hwnd,int width,int height,char *data,int deep){
	
	BITMAPINFOHEADER   *bi;
    BITMAPINFO bitmap;
    bi = &bitmap.bmiHeader;
	memset(&bitmap,0,sizeof(bitmap));
	bi->biSize		= sizeof(BITMAPINFOHEADER);
	bi->biWidth		= width;
	bi->biHeight 		= height;
	bi->biPlanes 		= 1;
	bi->biBitCount		= deep;			//bm.bmPlanes * bm.bmBitsPixel;
	bi->biCompression	= BI_RGB;
	bi->biSizeImage		= bi->biWidth*bi->biHeight*deep/3;
	bi->biXPelsPerMeter	= 0;
	bi->biYPelsPerMeter	= 0;
	bi->biClrUsed		= 0;
	bi->biClrImportant	= 0;
	RECT rc;
	HDC hdc =   GetDC(hwnd);
	::GetWindowRect(hwnd,&rc);
	StretchDIBits( hdc,
		0,
		0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		0,
		0,
		width,
		height,
		(CONST VOID *)data,
		&bitmap,
		DIB_RGB_COLORS,
		SRCCOPY );
	
	//ReleaseDC(hwnd,hdc);
	ReleaseDC(hwnd,hdc);
}

void DrawBitmap(HWND hwnd,int width,int height,char *data,int deep){

	BITMAPINFOHEADER   *bi;
    BITMAPINFO bitmap;
    bi = &bitmap.bmiHeader;
	memset(&bitmap,0,sizeof(bitmap));
	bi->biSize		= sizeof(BITMAPINFOHEADER);
	bi->biWidth		= width;
	bi->biHeight 		= height;
	bi->biPlanes 		= 1;
	bi->biBitCount		= deep;			//bm.bmPlanes * bm.bmBitsPixel;
	bi->biCompression	= BI_RGB;
	bi->biSizeImage		= width*height*deep;
	bi->biXPelsPerMeter	= 0;
	bi->biYPelsPerMeter	= 0;
	bi->biClrUsed		= 0;
	bi->biClrImportant	= 0;

	HDC hdc =   GetDC(hwnd);

	StretchDIBits( hdc,
		0,
		0,
		width-1,
		height-1,
		//rc.right-1,
		//rc.bottom-1,
		0,
		0,
		width,
		height,
		(CONST VOID *)data,
		&bitmap,
		DIB_RGB_COLORS,
		SRCCOPY );

	//ReleaseDC(hwnd,hdc);
	ReleaseDC(hwnd,hdc);
}