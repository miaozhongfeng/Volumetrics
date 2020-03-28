//=====================================================================
// Definition of OpenMesh type. Others can add attributes to 
// MeshTraits struct if they need it
//
// Author: Voloshin Ivan
// ======================================================================

#ifndef MESH_DEF_H
#define MESH_DEF_H 
#include <OpenMesh\Core\Mesh\Types\TriMesh_ArrayKernelT.hh>
struct MeshTraits :  OpenMesh::DefaultTraits
{
	FaceAttributes    ( OpenMesh::Attributes::Status );
};
typedef OpenMesh::TriMesh_ArrayKernelT<MeshTraits> MeshType; //our mesh type
#endif