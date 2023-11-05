#ifndef _IGIS_MESSAGE_H
#define _IGIS_MESSAGE_H

#include "swBase.h"

#include "GIS_ValueManager.h"

class swEXPORT IGIS_Message  
{
public:
	IGIS_Message();
	virtual ~IGIS_Message();
public:
	  virtual UINT GetMessageType() = 0;
	  virtual CGIS_ValueManager* GetMessageData() = 0;
};

#endif
