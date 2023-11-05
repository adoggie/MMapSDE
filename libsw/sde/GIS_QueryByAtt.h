
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
	//����������ݣ������ظ����غ�ж��
	BOOL QueryByAnnotation( CPtrList& pReturnFeaSet , CPtrList* IndexList );
	//���ݴ����С��ѡ������
	BOOL GetByClaAndLay(CPtrList& pReturnFeaSet, swString strMapPath, int nMapID, int nLayerID,int nClassID);
	//�������ֺͲ�IDѡ������
	BOOL GetByNameAndLay(CPtrList& pReturnFeaSet, swString strMapPath, int nMapID, swString strName, int nLayerID);
	//�õ���ͷ�ַ���ѡȡ�ַ��������б������п�����ĳһ�ض����������룬Ĭ��Ϊ���У�
	BOOL GetNameNIN(CPtrList& pReturnFeaSet, swString strMapPath, int nCityID, swString strName, BYTE ntype,int nPostID = 0);
	//�õ�ĳ������������������п�����ĳһ�ض����������룬Ĭ��Ϊ���У������Ʋ������ޣ�Ĭ��Ϊ���У�
	BOOL GetLayerNIN(CPtrList& pReturnFeaSet, swString strMapPath, int nCityID,
		int nLayerID,int nPostID = 0,int nMax = 0 , int nClassID = 0 ,swString strFind = "");
	//�Ӵ����еõ�С��
	BOOL GetClassNIN(CPtrList& pReturnFeaSet,CPtrList* pFeaSet, swString strMapPath,int nClassID);
	//����
	BOOL GetNINByName(CPtrList& pReturnFeaSet,CPtrList* pFeaSet, swString strName);
	//Feature
	BOOL QueryByID(CGIS_Feature** pFeature,UINT nBreathMapID,UINT nFeatureID);
	//Feature
	BOOL QueryByID(CGIS_Feature** pFeature,UINT nBreathMapID,UINT nLayID,UINT nFeatureID);
	//������ֹ��ѯ
	void SetbStopSearch( BOOL bStopSearch );
	//��ƴ��ѯ������ֵ0:��ѯʧ�ܡ�1:��ѯ���Ϊ���н��������Ĳ�ѯ�����÷��ؼ��Ķ��β�ѯ����2:��ѯ���Ϊ���ֽ��
	int GetInputHead(CPtrList& pReturnFeaSet, int nCityID, char* InputHead, 
		int nInputCount,BYTE ntype ,int nBigClass=-1,int nSmallClass=-1 );
	//�����������ļ���
	CPtrList* GetLevelList();
	//���ݴ���ID�ͳ���ID��������RICHPOI
	BOOL GetRichPOIByCityID( CPtrList& pReturnFeaSet, int nType, int nCityID );
	//����RICHPOI��Ϣ���ظ�RICHPOI����ϸ������Ϣ
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
