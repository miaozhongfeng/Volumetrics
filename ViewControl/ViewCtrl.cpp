// ViewCtrl.cpp : Implementation of the CViewCtrl ActiveX Control class.

#include "stdafx.h"
#include "ViewControl.h"
#include "ViewCtrl.h"

#include "Kernel\materials.h"
#include "Kernel\exceptions.h"
#include "Kernel\labels.h"
#include "Kernel\trimeshes.h"
#include "Kernel\matrix4.h"
#include "Kernel\quaternion.h"
#include "Kernel\ray.h"
#include "..\Mesh\MeshVolume.h"

#include "Kernel\ExportPDF.h"
#include "Geometry\CriticalPointsFinder.h"
#include "Debug\Debugger.h"

#include "BurrPlaneFinder.h"

#include <sstream>
#include <stdio.h>

extern void calcNormalizedView(CTriMesh* pObj, Matrix4& normView);
extern void setNormalizedView(CTriMesh* pObj, Matrix4& normView);

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEBUG_CENTER_LINES 1
#define FOR_INTERNAL_TESTING 0

static CViewCtrl* g_ViewCtrl;

//Sergey A.
//Bur planes. Used to check if burplane had changed.
Plane oldBurplane[2];
int lastColoredPart = 0;
GLuint _dlMeshPart1, _dlMeshPart2;
int g_branchToColor = -1;

CViewCtrl* GetGlobalView()
{
	return g_ViewCtrl;
}


IMPLEMENT_DYNCREATE(CViewCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CViewCtrl, COleControl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
    ON_COMMAND(ANTLERS_POPUP_ACTION_BEGIN, &CViewCtrl::dumpFunc)
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CViewCtrl, COleControl)
	DISP_FUNCTION_ID(CViewCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "toggleOrbitMode", 100, toggleOrbitMode, VT_I2, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "toggleZoomMode", 101, toggleZoomMode, VT_I2, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "togglePanMode", 102, togglePanMode, VT_I2, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "resetMode", 103, resetMode, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "loadAntler", dispidloadAntler, loadAntler, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CViewCtrl, "setProgressBarHandle", dispidsetProgressBarHandle, setProgressBarHandle, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CViewCtrl, "normalizeView", dispidNormalize, normalizeView, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "enterAntlerPlane", 108, enterAntlerPlane, VT_BOOL, VTS_UI4)
	DISP_FUNCTION_ID(CViewCtrl, "exportToPDF", 109, exportToPDF, VT_EMPTY, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CViewCtrl, "viewScoreSheet", 110, viewScoreSheet, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CViewCtrl, "getNavigationMode", 111, getNavigationMode, VT_I2, VTS_NONE)
    DISP_FUNCTION_ID(CViewCtrl, "findCriticalPoints", 112, findCriticalPoints, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "showVolume", 113, showVolume, VT_I2, VTS_UI4)
	DISP_FUNCTION_ID(CViewCtrl, "toggleShowCenterLine", 114, toggleShowCenterLine, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "toggleShowAnnotations", 115, toggleShowAnnotations, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "planeDetectionMode", 116, planeDetectionMode, VT_I2, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "toggleShowAxis", 117, toggleShowAxis, VT_EMPTY, VTS_NONE)
    DISP_FUNCTION_ID(CViewCtrl, "enterSelectMode", 118, enterSelectMode, VT_BOOL, VTS_NONE)
    DISP_FUNCTION_ID(CViewCtrl, "getToolMode", 119, getToolMode, VT_I2, VTS_NONE)
	DISP_FUNCTION_ID(CViewCtrl, "getVisibleElements", 120, getVisibleElements, VT_I2, VTS_NONE)
    DISP_FUNCTION_ID(CViewCtrl, "enterAntlerAutoPlanes", 121, enterAntlerAutoPlanes, VT_BOOL, VTS_NONE)
    DISP_FUNCTION_ID(CViewCtrl, "enterLabelingMode", 122, enterLabelingMode, VT_BOOL, VTS_NONE)
    DISP_FUNCTION_ID(CViewCtrl, "setNormalizedView", 123, setNormalizedView, VT_EMPTY, VTS_NONE)
    DISP_FUNCTION_ID(CViewCtrl, "normalizedViewRestore", 124, normalizedViewRestore, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CViewCtrl, COleControl)
END_EVENT_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CViewCtrl, "VIEWCONTROL.AntlersViewCtrl.1",
	0x3d224ceb, 0x2a80, 0x4b47, 0x8e, 0xc5, 0x94, 0x69, 0x65, 0x83, 0x73, 0x3a)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CViewCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DViewControl =
		{ 0x92046CFC, 0xE282, 0x466B, { 0x8E, 0x40, 0x57, 0xE8, 0xFC, 0x4E, 0xEB, 0x1D } };
const IID BASED_CODE IID_DViewControlEvents =
		{ 0xF105BD7D, 0xC6E7, 0x4B81, { 0x8A, 0x66, 0x1C, 0xA2, 0x95, 0xA7, 0x22, 0x51 } };



// Control type information

static const DWORD BASED_CODE _dwViewControlOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_IGNOREACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CViewCtrl, IDS_VIEWCONTROL, _dwViewControlOleMisc)


// -------------------------------------------------------------------------------------------------
// CViewCtrl::CViewCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CViewCtrl

BOOL CViewCtrl::CViewCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_VIEWCONTROL,
			IDB_VIEWCONTROL,
			afxRegInsertable | afxRegApartmentThreading,
			_dwViewControlOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

// -------------------------------------------------------------------------------------------------
// CViewCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
DWORD CViewCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;

	// The control can receive mouse notifications when inactive.
	// TODO: if you write handlers for WM_SETCURSOR and WM_MOUSEMOVE,
	//		avoid using the m_hWnd member variable without first
	//		checking that its value is non-NULL.
	dwFlags |= pointerInactive;
	return dwFlags;
}


// -------------------------------------------------------------------------------------------------
// CViewCtrl::OnResetState - Reset control to default state

void CViewCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


// -------------------------------------------------------------------------------------------------
// CViewCtrl::AboutBox - Display an "About" box to the user

void CViewCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VIEWCONTROL);
	dlgAbout.DoModal();
}

SHORT CViewCtrl::getNavigationMode()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_NavMode;
}

SHORT CViewCtrl::toggleOrbitMode()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_NavMode == NM_ORBIT)
	{
		m_NavMode = NM_NONE;
		if(m_CurCursor != NM_NONE)
			setCursor(NM_NONE);

		return false;
	}
	else
	{
		m_NavMode = NM_ORBIT;
		if(m_CurCursor != NM_ORBIT)
			setCursor(NM_ORBIT);

		return true;
	}
}

SHORT CViewCtrl::toggleZoomMode()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_NavMode == NM_ZOOM)
	{
		m_NavMode = NM_NONE;
		if(m_CurCursor != NM_NONE)
			setCursor(NM_NONE);

		return false;
	}
	else
	{
		m_NavMode = NM_ZOOM;
		if(m_CurCursor != NM_ZOOM)
			setCursor(NM_ZOOM);

		return true;
	}
}

SHORT CViewCtrl::togglePanMode()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_NavMode == NM_PAN)
	{
		m_NavMode = NM_NONE;
		if(m_CurCursor != NM_NONE)
			setCursor(NM_NONE);

		return false;
	}
	else
	{
		m_NavMode = NM_PAN;
		if(m_CurCursor != NM_PAN)
			setCursor(NM_PAN);

		return true;
	}
}

void CViewCtrl::resetMode()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_NavMode = NM_NONE;
	resetView();
	//MessageBox(L"resetMode",L"resetMode",0);
}

CMaterial MatFBOrangeNew = CMaterial(
                                     CColor(0.8, 0.6, 0.0, 0.5), 
                                     CColor(1.0, 0.8, 0.0, 0.5), 
                                     CColor(1.0, 1.0, 1.0, 0.5),
                                     CColor(0.0, 0.0, 0.0, 0.0), 
                                     128.0,
                                     CColor(0.8, 0.6, 0.0, 0.5), 
                                     CColor(1.0, 0.8, 0.0, 0.5), 
                                     CColor(1.0, 1.0, 1.0, 0.5),
                                     CColor(0.0, 0.0, 0.0, 0.0), 
                                     128.0);
CColor burPlaneColor(0.8, 0.6, 0.0, 0.5);
CColor ColorSilverAxis(0.19225, 0.19225, 0.19225, 0.4),
        ColorRubyAxis(0.92, 0.063, 0.063, 1.0),
        ColorGreenAxis(0.094, 0.757, 0.094, 1.0),
        ColorBlueAxis(0.7, 0.98, 0.97);

/////////////////////////////////////////////////////////////////////////////////////////////////
//										Branch length tool
/////////////////////////////////////////////////////////////////////////////////////////////////

class sel_sphere
{
private:
	GLUquadricObj *quadratic;
public:
	float x,y,z;
	float r;
	int ID;
	bool selected;
	
	void Init(float _x, float _y, float _z, float _r, int _ID);
	void Draw(void);

	~sel_sphere();
};

class select_tool
{
public:
    select_tool() :
        n(0),
        spheres(NULL),
        is_active(false)
    {}

    ~select_tool()
    {
       Clear();
    }

	sel_sphere *spheres;

	bool is_active;
	size_t n;

	void CheckObjectID(int mouse_x, int mouse_y, LPRECT g_rRect);
    int IntersectObjects(Ray& ray);
	void Init(CenterLine *centerline);
	void Draw(void);
    void Clear();
};

/*==================================================*/
sel_sphere::~sel_sphere()
{
	if(quadratic)
		gluDeleteQuadric(quadratic);
}
void sel_sphere::Init(float _x, float _y, float _z, float _r, int _ID)
{
	x=_x;
	y=_y;
	z=_z;
	r=_r;
	ID=_ID;

	quadratic=gluNewQuadric();  
    gluQuadricNormals(quadratic, GLU_SMOOTH); 
//    gluQuadricTexture(quadratic, GL_TRUE);
}

void sel_sphere::Draw(void)
{
	float _r = r;
	if(GetGlobalView()->getMesh())
		_r = _r / GetGlobalView()->getMesh()->_ratio;

    MatFBOrangeNew.Apply();
	glPushMatrix();
		if(GetGlobalView()->getMesh())
			glMultMatrixf(GetGlobalView()->getMesh()->_worldMatrix.Transpose());
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
            glTranslatef(x,y,z);
    //		glColor3f(0.0f, 1.0f, 0.0f);	
		    gluSphere(quadratic,_r,16,16);

        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
	glPopMatrix();
}
/*=============================================*/

void select_tool::Init(CenterLine *centerline)
{
	if(!centerline)
		return;

    Clear();

	n = centerline->m_annotations[0].size()+centerline->m_annotations[1].size();
	SAFE_DELETE_ARRAY(spheres);
	spheres = new sel_sphere[n];
	
    int cnt = 0;
	for(int i = 0; i<2; ++i)
	{
		for(size_t j = 0; j<centerline->m_annotations[i].size(); ++j, ++cnt)
		{
			spheres[cnt].Init(centerline->m_annotations[i][j].x,centerline->m_annotations[i][j].y,centerline->m_annotations[i][j].z, 0.02f, cnt);
		}
	}
	is_active = true;
}

void select_tool::Clear()
{
    SAFE_DELETE_ARRAY(spheres);
    is_active = false;
    n = 0;
}

void select_tool::Draw(void)
{
	if(is_active)
	{
		int k = 10;
		glInitNames();
		
		for(size_t i = 0; i < n; i++)
		{
			glPushName(k + i);
			spheres[i].Draw();
			glPopName();
		}
	}
}

void select_tool::CheckObjectID(int mouse_x, int mouse_y, LPRECT g_rRect)
{ 
	int objectsFound = 0;
	int	viewportCoords[4] = {0};						

	unsigned int selectBuffer[32] = {0};				
														
	glSelectBuffer(32, selectBuffer);
	glGetIntegerv(GL_VIEWPORT, viewportCoords);
	glMatrixMode(GL_PROJECTION);		
	
	glPushMatrix();						
		glRenderMode(GL_SELECT);

		glLoadIdentity();	
		gluPickMatrix(mouse_x, viewportCoords[3] - mouse_y, 2, 2, viewportCoords);
		gluPerspective(45.0f,(float)g_rRect->right/(float)g_rRect->bottom,0.1f,150.0f);
		glMatrixMode(GL_MODELVIEW);
		
		Draw();
		
		objectsFound = glRenderMode(GL_RENDER);
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();					

	glMatrixMode(GL_MODELVIEW);		

	if (objectsFound > 0)
	{		
		unsigned int lowestDepth = selectBuffer[1];
		int selectedObject = selectBuffer[3];
		for(int i = 1; i < objectsFound; i++)
		{
			if(selectBuffer[(i * 4) + 1] < lowestDepth)
			{
				lowestDepth = selectBuffer[(i * 4) + 1];
				selectedObject = selectBuffer[(i * 4) + 3];
			}
		}
		MessageBox(NULL,L"This is a tip",L"Object clicked",MB_OK );
//		return selectedObject;
	}
//	return 0;											
}

int select_tool::IntersectObjects(Ray& ray)
{
    int selectedObject = -1;

    Vector3 clos0, clos1;//, vec;
    float distMin = Math::POS_INFINITY;
    float sdistMin = Math::POS_INFINITY;
    float dist, sdist;

    for(size_t i = 0; i < n ; ++i)
    {
        /*OpenMesh::Vec3f v(spheres[i].x, spheres[i].y, spheres[i].z);
        
        vec.x = v[0];
        vec.y = v[1];
        vec.z = v[2];*/

		Vector3 vec(spheres[i].x, spheres[i].y, spheres[i].z);
		float r = spheres[i].r;

		if(GetGlobalView()->getMesh())
		{
			//r = r / GetGlobalView()->getMesh()->_ratio;
			vec = GetGlobalView()->getMesh()->_worldMatrix.TransformAffine(vec);
		}

        dist = ray.squaredDistanceTo(vec, clos0, clos1);
        if(dist < r * r)
        {
            sdist = (ray.getOrigin() - vec).SquaredLength();
            if(sdistMin > sdist)
            {
                distMin = dist;
                sdistMin = sdist;
                selectedObject = spheres[i].ID;
            }
        }
    }
//   if(selectedObject!=-1)
//        MessageBox(NULL,L"This is a tip",L"Object clicked",MB_OK );
    return selectedObject;
}

select_tool selection;

/////////////////////////////////////////////////////////////////////////////////////////////////
//										End of Branch length tool
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
//										THE RESIZE GL SCENE
/////////////////////////////////////////////////////////////////////////////////////////////////
void CViewCtrl::resizeGL(int width, int height)
{
	height = height ? height : 1.0;

	m_width = width;
	m_height = height;

	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(_fov, (GLfloat) width / (GLfloat) height, _zNear, _zFar);
	glGetFloatv( GL_PROJECTION_MATRIX, _projectionMatrix );
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

std::wstring getAppDirW()
{
	wchar_t buff[1024];
	GetModuleFileName(0,buff,1024);
	std::wstring fname, appPath;
	splitFilename(buff, fname, appPath);

	return appPath;
}

//-----------------------------------------------------------------------
void splitFilename(const std::string& qualifiedName, 
    std::string& outBasename, std::string& outPath)
{
    std::string path = qualifiedName;
    // Replace \ with / first
	::std::replace( path.begin(), path.end(), '\\', '/' );
    // split based on final /
    size_t i = path.find_last_of(L'/');

    if (i == std::string::npos)
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

std::string getAppDirA()
{
	char buff[1024];
	GetModuleFileNameA(0,buff,1024);
	std::string fname, appPath;
	splitFilename(buff, fname, appPath);

	return appPath;
}

void CViewCtrl::rebuildView()
{
	_rotationMatrix.FromEulerAnglesXYZ(_angleX, _angleY, _angleZ);
	
	Vector3 up = Vector3::UNIT_Y * _rotationMatrix;
	Vector3 eye = _to + _dir * _rotationMatrix * 2;//_viewLength;

	// TODO: replace in future
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, _to.x, _to.y, _to.z, up.x, up.y, up.z);
	glScalef(2.0f/_viewLength,2.0f/_viewLength,2.0f/_viewLength);
	glGetFloatv( GL_MODELVIEW_MATRIX, _viewMatrix );

	eye = _dir * _rotationMatrix * 2;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, 0, 0, 0, up.x, up.y, up.z);
	glGetFloatv( GL_MODELVIEW_MATRIX, _axisViewMatrix );
}

void CViewCtrl::resetView()
{
	_dir = Vector3(0,0,1);
	_to = Vector3(0,0,0);
	_viewLength = 2;

	_zoomFactor = 0.1;
	_angleX = _angleY = _angleZ = 0.0;

	rebuildView();
    m_annotation.update();

    _normMatrix = _viewMatrix;

	updateGL();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//										THE OPENGL INIT
/////////////////////////////////////////////////////////////////////////////////////////////////
int CViewCtrl::initGL()
{
	glewInit();

	//appPath = L"D:/!!Projects/Antlers/bin/release/";

	/*try
	{
		_pShader = new CShader();
		std::wstring vertFileName = appPath + L"Media/glass.vert";
		std::wstring fragFileName = appPath + L"Media/glass.frag";
		_pShader->InstallShaders(vertFileName, fragFileName);
	}
	catch (Exception &e)
	{
		e.ShowReason();
	}*/

	//appPath = L"";

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// RGBA
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// floatly Nice Perspective Calculations

	SetTimer(UPDATE_TIMER,33,NULL);

	resetView();

	_pDirectionalLight = 0;
	_pDirectionalLight = new CDirectionalLight(
		GL_LIGHT0,
		CHCoordinate(0.0, 0.0, 5.0, 0.0),
		CColor(0.8, 0.8, 0.8, 1.0),
		CColor(1.0, 1.0, 1.0, 1.0),
		CColor(1.0, 1.0, 1.0, 1.0));
	if (_pDirectionalLight)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		_pDirectionalLight->Enable();
	}

	_dlAxes = 0;
	_dlAxes = glGenLists(1);
	glNewList(_dlAxes, GL_COMPILE);
    
		glPushMatrix();
			MatFBSilver.Apply();
			glScalef(0.0125, 0.0125, 0.0125);
			glutSolidIcosahedron();
		glPopMatrix();

glDisable(GL_LIGHTING);
		glPushMatrix();
			//MatFBRuby.Apply();
            glColor3f(ColorRubyAxis.r, ColorRubyAxis.g, ColorRubyAxis.b);
			glLineWidth(3.0);
			glBegin(GL_LINES);
				glNormal3f(1.0, 0.0, 0.0);
				glVertex3f(0.0, 0.0, 0.0);
				glNormal3f(1.0, 0.0, 0.0);
				glVertex3f(0.5, 0.0, 0.0);
			glEnd();
			glLineWidth(1.0);
		glPopMatrix();
		glPushMatrix();
			//MatFBEmerald.Apply();
            glColor3f(ColorGreenAxis.r, ColorGreenAxis.g, ColorGreenAxis.b);
            glLineWidth(3.0);
			glBegin(GL_LINES);
				glNormal3f(0.0, 1.0, 0.0);
				glVertex3f(0.0, 0.0, 0.0);
				glNormal3f(0.0, 1.0, 0.0);
				glVertex3f(0.0, 0.5, 0.0);
			glEnd();
			glLineWidth(1.0);
		glPopMatrix();
		glPushMatrix();
			//MatFBTurquoise.Apply();
            glColor3f(ColorBlueAxis.r, ColorBlueAxis.g, ColorBlueAxis.b);
            glLineWidth(3.0);
			glBegin(GL_LINES);
				glNormal3f(0.0, 0.0, 1.0);
				glVertex3f(0.0, 0.0, 0.0);
				glNormal3f(0.0, 0.0, 1.0);
				glVertex3f(0.0, 0.0, 0.5);
			glEnd();
			glLineWidth(1.0);
		glPopMatrix();

        //MatFBRuby.Apply();
        glColor3f(ColorRubyAxis.r, ColorRubyAxis.g, ColorRubyAxis.b);
        RenderBitmapString(0.58, 0.0, 0.0, GLUT_BITMAP_HELVETICA_12, "x");
        //MatFBEmerald.Apply();
        glColor3f(ColorGreenAxis.r, ColorGreenAxis.g, ColorGreenAxis.b);
        RenderBitmapString(0.0, 0.58, 0.0, GLUT_BITMAP_HELVETICA_12, "y");
        //MatFBTurquoise.Apply();
        glColor3f(ColorBlueAxis.r, ColorBlueAxis.g, ColorBlueAxis.b);
        RenderBitmapString(0.0, 0.0, 0.58, GLUT_BITMAP_HELVETICA_12, "z");

glEnable(GL_LIGHTING);

		glPushMatrix();
			MatFBRuby.Apply();
			glTranslatef(0.5, 0.0, 0.0);
			glRotatef(90.0, 0.0, 1.0, 0.0);
			glutSolidCone(0.0125, 0.05, 10, 10);
		glPopMatrix();
		glPushMatrix();
			MatFBEmerald.Apply();
			glTranslatef(0.0, 0.5, 0.0);
			glRotatef(-90.0, 1.0, 0.0, 0.0);
			glutSolidCone(0.0125, 0.05, 10, 10);
		glPopMatrix();
		glPushMatrix();
			MatFBTurquoise.Apply();
			glTranslatef(0.0, 0.0, 0.5);
			glutSolidCone(0.0125, 0.05, 10, 10);
		glPopMatrix();
	glEndList();

	return TRUE;									
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//										THE OPENGL RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////
int CViewCtrl::updateGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(_axisViewMatrix);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_LINE_SMOOTH);

	// Draw axis
	if (m_showAxis && _dlAxes)
        glCallList(_dlAxes);

#if 0 // Flat shading
    glShadeModel(GL_FLAT);
#endif

	glLoadMatrixf(_viewMatrix);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDepthRange (0.0, 1.0);
	if(_triangMesh)
	{
		//Sergey A. Showtext
		ShowVolumeText(-0.95, -0.80);
		//Sergey A. Draw uncolored part of the mesh
		//MatFBSilver.SetTransparency(1);
		if( (g_branchToColor != 0) && ((g_branchToColor != 1) || (!_dlBurPlane[0])) && ((g_branchToColor != 2) || (!_dlBurPlane[1])) )
		{
			MatFBSilver.Apply();

			//glDisable(GL_DEPTH_TEST);
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			/*if(_pShader)
				_pShader->Enable();*/
			_triangMesh->DrawMesh();
			/*if(_pShader)
				_pShader->Disable();*/

			//glDisable(GL_BLEND);
			//glEnable(GL_DEPTH_TEST);

			//MatFBOrange.Apply();
			//_triangMesh->DrawBranchSeed();
		}
		if(g_branchToColor == 0)
		{
			if(_dlBurPlane[0] && _dlBurPlane[1])
			{
				ColorBranch(2);
				MatFBSilver.Apply();
				_triangMesh->DrawMeshPart(_dlMeshPart2);
			}
		}
		else
		if(g_branchToColor == 1)
		{
			if(_dlBurPlane[0])
			{
				ColorBranch(0);
				MatFBSilver.Apply();
				_triangMesh->DrawMeshPart(_dlMeshPart2);
			}
		}
		else
		if(g_branchToColor == 2)
		{
			if(_dlBurPlane[1])
			{
				ColorBranch(1);
				MatFBSilver.Apply();
				_triangMesh->DrawMeshPart(_dlMeshPart2);
			}
		}
	}

#if DEBUG_CENTER_LINES   // Draw centerlines debug info.
	if(m_showCenterLine)
		if(_centerLine)
			_centerLine->Render();
#endif

#if 0   // Draw wireframe.
    glDisable(GL_LIGHTING);
    glColor4f(1, 0, 1, 1.f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_POLYGON_OFFSET_LINE);
    //glPolygonOffset(-0.5f, 0);
    if(_triangMesh)
        _triangMesh->DrawMesh();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LIGHTING);
    glDisable(GL_POLYGON_OFFSET_LINE);
#endif
	// Alex S.: For critical points algorithm testing. Should be removed later.
//	drawCriticalPoints();

	m_annotation.render(m_showAnnotations || _currToolMode == TM_SEL_LENGTH, m_showAnnotations);

	if(_burPlaneMode && _currSelected.x != Math::MAX_FLOAT)
	{
		//glDepthRange (0, 0.999);

		MatFBRed.Apply();
		glPushMatrix();
		glTranslatef(_currSelected.x, _currSelected.y, _currSelected.z);
		glutSolidSphere(0.005,16,16);
		glPopMatrix();
	}

	for(int i=0; i<2; i++) //_currBurNum
		if(_burPlanePoints[i].size())
		{
			MatFBViolet.Apply();

			vector<Vector3>::iterator pli = _burPlanePoints[i].begin();
			vector<Vector3>::iterator plend = _burPlanePoints[i].end();

			for(; pli != plend; ++pli)
			{
				glPushMatrix();
				glTranslatef(pli->x, pli->y, pli->z);
				glutSolidSphere(0.005,16,16);
				glPopMatrix();
			}
		}

	//draw colored mesh part
	if(_triangMesh && g_branchToColor >= 0)
		{
			CMaterial MatFBBlue__ = CMaterial(
				CColor(0.0, 0.2, 0.8, 0.7), 
				CColor(0.3, 0.3, 0.5, 0.7), 
				CColor(0.2, 0.2, 0.2, 0.7),
				CColor(0.2, 0.2, 0.2, 0.7),
				10.0,
				CColor(0.0, 0.2, 0.8, 0.7), 
				CColor(0.3, 0.3, 0.5, 0.7), 
				CColor(0.2, 0.2, 0.2, 0.7),
				CColor(0.2, 0.2, 0.2, 0.7),
				10.0);
			
			glDepthMask(GL_FALSE);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
			drawBurPlanes();

			// 1st pass
			// render only alpha chanel
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
			glDepthMask(GL_FALSE);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			//glBlendEquation(GL_FUNC_ADD);

			MatFBBlue__.Apply();
			switch(g_branchToColor) 
			{
				case 0:
					//_triangMesh->DrawMesh();
					if(_dlBurPlane[0] && _dlBurPlane[1]) 
						_triangMesh->DrawMeshPart(_dlMeshPart1);
					break;
				case 1:
					if(_dlBurPlane[0]) 
						_triangMesh->DrawMeshPart(_dlMeshPart1);
					break;
				case 2:
					if(_dlBurPlane[1]) 
						_triangMesh->DrawMeshPart(_dlMeshPart1);
					break;
			}

			// z-biasing a little
			glDepthRange (0, 1.0);
			// 2nd pass
			// render only to Z-buffer
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);

			switch(g_branchToColor) 
			{
				case 0:
					//_triangMesh->DrawMesh();
					if(_dlBurPlane[0] && _dlBurPlane[1]) 
						_triangMesh->DrawMeshPart(_dlMeshPart1);
					break;
				case 1:
					if(_dlBurPlane[0]) 
						_triangMesh->DrawMeshPart(_dlMeshPart1);
					break;
				case 2:
					if(_dlBurPlane[1]) 
						_triangMesh->DrawMeshPart(_dlMeshPart1);
					break;
			}

			// z-biasing a little
			glDepthRange (0.0, 0.999999);
			// 3rd pass
			// render volume 
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthFunc(GL_LEQUAL);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
			//glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);

			switch(g_branchToColor) 
			{
				case 0:
					//_triangMesh->DrawMesh();
					if(_dlBurPlane[0] && _dlBurPlane[1]) 
						_triangMesh->DrawMeshPart(_dlMeshPart1);
					break;
				case 1:
					if(_dlBurPlane[0]) 
						_triangMesh->DrawMeshPart(_dlMeshPart1);
					break;
				case 2:
					if(_dlBurPlane[1]) 
						_triangMesh->DrawMeshPart(_dlMeshPart1);
					break;
			}
			
			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);
		}

	//glDepthRange (0.0, 1.0);
	drawBurPlanes();

    // draw tips
    if(_currToolMode == TM_SEL_LENGTH || _currToolMode == TM_TIP_LABELING)
    {
        //drawCriticalPoints();
	    selection.Draw();
    }

	// output to screen
	SwapBuffers(m_hDC);

	return TRUE;
}

// -------------------------------------------------------------------------------------------------
void CViewCtrl::freeGL()
{
	KillTimer(UPDATE_TIMER);

	SAFE_DELETE(_pShader);
	SAFE_DELETE(_pDirectionalLight);

	SAFE_DELETE(_triangMesh);
	SAFE_DELETE(_centerLine);
	//selection.Destroy();
    selection.Clear();

	deleteBurPlanes();

	if (_dlAxes)
	{
		glDeleteLists(_dlAxes, 1);
		_dlAxes = 0;
	}	

	if (m_hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(L"Release Of DC And RC Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(m_hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(L"Release Rendering Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		m_hRC = NULL;
	}

	if (m_hDC && !::ReleaseDC(GetSafeHwnd(),m_hDC))					// Are We Able To Release The DC
	{
		MessageBox(L"Release Device Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_hDC = NULL;										// Set DC To NULL
	}
}

// -------------------------------------------------------------------------------------------------
bool CViewCtrl::linkGLWindow(int width, int height)
{
	m_hDC=::GetDC(GetSafeHwnd());

	GLuint PixelFormat;
	BYTE bits = 24;

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		8,											// 8 bit Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!m_hDC)
	{
		MessageBox(L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;	
	}

	if (!(PixelFormat=ChoosePixelFormat(m_hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		freeGL();										// Reset The Display
		MessageBox(L"Can't Find A Suitable PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;	
	}

	/*DescribePixelFormat(m_hDC, PixelFormat, sizeof(pfd), &pfd);
	if(pfd.cAlphaBits < 8)
		MessageBox(L"Alpha buffer not found.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);*/

	if(!SetPixelFormat(m_hDC,PixelFormat,&pfd))	// Are We Able To Set The Pixel Format?
	{
		freeGL();								// Reset The Display
		MessageBox(L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(m_hRC=wglCreateContext(m_hDC)))		// Are We Able To Get A Rendering Context?
	{
		freeGL();								// Reset The Display
		MessageBox(L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;	
	}

	if(!wglMakeCurrent(m_hDC,m_hRC))			// Try To Activate The Rendering Context
	{
		freeGL();								// Reset The Display
		MessageBox(L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;	
	}

	resizeGL(width, height);

	if (!initGL())
	{
		freeGL();								// Reset The Display
		MessageBox(L"Initialization Failed.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;					
	}

	return TRUE;		
}


// -------------------------------------------------------------------------------------------------
// CViewCtrl::CViewCtrl - Constructor
// -------------------------------------------------------------------------------------------------

CViewCtrl::CViewCtrl()
{
	InitializeIIDs(&IID_DViewControl, &IID_DViewControlEvents);
	// TODO: Initialize your control's instance data here.

	m_compThread = NULL;
	m_hRC = NULL;
	m_hDC = NULL;
	m_hWnd = NULL;

	_pShader = NULL;
	_pDirectionalLight = NULL;
	_triangMesh = NULL;
	_centerLine = NULL;

	m_bPrBarLoaded = false;
	m_NavMode = NM_NONE;
	m_CurCursor = NM_NONE; // default
	
	_burPlaneMode = false;
    _currToolMode = TM_NONE;
	_dlBurPlane[0] = 0;
	_dlBurPlane[1] = 0;

	_fov = 45.0;
	_zNear = 0.1; _zFar = 1500.0;

	m_showCenterLine = false;
	m_showAnnotations = false;
	m_showAxis = true;

	m_updateBval = false;

	g_ViewCtrl = this;

	m_CurFace = 1;
	m_NumOfFaces = 1;

}


// -------------------------------------------------------------------------------------------------
// CViewCtrl::~CViewCtrl - Destructor

CViewCtrl::~CViewCtrl()
{
	// TODO: Cleanup your control's instance data here.
	m_ProgressBar.Detach();
}


// -------------------------------------------------------------------------------------------------
void CViewCtrl::OnDestroy()
{
	freeGL();
}

// -------------------------------------------------------------------------------------------------
// CViewCtrl::OnDraw - Drawing function

void CViewCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	updateGL();
}

// -------------------------------------------------------------------------------------------------
void CViewCtrl::OnSize( UINT, int x, int y )
{
	resizeGL(x,y);
	updateGL();
}

// -------------------------------------------------------------------------------------------------
int CViewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	linkGLWindow(100,100);

	return 0;
}

#ifdef _WIN64
void CViewCtrl::OnTimer(UINT_PTR timer)
{
	if(UPDATE_TIMER == timer)
	{
		//updateGL();
		if(m_CurCursor == NM_WAIT)
			setCursor(NM_WAIT);
	}
}
#else
void CViewCtrl::OnTimer(UINT timer)
{
	if(UPDATE_TIMER == timer)
	{
		//updateGL();
		if(m_CurCursor == NM_WAIT)
			setCursor(NM_WAIT);
	}
}
#endif

BOOL CViewCtrl::OnSetCursor(CWnd* wnd, UINT, UINT)
{
	if(wnd == this)
	{
		setCursor(m_CurCursor);
	}
	return TRUE;
}

void CViewCtrl::setCursor(NavigationModes cursor)
{
	m_CurCursor = cursor;

	if(m_CurCursor == NM_NONE)
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	else
	if(m_CurCursor == NM_ORBIT)
		SetCursor(AfxGetApp()->LoadCursor(IDC_ORBIT));
	else
	if(m_CurCursor == NM_ZOOM)
		SetCursor(AfxGetApp()->LoadCursor(IDC_ZOOM));
	else
	if(m_CurCursor == NM_PAN)
		SetCursor(AfxGetApp()->LoadCursor(IDC_PAN));
	else
	if(m_CurCursor == NM_WAIT)
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
}

//-----------------------------------------------------------------------
Ray CViewCtrl::getCameraToViewportRay(Real screenX, Real screenY) const
{
	/*
	if(m_width)
		screenX = screenX/m_width;
	if(m_width)
		screenY = screenY/m_height;
	Matrix4 inverseVP = (_projectionMatrix * _viewMatrix).Inverse();

	Real nx = (2.0f * screenX) - 1.0f;
	Real ny = 1.0f - (2.0f * screenY);
	Vector3 nearPoint(nx, ny, -1.f);
	// Use midPoint rather than far point to avoid issues with infinite projection
	Vector3 midPoint (nx, ny,  0.0f);

	// Get ray origin and ray target on near plane in world space
	Vector3 rayOrigin, rayTarget;
	
	rayOrigin = inverseVP * nearPoint;
	rayTarget = inverseVP * midPoint;

	Vector3 rayDirection = rayTarget - rayOrigin;
	rayDirection.Normalize();*/

	GLint viewport[4];
	GLdouble modelMatrix[16];
  	GLdouble projMatrix[16];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	GLdouble x,y,z;
	gluUnProject(screenX, m_height - screenY, 0, modelMatrix, projMatrix, viewport, &x, &y, &z);

	Vector3 rayOrigin, rayTarget;
	rayOrigin = _to / 2 * _viewLength + _dir * _rotationMatrix * _viewLength;
	rayTarget = Vector3(x,y,z);

	Vector3 rayDirection = rayTarget - rayOrigin;
	rayDirection.Normalize();

	return Ray(rayOrigin, rayDirection);
} 

void CViewCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	bool bAltState = (GetKeyState(VK_MENU) & 0x80) != 0;
	bool needUpdate = false;

	float dx = (float)(point.x - _lastMousePosition.x) / m_width;
	float dy = (float)(point.y - _lastMousePosition.y) / m_height;

	static float viewLen = 2;

	if(m_NavMode == NM_ORBIT && m_CurCursor != NM_ORBIT)
		setCursor(NM_ORBIT);
	else
	if(m_NavMode == NM_ZOOM && m_CurCursor != NM_ZOOM)
		setCursor(NM_ZOOM);
	else
	if(m_NavMode == NM_PAN && m_CurCursor != NM_PAN)
		setCursor(NM_PAN);

    if ((nFlags & MK_LBUTTON) != 0) 
	{
		if ((nFlags & MK_CONTROL) != 0 || m_NavMode == NM_ORBIT)
		{
			if(m_CurCursor != NM_ORBIT)
				setCursor(NM_ORBIT);

			_angleX += 4.0 * dy;
			if (_angleX > 360.0)
				_angleX -= 360.0;
			_angleY += 4.0 * dx;
			if (_angleY > 360.0)
				_angleY -= 360.0;
			
			needUpdate = true;
		}
		else
		if (m_NavMode == NM_ZOOM)
		{
			float zm = dx + dy;

			_viewLength *= 1.0 - zm;
			while( _viewLength < .05 )
				_viewLength *= 2.0;

			needUpdate = true;
		}
		else
		if (m_NavMode == NM_PAN)
		{	
			_to = _to + Vector3(-dx, dy, 0.0f) * _rotationMatrix;
			needUpdate = true;
		}
		/*else
		if (bAltState) // TODO: rotate around Z
		{
			if(m_CurCursor != NM_ORBIT)
				setCursor(NM_ORBIT);

			_angleX += 4.0 * dy;
			if (_angleX > 360.0)
				_angleX -= 360.0;
			_angleY += 4.0 * dx;
			if (_angleY > 360.0)
				_angleY -= 360.0;

			needUpdate = true;
		}*/
    } 
	else
	if ((nFlags & MK_RBUTTON) != 0)
	{
		if(m_CurCursor != NM_PAN)
			setCursor(NM_PAN);

		_to = _to + Vector3(-dx, dy, 0.0f) * _rotationMatrix;
		needUpdate = true;
	}
	else
	if(m_NavMode == NM_NONE && m_CurCursor != NM_NONE && m_CurCursor != NM_WAIT)
		setCursor(NM_NONE);

    _lastMousePosition = point;

	if(_triangMesh && _triangMesh->_loaded && m_height && m_width && m_NavMode == NM_NONE && _burPlaneMode)
	{
		_currSelected = Vector3(Math::MAX_FLOAT,0,0);
		Ray ray = getCameraToViewportRay(point.x,point.y);

		MeshType::ConstVertexIter vIt(_triangMesh->_mesh.vertices_begin());
		MeshType::ConstVertexIter vEnd(_triangMesh->_mesh.vertices_end());   
		MeshType::ConstVertexFaceIter vfIt;
		MeshType::ConstFaceVertexIter fvIt;

		MeshType::FaceIter fi(_triangMesh->_mesh.faces_begin());
		MeshType::FaceIter fEnd (_triangMesh->_mesh.faces_end());   
		for(; fi != fEnd; ++fi)
			_triangMesh->_normalizedMesh.status(fi).set_tagged(false);
  
		std::pair<bool, Real> result;
		Vector3 clos0, clos1, vec, vecW, worldPos;
		Vector3 v[4];
		float dist = Math::POS_INFINITY;
		float dist2;
		int i = 0;
		for (; vIt!=vEnd; ++vIt)
		{
			vec = *(Vector3*)&(_triangMesh->_normalizedMesh.point(vIt));
            vecW = *(Vector3*)&(_triangMesh->_mesh.point(vIt));
            // Liana: bug #86
            //- when detect left burr plane, I should be able to select points only on deer's left side (my right side)
            //- when detect right burr plane, I should be able to select points only on deer's right side (my left side)
            bool bLeft = ((vecW - _triangMesh->_skullBox.Center).Dot(_triangMesh->_skullBox.Axis[0]) < 0);
          
            // Commented in order to resolve manual burr plane selection issue.
            // If mesh is not aligned than burr plane can be non-selectable.
            //  if(bLeft ^ (_currToolMode == TM_BUR_PLANE_R))
          //      continue;

			dist2 = ray.squaredDistanceTo(vec, clos0, clos1);
			if(dist2 < 0.03f)
			{
				vfIt = _triangMesh->_normalizedMesh.cvf_iter(vIt.handle());
				for(; vfIt; ++vfIt)
					if(!_triangMesh->_normalizedMesh.status(vfIt).tagged())
					{
						_triangMesh->_normalizedMesh.status(vfIt).set_tagged(true);
						fvIt = _triangMesh->_normalizedMesh.cfv_iter(vfIt); 
						v[0] = *(Vector3*)&(_triangMesh->_normalizedMesh.point(fvIt));
						++fvIt;
						v[1] = *(Vector3*)&(_triangMesh->_normalizedMesh.point(fvIt));
						++fvIt;
						v[2] = *(Vector3*)&(_triangMesh->_normalizedMesh.point(fvIt));
						// normal
						v[3] = *(Vector3*)&(_triangMesh->_normalizedMesh.normal(vfIt));

						result = Math::intersects(ray, v[0], v[1], v[2], v[3], true, false);

						if(result.first)
						{
							vec = ray.getOrigin() + ray.getDirection() * result.second;
							dist2 = (ray.getOrigin() - vec).Length();
							if(dist > dist2)
							{
								_currSelected = vec;
								dist = dist2;
							}
						}
					}
			}
		}
	}

	if(needUpdate)
	{
		rebuildView();
        // or update each time in rebuildView(); ???
        if(m_NavMode == NM_ZOOM)
            m_annotation.update();
		updateGL();
	}
	if(_burPlaneMode)
	{
		updateGL();
	}
}

void CViewCtrl::OnKeyDown( UINT nChar, UINT, UINT )
{
	if(nChar == VK_ESCAPE)
	{
		if(m_NavMode != NM_NONE && m_CurCursor != NM_WAIT)
		{
			m_NavMode = NM_NONE;
			setCursor(NM_NONE);
		}
		else
		if(_burPlaneMode)
		{
			_burPlanePoints[_currBurNum].pop_back(); // ESC undo last vertex selection
			updateGL();
		}
		/*else
		{
			m_annotation.removeLast();
		}*/
	}
}

void CViewCtrl::updateAntlerTipMap(void)
{
    for(size_t i = 0 ; i < 2; ++i)
    {
        m_tipMap[i].clear();
        m_tipsE[i].clear();
        for(size_t j = 0 ; j < _centerLine->m_tips[i].size(); ++j)
        {
            if(_centerLine->m_tips[i][j].m_name == "E")
                m_tipsE[i].push_back(&_centerLine->m_tips[i][j]);
            else if(_centerLine->m_tips[i][j].m_name != "")
                m_tipMap[i][_centerLine->m_tips[i][j].m_name] = &_centerLine->m_tips[i][j];
        }
    }
}
void CViewCtrl::updateAnnotations(void)
{
    m_annotation.clear();
    size_t cnt = 0;
    for(int i = 0; i < 2; i++)
    {
        std::vector<AntlerTip>::const_iterator iter, iterE;
        iter = _centerLine->m_tips[i].begin();
        iterE = _centerLine->m_tips[i].end();
        for(iter; iter != iterE; ++iter, ++cnt)
        {
            char strLength[256];
            sprintf_s(strLength, "%.2f in.", iter->m_length);
            m_annotation.addLeader(cnt, iter->m_name, strLength, iter->m_point, 0.05);
        }
    }


	if(_centerLine->m_dims.size() > 1)
	{
		m_annotation.addDimLine("D",
							//_centerLine->m_dims[0].m_distance,
							_centerLine->m_dims[0].m_point[0],
							_centerLine->m_dims[0].m_point[1]);

		Vector3 p[2];
		p[0] = _triangMesh->_worldMatrix.TransformAffine(_centerLine->m_dims[1].m_point[0]);
		p[1] = _triangMesh->_worldMatrix.TransformAffine(_centerLine->m_dims[1].m_point[1]);

		m_annotation.addDimLine("C",
							//_centerLine->m_dims[1].m_distance,
							_centerLine->m_dims[1].m_point[0],
							_centerLine->m_dims[1].m_point[1], 0.40 + (p[1].y - p[0].y));
	}

	if(m_updateBval)
	{
		if(_centerLine->m_dims.size() > 2)
			_centerLine->m_dims.pop_back();
		bool bFind;

		Vector3 p[2];

		for(int i = 0; i < 2; i++)
		{
			bFind = false;

			if(m_tipMap[i].find("F") != m_tipMap[i].end())
			{
				p[i] = m_tipMap[i]["F"]->m_branch->m_points[0];
				bFind = true;
			}

			if(!bFind)
				break;
		}

		if(bFind)
		{
			float len = (p[1] - p[0]).Length();
			len /= 25.4; // convert to inches

			len = ROUND2(len);
			//p[0] = _triangMesh->_worldMatrix.TransformAffine(p[0]);
			//p[1] = _triangMesh->_worldMatrix.TransformAffine(p[1]);
			_centerLine->m_dims.push_back(DimUnit(p[0], p[1], len));
		}

		m_updateBval = false;
	}

	if(_centerLine->m_dims.size() > 2)
		m_annotation.addDimLine("B",
							//_centerLine->m_dims[2].m_distance,
							_centerLine->m_dims[2].m_point[0],
							_centerLine->m_dims[2].m_point[1]);

    m_annotation.showAnnotations(m_showAnnotations);
}

void CViewCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	SetCapture();

	if(_triangMesh && _triangMesh->_loaded && m_height && m_width &&
		(GetKeyState(VK_MENU) & 0x80) == 0 &&
		(nFlags & MK_CONTROL) == 0 &&
		(nFlags & MK_SHIFT) == 0 &&	m_NavMode == NM_NONE)
	{
		if(_burPlaneMode && _currSelected.x != Math::MAX_FLOAT)
		{
			vector<Vector3>::iterator pli = _burPlanePoints[_currBurNum].begin();
			vector<Vector3>::iterator plend = _burPlanePoints[_currBurNum].end();

			for(; pli != plend; ++pli)
				if(pli->PositionEquals(_currSelected))
				{
					MessageBox(L"This vertex was already selected, select another one.", L"Antlers", 0);
					return;
				}

			_burPlanePoints[_currBurNum].push_back(_currSelected);
			_burPlanePointsObject[_currBurNum].push_back(_currSelectedObject);
			if(_burPlanePoints[_currBurNum].size() == 3)
			{
				//MessageBox(L"This vertex was already selected, select another one.", L"Antlers", 0);
				_burPlaneMode = false;
                _currToolMode = TM_NONE;
				updateBurPlane(_currBurNum);
				_triangMesh->ComputeBranchVolume(_currBurNum, _burPlane[_currBurNum]);
			}
		}
        else if(_currToolMode == TM_SEL_LENGTH)
        {
        //    LPRECT rect;
        //    this->GetClientRect(rect);
        //    selection.CheckObjectID(point.x,point.y,rect);
            int selObj = selection.IntersectObjects(getCameraToViewportRay(point.x,point.y));
            if(selObj != -1)
                m_annotation.showLength(true, selObj);
        }
        else if(_currToolMode == TM_TIP_LABELING)
        {
            {
                int selObj = selection.IntersectObjects(getCameraToViewportRay(point.x,point.y));
                if(selObj != -1)
                {
                    //TODO: rename 
                    //1. get new name for the tip

                    std::wstring nameListW[21] = {L"E", L"F", L"G1", L"G2", L"G3", L"G4", 
                        L"G5", L"G6", L"G7", L"G8", L"G9",
                        L"G10", L"G11", L"G12", L"G13", L"G14",
                        L"G15", L"G16", L"G17", L"G18", L"G19" };

                    int newName = -1;

                    POINT pt;
                    GetCursorPos(&pt);
                    HMENU hMenu = CreatePopupMenu();
                    if(hMenu)
                    {
                        HWND hWnd = GetSafeHwnd();
                        EnableWindow(FALSE);

                        size_t n_left = _centerLine->m_annotations[0].size();
                        size_t n_right = _centerLine->m_annotations[1].size();

                        size_t nStart = ( selObj < n_left ? 0 : n_left);
                        size_t iBranch = ( selObj < n_left ? 0 : 1);

                        size_t n_antlers = iBranch == 0? n_left: n_right;
                        n_antlers += 1;
                        for(int i = 0 ; i < n_antlers; ++i)
                        {
                            InsertMenu(hMenu, (UINT)-1, (UINT)MF_BYPOSITION|MF_ENABLED, ANTLERS_POPUP_ACTION_BEGIN+i, nameListW[i].c_str());
                        }
                        // note:    must set window to the foreground or the
                        //          menu won't disappear when it should

                        SetForegroundWindow();
                        UINT returnId = TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hWnd, NULL);
                        
                        DestroyMenu(hMenu);
                        EnableWindow(TRUE);

                        ::PostMessage(hWnd, WM_NULL, 0, 0);                        
                        InvalidateRgn(NULL, TRUE);

                        if(returnId!=0)
                        {
                            newName = returnId - ANTLERS_POPUP_ACTION_BEGIN;
                            AntlersDebugger::logMessage("Selected name is %d ", newName);
                            // resort annotation
                           size_t nOldName = selObj - nStart;

                            if(_centerLine->m_tips[iBranch][nOldName].m_name != AntlerTip::nameList[newName])
                            {
                                if(AntlerTip::nameList[newName] == "F" || _centerLine->m_tips[iBranch][nOldName].m_name == "F")
                                {
                                    if(MessageBox(L"Do you really want to change position of F tip?\n Note: All lengths will be recomputed or set to zero.", 
                                        L"Antlers",MB_YESNO) == IDNO)
                                    {
                                        updateGL();
                                        return;
                                    }
                                }

                                std::string oldName = _centerLine->m_tips[iBranch][nOldName].m_name;
                                _centerLine->m_tips[iBranch][nOldName].m_name = AntlerTip::nameList[newName];
                                if(m_tipMap[iBranch].find(AntlerTip::nameList[newName]) 
                                    != m_tipMap[iBranch].end())
                                {
                                    m_tipMap[iBranch][AntlerTip::nameList[newName]]->m_name = "";
                                }

                                if(AntlerTip::nameList[newName] == "F" 
                                    || oldName == "F")
                                {
                                    AntlersDebugger::logMessage("Recomputing lengths");
                                    //TODO: update tree hierarchy and length values
                                    m_updateBval = true;
                                    if(AntlerTip::nameList[newName] == "F")
                                    {
                                        Branch* mainBeam[2] = { NULL, NULL };
                                        mainBeam[iBranch] = _centerLine->m_tips[iBranch][nOldName].m_branch;
                                        
                                        if(m_tipMap[1-iBranch].find("F") != m_tipMap[1-iBranch].end())
                                            mainBeam[1-iBranch] = m_tipMap[1-iBranch]["F"]->m_branch;

                                        _centerLine->updateBranchLength(mainBeam[0], mainBeam[1], false);
                                    }
                                    else
                                    {
                                        for(size_t i = 0; i < _centerLine->m_tips[iBranch].size(); ++i)
                                            _centerLine->m_tips[iBranch][i].m_length = 0;
                                    }
                                }

                                updateAntlerTipMap();
                                updateAnnotations();

                                selection.Init(_centerLine);
                            }
                        }
                    }
                }
            }
        }
		else
		{
			/*static FILE* f = NULL;
			if(!f)
				fopen_s(&f, "Annot.txt", "w");

			_dimPoints.push_back(_currSelected);
			if(m_annotation.size()<3)
			{
				if(_dimPoints.size() == 2)
				{
					if(m_annotation.size() == 2)
						m_annotation.addDimLine("127.344", _dimPoints[0], _dimPoints[1],  
						(_triangMesh->_ymax - _triangMesh->_ymin) * _triangMesh->_ratio * 0.37 - _dimPoints[0].y);
					else
						m_annotation.addDimLine("127.344", _dimPoints[0], _dimPoints[1]);

					fprintf(f, "%lg %lg %lg\n", _dimPoints[0].x, _dimPoints[0].y,_dimPoints[0].z);
					fprintf(f, "%lg %lg %lg\n", _dimPoints[1].x, _dimPoints[1].y,_dimPoints[1].z);
					fprintf(f, "%lg\n\n", (_triangMesh->_ymax - _triangMesh->_ymin) * _triangMesh->_ratio * 0.37 - _dimPoints[0].y);

					_dimPoints.clear();
				}
			}
			else
			{
				m_annotation.addLeader("G6", _dimPoints[0], 0.05);
				fprintf(f, "%lg %lg %lg\n", _dimPoints[0].x, _dimPoints[0].y,_dimPoints[0].z);
				_dimPoints.clear();
			}*/
		}
		updateGL();
	}
}

void CViewCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	_lastMousePosition = point;
	ReleaseCapture();
}
typedef OpenMesh::VPropHandleT<float> OM_Property;
extern void ComputeCurvature(MeshType* mesh, OM_Property& gausCurvP, OM_Property&meanCurvP);

void CViewCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
    /*
	_triangMesh->Smooth();

    OM_Property gausCurvP, meanCurvP;
    ComputeCurvature(&_triangMesh->_mesh, gausCurvP, meanCurvP);

    updateGL();
*/
	SetCapture();
}

void CViewCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
	_lastMousePosition = point;
	ReleaseCapture();
}

BOOL CViewCtrl::OnMouseWheel( UINT, short zDelta, CPoint )
{
	if (zDelta > 0)
		_viewLength *= 1.0 - _zoomFactor;
	else
		_viewLength *= 1.0 + _zoomFactor;

	while( _viewLength < .05 )
		_viewLength *= 2.0;

	rebuildView();
    m_annotation.update();

	updateGL();

	return TRUE;
}

BOOL CViewCtrl::loadAntler(LPCTSTR fileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_NavMode = NM_NONE;
	if(m_CurCursor != NM_NONE)
		setCursor(NM_NONE);

    _currToolMode = TM_NONE;
	_burPlaneMode = false;
	_burPlanePoints[0].clear();
	_burPlanePoints[1].clear();
	_currSelected = Vector3(0,0,0);
	deleteBurPlanes();
	m_annotation.clear();

	SAFE_DELETE(_centerLine);

    selection.Clear();

	m_showAnnotations = false;
	m_showCenterLine = false;

	std::wostringstream str;

	DWORD triagtime = GetTickCount();
	if(!_triangMesh)
		_triangMesh = new CTriMesh();
	bool res = _triangMesh->Load(fileName, m_NumOfFaces);
	triagtime = GetTickCount() - triagtime;

	// check holes
	float boundaryHoleSize = 0.0f;
	_triangMesh->CheckHoles(boundaryHoleSize);

	const float holeMaxSize = 8.0f;
	if(boundaryHoleSize > holeMaxSize)
	{
		MessageBox(L"Unable to automatically fill holes in the model.", L"Wrong file!", 0);
		return false;
	}

	if(boundaryHoleSize > 0.0f)
	{
		_triangMesh->FixHoles();
		MessageBox(L" All holes in the model were filled.", L"Success!", 0);
	}
	
	DWORD voltime = GetTickCount();
	_triangMesh->ComputeVolume();
	g_branchToColor = -1;

	voltime = GetTickCount() - voltime;

	if (res)
	{
		str<<"Loading mesh: "<<triagtime<<" ms."
			<<"\nCalculating volume: "<<voltime<<" ms."
			<<"\nFaces: "<<_triangMesh->_mesh.n_faces()
			<<"\nVolume: "<<_triangMesh->_volume;
		//MessageBox(str.str().c_str(),L"Some",0);

		// calculating normal view matrix
		Matrix4 normView = Matrix4::IDENTITY;
		DWORD normtime = GetTickCount();
		calcNormalizedView(_triangMesh, normView);
		normtime = GetTickCount() - normtime;
		std::wostringstream str_n;
		str_n<<"Calculating normalized view: "<<normtime<<" ms.";
		//MessageBox(str_n.str().c_str(),L"Some",0);

		_triangMesh->SetNormalMatrix(normView);	
		_triangMesh->GenerateBranchSeed();

		resetView();
		updateGL();

        _normMatrixAuto = _triangMesh->_normalizedView;

		return true;
	}
	else
	{
		MessageBox(L"Antlers - Exception occured", L"Wrong file!", 0);
		return false;
	}
}

void CViewCtrl::setProgressBarHandle(ULONG handle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(handle)
	{
		m_ProgressBar.Attach((HWND)handle);
		if(m_ProgressBar.GetSafeHwnd())
		{
			m_bPrBarLoaded = true;
			m_ProgressBar.SetRange(0,100);
			m_ProgressBar.SetStep(1);
		}
		else
			m_bPrBarLoaded = false;
	}
}

SHORT CViewCtrl::showVolume(ULONG num)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(num > 2 || !_triangMesh)
	{
		g_branchToColor = -1;
		return -1;
	}

	if(g_branchToColor == num) 
		g_branchToColor = -1;
	else 
		g_branchToColor = num;

	/*NeedToColorBranch[0] = false; NeedToColorBranch[1] = false; NeedToColorBranch[2] = false;
	NeedToColorBranch[num] = true;*/
	//rebuildView();
	updateGL();
	/*MessageBox(str.str().c_str(),L"Antlers",0);
	NeedToColorBranch[0] = false; NeedToColorBranch[1] = false; NeedToColorBranch[2] = false;
	updateGL();*/

	return g_branchToColor;
}

SHORT CViewCtrl::planeDetectionMode()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
/*
    if(_burPlaneMode)
	{
		return _currBurNum;
	}
	else
		return -1;
*/
    if(_currToolMode == TM_BUR_PLANE_L)
        return 0;
    if(_currToolMode == TM_BUR_PLANE_R)
        return 1;
    else
        return -1;

}

SHORT CViewCtrl::getToolMode()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
    return (SHORT)_currToolMode;
}

BOOL CViewCtrl::enterAntlerPlane(ULONG num)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(num<0 || num>1)
		return false; // invalid index;

	if(_burPlaneMode && num == _currBurNum)
		if(MessageBox(L"Do you really want to quit burr plane detection mode?",L"Antlers",MB_YESNO) == IDYES)
		{
			_burPlanePoints[num].clear();
			_burPlaneMode = false;
            _currToolMode = TM_NONE;
			updateGL();
			return false;
		}
		else
			return true;

    // clear selection
    if(_currToolMode == TM_SEL_LENGTH){
        m_annotation.clearLengthInfo();
    }

	_currBurNum = num;
	m_NavMode = NM_NONE;
	if(m_CurCursor != NM_NONE)
		setCursor(NM_NONE);

	for(int i=0; i<2; i++)
		if(!_dlBurPlane[i] && !_burPlanePoints[i].empty())
			_burPlanePoints[i].clear();

	if (_dlBurPlane[_currBurNum])
		if(MessageBox(L"The burr plane is already detected.\nDo you want to redetect it and lose previous result?",L"Antlers",MB_YESNO) == IDNO)
		{
			_burPlaneMode = false;
			return false;
		}

	_burPlaneMode = true;
    _currToolMode = _currBurNum ? TM_BUR_PLANE_R : TM_BUR_PLANE_L;

	_burPlanePointsObject[_currBurNum].clear();
	_burPlanePoints[_currBurNum].clear();
	_currSelected = Vector3(Math::MAX_FLOAT,0,0);
	deleteBurPlane(_currBurNum);

#if FOR_INTERNAL_TESTING == 0
    selection.Clear();
	m_annotation.clear();
	m_showCenterLine = false;
    m_showAnnotations = false;
	SAFE_DELETE(_centerLine);
#endif

	return true;
}

BOOL CViewCtrl::enterAntlerAutoPlanes()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(_currToolMode == TM_BUR_PLANE_A){
        return false; // Should not enter here !
    }

    //deselect previous selected mode !
    //TODO: ideally should be some common interface for all modes
    if(_currToolMode == TM_BUR_PLANE_L || _currToolMode == TM_BUR_PLANE_R){
        if(enterAntlerPlane(_currToolMode == TM_BUR_PLANE_L ? 0 : 1))
            return false;
    }
  
    if(_dlBurPlane[0] || _dlBurPlane[1]){
        if(MessageBox(L"The burr plane is already detected.\nDo you want to redetect it and lose previous result?",L"Antlers",MB_YESNO) == IDNO)
        {
            return false;
        }
    }
   
    _burPlanePoints[0].clear();
    _burPlanePointsObject[0].clear();

    _burPlanePoints[1].clear();
    _burPlanePointsObject[1].clear();

    // clear selection
    if(_currToolMode == TM_SEL_LENGTH){
        m_annotation.clearLengthInfo();
    }

    m_NavMode = NM_NONE;
    if(m_CurCursor != NM_WAIT)
        setCursor(NM_WAIT);

    _currToolMode = TM_BUR_PLANE_A;

#if FOR_INTERNAL_TESTING == 0
    selection.Clear();
    m_annotation.clear();
    m_showCenterLine = false;
    m_showAnnotations = false;
    SAFE_DELETE(_centerLine);
#endif

    //TODO: Compute burr plane here or call the thread

    BurrPlaneFinder bpf(_triangMesh);
    if(!bpf.FindBurrPlanes())
        MessageBox(L"Could not find burr planes.\nPlease detect burr planes manually", L"Antlers");

    Vector3 norLeft, norRight, centerLeft, centerRight;
    norLeft = Vector3(bpf.GetBurrPlanes()[0].normal[0], bpf.GetBurrPlanes()[0].normal[1], bpf.GetBurrPlanes()[0].normal[2]);
    norRight = Vector3(bpf.GetBurrPlanes()[1].normal[0], bpf.GetBurrPlanes()[1].normal[1], bpf.GetBurrPlanes()[1].normal[2]);
    centerLeft = Vector3(bpf.GetBurrPlanes()[0].center[0], bpf.GetBurrPlanes()[0].center[1], bpf.GetBurrPlanes()[0].center[2]);
    centerRight = Vector3(bpf.GetBurrPlanes()[1].center[0], bpf.GetBurrPlanes()[1].center[1], bpf.GetBurrPlanes()[1].center[2]);
    
	// swap 0 with 1 user left must be deer right etc
    _burPlane[1] = Plane(norLeft, centerLeft);
    _burPlane[0] = Plane(norRight, centerRight);

    _burCenter[1] = centerLeft;
    _burCenter[0] = centerRight;

    _burrRingRadius[1] = bpf.GetBurrPlanes()[0].radius*_triangMesh->_ratio;
    _burrRingRadius[0] = bpf.GetBurrPlanes()[1].radius*_triangMesh->_ratio;
    
    Matrix4 fromWorldToLocal = _triangMesh->_worldMatrix;
    _burCenterWorld[1] = fromWorldToLocal.TransformAffine(_burCenter[1]);
    _burCenterWorld[0] = fromWorldToLocal.TransformAffine(_burCenter[0]);

    _burPlaneWorld[1] = Plane(_triangMesh->_rotationMatrix*norLeft, _burCenterWorld[1]);
    _burPlaneWorld[0] = Plane(_triangMesh->_rotationMatrix*norRight, _burCenterWorld[0]);

    updateBurPlaneCallList(0);
    updateBurPlaneCallList(1);

	_triangMesh->ComputeBranchVolume(0, _burPlane[0]);
	_triangMesh->ComputeBranchVolume(1, _burPlane[1]);

    _currToolMode = TM_NONE;

    setCursor(NM_NONE);
    return true;
}

void CViewCtrl::setLoadingProgress(int value)
{
	if(m_bPrBarLoaded && m_ProgressBar.GetPos() != value)
	{
		m_ProgressBar.SetPos(value);
	}
}

void CViewCtrl::setLoadingFacesCur(int value)
{
	m_CurFace = value;
	setLoadingProgress(m_CurFace * 100.0f / m_NumOfFaces);
}

void CViewCtrl::normalizeView(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	resetView();
}
void CViewCtrl::normalizedViewRestore(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Matrix4 normView = _normMatrixAuto;
    ::setNormalizedView(_triangMesh, normView);

    Matrix4 oldWorldMat = _triangMesh->_worldMatrix;
    Matrix3 oldRotMat   = _triangMesh->_rotationMatrix;

    _triangMesh->SetNormalMatrix(normView);	
    //    _triangMesh->GenerateBranchSeed();

    resetView();
    updateGL();

    //TODO: replace all world variables

    Matrix4 trMatrix = _triangMesh->_worldMatrix*(oldWorldMat.Inverse());
    Matrix3 trVecMatrix = _triangMesh->_rotationMatrix*(oldRotMat.Inverse());

    _currSelected = Vector3(Math::MAX_FLOAT,0,0);

    for(int i = 0; i < 2; ++i)
    {
        for(size_t j = 0 ; j < _burPlanePoints[i].size(); ++j)
            _burPlanePoints[i][j] = trMatrix*_burPlanePoints[i][j];
    }
    _burCenterWorld[0] = trMatrix*_burCenterWorld[0];
    _burCenterWorld[1] = trMatrix*_burCenterWorld[1];

    _burPlaneWorld[1] = Plane(trVecMatrix*_burPlaneWorld[1].Normal, _burCenterWorld[1]);
    _burPlaneWorld[0] = Plane(trVecMatrix*_burPlaneWorld[0].Normal, _burCenterWorld[0]);

    if(_dlBurPlane[0])
        updateBurPlaneCallList(0);
    if(_dlBurPlane[1])
        updateBurPlaneCallList(1);
}
void CViewCtrl::setNormalizedView(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(MessageBox(L"Do you really want to set current view as normalized view ? ", L"Antlers", MB_YESNO) == IDNO)
        return;

    Matrix4 normView =  _viewMatrix.Transpose()*_triangMesh->_worldMatrix;
    normView.Inverse();
    ::setNormalizedView(_triangMesh, normView);

    Matrix4 oldWorldMat = _triangMesh->_worldMatrix;
    Matrix3 oldRotMat   = _triangMesh->_rotationMatrix;

    _triangMesh->SetNormalMatrix(normView);	
//    _triangMesh->GenerateBranchSeed();

    resetView();
    updateGL();
    
    //TODO: replace all world variables

    Matrix4 trMatrix = _triangMesh->_worldMatrix*(oldWorldMat.Inverse());
    Matrix3 trVecMatrix = _triangMesh->_rotationMatrix*(oldRotMat.Inverse());
    
    _currSelected = Vector3(Math::MAX_FLOAT,0,0);

    for(int i = 0; i < 2; ++i)
    {
        for(size_t j = 0 ; j < _burPlanePoints[i].size(); ++j)
           _burPlanePoints[i][j] = trMatrix*_burPlanePoints[i][j];
    }
    _burCenterWorld[0] = trMatrix*_burCenterWorld[0];
    _burCenterWorld[1] = trMatrix*_burCenterWorld[1];

    _burPlaneWorld[1] = Plane(trVecMatrix*_burPlaneWorld[1].Normal, _burCenterWorld[1]);
    _burPlaneWorld[0] = Plane(trVecMatrix*_burPlaneWorld[0].Normal, _burCenterWorld[0]);

    if(_dlBurPlane[0])
        updateBurPlaneCallList(0);
    if(_dlBurPlane[1])
        updateBurPlaneCallList(1);
}

void CViewCtrl::exportToPDF(LPCTSTR fileName, ULONG type)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ExportToPDF(fileName, type);
}

void CViewCtrl::viewScoreSheet(ULONG type)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	m_scoreWnd.updateScore(type);
	if(!m_scoreWnd.GetSafeHwnd())
		m_scoreWnd.CreateEx(0,
						0, L"Scoresheet",
						WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
						WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
						100, 100, 8.5 * 72, 11 * 72, NULL, 0);

	m_scoreWnd.CenterWindow();
	m_scoreWnd.ShowWindow(SW_SHOW);
}

void CViewCtrl::deleteBurPlanes()
{
	for(int i=0; i<2; i++)
		if (_dlBurPlane[i])
		{
			glDeleteLists(_dlBurPlane[i], 1);
			_dlBurPlane[i] = 0;
		}
	//NeedToColorBranch[1] = false; NeedToColorBranch[2] = false;
}

void CViewCtrl::drawBurPlanes()
{
	for(int i=0; i<2; i++)
		if (_dlBurPlane[i])
			glCallList(_dlBurPlane[i]);
}

void CViewCtrl::deleteBurPlane(int num, bool bUpdateScene/*=true*/)
{
	if (_dlBurPlane[num])
	{
		glDeleteLists(_dlBurPlane[num], 1);
		_dlBurPlane[num] = 0;
	}
	//NeedToColorBranch[num + 1] = false;
    if(bUpdateScene)
	    updateGL();
}
bool CViewCtrl::updateBurPlaneCallList(int num)
{
    deleteBurPlane(num, false);

    _dlBurPlane[num] = glGenLists(1);
    if (!_dlBurPlane[num])
        return false;

    Matrix4 mat;
    mat.MakeIdentity();
    
    Quaternion rot = _burPlaneWorld[num].Normal.GetRotationTo(Vector3::UNIT_Z);

    Matrix3 rot3x3 (Matrix3::IDENTITY);
    rot.ToRotationMatrix(rot3x3);

    mat = rot3x3.Transpose();
    mat.SetTranslation(_burCenterWorld[num].x, _burCenterWorld[num].y, _burCenterWorld[num].z);

    float step = 2*_burrRingRadius[num];

    glNewList(_dlBurPlane[num], GL_COMPILE);
        glPushMatrix();

        glDisable(GL_LIGHTING);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   
        //MatFBOrangeNew.Apply();
        glColor4f(burPlaneColor.r, burPlaneColor.g, burPlaneColor.b, burPlaneColor.a);

        glMultMatrixf(mat.Transpose());

        glBegin(GL_QUADS);
            glVertex3f(-step, -step, 0);
            glVertex3f(-step, step, 0);
            glVertex3f(step, step, 0);
            glVertex3f(step, -step, 0);
        glEnd();

        glDisable(GL_BLEND);
        glPopMatrix();

        glEnable(GL_LIGHTING);

    glEndList();

    updateGL();
    return true;
}
bool CViewCtrl::updateBurPlane(int num)
{
//	deleteBurPlane(num);

	Vector3 _invP[3];
	{
		Matrix3 rot3x3 = Matrix3::IDENTITY;
		rot3x3.FromEulerAnglesXYZ(_triangMesh->_angleX * Math::DEG_TO_RAD, 
			_triangMesh->_angleY * Math::DEG_TO_RAD, 
			_triangMesh->_angleZ * Math::DEG_TO_RAD);
		rot3x3 *= 1.f/_triangMesh->_ratio;

		Matrix4 resInv;
		resInv = Matrix4::IDENTITY;
		resInv = rot3x3.Transpose();
		resInv.SetTranslation(-_triangMesh->_translation);

		_invP[0] = resInv * _burPlanePoints[num][0];
		_invP[1] = resInv * _burPlanePoints[num][1];
		_invP[2] = resInv * _burPlanePoints[num][2];
	}

	// in world space
	_burPlane[num].Redefine(_burPlanePoints[num][0],
							_burPlanePoints[num][1],
							_burPlanePoints[num][2]);
    
    _burPlaneWorld[num] = _burPlane[num];

	Vector3 centerPoint = _burPlanePoints[num][0] + _burPlanePoints[num][1] + _burPlanePoints[num][2];
	centerPoint /= 3;

	float fLen = max(max((centerPoint - _burPlanePoints[num][0]).Length(),
						 (centerPoint - _burPlanePoints[num][1]).Length()),
						 (centerPoint - _burPlanePoints[num][2]).Length());

    _burrRingRadius[num] = fLen;

	// in object space
	_burPlane[num].Redefine(_invP[0], _invP[1],	_invP[2]);
	_burCenter[num] = _invP[0] + _invP[1] + _invP[2];
	_burCenter[num] /= 3;

    _burCenterWorld[num] = (_burPlanePoints[num][0] + _burPlanePoints[num][1] + _burPlanePoints[num][2])/3.0;
	
    return updateBurPlaneCallList(num);
}

//Sergey A. For coloring of part involved into volume computation
void CViewCtrl::ColorBranch(int iNum)
{
    if (!_triangMesh || !_triangMesh->_loaded){
        return;
    }

	vector<MeshType::FaceHandle>	WholeTri, WholeTri1, //list of triangles that are not intersectied by plane, or lying on plane
									SplitTri; //these are intersected by plane, or edge on plane
	MeshType::FaceVertexIter fv_it;
	OpenMesh::Vec3f v3f;
	Vector3 v3;
	bool updatePlane = false;
		
	WholeTri.reserve(_triangMesh->_mesh.n_faces());
	WholeTri1.reserve(_triangMesh->_mesh.n_faces());
	SplitTri.reserve(_triangMesh->_mesh.n_faces());

	if(lastColoredPart != iNum)
		updatePlane = true;
	else
	if(iNum < 2 && (!_burPlane[iNum].Normal.PositionEquals(oldBurplane[iNum].Normal) || 
			!Math::RealEqual(_burPlane[iNum].D, oldBurplane[iNum].D)))
		updatePlane = true;
	else
	if(iNum == 2)
	if(!_burPlane[0].Normal.PositionEquals(oldBurplane[0].Normal) || 
			!Math::RealEqual(_burPlane[0].D, oldBurplane[0].D) || 
			!_burPlane[1].Normal.PositionEquals(oldBurplane[1].Normal) || 
			!Math::RealEqual(_burPlane[1].D, oldBurplane[1].D))
		updatePlane = true;
	
	if(updatePlane)
	{
		WholeTri.clear();
		SplitTri.clear();

		if(iNum == 0 || iNum == 2)
		{
			MeshVolume::FloodFill_(&_triangMesh->_mesh, _triangMesh->_branchSeed[0], WholeTri, SplitTri, OpenMesh::Vec4f(_burPlane[0][0], _burPlane[0][1], _burPlane[0][2], 0.0f));
			if(WholeTri.size() < 1) 
				MeshVolume::FloodFill_(&_triangMesh->_mesh, _triangMesh->_branchSeed[0], WholeTri, SplitTri, -OpenMesh::Vec4f(_burPlane[0][0], _burPlane[0][1], _burPlane[0][2], 0.0f));

			oldBurplane[0] = _burPlane[0];
		}

		if(iNum == 1 || iNum == 2)
		{
			size_t sz = WholeTri.size();
			MeshVolume::FloodFill_(&_triangMesh->_mesh, _triangMesh->_branchSeed[1], WholeTri, SplitTri, OpenMesh::Vec4f(_burPlane[1][0], _burPlane[1][1], _burPlane[1][2], 0.0f));
			if(WholeTri.size() <= sz) 
				MeshVolume::FloodFill_(&_triangMesh->_mesh, _triangMesh->_branchSeed[1], WholeTri, SplitTri, -OpenMesh::Vec4f(_burPlane[1][0], _burPlane[1][1], _burPlane[1][2], 0.0f));

			oldBurplane[1] = _burPlane[1];
		}

		lastColoredPart = iNum;
		
		MeshType::FaceIter it_end = _triangMesh->_mesh.faces_end();
		for (MeshType::FaceIter i = _triangMesh->_mesh.faces_begin(); i != it_end; ++i)
			_triangMesh->_mesh.status(i).set_tagged(false);
		for (size_t i = 0; i < WholeTri.size(); ++i)
			_triangMesh->_mesh.status(WholeTri[i]).set_tagged(true);
		WholeTri1.clear();
		for (MeshType::FaceIter i = _triangMesh->_mesh.faces_begin(); i != it_end; ++i)
			if(_triangMesh->_mesh.status(i).tagged() == false) WholeTri1.push_back(i);
	}
		
	if(updatePlane)
	{
	if (_dlMeshPart1)
	{
		glDeleteLists(_dlMeshPart1, 2);
		_dlMeshPart1 = 0;
	}
	_dlMeshPart1 = glGenLists(2);
	glNewList(_dlMeshPart1, GL_COMPILE);
		glPushMatrix();
			MeshType::ConstFaceIter		fIt(_triangMesh->_mesh.faces_begin()), 
										fEnd(_triangMesh->_mesh.faces_end());

			MeshType::ConstFaceVertexIter fvIt;
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, _triangMesh->_mesh.points());

			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, _triangMesh->_mesh.vertex_normals());

			glBegin(GL_TRIANGLES);

				for (size_t i = 0; i < WholeTri.size(); ++i)
				{
				  fvIt = _triangMesh->_mesh.cfv_iter(WholeTri[i]); 
				  glArrayElement(fvIt.handle().idx());
				  ++fvIt;
				  glArrayElement(fvIt.handle().idx());
				  ++fvIt;
				  glArrayElement(fvIt.handle().idx());
				}

			glEnd();

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
		glPopMatrix();
	glEndList();
	}

	if(updatePlane)
	{
	if (_dlMeshPart2)
	{
		glDeleteLists(_dlMeshPart2, 2);
		_dlMeshPart2 = 0;
	}
	_dlMeshPart2 = glGenLists(2);
	glNewList(_dlMeshPart2, GL_COMPILE);
		glPushMatrix();
			MeshType::ConstFaceIter		fIt(_triangMesh->_mesh.faces_begin()), 
										fEnd(_triangMesh->_mesh.faces_end());

			MeshType::ConstFaceVertexIter fvIt;
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, _triangMesh->_mesh.points());

			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, _triangMesh->_mesh.vertex_normals());

			glBegin(GL_TRIANGLES);

				for (size_t i = 0; i < WholeTri1.size(); ++i)
				{
				  fvIt = _triangMesh->_mesh.cfv_iter(WholeTri1[i]); 
				  glArrayElement(fvIt.handle().idx());
				  ++fvIt;
				  glArrayElement(fvIt.handle().idx());
				  ++fvIt;
				  glArrayElement(fvIt.handle().idx());
				}

			glEnd();

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
		glPopMatrix();
	glEndList();
	}

	//Draw Text
}

void CViewCtrl::ShowVolumeText(float xpos, float ypos)
{
	if (!_triangMesh || !_triangMesh->_loaded || g_branchToColor < 0)
        return;


	std::ostringstream str;
	str.setf(ios::fixed);
	str.precision(2);
	double vol;
	int iNum = g_branchToColor - 1;
	if(iNum < -1 || iNum > 1)
		return;

	wstring prefixW[2] = {L"Left", L"Right"};
	string prefixA[2] = {"Left", "Right"};

	if(iNum == -1)
	{
		if(!_dlBurPlane[0] || !_dlBurPlane[1])
		{
			g_branchToColor = -1;
			MessageBox(L"Both burr planes must be detected before computing rack volume.",L"Antlers",0);
			return;
		}
		else
		{		
			vol = _triangMesh->_branchVolume[0] + _triangMesh->_branchVolume[1];
			if( _triangMesh->_branchVolume[0] > 0 && _triangMesh->_branchVolume[1] > 0)
				//str<<"Rack volume: "<<vol<<" cubic millimeters";
				str<<"Rack volume: "<<vol<<" cubic inches";
			else
			{
				g_branchToColor = -1;
				MessageBox(L"Rack volume can't be computed because the mesh isn't closed.",L"Antlers",0);
				return;
			}
		}
	}
	else
	{
		if(!_dlBurPlane[iNum])
		{
			g_branchToColor = -1;
			MessageBox((prefixW[iNum] + L" burr plane must be detected before the volume can be computed.").c_str(),L"Antlers",0);
			return;
		}
		else
		{		
			vol = _triangMesh->_branchVolume[iNum];
			if(vol >= 0)
				//str<<prefixA[iNum] + " branch volume: "<<vol<<" cubic millimeters";
				str<<prefixA[iNum] + " branch volume: "<<vol<<" cubic inches";
			else
			{
				g_branchToColor = -1;
				MessageBox((prefixW[iNum] + L" branch volume can't be computed because the mesh isn't closed.").c_str(),L"Antlers",0);
				return;
			}
		}
	}
	
	glDisable(GL_LIGHTING);
	//glColor4f(0.8, 0.95, 0.85, 0.5f);
	glColor4f(1, 1, 1, 1);
	RenderBitmapString2D(xpos, ypos, GLUT_BITMAP_HELVETICA_18, str.str());
	glEnable(GL_LIGHTING);
	
}
// Alex S.: For critical points algorithm testing. Should be removed later.
void CViewCtrl::drawCriticalPoints()
{
    if (!_triangMesh || !_triangMesh->_loaded){
        return;
    }

    // Draw critical points
    glPushAttrib(GL_CURRENT_BIT | GL_POINT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
    glPushMatrix();

	glMultMatrixf(_triangMesh->_worldMatrix.Transpose());

    // HACK - operate projection matrix directly to create an offset effect.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    GLfloat matrix[16];
    // Retrieve the projection matrix
    glGetFloatv(GL_PROJECTION_MATRIX, matrix);
    // Modify entry (3,3) of the projection matrix
    matrix[10] *= (1.0f - 0.001f);
    // Send the projection matrix back to OpenGL
    glLoadMatrixf(matrix);
    glMatrixMode(GL_MODELVIEW);


    glDisable(GL_LIGHTING);
    //glDisable(GL_DEPTH_TEST);

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Root points
#if 0
    glPointSize(7);
    glColor4f(0, 1, 0, 1);
    glBegin(GL_POINTS);
    for (unsigned pntNum = 0; pntNum < _rootPoints.size(); ++pntNum){
        glVertex3fv(&_rootPoints[pntNum][0]);
    }
    glEnd();
#endif
#if 1
    // Extremum points.
    glPointSize(5);
    glColor4f(1, 0, 0, 0.5f);
    glBegin(GL_POINTS);
    for (unsigned pntNum = 0; pntNum < _extremumPoints.size(); ++pntNum){
        glVertex3fv(&_extremumPoints[pntNum][0]);
        
    }
    glEnd();
#endif
    // Saddle points.
#if 0
    AntlersDebugger::logMessage("Found %ud saddle points.", _saddlePoints.size());

    glPointSize(7);
    glColor4f(1, 1, 0, 1.0f);
    glBegin(GL_POINTS);
    for (unsigned pntNum = 0; pntNum < _saddlePoints.size(); ++pntNum){
        glVertex3fv(&_saddlePoints[pntNum][0]);
        AntlersDebugger::logMessage("%lg %lg %lg\n", _saddlePoints[pntNum][0], _saddlePoints[pntNum][1], _saddlePoints[pntNum][2]);
    }
    glEnd();
#endif

    // Geo tree
#if 0
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1);
    glColor4f(0, 0, 1, 1);
    glBegin(GL_LINES);
    for (unsigned lineNum = 0; lineNum < _geoTreeLines.size(); ++lineNum){
        glVertex3fv(&_geoTreeLines[lineNum].m_p1[0]);
        glVertex3fv(&_geoTreeLines[lineNum].m_p2[0]);
    }
    glEnd();
#endif

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();

}

// Alex S.: For critical points algorithm testing. Should be removed later.
void CViewCtrl::findCriticalPoints()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (_triangMesh == NULL){
        return;
    }

    _extremumPoints.clear();
    _saddlePoints.clear();
    _rootPoints.clear();
    _geoTreeLines.clear();

    CriticalPointsFinder cpFinder(&_triangMesh->_mesh);//, _triangMesh->_normalizedView);
    cpFinder.FindCriticalPoints(false);//, true);
    cpFinder.GetRootPoints(_rootPoints);
    cpFinder.GetGeoTreeLines(_geoTreeLines);
    cpFinder.GetExtremumPoints(_extremumPoints);
    cpFinder.GetSaddlePoints(_saddlePoints);
}

UINT CViewCtrl::centerLineThread(LPVOID me)
{
	CViewCtrl * self = (CViewCtrl *)me;
    self->computeCenterLineThread();
    return 0;
}

void CViewCtrl::computeCenterLine()
{
	m_threadEvent = ::CreateEvent(NULL, // no security attributes
								  TRUE, // manual-reset
								  FALSE,// initially non-signaled
								  NULL);// anonymous

    m_compThread = AfxBeginThread(centerLineThread, this);

    switch(::WaitForSingleObject(m_threadEvent, 1000))
	{	// waitfor
		case WAIT_TIMEOUT:
			// problem with thread startup
			break;
		case WAIT_OBJECT_0:
			// thread is running
			// update GUI + GL every 50 ms
			while(m_isComputing)
			{
				Sleep(50);
				MSG msg;
				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}

				updateGL();
			}
			break;
	}	// waitfor02

	CloseHandle(m_threadEvent);
	m_threadEvent = NULL; // make nice with handle var for debugging
}

void CViewCtrl::computeCenterLineThread()
{
	setCursor(NM_WAIT);

	m_isComputing = true;
	::SetEvent(m_threadEvent);

	if(!_triangMesh)
		return;

	std::wostringstream str_n;
	DWORD crtime = GetTickCount();
	findCriticalPoints();
	crtime = GetTickCount() - crtime;
	str_n<<"\nFinding critical points: "<<crtime<<" ms.";

	DWORD cltime = GetTickCount();
	SAFE_DELETE(_centerLine);
	_centerLine = new CenterLine(_triangMesh, _extremumPoints, 0.055f / _triangMesh->_ratio, 
									_dlBurPlane[0]?&_burPlane[0]:NULL, _dlBurPlane[1]?&_burPlane[1]:NULL, _burCenter);
	_centerLine->Compute();
	setCursor(NM_NONE);

	cltime = GetTickCount() - cltime;
	str_n<<"\nBurr ring radius: "<<0.055f / _triangMesh->_ratio<<" in.";
	str_n<<"\nComputing center line: "<<cltime<<" ms.";

	//MessageBox(str_n.str().c_str(),L"Some",0);
	MessageBox(L"Centerline computed successfully",L"Antlers",0);

	m_isComputing = false;

/////////////////////////////////////////////////////////////////////////////////////////////////
//		Used by Branch length tool
/////////////////////////////////////////////////////////////////////////////////////////////////

	selection.Init(_centerLine);
/////////////////////////////////////////////////////////////////////////////////////////////////
//	
/////////////////////////////////////////////////////////////////////////////////////////////////

}

void CViewCtrl::toggleShowCenterLine()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(!_triangMesh)
		return;

#if FOR_INTERNAL_TESTING == 0
	if(!_dlBurPlane[0] || !_dlBurPlane[1])
	{
		MessageBox(L"Burr planes must be detected before the centerline can be computed.",L"Antlers",0);
		return;
	}
#endif

	m_showCenterLine = !m_showCenterLine;
	if(m_showCenterLine)
	{
		if(!_centerLine)
		{
			m_annotation.clear();
			computeCenterLine();

            updateAntlerTipMap();
            updateAnnotations();

            //m_showAnnotations = true; // ????
            //m_annotation.showAnnotations(m_showAnnotations);

			//m_annotation.addLeader("F", _centerLine->F[1], 0.05);
			/*//m_annotation.addLeader("G1", Vector3(-0.249145, 0.0699531, 0.341661), 0.05);

			m_annotation.addDimLine("127.344",
									Vector3(-0.466064, 0.278193, -0.174015),
									Vector3(0.477443, 0.318718, -0.0911556));
			m_annotation.addDimLine("127.344",
									Vector3(-0.249371, 0.0682525, 0.343276),
									Vector3(0.384714, 0.253749, 0.292404));
			m_annotation.addDimLine("127.344",
									Vector3(-0.573624, -0.455237, 0.215293),
									Vector3(0.618653, 0.0280873, -0.113667), 0.872049);

			m_annotation.addLeader("G2", Vector3(-0.248036, 0.0688612, 0.339669), 0.05);
			m_annotation.addLeader("G3", Vector3(-0.362698, 0.403557, 0.22439), 0.05);
			m_annotation.addLeader("G4", Vector3(-0.464679, 0.456487, -0.176659), 0.05);*/
		}
	}

	updateGL();
}

void CViewCtrl::toggleShowAnnotations()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(!m_showAnnotations && !_centerLine)
    {
        MessageBox(L"Center line must be computed before.",L"Antlers",0);
        return;
    }

	m_showAnnotations = !m_showAnnotations;

    m_annotation.showAnnotations(m_showAnnotations);

	updateGL();
}

void CViewCtrl::toggleShowAxis()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_showAxis = !m_showAxis;

	updateGL();
}

SHORT CViewCtrl::getVisibleElements()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SHORT res = 0;
	if(m_showAxis)
		res |= VE_AXIS;
	if(m_showAnnotations)
		res |= VE_ANNOTATIONS;
	if(m_showCenterLine)
		res |= VE_CENTERLINE;

	return res;
}

BOOL CViewCtrl::enterSelectMode()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(_currToolMode == TM_SEL_LENGTH)
      //  if(MessageBox(L"Do you really want to quit branch length tool?",L"Antlers",MB_YESNO) == IDYES)
        {
           //TODO: clear selections
            m_annotation.clearLengthInfo();
            _currToolMode = TM_NONE;
            updateGL();
            return false;
        }
      //  else
      //      return true;

    if(!_centerLine)
    {
        MessageBox(L"Center line must be computed before.",L"Antlers",0);
        return false;
    }

    //deselect previous selected mode !
    //TODO: ideally should be some common interface for all modes
    if(_currToolMode == TM_BUR_PLANE_L || _currToolMode == TM_BUR_PLANE_R)
    {
        if(enterAntlerPlane(_currToolMode == TM_BUR_PLANE_L ? 0 : 1))
            return false; //?? 
    }

    _currToolMode = TM_SEL_LENGTH;
    m_NavMode = NM_NONE;
    if(m_CurCursor != NM_NONE)
        setCursor(NM_NONE);

    updateGL();

    return true;
}

BOOL CViewCtrl::enterLabelingMode()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(_currToolMode == TM_TIP_LABELING)
    {
        //TODO: hide combo
        _currToolMode = TM_NONE;
        updateGL();
        return false;
    }

    if(!_centerLine)
    {
        MessageBox(L"Center line must be computed before.",L"Antlers",0);
        return false;
    }

    //deselect previous selected mode !
    //TODO: ideally should be some common interface for all modes
    if(_currToolMode == TM_BUR_PLANE_L || _currToolMode == TM_BUR_PLANE_R)
    {
        if(enterAntlerPlane(_currToolMode == TM_BUR_PLANE_L ? 0 : 1))
            return false; //?? 
    }

    if(_currToolMode == TM_SEL_LENGTH)
        m_annotation.clearLengthInfo();

    _currToolMode = TM_TIP_LABELING;
    m_NavMode = NM_NONE;
    if(m_CurCursor != NM_NONE)
        setCursor(NM_NONE);

    m_showAnnotations = true;
    m_annotation.showAnnotations(m_showAnnotations);

    updateGL();

    return true;
}

void CViewCtrl::dumpFunc()
{
}
