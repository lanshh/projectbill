// DatabaseSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProjectBill.h"
#include "DatabaseSettingDlg.h"
TCHAR *TableFormat = TEXT("CREATE TABLE [dbo].[%s]([M_WOID] [nvarchar](255) NULL,")
                TEXT("[M_HandsetLabelPrined] [nvarchar](255) NULL,")
                TEXT("[M_IMEI1] [nvarchar](255) NULL,")
                TEXT("[M_IMEI2] [nvarchar](255) NULL,")
                TEXT("[M_SN] [nvarchar](255) NULL,")
                TEXT("[M_WifiAddr] [nvarchar](255) NULL,")
                TEXT("[M_BTAddr] [nvarchar](255) NULL,")
                TEXT("[M_IMEI_Flag] [tinyint] NULL,")
                TEXT("[M_IMEI_Datetime] [datetime] NULL,")
                TEXT("[M_Reset_Flag] [tinyint] NULL,")
                TEXT("[M_IMEI_CHECK] [tinyint] NULL,")
                TEXT("[M_Gift_Weight] [float] NULL,")
                TEXT("[M_Gift_Flag] [tinyint] NULL,")
                TEXT("[M_Gift_Datetime] [datetime] NULL,")
                TEXT("[M_SN_IMEI_CHECK] [nvarchar](255) NULL,")
                TEXT("[M_BoxID] [nvarchar](255) NULL,")
                TEXT("[M_Box_Datetime] [datetime] NULL,")
                TEXT("[M_Box_Weight] [nvarchar](255) NULL,")
                TEXT("[M_Box_Gift_CHECK] [nvarchar](255) NULL,")
                TEXT("[M_PalletID] [nvarchar](255) NULL,")
                TEXT("[M_Pallet_Datetime] [datetime] NULL,")
                TEXT("[M_Pallet_Weight] [nvarchar](255) NULL,")
                TEXT("[M_Pallet_Box_CHECK] [nvarchar](255) NULL,")
                TEXT("[M_ChargerSN] [nvarchar](255) NULL,")
                TEXT("[M_BatterySN] [nvarchar](255) NULL,")
                TEXT("[M_BT_Flag] [nvarchar](255) NULL,")
                TEXT("[M_BT_Datetime] [nvarchar](255) NULL,")
                TEXT("[M_FT_Flag] [nvarchar](255) NULL,")
                TEXT("[M_FT_Datetime] [nvarchar](255) NULL,")
                TEXT("[M_AT_Flag] [nvarchar](255) NULL,")
                TEXT("[M_AT_Datetime] [nvarchar](255) NULL,")
                TEXT("[M_IMEI3] [nvarchar](255) NULL,")
                TEXT("[M_IMEI4] [nvarchar](255) NULL,")
                TEXT("[M_SPC1] [nvarchar](255) NULL,")
                TEXT("[M_SPC2] [nvarchar](255) NULL,")
                TEXT("[M_SPC3] [nvarchar](255) NULL,")
                TEXT("[M_SPC4] [nvarchar](255) NULL,")
                TEXT("[M_SPC5] [nvarchar](255) NULL,")
                TEXT("[M_IMEI_Print_Flag] [nvarchar](255) NULL,")
                TEXT("[M_IMEI_Print_Datetime] [nvarchar](255) NULL,")
                TEXT("[M_IMEI_IMEI_Check] [tinyint] NULL,")
                TEXT("[M_IMEI_IMEI_Datetime] [nvarchar](255) NULL,")
                TEXT("[M_SubSN] [nvarchar](255) NULL,")
                TEXT("[M_PreBuildBoxID] [nvarchar](255) NULL,")
                TEXT("[M_PreBuildBoxID_Datetime] [nvarchar](255) NULL,")
                TEXT("[M_PreBuildBoxID_Flag] [nvarchar](255) NULL,")
                TEXT("[M_PrePalletBatchIndn] [nvarchar](255) NULL,")
                TEXT("[M_PalletIDOnYard] [nvarchar](255) NULL,")
                TEXT("[M_SerialNo] [nvarchar](255) NULL,")
                TEXT("[M_SerialNo_Flag] [nvarchar](255) NULL,")
                TEXT("[M_OemLock] [nvarchar](255) NULL,")
                TEXT("[M_Barcode] [nvarchar](255) NULL")
                TEXT(") ON [PRIMARY]");

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
	: CDialog(CDatabaseSettingDlg::IDD, pParent), m_Configs(Configs),m_LocalLang(LocalLang),SnwtoolDataBase(&Configs,&LocalLang)
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
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BT_CANCEL, &CDatabaseSettingDlg::OnBnClickedBtCancel)
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

    m_ListTable.SetCellMargin(1.2);
    CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;
    m_ListTable.SetDefaultRowTrait(pRowTrait);
        
    // Create Columns
	m_ListTable.InsertHiddenLabelColumn();	// Requires one never uses column 0
    /* ListViewForTableInitial(GetDlgItem(hWnd,IDC_LIST_INFO)); **/
    for(int col = 0; col < 3 ; ++col) {
        CGridColumnTrait* pTrait = NULL;
		if (col==0) {
			CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("0"));
            pComboTrait->AddItem((DWORD_PTR)1,TEXT("1"));
            pComboTrait->AddItem((DWORD_PTR)2,TEXT("2"));
            pComboTrait->AddItem((DWORD_PTR)3,TEXT("3"));
			pTrait = pComboTrait;
		}
		if (col==1) {
			pTrait = new CGridColumnTraitEdit;
		} if (col== 2) {
			CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("0"));
            pComboTrait->AddItem((DWORD_PTR)1,TEXT("1"));
            pComboTrait->AddItem((DWORD_PTR)2,TEXT("2"));
            pComboTrait->AddItem((DWORD_PTR)3,TEXT("3"));
			pTrait = pComboTrait;
		}
		if (col==3)	// Year won
		{
			CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
            pComboTrait->AddItem((DWORD_PTR)0,TEXT("0"));
            pComboTrait->AddItem((DWORD_PTR)1,TEXT("1"));
            pComboTrait->AddItem((DWORD_PTR)2,TEXT("2"));
            pComboTrait->AddItem((DWORD_PTR)3,TEXT("3"));
			pTrait = pComboTrait;
		}
		m_ListTable.InsertColumnTrait(col+1, TEXT("12"), LVCFMT_LEFT, 100, col, pTrait);
    }

    int nItem = 0;
    for(size_t rowId = 0; rowId < 2 ; ++rowId)
    {
    
        nItem = m_ListTable.InsertItem(++nItem, TEXT("12"));
        m_ListTable.SetItemData(nItem, rowId);
        for(int col = 0; col < 2 ; ++col)
        {
            /*m_ListCtrl.SetItemText(nItem, nCellCol, strCellText);**/
        }

    }


    CViewConfigSectionWinApp* pColumnProfile = new CViewConfigSectionWinApp(_T("Sample List"));
	pColumnProfile->AddProfile(_T("Default"));
	pColumnProfile->AddProfile(_T("Special"));
	m_ListTable.SetupColumnConfig(pColumnProfile);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

// CDatabaseSettingDlg message handlers

void CDatabaseSettingDlg::OnBnClickedBtOk()
{
    // TODO: Add your control notification handler code here
    if(SaveConfig()) {
        OnOK();
    }
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
BOOL CDatabaseSettingDlg::SaveConfig()
{
    DWORD   dwAddr;
    TCHAR   szItem[64];
    TCHAR   szUserName[64]      = {0};
    TCHAR   szPassword[64]      = {0};
    TCHAR   szDBTable[64]       = {0};
    TCHAR   szDBName[64]        = {0};
    BOOL IsBlank = SendDlgItemMessage(IDC_IPADDRESS_DBSERVER,IPM_ISBLANK,(WPARAM)0,(LPARAM)0);
    if(IsBlank) {
        return FALSE;
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
        return FALSE;
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
    return TRUE;
}
void CDatabaseSettingDlg::OnBnClickedButtonCreate()
{
    // TODO: Add your control notification handler code here
    TCHAR szCmd[5000];
    TCHAR szPrompt[2560];
    if(!SaveConfig()) {
        AfxMessageBox(TEXT("Your Database Config Error!!!"));
        return;
    }
    swprintf(szCmd,sizeof(szCmd)/sizeof(szCmd[0]),TableFormat,m_Configs.strDataBaseTable.c_str());
    if(SnwtoolDataBase.CreateTable(szCmd)){
        swprintf(szPrompt,sizeof(szPrompt)/sizeof(szPrompt[0]),
            TEXT("Create Table(%s) on Database(%s) success!"),
            m_Configs.strDataBaseTable.c_str(),
            m_Configs.strDataBaseName.c_str()
            );
        AfxMessageBox(szPrompt);
    } else {
        swprintf(szPrompt,sizeof(szPrompt)/sizeof(szPrompt[0]),
            TEXT("Create Table(%s) on Database(%s) Fail\r\n %s!!!"),
            m_Configs.strDataBaseTable.c_str(),
            m_Configs.strDataBaseName.c_str(),
            SnwtoolDataBase.GetLastErr().c_str()
            );
        AfxMessageBox(szPrompt,MB_ICONERROR);
    }
}

void CDatabaseSettingDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnClose();
}

void CDatabaseSettingDlg::OnBnClickedBtCancel()
{
    // TODO: Add your control notification handler code here
    CDialog::OnCancel();
}
