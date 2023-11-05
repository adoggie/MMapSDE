#ifndef _RICH_POIVIEW_H
#define _RICH_POIVIEW_H

#include "swBase.h"

class CRichPOIView  
{
public:
	CRichPOIView();
	virtual ~CRichPOIView();

public:
	//返回所需显示的RichPOI的集合
	CPtrList* GetRichPOISet( int nBandID[500] , int nBandNum );
	void SetRichPOIFilePath( swString strFilePath );
private:
	//需要绘制的RICHPOI总和
	CPtrList* m_pListBandPOI;
	//RICHPOI空间索引
	CPtrList* m_pSpaceIndex;
	//初试化RICHPOI的空间索引
	BOOL InitRichPOIView();
	//加载某图幅的RICHPOI数据
	BOOL LoadRichPOIByBandID( int nBandID );
	swString m_strFilePath;
};

#endif 
