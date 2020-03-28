// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Antlers.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_EXPORT_TO_PDF, &CMainFrame::OnExportToPdf)
	ON_COMMAND(ID_EXPORT_TO_PDF_BC, &CMainFrame::OnExportToPdfBC)
	ON_COMMAND(ID_EXPORT_TO_PDF_S, &CMainFrame::OnExportToPdfS)
	ON_COMMAND(IDR_VIEW_CENTERLINE, &CMainFrame::OnViewCenterLine)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	//ID_PROGRBAR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bPrBarCreated = FALSE;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 100, 100), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.LoadTrueColorToolBar(16,
									   IDB_MAIN_TOOLBAR,
		                               IDB_MAIN_TOOLBAR_HOT,
									   IDB_MAIN_TOOLBAR_DISABLED);


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	else 
	{
		RECT rc;
		m_wndStatusBar.GetItemRect(1,&rc);
		if (m_bPrBarCreated==FALSE)
		{
			m_ProgressBar.Create(WS_VISIBLE|WS_CHILD, rc,&m_wndStatusBar, 1);

			m_ProgressBar.SetRange(0,100);
			m_ProgressBar.SetStep(1);
			m_wndView.m_view.setProgressBarHandle((ULONG)m_ProgressBar.GetSafeHwnd());

			m_ProgressBar.ShowWindow(SW_HIDE);
			m_bPrBarCreated=TRUE;
		}
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_wndView.m_wndStatusBar = &m_wndStatusBar;

	/*m_ProgressBar.ShowWindow(SW_SHOW);
	m_wndView.loadAntler(L"Media/2001 Whitetail 4-9-07.stl");
	m_ProgressBar.ShowWindow(SW_HIDE);*/

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnSize( UINT nType, int x, int y )
{
	CFrameWnd::OnSize(nType, x, y);
	if (m_bPrBarCreated)
	{
		RECT rc;
		m_wndStatusBar.GetItemRect(0,&rc);
		m_ProgressBar.SetWindowPos(&wndTop, rc.left, rc.top,
		rc.right - rc.left,rc.bottom - rc.top, 0);
	}
}

//-----------------------------------------------------------------------
void splitFilename(const std::wstring& qualifiedName, 
    std::wstring& outBasename, std::wstring& outPath)
{
    std::wstring path = qualifiedName;
    // Replace \ with / first
	::std::replace( path.begin(), path.end(), L'\\', L'/' );
    // split based on final /
    size_t i = path.find_last_of(L'/');

    if (i == std::wstring::npos)
    {
        outPath.clear();
		outBasename = qualifiedName;
    }
    else
    {
        outBasename = path.substr(i+1, path.size() - i - 1);
        outPath = path.substr(0, i+1);
    }
}

void CMainFrame::OnFileOpen()
{
	m_wndView.uncheckVolumeButtons();

	std::wstring fName = OpenFileDialog(L"Binary mesh (*.stl)\0*.stl\0");
	if(fName != L"")
	{
		m_ProgressBar.ShowWindow(SW_SHOW);
		m_wndToolBar.EnableWindow(false);
		this->EnableWindow(false);
		m_wndView.loadAntler(fName);
		this->EnableWindow(true);
		m_wndToolBar.EnableWindow(true);
		m_wndView.SetFocus();
		m_ProgressBar.ShowWindow(SW_HIDE);

		std::wstring fileName, dir;
		splitFilename(fName, fileName, dir);

		if(m_wndView.m_isMeshLoaded)
			this->SetTitle((fileName + L" - Antlers").c_str());
		else
			this->SetTitle(L"Antlers");

		// upfate title
		this->OnUpdateFrameTitle(TRUE);
	}
}


std::wstring CMainFrame::OpenFileDialog(wchar_t* filter)
{
	OPENFILENAME fName;
	ZeroMemory(&fName,sizeof(OPENFILENAME));
	fName.lStructSize=sizeof(OPENFILENAME);
	fName.hwndOwner=GetSafeHwnd();
	fName.hInstance=GetModuleHandle(NULL);
	fName.lpstrFilter=filter;
	wchar_t strFileName[1024];
	strFileName[0]=0;
	fName.lpstrFile=strFileName;
	fName.nMaxFile=1024;
	fName.lpstrInitialDir=L".";
	fName.lpstrTitle=L"Open mesh file ...";
	fName.Flags= OFN_LONGNAMES | OFN_ENABLESIZING | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if(GetOpenFileName(&fName))
	{
		return fName.lpstrFile;
	} 
	else
		return L"";
}

std::wstring CMainFrame::SaveFileDialog(wchar_t* filter)
{
	OPENFILENAME fName;
	ZeroMemory(&fName,sizeof(OPENFILENAME));
	fName.lStructSize=sizeof(OPENFILENAME);
	fName.hwndOwner=GetSafeHwnd();
	fName.hInstance=GetModuleHandle(NULL);
	fName.lpstrFilter=filter;
	wchar_t strFileName[1024];
	strFileName[0]=0;
	fName.lpstrFile=strFileName;
	fName.nMaxFile=1024;
	fName.lpstrInitialDir=L".";
	fName.lpstrTitle=L"Export results ...";
	fName.lpstrDefExt=L"pdf";
	fName.Flags= OFN_LONGNAMES | OFN_ENABLESIZING | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
	if(GetSaveFileName(&fName))
	{
		return fName.lpstrFile;
	} 
	else
		return L"";
}

void CMainFrame::OnExportToPdf()
{
	m_wndView.uncheckVolumeButtons();

	std::wstring fName = SaveFileDialog(L"Portable Document Format (*.pdf)\0*.pdf\0");
	if(fName != L"")
	{
		m_wndView.exportToPDF(fName, ST_POPE_AND_YOUNG);
	}
}

void CMainFrame::OnExportToPdfBC()
{
	m_wndView.uncheckVolumeButtons();

	std::wstring fName = SaveFileDialog(L"Portable Document Format (*.pdf)\0*.pdf\0");
	if(fName != L"")
	{
		m_wndView.exportToPDF(fName, ST_BOONE_AND_CROCKETT);
	}
}

void CMainFrame::OnExportToPdfS()
{
	m_wndView.uncheckVolumeButtons();

	ScoresheetType type;
	if (MessageBox(L"Is the antler a whitetail deer?\n(Yes = whitetail deer, No = mule deer)", L"Antler", MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) == IDYES) 
		type = ST_SAFARI_WHITETAIL;
	else
		type = ST_SAFARI_MULE;

	std::wstring fName = SaveFileDialog(L"Portable Document Format (*.pdf)\0*.pdf\0");
	if(fName != L"")
	{
		m_wndView.exportToPDF(fName, type);
	}
}

void CMainFrame::OnViewCenterLine()
{
	if((m_wndView.m_view.getVisibleElements() & VE_CENTERLINE) == 0)
		m_wndView.uncheckVolumeButtons();

	this->EnableWindow(false);
	m_wndStatusBar.SetPaneText(0,L"Please wait while computing center line");
	m_wndView.m_view.toggleShowCenterLine();
	m_wndStatusBar.SetPaneText(0,L"Ready");
	this->EnableWindow(true);
	m_wndView.SetFocus();
}

void CMainFrame::uncheckVolumeButtons()
{
	m_wndView.uncheckVolumeButtons();
}