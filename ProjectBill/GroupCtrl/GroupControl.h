

#if !defined(AFX_GROUPCONTROL_H__6AEE0F71_9D52_11D6_860D_0000B48746CF__INCLUDED_)
#define AFX_GROUPCONTROL_H__6AEE0F71_9D52_11D6_860D_0000B48746CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef BOOL (*GROUPCONTROLACTIONFUNC)(CWnd*, LPARAM);

/////////////////////////////////////////////////////////////////////////////
// CGroupControl window

class CGroupControl : public CButton
{
// Construction
public:
	CGroupControl();

// Attributes
public:

// Operations
public:
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupControl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsInGroup(CWnd* pCtrl, BOOL& bOverlapped) const;
	void SetUseTabOrder(BOOL bUseTabOrder = TRUE) { m_bUseTabOrder = bUseTabOrder; }
	void SetAllowOverlap(BOOL bAllowOverlap = TRUE) { m_bAllowOverlap = bAllowOverlap; }
	BOOL GetUseTabOrder() const { return m_bUseTabOrder; }
	BOOL GetAllowOverlap() const { return m_bAllowOverlap; }
	BOOL DoGroupControlAction(GROUPCONTROLACTIONFUNC pfnGCAF, LPARAM lParam = 0) const;
	BOOL ControlsIgnored() const { return m_bIgnoreControls; }
	void IgnoreControls(BOOL bIgnore = TRUE) { m_bIgnoreControls = bIgnore; }
	virtual ~CGroupControl();
    BOOL    AddIgnoreID(INT Id);

	// Generated message map functions
protected:
	BOOL m_bAllowOverlap;
	BOOL m_bUseTabOrder;
    INT  m_IgnoreId[6];

	BOOL m_bIgnoreControls;

	//{{AFX_MSG(CGroupControl)
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPCONTROL_H__6AEE0F71_9D52_11D6_860D_0000B48746CF__INCLUDED_)
