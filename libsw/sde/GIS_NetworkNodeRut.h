
#ifndef _GIS_NETWORK_NODERUT_H
#define _GIS_NETWORK_NODERUT_H

#include "swBase.h"

class swEXPORT CGIS_NetworkNodeRut  
{
public:
	CGIS_NetworkNodeRut();
	virtual ~CGIS_NetworkNodeRut();

	void SetRutID( int nRutID );
	int GetRutID( );
	void SetRutPtNum( short nPtNum );
	short GetRutPtNum( );
	void SetRutPtList( ST_GEO_PTXY *pPtList );
	ST_GEO_PTXY* GetRutPtList( );
	void SetRutAnnoNum( BYTE nAnnoNum );
	BYTE GetRutAnnoNum( );
	void SetRutAnno( char *szAnno );
	char* GetRutAnno( );

private:
	int m_nRutID;	//Â·¶ÎID

	ST_GEO_PTXY *m_pPtList;
	short m_nPtNum;
	char *m_szAnno;
	BYTE m_nAnnoNum;
};

#endif 
