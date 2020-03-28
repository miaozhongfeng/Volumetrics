// ViewControl.cpp : Implementation of CViewControlApp and DLL registration.

#include "stdafx.h"
#include "ViewControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CViewControlApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x87077A94, 0x29BD, 0x47C8, { 0x89, 0x33, 0x4, 0x62, 0x71, 0xB5, 0x1C, 0x85 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CViewControlApp::InitInstance - DLL initialization

BOOL CViewControlApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}



// CViewControlApp::ExitInstance - DLL termination

int CViewControlApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
