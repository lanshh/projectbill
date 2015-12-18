
#include "stdafx.h"
#include "GroupControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupControl

CGroupControl::CGroupControl()
{
  m_bAllowOverlap = FALSE;
  m_bUseTabOrder = FALSE;
  m_bIgnoreControls = FALSE;
  memset(m_IgnoreId,0,sizeof(m_IgnoreId));
}

CGroupControl::~CGroupControl()
{
}

BEGIN_MESSAGE_MAP(CGroupControl, CButton)
	//{{AFX_MSG_MAP(CGroupControl)
	ON_WM_ENABLE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupControl message handlers

BOOL CGroupControl::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID)
{
  dwStyle &= ~(0xF);		// remove styles with conflict with group box
  dwStyle |= BS_GROUPBOX;	// add group box style
  return CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}

void CGroupControl::PreSubclassWindow() 
{
  CButton::PreSubclassWindow();

  // if this 'button' is not a group then
  // it's a bit meaningless to use this control at all
  ASSERT((GetStyle() & 0xF) & BS_GROUPBOX);
}

BOOL CGroupControl::IsInGroup(CWnd *pCtrl, BOOL &bOverlapped) const
{
  BOOL bIsInGroup = FALSE;
  if (pCtrl == NULL)
    return FALSE;
  CRect rcGroup, rc;
  GetWindowRect(&rcGroup);
  pCtrl->GetWindowRect(&rc);
  if (rcGroup.PtInRect(rc.BottomRight()) && rcGroup.PtInRect(rc.TopLeft()))
    bIsInGroup = TRUE;
  if (! bIsInGroup && (rcGroup.PtInRect(rc.BottomRight()) || rcGroup.PtInRect(rc.TopLeft())))
    bOverlapped = bIsInGroup = TRUE;
  for(int i = 0;i < sizeof(m_IgnoreId)/sizeof(m_IgnoreId[0]);i ++ ) {
      if( pCtrl->GetDlgCtrlID() == m_IgnoreId[i] ) {
        bIsInGroup = FALSE;
        break;
    }
  }
  return bIsInGroup;
}
BOOL CGroupControl::AddIgnoreID(INT Id)
{
    int i;
    for(i = 0 ;(m_IgnoreId[i]!=0)&&(i < sizeof(m_IgnoreId)/sizeof(m_IgnoreId[0]));i ++ ) {
        if(m_IgnoreId[i] == Id) {
            return FALSE;
        }
    }
    if((i != sizeof(m_IgnoreId)/sizeof(m_IgnoreId[0]))) m_IgnoreId[i] = Id;
    return TRUE;
}

BOOL CGroupControl::DoGroupControlAction(GROUPCONTROLACTIONFUNC pfnGCAF, LPARAM lParam/*=0*/) const
{
  if (pfnGCAF == NULL)
    return FALSE;

  if (m_bIgnoreControls)
    return FALSE;

  // don't do anything if we're not a groupbox
  if (!((GetStyle() & 0xF) & BS_GROUPBOX))
    return FALSE;

  // go through all controls that lie inside the group and enable/disable
  // those controls also
  CRect rcGroup;
  GetWindowRect(&rcGroup);

  // go through siblings, and see if they lie within the boundary
  // of this group control
  CWnd* pCtrl = NULL;
  if (m_bUseTabOrder)
    pCtrl = GetNextWindow();
  else
    pCtrl = GetParent()->GetWindow(GW_CHILD);
  while (pCtrl != NULL)
  {
    if (pCtrl->GetSafeHwnd() != GetSafeHwnd())
    {
      BOOL bOverlapped = FALSE;
      BOOL bIsInGroup = IsInGroup(pCtrl, bOverlapped);
      if (bIsInGroup && (m_bAllowOverlap || ! bOverlapped))
      {
	if (! pfnGCAF(pCtrl, lParam))
	  return FALSE;
      }
      else if (! bOverlapped && m_bUseTabOrder)  // found out side of group, so ditch out
	break;
    }
    pCtrl = pCtrl->GetNextWindow();
  }
  return TRUE;
}

static BOOL GroupControlActionFunc_Enable(CWnd* pCtrl, LPARAM lParam)
{
  if (pCtrl == NULL)
    return TRUE;
  BOOL bEnable = (BOOL)lParam;
  pCtrl->EnableWindow(bEnable);
  return TRUE;
}

void CGroupControl::OnEnable(BOOL bEnable) 
{
  CButton::OnEnable(bEnable);
  if (! m_bIgnoreControls)
    DoGroupControlAction(GroupControlActionFunc_Enable, bEnable);
}

static BOOL GroupControlActionFunc_Move(CWnd* pCtrl, LPARAM lParam)
{
  if (pCtrl == NULL)
    return TRUE;
  short nDeltaX = LOWORD(lParam);
  short nDeltaY = HIWORD(lParam);
  CRect rc;
  pCtrl->GetWindowRect(&rc);
  rc.OffsetRect(nDeltaX, nDeltaY);
  pCtrl->GetParent()->ScreenToClient(&rc);
  pCtrl->MoveWindow(rc);
  return TRUE;
}

void CGroupControl::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
  CButton::OnWindowPosChanging(lpwndpos);

  if (m_bIgnoreControls)
    return;

  if (lpwndpos->flags & SWP_NOMOVE)
    return;
  
  // see if we've moved x or y, and 
  // move group controls with group box
  CRect rcGroup;
  GetWindowRect(&rcGroup);
  GetParent()->ScreenToClient(&rcGroup);

  int nDeltaX = lpwndpos->x - rcGroup.left;
  int nDeltaY = lpwndpos->y - rcGroup.top;

  if (nDeltaX == 0 && nDeltaY == 0)
    return;

  DoGroupControlAction(GroupControlActionFunc_Move, MAKELPARAM(nDeltaX, nDeltaY));

  Invalidate();
}

static BOOL GroupControlActionFunc_Show(CWnd* pCtrl, LPARAM lParam)
{
  if (pCtrl == NULL)
    return TRUE;
  BOOL bShow = (BOOL)lParam;
  pCtrl->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
  return TRUE;
}

void CGroupControl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
  CButton::OnShowWindow(bShow, nStatus);
  if (! m_bIgnoreControls)
    DoGroupControlAction(GroupControlActionFunc_Show, bShow);
}
