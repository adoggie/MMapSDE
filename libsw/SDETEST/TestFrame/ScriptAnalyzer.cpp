// ScriptAnalyzer.cpp: implementation of the CScriptAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ScriptAnalyzer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CScriptAnalyzer::CScriptAnalyzer()
{

}

CScriptAnalyzer::~CScriptAnalyzer()
{

}

BOOL CScriptAnalyzer::RunText(CString& strText)
{
	
	CString strLine = strText;
	strLine.TrimLeft();
	strLine.TrimRight();
    if(strLine.IsEmpty())
        return FALSE;

    if(strLine.GetAt(0) == _T('#'))
    {
        TRACE1("%s\n",strLine);
        return FALSE;
    }
	
	int nIndex = MetchCommand(strLine);
	if(nIndex == -1)
	{
		m_ParaStack.push(strLine);
		return FALSE;
	}


    //////////////////////////////////////////////////////////////////////////
    BOOL bResult = FALSE;
    CString strRunType = strText.Left(4);
    if(!_tcscmp(_T("同步调用"),strRunType))
    {
        bResult = SeriesRun(nIndex);
    }
    else if(!_tcscmp(_T("异步调用"),strRunType))
    {
        bResult = OverlappedRun(nIndex);
    }
	return bResult;
	
}

BOOL CScriptAnalyzer::AddCommand(stuCommandData& stCommandData)
{
	m_arCommandData.Add(stCommandData);

	return TRUE;
}
int CScriptAnalyzer::MetchCommand(CString strText)
{
    strText = strText.Right(strText.GetLength()-5);
	int nCount = m_arCommandData.GetSize();
	for(int i = 0; i < nCount; i++)
	{
		if(!_tcscmp(m_arCommandData[i].szCommand,strText))
			return i;
	}

	return -1;
}

BOOL CScriptAnalyzer::SeriesRun(int nIndex)
{
	if(nIndex < 0 || nIndex >= m_arCommandData.GetSize())
		return FALSE;


	CStringArray arPara;
	stuCommandData stCommandData = m_arCommandData[nIndex];
	int nParaCount = stCommandData.nParaCount;

    if( -1 == nParaCount )
    {
        nParaCount = m_ParaStack.size();
    }
    int i;
    for(i = 0; i < nParaCount; i++)
    {
        if(m_ParaStack.empty())
            return FALSE;
        
        arPara.Add(m_ParaStack.top()) ;
        m_ParaStack.pop();
    }        
    

	

    //////////////////////////////////////////////////////////////////////////
    //取出参数
	CStringArray arParaReverse;
	for(i = nParaCount - 1; i >= 0; i--)
	{
		arParaReverse.Add(arPara[i]);
	}
    
    
	return (*(stCommandData.Proc))(arParaReverse);
}

BOOL CScriptAnalyzer::OverlappedRun(int nIndex)
{
    if(nIndex < 0 || nIndex >= m_arCommandData.GetSize())
		return FALSE;


	CStringArray arPara;
	stuCommandData stCommandData = m_arCommandData[nIndex];
	int nParaCount = stCommandData.nParaCount;
	int i;
	for(i = 0; i < nParaCount; i++)
	{
        if(m_ParaStack.empty())
            return FALSE;

		arPara.Add(m_ParaStack.top()) ;
		m_ParaStack.pop();
	}	

    //////////////////////////////////////////////////////////////////////////

    tagThreadData* pData = new tagThreadData;
   
    //取出参数
	CStringArray arParaReverse;
	for(i = nParaCount - 1; i >= 0; i--)
	{
		pData->arString.Add(arPara[i]);
	}
    
    pData->stCommandData = stCommandData;


	TRACE0("启动线程\n");
	AfxBeginThread(CallThread,(LPVOID)pData,THREAD_PRIORITY_NORMAL,0);

	return TRUE;
}

UINT CScriptAnalyzer::CallThread(void* pParam)
{
    tagThreadData* pData =(tagThreadData*)pParam;
    ASSERT(pParam);

    stuCommandData stCommandData = pData->stCommandData;
    CStringArray arPara;
    for(int i = 0; i < pData->stCommandData.nParaCount; i++)
    {
        arPara.Add( pData->arString[i]);
    }
    delete pData;
    pData = NULL;
    
    return (*(stCommandData.Proc))(arPara);

}
