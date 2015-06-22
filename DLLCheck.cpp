// DLLCheck.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessManage.h"
#include "ProcessManageDlg.h"
#include "DLLCheck.h"
#include <Tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLLCheck dialog


CDLLCheck::CDLLCheck(CWnd* pParent /*=NULL*/)
	: CDialog(CDLLCheck::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLLCheck)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
}


void CDLLCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLLCheck)
	DDX_Control(pDX, IDC_LIST_DLL, m_CheckDLL);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDLLCheck, CDialog)
	//{{AFX_MSG_MAP(CDLLCheck)
	ON_BN_CLICKED(IDC_BUTTON2, OnBtnDLLQuit)
	ON_BN_CLICKED(IDC_BUTTON1, OnBtnUnInjectDll)
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLLCheck message handlers

void CDLLCheck::OnBtnDLLQuit() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}

BOOL CDLLCheck::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	// TODO: Add extra initialization here	
	DebugPrivilege();
	InitDLLList();
	ShowModule();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDLLCheck::InitDLLList()  
{  
        //设置“List Control”控件的扩展风格  
        m_CheckDLL.SetExtendedStyle(  
                m_CheckDLL.GetExtendedStyle()  
                | LVS_EX_GRIDLINES        //有网络格  
                | LVS_EX_FULLROWSELECT);  //选中某行使整行高亮（只适用于report风格）  
  
        //添加列目  
        m_CheckDLL.InsertColumn(0, _T("序号"));  
        m_CheckDLL.InsertColumn(1, _T("名    称"));  
        m_CheckDLL.InsertColumn(2, _T("路    径"));  
        //设置列的宽度  
        m_CheckDLL.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);  
        m_CheckDLL.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);  
        m_CheckDLL.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER); 		
}  

void CDLLCheck::ShowModule()
{
    //清空列表
	m_CheckDLL.DeleteAllItems();
    //获取父窗口中的公共变量（所选中进程的PID值）
	CProcessManageDlg *p;
    p = (CProcessManageDlg *) GetParent();
    int nPid = p->pid;
    
	MODULEENTRY32 Me32 = { 0 };
    Me32.dwSize = sizeof(MODULEENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPid);
    if ( hSnap == INVALID_HANDLE_VALUE )
    {
        AfxMessageBox("再来一瓶！");
		//创建失败应当退出本窗口
		CDLLCheck::OnBtnDLLQuit();
        return ;
    }
    
	BOOL bRet = Module32First(hSnap, &Me32);
    int i = 0;
    CString str;
    while ( bRet )
    {
        str.Format("%d", i);
        m_CheckDLL.InsertItem(i, str);
        m_CheckDLL.SetItemText(i, 1, Me32.szModule);
        m_CheckDLL.SetItemText(i, 2, Me32.szExePath);
        i ++;
        bRet = Module32Next(hSnap, &Me32);
    }
}

void CDLLCheck::DebugPrivilege()
{
    HANDLE hToken = NULL;

    BOOL bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
    
    if ( bRet == TRUE )
    {
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
        
        CloseHandle(hToken);
    }
}

void CDLLCheck::OnBtnUnInjectDll() 
{
	    // TODO: Add your control notification handler code here
    	CProcessManageDlg *p;
        p = (CProcessManageDlg *) GetParent();
        int nPid = p->pid;

	    //获取列表框中所选中的位置  
        POSITION Pos = m_CheckDLL.GetFirstSelectedItemPosition();  
        int nSelect = -1;  
        while ( Pos )  
        {  
                nSelect = m_CheckDLL.GetNextSelectedItem(Pos);  
        }  
        //如果在列表框中没有进行选择，则报错      
        if ( -1 == nSelect )  
        {  
                AfxMessageBox("请选择模块！");  
                return;  
        }  
        //获取列表框中DLL的名称      
        char  szDllName[MAX_PATH] = { 0 };  
        m_CheckDLL.GetItemText(nSelect, 1, szDllName, MAX_PATH);  

        UnInjectDll(nPid,szDllName);
		ShowModule();
}

void CDLLCheck::UnInjectDll(DWORD dwPid, char *szDllName)
{
        BOOL flag = FALSE;
        if ( dwPid == 0 || strlen(szDllName) == 0 )
        {
                return;
        }
        //获取系统运行进程、线程等的列表
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
        MODULEENTRY32 Me32 = { 0 };
        Me32.dwSize = sizeof(MODULEENTRY32);
        //检索与进程相关联的第一个模块的信息
        BOOL bRet = Module32First(hSnap, &Me32);
        while ( bRet )
        {
                //查找所注入的DLL
                if ( strcmp(Me32.szModule, szDllName) == 0 )
                {
                        flag = TRUE;
                        break;
                }
                //检索下一个模块信息
                bRet = Module32Next(hSnap, &Me32);
        }
        if (flag == FALSE)
        {
                AfxMessageBox("找不到相应的模块!");
                return;
        }

        CloseHandle(hSnap);

        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        if ( hProcess == NULL )
        {
                AfxMessageBox("进程打开失败!");
                return ;
        }

        FARPROC pFunAddr = GetProcAddress(GetModuleHandle("kernel32.dll"),"FreeLibrary");
    
        HANDLE hThread = CreateRemoteThread(hProcess,
                        NULL,
                        0,
                        (LPTHREAD_START_ROUTINE)pFunAddr,
                        Me32.hModule,
                        0,
                        NULL);
        if (hThread == NULL)
        {
                AfxMessageBox("创建远程线程失败!");
                return;
        }
	
        AfxMessageBox("成功卸载!");
        //等待线程退出
        WaitForSingleObject(hThread, INFINITE);
    
        CloseHandle(hThread);
        CloseHandle(hProcess);
}

//右键查看全部路径
void CDLLCheck::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
	//获取列表框中所选中的位置  
	POSITION Pos = m_CheckDLL.GetFirstSelectedItemPosition();  
	int nSelect = -1;  
	while ( Pos )  
	{  
		nSelect = m_CheckDLL.GetNextSelectedItem(Pos);  
	}  
	//如果在列表框中没有进行选择，则报错      
	if ( -1 == nSelect )  
	{  
		AfxMessageBox("请选择模块！");  
		return;  
	}  
	//获取列表框中DLL的名称      
	char  szDllPath[MAX_PATH] = { 0 };  
	m_CheckDLL.GetItemText(nSelect, 2, szDllPath, MAX_PATH);
	AfxMessageBox(szDllPath);
}
