
//#include "stdafx.h"
//#include "SDELib.h"
#include "IGIS_LayerFile.h"


IGIS_LayerFile::IGIS_LayerFile( ){
	m_bLoadMode = TRUE;
}

IGIS_LayerFile::IGIS_LayerFile( swString bstrLFName )
	:m_pLayer(NULL), m_bstrLFName( bstrLFName ){
	
}

IGIS_LayerFile::~IGIS_LayerFile()
{
	
}
void IGIS_LayerFile::SetLayer(IGIS_Layer *pLayer){
	m_pLayer = pLayer;
}

swString IGIS_LayerFile::GetLFName(){
	return m_bstrLFName;
}

EnLayFileType IGIS_LayerFile::GetLayFileType( ){
	return m_enLFType;
}

BOOL IGIS_LayerFile::IsLoadMode( )
{
	return m_bLoadMode;
}
