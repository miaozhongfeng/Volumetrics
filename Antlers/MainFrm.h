// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include <afxcmn.h>
#include <algorithm>
#include <string>

#include "ViewFrame.h"
#include "ChildView.h"
#include "TrueColorToolBar.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:
	std::wstring CMainFrame::OpenFileDialog(wchar_t* filter);
	std::wstring CMainFrame::SaveFileDialog(wchar_t* filter);
	void uncheckVolumeButtons();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar			m_wndStatusBar;
	CTrueColorToolBar	m_wndToolBar;
	CViewFrame			m_wndView;
	//CChildView			m_wndView;

	BOOL			m_bPrBarCreated;
	CProgressCtrl	m_ProgressBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnSize( UINT, int, int );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnExportToPdf();
	afx_msg void OnExportToPdfBC();
	afx_msg void OnExportToPdfS();
	afx_msg void OnViewCenterLine();
};


