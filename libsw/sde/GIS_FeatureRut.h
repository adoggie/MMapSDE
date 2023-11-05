
#ifndef _GIS_FEATURERUT_H
#define _GIS_FEATURERUT_H


#include "swBase.h"
#include "GIS_Feature.h"


class swEXPORT CGIS_FeatureRut : public CGIS_Feature  
{
//////////////////////////////////////////////////////////////////////////
public:
    typedef struct _tagLaneForOutID 
    {
        WORD	wOutLinkID;
        BYTE *	pbyLaneDir;
        
        _tagLaneForOutID()
        {
            pbyLaneDir = NULL;
        };
        ~_tagLaneForOutID()
        {
            release();
        };
        void release()
        {
        };
    }LANEFOROUTID, *LPLANEFOROUTID;
    
    struct LaneNumInfo
    {
        BYTE bit4OutLinkNum : 4;
        BYTE bit4LaneNum : 4;
    };
typedef struct _tagLaneInfo
{
	enum enLaneDir
	{
		Straight	= 0x80,			//直行
		SlightRight = 0x40,			//缓右
		Right		= 0x20,			//右
		SharpRight	= 0x10,			//急右
		UTurn		= 0x08,			//掉头
		SharpLeft	= 0x04,			//急左
		Left		= 0x02,			//左
		SlightLeft	= 0x01			//缓左
	};

    public:


	LaneNumInfo	byStartLaneNumInfo;
	BYTE *  pbyStartLaneDir;
	LPLANEFOROUTID lpStartLaneForOutID;

	LaneNumInfo	byEndLaneNumInfo;
	BYTE *  pbyEndLaneDir;
	LPLANEFOROUTID lpEndLaneForOutID;

	_tagLaneInfo()
	{
		memset(this,0,sizeof(_tagLaneInfo));
	};
	~_tagLaneInfo()
	{
		release();
	};
    void release()
    {
        if (pbyStartLaneDir != NULL)
        {
            delete [] pbyStartLaneDir;
            pbyStartLaneDir = NULL;
        }
        if (pbyEndLaneDir != NULL)
        {
            delete [] pbyEndLaneDir;
            pbyEndLaneDir = NULL;
        }
        
        if (lpStartLaneForOutID != NULL)
        {
            delete [] lpStartLaneForOutID;
            lpStartLaneForOutID = NULL;
        }
        if (lpEndLaneForOutID != NULL)
            
        {
            delete [] lpEndLaneForOutID;
            lpEndLaneForOutID = NULL;
        }
        
        
    };

    public:
        ST_LANDINFO* ExtractStartLaneInfo(unsigned short nID)
        {
            LPLANEFOROUTID pLaneForOutID = GetStartLaneInfoByExitRoadSectionID(nID);
            if(pLaneForOutID)
            {
                ST_LANDINFO* pLaneInfo = new ST_LANDINFO;
                pLaneInfo->nLanNum = byStartLaneNumInfo.bit4LaneNum;
                pLaneInfo->pbyLaneDir = new BYTE[pLaneInfo->nLanNum];
                pLaneInfo->pbyOutLaneDir = new BYTE[pLaneInfo->nLanNum];

                memcpy(pLaneInfo->pbyLaneDir,pbyStartLaneDir,sizeof(pLaneInfo->pbyLaneDir[0])*pLaneInfo->nLanNum);
                memcpy(pLaneInfo->pbyOutLaneDir,pLaneForOutID->pbyLaneDir,sizeof(pLaneInfo->pbyOutLaneDir[0])*pLaneInfo->nLanNum);
                return pLaneInfo;
            }
            return NULL;
        }
        ST_LANDINFO* ExtractEndLaneInfo(unsigned short nID)
        {
            LPLANEFOROUTID pLaneForOutID = GetEndLaneInfoByExitRoadSectionID(nID);
            if(pLaneForOutID)
            {
                ST_LANDINFO* pLaneInfo = new ST_LANDINFO;
                pLaneInfo->nLanNum = byEndLaneNumInfo.bit4LaneNum;
                pLaneInfo->pbyLaneDir = new BYTE[pLaneInfo->nLanNum];
                pLaneInfo->pbyOutLaneDir = new BYTE[pLaneInfo->nLanNum];

                memcpy(pLaneInfo->pbyLaneDir,pbyEndLaneDir,sizeof(pLaneInfo->pbyLaneDir[0])*pLaneInfo->nLanNum);
                memcpy(pLaneInfo->pbyOutLaneDir,pLaneForOutID->pbyLaneDir,sizeof(pLaneInfo->pbyOutLaneDir[0])*pLaneInfo->nLanNum);
                return pLaneInfo;
            }
            return NULL;
        }
    protected:
    LPLANEFOROUTID   GetStartLaneInfoByExitRoadSectionID(unsigned short nID)
    {
        int nCount = byStartLaneNumInfo.bit4OutLinkNum;
        for(int i = 0; i < nCount; i++)
        {
            if(lpStartLaneForOutID[i].wOutLinkID == nID)
                return &lpStartLaneForOutID[i];
        }
        return NULL;
    }
    LPLANEFOROUTID GetEndLaneInfoByExitRoadSectionID(unsigned short nID)
    {
        int nCount = byEndLaneNumInfo.bit4OutLinkNum;
        for(int i = 0; i < nCount; i++)
        {
            if(lpEndLaneForOutID[i].wOutLinkID == nID)
                return &lpEndLaneForOutID[i];
        }
        return NULL;
}

}LANEINFO,*LPLANEINFO;
typedef struct _tagSignInfo
{
	WORD 		wOutRutID;
	BYTE		byNameLen;
	char *		pszName;

	_tagSignInfo()
	{
		pszName = NULL;
	};
	~_tagSignInfo()
	{
		release();
	};
	void release()
	{
		if (pszName != NULL)
		{
			delete [] pszName;
			pszName = NULL;
		}
	};

}SIGNINFO, *LPSIGNINFO; 
typedef struct _tagImgInfo
{
    WORD	wOutRutID;
    int		nImgID;
    int		nArrowID;
}IMGINFO, *LPIMGINFO;
public:
	virtual CGIS_Feature* Clone();
	virtual CGIS_Feature* CloneV2();
	CGIS_FeatureRut(EnGeoObjType enOType);
	virtual ~CGIS_FeatureRut();


	void SetRectObj( ST_GEO_RECT rtObj );
	ST_GEO_RECT GetRectObj( );
	int GetRutID( );
	void SetRutID( int nRutID );
	BYTE GetRutDirection( );
	void SetRutDirection( BYTE byDirection );

    //////////////////////////////////////////////////////////////////////////
    void        SetImgInfoDirect    (BYTE nLength,LPIMGINFO pArray);    
    BOOL        SetImgInfo          (BYTE nLength,LPIMGINFO pArray);    
    void        SetSignInfoDirect   (BYTE nLength,LPSIGNINFO pArray);    
    BOOL        SetSignInfo         (BYTE nLength,LPSIGNINFO pArray);    
    //////////////////////////////////////////////////////////////////////////


    BYTE        GetImgInfoCount(){return m_byImgInfoCount;}
    LPIMGINFO   GetImgInfoArray(){return m_pImgInfo;}
    BYTE        GetSignInfoCount(){return m_bySignInfoCount;}
    LPSIGNINFO  GetSignInfoArray(){return m_pSignInfo;}
    //////////////////////////////////////////////////////////////////////////
    

    //////////////////////////////////////////////////////////////////////////
    LPIMGINFO   GetImgInfoByExitRoadSectionID(unsigned short nID);
    LPSIGNINFO   GetSignInfoByExitRoadSectionID(unsigned short nID);
    LPLANEFOROUTID   GetStartLaneInfoByExitRoadSectionID(LPLANEINFO* pLaneInfo,unsigned short nID);
    LPLANEFOROUTID   GetEndLaneInfoByExitRoadSectionID(LPLANEINFO* pLaneInfo,unsigned short nID);
    //////////////////////////////////////////////////////////////////////////
    

//////////////////////////////////////////////////////////////////////////
//bsxy
protected:  
    BOOL SetImgInfoAt   (IMemoryMng* pMemoryMng,int nLength,LPIMGINFO pArray   );
    BOOL SetSignInfoAt  (IMemoryMng* pMemoryMng,int nLength,LPSIGNINFO pArray        );
    BOOL        SetStartLaneInfoAt        (IMemoryMng* pMemoryMng,int nLength,BYTE* pArray);    
    BOOL        SetEndLaneInfoAt          (IMemoryMng* pMemoryMng,int nLength,BYTE* pArray);    
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//lijun add for memmory manage
#ifdef MEMORY_SELF_MANAGE
public:
	void* operator new(size_t size);
	void* operator new(size_t size,CGIS_FeatureRut* pObjAddr);
    void* operator new(size_t size,IMemoryMng2* pMemoryMng);
    void operator delete (void*,size_t);
#endif	
//////////////////////////////////////////////////////////////////////////    

private:
	ST_GEO_RECT	m_rtObj;
	short m_nRutID;
//  DWORD dwUnuse; 没有使用此变量
	BYTE		m_byDirection;	//路段单双行标志///(0表示双行，1表示end到start,2表示start到end,3表示双向禁行)
    
	BYTE	m_bySignInfoCount;
	BYTE	m_byImgInfoCount;
	BYTE	m_byShunXingCount;

	LPIMGINFO m_pImgInfo;
	LPSIGNINFO m_pSignInfo;


	//shunxing
	WORD *		m_pwShunXingOutRutID;
	DWORD	    m_dwLaneInfoOffset;	//(车道信息偏移)基于车道数据包起始位置的偏移

public:
    void        SetLaneInfoOffset(DWORD dwOffset){m_dwLaneInfoOffset = dwOffset;}   
    DWORD       GetLaneInfoOffset(){return m_dwLaneInfoOffset;}
    void ReadLaneInfo(BYTE * pBuf/*in*/, DWORD dwOffset/*in*/, LPLANEINFO pLaneInfo/*out*/);

};

#endif 
