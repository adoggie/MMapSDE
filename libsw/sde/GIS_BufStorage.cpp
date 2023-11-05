// GIS_BufStorage.cpp: implementation of the CGIS_BufStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_BufStorage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_BufStorage::CGIS_BufStorage()
{

}

CGIS_BufStorage::~CGIS_BufStorage()
{

}
void CGIS_BufStorage::SetBufStorageType( EnBufStorType enBSType )
{
	m_enBSType = enBSType;
}
EnBufStorType CGIS_BufStorage::GetBufStorageType( )
{
	return m_enBSType;
}
