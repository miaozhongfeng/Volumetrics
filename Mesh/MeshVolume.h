//=====================================================================
// Functions for computing mesh whole or partial volume
//
// Author: Voloshin Ivan
// ======================================================================

#ifndef MESH_VOLUME_H
#define MESH_VOLUME_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "..\ViewControl\Kernel\TriMeshes.h"
#include <limits>
#include <vector>
#include <list>

namespace MeshVolume
{
	bool FindClosestFace(const OpenMesh::Vec3f & point, MeshType *mesh, MeshType::FaceHandle &face);
	bool ComputeMeshVolume(MeshType *mesh, float &volume);
	bool ComputePartialMeshVolume(MeshType *mesh, MeshType::FaceHandle seedTriangle, const OpenMesh::Vec4f &plane, float &volume);
	void FloodFill3(MeshType *mesh, MeshType::FaceHandle seed, std::vector<MeshType::FaceHandle> &whole1,
		std::vector<MeshType::FaceHandle> &split1, const OpenMesh::Vec4f &plane);
	void FloodFill_(MeshType *mesh, MeshType::FaceHandle seed, std::vector<MeshType::FaceHandle> &whole1,
		std::vector<MeshType::FaceHandle> &split1, const OpenMesh::Vec4f &plane);
	void FloodAntiFill3(MeshType *mesh, MeshType::FaceHandle seed, std::vector<MeshType::FaceHandle> &whole1,
		std::vector<MeshType::FaceHandle> &split1, const OpenMesh::Vec4f &plane);
}
#endif