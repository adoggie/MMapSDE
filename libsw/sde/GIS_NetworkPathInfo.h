
#ifndef _GIS_NETWORK_PATHINFO_H
#define _GIS_NETWORK_PATHINFO_H

#include "swBase.h"

class swEXPORT CGIS_NetworkPathInfo  
{
public:
	CGIS_NetworkPathInfo( swString strVFile );
	virtual ~CGIS_NetworkPathInfo();

public:
	///����/ȡ�� ���㷽ʽ
	EnCalPathMethod GetCalPathMethod( );
	void SetCalPathMethod( EnCalPathMethod enCalMethod );
	//����/ȡ�� ���ȷ�ʽ
	EnRoutePreference GetRoutePreference( );
	void SetRoutePreference( EnRoutePreference enRoutePref );
	//����/ȡ�� ���乤������
	EnVehicleType GetVehicleType( );
	void SetVehicleType( EnVehicleType enVehicleType );

	void SetCFRInfo( CGIS_FeatureRut *pRut, BYTE nDType ){
		m_pFRut = pRut;
		m_nDType = nDType;
	}
	CGIS_FeatureRut *GetCFRut( ){ return m_pFRut; }
	BYTE GetCFDType(){ return m_nDType;	}

	///���ûرܷ�ʽ
	void SetEviteInfo( BOOL bTollEvite, BOOL bHWEvite, BOOL bSelEvite, BOOL bUnlayEvite );
	BOOL IsTollEvite( );
	BOOL IsHWayEvite( );
	BOOL IsSelEvite( );
	BOOL IsUnlayEvite( );
	
	///����/ȡ�� Ҫ�ܿ��ĺ��ߵ���Ϣ
	void SetAvoidRutInfo( UINT nARNum, int *pARMapIDs, int *pARBIDs, int *pARRutIDs );
	void GetAvoidRutInfo( UINT &nARNum, int **pARMapIDs, int **pARBIDs, int **pARRutIDs );
    
	///����/ȡ�� Ҫ�ܿ��ĺ��ߵ���Ϣ
	void SetLimitRutInfo( UINT nARNum, int *pARMapIDs, int *pARBIDs, int *pARRutIDs );
	void GetLimitRutInfo( UINT &nARNum, int **pARMapIDs, int **pARBIDs, int **pARRutIDs );
	
	///�������������õ���������
	static BOOL GetMoibleVoiceIndex( ST_VOICE_NODE *pVNode, ST_VOICE_RUT *pVRStart, ST_VOICE_RUT *pVREnd, ST_VOICE_INDEX &indVoice );

	///���ļ��еõ�������ʾ��Ϣ
	static BOOL GetVoiceString(ST_VOICE_INDEX vInd,
			swString &strLVoice, swString &strSVoice, swString &strIVoice);

	//�жϽڵ㴦ת�䷽�������( ����Ƕ����ͣ�1.ֱ�� 2.����3.����4.����5.U�͹գ�//���䷽��(27����28����) )
	static void JudgeNodeAngleAndTurn( int nAngle, BYTE &byAngle, BYTE &byTurn );


    //////////////////////////////////////////////////////////////////////////
    //lijun 20070718���
    //����·������ͼ�Ĳü�����
    void SetInterSectionClipRectangle(float fExtendX,float fExtendY);
    void GetInterSectionClipRectangle(float& fExtendX,float& fExtendY);
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //lijun 20070821���
    //����·���������ʼʱ��
    void SetStartCalculateTime(SYSTEMTIME *plocTime);
    BYTE GetStartCalculateTime();
    //////////////////////////////////////////////////////////////////////////
    
private:
	///���㷽ʽ
	EnCalPathMethod m_enCalMethod;
	///��������
	EnRoutePreference m_enRoutePref;
	///���乤������
	EnVehicleType m_enVehicleType;
	
	//���ûرܷ�ʽ
	BOOL m_bTollEvite;		// �շ�
	BOOL m_bHWEvite;		// ����
	BOOL m_bSelEvite;		// ��ת
	BOOL m_bUnlayEvite;		// ��·

	UINT m_nARNum;
	int  *m_pARMapIDs;
	int	 *m_pARBIDs; 
	int	 *m_pARRutIDs;
    
	UINT m_nLimitRNum;
	int  *m_pLimitRMapIDs;
	int	 *m_pLimitRBIDs; 
	int	 *m_pLimitRRutIDs;

	CGIS_FeatureRut *m_pFRut;
	BYTE m_nDType;
	
	static FILE* m_pVFile;			//�����ļ���ָ��

    float m_fInterSectionClipRectangleExtendX;    //·������ͼ�Ĳü����� X������չֵ
    float m_fInterSectionClipRectangleExtendY;    //·������ͼ�Ĳü����� Y������չֵ

    BYTE   m_nStartCalculateTime;

};

#endif
