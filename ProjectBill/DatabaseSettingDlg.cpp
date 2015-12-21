// DatabaseSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProjectBill.h"
#include "DatabaseSettingDlg.h"

struct 
{
    TCHAR * str;
    int     type;
} Flag2Str[] = 
{
    {TEXT("SN")      ,FLAG_SN},
    {TEXT("WifiMac") ,FLAG_WIFIMAC},
    {TEXT("BtMac")   ,FLAG_BTMAC},
    {TEXT("IMEI1")   ,FLAG_IMEI1},
    {TEXT("IMEI2")   ,FLAG_IMEI2},
    {TEXT("AUTO")    ,FLAG_AUTO},
};

// CDatabaseSettingDlg dialog

IMPLEMENT_DYNAMIC(CDatabaseSettingDlg, CDialog)

CDatabaseSettingDlg::CDatabaseSettingDlg( CIniSettingBase &Configs,CIniLocalLan &LocalLang,CWnd* pParent /*=NULL*/)
	: CDialog(CDatabaseSettingDlg::IDD, pParent), m_Configs(Configs),m_LocalLang(LocalLang)
{

}

CDatabaseSettingDlg::~CDatabaseSettingDlg()
{
}

void CDatabaseSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_INFO, m_ListTable);
}


BEGIN_MESSAGE_MAP(CDatabaseSettingDlg, CDialog)
    ON_BN_CLICKED(IDC_BT_OK, &CDatabaseSettingDlg::OnBnClickedBtOk)
    ON_BN_CLICKED(IDC_CHECK_SN, &CDatabaseSettingDlg::OnBnClickedCheckSn)
    ON_BN_CLICKED(IDC_CHECK_WIFI, &CDatabaseSettingDlg::OnBnClickedCheckWifi)
    ON_BN_CLICKED(IDC_CHECK_BT, &CDatabaseSettingDlg::OnBnClickedCheckBt)
    ON_BN_CLICKED(IDC_CHECK_IMEI1, &CDatabaseSettingDlg::OnBnClickedCheckImei1)
    ON_BN_CLICKED(IDC_CHECK_IMEI2, &CDatabaseSettingDlg::OnBnClickedCheckImei2)
    ON_BN_CLICKED(IDC_BUTTON_CREATE, &CDatabaseSettingDlg::OnBnClickedButtonCreate)
END_MESSAGE_MAP()

BOOL CDatabaseSettingDlg::OnInitDialog()
{
	// TODO: Add extra initialization here
    CDialog::OnInitDialog();
    DWORD dwAddr = m_Configs.GetIpv4();
    SendDlgItemMessage(IDC_IPADDRESS_DBSERVER,IPM_SETADDRESS,(WPARAM)0,(LPARAM)dwAddr);
    SetDlgItemText(IDC_EDIT_USER        ,m_Configs.strUserName.c_str());
    SetDlgItemText(IDC_EDIT_PWD         ,m_Configs.strPassword.c_str());
    SetDlgItemText(IDC_EDIT_DATABASENAME,m_Configs.strDataBaseName.c_str());
    SetDlgItemText(IDC_EDIT_TABLE       ,m_Configs.strDataBaseTable.c_str());

    for(int i = 0;i < FLAG_CNT ; i++ ) {
        SetDlgItemText(IDC_EDIT_TABLE_SN + i,m_Configs.strItemName[FLAG_SN + i].c_str());
    }

    CComboBox *hComSupportDatabase = (CComboBox *)GetDlgItem(IDC_COMBO_DBTYPE);
    for(int i = 0; i < m_Configs.nSupportDatabse; i++) {
        hComSupportDatabase->AddString(m_Configs.strSupportDatabse[i].c_str());
    }
    hComSupportDatabase->SetCurSel(0);

    /*primary key **/
    CComboBox *hComPrimaryKey = (CComboBox *)GetDlgItem(IDC_COMBO_INDEX);
    INT     CurSel          = -1;
    for(int i = 0 ; i < sizeof(Flag2Str)/sizeof(Flag2Str[0]); i ++ ) {
        hComPrimaryKey->AddString(Flag2Str[i].str);
        if(m_Configs.nPrimaryKey == Flag2Str[i].type) {
            CurSel = i;
        }
    }
    if(-1 != CurSel ) {
         hComPrimaryKey->SetCurSel(CurSel);
    }
    /***/
    
    for(int i = 0;i < FLAG_CNT ; i++ ) {
        bool bItemUsed = m_Configs.bItemUsed[i];
        ((CButton * )GetDlgItem(IDC_CHECK_SN + i)) ->SetCheck(bItemUsed ?BST_CHECKED:BST_UNCHECKED);
        GetDlgItem(IDC_EDIT_TABLE_SN + i)->EnableWindow(bItemUsed);

    }
    // Create Columns
	m_ListTable.InsertHiddenLabelColumn();	// Requires one never uses column 0
    /* ListViewForTableInitial(GetDlgItem(hWnd,IDC_LIST_INFO)); **/
    for(int col = 0; col < 3 ; ++col) {
        CGridColumnTrait* pTrait = NULL;
		if (col==0) {
			CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("0"));
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("1"));
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("2"));
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("3"));
			pTrait = pComboTrait;
		}
		if (col==1) {
			pTrait = new CGridColumnTraitEdit;
		} if (col== 2) {
			CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("0"));
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("1"));
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("2"));
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("3"));
			pTrait = pComboTrait;
		}
		if (col==3)	// Year won
		{
			CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("0"));
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("1"));
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("2"));
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("3"));
			pTrait = pComboTrait;
		}
		m_ListTable.InsertColumnTrait(col+1, TEXT(""), LVCFMT_LEFT, 100, col, pTrait);
    }

    int nItem = 0;
    for(size_t rowId = 0; rowId < 2 ; ++rowId)
    {
    
        nItem = m_ListTable.InsertItem(++nItem, TEXT("12"));
        for(int col = 0; col < 2 ; ++col)
        {

        }

    }
    return TRUE;  // return TRUE  unless you set the focus to a control
}

// CDatabaseSettingDlg message handlers

void CDatabaseSettingDlg::OnBnClickedBtOk()
{
    // TODO: Add your control notification handler code here
    DWORD   dwAddr;
    TCHAR   szItem[64];
    TCHAR   szUserName[64]      = {0};
    TCHAR   szPassword[64]      = {0};
    TCHAR   szDBTable[64]       = {0};
    TCHAR   szDBName[64]        = {0};

    BOOL IsBlank = SendDlgItemMessage(IDC_IPADDRESS_DBSERVER,IPM_ISBLANK,(WPARAM)0,(LPARAM)0);
    if(IsBlank) {
        return;
    }
    GetDlgItemText(IDC_EDIT_USER         ,szUserName,64);
    GetDlgItemText(IDC_EDIT_PWD          ,szPassword,64);
    GetDlgItemText(IDC_EDIT_DATABASENAME ,szDBName  ,64);
    GetDlgItemText(IDC_EDIT_TABLE        ,szDBTable ,64);

    BOOL bHaveItem = FALSE;
    for(int i = 0;i < FLAG_CNT ; i++ ) {
        if(((CButton * )GetDlgItem(IDC_CHECK_SN + i))->GetCheck()==BST_CHECKED ) {
            bHaveItem = TRUE;
            break;
        }
    }
    if(!bHaveItem) {
        AfxMessageBox(TEXT("your hava select no item"));
        return;
        m_Configs.bConnectDataBase = false;
    }
    /*
    check name and password valid 
    **/
    
    for(int i = 0;i < FLAG_CNT ; i++ ) {
        memset(szItem,TEXT('\0'),sizeof(szItem));
        GetDlgItemText(IDC_EDIT_TABLE_SN + i  ,szItem,64);
        m_Configs.strItemName[FLAG_SN + i] = szItem;
    }
    for(int i = 0;i < FLAG_CNT ; i++ ) {
        m_Configs.bItemUsed[i] = ((CButton * )GetDlgItem(IDC_CHECK_SN + i))->GetCheck()==BST_CHECKED;
    }

     CComboBox *hComPrimaryKey = (CComboBox *)GetDlgItem(IDC_COMBO_INDEX);
     DWORD sel =hComPrimaryKey->GetCurSel();
    if((sel >= 0 )&&(sel < sizeof(Flag2Str)/sizeof(Flag2Str[0]))) {
        m_Configs.nPrimaryKey = Flag2Str[sel].type;
    } else {
        m_Configs.nPrimaryKey = FLAG_AUTO;
    }
    SendDlgItemMessage(IDC_IPADDRESS_DBSERVER,IPM_GETADDRESS,(WPARAM)0,(LPARAM)&dwAddr);
    m_Configs.SetIpv4(dwAddr);
    m_Configs.strUserName       = szUserName;
    m_Configs.strPassword       = szPassword;
    m_Configs.strDataBaseName   = szDBName;
    m_Configs.strDataBaseTable  = szDBTable;
    OnOK();
}
/*
LRESULT CALLBACK DlgDataBaseChkProc (HWND hWnd,  WPARAM wParam, LPARAM lParam)
{
    int CheckId = LOWORD (wParam);
    if((IDC_CHECK_SN <= CheckId)&&((IDC_CHECK_SN + 4)>= CheckId)) {
        EnableWindow(GetDlgItem(hWnd,IDC_EDIT_SN  + CheckId - IDC_CHECK_SN) ,
        Button_GetCheck(GetDlgItem(hWnd,CheckId))==BST_CHECKED?TRUE:FALSE
        );
    }
    return FALSE;
}
**/

void CDatabaseSettingDlg::OnBnClickedCheckSn()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_EDIT_TABLE_SN)->EnableWindow(
        ((CButton * )GetDlgItem(IDC_CHECK_SN ))->GetCheck()==BST_CHECKED);

}

void CDatabaseSettingDlg::OnBnClickedCheckWifi()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_EDIT_TABLE_WIFI)->EnableWindow(
        ((CButton * )GetDlgItem(IDC_CHECK_WIFI ))->GetCheck()==BST_CHECKED);
}

void CDatabaseSettingDlg::OnBnClickedCheckBt()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_EDIT_TABLE_BT)->EnableWindow(
        ((CButton * )GetDlgItem(IDC_CHECK_BT ))->GetCheck()==BST_CHECKED);
}

void CDatabaseSettingDlg::OnBnClickedCheckImei1()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_EDIT_TABLE_IMEI1)->EnableWindow(
        ((CButton * )GetDlgItem(IDC_CHECK_IMEI1))->GetCheck()==BST_CHECKED);
}

void CDatabaseSettingDlg::OnBnClickedCheckImei2()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_EDIT_TABLE_IMEI2)->EnableWindow(
        ((CButton * )GetDlgItem(IDC_CHECK_IMEI2 ))->GetCheck()==BST_CHECKED);
}

void CDatabaseSettingDlg::OnBnClickedButtonCreate()
{
    // TODO: Add your control notification handler code here
}
