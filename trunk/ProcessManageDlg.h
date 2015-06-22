// ProcessManageDlg.h : header file
//

#if !defined(AFX_PROCESSMANAGEDLG_H__D7D4D0A2_44DF_41BB_A274_82BDB77FC2EF__INCLUDED_)
#define AFX_PROCESSMANAGEDLG_H__D7D4D0A2_44DF_41BB_A274_82BDB77FC2EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CProcessManageDlg dialog

class CProcessManageDlg : public CDialog
{
// Construction
public:
	CProcessManageDlg(CWnd* pParent = NULL);	// standard constructor
    int GetSelectPid();
	void InitProcessList();
	void ShowProcess();
	int pid;
// Dialog Data
	//{{AFX_DATA(CProcessManageDlg)
	enum { IDD = IDD_PROCESSMANAGE_DIALOG };
	CListCtrl	m_ProcessList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessManageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CProcessManageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnQuit();
	afx_msg void OnBtnStop();
	afx_msg void OnButtonTerminate();
	afx_msg void OnButtonResume();
	afx_msg void OnBtnDLL();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnProcessTerminate();
	afx_msg void OnFlushOne();
	afx_msg void OnProcessPause();
	afx_msg void OnProcessResume();
	afx_msg void OnViewmodule();
	int TimeM;
	
	int CEditA;
	afx_msg void OnBnClickedButtonautoshutdown();
	afx_msg void OnNtsdkillProcess();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSMANAGEDLG_H__D7D4D0A2_44DF_41BB_A274_82BDB77FC2EF__INCLUDED_)
