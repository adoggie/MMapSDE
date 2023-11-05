// SDETestDoc.cpp : implementation of the CSDETestDoc class
//

#include "stdafx.h"
#include "SDETest.h"

#include "SDETestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDETestDoc

IMPLEMENT_DYNCREATE(CSDETestDoc, CDocument)

BEGIN_MESSAGE_MAP(CSDETestDoc, CDocument)
	//{{AFX_MSG_MAP(CSDETestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDETestDoc construction/destruction

CSDETestDoc::CSDETestDoc()
{
	// TODO: add one-time construction code here

}

CSDETestDoc::~CSDETestDoc()
{
}

BOOL CSDETestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSDETestDoc serialization

void CSDETestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSDETestDoc diagnostics

#ifdef _DEBUG
void CSDETestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSDETestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSDETestDoc commands
