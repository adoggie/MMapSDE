
#include "IGIS_MapFile.h"

IGIS_MapFile::IGIS_MapFile()
	:m_pMap(NULL){

}

IGIS_MapFile::~IGIS_MapFile(){

}

void IGIS_MapFile::SetMap( IGIS_Map *pMap ){
	m_pMap = pMap;
}

