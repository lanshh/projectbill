
// ProjectBillDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProjectBill.h"
#include "ProjectBillDlg.h"
/*#include "./common/strConvert.h" **/
#include "./ADO/SqlApi.h"
#include "./label/label.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define WM_UPDATE_MSG       (WM_USER+111)
#define PROCESS_PERCENT     1
UINT GenThread(LPVOID lpParam)
{
    CProjectBillDlg * ProjBill;
    if(lpParam){
        ProjBill = ( CProjectBillDlg *)lpParam;
        ProjBill->GenThread(lpParam);
        ProjBill->m_pGenThread = NULL;
    }
    return 0;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg(CIniSettingBase &Configs,CIniLocalLan &LocalLang);
// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
private:
    CLabel          m_rklink;
    CLabel          m_AppName;
    CIniSettingBase &m_Configs;
    CIniLocalLan    &m_LocalLang;
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg(CIniSettingBase &Configs,CIniLocalLan &LocalLang) : CDialog(CAboutDlg::IDD)
    ,m_Configs(Configs),m_LocalLang(LocalLang)
{
}
void CAboutDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    OnOK();
}
BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_rklink.SetTransparent(TRUE);
	m_rklink.SetFontSize(8);
	m_rklink.SetFontBold(TRUE);
    m_rklink.SetText(TEXT("福州瑞芯微电子有限公司"), 0xFF1111);
    m_rklink.SetLink(TRUE,FALSE);
    m_rklink.SetHyperLink(CString(TEXT("http://www.rock-chips.com/")));
    /*m_rklink.SetLinkCursor( (HCURSOR)IDC_IBEAM); **/
    /*m_rklink.FlashText(TRUE);**/
    m_AppName.SetTransparent(TRUE);
    m_AppName.SetFontSize(8);
    m_AppName.SetFontBold(TRUE);
    m_AppName.SetText((std::wstring(TEXT("ProjectBill"))+ TEXT(APP_VER)).c_str(), 0xFF1111);
    /*GetDlgItem(IDC_STATIC_APPNAME)->SetWindowText((m_LocalLang.GetStr(TEXT("APPNAME")) + TEXT(APP_VERSION)).c_str());**/
    return FALSE;
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_RK, m_rklink);
    DDX_Control(pDX, IDC_STATIC_APPNAME, m_AppName);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CProjectBillDlg dialog




CProjectBillDlg::CProjectBillDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectBillDlg::IDD, pParent),SnwtoolDataBase(&m_Configs,&m_LocalLang),m_pGenThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProjectBillDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_GRPSN, m_GrpSn);
    DDX_Control(pDX, IDC_STATIC_GRPWIFIMAC, m_GrpWifiMac);
    DDX_Control(pDX, IDC_STATIC_GRPBTMAC, m_GrpBtMac);
    DDX_Control(pDX, IDC_STATIC_GRPIMEI1, m_GrpImei1);
    DDX_Control(pDX, IDC_STATIC_GRP_IMEI2, m_GrpImei2);
    DDX_Control(pDX, IDC_PROGRESS1, m_AddProgress);
}

BEGIN_MESSAGE_MAP(CProjectBillDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_COMMAND(ID_SETTING_DATABASE, &CProjectBillDlg::OnSettingDatabase)
    ON_BN_CLICKED(IDC_BUTTON_GENBILL, &CProjectBillDlg::OnBnClickedButtonGenbill)
    ON_MESSAGE(WM_UPDATE_MSG,&CProjectBillDlg::OnHandleUpdateMsg)
    ON_WM_CLOSE()
    ON_UPDATE_COMMAND_UI(ID_SETTING_DATABASE, &CProjectBillDlg::OnUpdateSettingDatabase)
    ON_BN_CLICKED(IDC_CHECK_NEWTABLE, &CProjectBillDlg::OnBnClickedCheckNewtable)
    ON_BN_CLICKED(IDC_CHECK_M_WOID, &CProjectBillDlg::OnBnClickedCheckMWoid)
END_MESSAGE_MAP()


// CProjectBillDlg message handlers

BOOL CProjectBillDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    TCHAR szTemp[28];
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

    TCHAR MyexeFullPath[MAX_PATH] = {0}; 
    m_strModulePath = TEXT("C:\\");
    if(GetModuleFileName(NULL,MyexeFullPath,MAX_PATH)) {
        TCHAR *pos = _tcsrchr(MyexeFullPath, TEXT('\\'));
        if(pos) {
            pos[1] = 0;
            m_strModulePath = MyexeFullPath;
        }
    }
    m_AddProgress.SetRange( 0, 100 );
    /*configs **/
    bool bLoadConfig = m_Configs.LoadToolSetting((LPCTSTR)(m_strModulePath + TEXT("config.ini")));
    if(!bLoadConfig) {
        AfxMessageBox(TEXT("Can not load config file!!!"));
        exit(0);
    }
    SetWindowText((std::wstring(TEXT("ProjectBill")) + TEXT(APP_VER)).c_str());
    for(int i = 0 ; i < FLAG_CNT; i++) {
        SetDlgItemText(IDC_EDIT_SN + i,m_Configs.strItemStart[FLAG_SN + i].c_str());
    }

    for(int i = 0 ; i < FLAG_CNT; i++) {
        swprintf(szTemp,sizeof(szTemp)/sizeof(szTemp[0]),TEXT("%d"),m_Configs.nItemCount[FLAG_SN + i]);
        SetDlgItemText(IDC_EDIT_SNLENGTH + i,szTemp);
    }

    for(int i = 0 ; i < FLAG_CNT; i++) {
        swprintf(szTemp,sizeof(szTemp)/sizeof(szTemp[0]),TEXT("%d"),m_Configs.nItemSpan[FLAG_SN + i]);
        SetDlgItemText(IDC_EDIT_SNSPAN + i,szTemp);
    }

    ((CButton * )GetDlgItem(IDC_CHECK_NEWTABLE))->SetCheck(m_Configs.bCreateNewTable?BST_CHECKED:BST_UNCHECKED);
    SetDlgItemText(IDC_EDIT_NEWTABLE_NAME,m_Configs.strNewTableName.c_str());
    GetDlgItem(IDC_EDIT_NEWTABLE_NAME)->EnableWindow(m_Configs.bCreateNewTable);

    swprintf(szTemp,sizeof(szTemp)/sizeof(szTemp[0]),TEXT("WOID(%s)"),m_Configs.strTableColumnName[COL_WOID].c_str());
    SetDlgItemText(IDC_CHECK_M_WOID,szTemp);
    ((CButton * )GetDlgItem(IDC_CHECK_M_WOID))->SetCheck(m_Configs.bGenWoid?BST_CHECKED:BST_UNCHECKED);
    SetDlgItemText(IDC_EDIT_M_WOID,m_Configs.strWoid.c_str());
    GetDlgItem(IDC_EDIT_M_WOID)->EnableWindow(m_Configs.bGenWoid);



    InitGroupControls();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProjectBillDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout(m_Configs,m_LocalLang);
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

void CProjectBillDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProjectBillDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CProjectBillDlg::OnSettingDatabase()
{
    // TODO: Add your command handler code here
    CDatabaseSettingDlg dlgConfig(m_Configs,m_LocalLang);
    if(IDOK == dlgConfig.DoModal() ) {
        m_Configs.SaveToolSetting(std::wstring(TEXT("")));
        InitGroupControls();

    }
}
void CProjectBillDlg::InitGroupControls()
{
    m_GrpSn.EnableWindow(m_Configs.bItemUsed[0]); 
    m_GrpWifiMac.EnableWindow(m_Configs.bItemUsed[1]);
    m_GrpBtMac.EnableWindow(m_Configs.bItemUsed[2]);
    m_GrpImei1.EnableWindow(m_Configs.bItemUsed[3]);
    m_GrpImei2.EnableWindow(m_Configs.bItemUsed[4]);
}

void CProjectBillDlg::UpdateTestStatus(int Msg, int MsgId,VOID *strDesc)
{
    PostMessage(Msg,(WPARAM)MsgId,(LPARAM)strDesc);
}
#if 0
UINT CProjectBillDlg::GenThread(LPVOID lpParam)
{
    TCHAR   szPrompt[260];
    TCHAR   szItem[FLAG_CNT][256]   = {0};
    int     nItemCount[FLAG_CNT]    = {0};
    int     nSnSpan,nWifiMacSpan,nBtMacSnSpan,nImei1SnSpan,nImei2SnSpan;
    int     nMaxCount               = 0;
    DWORD   dwTicks         = GetTickCount();
    RECORD  RecordInput;
    vector<RECORD> vItemsInput;
    vItemsInput.clear();
    UpdateTestStatus(WM_UPDATE_MSG,PROCESS_PERCENT,0);
    if(!SnwtoolDataBase.OpenTableForAddNew()){
        AfxMessageBox(TEXT("Faild open table"));
        return -1;
    }
    for(int i = 0; i <FLAG_CNT;i ++ ) {
        nItemCount[i]  = m_Configs.strItemStart[i].empty()?0:m_Configs.nItemCount[i];
        if(0 < nItemCount[i] ) {
            swprintf(szItem[i],256,m_Configs.strItemStart[i].c_str());
        }
        nMaxCount   = (nItemCount[i] > nMaxCount)?nItemCount[i]:nMaxCount;
    }
    nSnSpan         = m_Configs.nItemSpan[0];
    nWifiMacSpan    = m_Configs.nItemSpan[1];
    nBtMacSnSpan    = m_Configs.nItemSpan[2];
    nImei1SnSpan    = m_Configs.nItemSpan[3]; 
    nImei2SnSpan    = m_Configs.nItemSpan[4];
    int j;
    for(j = 0; j < nMaxCount ; j++ ) {
        for(int i = 0;i < FLAG_CNT; i ++ ) {
            if(j < nItemCount[i]) {
                RecordInput.m_strItem[i] = szItem[i];
            } else if(j == nItemCount[i]){
                RecordInput.m_strItem[i].clear();
            }
        }
        vItemsInput.push_back(RecordInput);
        if(25 == vItemsInput.size()) {
            if(!SnwtoolDataBase.AddItems2Database(vItemsInput)) {
                swprintf(szPrompt,dim(szPrompt),TEXT("Faild add item %d"),j);
                AfxMessageBox(szPrompt);
                return -1;
            }
            vItemsInput.clear();
            UpdateTestStatus(WM_UPDATE_MSG,PROCESS_PERCENT,(void *)((j*100)/nMaxCount));
        }
        if(j < nItemCount[FLAG_SN])     IntStrIncreaseSkipAlpha (szItem[FLAG_SN],nSnSpan);
        if(j < nItemCount[FLAG_WIFIMAC])HexStrIncreaseSkipAlpha (szItem[FLAG_WIFIMAC],nWifiMacSpan);
        if(j < nItemCount[FLAG_BTMAC])  HexStrIncreaseSkipAlpha (szItem[FLAG_BTMAC],nBtMacSnSpan);
        if(j < nItemCount[FLAG_IMEI1])  ImeiInc(szItem[FLAG_IMEI1],nImei1SnSpan);
        if(j < nItemCount[FLAG_IMEI2])  ImeiInc(szItem[FLAG_IMEI2],nImei2SnSpan);
    }
    if(0 != vItemsInput.size()) {
        if(!SnwtoolDataBase.AddItems2Database(vItemsInput)) {
            swprintf(szPrompt,dim(szPrompt),TEXT("Faild add item %d"),j);
            AfxMessageBox(szPrompt);
            return -1;
        }
        vItemsInput.clear();
    }
    UpdateTestStatus(WM_UPDATE_MSG,PROCESS_PERCENT,(void *)100);
    swprintf(szPrompt,dim(szPrompt),TEXT("Add %d items finished,ticks=%u"),j,GetTickCount() - dwTicks);
    AfxMessageBox(szPrompt);
    return 0;
}
#else 
UINT CProjectBillDlg::GenThread(LPVOID lpParam)
{
    TCHAR   szCmd[5000];
    TCHAR   szPrompt[260];
    TCHAR   szItem[FLAG_CNT][256]   = {0};
    int     nItemCount[FLAG_CNT]    = {0};
    int     nSnSpan,nWifiMacSpan,nBtMacSnSpan,nImei1SnSpan,nImei2SnSpan;
    int     nMaxCount               = 0;
    DWORD   dwTicks         = GetTickCount();
    RECORD  RecordInput;
    UpdateTestStatus(WM_UPDATE_MSG,PROCESS_PERCENT,0);

    if(m_Configs.bCreateNewTable) {
        if(m_Configs.strNewTableName.empty()) {
            AfxMessageBox(TEXT("Table name empty\r\nFaild create table!!!"),MB_ICONERROR);
            return -1;
        }
        swprintf(szCmd,sizeof(szCmd)/sizeof(szCmd[0]),TableFormat,m_Configs.strNewTableName.c_str());
        if(SnwtoolDataBase.CreateTable(szCmd)){
            /*
            swprintf(szPrompt,sizeof(szPrompt)/sizeof(szPrompt[0]),
                TEXT("Create Table(%s) on Database(%s) success!"),
                m_Configs.strDataBaseTable.c_str(),
                m_Configs.strDataBaseName.c_str()
                );
            AfxMessageBox(szPrompt);
            **/
        } else {
            swprintf(szPrompt,sizeof(szPrompt)/sizeof(szPrompt[0]),
                TEXT("Create Table(%s) on Database(%s) Fail\r\n %s!!!"),
                m_Configs.strNewTableName.c_str(),
                m_Configs.strDataBaseName.c_str(),
                SnwtoolDataBase.GetLastErr().c_str()
                );
            AfxMessageBox(szPrompt,MB_ICONERROR);
            return -1;
        }
        m_Configs.strDataBaseTable = m_Configs.strNewTableName;
    }

    if(!SnwtoolDataBase.OpenTableForAddNew()){
        AfxMessageBox(TEXT("Faild open table"));
        return -1;
    }
    for(int i = 0; i <FLAG_CNT;i ++ ) {
        nItemCount[i]  = m_Configs.strItemStart[i].empty()?0:m_Configs.nItemCount[i];
        if(0 < nItemCount[i] ) {
            swprintf(szItem[i],256,m_Configs.strItemStart[i].c_str());
        }
        nMaxCount   = (nItemCount[i] > nMaxCount)?nItemCount[i]:nMaxCount;
    }
    nSnSpan         = m_Configs.nItemSpan[0];
    nWifiMacSpan    = m_Configs.nItemSpan[1];
    nBtMacSnSpan    = m_Configs.nItemSpan[2];
    nImei1SnSpan    = m_Configs.nItemSpan[3]; 
    nImei2SnSpan    = m_Configs.nItemSpan[4];
    int j;
    if(m_Configs.bGenWoid&&(!m_Configs.strWoid.empty())&&(!m_Configs.strTableColumnName[COL_WOID].empty())) {
        RecordInput.m_strItem[FLAG_CNT + COL_WOID] = m_Configs.strWoid;
    }
    for(j = 0; j < nMaxCount ; j++ ) {
        for(int i = 0;i < FLAG_CNT; i ++ ) {
            if(j < nItemCount[i]) {
                RecordInput.m_strItem[i] = szItem[i];
            } else if(j == nItemCount[i]){
                RecordInput.m_strItem[i].clear();
            }
        }

        if(!SnwtoolDataBase.AddSingleItem2Database(RecordInput)) {
            swprintf(szPrompt,dim(szPrompt),TEXT("Faild add item %d"),j);
            AfxMessageBox(szPrompt);
            return -1;
        }

        if(j < nItemCount[FLAG_SN])     IntStrIncreaseSkipAlpha (szItem[FLAG_SN],nSnSpan);
        if(j < nItemCount[FLAG_WIFIMAC])HexStrIncreaseSkipAlpha (szItem[FLAG_WIFIMAC],nWifiMacSpan);
        if(j < nItemCount[FLAG_BTMAC])  HexStrIncreaseSkipAlpha (szItem[FLAG_BTMAC],nBtMacSnSpan);
        if(j < nItemCount[FLAG_IMEI1])  ImeiInc(szItem[FLAG_IMEI1],nImei1SnSpan);
        if(j < nItemCount[FLAG_IMEI2])  ImeiInc(szItem[FLAG_IMEI2],nImei2SnSpan);
        if(0 == (j%50)) UpdateTestStatus(WM_UPDATE_MSG,PROCESS_PERCENT,(void *)((j*100)/nMaxCount));
    }
    UpdateTestStatus(WM_UPDATE_MSG,PROCESS_PERCENT,(void *)100);
    swprintf(szPrompt,dim(szPrompt),TEXT("Add %d items finished,ticks=%u"),j,GetTickCount() - dwTicks);
    AfxMessageBox(szPrompt);
    return 0;
}


#endif
void CProjectBillDlg::OnBnClickedButtonGenbill()
{
    // TODO: Add your control notification handler code here
    TCHAR szPrompt[128];
    if(m_pGenThread) {
        AfxMessageBox(TEXT("Application is busy!!!"));
        return ;
    }
    Update2Config();
    for(int i = FLAG_SN ; i < FLAG_CNT ; i ++ ) {
        if(!CheckItemValid(m_Configs.strItemStart[i - FLAG_SN].c_str(),i )) {
            swprintf(szPrompt,128,TEXT("Item %s's start invalid!!!"),m_Configs.strItemName[i].c_str());
            AfxMessageBox(szPrompt);
            return ;
        }
    }
    for(int i = 0; i < FLAG_CNT; i++ ) {
        int span = m_Configs.nItemSpan[i];
        if( 0 > span ||10 < span) {
            swprintf(szPrompt,128,TEXT("Item %s's span invalid!!!"),m_Configs.strItemName[i].c_str());
            AfxMessageBox(szPrompt);
            return ;
        }
    }
    if(m_Configs.bGenWoid) {
        if(m_Configs.strWoid.empty()) {
            swprintf(szPrompt,128,TEXT("Item %s is null!!!\r\nClick yes continue"),
                m_Configs.strTableColumnName[COL_WOID].c_str());
            if(IDYES != AfxMessageBox(szPrompt,MB_YESNO)){
                return ;
            }
        }
    }
    m_pGenThread = AfxBeginThread(::GenThread,(LPVOID)this);
}
BOOL CProjectBillDlg::Update2Config()
{
    TCHAR szText[128];
    /*item start**/
    for(int i = 0 ; i < FLAG_CNT; i++) {
        memset(szText,0,sizeof(szText));
        GetDlgItemText(IDC_EDIT_SN + i,szText,128);
        m_Configs.strItemStart[i] = szText;
    }
    /*item length**/
    for(int i = 0 ; i < FLAG_CNT; i++) {
        memset(szText,0,sizeof(szText));
        GetDlgItemText(IDC_EDIT_SNLENGTH + i,szText,128);
        m_Configs.nItemCount[i] = _ttoi(szText);
    }
    /*item span**/
    for(int i = 0 ; i < FLAG_CNT; i++) {
        memset(szText,0,sizeof(szText));
        GetDlgItemText(IDC_EDIT_SNSPAN + i,szText,128);
        m_Configs.nItemSpan[i] = _ttoi(szText);
    }

    m_Configs.bCreateNewTable = ((CButton * )GetDlgItem(IDC_CHECK_NEWTABLE))->GetCheck()==BST_CHECKED;
    memset(szText,0,sizeof(szText));
    GetDlgItemText(IDC_EDIT_NEWTABLE_NAME,szText,128);
    m_Configs.strNewTableName = szText;

    m_Configs.bGenWoid = ((CButton * )GetDlgItem(IDC_CHECK_M_WOID))->GetCheck()==BST_CHECKED;
    memset(szText,0,sizeof(szText));
    GetDlgItemText(IDC_EDIT_M_WOID,szText,128);
    m_Configs.strWoid = szText;



    return TRUE;
}
void CProjectBillDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    if(!IsConfigurable()) {
        AfxMessageBox(TEXT("Tool is busy,please waiting!!!"));
        return ;
    }
    Update2Config();
    if(!m_Configs.SaveToolSetting(TEXT(""))) {
        AfxMessageBox(TEXT("Save config fail!!!"));
        return ;
    }
    CDialog::OnClose();
}
LRESULT CProjectBillDlg::OnHandleUpdateMsg(WPARAM wParam,LPARAM lParam)
{
    TCHAR szPrompt[128];
    switch(wParam) {
    case PROCESS_PERCENT:
        m_AddProgress.SetPos((int)lParam);
        swprintf(szPrompt,128,TEXT("%d%%"),lParam);
        SetDlgItemText(IDC_STATIC_PERCENT,szPrompt);
        break;
    }

    return 0;
}
BOOL CProjectBillDlg::IsConfigurable()
{
    return NULL == m_pGenThread;
}
void CProjectBillDlg::OnUpdateSettingDatabase(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable(IsConfigurable());
}

void CProjectBillDlg::OnBnClickedCheckNewtable()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_EDIT_NEWTABLE_NAME)->EnableWindow(
        ((CButton * )GetDlgItem(IDC_CHECK_NEWTABLE))->GetCheck()==BST_CHECKED);
}

void CProjectBillDlg::OnBnClickedCheckMWoid()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_EDIT_M_WOID)->EnableWindow(
        ((CButton * )GetDlgItem(IDC_CHECK_M_WOID))->GetCheck()==BST_CHECKED);
}
