
#ifndef _GIS_MESSAGE_H
#define _GIS_MESSAGE_H

#include "swBase.h"
#include "IGIS_Message.h"

class swEXPORT CGIS_Message : public IGIS_Message{
public:
	CGIS_Message();
	virtual ~CGIS_Message();
private:
	UINT m_Type;
	CGIS_ValueManager* m_pValueManager;
public:
	  virtual UINT GetMessageType();
	  virtual CGIS_ValueManager* GetMessageData();
	  BOOL SetValueManager(CGIS_ValueManager* pValueManager);
	  BOOL SetType(UINT IType);
};

#endif 
