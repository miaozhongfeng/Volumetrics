// Antlers.h : main header file for the Antlers application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CAntlersApp:
// See Antlers.cpp for the implementation of this class
//

class CAntlersApp : public CWinApp
{
public:
	CAntlersApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAntlersApp theApp;