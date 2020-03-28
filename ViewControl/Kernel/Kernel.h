
#ifndef KERNEL_H
#define KERNEL_H

#include <GL/glew.h>
#include <GL/glut.h>

#include <cmath>
#include <cassert>

// STL containers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <bitset>

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>

#pragma warning (disable : 4267)

// This macro should be used in order to avoid compiler errors when including Open mesh lib.
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// Open mesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#pragma warning (default : 4267)

typedef float Real;

typedef OpenMesh::VectorT<Real,3>           OM_Vector3;
typedef OpenMesh::VPropHandleT<Real>        OM_Property;

typedef std::vector<OM_Vector3>             VertVector;
typedef std::vector<OpenMesh::VertexHandle> HandlesVector;

class AxisAlignedBox;
class Vector2;
class Vector3;
class Vector4;
class Matrix3;
class Matrix4;
class Plane;
class Quaternion;
class Math;
class Ray;
class Sphere;
class Box3;

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

#endif
