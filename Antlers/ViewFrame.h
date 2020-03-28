#pragma once

#include <string>

#include "ChildView.h"

// CViewFrame view

enum NavigationModes
{
	NM_NONE,
	NM_ORBIT,
	NM_ZOOM,
	NM_PAN
};

enum VisibleElements
{
	VE_NONE = 0,
	VE_AXIS = 1,
	VE_ANNOTATIONS = 2,
	VE_CENTERLINE = 4
};

typedef enum 
{
    TM_NONE = -1,
    TM_BUR_PLANE_L, // detect left burr plane
    TM_BUR_PLANE_R, // detect right burr plane
    TM_BUR_PLANE_A, // auto detection of burr planes (both left and right)
    TM_SEL_LENGTH,  // branch select tool
    TM_TIP_LABELING // manual labeling tool
} ToolModes;

enum ScoresheetType
{
	ST_POPE_AND_YOUNG = 0,
	ST_BOONE_AND_CROCKETT = 1,
	ST_SAFARI_WHITETAIL = 2, 
	ST_SAFARI_MULE = 3
};

class CViewFrame : public CWnd
{
	DECLARE_DYNCREATE(CViewFrame)

public:
	CViewFrame();           // protected constructor used by dynamic creation
	virtual ~CViewFrame();

	void uncheckVolumeButtons();
	bool loadAntler(std::wstring fName);
	void exportToPDF(std::wstring fName, ScoresheetType type);
	bool registerControl(std::wstring fName, bool reg = true);

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view

	CStatusBar	*m_wndStatusBar;
	CChildView	m_view;
	bool		m_isMeshLoaded;
	int			m_showVolume;
	bool		m_identifyingPlane;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSizing( UINT, LPRECT ); 
	afx_msg void OnSize( UINT, int, int );

	afx_msg void OnKeyDown( UINT, UINT, UINT );
	afx_msg BOOL OnMouseWheel( UINT, short, CPoint );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewOrbit();
	afx_msg void OnViewZoom();
	afx_msg void OnViewPan();
	afx_msg void OnViewReset();
	afx_msg void OnViewNormalize();
    afx_msg void OnViewNormalizeRestore();
    afx_msg void OnViewSetNormalized();
	afx_msg void OnEnterAntlerPlane1();
	afx_msg void OnEnterAntlerPlane2();
    afx_msg void OnEnterAntlerAutoPlanes();
    afx_msg void OnEnterSelMode();
    afx_msg void OnEnterLabelingMode();
	afx_msg void OnViewScore();
	afx_msg void OnViewScoreBC();
	afx_msg void OnViewScoreS();

	afx_msg void OnUpdateToolbar(CCmdUI* pCmdUI);
	afx_msg void OnOrbitUpdate( CCmdUI* pCmdUI );
	afx_msg void OnZoomUpdate( CCmdUI* pCmdUI );
	afx_msg void OnPanUpdate( CCmdUI* pCmdUI );

	afx_msg void OnUpdateVolume( CCmdUI* pCmdUI );
	afx_msg void OnUpdateLeftVolume( CCmdUI* pCmdUI );
	afx_msg void OnUpdateRightVolume( CCmdUI* pCmdUI );

	afx_msg void OnUpdatePlane1( CCmdUI* pCmdUI );
	afx_msg void OnUpdatePlane2( CCmdUI* pCmdUI );
    afx_msg void OnUpdateAutoPlanes( CCmdUI* pCmdUI );

    afx_msg void OnUpdateSelLength( CCmdUI* pCmdUI );
    afx_msg void OnUpdateLabelingTool( CCmdUI* pCmdUI );

	afx_msg void OnUpdateAxis(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCenterline(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnnotations(CCmdUI* pCmdUI);

	afx_msg void OnViewVolume();
	afx_msg void OnViewVolumeLeft();
	afx_msg void OnViewVolumeRight();

	afx_msg void OnViewAnnotations();
	afx_msg void OnViewAxis();

    // Alex S.: For critical points algorithm testing. Should be removed later.
    afx_msg void OnCriticalPoints();

};

 
