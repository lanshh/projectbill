
// ProjectBillDlg.h : header file
//

#pragma once
#include "./settings/SettingBase.h"
#include "DatabaseSettingDlg.h"
#include "./GroupCtrl/GroupControl.h"
#include "./ADO/SqlApi.h"
#include "afxwin.h"
#include "afxcmn.h"
// CProjectBillDlg dialog
class CProjectBillDlg : public CDialog
{
    friend UINT GenThread(LPVOID lpParam);
// Construction
public:
	CProjectBillDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROJECTBILL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
    CString         m_strModulePath;
    CIniSettingBase m_Configs;
    CIniLocalLan    m_LocalLang;
    CSNWToolSqlDB   SnwtoolDataBase;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSettingDatabase();
    void    InitGroupControls();
    BOOL    Update2Config();
    UINT    GenThread(LPVOID lpParam);
    void    UpdateTestStatus(int Msg, int MsgId,VOID *strDesc);
    afx_msg LRESULT OnHandleUpdateMsg(WPARAM wParam,LPARAM lParam);
    CWinThread  *m_pGenThread;
    BOOL    m_bUserAbort;
    CGroupControl m_GrpSn;
    CGroupControl m_GrpWifiMac;
    CGroupControl m_GrpBtMac;
    CGroupControl m_GrpImei1;
    CGroupControl m_GrpImei2;
    afx_msg void OnBnClickedButtonGenbill();
    afx_msg void OnClose();
    CProgressCtrl m_AddProgress;
    afx_msg void OnUpdateSettingDatabase(CCmdUI *pCmdUI);
    BOOL    IsConfigurable();
    afx_msg void OnBnClickedCheckNewtable();
};
