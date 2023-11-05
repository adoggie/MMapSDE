
#ifndef _GIS_QUERY_BYATTR_H
#define _GIS_QUERY_BYATTR_H

#include "swBase.h"
#include "IGIS_Query.h"

class swEXPORT CGIS_QueryByAtt : public IGIS_Query
{
public:
	CGIS_QueryByAtt();
	virtual ~CGIS_QueryByAtt();
public:
	//Feature
	BOOL QueryByType(CPtrList& pReturnFeaSet,UINT LayID, unsigned short ClassID);
	//Feature
	BOOL QueryByType(CPtrList& pReturnFeaSet,CPtrList* pFeaSet,UINT LayID, unsigned short ClassID);
	//Feature
	BOOL QueryByAnnotation(CPtrList& pReturnFeaSet,CPtrList* pFeaSet,swString strAnno);
	//Feature
	BOOL QueryByAnnotation(CPtrList& pReturnFeaSet,UINT LayID,unsigned short ClassID,swString strAnno);
	//Feature
	BOOL QueryByAnnotation(CPtrList& pReturnFeaSet,int BreathdMapId,int nLayerID,int nClassID,swString strAnno);
	//多个索引数据，避免重复加载和卸载
	BOOL QueryByAnnotation( CPtrList& pReturnFeaSet , CPtrList* IndexList );
	//根据大类和小类选择索引
	BOOL GetByClaAndLay(CPtrList& pReturnFeaSet, swString strMapPath, int nMapID, int nLayerID,int nClassID);
	//根据名字和层ID选择索引
	BOOL GetByNameAndLay(CPtrList& pReturnFeaSet, swString strMapPath, int nMapID, swString strName, int nLayerID);
	//得到开头字符是选取字符的索引列表（参数中可限制某一特定的邮政编码，默认为所有）
	BOOL GetNameNIN(CPtrList& pReturnFeaSet, swString strMapPath, int nCityID, swString strName, BYTE ntype,int nPostID = 0);
	//得到某层的所有索引（参数中可限制某一特定的邮政编码，默认为所有，可限制查找上限，默认为所有）
	BOOL GetLayerNIN(CPtrList& pReturnFeaSet, swString strMapPath, int nCityID,
		int nLayerID,int nPostID = 0,int nMax = 0 , int nClassID = 0 ,swString strFind = "");
	//从大类中得到小类
	BOOL GetClassNIN(CPtrList& pReturnFeaSet,CPtrList* pFeaSet, swString strMapPath,int nClassID);
	//索引
	BOOL GetNINByName(CPtrList& pReturnFeaSet,CPtrList* pFeaSet, swString strName);
	//Feature
	BOOL QueryByID(CGIS_Feature** pFeature,UINT nBreathMapID,UINT nFeatureID);
	//Feature
	BOOL QueryByID(CGIS_Feature** pFeature,UINT nBreathMapID,UINT nLayID,UINT nFeatureID);
	//设置终止查询
	void SetbStopSearch( BOOL bStopSearch );
	//简拼查询　返回值0:查询失败　1:查询结果为所有结果（下面的查询可以用返回集的二次查询）　2:查询结果为部分结果
	int GetInputHead(CPtrList& pReturnFeaSet, int nCityID, char* InputHead, 
		int nInputCount,BYTE ntype ,int nBigClass=-1,int nSmallClass=-1 );
	//返回所有类别的集合
	CPtrList* GetLevelList();
	//根据大类ID和城市ID返回所有RICHPOI
	BOOL GetRichPOIByCityID( CPtrList& pReturnFeaSet, int nType, int nCityID );
	//根据RICHPOI信息返回该RICHPOI的详细描述信息
	BOOL GetRichPOIText( RICHPOI_INFO RichPOIInfo , swString& strRichPOI );
public:
	
private:
	int             GetNameID(swString strName);
	BOOL m_bStopSearch;
	//CPtrList* m_pListLevel;
	std::list<RICHPOI_LEVEL1*> m_pListLevel;
private:
	BOOL InitRichPOILevel();
};

#endif
