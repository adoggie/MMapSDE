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
	//��other���� 
	bool isWithin(const tagRectT<T>& other){
		if(x>=other.x && x<=other.x+other.width && y>=other.y && y<=other.y+other.height){
			if(x+width <= other.x+other.width && y + height <= other.y + other.height){
				return true; //��ȫ����other�ڲ�
			}
		}
		return false;
	}	
	//����other
	bool isContain(const tagRectT<T>& other){
		return other.isWithin(*this);
	}	
	//��other���ཻ����
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
	bool isWithin(const tagRectT<T>& rect); //�Ƿ����ھ���������
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
	swUInt8		level;				// 0-14 15�����ż���
	swUInt32	value;				// һ�����ر�ʾ������ 
	swMapBreadthSizeId		bsId;	//ͼ���С������� swMapBreadthSizeInfo::id
};
typedef std::vector<swMapScaleLevel> swMapScaleLevelTableT; 

//#define MAX_SCALE_LEVEL 14

//��ͼ�����С���
struct swMapBreadthSizeInfo{
	swMapBreadthSizeId			id;			//����С���͵Ķ���
	swGeoSize	size;
};
*/
#define IN 
#define INOUT
#define OUT

typedef swUInt16 swMapPersistContainerIdentT;
typedef swUInt8	swMapLayerIdentT;
//�����Ӧ�������洢����Ҳ�����ڲ�ͬ�Ĵ洢�ռ��д�����ͬ������
//����ȫ����ͼ���ָ�Ϊ�����ʮ��ʡ�е�ͼ���д洢��ʡ�н��Ӵ���������ͬ���������򣬵����ݱ��ָ�洢

/*
�����Ų���32λ���޷���������������ֻʹ��24λ��
0xawbbbccc
bbb - ͼ���x��� 4k����λ
ccc - ͼ���y��� 4k����λ
a - δʹ�ã�Ĭ��Ϊ0
w  - ȡֵ��Χ0~f,��ʾ����������͡� 0��ʾĬ�ϵ�������С
*/

typedef swUInt32 swMapBreadthIdentT;
#define swPersistCont_id(id) ( (id>>24)&0xff )
#define swBreadth_Axis_x(id) ( (id>>12)&0xfff)
#define swBreadth_Axis_y(id) ( id&0xfff )
#define swBreadth_SizeIdx(id) ( (id>>24)&0x0f ) //��������������

#define swMakeBreadthId(x,y) swMakeBreadthId2(x,y,0)
#define swMakeBreadthId2(x,y,sizeIdx) ( (sizeIdx&0x0f)<<24 | x<<12 | y )

#define swBreadth_NULL	(0x80000000) //���λ��1��ʾ���������Ч

//typedef std::map<swUInt32, std::vector<swMapPersistContainerIdentT> > swBreadth2FeatureContainerListT;  
//typedef std::vector< shared_ptr<swMapBreadth> > swMapBreadthListT;




///�ֽ������� ���ڷ���ͼ�������Ա��һ������feature����
class swByteStream{
public:
	//��������β��
	class ExceptionReadError:public std::exception{
		const char *  what() const{
			return "reached stream end";
		}
	};
	swByteStream(bool bigedian= true);
	swByteStream(swByte* start,swUInt32 size,bool bigedian= true);
	void operator =(const swByteStream& stream);
	swByte*		begin();
	swByte*		forward(swUInt32 n);	//ǰ��n�ֽڣ����ص�ǰλ��,���緵��NULL 
	swByte*		back(swUInt32 n);			//����n�ֽ�,�����򷵻���ʼ��ַ
	swByte*		rewind();							//�����ƻؿ�ʼλ��
	swUInt32	size();
	void			reset();
	void			setCurrent(swByte*);
	swByte*		current();
	bool			isEOF(); //�Ƿ񵽴�β��
	swUInt32	leftBytes(); //�����ֽ���
	bool			readValue(swUInt32& val,bool forward=true); //��ȡֵ֮����ǰ��
	bool			readValue(swUInt16& val,bool forward=true);
	bool			readValue(swUInt8& val,bool forward=true);
	bool			readValue(swFloat& val,bool forward=true);
	bool			readValue(swDouble& val,bool forward=true);
	bool			readValue(swGeoRect&val,bool forward=true);
	bool			readValue(swGeoPoint&val,bool forward=true);
	bool			readValue(swGeoSize&val,bool forward=true);
	bool			readString(swString& val,bool forward=true); //��1�ֽڵĶ��ַ���(0~255),��Ӧstring_desc_t����
	bool			readString(std::string& val,bool forward=true); //(���ڴ洢utf-8)��1�ֽڵĶ��ַ���(0~255),��Ӧstring_desc_t����
private:
	swByte*		_ptr;
	swUInt32	_size;
	swByte*		_curr;
	bool			_be; //�Ƿ������ֽڶ�ȡ
};

//////////////////////////////////////////////////////////////////////////
template < typename T=swUInt8>
class swCharArray{
	char * str;
	T     len;
};

typedef swCharArray<swUInt8>	swShortNameT;  //����0~255�ֽڵĶ��ַ���


///���л�ʱע�������С��ת��
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
bool			swReadString(swFile* file,swString& val); //��0-255�ֽڳ���
bool			swReadString(swFile* file,std::string& val); //��0-255�ֽڳ���


#define swAssertReturn(b) if(!(b))	return false;
#define swAssertReturnNull(b) if(!(b))	return ;



// template <typename T>
// bool swGeom_isWithin(const tagRectT<T>& a,const tagRectT<T>& b); // is a within b,a��b�������У�a ��b�ڲ�ʱ����true
// bool swGeom_isIntersect(const tagRectT<T>& a,const tagRectT<T>& b); // ���������Ƿ��ཻ,Ҳ�������ཻ�Ĺ�������


#define SWMAP_SCALELEVEL_NUM	 16		 //��ͼ���ż�����

#define SWMAP_LAYER_DATA_DESC_LEN 13	//map_layer_data_desc_t ������������С
#define SWMAP_BREADTH_DATA_DESC_LEN 13	 //map_breadth_data_desc_t �̶�ͷ��Ϣ
#define SWMAP_BREADTHZONE_DESC_LEN 15		 //map_breadthzone_desc_t ������ͷ��Ϣ

#define SWMAP_GET_LAYER_INDEX(layerid) (layerid)

#define SWMAP_BITMAP_TILE_WIDTH 150
#define SWMAP_BITMAP_TILE_HEIGHT 100


//////////////////////////////////////////////////////////////////////////
#define SWMAP_ERR_HEAPBLOCK_MALLOC_FAILED 101 //���ڴ�����ʧ��
#define SWMAP_ERR_HEAPBLOCK_LOW						102		//���ö��ڴ�������� 
//#define 
#endif