// ShowProcessDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CodeInject.h"
#include "ShowProcessDlg.h"
#include "afxdialogex.h"
#include "HookBypass.h"
// CShowProcessDlg 对话框

IMPLEMENT_DYNAMIC(CShowProcessDlg, CDialogEx)

CShowProcessDlg::CShowProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SHOWPROCESS_DIALOG, pParent)
{

}

CShowProcessDlg::~CShowProcessDlg()
{
}

void CShowProcessDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LISTPROCESS, m_listProcess);
    DDX_Control(pDX, IDC_LISTMODULE, m_listModule);
    DDX_Control(pDX, IDC_COMINJECTOR, m_comInjector);
}


BEGIN_MESSAGE_MAP(CShowProcessDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTNINJECTDLL, &CShowProcessDlg::OnBnClickedBtninjectdll)
    ON_NOTIFY(NM_CLICK, IDC_LISTPROCESS, &CShowProcessDlg::OnNMClickListprocess)
    ON_BN_CLICKED(IDC_BTNUNINJECTDLL, &CShowProcessDlg::OnBnClickedBtnuninjectdll)
    ON_BN_CLICKED(IDC_BTNREFRESH, &CShowProcessDlg::OnBnClickedBtnrefresh)
END_MESSAGE_MAP()


// CShowProcessDlg 消息处理程序
DWORD CALLBACK FindGameThread(LPVOID lParam)
{
    CShowProcessDlg *pDlg = static_cast<CShowProcessDlg*>(lParam);
    if (pDlg == NULL && pDlg->m_hWnd == NULL)
    {
        return 0;
    }
    HANDLE hProcess;                        //进程句柄
    BOOL bProcess = FALSE;                  //获取进程信息的函数返回值

    PROCESSENTRY32 pe;                    //保存进程信息
    pe.dwSize = sizeof(PROCESSENTRY32);
    
    hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获取进程快照

    bProcess = Process32First(hProcess, &pe);              //获取第一个进程信息
    while (bProcess)
    {
        pDlg->InsertProcessInfo(pe);
        
        bProcess = Process32Next(hProcess, &pe);
    }
        
    return 0;
}

BOOL CShowProcessDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_listProcess.InsertColumn(0,_T("PID"), LVCFMT_CENTER,120);
    m_listProcess.InsertColumn(1, _T("进程名"), LVCFMT_CENTER,180);
    m_listProcess.InsertColumn(2, _T("路径"), LVCFMT_CENTER,420);
    m_listProcess.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    m_listModule.InsertColumn(0, _T("模块名"), LVCFMT_CENTER, 120);
    m_listModule.InsertColumn(1, _T("模块大小"), LVCFMT_CENTER, 180);
    m_listModule.InsertColumn(2, _T("路径"), LVCFMT_CENTER, 420);
    m_listModule.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    m_comInjector.InsertString(-1, _T("远程线程注入"));
    m_comInjector.InsertString(-1, _T("HOOKAPI后远程线程"));
    m_comInjector.SetCurSel(0);
    CloseHandle(CreateThread(NULL, 0, FindGameThread, this, 0, NULL));

    return TRUE;  // return TRUE unless you set the focus to a control
}


void CShowProcessDlg::OnBnClickedBtninjectdll()
{
    TCHAR szFileName[MAX_PATH] = { 0 };

    OPENFILENAME openFileName = { 0 };
    openFileName.lStructSize = sizeof(OPENFILENAME);
    openFileName.lpstrInitialDir = NULL;
    openFileName.nMaxFile = MAX_PATH;
    openFileName.lpstrFilter = L"DLL(*.dll)\0*.dll\0";
    openFileName.lpstrFile = szFileName;
    openFileName.nFilterIndex = 1;
    openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (!GetOpenFileName(&openFileName))
    {
        return;
    }
    bool bIsInject = false;
    switch (m_comInjector.GetCurSel())
    {
    case 0:
        bIsInject = RemoteThreadInjectdll(szFileName);
        break;
    case 1:
        bIsInject = MemonyInjectdll(szFileName);
        break;
    default:
        break;
    }
    
    if (!bIsInject)
    {
        MessageBox(_T("注入失败"));
    }
    else
    {
        MessageBox(szFileName);
    }

//     HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _ttoi(m_strPid));
//     if (hProcess == NULL)
//     {
//         return;
//     }
// 
//     LPVOID lpAdd = NULL;
//     lpAdd = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
//     if (lpAdd == NULL)
//     {
//         return;
//     }
//     char szPath[MAX_PATH] = { 0 };
//     TcharToChar(szFileName, szPath);
//     if (!WriteProcessMemory(hProcess, lpAdd, szPath, strlen(szPath) + 1, NULL))
//     {
//         VirtualFreeEx(hProcess, lpAdd, MAX_PATH, MEM_RELEASE);
//         return;
//     }
// 
//     HMODULE hModule = GetModuleHandle(L"kernel32.dll");
// 
//     HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA"), lpAdd, 0, NULL);
//     CloseHandle(hThread);
//     VirtualFreeEx(hProcess, lpAdd, MAX_PATH, MEM_RELEASE);
}

void CShowProcessDlg::InsertProcessInfo(PROCESSENTRY32& pe)
{
    m_listProcess.GetItemCount();
    CString strPid;
    strPid.Format(_T("%08X  [%d]"), pe.th32ProcessID, pe.th32ProcessID);
    int index = m_listProcess.InsertItem(0, strPid);
    m_listProcess.SetItemText(index, 1, pe.szExeFile);
    m_listProcess.SetItemText(index, 2, pe.szExeFile);
}

void CShowProcessDlg::OnNMClickListprocess(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    HANDLE hModule;//获取模块信息的函数返回值       
    BOOL bModule = FALSE;//模块句柄                
    MODULEENTRY32  me;                    //保存模块信息
    me.dwSize = sizeof(MODULEENTRY32);
    int nCurSel = m_listProcess.GetSelectionMark();
    if (nCurSel <= -1)
    {
        return;
    }
    m_strPid.Empty();
    CString strPid = m_listProcess.GetItemText(nCurSel, 0);
    int nPos = strPid.Find(_T("["));
    if (nPos == -1)
    {
        return;
    }
    m_listModule.DeleteAllItems();
    strPid.Replace(_T("]"), _T(""));
    m_strPid = strPid.Right(strPid.GetLength() - nPos - 1);
    hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _ttoi(m_strPid));  //获取模块快照
    if (hModule != INVALID_HANDLE_VALUE)
    {
        bModule = Module32First(hModule, &me);      //获取第一个模块信息,即进程相应可执行文件的信息
        while (bModule)
        {
            int index = m_listModule.InsertItem(0, me.szModule);
            CString strBuff;
            strBuff.Format(_T("%d"),me.dwSize);
            m_listModule.SetItemText(index, 1, strBuff);
            m_listModule.SetItemText(index, 2, me.szExePath);
            bModule = Module32Next(hModule, &me);  //获取其他模块信息
        }
    }
    *pResult = 0;
}

CString CShowProcessDlg::GetSelectPid()
{
    return m_strPid;
}

void CShowProcessDlg::OnBnClickedBtnuninjectdll()
{
    int nCurSel = m_listModule.GetSelectionMark();
    if (nCurSel <= -1)
    {
        return;
    }
    CString strMoudle = m_listModule.GetItemText(nCurSel, 2);
    Uninstall(_ttoi(m_strPid),strMoudle);
}

void CShowProcessDlg::Uninstall(DWORD dwPid, CString szGameDllPath)
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
    // 获取模块快照  
    hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
    if (INVALID_HANDLE_VALUE == hModuleSnap)
    {
        return ;
    }
    MODULEENTRY32 me32;
    memset(&me32, 0, sizeof(MODULEENTRY32));
    me32.dwSize = sizeof(MODULEENTRY32);
    // 开始遍历  
    if (FALSE == ::Module32First(hModuleSnap, &me32))
    {
        ::CloseHandle(hModuleSnap);
        return ;
    }
    // 遍历查找指定模块  
    bool isFound = false;
    do
    {
        isFound = (0 == _tcsicmp(me32.szExePath, szGameDllPath));
        if (isFound) // 找到指定模块  
        {
            break;
        }
    } while (TRUE == ::Module32Next(hModuleSnap, &me32));
    ::CloseHandle(hModuleSnap);
    if (false == isFound)
    {
        return ;
    }
    // 获取目标进程句柄  
    hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    if (NULL == hProcess)
    {
        return ;
    }
    // 从 Kernel32.dll 中获取 FreeLibrary 函数地址  
    LPTHREAD_START_ROUTINE lpThreadFun = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "FreeLibrary");
    if (NULL == lpThreadFun)
    {
        ::CloseHandle(hProcess);
        return ;
    }
    // 创建远程线程调用 FreeLibrary  
    hThread = ::CreateRemoteThread(hProcess, NULL, 0, lpThreadFun, me32.modBaseAddr /* 模块地址 */, 0, NULL);
    if (NULL == hThread)
    {
        ::CloseHandle(hProcess);
        return ;
    }
    // 等待远程线程结束  
    ::WaitForSingleObject(hThread, INFINITE);
    // 清理  
    ::CloseHandle(hThread);
    ::CloseHandle(hProcess);
    return ;
}

void CShowProcessDlg::TcharToChar(const TCHAR *tchar, char *_char)
{
    int iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

void CShowProcessDlg::OnBnClickedBtnrefresh()
{
    m_listProcess.DeleteAllItems();
    m_listModule.DeleteAllItems();
    CreateThread(NULL, 0, FindGameThread, this, 0, NULL);
}

bool CShowProcessDlg::RemoteThreadInjectdll(TCHAR* szDllPath)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _ttoi(m_strPid));
    if (hProcess == NULL)
    {
        return false;
    }

    LPVOID lpAdd = NULL;
    lpAdd = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (lpAdd == NULL)
    {
        return false;
    }
    char szPath[MAX_PATH] = { 0 };
    TcharToChar(szDllPath, szPath);
    if (!WriteProcessMemory(hProcess, lpAdd, szPath, strlen(szPath) + 1, NULL))
    {
        VirtualFreeEx(hProcess, lpAdd, MAX_PATH, MEM_RELEASE);
        return false;
    }

    HMODULE hModule = GetModuleHandle(L"kernel32.dll");

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA"), lpAdd, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, lpAdd, MAX_PATH, MEM_RELEASE);

    return true;
}

bool CShowProcessDlg::MemonyInjectdll(TCHAR* szDllPath)
{
    HookBypass::SetGamepid(_ttoi(m_strPid));
   
    if (!HookBypass::BypassCSGO_hook())
    {
        MessageBoxA(0, "Filed to bypass VAC hook!", "Filed to bypass VAC hook!", 0);
        return false;
    }
    
    RemoteThreadInjectdll(szDllPath);
//     HANDLE hGame = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, _ttoi(m_strPid));
//     char dllpath[MAX_PATH];
//     ZeroMemory(dllpath, sizeof(dllpath));
//     TcharToChar(szDllPath, dllpath);
//     LPVOID allocatedMem = VirtualAllocEx(hGame, NULL, sizeof(dllpath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
//     WriteProcessMemory(hGame, allocatedMem, dllpath, sizeof(dllpath), NULL);
//     HANDLE hThread = CreateRemoteThread(hGame, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocatedMem, 0, 0);
//     WaitForSingleObject(hThread, INFINITE);
//     VirtualFreeEx(hGame, allocatedMem, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    HookBypass::RestoreCSGO_hook();
    MessageBoxA(0, "Inject Successfully!", "BobHInjector", 0);

    return true;
}