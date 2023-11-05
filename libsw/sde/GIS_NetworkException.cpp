// GIS_NetworkException.cpp: implementation of the CGIS_NetworkException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CGIS_NetworkException, CException)
CGIS_NetworkException _simpleNetworkException(FALSE, CGIS_NetworkException::MOUDLE_NONE,CGIS_NetworkException::ERROR_NONE);
 
void AFXAPI AfxThrowNetworkException()
{
	THROW((CGIS_NetworkException*)&_simpleNetworkException);
}

CGIS_NetworkException::CGIS_NetworkException(BOOL bAutoDelete,enModule eModule,enErrorCode eErrorCode)
    :CSimpleException(bAutoDelete),m_eModule(eModule),m_eErrorCode(eErrorCode)
{
}

CGIS_NetworkException::~CGIS_NetworkException()
{

}
