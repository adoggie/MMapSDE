
#include "stdafx.h"
#include "IGIS_SDETool.h"

IGIS_SDETool::IGIS_SDETool(EnSDEToolType eSDEToolType):
    m_enSDEToolType(eSDEToolType)
{

}

IGIS_SDETool::~IGIS_SDETool()
{

}
EnSDEToolType IGIS_SDETool::GetSDEToolType( )
{
	return m_enSDEToolType;
}
