
/*
	convert rgb data to jpeg ,and invert
	created by zbin 2005.04.22
*/

#ifndef _RGB2JPEG_H
#define _RGB2JPEG_H

bool JpegCompress(int w,int h,const char * rgb_data,int rgb_size,
				  char * jpeg_data,int *jpeg_size);

bool JpegUnCompress(const char * jpeg_data,int jpeg_size,
					char *rgb_data,int rgb_size,int w,int h);				  				  

#endif
