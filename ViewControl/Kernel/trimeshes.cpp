
#include "stdafx.h"
#include "TriMeshes.h"
#include "../ViewCtrl.h"
#include "../../Mesh/MeshVolume.h"

#include "Quaternion.h"

#include <fstream>
#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>

#include "OpenFlipper/Plugin-HoleFilling/HoleFillerT.hh"
#include "OpenFlipper/Plugin-HoleFilling/HoleInfoT.hh"

#include "ProgressImporterT.hh"

using namespace std;
using namespace OpenMesh;

/*
namespace OpenMesh {

	template <class Kernel>
	typename My_TriMeshT<Kernel>::FaceHandle
	My_TriMeshT<Kernel>::
	add_face(const std::vector<VertexHandle>& _vhandles)
	{
		MSG  msg;
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		GetGlobalView()->setLoadingProgress(g_CurFace*100/g_NumOfFaces);
		g_CurFace++;
		return TriMeshT::add_face(_vhandles);
	}
}*/

template <class Mesh> class SmootherT
{
public:

  typedef typename Mesh::Point            cog_t;
  typedef OpenMesh::VPropHandleT< cog_t > Property_cog;

public:

  // construct with a given mesh
  SmootherT(Mesh& _mesh) 
    : mesh_(_mesh)
  { 
    mesh_.add_property( cog_ );
  }

  ~SmootherT()
  {
    mesh_.remove_property( cog_ );
  }

  // smooth mesh _iterations times
  void smooth(unsigned int _iterations)
  {
    for (unsigned int i=0; i < _iterations; ++i)
    {
      std::for_each(mesh_.vertices_begin(), 
                    mesh_.vertices_end(), 
                    ComputeCOG(mesh_, cog_));

      std::for_each(mesh_.vertices_begin(), 
                    mesh_.vertices_end(), 
                    SetCOG(mesh_, cog_));
    }
  }


private:


  //--- private classes ---

  class ComputeCOG
  {
  public:
    ComputeCOG(Mesh& _mesh, Property_cog& _cog) 
      : mesh_(_mesh), cog_(_cog)
    {}

    void operator()(typename Mesh::Vertex& _v)
    {
      typename Mesh::VertexHandle      vh( mesh_.handle(_v) );
      typename Mesh::VertexVertexIter  vv_it;
      typename Mesh::Scalar            valence(0.0);
    
      mesh_.property(cog_, vh) = typename Mesh::Point(0.0, 0.0, 0.0);

      for (vv_it=mesh_.vv_iter(vh); vv_it; ++vv_it)
      {
        mesh_.property(cog_, vh) += mesh_.point( vv_it );
        ++valence;
      }

      mesh_.property(cog_, mesh_.handle(_v) ) /= valence;
    }

  private:
    Mesh&         mesh_;
    Property_cog& cog_;
  };


  class SetCOG
  {
  public:
    SetCOG(Mesh& _mesh, Property_cog& _cog) 
      : mesh_(_mesh), cog_(_cog)
    {}

    void operator()(typename Mesh::Vertex& _v)
    {
      typename Mesh::VertexHandle vh(mesh_.handle(_v));

      if (!mesh_.is_boundary(vh))
        mesh_.set_point( vh, mesh_.property(cog_, vh) );
    }

  private:

    Mesh&         mesh_;
    Property_cog& cog_;
  };


  //--- private elements ---

  Mesh&        mesh_;
  Property_cog cog_;
};

CTriMesh::CTriMesh()
{
	_xmin = _xmax = 0.0;
	_ymin = _ymax = 0.0;
	_zmin = _zmax = 0.0;
	_dlMesh = 0;
	_dlBrandSeed = 0;

	_loaded = false;
}

CTriMesh::~CTriMesh()
{
	DeleteDisplayLists();
}


void CTriMesh::GenerateBranchSeed()
{
	MeshType::ConstVertexIter	fIt(_mesh.vertices_begin()), 
								fEnd(_mesh.vertices_end());

	MeshType::ConstVertexFaceIter vfIt;
	Vector3 l_vec, r_vec, vec;

	//l_vec = _worldMatrix.TransformAffine(*(Vector3*)&(_mesh.point(fIt)));
	l_vec = *(Vector3*)&(_normalizedMesh.point(fIt));
	r_vec = l_vec;
	_branchSeed[0] = _branchSeed[1] = _mesh.faces_begin().handle();

	for (; fIt!=fEnd; ++fIt)
	{
		//vec = _worldMatrix.TransformAffine(*(Vector3*)&(_mesh.point(fIt)));
		vec = *(Vector3*)&(_normalizedMesh.point(fIt));

		if(vec.x < l_vec.x)
		{
			l_vec = vec;
			vfIt = _mesh.cvf_iter(fIt.handle());
			_branchSeed[1] = vfIt.handle();
		}
		else
			if(vec.x > r_vec.x)
			{
				r_vec = vec;
				vfIt = _mesh.cvf_iter(fIt.handle());
				_branchSeed[0] = vfIt.handle();
			}
	}



	_dlBrandSeed = glGenLists(1);
	if (!_dlBrandSeed)
	{
		glDeleteLists(_dlBrandSeed, 1);
		_dlMesh = 0;
		return;
	}

    _maxWidth = max(_xmax - _xmin, max(_ymax - _ymin, _zmax - _zmin));

	_ratio = 1.2 / _maxWidth;
	_translation = Vector3(-(_xmax+_xmin) / 2.0, -(_ymax+_ymin) / 2.0, -(_zmax+_zmin) / 2.0);

	glNewList(_dlBrandSeed, GL_COMPILE);
		glPushMatrix();
			/*glScalef(_ratio, _ratio, _ratio);
			glTranslatef(_translation.x, _translation.y, _translation.z);*/
			MeshType::ConstFaceVertexIter fvIt;

			glBegin(GL_TRIANGLES);

			for(int i=0; i<2; i++)
			{
				fvIt = _mesh.cfv_iter(_branchSeed[i]); 
				glVertex3fv((float*)&(_mesh.point(fvIt.handle())));
				++fvIt;
				glVertex3fv((float*)&(_mesh.point(fvIt.handle())));
				++fvIt;
				glVertex3fv((float*)&(_mesh.point(fvIt.handle())));
			}

			glEnd();
		glPopMatrix();
	glEndList();
}

#define ROUND2(x) ((double)((int)((x + 0.005)*100)))/100.0;

bool CTriMesh::ComputeBranchVolume(int iNum, const Plane &pl)
{
	OpenMesh::Vec4f v4f;
	v4f[0] = pl[0];
	v4f[1] = pl[1];
	v4f[2] = pl[2];
	v4f[3] = 0.0f;

	if(MeshVolume::ComputePartialMeshVolume(&_mesh, _branchSeed[iNum], v4f, _branchVolume[iNum]) == false)
	//if(_branchVolume[iNum] <= 0) // ?? fix ComputePartialMeshVolume()
		if(!MeshVolume::ComputePartialMeshVolume(&_mesh, _branchSeed[iNum], -v4f, _branchVolume[iNum]))
			_branchVolume[iNum] = -1;

	// convert from cubic mm -> cubic inches
	_branchVolume[iNum] /= 25.4 * 25.4 * 25.4;

	// round
	_branchVolume[iNum] = ROUND2(_branchVolume[iNum]);

	return true;
}

bool CTriMesh::ComputeVolume()
{
	bool res;
	if (!(res = MeshVolume::ComputeMeshVolume(&_mesh, _volume)))
		_volume = -1;

	// round
	_volume = ROUND2(_volume);

	return res;
}

bool CTriMesh::Load(const wchar_t* fName, int& numFaces)
{
	size_t i;
	char pStr[1024];

	// Conversion
	wcstombs_s(&i, pStr, (size_t)1024, 
			   fName, (size_t)1024 );

	return Load(pStr, numFaces);
}

bool CTriMesh::Load(std::string fName, int& numFaces)
{
	_loaded = false;

	DeleteDisplayLists();

	fstream f;
	f.open(fName.c_str(), ios_base::in | ios_base::binary);
	if (!f)
	{
		numFaces = 1;
		return false;
	}
	else
	{
		char header[80];
		f.read(header, sizeof(header));
		unsigned long int nrOfFacets;
		f.read((char *)&nrOfFacets, sizeof(nrOfFacets));

		numFaces = nrOfFacets;
		if(numFaces <=0)
			numFaces = 1;

		f.close();
	}

	if(!OpenMesh::IO::read_mesh_progress(_mesh, fName))
	{
		return false;
	}

	_normalizedMesh = _mesh;

	_mesh.request_face_normals();
	_mesh.request_vertex_normals();
	_mesh.update_face_normals();
	_mesh.update_vertex_normals();

	UpdateDisplayLists();

	_loaded = true;
	return true;
}

void CTriMesh::Smooth()
{
	typedef Smoother::JacobiLaplaceSmootherT<MeshType> MySmoother;
	MySmoother smoother(_mesh);
	smoother.initialize(MySmoother::Tangential_and_Normal, MySmoother::C0);
	smoother.smooth(100);

	//SmootherT<Mesh> smoother(_mesh);
	//smoother.smooth(15);

	_mesh.update_face_normals();
	_mesh.update_vertex_normals();

	UpdateDisplayLists();
}

bool CTriMesh::CheckHoles(float& maxLength)
{
	HoleInfo<MeshType> holeInfo(&_mesh);
	holeInfo.getHoles();

	if(holeInfo.holes()->empty())
		return false;

	maxLength = 0.0f;
	float diagonal = 0.0f, length = 0.0f;

	for(size_t i = 0; i < holeInfo.holes()->size(); ++i)
	{
		size_t edges;
		holeInfo.getHoleInfo(i, edges, diagonal, length);
		maxLength = std::max(maxLength, length);
	}

	return true;
}

bool CTriMesh::FixHoles()
{
	HoleInfo<MeshType> holeInfo(&_mesh);
	holeInfo.getHoles();

	holeInfo.fillAllHoles(2);

	_mesh.update_face_normals();
	_mesh.update_vertex_normals();

	UpdateDisplayLists();

	return true;
}


void CTriMesh::DeleteDisplayLists()
{
	if (_dlMesh)
	{
		glDeleteLists(_dlMesh, 1);
		_dlMesh = 0;
	}
	if (_dlBrandSeed)
	{
		glDeleteLists(_dlBrandSeed, 1);
		_dlBrandSeed = 0;
	}
}

void CTriMesh::DrawMesh() const
{
	if (_dlMesh)
	{
		glPushMatrix();
		glMultMatrixf(_worldMatrix.Transpose());
		glCallList(_dlMesh);
		glPopMatrix();
	}
}

void CTriMesh::DrawMeshPart(GLuint _dlMeshPart) const
{
	if (_dlMeshPart)
	{
		glPushMatrix();

		glMultMatrixf(_worldMatrix.Transpose());
		glCallList(_dlMeshPart);
		glPopMatrix();
	}
}

void CTriMesh::DrawBranchSeed() const
{
	if (_dlBrandSeed)
	{
		glPushMatrix();
		glMultMatrixf(_worldMatrix.Transpose());
		glCallList(_dlBrandSeed);
		glPopMatrix();
	}
}

void CTriMesh::SetNormalMatrix(const Matrix4 &mat)
{

	mat.Extract3x3Matrix(_rotationMatrix);
	_rotationMatrix.ToEulerAnglesXYZ(_angleX, _angleY, _angleZ);
	_rotationMatrix.FromEulerAnglesXYZ(_angleX, _angleY, _angleZ);

	// ???
	_angleX *= Math::RAD_TO_DEG;
	_angleY *= Math::RAD_TO_DEG;
	_angleZ *= Math::RAD_TO_DEG;
	_normalizedView = mat;


	Matrix4 trans = Matrix4::IDENTITY;
	Matrix4 rot = Matrix4::IDENTITY;
	Matrix4 scale = Matrix4::IDENTITY;
	scale.SetScale(Vector3(_ratio, _ratio, _ratio));
	rot = _rotationMatrix;
	trans.SetTranslation(_translation.x, _translation.y, _translation.z);

	_worldMatrix = rot * (scale * trans);

	MeshType::ConstVertexIter vIt(_mesh.vertices_begin());
	MeshType::ConstVertexIter vEnd(_mesh.vertices_end());     

	for (; vIt!=vEnd; ++vIt)
	{
		Vector3 &vec = *(Vector3*)&(_normalizedMesh.point(vIt));
		vec = _worldMatrix.TransformAffine(*(Vector3*)&(_mesh.point(vIt)));
	}

	_normalizedMesh.request_face_normals();
	_normalizedMesh.update_face_normals();
}
void CTriMesh::SetSkullBox(const Box3 &box)
{
    _skullBox = box;
}
bool CTriMesh::UpdateDisplayLists()
{
	DeleteDisplayLists();
	_dlMesh = glGenLists(1);
	if (!_dlMesh)
		return false;

	size_t index = 0;
	size_t count = _mesh.n_faces();

    // bounding box
    MeshType::ConstVertexIter vIt(_mesh.vertices_begin());
    MeshType::ConstVertexIter vEnd(_mesh.vertices_end());     
	OpenMesh::Vec3f bbMin, bbMax;
    
    bbMin = bbMax = OpenMesh::vector_cast<OpenMesh::Vec3f>(_mesh.point(vIt));
    
    for (size_t count=0; vIt!=vEnd; ++vIt, ++count)
    {
      bbMin.minimize( OpenMesh::vector_cast<OpenMesh::Vec3f>(_mesh.point(vIt)));
      bbMax.maximize( OpenMesh::vector_cast<OpenMesh::Vec3f>(_mesh.point(vIt)));
    }

	// TODO: rewrite ?!
	_xmin = bbMin[0];
	_ymin = bbMin[1];
	_zmin = bbMin[2];

	_xmax = bbMax[0];
	_ymax = bbMax[1];
	_zmax = bbMax[2];


    _maxWidth = max(_xmax - _xmin, max(_ymax - _ymin, _zmax - _zmin));

	_ratio = 1.2 / _maxWidth;
	_translation = Vector3(-(_xmax+_xmin) / 2.0, -(_ymax+_ymin) / 2.0, -(_zmax+_zmin) / 2.0);

	glNewList(_dlMesh, GL_COMPILE);
		glPushMatrix();
			/*glScalef(_ratio, _ratio, _ratio);
			glTranslatef(_translation.x, _translation.y, _translation.z);*/

			MeshType::ConstFaceIter		fIt(_mesh.faces_begin()), 
										fEnd(_mesh.faces_end());

			MeshType::ConstFaceVertexIter fvIt;
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, _mesh.points());

			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, _mesh.vertex_normals());

			glBegin(GL_TRIANGLES);

				for (; fIt!=fEnd; ++fIt)
				{
				  fvIt = _mesh.cfv_iter(fIt.handle()); 
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

	return true;
}
