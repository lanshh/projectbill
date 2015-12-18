
// ProjectBillDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProjectBill.h"
#include "ProjectBillDlg.h"
/*#include "./common/strConvert.h" **/
#include "./ADO/SqlApi.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define WM_UPDATE_MSG       (WM_USER+111)

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
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CProjectBillDlg dialog




CProjectBillDlg::CProjectBillDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectBillDlg::IDD, pParent),SnwtoolDataBase(&m_Configs,&m_LocalLang)
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
    ON_WM_CLOSE()
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

    /*configs **/
    bool bLoadConfig = m_Configs.LoadToolSetting((LPCTSTR)(m_strModulePath + TEXT("config.ini")));
    if(!bLoadConfig) {
        AfxMessageBox(TEXT("Can not load config file!!!"));
        exit(0);
    }
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

void CProjectBillDlg::UpdateTestStatus(int State, int nResult,const TCHAR* strItem,TCHAR* strDesc)
{
    SendMessage(WM_UPDATE_MSG,(WPARAM)strItem,(LPARAM)strDesc);
}

UINT CProjectBillDlg::GenThread(LPVOID lpParam)
{
    TCHAR szItem[FLAG_CNT][256];
    int    nMaxCount;
    RECORD RecordInput;
    vector<RECORD> vItemsInput;
    if(!SnwtoolDataBase.OpenTableForAddNew()){
        AfxMessageBox(TEXT("Faild open table"));
    }
    nMaxCount = 0;
    for(int i = FLAG_SN; i <FLAG_CNT;i ++ ) {
        swprintf(szItem[i - FLAG_SN],256,m_Configs.strItemStart[i - FLAG_SN].c_str());
        nMaxCount = (m_Configs.nItemCount[i - FLAG_SN] > nMaxCount)?m_Configs.nItemCount[i - FLAG_SN]:nMaxCount;
    }
    m_AddProgress.SetRange( 0, 100 );
    m_AddProgress.SetPos(0);
    for(int j = 0; j < nMaxCount ; j++ ) {
        for(int i = FLAG_SN;i < FLAG_CNT; i ++ ) {
            RecordInput.m_strItem[i - FLAG_SN] = szItem[i - FLAG_SN];
        }
        if(!SnwtoolDataBase.AddSingleItem2Database(RecordInput)) {
            AfxMessageBox(TEXT("Faild add item"));
        }
        m_AddProgress.SetPos((int)(((float)j*100)/(float)nMaxCount));
        IntStrIncreaseSkipAlpha (szItem[FLAG_SN]);
        HexStrIncreaseSkipAlpha (szItem[FLAG_WIFIMAC]);
        HexStrIncreaseSkipAlpha (szItem[FLAG_BTMAC]);
        IntStrIncreaseSkipAlpha (szItem[FLAG_IMEI1]);
        IntStrIncreaseSkipAlpha (szItem[FLAG_IMEI2]);
    }
    AfxMessageBox(TEXT("add item finished"));
    return 0;
}

void CProjectBillDlg::OnBnClickedButtonGenbill()
{
    // TODO: Add your control notification handler code here
    Update2Config();
    for(int i = FLAG_SN ; i < FLAG_CNT ; i ++ ) {
        if(!CheckItemValid(m_Configs.strItemStart[i - FLAG_SN].c_str(),i )) {
            AfxMessageBox(m_Configs.strItemStart[i - FLAG_SN].c_str());
            return ;
        }
    }
    m_pGenThread = AfxBeginThread(::GenThread,(LPVOID)this);
}
void CProjectBillDlg::Update2Config()
{
    TCHAR szText[128];
    for(int i = 0 ; i < FLAG_CNT; i++) {
        memset(szText,0,sizeof(szText));
        GetDlgItemText(IDC_EDIT_SN + i,szText,128);
        m_Configs.strItemStart[FLAG_SN + i] = szText;
    }
    for(int i = 0 ; i < FLAG_CNT; i++) {
        memset(szText,0,sizeof(szText));
        GetDlgItemText(IDC_EDIT_SNLENGTH + i,szText,128);
        m_Configs.nItemCount[FLAG_SN + i] = _ttoi(szText);
    }
    for(int i = 0 ; i < FLAG_CNT; i++) {
        memset(szText,0,sizeof(szText));
        GetDlgItemText(IDC_EDIT_SNSPAN + i,szText,128);
        m_Configs.nItemSpan[FLAG_SN + i] = _ttoi(szText);
    }
}
void CProjectBillDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default

    Update2Config();
    if(!m_Configs.SaveToolSetting(TEXT(""))) {
        AfxMessageBox(TEXT("Save config fail!!!"));
        return ;
    }
    
    CDialog::OnClose();
}

