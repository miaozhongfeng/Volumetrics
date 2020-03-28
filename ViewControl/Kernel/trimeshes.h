#pragma once

#include "Kernel.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Box3.h"


/*namespace OpenMesh {

	template <class Kernel>
//	class My_TriMeshT : public TriMeshT<Kernel>
		class My_TriMeshT : public TriMesh_ArrayKernelT<Kernel>
	{
	public:
		FaceHandle add_face(const std::vector<VertexHandle>& _vhandles);
	};

	/// Helper class to create a TriMesh-type based on ArrayKernelT
	template <class Traits>
	struct My_TriMesh_ArrayKernel_GeneratorT
	{
		typedef FinalMeshItemsT<Traits, true>               MeshItems;
		typedef AttribKernelT<MeshItems, TriConnectivity>   AttribKernel;
		typedef TriMeshT<AttribKernel>                      Mesh;
	};


	// \ingroup mesh_types_group 
	//	Triangle mesh based on the ArrayKernel.
	//	\see OpenMesh::TriMeshT
	//	\see OpenMesh::ArrayKernelT
	
	template <class Traits = DefaultTraits>  
	class My_TriMesh_ArrayKernelT 
	  : public My_TriMesh_ArrayKernel_GeneratorT<Traits>::Mesh 
	{};

}*/

struct MeshTraits : OpenMesh::DefaultTraits
{
	FaceAttributes(OpenMesh::Attributes::Status);
	VertexAttributes(OpenMesh::Attributes::Status);
	//HalfedgeAttributes( OpenMesh::Attributes::Status );
};

typedef OpenMesh::TriMesh_ArrayKernelT<MeshTraits> MeshType; //our mesh type
//typedef OpenMesh::My_TriMeshT<MeshTraits> MeshType; //our mesh type

class CTriMesh
{
public:
//protected:
	MeshType	_mesh;
	MeshType	_normalizedMesh;

    Box3        _skullBox;

	float		_xmin, _xmax,
				_ymin, _ymax,
				_zmin, _zmax;
    float        _maxWidth;
	GLuint		_dlMesh,
				_dlBrandSeed;
				
	float		_ratio;
	Vector3		_translation;

	bool		_loaded;

	float		_branchVolume[2];
	float		_volume;
	float		_angleX, _angleY, _angleZ;
	Matrix4		_normalizedView;
	Matrix3		_rotationMatrix;
	Matrix4		_worldMatrix;

	MeshType::FaceHandle	_branchSeed[2];

public:

	CTriMesh();
	
	bool Load(const wchar_t* fName, int& numFaces);
	bool Load(std::string fName, int& numFaces);
	
	void DeleteDisplayLists();
	void DrawMesh() const;
	void DrawMeshPart(GLuint _dlMeshPart) const;
	void DrawBranchSeed() const;
	bool UpdateDisplayLists();
	void SetNormalMatrix(const Matrix4 &mat);
    void SetSkullBox(const Box3 &box);
	bool ComputeVolume();
	bool ComputeBranchVolume(int iNum, const Plane &pl);
	void GenerateBranchSeed();
	void Smooth();

	bool CheckHoles(float& maxLength);
	bool FixHoles();

	~CTriMesh();
};
