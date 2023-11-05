

#ifndef _GIS_ATTR_H
#define _GIS_ATTR_H

class CGIS_Attribute  
{
public:
	CGIS_Attribute();
	virtual ~CGIS_Attribute();
	
	void* GetValue( );
	void SetValue( void *pAtt );
private:
	void *m_pAtt;
};

#endif
