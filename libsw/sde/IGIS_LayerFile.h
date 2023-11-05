
#ifndef _IGIS_LAYERFILE_H
#define _IGIS_LAYERFILE_H

#include "swBase.h"

#include "IGIS_GeoFile.h"
#include "IGIS_Layer.h"

class IGIS_LayerFile : public IGIS_GeoFile  {
public:
	IGIS_LayerFile( );
	IGIS_LayerFile( swString bstrLFName );
	virtual ~IGIS_LayerFile();
	
public:
	virtual BOOL OpenLayer() = 0;
	virtual BOOL LoadLayer() = 0;
	virtual BOOL SaveToFile( swString bstrFName ) = 0;

	void SetLayer( IGIS_Layer *pLayer );
	swString GetLFName( );
	EnLayFileType GetLayFileType( );
	BOOL IsLoadMode( );
protected:
	swString m_bstrLFName;
	IGIS_Layer *m_pLayer;	//图层数据对象
	EnLayFileType m_enLFType;

	BOOL m_bLoadMode;
};

#endif 
