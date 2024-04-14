
// CodeInjectDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "DragPic.h"
#include "dm.tlh"
//#include "XASM.h"

// CCodeInjectDlg 对话框
class CCodeInjectDlg : public CDialogEx
{
// 构造
public:
	CCodeInjectDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CODEINJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CEdit m_editWindowTitle;
	CEdit m_editPid;
	CEdit m_editAsm;
	CDragPic m_Pic;
	CEdit m_editLog;
    Idmsoft *m_pdm;
public:
	afx_msg void OnBnClickedBtninjectcode();
	afx_msg void OnBnClickedBtninjectdll();
	void DebugLog(CString str);
	void DebugErr(CString str);
	BOOL AdjustPr();
	void InjectAsm(DWORD pid);
	void InjectDll(DWORD pid);
	void TcharToChar(const TCHAR * tchar, char * _char);
    afx_msg void OnBnClickedBtnviewprocess();
};
