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
        //���á�List Control���ؼ�����չ���  
        m_CheckDLL.SetExtendedStyle(  
                m_CheckDLL.GetExtendedStyle()  
                | LVS_EX_GRIDLINES        //�������  
                | LVS_EX_FULLROWSELECT);  //ѡ��ĳ��ʹ���и�����ֻ������report���  
  
        //�����Ŀ  
        m_CheckDLL.InsertColumn(0, _T("���"));  
        m_CheckDLL.InsertColumn(1, _T("��    ��"));  
        m_CheckDLL.InsertColumn(2, _T("·    ��"));  
        //�����еĿ��  
        m_CheckDLL.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);  
        m_CheckDLL.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);  
        m_CheckDLL.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER); 		
}  

void CDLLCheck::ShowModule()
{
    //����б�
	m_CheckDLL.DeleteAllItems();
    //��ȡ�������еĹ�����������ѡ�н��̵�PIDֵ��
	CProcessManageDlg *p;
    p = (CProcessManageDlg *) GetParent();
    int nPid = p->pid;
    
	MODULEENTRY32 Me32 = { 0 };
    Me32.dwSize = sizeof(MODULEENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPid);
    if ( hSnap == INVALID_HANDLE_VALUE )
    {
        AfxMessageBox("����һƿ��");
		//����ʧ��Ӧ���˳�������
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

	    //��ȡ�б������ѡ�е�λ��  
        POSITION Pos = m_CheckDLL.GetFirstSelectedItemPosition();  
        int nSelect = -1;  
        while ( Pos )  
        {  
                nSelect = m_CheckDLL.GetNextSelectedItem(Pos);  
        }  
        //������б����û�н���ѡ���򱨴�      
        if ( -1 == nSelect )  
        {  
                AfxMessageBox("��ѡ��ģ�飡");  
                return;  
        }  
        //��ȡ�б����DLL������      
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
        //��ȡϵͳ���н��̡��̵߳ȵ��б�
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
        MODULEENTRY32 Me32 = { 0 };
        Me32.dwSize = sizeof(MODULEENTRY32);
        //���������������ĵ�һ��ģ�����Ϣ
        BOOL bRet = Module32First(hSnap, &Me32);
        while ( bRet )
        {
                //������ע���DLL
                if ( strcmp(Me32.szModule, szDllName) == 0 )
                {
                        flag = TRUE;
                        break;
                }
                //������һ��ģ����Ϣ
                bRet = Module32Next(hSnap, &Me32);
        }
        if (flag == FALSE)
        {
                AfxMessageBox("�Ҳ�����Ӧ��ģ��!");
                return;
        }

        CloseHandle(hSnap);

        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        if ( hProcess == NULL )
        {
                AfxMessageBox("���̴�ʧ��!");
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
                AfxMessageBox("����Զ���߳�ʧ��!");
                return;
        }
	
        AfxMessageBox("�ɹ�ж��!");
        //�ȴ��߳��˳�
        WaitForSingleObject(hThread, INFINITE);
    
        CloseHandle(hThread);
        CloseHandle(hProcess);
}

//�Ҽ��鿴ȫ��·��
void CDLLCheck::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: �ڴ˴������Ϣ����������
	//��ȡ�б������ѡ�е�λ��  
	POSITION Pos = m_CheckDLL.GetFirstSelectedItemPosition();  
	int nSelect = -1;  
	while ( Pos )  
	{  
		nSelect = m_CheckDLL.GetNextSelectedItem(Pos);  
	}  
	//������б����û�н���ѡ���򱨴�      
	if ( -1 == nSelect )  
	{  
		AfxMessageBox("��ѡ��ģ�飡");  
		return;  
	}  
	//��ȡ�б����DLL������      
	char  szDllPath[MAX_PATH] = { 0 };  
	m_CheckDLL.GetItemText(nSelect, 2, szDllPath, MAX_PATH);
	AfxMessageBox(szDllPath);
}
