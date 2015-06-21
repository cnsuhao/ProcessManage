// ProcessManageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessManage.h"
#include "ProcessManageDlg.h"
#include <Tlhelp32.h>
#include "DLLCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessManageDlg dialog

CProcessManageDlg::CProcessManageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessManageDlg::IDD, pParent)
	, CEditA(0)
{
	//{{AFX_DATA_INIT(CProcessManageDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	TimeM = 0;
}

void CProcessManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CProcessManageDlg)
	DDX_Control(pDX, IDC_LIST_PROCESS, m_ProcessList);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_EditShutdownTime, CEditA);
}

BEGIN_MESSAGE_MAP(CProcessManageDlg, CDialog)
	//{{AFX_MSG_MAP(CProcessManageDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON5, OnBtnQuit)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBtnStop)
	ON_BN_CLICKED(IDC_BUTTON_TERMINATE, OnButtonTerminate)
	ON_BN_CLICKED(IDC_BUTTON_RESUME, OnButtonResume)
	ON_BN_CLICKED(IDC_BUTTON4, OnBtnDLL)
	//ON_COMMAND_RANGE(,,)
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_Process_Terminate, &CProcessManageDlg::OnProcessTerminate)
	ON_COMMAND(ID_Flush_One, &CProcessManageDlg::OnFlushOne)
	ON_COMMAND(ID_Process_Pause, &CProcessManageDlg::OnProcessPause)
	ON_COMMAND(ID_Process_Resume, &CProcessManageDlg::OnProcessResume)
	ON_COMMAND(ID_ViewModule, &CProcessManageDlg::OnViewmodule)
	ON_BN_CLICKED(IDC_ButtonAutoShutdown, &CProcessManageDlg::OnBnClickedButtonautoshutdown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessManageDlg message handlers

BOOL CProcessManageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitProcessList();
	ShowProcess();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessManageDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessManageDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessManageDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CProcessManageDlg::InitProcessList()  
{  
	//���á�List Control���ؼ�����չ���  
	m_ProcessList.SetExtendedStyle(  
		m_ProcessList.GetExtendedStyle()  
		| LVS_EX_GRIDLINES        //�������  
		| LVS_EX_FULLROWSELECT);  //ѡ��ĳ��ʹ���и�����ֻ������report���  

	//�����Ŀ  
	m_ProcessList.InsertColumn(0, _T("���"));  
	m_ProcessList.InsertColumn(1, _T("��������          "));  
	m_ProcessList.InsertColumn(2, _T("PIDֵ")); 
	m_ProcessList.InsertColumn(3, _T("�߳���"));
	m_ProcessList.InsertColumn(4, _T("������ID"));
	m_ProcessList.InsertColumn(5, _T("�߳����ȼ�"));
	//�����еĿ��  
	m_ProcessList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);  
	m_ProcessList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);  
	m_ProcessList.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);  
	m_ProcessList.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
	m_ProcessList.SetColumnWidth(4, LVSCW_AUTOSIZE_USEHEADER);
	m_ProcessList.SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
}  

void CProcessManageDlg::OnBtnQuit() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}

void CProcessManageDlg::ShowProcess()
{
	//����б�
	m_ProcessList.DeleteAllItems();
	//��ϵͳ�����еĽ����ĸ�����
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("���̿��մ���ʧ�ܣ�");
		return ;
	}

	PROCESSENTRY32 Pe32 = { 0 };
	//��ʹ������ṹǰ�����������Ĵ�С
	Pe32.dwSize = sizeof(PROCESSENTRY32);
	//�������̿��գ�������ʾÿ�����̵���Ϣ
	BOOL bRet = Process32First(hSnap, &Pe32);
	int i = 0;
	CString str;

	while ( bRet )
	{
		str.Format("%d", i);
		m_ProcessList.InsertItem(i, str);             
		//������
		m_ProcessList.SetItemText(i, 1, Pe32.szExeFile);
		//����ID
		str.Format("%d", Pe32.th32ProcessID);
		m_ProcessList.SetItemText(i, 2, str);
		//�˽��̿������̼߳���
		str.Format("%d", Pe32.cntThreads);
		m_ProcessList.SetItemText(i, 3, str);
		//������ID
		str.Format("%d", Pe32.th32ParentProcessID);
		m_ProcessList.SetItemText(i, 4, str);
		//�߳�����Ȩ
		str.Format("%d", Pe32.pcPriClassBase);
		m_ProcessList.SetItemText(i, 5, str);

		i ++;
		bRet = Process32Next(hSnap, &Pe32);
	}

	CloseHandle(hSnap);
}

int CProcessManageDlg::GetSelectPid()
{
	pid = -1;
	//��ȡ�б������ѡ�е�λ��
	POSITION Pos = m_ProcessList.GetFirstSelectedItemPosition();
	int nSelect = -1;
	while ( Pos )
	{
		nSelect = m_ProcessList.GetNextSelectedItem(Pos);
	}
	//������б����û�н���ѡ���򱨴�    
	if ( -1 == nSelect )
	{
		AfxMessageBox("��ѡ����̣�");
		return -1;
	}
	//��ȡ�б������ʾ��PIDֵ    
	char  szPid[10] = { 0 };
	m_ProcessList.GetItemText(nSelect, 2, szPid, 10);
	pid = atoi(szPid);

	return pid;
}


//��ͣ����
void CProcessManageDlg::OnBtnStop() 
{
	// TODO: Add your control notification handler code here
	int nPid = -1;
	nPid = GetSelectPid();
	//�����߳̿���
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("��ͣ����ʧ�ܣ�");
		return ;
	}

	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);

	while ( bRet )
	{
		//�ж��߳�����
		if ( Te32.th32OwnerProcessID == nPid )
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);

			SuspendThread(hThread);

			CloseHandle(hThread);
		}

		bRet = Thread32Next(hSnap, &Te32);
	}
}

//�رս���
void CProcessManageDlg::OnButtonTerminate() 
{
	// TODO: Add your control notification handler code here
	int nPid = GetSelectPid();

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, nPid);

	TerminateProcess(hProcess, 0);

	CloseHandle(hProcess);

	ShowProcess();
}

//������ͣ����
void CProcessManageDlg::OnButtonResume() 
{
	// TODO: Add your control notification handler code here
	int nPid = -1;
	nPid = GetSelectPid();

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("���ָ̻�ʧ�ܣ�");
		return ;
	}

	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);

	while ( bRet )
	{
		if ( Te32.th32OwnerProcessID == nPid )
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);

			ResumeThread(hThread);

			CloseHandle(hThread);
		}

		bRet = Thread32Next(hSnap, &Te32);
	}	
}

void CProcessManageDlg::OnBtnDLL() 
{
	// TODO: Add your control notification handler code here
	pid = GetSelectPid();
	CDLLCheck DLLCheck;
	DLLCheck.DoModal();
}

//�Ҽ��˵�ʵ��
void CProcessManageDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: �ڴ˴������Ϣ����������
	CMenu popMenu;

	popMenu.LoadMenu(IDR_ProcessRButtonMenu); //����˵�

	CMenu *pPopup;

	pPopup=popMenu.GetSubMenu(0); //����Ӳ˵�ָ��



	//pPopup->EnableMenuItem(ID_H,MF_BYCOMMAND|MF_ENABLED); //����˵���ʹ��

	//pPopup->EnableMenuItem(ID_LB001,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED); //������˵���ʹ��



	//ClientToScreen(&point); //���ͻ�������ת������Ļ����

	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this); //��ʾ�����˵�����������Ϊ(����ڲ˵����|�����Ҽ���x��y��this)

	pPopup->Detach();

	popMenu.DestroyMenu();
}

//�Ҽ��رս���
void CProcessManageDlg::OnProcessTerminate()
{
	// TODO: �ڴ���������������
	int nPid = GetSelectPid();

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, nPid);

	TerminateProcess(hProcess, 0);

	CloseHandle(hProcess);

	ShowProcess();
}

//�Ҽ�ˢ��һ��
void CProcessManageDlg::OnFlushOne()
{
	// TODO: �ڴ���������������
	ShowProcess();

}

//�Ҽ�������ͣ
void CProcessManageDlg::OnProcessPause()
{
	// TODO: �ڴ���������������
	int nPid = -1;
	nPid = GetSelectPid();
	//�����߳̿���
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("��ͣ����ʧ�ܣ�");
		return ;
	}

	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);

	while ( bRet )
	{
		//�ж��߳�����
		if ( Te32.th32OwnerProcessID == nPid )
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);

			SuspendThread(hThread);

			CloseHandle(hThread);
		}

		bRet = Thread32Next(hSnap, &Te32);
	}
}

//�Ҽ�������ͣ����
void CProcessManageDlg::OnProcessResume()
{
	// TODO: �ڴ���������������
	int nPid = -1;
	nPid = GetSelectPid();

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("���ָ̻�ʧ�ܣ�");
		return ;
	}

	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);

	while ( bRet )
	{
		if ( Te32.th32OwnerProcessID == nPid )
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);

			ResumeThread(hThread);

			CloseHandle(hThread);
		}

		bRet = Thread32Next(hSnap, &Te32);
	}	
}

//�Ҽ��鿴ģ��
void CProcessManageDlg::OnViewmodule()
{
	// TODO: �ڴ���������������
	pid = GetSelectPid();
	CDLLCheck DLLCheck;
	DLLCheck.DoModal();
}


void CProcessManageDlg::OnBnClickedButtonautoshutdown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	int cB=CEditA;

	if (cB>=60)
	{
		char cBuffA[100]={0};//ԴBuff
		char cBuffB[100]="shutdown -s -t ";//Ŀ��Buff

		_itoa_s(cB,cBuffA,10);

		strcat_s(cBuffB,cBuffA);

		system(cBuffB);
	}
	else
	{

		AfxMessageBox("ʱ��Ӧ�ô���60��");

	}	
}
