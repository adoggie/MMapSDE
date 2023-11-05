// SDETestDoc.h : interface of the CSDETestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDETESTDOC_H__B78CDA34_B49A_479F_B965_236074C0FAF3__INCLUDED_)
#define AFX_SDETESTDOC_H__B78CDA34_B49A_479F_B965_236074C0FAF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CSDETestDoc : public CDocument
{
protected: // create from serialization only
	CSDETestDoc();
	DECLARE_DYNCREATE(CSDETestDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDETestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSDETestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSDETestDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDETESTDOC_H__B78CDA34_B49A_479F_B965_236074C0FAF3__INCLUDED_)
