// DataFileAnalyse.h: interface for the CDataFileAnalyse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAFILEANALYSE_H__662E64C2_0F3F_4066_82AD_DFE44E3AE705__INCLUDED_)
#define AFX_DATAFILEANALYSE_H__662E64C2_0F3F_4066_82AD_DFE44E3AE705__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDataFileAnalyse  
{
public:
	CDataFileAnalyse();
	virtual ~CDataFileAnalyse();
//����ͼ����
	unsigned long m_nBreadthCount;

//���ڽڵ��
	unsigned long m_nVertexCount;
	unsigned long m_nSideVertexCount;

	unsigned long m_nVertexWithSpecialEdgeCount[10];

//����·�ε�
	unsigned long m_nRoadSectionCount;

	void DumpState();

};

#endif // !defined(AFX_DATAFILEANALYSE_H__662E64C2_0F3F_4066_82AD_DFE44E3AE705__INCLUDED_)
