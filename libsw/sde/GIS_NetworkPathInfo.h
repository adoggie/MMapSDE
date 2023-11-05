
#ifndef _GIS_NETWORK_PATHINFO_H
#define _GIS_NETWORK_PATHINFO_H

#include "swBase.h"

class swEXPORT CGIS_NetworkPathInfo  
{
public:
	CGIS_NetworkPathInfo( swString strVFile );
	virtual ~CGIS_NetworkPathInfo();

public:
	///设置/取得 计算方式
	EnCalPathMethod GetCalPathMethod( );
	void SetCalPathMethod( EnCalPathMethod enCalMethod );
	//设置/取得 优先方式
	EnRoutePreference GetRoutePreference( );
	void SetRoutePreference( EnRoutePreference enRoutePref );
	//设置/取得 运输工具类型
	EnVehicleType GetVehicleType( );
	void SetVehicleType( EnVehicleType enVehicleType );

	void SetCFRInfo( CGIS_FeatureRut *pRut, BYTE nDType ){
		m_pFRut = pRut;
		m_nDType = nDType;
	}
	CGIS_FeatureRut *GetCFRut( ){ return m_pFRut; }
	BYTE GetCFDType(){ return m_nDType;	}

	///设置回避方式
	void SetEviteInfo( BOOL bTollEvite, BOOL bHWEvite, BOOL bSelEvite, BOOL bUnlayEvite );
	BOOL IsTollEvite( );
	BOOL IsHWayEvite( );
	BOOL IsSelEvite( );
	BOOL IsUnlayEvite( );
	
	///设置/取得 要避开的航线的信息
	void SetAvoidRutInfo( UINT nARNum, int *pARMapIDs, int *pARBIDs, int *pARRutIDs );
	void GetAvoidRutInfo( UINT &nARNum, int **pARMapIDs, int **pARBIDs, int **pARRutIDs );
    
	///设置/取得 要避开的航线的信息
	void SetLimitRutInfo( UINT nARNum, int *pARMapIDs, int *pARBIDs, int *pARRutIDs );
	void GetLimitRutInfo( UINT &nARNum, int **pARMapIDs, int **pARBIDs, int **pARRutIDs );
	
	///根据语音条件得到语音索引
	static BOOL GetMoibleVoiceIndex( ST_VOICE_NODE *pVNode, ST_VOICE_RUT *pVRStart, ST_VOICE_RUT *pVREnd, ST_VOICE_INDEX &indVoice );

	///从文件中得到语音提示信息
	static BOOL GetVoiceString(ST_VOICE_INDEX vInd,
			swString &strLVoice, swString &strSVoice, swString &strIVoice);

	//判断节点处转弯方向和类型( 拐弯角度类型（1.直行 2.缓拐3.拐弯4.急拐5.U型拐）//拐弯方向(27．左28．右) )
	static void JudgeNodeAngleAndTurn( int nAngle, BYTE &byAngle, BYTE &byTurn );


    //////////////////////////////////////////////////////////////////////////
    //lijun 20070718添加
    //设置路口扩大图的裁剪矩形
    void SetInterSectionClipRectangle(float fExtendX,float fExtendY);
    void GetInterSectionClipRectangle(float& fExtendX,float& fExtendY);
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //lijun 20070821添加
    //设置路径计算的起始时间
    void SetStartCalculateTime(SYSTEMTIME *plocTime);
    BYTE GetStartCalculateTime();
    //////////////////////////////////////////////////////////////////////////
    
private:
	///计算方式
	EnCalPathMethod m_enCalMethod;
	///优先类型
	EnRoutePreference m_enRoutePref;
	///运输工具类型
	EnVehicleType m_enVehicleType;
	
	//设置回避方式
	BOOL m_bTollEvite;		// 收费
	BOOL m_bHWEvite;		// 高速
	BOOL m_bSelEvite;		// 回转
	BOOL m_bUnlayEvite;		// 土路

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
	
	static FILE* m_pVFile;			//语音文件的指针

    float m_fInterSectionClipRectangleExtendX;    //路口扩大图的裁剪矩形 X方向扩展值
    float m_fInterSectionClipRectangleExtendY;    //路口扩大图的裁剪矩形 Y方向扩展值

    BYTE   m_nStartCalculateTime;

};

#endif
