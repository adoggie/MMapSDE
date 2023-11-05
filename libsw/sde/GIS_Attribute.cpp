
#include "stdafx.h"
#include "SDELib.h"
#include "GIS_Attribute.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CGIS_Attribute::CGIS_Attribute()
{
	m_pAtt = NULL;
}

CGIS_Attribute::~CGIS_Attribute()
{
	if( m_pAtt )
		delete m_pAtt;
}
void* CGIS_Attribute::GetValue( )
{
	return m_pAtt;
}
void CGIS_Attribute::SetValue( void *pAtt )
{
	m_pAtt = pAtt;
/*	if( m_pAtt )
		delete m_pAtt;
	memcpy( m_pAtt, pAtt, sizeof(pAtt) );
*/
}
