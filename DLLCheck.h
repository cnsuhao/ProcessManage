#if !defined(AFX_DLLCHECK_H__0C7F8D9E_2918_47EA_AF82_B6F09E6D665D__INCLUDED_)
#define AFX_DLLCHECK_H__0C7F8D9E_2918_47EA_AF82_B6F09E6D665D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLLCheck.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLLCheck dialog

class CDLLCheck : public CDialog
{
// Construction
public:
	CDLLCheck(CWnd* pParent = NULL);   // standard constructor
	void ShowModule();
	void DebugPrivilege();
	void InitDLLList();
	void UnInjectDll(DWORD dwPid, char *szDllName);
// Dialog Data
	//{{AFX_DATA(CDLLCheck)
	enum { IDD = IDD_DIALOG_DLL };
	CListCtrl	m_CheckDLL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLLCheck)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLLCheck)
	afx_msg void OnBtnDLLQuit();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnUnInjectDll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLCHECK_H__0C7F8D9E_2918_47EA_AF82_B6F09E6D665D__INCLUDED_)
