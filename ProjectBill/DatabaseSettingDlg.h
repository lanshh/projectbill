#pragma once

#include "./settings/SettingBase.h"
#include ".\CGridListCtrlEx\CGridListCtrlGroups.h"
#include ".\CGridListCtrlEx\CGridColumnTraitDateTime.h"
#include ".\CGridListCtrlEx\CGridColumnTraitEdit.h"
#include ".\CGridListCtrlEx\CGridColumnTraitCombo.h"
#include ".\CGridListCtrlEx\CGridColumnTraitHyperLink.h"
#include ".\CGridListCtrlEx\CGridRowTraitXP.h"
#include ".\CGridListCtrlEx\ViewConfigSection.h"
#include "afxcmn.h"
// CDatabaseSettingDlg dialog

class CDatabaseSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CDatabaseSettingDlg)

public:
	CDatabaseSettingDlg(CIniSettingBase &Configs,CIniLocalLan &LocalLang,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDatabaseSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_DATABASE };
private:
    CIniSettingBase &m_Configs;
    CIniLocalLan    &m_LocalLang;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtOk();
    afx_msg void OnBnClickedCheckSn();
    afx_msg void OnBnClickedCheckWifi();
    afx_msg void OnBnClickedCheckBt();
    afx_msg void OnBnClickedCheckImei1();
    afx_msg void OnBnClickedCheckImei2();
    afx_msg void OnBnClickedButtonCreate();
    CGridListCtrlGroups m_ListTable;
};
