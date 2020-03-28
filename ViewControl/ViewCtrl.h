#pragma once

// ViewCtrl.h : Declaration of the CViewCtrl ActiveX Control class.

#include "stdafx.h"

#include "ScoreSheet.h"

#include "Kernel\shaders.h"
#include "Kernel\lights.h"
#include "Kernel\Vector3.h"
#include "Kernel\Matrix4.h"
#include "Kernel\Plane.h"
#include "Kernel\Annotations.h"
#include "CenterLine.h"

// CViewCtrl : See ViewCtrl.cpp for implementation.

#define UPDATE_TIMER 0xff

using namespace std;

enum NavigationModes
{
	NM_NONE,
	NM_ORBIT,
	NM_ZOOM,
	NM_PAN,
	// just to change cursor
	NM_WAIT
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
    TM_TIP_LABELING  // manual labeling tool

} ToolModes;

#define	ANTLERS_BASEMSG				(WM_USER+3000)
#define ANTLERS_POPUP_ACTION_BEGIN  (ANTLERS_BASEMSG+200)

class CTriMesh;
class GeoTreeLine;
class CriticalPointsFinder;

typedef std::map<std::string, AntlerTip*> AntlerTipsMap;

//------------------------------------------------------------
// ---

// UNICODE
void splitFilename(const std::wstring& qualifiedName, 
    std::wstring& outBasename, std::wstring& outPath);
std::wstring getAppDirW();

// ANSI
void splitFilename(const std::string& qualifiedName, 
    std::string& outBasename, std::string& outPath);
std::string getAppDirA();


//------------------------------------------------------------
// Main class
class CViewCtrl : public COleControl
{
	DECLARE_DYNCREATE(CViewCtrl)

// Constructor
public:
	CViewCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();

	int initGL();
	int updateGL();
	bool linkGLWindow(int width, int height);
	void freeGL();
	void resizeGL(int width, int height);
	void resetView();
	Ray getCameraToViewportRay(Real screenX, Real screenY) const;

	void setLoadingProgress(int value); // 0 - 100
	void setLoadingFacesCur(int value);

	bool updateBurPlane(int num);
    bool updateBurPlaneCallList(int num);
	void drawBurPlanes();
	void deleteBurPlane(int num, bool bUpdateScene = true);
	void deleteBurPlanes();

	// Sergey A. For exporting to pdf
	//********************************************************************
	inline float getBranchVolume(int iNum)
	{
		return _triangMesh->_branchVolume[iNum];
	}
	//-------------------------------
	inline bool isBurPlaneInialized(int iNum)
	{
		return _dlBurPlane[iNum] != 0;
	}
	//********************************************************************
	inline CenterLine* getCenterLine()
	{
		return _centerLine;
	}
    inline const AntlerTipsMap& getTipsMap(size_t num) const
    {
        if(num > 0 && num < 2)
            return m_tipMap[num];
        return m_tipMap[0];
    }
    inline AntlerTipsMap& getTipsMap(size_t num)
    {
        if(num > 0 && num < 2)
            return m_tipMap[num];
        return m_tipMap[0];
    }
    inline const std::vector<AntlerTip*>& getETips(size_t num) const
    {
        if(num > 0 && num < 2)
            return m_tipsE[num];
        return m_tipsE[0];
    }
    inline const Matrix4& getViewMatrix() const {return _viewMatrix;}
    inline const Matrix4& getNormMatrix() const {return _normMatrix;}
    inline const float& getFoomFactor() const {return _viewLength;}
	inline const CTriMesh* getMesh() const {return _triangMesh;}
private:
	HGLRC		m_hRC;		// Permanent Rendering Context
	HDC			m_hDC;

	CDirectionalLight	*_pDirectionalLight;

	CenterLine			*_centerLine;
	CTriMesh			*_triangMesh;
	CShader				*_pShader;
	GLuint				_dlAxes;

	GLfloat				_fov,
						_zNear, _zFar;

	// camera direction, target, viewRay length
	Vector3				_dir, _to;
	float				_viewLength;

	GLfloat				_zoom, _zoomFactor,
						_angleX, _angleY, _angleZ;

	Matrix3				_rotationMatrix;
	Matrix4				_projectionMatrix;
	Matrix4				_normMatrix;
    Matrix4				_normMatrixAuto;
    Matrix4				_viewMatrix;
	Matrix4				_axisViewMatrix;

	bool				_burPlaneMode;
    ToolModes           _currToolMode;
	int					_currBurNum; // 0-1
	Vector3				_currSelected;				// in world space
	Vector3				_currSelectedObject;
	vector<Vector3>		_burPlanePoints[2];			// in world space
	vector<Vector3>		_burPlanePointsObject[2];	// in object space
	Plane				_burPlane[2];				// in object space
	GLuint				_dlBurPlane[2];
	Vector3				_burCenter[2];
    Plane				_burPlaneWorld[2];
    Vector3             _burCenterWorld[2];        // in world space
    float               _burrRingRadius[2];        // burr rings' radii

	CPoint				_lastMousePosition;

	int				m_width, m_height;
	CProgressCtrl	m_ProgressBar;
	bool			m_bPrBarLoaded;

	NavigationModes	m_NavMode;
	NavigationModes	m_CurCursor;

	int					m_updateBval;	// if F tip changed and B value need to be recomputed
	AntlerAnnotation	m_annotation;
    std::map<std::string, AntlerTip*> m_tipMap[2]; // [0] - Left, [1] - Right
    std::vector<AntlerTip*>           m_tipsE[2];      // array of abnormal tips - "E" tips

    /** Alex S.: For critical points algorithm testing. Should be removed later. **/
    std::vector<Vector3>     _extremumPoints;
    std::vector<Vector3>     _saddlePoints;
    std::vector<Vector3>     _rootPoints;
    std::vector<GeoTreeLine> _geoTreeLines;
    /******************************************************************************/

	void rebuildView();
	void setCursor(NavigationModes cursor);

	vector<Vector3>		_dimPoints;

	CScoreSheet	m_scoreWnd;

	bool		m_showCenterLine;
	bool		m_showAnnotations;
	bool		m_showAxis;
	HANDLE		m_threadEvent;
	CWinThread* m_compThread;

	static UINT centerLineThread(LPVOID me);
	void computeCenterLine();
	void computeCenterLineThread();

	volatile bool m_isComputing;

	int m_CurFace;
	int m_NumOfFaces;

	//bool NeedToColorBranch[3]; //Sergey A. For coloring of computed volume
// Implementation
protected:
	~CViewCtrl();

	DECLARE_OLECREATE_EX(CViewCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CViewCtrl)      // GetTypeInfo
	DECLARE_OLECTLTYPE(CViewCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

	afx_msg void OnMouseMove( UINT, CPoint );
	afx_msg void OnLButtonDown( UINT, CPoint );
	afx_msg void OnLButtonUp( UINT, CPoint );
	afx_msg void OnRButtonDown( UINT, CPoint );
	afx_msg void OnRButtonUp( UINT, CPoint );
	afx_msg BOOL OnMouseWheel( UINT, short, CPoint );

#ifdef _WIN64
	afx_msg void OnTimer( UINT_PTR );					// update view by timer
#else
	afx_msg void OnTimer(UINT);							// update view by timer
#endif
	afx_msg void OnDestroy();
	afx_msg void OnSize( UINT, int, int );
	afx_msg void OnKeyDown( UINT, UINT, UINT );
	afx_msg BOOL OnSetCursor(CWnd*, UINT, UINT);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
        dispidNormalize = 107L,
		dispidsetProgressBarHandle = 106L,
		dispidloadAntler = 104L
	};

protected:
	BOOL loadAntler(LPCTSTR filename);
	SHORT toggleOrbitMode();
	SHORT toggleZoomMode();
	SHORT togglePanMode();
	SHORT getNavigationMode();

	SHORT showVolume(ULONG num); // 0 - rack vol, 1 - left branch, 2 - right branch
	SHORT planeDetectionMode(); // 0,1 if plane detection, -1 if no
    SHORT getToolMode();        // returns _currToolMode

	void toggleShowCenterLine();
	void toggleShowAnnotations();
	void toggleShowAxis();
	SHORT getVisibleElements();
	void resetMode();
    BOOL enterSelectMode();
    BOOL enterLabelingMode();
	BOOL enterAntlerPlane(ULONG num); // 0-1
    BOOL enterAntlerAutoPlanes();
	void setProgressBarHandle(ULONG handle);
	void normalizeView(void); 
    void setNormalizedView(void); 
    void normalizedViewRestore(void); 

    /** Alex S.: For critical points algorithm testing. Should be removed later. **/
    void findCriticalPoints(void);
    void drawCriticalPoints();
	/******************************************************************************/
	void CViewCtrl::ColorBranch(int iNum);
	void CViewCtrl::ShowVolumeText(float xpos, float ypos);
	void exportToPDF(LPCTSTR filename, ULONG type);
	void viewScoreSheet(ULONG type);

    void updateAntlerTipMap(void);
    void updateAnnotations(void);

    void dumpFunc();
};


CViewCtrl* GetGlobalView();
