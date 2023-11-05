
#ifndef _BITMAP_H
#define _BITMAP_H

#include <windows.h>

void SaveImage(const char * bmp_file,void *rgb_data,int rgb_len,BITMAPINFOHEADER * bi);
void FillBitmapInfoHdr(BITMAPINFOHEADER * bi,int width,int height,int deep);
bool GetImageData( HWND target,RECT rect,void ** data,int * len);
bool GetImageData2( HWND target,RECT rect,void * data,int * len);
void CaptureImage(HWND target,const char * destfile);
void DrawBitmap(HWND hwnd,int width,int height,char *data,int deep);			
void DrawBitmap2(HWND hwnd,RECT rect,char *data,int deep);
void StretchDrawBitmap(HWND hwnd,int width,int height,char *data,int deep);
#endif
