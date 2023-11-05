
#ifndef _GIS_NETWORK_EXCEPTION_H
#define _GIS_NETWORK_EXCEPTION_H

#include "swBase.h"
class CGIS_NetworkException : public CSimpleException
{
	DECLARE_DYNAMIC(CGIS_NetworkException)
//////////////////////////////////////////////////////////////////////////
public:
    enum enErrorCode
    {
        ERROR_NONE                                  = 0,
        ERROR_UNDEFINE                              = 2,
        ERROR_CAN_NOT_FIND_ROADSECTION_CLOSE_TO     = -1,
        ERROR_NETWORK_DISCONNECT                    = -2,
        ERROR_FILE_IO                               = -3,
        ERROR_OUT_OF_MEMORY                         = -4,
        ERROR_DISTANCE_TOO_LONG                     = -5,
        ERROR_CONDITION_NOT_ENOUGH                  = -6,
        ERROR_STOP_BY_OUTSIDE                       = -7,
        ERROR_TRUNK_NETWORK_DISCONNECT              = -8,
        ERROR_DATA_ERROR                            = -9,
        ERROR_INVALID_START_VERTEX_IN_TRUNK_NETWORK = -10,
        ERROR_INVALID_END_VERTEX_IN_TRUNK_NETWORK   = -11,
    };

    enum enModule
    {
        MOUDLE_NONE,
        MOUDLE_CALCULATE_PATH,
        MOUDLE_FULL_PATH_LEG_DATA,
        MOUDLE_CALCULATE_TURN_LIST,
    };

//////////////////////////////////////////////////////////////////////////
public:
	CGIS_NetworkException(BOOL bAutoDelete,enModule eModule,enErrorCode eErrorCode);
	virtual ~CGIS_NetworkException();
    enModule GetModule(){return m_eModule;}
    enErrorCode GetErrorCode(){return m_eErrorCode;}
    
protected:
	enModule m_eModule;
    enErrorCode m_eErrorCode;
};


//////////////////////////////////////////////////////////////////////////
void AFXAPI AfxThrowNetworkException();
//////////////////////////////////////////////////////////////////////////
#endif 
