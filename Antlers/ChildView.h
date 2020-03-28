// ChildView.h  : Declaration of ActiveX Control wrapper class(es) created by Microsoft Visual C++

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CChildView

class CChildView : public CWnd
{
protected:
	DECLARE_DYNCREATE(CChildView)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x3D224CEB, 0x2A80, 0x4B47, { 0x8E, 0xC5, 0x94, 0x69, 0x65, 0x83, 0x73, 0x3A } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// Attributes
public:

// Operations
public:

	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	SHORT toggleOrbitMode()
	{
		short res;
		InvokeHelper(0x64, DISPATCH_METHOD, VT_I2, (void*)&res, NULL);
		return res;
	}
	SHORT toggleZoomMode()
	{
		short res;
		InvokeHelper(0x65, DISPATCH_METHOD, VT_I2, (void*)&res, NULL);
		return res;
	}
	SHORT togglePanMode()
	{
		short res;
		InvokeHelper(0x66, DISPATCH_METHOD, VT_I2, (void*)&res, NULL);
		return res;
	}
	void resetMode()
	{
		InvokeHelper(0x67, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL loadAntler(LPCTSTR filename)
	{
		BOOL res;
		static BYTE parms[] = VTS_BSTR;
		InvokeHelper(0x68, DISPATCH_METHOD, VT_BOOL, (void*)&res, parms, filename);
		return res;
	}
	void setProgressBarHandle(ULONG handle)
	{
		static BYTE parms[] = VTS_UI4;
		InvokeHelper(0x6A, DISPATCH_METHOD, VT_EMPTY, NULL, parms, handle);
	}
	void normalize()
	{
		InvokeHelper(0x6B, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL enterAntlerPlane(ULONG num)
	{
		BOOL res;
		static BYTE parms[] = VTS_UI4;
		InvokeHelper(0x6C, DISPATCH_METHOD, VT_BOOL, (void*)&res, parms, num);
		return res;
	}
	void exportToPDF(LPCTSTR filename, ULONG type)
	{
		static BYTE parms[] =  VTS_BSTR VTS_UI4;
		InvokeHelper(0x6D, DISPATCH_METHOD, VT_EMPTY, NULL, parms, filename, type);
	}
	void viewScoreSheet(ULONG type)
	{
		static BYTE parms[] = VTS_UI4;
		InvokeHelper(0x6E, DISPATCH_METHOD, VT_EMPTY, NULL, parms, type);
	}
	SHORT getNavigationMode()
	{
		short res;
		InvokeHelper(0x6F, DISPATCH_METHOD, VT_I2, (void*)&res, NULL);
		return res;
	}
    void findCriticalPoints()
    {
        InvokeHelper(0x70, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
    }
	SHORT showVolume(ULONG num)
	{
		short res;
		static BYTE parms[] = VTS_UI4;
		InvokeHelper(0x71, DISPATCH_METHOD, VT_I2, (void*)&res, parms, num);
		return res;
	}
	void toggleShowCenterLine()
	{
		InvokeHelper(0x72, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void toggleShowAnnotations()
	{
		InvokeHelper(0x73, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	SHORT planeDetectionMode()
	{
		// return plane number or -1 if no detection
		short res;
		InvokeHelper(0x74, DISPATCH_METHOD, VT_I2, (void*)&res, NULL);
		return res;
	}
	void toggleShowAxis()
	{
		InvokeHelper(0x75, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
    BOOL enterSelectMode()
    {
        BOOL res;
        InvokeHelper(0x76, DISPATCH_METHOD, VT_BOOL, (void*)&res, NULL);
        return res;
    }
    SHORT getToolMode()
    {
        // return plane number or -1 if no detection
        short res;
        InvokeHelper(0x77, DISPATCH_METHOD, VT_I2, (void*)&res, NULL);
        return res;
    }
    SHORT getVisibleElements()
    {
        short res;
        InvokeHelper(0x78, DISPATCH_METHOD, VT_I2, (void*)&res, NULL);
        return res;
    }
    BOOL enterAntlerAutoPlanes()
    {
        BOOL res;
        InvokeHelper(0x79, DISPATCH_METHOD, VT_BOOL, (void*)&res, NULL);
        return res;
    }
    BOOL enterLabelingMode()
    {
        BOOL res;
        InvokeHelper(0x7A, DISPATCH_METHOD, VT_BOOL, (void*)&res, NULL);
        return res;
    }
    void setNormalized()
    {
        InvokeHelper(0x7B, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
    }
    void normalizeRestore()
    {
        InvokeHelper(0x7C, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
    }
};
