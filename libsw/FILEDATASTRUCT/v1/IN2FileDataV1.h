// IN2FileDataV1.h: interface for the CIN2FileDataV1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IN2FILEDATAV1_H__DE986EAF_95C1_4778_8C92_9EEFB1243577__INCLUDED_)
#define AFX_IN2FILEDATAV1_H__DE986EAF_95C1_4778_8C92_9EEFB1243577__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGIS_BreadthMap;
class CTempGeoMap;
//////////////////////////////////////////////////////////////////////////
class CIN2FileDataV1  
{
public:
	CIN2FileDataV1();
	virtual ~CIN2FileDataV1();

    BOOL Load(LPCTSTR arFilePath);     
    BOOL Write(LPCTSTR strFileName);  
    
    CGIS_BreadthMap* GetBreadthMapByID( int nRawBreadthID );
    CGIS_BreadthMap* GetBreadthMapByIndex( int nBreadthID );
    unsigned long GetBreadthCount()
    {
        return m_nBreadthCount;
    }
    CGIS_BreadthMap* GetBreadthList()
    {
        return m_pArrayOfBreadthMap;
    }

    CTempGeoMap* GetMapByIndex(int nIndex);
    BOOL InitialOperation();
    BOOL ClearOperation();
//////////////////////////////////////////////////////////////////////////
protected:
    CPtrList m_ListOfMap;
    
   	CGIS_BreadthMap*    m_pArrayOfBreadthMap;
    int                 m_nBreadthCount;

};

#endif // !defined(AFX_IN2FILEDATAV1_H__DE986EAF_95C1_4778_8C92_9EEFB1243577__INCLUDED_)
