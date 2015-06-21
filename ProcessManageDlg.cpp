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
	//设置“List Control”控件的扩展风格  
	m_ProcessList.SetExtendedStyle(  
		m_ProcessList.GetExtendedStyle()  
		| LVS_EX_GRIDLINES        //有网络格  
		| LVS_EX_FULLROWSELECT);  //选中某行使整行高亮（只适用于report风格）  

	//添加列目  
	m_ProcessList.InsertColumn(0, _T("序号"));  
	m_ProcessList.InsertColumn(1, _T("进程名称          "));  
	m_ProcessList.InsertColumn(2, _T("PID值")); 
	m_ProcessList.InsertColumn(3, _T("线程数"));
	m_ProcessList.InsertColumn(4, _T("父进程ID"));
	m_ProcessList.InsertColumn(5, _T("线程优先级"));
	//设置列的宽度  
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
	//清空列表
	m_ProcessList.DeleteAllItems();
	//给系统内所有的进程拍个快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("进程快照创建失败！");
		return ;
	}

	PROCESSENTRY32 Pe32 = { 0 };
	//在使用这个结构前，先设置它的大小
	Pe32.dwSize = sizeof(PROCESSENTRY32);
	//遍历进程快照，轮流显示每个进程的信息
	BOOL bRet = Process32First(hSnap, &Pe32);
	int i = 0;
	CString str;

	while ( bRet )
	{
		str.Format("%d", i);
		m_ProcessList.InsertItem(i, str);             
		//进程名
		m_ProcessList.SetItemText(i, 1, Pe32.szExeFile);
		//进程ID
		str.Format("%d", Pe32.th32ProcessID);
		m_ProcessList.SetItemText(i, 2, str);
		//此进程开启的线程计数
		str.Format("%d", Pe32.cntThreads);
		m_ProcessList.SetItemText(i, 3, str);
		//父进程ID
		str.Format("%d", Pe32.th32ParentProcessID);
		m_ProcessList.SetItemText(i, 4, str);
		//线程优先权
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
	//获取列表框中所选中的位置
	POSITION Pos = m_ProcessList.GetFirstSelectedItemPosition();
	int nSelect = -1;
	while ( Pos )
	{
		nSelect = m_ProcessList.GetNextSelectedItem(Pos);
	}
	//如果在列表框中没有进行选择，则报错    
	if ( -1 == nSelect )
	{
		AfxMessageBox("请选择进程！");
		return -1;
	}
	//获取列表框中显示的PID值    
	char  szPid[10] = { 0 };
	m_ProcessList.GetItemText(nSelect, 2, szPid, 10);
	pid = atoi(szPid);

	return pid;
}


//暂停进程
void CProcessManageDlg::OnBtnStop() 
{
	// TODO: Add your control notification handler code here
	int nPid = -1;
	nPid = GetSelectPid();
	//创建线程快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("暂停进程失败！");
		return ;
	}

	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);

	while ( bRet )
	{
		//判断线程所属
		if ( Te32.th32OwnerProcessID == nPid )
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);

			SuspendThread(hThread);

			CloseHandle(hThread);
		}

		bRet = Thread32Next(hSnap, &Te32);
	}
}

//关闭进程
void CProcessManageDlg::OnButtonTerminate() 
{
	// TODO: Add your control notification handler code here
	int nPid = GetSelectPid();

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, nPid);

	TerminateProcess(hProcess, 0);

	CloseHandle(hProcess);

	ShowProcess();
}

//撤销暂停进程
void CProcessManageDlg::OnButtonResume() 
{
	// TODO: Add your control notification handler code here
	int nPid = -1;
	nPid = GetSelectPid();

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("进程恢复失败！");
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

//右键菜单实现
void CProcessManageDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	CMenu popMenu;

	popMenu.LoadMenu(IDR_ProcessRButtonMenu); //载入菜单

	CMenu *pPopup;

	pPopup=popMenu.GetSubMenu(0); //获得子菜单指针



	//pPopup->EnableMenuItem(ID_H,MF_BYCOMMAND|MF_ENABLED); //允许菜单项使用

	//pPopup->EnableMenuItem(ID_LB001,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED); //不允许菜单项使用



	//ClientToScreen(&point); //将客户区坐标转换成屏幕坐标

	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this); //显示弹出菜单，参数依次为(鼠标在菜单左边|跟踪右键，x，y，this)

	pPopup->Detach();

	popMenu.DestroyMenu();
}

//右键关闭进程
void CProcessManageDlg::OnProcessTerminate()
{
	// TODO: 在此添加命令处理程序代码
	int nPid = GetSelectPid();

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, nPid);

	TerminateProcess(hProcess, 0);

	CloseHandle(hProcess);

	ShowProcess();
}

//右键刷新一下
void CProcessManageDlg::OnFlushOne()
{
	// TODO: 在此添加命令处理程序代码
	ShowProcess();

}

//右键进程暂停
void CProcessManageDlg::OnProcessPause()
{
	// TODO: 在此添加命令处理程序代码
	int nPid = -1;
	nPid = GetSelectPid();
	//创建线程快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("暂停进程失败！");
		return ;
	}

	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);

	while ( bRet )
	{
		//判断线程所属
		if ( Te32.th32OwnerProcessID == nPid )
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);

			SuspendThread(hThread);

			CloseHandle(hThread);
		}

		bRet = Thread32Next(hSnap, &Te32);
	}
}

//右键撤销暂停进程
void CProcessManageDlg::OnProcessResume()
{
	// TODO: 在此添加命令处理程序代码
	int nPid = -1;
	nPid = GetSelectPid();

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
	if ( hSnap == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox("进程恢复失败！");
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

//右键查看模块
void CProcessManageDlg::OnViewmodule()
{
	// TODO: 在此添加命令处理程序代码
	pid = GetSelectPid();
	CDLLCheck DLLCheck;
	DLLCheck.DoModal();
}


void CProcessManageDlg::OnBnClickedButtonautoshutdown()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	int cB=CEditA;

	if (cB>=60)
	{
		char cBuffA[100]={0};//源Buff
		char cBuffB[100]="shutdown -s -t ";//目的Buff

		_itoa_s(cB,cBuffA,10);

		strcat_s(cBuffB,cBuffA);

		system(cBuffB);
	}
	else
	{

		AfxMessageBox("时间应该大于60秒");

	}	
}
