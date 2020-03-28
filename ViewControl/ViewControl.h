#pragma once

// ViewControl.h : main header file for ViewControl.DLL

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif
#include "resource.h"       // main symbols


// CViewControlApp : See ViewControl.cpp for implementation.

class CViewControlApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

