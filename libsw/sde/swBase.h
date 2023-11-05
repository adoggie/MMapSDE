
#ifndef _SW_BASE_H
#define _SW_BASE_H
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT 
/*
#include <afxwin.h>
#include <afx.h>
#include <afxcmn.h>
*/
#include "swConfig.h"

#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <boost/shared_ptr.hpp>
using namespace boost;

#include <wx/wx.h>

 

typedef int swint_t;
typedef short swint16_t;
typedef unsigned  int swuint_t;
typedef unsigned short swuint16_t;
typedef char swchar_t;
typedef unsigned char swbyte_t;

//#define swString swString 
#define swString wxString 
#define swFile	wxFile
#define swRect wxRect
#define UUID GUID 
//typedef unsigned char BYTE;

template <typename T>
struct tagRectT{
	T x,y,width,height;
	tagRectT(){
		x = y = width = height = 0;
	}
};
typedef tagRectT<double> swGeoRectT;

template <typename T>
struct tagRect2T{
	T dLeft, dTop, dRight, dBottom;
	tagRect2T(){
		dLeft = dTop = dRight = dBottom = 0;
	}
};

template <typename T>
struct tagPointT{
#define dx x
#define dy y
	T x, y;
	tagPointT(){
		x=y= 0;
	}
};
typedef tagPointT<float> swGeoPointT;

#define swMutex wxMutex
#define swMutexLocker wxMutexLocker
struct swBaseObject{
	void lock();
	void unlock();
	swMutex& getMutex(){
		return _mutex;
	}
	swMutex	_mutex;
};

#define POSITION void*
#endif

