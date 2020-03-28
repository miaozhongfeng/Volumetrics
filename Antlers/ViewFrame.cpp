// ViewFrame.cpp : implementation file
//

#include "stdafx.h"
#include "Antlers.h"
#include "ViewFrame.h"

// CViewFrame

IMPLEMENT_DYNCREATE(CViewFrame, CView)

CViewFrame::CViewFrame()
{
	m_showVolume = -1;
	m_isMeshLoaded = false;
	m_identifyingPlane = false;
	m_wndStatusBar = NULL;
}

CViewFrame::~CViewFrame()
{
}

BEGIN_MESSAGE_MAP(CViewFrame, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_VIEW_ORBIT, &CViewFrame::OnViewOrbit)
	ON_COMMAND(ID_VIEW_ZOOM, &CViewFrame::OnViewZoom)
	ON_COMMAND(ID_VIEW_PAN, &CViewFrame::OnViewPan)
	//ON_COMMAND(ID_VIEW_RESET, &CViewFrame::OnViewReset)
	ON_COMMAND(ID_VIEW_NORMALIZE, &CViewFrame::OnViewNormalize)
    ON_COMMAND(ID_VIEW_NORMALIZE_RESTORE, &CViewFrame::OnViewNormalizeRestore)
    ON_COMMAND(ID_VIEW_SET_NORMALIZED, &CViewFrame::OnViewSetNormalized)
	ON_COMMAND(IDR_ANTLER_PLANE1, &CViewFrame::OnEnterAntlerPlane1)
	ON_COMMAND(IDR_ANTLER_PLANE2, &CViewFrame::OnEnterAntlerPlane2)
    ON_COMMAND(IDR_ANTLER_AUTO_PLANE, &CViewFrame::OnEnterAntlerAutoPlanes)
    ON_COMMAND(IDR_SELECT_TIPS, &CViewFrame::OnEnterSelMode)
    ON_COMMAND(IDR_LABELING, &CViewFrame::OnEnterLabelingMode)
	ON_COMMAND(ID_VIEW_SCORE, &CViewFrame::OnViewScore)
	ON_COMMAND(ID_VIEW_SCORE_BC, &CViewFrame::OnViewScoreBC)
	ON_COMMAND(ID_VIEW_SCORE_S, &CViewFrame::OnViewScoreS)

	ON_COMMAND(IDR_VIEW_VOLUME, &CViewFrame::OnViewVolume)
	ON_COMMAND(IDR_VIEW_VOLUME_LEFT, &CViewFrame::OnViewVolumeLeft)
	ON_COMMAND(IDR_VIEW_VOLUME_RIGHT, &CViewFrame::OnViewVolumeRight)

	ON_UPDATE_COMMAND_UI(ID_VIEW_ORBIT, &CViewFrame::OnOrbitUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM, &CViewFrame::OnZoomUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PAN, &CViewFrame::OnPanUpdate)

	ON_UPDATE_COMMAND_UI(IDR_ANTLER_PLANE1, &CViewFrame::OnUpdatePlane1)
	ON_UPDATE_COMMAND_UI(IDR_ANTLER_PLANE2, &CViewFrame::OnUpdatePlane2)
    ON_UPDATE_COMMAND_UI(IDR_ANTLER_AUTO_PLANE, &CViewFrame::OnUpdateAutoPlanes)

    ON_UPDATE_COMMAND_UI(IDR_SELECT_TIPS, &CViewFrame::OnUpdateSelLength)
    ON_UPDATE_COMMAND_UI(IDR_LABELING, &CViewFrame::OnUpdateLabelingTool)

	ON_UPDATE_COMMAND_UI(IDR_VIEW_VOLUME, &CViewFrame::OnUpdateVolume)
	ON_UPDATE_COMMAND_UI(IDR_VIEW_VOLUME_LEFT, &CViewFrame::OnUpdateLeftVolume)
	ON_UPDATE_COMMAND_UI(IDR_VIEW_VOLUME_RIGHT, &CViewFrame::OnUpdateRightVolume)

	ON_UPDATE_COMMAND_UI(ID_VIEW_NORMALIZE, &CViewFrame::OnUpdateToolbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_NORMALIZE_RESTORE, &CViewFrame::OnUpdateToolbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SET_NORMALIZED, &CViewFrame::OnUpdateToolbar)
	//ON_UPDATE_COMMAND_UI(ID_VIEW_RESET, &CViewFrame::OnUpdateToolbar)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_TO_PDF, &CViewFrame::OnUpdateToolbar)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_TO_PDF_BC, &CViewFrame::OnUpdateToolbar)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_TO_PDF_S, &CViewFrame::OnUpdateToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCORE, &CViewFrame::OnUpdateToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCORE_BC, &CViewFrame::OnUpdateToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCORE_S, &CViewFrame::OnUpdateToolbar)
	ON_UPDATE_COMMAND_UI(IDR_VIEW_CENTERLINE, &CViewFrame::OnUpdateCenterline)
	ON_UPDATE_COMMAND_UI(IDR_VIEW_ANNOTATIONS, &CViewFrame::OnUpdateAnnotations)

	ON_COMMAND(ID_VIEW_AXIS, &CViewFrame::OnViewAxis)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AXIS, &CViewFrame::OnUpdateAxis)
	ON_COMMAND(IDR_VIEW_ANNOTATIONS, &CViewFrame::OnViewAnnotations)

    // Alex S.: For critical points algorithm testing. Should be removed later.
    // Called on F5.
    ON_COMMAND(ID_CRITICAL_POINTS, &CViewFrame::OnCriticalPoints)
    
END_MESSAGE_MAP()

bool CViewFrame::registerControl(std::wstring fName, bool reg)
{
	const char _szDllRegSvr[] = "DllRegisterServer";
	const char _szDllUnregSvr[] = "DllUnregisterServer";
	HRESULT (STDAPICALLTYPE * lpDllEntryPoint)(void);
	LPCSTR pszDllEntryPoint;
	if(reg)
		pszDllEntryPoint = _szDllRegSvr;
	else
		pszDllEntryPoint = _szDllUnregSvr;

	LPCTSTR pszDllName = fName.c_str();
	bool bRes = true;

	// Initialize OLE.
	if (FAILED(OleInitialize(NULL))) {
		//MessageBox(L"OLE INIT FAILED", L"Antler", 0);
		return false;
	}

	SetErrorMode(SEM_FAILCRITICALERRORS);       // Make sure LoadLib fails.
	
	// Load the library.
	HINSTANCE hLib = LoadLibraryEx(pszDllName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	if (hLib < (HINSTANCE)HINSTANCE_ERROR) {
		//MessageBox(L"HINSTANCE_ERROR", L"Antler", 0);
		bRes = false;
		goto CleanupOle;
	}

	// Find the entry point.
	(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, pszDllEntryPoint);

	if (lpDllEntryPoint == NULL) {
		//MessageBox(L"NO ENTRY POINT", L"Antler", 0);
		bRes = false;
		goto CleanupLibrary;
	}

	// Call the entry point.
	if (FAILED((*lpDllEntryPoint)())) {
		//MessageBox(L"CALL FAILED", L"Antler", 0);
		bRes = false;
		goto CleanupLibrary;
	}

CleanupLibrary:
	FreeLibrary(hLib);

CleanupOle:
	OleUninitialize();

	return bRes;
}

// CViewFrame drawing
void CViewFrame::OnDraw(CDC* pDC)
{

}

void CViewFrame::OnSizing( UINT, LPRECT )
{
}

void CViewFrame::OnSize( UINT, int x, int y )
{
	
	m_view.SetWindowPos(0, 
						0, 0,
						x, y, 
						SWP_NOMOVE | SWP_NOZORDER); 
}

int CViewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// TODO: check for dll ver in future
	registerControl(L"ViewControl.dll", false);
	registerControl(L"ViewControl.dll");
	if(!m_view.Create(NULL, WS_VISIBLE, CRect(0,0,100,100), this, 0))
		if(registerControl(L"ViewControl.dll"))
		{
			if(!m_view.Create(NULL, WS_VISIBLE, CRect(0,0,100,100), this, 0))
				MessageBox(L"Can't find ActiveX control for Antler", L"Antler");
		}
		else
			MessageBox(L"Can't register ActiveX control for Antler", L"Antler");

	return 0;
}

// CViewFrame message handlers

BOOL CViewFrame::OnMouseWheel( UINT nFlags, short zDelta, CPoint point )
{
	RECT rect;
	m_view.GetWindowRect(&rect);
	if(PtInRect(&rect, point))
		m_view.SendMessage(WM_MOUSEWHEEL, MAKELONG(nFlags, zDelta), MAKELONG(point.x, point.y));

	return TRUE;
}

void CViewFrame::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_view.SendMessage(WM_KEYDOWN, nChar, MAKELONG(nFlags, 0));
}

bool CViewFrame::loadAntler(std::wstring fName)
{
	m_isMeshLoaded = m_view.loadAntler(fName.c_str())?true:false;
	return m_isMeshLoaded;
}
void CViewFrame::exportToPDF(std::wstring fName, ScoresheetType type)
{
	m_view.exportToPDF(fName.c_str(), type);
}

void CViewFrame::OnViewOrbit()
{
	m_view.toggleOrbitMode();
}

void CViewFrame::OnViewZoom()
{
	m_view.toggleZoomMode();
}

void CViewFrame::OnViewPan()
{
	m_view.togglePanMode();
}

void CViewFrame::OnViewReset()
{
	m_view.resetMode();	
}

void CViewFrame::OnViewNormalize()
{
	m_view.normalize();	
}
void CViewFrame::OnViewNormalizeRestore()
{
    m_view.normalizeRestore();	
}
void CViewFrame::OnViewSetNormalized()
{
    m_view.setNormalized();	
}

void CViewFrame::OnEnterAntlerPlane1()
{
	uncheckVolumeButtons();
	if(m_view.enterAntlerPlane(0))
	{
		m_wndStatusBar->SetPaneText(0,L"Identify left burr plane by selecting 3 points on mesh");
		m_identifyingPlane = true;
	}
}

void CViewFrame::OnEnterAntlerPlane2()
{
	uncheckVolumeButtons();
	if(m_view.enterAntlerPlane(1))
	{
		m_wndStatusBar->SetPaneText(0,L"Identify right burr plane by selecting 3 points on mesh");
		m_identifyingPlane = true;
	}
}
void CViewFrame::OnEnterAntlerAutoPlanes()
{
    uncheckVolumeButtons();
    if(m_view.enterAntlerAutoPlanes())
    {
        m_wndStatusBar->SetPaneText(0,L"Ready");
        m_identifyingPlane = false;
    }
}
void CViewFrame::OnEnterSelMode()
{
	if(m_view.getToolMode() != TM_SEL_LENGTH)
		uncheckVolumeButtons();
    if(m_view.enterSelectMode())
    {
        m_wndStatusBar->SetPaneText(0,L"Select the tip of a branch to see centerline and length of the branch");
        m_identifyingPlane = false;
    }
}

void CViewFrame::OnEnterLabelingMode()
{
    if(m_view.getToolMode() != TM_TIP_LABELING)
        uncheckVolumeButtons();
    if(m_view.enterLabelingMode())
    {
        m_wndStatusBar->SetPaneText(0,L"Select the tip of a branch and change its name");
        m_identifyingPlane = false;
    }
}

void CViewFrame::OnViewScore()
{
	uncheckVolumeButtons();
	m_view.viewScoreSheet(ST_POPE_AND_YOUNG);
}

void CViewFrame::OnViewScoreBC()
{
	uncheckVolumeButtons();
	m_view.viewScoreSheet(ST_BOONE_AND_CROCKETT);
}

void CViewFrame::OnViewScoreS()
{
	uncheckVolumeButtons();
	if (MessageBox(L"Is the antler a whitetail deer?\n(Yes = whitetail deer, No = mule deer)", L"Antler", MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) == IDYES) 
		m_view.viewScoreSheet(ST_SAFARI_WHITETAIL);
	else
		m_view.viewScoreSheet(ST_SAFARI_MULE);
}

// Alex S.: For critical points algorithm testing. Should be removed later.
// Called on F5.
void CViewFrame::OnCriticalPoints()
{
	//MessageBox(L"Can't register ActiveX control for Antler", L"Antler");
    m_view.findCriticalPoints();
    m_view.Invalidate();
}

void CViewFrame::OnViewAxis()
{
	if((m_view.getVisibleElements() & VE_AXIS) == 0)
		uncheckVolumeButtons();
	m_view.toggleShowAxis();
}

void CViewFrame::OnViewAnnotations()
{
	if((m_view.getVisibleElements() & VE_ANNOTATIONS) == 0)
		uncheckVolumeButtons();
	m_view.toggleShowAnnotations();
}

void CViewFrame::OnUpdateToolbar(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_isMeshLoaded);
}

void CViewFrame::OnUpdateAxis(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_view.getVisibleElements() & VE_AXIS);
}

void CViewFrame::OnUpdateCenterline(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_isMeshLoaded);
	pCmdUI->SetCheck((m_view.getVisibleElements() & VE_CENTERLINE) > 0);
}

void CViewFrame::OnUpdateAnnotations(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_isMeshLoaded);
	pCmdUI->SetCheck((m_view.getVisibleElements() & VE_ANNOTATIONS) > 0);
}

void CViewFrame::OnOrbitUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_isMeshLoaded);
	pCmdUI->SetCheck(m_view.getNavigationMode() == NM_ORBIT?1:0);
}
void CViewFrame::OnPanUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_isMeshLoaded);
	pCmdUI->SetCheck(m_view.getNavigationMode() == NM_PAN?1:0);
}
void CViewFrame::OnZoomUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_isMeshLoaded);
	pCmdUI->SetCheck(m_view.getNavigationMode() == NM_ZOOM?1:0);
}

void CViewFrame::OnUpdateVolume( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_isMeshLoaded);
	pCmdUI->SetCheck(m_showVolume == 0?1:0);
}

void CViewFrame::OnUpdateLeftVolume( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_isMeshLoaded);
	pCmdUI->SetCheck(m_showVolume == 1?1:0);
}

void CViewFrame::OnUpdateRightVolume( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_isMeshLoaded);
	pCmdUI->SetCheck(m_showVolume == 2?1:0);
}

void CViewFrame::OnUpdatePlane1( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_isMeshLoaded);
	short res = m_view.planeDetectionMode();
	pCmdUI->SetCheck(res == 0?1:0);
	if(m_identifyingPlane && res<0)
	{
		m_identifyingPlane = false;
		m_wndStatusBar->SetPaneText(0,L"Ready");
	}
}

void CViewFrame::OnUpdatePlane2( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_isMeshLoaded);
	short res = m_view.planeDetectionMode();
	pCmdUI->SetCheck(res == 1?1:0);
	if(m_identifyingPlane && res<0)
	{
		m_identifyingPlane = false;
		m_wndStatusBar->SetPaneText(0,L"Ready");
	}
}
//TODO: DO
void CViewFrame::OnUpdateAutoPlanes( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_isMeshLoaded);
    pCmdUI->Enable(m_isMeshLoaded);
    pCmdUI->SetCheck(false);
    if(false)
    {
        m_identifyingPlane = false;
        m_wndStatusBar->SetPaneText(0,L"Ready");
    }
}

void CViewFrame::OnUpdateSelLength( CCmdUI* pCmdUI )
{
    pCmdUI->Enable(m_isMeshLoaded);
    short res = m_view.getToolMode();
    pCmdUI->SetCheck(res == (short)TM_SEL_LENGTH?1:0);
}

void CViewFrame::OnUpdateLabelingTool( CCmdUI* pCmdUI )
{
    pCmdUI->Enable(m_isMeshLoaded);
    short res = m_view.getToolMode();
    pCmdUI->SetCheck(res == (short)TM_TIP_LABELING?1:0);
}

void CViewFrame::OnViewVolume()
{
	m_showVolume = m_view.showVolume(0);
}

void CViewFrame::OnViewVolumeLeft()
{
	m_showVolume = m_view.showVolume(1);
}

void CViewFrame::OnViewVolumeRight()
{
	m_showVolume = m_view.showVolume(2);
}

void CViewFrame::uncheckVolumeButtons()
{
	m_showVolume = -1;
	// >2 to disable
	m_view.showVolume(3);
}