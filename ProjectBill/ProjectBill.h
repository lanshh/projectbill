
// ProjectBill.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CProjectBillApp:
// See ProjectBill.cpp for the implementation of this class
//

class CProjectBillApp : public CWinAppEx
{
public:
	CProjectBillApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CProjectBillApp theApp;