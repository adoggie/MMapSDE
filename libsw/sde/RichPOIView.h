#ifndef _RICH_POIVIEW_H
#define _RICH_POIVIEW_H

#include "swBase.h"

class CRichPOIView  
{
public:
	CRichPOIView();
	virtual ~CRichPOIView();

public:
	//����������ʾ��RichPOI�ļ���
	CPtrList* GetRichPOISet( int nBandID[500] , int nBandNum );
	void SetRichPOIFilePath( swString strFilePath );
private:
	//��Ҫ���Ƶ�RICHPOI�ܺ�
	CPtrList* m_pListBandPOI;
	//RICHPOI�ռ�����
	CPtrList* m_pSpaceIndex;
	//���Ի�RICHPOI�Ŀռ�����
	BOOL InitRichPOIView();
	//����ĳͼ����RICHPOI����
	BOOL LoadRichPOIByBandID( int nBandID );
	swString m_strFilePath;
};

#endif 
