#ifndef _SW_BASE_H
#define _SW_BASE_H

#pragma warning(disable:4005 )
#include "config.h"

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/xml/xml.h>
#include <wx/filename.h>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
using namespace boost;


typedef wxString 	swString;
typedef wxBrush		swBrush;
typedef wxFont		swFont;
typedef wxPen			swPen;
typedef wxColour		swColor;
typedef wxCriticalSection swMutex;
typedef wxCriticalSectionLocker swScopeLock;
typedef wxDC		swDC;
typedef wxFile	swFile;
typedef wxXmlDocument swXmlDocument;
typedef wxXmlNode		swXmlNode;
typedef wxXmlProperty	swXmlProperty;
typedef wxPoint	swDrawPoint;
typedef wxSize	swDrawSize;
typedef wxMemoryDC	swMemoryDC;
typedef wxBitmap	swBitmap;
//#define swBitmap	wxBitmap
typedef wxImage		swImage;
typedef wxFileName	swFileName;
typedef wxDateTime	swDateTime;
typedef wxThread		swThread;
#define  swSleep(x)  ::Sleep(x)

typedef std::map<std::string,std::string> swStringProps;

#define swT wxT
#define swFromCurrent wxFromCurrent


typedef int 		swInt32;
typedef float 	swFloat;
typedef float 	swGeoReal;
typedef double 	swDouble;
typedef unsigned int swUInt32;
typedef char 		swChar;
typedef unsigned char 	swByte;
typedef char 		swInt8;
typedef unsigned char 	swUInt8;
typedef short		swInt16;
typedef unsigned short swUInt16;

//////////////////////////////////////////////////////////////////////////
template <typename T>
struct tagRectT{
	T x,y;
	T width,height;
	bool operator ==(const tagRectT<T>& other){
		if( x == other.x && y == other.y && width == other.width && height == other.height){
			return true;
		}
		return false;
	}
	//被other包含 
	bool isWithin(const tagRectT<T>& other){
		if(x>=other.x && x<=other.x+other.width && y>=other.y && y<=other.y+other.height){
			if(x+width <= other.x+other.width && y + height <= other.y + other.height){
				return true; //完全落在other内部
			}
		}
		return false;
	}	
	//包含other
	bool isContain(const tagRectT<T>& other){
		return other.isWithin(*this);
	}	
	//与other有相交部分
	bool isIntersect(const tagRectT<T>& other){
		if( x+width < other.x || y+height < other.y || x > other.x+other.width || y > other.y+other.height ){
			return false;
		}
		return true;
	}
};

typedef tagRectT<float> swGeoRect;
typedef tagRectT<int> 	swViewRect;

typedef swGeoRect swRect;
//////////////////////////////////////////////////////////////////////////
//Point
template <typename T>
struct tagPointT{
	T x,y;
	bool operator == (const tagPointT<T>& other){
		if( x == other.x && y == other.y){
			return true;
		}
		return false;
	}
	bool operator !=(const tagPointT<T>& other){
		return ! ((*this)==other);
	}
	tagPointT(const T& x_,const T& y_){
		x = x_; y = y_;
	}
	tagPointT(){}
	bool isWithin(const tagRectT<T>& rect); //是否落在矩形区域内
};
typedef tagPointT<float> swGeoPoint;
typedef tagPointT<int> 	swViewPoint;
typedef swGeoPoint swFloatPoint;
//////////////////////////////////////////////////////////////////////////
//Size
template <typename T>
struct tagSizeT{
	T width,height;
	tagSizeT(){}
	tagSizeT(const T& w,const T& h){
		width = w; height = h;
	}
	bool operator == (const tagSizeT<T>& other){
		if( width == other.width && height == other.height){
			return true;
		}
		return false;
	}
};
typedef tagSizeT<float> swGeoSize;
typedef tagSizeT<int> swViewSize;

//////////////////////////////////////////////////////////////////////////

typedef  std::map<std::string,std::string> swStringProps;
typedef swUInt8	swMapBreadthSizeId;
//////////////////////////////////////////////////////////////////////////
/*
struct swMapScaleLevel{
	swUInt8		level;				// 0-14 15级缩放级别
	swUInt32	value;				// 一个像素表示多少米 
	swMapBreadthSizeId		bsId;	//图块大小规格类型 swMapBreadthSizeInfo::id
};
typedef std::vector<swMapScaleLevel> swMapScaleLevelTableT; 

//#define MAX_SCALE_LEVEL 14

//地图网格大小规格
struct swMapBreadthSizeInfo{
	swMapBreadthSizeId			id;			//规格大小类型的定义
	swGeoSize	size;
};
*/
#define IN 
#define INOUT
#define OUT

typedef swUInt16 swMapPersistContainerIdentT;
typedef swUInt8	swMapLayerIdentT;
//网格对应多个物理存储对象，也就是在不同的存储空间中存在相同的网格
//例如全国地图被分割为多个几十个省市地图进行存储，省市交接处可以是相同的网格区域，但数据被分割存储

/*
网格编号采用32位的无符整数，但其真正只使用24位，
0xawbbbccc
bbb - 图块的x编号 4k个单位
ccc - 图块的y编号 4k个单位
a - 未使用，默认为0
w  - 取值范围0~f,表示网格块规格类型。 0表示默认的网格块大小
*/

typedef swUInt32 swMapBreadthIdentT;
#define swPersistCont_id(id) ( (id>>24)&0xff )
#define swBreadth_Axis_x(id) ( (id>>12)&0xfff)
#define swBreadth_Axis_y(id) ( id&0xfff )
#define swBreadth_SizeIdx(id) ( (id>>24)&0x0f ) //网格块规格索引编号

#define swMakeBreadthId(x,y) swMakeBreadthId2(x,y,0)
#define swMakeBreadthId2(x,y,sizeIdx) ( (sizeIdx&0x0f)<<24 | x<<12 | y )

#define swBreadth_NULL	(0x80000000) //最高位置1表示此网格块无效

//typedef std::map<swUInt32, std::vector<swMapPersistContainerIdentT> > swBreadth2FeatureContainerListT;  
//typedef std::vector< shared_ptr<swMapBreadth> > swMapBreadthListT;




///字节流对象 用于访问图层数据以便进一步创建feature对象
class swByteStream{
public:
	//度流到达尾部
	class ExceptionReadError:public std::exception{
		const char *  what() const{
			return "reached stream end";
		}
	};
	swByteStream(bool bigedian= true);
	swByteStream(swByte* start,swUInt32 size,bool bigedian= true);
	void operator =(const swByteStream& stream);
	swByte*		begin();
	swByte*		forward(swUInt32 n);	//前进n字节，返回当前位置,过界返回NULL 
	swByte*		back(swUInt32 n);			//回退n字节,过界则返回起始地址
	swByte*		rewind();							//重新绕回开始位置
	swUInt32	size();
	void			reset();
	void			setCurrent(swByte*);
	swByte*		current();
	bool			isEOF(); //是否到达尾部
	swUInt32	leftBytes(); //甚于字节数
	bool			readValue(swUInt32& val,bool forward=true); //读取值之后往前进
	bool			readValue(swUInt16& val,bool forward=true);
	bool			readValue(swUInt8& val,bool forward=true);
	bool			readValue(swFloat& val,bool forward=true);
	bool			readValue(swDouble& val,bool forward=true);
	bool			readValue(swGeoRect&val,bool forward=true);
	bool			readValue(swGeoPoint&val,bool forward=true);
	bool			readValue(swGeoSize&val,bool forward=true);
	bool			readString(swString& val,bool forward=true); //读1字节的短字符串(0~255),对应string_desc_t类型
	bool			readString(std::string& val,bool forward=true); //(用于存储utf-8)读1字节的短字符串(0~255),对应string_desc_t类型
private:
	swByte*		_ptr;
	swUInt32	_size;
	swByte*		_curr;
	bool			_be; //是否按网络字节读取
};

//////////////////////////////////////////////////////////////////////////
template < typename T=swUInt8>
class swCharArray{
	char * str;
	T     len;
};

typedef swCharArray<swUInt8>	swShortNameT;  //用于0~255字节的短字符串


///序列化时注意大数和小数转换
// void operator << (swInt32& ,swByteStream&);
// void operator << (swInt16& ,swByteStream&);
// void operator << (swUInt32& ,swByteStream&);
// void operator << (swUInt16& ,swByteStream&);
// void operator << (swFloat& ,swByteStream&);
// void operator << (swDouble& ,swByteStream&);
// void operator << (swByte&,swByteStream&);
// void operator << (swChar&,swByteStream&);
// 
// void operator << (swGeoRect& ,swByteStream&);
// void operator << (swGeoPoint& ,swByteStream&);
// void operator << (swGeoSize& ,swByteStream&);
// 
// void operator << (swShortNameT& ,swByteStream&); //
// void operator << (swString& ,swByteStream&); //
// 
// void operator = (swString& ,const swShortNameT&);


/*
bytes order swap
network order(bigendian) vs. host order 
*/

swUInt16 swN2H16(swUInt16 x);
swUInt32 swN2H32(swUInt32 x);
#define swH2N16(x) x
#define swH2N32(x) x

bool			swReadValue(swFile* file,swUInt32& val);
bool			swReadValue(swFile* file,swUInt16& val);
bool			swReadValue(swFile* file,swUInt8& val);
bool			swReadValue(swFile* file,swFloat& val);
bool			swReadValue(swFile* file,swDouble& val);
bool			swReadValue(swFile* file,swGeoRect&val);
bool			swReadValue(swFile* file,swGeoPoint&val);
bool			swReadValue(swFile* file,swGeoSize&val);
bool			swReadString(swFile* file,swString& val); //读0-255字节长度
bool			swReadString(swFile* file,std::string& val); //读0-255字节长度


#define swAssertReturn(b) if(!(b))	return false;
#define swAssertReturnNull(b) if(!(b))	return ;



// template <typename T>
// bool swGeom_isWithin(const tagRectT<T>& a,const tagRectT<T>& b); // is a within b,a与b正好相切，a 在b内部时返回true
// bool swGeom_isIntersect(const tagRectT<T>& a,const tagRectT<T>& b); // 两个矩形是否相交,也就是有相交的公共部分


#define SWMAP_SCALELEVEL_NUM	 16		 //地图缩放级别数

#define SWMAP_LAYER_DATA_DESC_LEN 13	//map_layer_data_desc_t 层数据描述大小
#define SWMAP_BREADTH_DATA_DESC_LEN 13	 //map_breadth_data_desc_t 固定头信息
#define SWMAP_BREADTHZONE_DESC_LEN 15		 //map_breadthzone_desc_t 网格区头信息

#define SWMAP_GET_LAYER_INDEX(layerid) (layerid)

#define SWMAP_BITMAP_TILE_WIDTH 150
#define SWMAP_BITMAP_TILE_HEIGHT 100


//////////////////////////////////////////////////////////////////////////
#define SWMAP_ERR_HEAPBLOCK_MALLOC_FAILED 101 //堆内存块分配失败
#define SWMAP_ERR_HEAPBLOCK_LOW						102		//可用堆内存块数量低 
//#define 
#endif