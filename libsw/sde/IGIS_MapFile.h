#ifndef _IGIS_MAPFILE_H
#define _IGIS_MAPFILE_H

#include "swBase.h"

#include "IGIS_Map.h"
#include "IGIS_GeoFile.h"
//地图文件头部数据信息结构体
struct ST_MAP_HRADER{
	char		chHeader[4];	// 头部标示
	int			nLayerNum;		// 层的数目
	ST_GEO_RECT	rectMap;		// 地图区域范围
};

struct ST_LAYER_INFO{
	char		chLayerType;	// 图层类型：1-point，2-line，3-poly，4-rut，else-unknown
	char		chLayerName[31];
	int			nPenColor;
	int			nBrushColor;
	BYTE		byPenWidth;
	BYTE		byPenStyle;
	BYTE		byBrushStyle;
	BYTE		bySymbolType;
	float		fMinGR;			// hansom 不明白此定义的实际意义
	float		fMaxGR;
	float		fMinAR;
	float		fMaxAR;
	ST_GEO_RECT	rectLayer;
};

class IGIS_MapFile : public IGIS_GeoFile {
public:
	IGIS_MapFile();
	virtual ~IGIS_MapFile();
public:
	void SetMap( IGIS_Map *pMap );
	virtual BOOL OpenMap( ) = 0;
	virtual BOOL LoadMap( ) = 0;
	virtual BOOL SaveToFile( swString bstrFName ) = 0;
	
protected:
	IGIS_Map *m_pMap;
	swString m_bstrFName;

};

#endif 
