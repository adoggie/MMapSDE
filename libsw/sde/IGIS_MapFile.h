#ifndef _IGIS_MAPFILE_H
#define _IGIS_MAPFILE_H

#include "swBase.h"

#include "IGIS_Map.h"
#include "IGIS_GeoFile.h"
//��ͼ�ļ�ͷ��������Ϣ�ṹ��
struct ST_MAP_HRADER{
	char		chHeader[4];	// ͷ����ʾ
	int			nLayerNum;		// �����Ŀ
	ST_GEO_RECT	rectMap;		// ��ͼ����Χ
};

struct ST_LAYER_INFO{
	char		chLayerType;	// ͼ�����ͣ�1-point��2-line��3-poly��4-rut��else-unknown
	char		chLayerName[31];
	int			nPenColor;
	int			nBrushColor;
	BYTE		byPenWidth;
	BYTE		byPenStyle;
	BYTE		byBrushStyle;
	BYTE		bySymbolType;
	float		fMinGR;			// hansom �����״˶����ʵ������
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
