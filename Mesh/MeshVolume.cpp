//=====================================================================
// Functions for computing mesh whole or partial volume
//
// Author: Voloshin Ivan
// ======================================================================
#include "stdafx.h"

#include "MeshVolume.h"
#include <limits>
#include <vector>
#include <list>

using namespace std;
namespace MeshVolume
{
#define EPS_ON_PLANE 0.0001f
using namespace OpenMesh;
struct MyFace //struct for fast triangle working
{
	MeshType::FaceHandle handle;
	Vec3f v[3];
};

//-----------------------------------------------
//finding closest face to point
//------------------------------------------------------------
bool FindClosestFace(const Vec3f & point, MeshType *mesh, MeshType::FaceHandle &face)
{
	if (mesh == NULL) return false;
	if (mesh->n_faces() == 0) return false;
	MeshType::FaceIter fi = mesh->faces_begin();
	MeshType::FaceIter fend = mesh->faces_end();
	float mindist = FLT_MAX;

	for (; fi != fend; ++fi)
	{
		//finding average position of points
		Vec3f v(0,0,0);
		uint n = 0;
		for (MeshType::FaceVertexIter fvi = mesh->fv_iter(fi); fvi; n++, ++fvi)
			v += mesh->point(fvi);
		v /= (float)n;
		
		float dst = (point-v).length();
		if (dst < mindist)
		{	mindist = dst; face = fi; }
	}
	return true;
}
//-----------------------------------------------------
//return false if mesh is not closed
//----------------------------------------------------
bool IsMeshClosed(MeshType *mesh)
{
	MeshType::FaceIter fi; //iterator for faces
	MeshType::FaceIter fe = mesh->faces_end(); //faces end
	for (fi = mesh->faces_begin(); fi != fe; ++fi)
	{
		int count = 0;
		for (MeshType::ConstFaceFaceIter ffi = mesh->cff_iter(fi); ffi; ++ffi, count++); //counting neighbor faces count
		if (count < 3) return false; //if too few neighbors
	}
	return true;
}
//------------------------------------------------------------------
// whole mesh volume
//-----------------------------------------------------------------
bool ComputeMeshVolume(MeshType *mesh, float &vol)
{
	vol = 0;

	if ((mesh == NULL) || (!IsMeshClosed(mesh))) return false;	
	double volume = 0.0f;
	MeshType::FaceIter fi; //iterator for faces
	MeshType::FaceIter fe = mesh->faces_end(); //faces end
	for (fi = mesh->faces_begin(); fi != fe; ++fi)
	{
		//reading all vertexes
		MeshType::Point v[3];
		MeshType::FaceVertexIter fvi = mesh->fv_iter(fi);
		for (int i = 0; i < 3; i++, ++fvi) v[i] = mesh->point(fvi);
		Vec3f v1 = v[1]-v[0], v2 = v[2]-v[0]; 

		Vec3f normal; //plane a,b,c
		double planeD; //plane d, also pyramid's height
		normal = v1%v2; //plane normal and also used in area finding
		double S = 0.5f*((normal).length());
		normal.normalize();
		planeD = -(v[0][0]*normal[0]+v[0][1]*normal[1]+v[0][2]*normal[2]);
			
		volume -= 1.0f/3.0f*S*planeD;
	}
	vol = volume;
	return true;
}
//------------------------------------------------------------------------
//returns true if array contains this face
//--------------------------------------------------------------------------------
inline bool HasFace(vector<MyFace> &ar, MeshType::FaceHandle h)
{
	for (uint i = 0; i < ar.size(); i++)
		if (ar[i].handle == h) return true;
	return false;
}
inline bool HasFace(list<MyFace> &ar, MeshType::FaceHandle h)
{
	for (list<MyFace>::iterator  i = ar.begin(); i != ar.end(); i++)
		if ((*i).handle == h) return true;
	return false;
}
/*
//--------------------------------------------------------------------------
//selects faces in front of plane (recursive)
// cause stack overflow when too many verices
//-------------------------------------------------------------------------------
void FloodFill(MeshType *mesh, MeshType::FaceHandle seed, vector<MyFace> &whole,
			   vector<MyFace> &split, const Vec4f &plane)
{
	if (HasFace(whole, seed) || (HasFace(split, seed))) return; //already selected
	MyFace face;
	face.handle = seed;
	MeshType::FaceVertexIter fvi = mesh->fv_iter(face.handle);

	int oncount = 0, // points on plane
		frontcount = 0, //points in front of plane
		backcount = 0; //points in back of plane
	for (uint i = 0; i < 3; i++, ++fvi) 
	{
		face.v[i] = mesh->point(fvi); //getting face vertexes
		//finding postion of point relative the plane
		float dst = face.v[i][0]*plane[0]+face.v[i][1]*plane[1]+face.v[i][2]*plane[2]+plane[3];
		if (abs(dst) <= EPS_ON_PLANE) oncount++;
		else if (dst > 0.0f) frontcount++;
		else backcount++;
	}
	if ((backcount == 3) || ((oncount == 1) && (backcount == 2))
		|| ((oncount == 2) && (backcount == 1))) return; //triangle is in back of plane

	bool bSplited = false;
	if (frontcount < 3)
	{
		if ((frontcount == 2) && (backcount == 1)) bSplited = true;
		else if ((frontcount == 1) && (backcount == 2)) bSplited = true; 
		else if ((oncount == 2) && (frontcount == 1)) bSplited = true;
	}
	if (bSplited) split.push_back(face);
	else whole.push_back(face);

	for (MeshType::FaceFaceIter ffi = mesh->ff_iter(seed); ffi; ++ffi)
		FloodFill(mesh, ffi.handle(), whole, split, plane); //recursion
}
*/
struct StackRec
{
	MeshType::FaceHandle face;
	MeshType::FaceFaceIter it;
};
//return false if trianlge is in back of plane, else return true,
//also put trianle either in whole or in split
inline bool ClassifyTriangle(MeshType *mesh, MeshType::FaceHandle faceh, vector<MyFace> &whole,
			   vector<MyFace> &split, const Vec4f &plane)
{
	MyFace face;
	face.handle = faceh;
	MeshType::FaceVertexIter fvi = mesh->fv_iter(face.handle);

	int oncount = 0, // points on plane
		frontcount = 0, //points in front of plane
		backcount = 0; //points in back of plane
	for (uint i = 0; i < 3; i++, ++fvi) 
	{
		face.v[i] = mesh->point(fvi); //getting face vertexes
		//finding postion of point relative the plane
		float dst = face.v[i][0]*plane[0]+face.v[i][1]*plane[1]+face.v[i][2]*plane[2]+plane[3];
		if (abs(dst) <= EPS_ON_PLANE) oncount++;
		else if (dst > 0.0f) frontcount++;
		else backcount++;
	}

	if ((backcount == 3) || ((oncount == 1) && (backcount == 2))
		|| ((oncount == 2) && (backcount == 1))) 		
		return false; //triangle is in back of plane, remove from stack
	

	bool bSplited = false;
	if (frontcount < 3)
	{
		if ((frontcount == 2) && (backcount == 1)) bSplited = true;
		else if ((frontcount == 1) && (backcount == 2)) bSplited = true; 
		else if ((oncount == 2) && (frontcount == 1)) bSplited = true;
	}
	if (bSplited) split.push_back(face);
	else whole.push_back(face);
	return true;
}
//--------------------------------------------------------------------------
//selects faces in front of plane (stack simulation in heap)
//return false if mesh is not closed
//-------------------------------------------------------------------------------
bool FloodFill2(MeshType *mesh, MeshType::FaceHandle seed, vector<MyFace> &whole,
			   vector<MyFace> &split, const Vec4f &plane)
{
	//if seed triangle is in back of plane - leave;
	if (!ClassifyTriangle(mesh, seed, whole,split,plane)) return false;

	//clear tag flags
	MeshType::FaceIter it_end =  mesh->faces_end();
	for (MeshType::FaceIter i = mesh->faces_begin(); i != it_end; ++i)
		mesh->status(i).set_tagged(false);
	
	vector<StackRec> Stack;
	Stack.reserve(mesh->n_faces());
	StackRec newsrec;
	newsrec.face = seed;
	newsrec.it = mesh->ff_iter(seed);
	Stack.push_back(newsrec);
	while(Stack.size() > 0)
	{
		StackRec *srec = &(Stack[Stack.size()-1]); //get rec from stack
		if (!srec->it) //done with this triangle
		{	
			Stack.pop_back();
			continue;
		}
		if (!mesh->status(srec->it).tagged())
		{
			if (ClassifyTriangle(mesh,srec->it.handle(),whole,split,plane))
			{
				newsrec.face = srec->it.handle();
				newsrec.it = mesh->ff_iter(srec->it.handle());
				mesh->status(newsrec.face).set_tagged(true);
				Stack.push_back(newsrec);

				int count = 0;
				for (MeshType::ConstFaceFaceIter ffi = mesh->cff_iter(newsrec.face); ffi; ++ffi, count++); //counting neighbor faces count
				if (count < 3) return false; //if too few neighbors
			}
		}
		++srec->it;
	}

	return true;
}
void FloodFill_(MeshType *mesh, MeshType::FaceHandle seed, std::vector<MeshType::FaceHandle> &whole1,
		std::vector<MeshType::FaceHandle> &split1, const OpenMesh::Vec4f &plane)
{
	vector<MyFace> whole, split;
	FloodFill2(mesh, seed, whole, split, plane);
	for(size_t i = 0; i < whole.size(); i++) whole1.push_back(whole[i].handle);
	for(size_t i = 0; i < split.size(); i++) split1.push_back(split[i].handle);
	whole.clear(); split.clear();
}
void FloodFill3(MeshType *mesh, MeshType::FaceHandle seed, vector<MeshType::FaceHandle> &whole1,
			   vector<MeshType::FaceHandle> &split1, const Vec4f &plane)
{
	vector<MyFace> whole, split;
	FloodFill2(mesh, seed, whole, split, plane);
	whole1.clear(); split1.clear();
	for(size_t i = 0; i < whole.size(); i++) whole1.push_back(whole[i].handle);
	for(size_t i = 0; i < split.size(); i++) split1.push_back(split[i].handle);
	whole.clear(); split.clear();
}
//------------------------------------------------------------------------
//---Converts 3d point to new basis, drops Z
//--------------------------------------------------------------
Vec2f PointToNewBasis(const Vec3f &point, const Vec3f &v1, const Vec3f &v2, const Vec3f &p )
{
	Vec3f rv = point - p;
	return Vec2f(dot(rv,v1), dot(rv,v2));
}
//---------------------------------------------------------------
//computes planes orthonormal basis
//---------------------------------------------------
void GetPlaneBasis(const Vec4f& plane, const Vec3f &p1, const Vec3f &p2, //plane and 2 points on it
				   Vec3f &vx, Vec3f &vy) // return basis vectors
{
	vx = p2 - p1;
    Vec3f n = Vec3f(&plane[0]);
	vy = cross(n, vx);
	vx.normalize();
	vy.normalize();
}
//---------------------------------------------------------------------------
//--Split triangles by plane, finding edges
//--(assumes that all triangles in split are cuted by plane)
//-----------------------------------------------------------------------------
void SplitTriangles(vector<MyFace> &split, vector<MyFace> &whole, vector<Vec2f> &edges, const Vec4f &plane)
{
	MyFace face;
	Vec3f basisV1, basisV2, basisOrg; //plane basis, initially not founded
	bool basisFound = false; //is plane basis found?

	for (uint i = 0; i < split.size(); i++)
	{
		Vec3f dst; //distances to plane		
		int oncount = 0, backcount = 0, frontcount = 0;
		for (uint j = 0; j < 3; j++)
		{
			dst[j] = split[i].v[j][0]*plane[0]+split[i].v[j][1]*plane[1]+split[i].v[j][2]*plane[2]+plane[3];
			if (abs(dst[j]) <= EPS_ON_PLANE) oncount++;
			else if (dst[j] > 0.0f) frontcount++;
			else backcount++;
		}
		if (oncount == 2) // the edge is on plane
		{
			Vec3f ep[2];
			for (uint j = 0; j < 3; j++)
				if (dst[j] <= EPS_ON_PLANE)
				{
					if (dst[j+1] <= EPS_ON_PLANE)
					{
						ep[0] = split[i].v[j];
						ep[1] = split[i].v[j+1];
					}
					else
					{
						ep[0] = split[i].v[j];
						ep[1] = split[i].v[j+2];
					}
					break;
				}

			if (!basisFound)
			{
				GetPlaneBasis(plane,ep[0],ep[1],basisV1,basisV2);
				basisOrg = ep[0];
				basisFound = true;
			}
			edges.push_back(PointToNewBasis(ep[0], basisV1,basisV2,basisOrg));
			edges.push_back(PointToNewBasis(ep[1], basisV1,basisV2,basisOrg));
			whole.push_back(split[i]);
		}
		else //plane splits triangle
		{
			int nEdgePoint1 = -1;	//number of edge of source triangle where first point of cutting edge is found
									//-1 - first point is not found yet
			int nEdgePoint2 = -1; //same as above for point 2

			Vec3f edgepts[2];	//0 - first finded edge point
								//1 - second point on edge
			uint pts[3];	//0 - point prevision to 0 point on edge on source triangle
							//1 - point next to 0 point on edge on source triangle
							//2 - point on source triangle oposite to point 0
							

			for (uint j = 0; j < 3; j++) //through all edges of triangle
			{
				int p1 = j, p2 = (j + 1) % 3; //indexes of edge verteces
				if (dst[p1] * dst[p2] < 0.0f)
				{
					//finding cross point
					//V = V1 - dV*Dist1/dDist;
					Vec3f crosspoint = split[i].v[p1] - (split[i].v[p2]-split[i].v[p1])*dst[p1]/(dst[p2]-dst[p1]);
					if (nEdgePoint1 == -1)
					{
						edgepts[0] = crosspoint;
						pts[0] = p1;
						pts[1] = p2;
						pts[2] = (p2+1)%3;
						nEdgePoint1 = j; //now searshing second point
					}
					else 
					{
						edgepts[1] = crosspoint;
						nEdgePoint2 = j;
					}
				}
			}
			if (nEdgePoint2 - nEdgePoint1 == 2) //need to flip all
			{
				swap(edgepts[0], edgepts[1]);
				swap(pts[1], pts[2]);
				swap(pts[0], pts[1]);
			}
			if (frontcount == 2) //split situation when 1 vertex is in back of plane
			{
				face.v[0] = edgepts[0];
				face.v[1] = edgepts[1];
				face.v[2] = split[i].v[pts[2]]; //opposite
				whole.push_back(face);
				face.v[0] = edgepts[0];
				face.v[1] = split[i].v[pts[2]]; //opposite
				face.v[2] = split[i].v[pts[0]]; //prevision
				whole.push_back(face);
			}
			else //when 2 points is in back of plane
			{
				swap(edgepts[0], edgepts[1]);
				face.v[0] = edgepts[0];
				face.v[1] = edgepts[1];
				face.v[2] = split[i].v[pts[0]]; 
				whole.push_back(face);
			}
			if (!basisFound)
			{
				GetPlaneBasis(plane,edgepts[0],edgepts[1],basisV1,basisV2);
				basisOrg = edgepts[0];
				basisFound = true;
			}
			edges.push_back(PointToNewBasis(edgepts[0], basisV1,basisV2,basisOrg));
			edges.push_back(PointToNewBasis(edgepts[1], basisV1,basisV2,basisOrg));
		}
	}
}
//---------------------------------------------------------------
//Compute area of 2d polygon
//---------------------------------------------------------
//2 points on each edge, order of points has meaning of sign
float PolygonArea(vector<Vec2f> & edges)
{
    float result;
    result = 0;
 
	for (uint i = 0; i < edges.size(); i+=2)
		result = result-(edges[i][0]+edges[i+1][0])*(edges[i][1]-edges[i+1][1]);
   
    result = 0.5f*result;
    return result;
}
//---------------------------------------------------------------
// compute volume of mesh part flood filled from seedTriangle to the plane
//-------------------------------------------------------------------------
bool ComputePartialMeshVolume(MeshType *mesh, MeshType::FaceHandle seedTriangle, const OpenMesh::Vec4f &plane, float &vol)
{
	vol = 0;
	if (mesh == NULL) return false;	
	vector<MyFace>	WholeTri, //list of triangles that are not intersectied by plane, or lying on plane
					SplitTri; //these are intersected by plane, or edge on plane
	WholeTri.reserve(mesh->n_faces());
	SplitTri.reserve(mesh->n_faces());
	if(!FloodFill2(mesh, seedTriangle, WholeTri, SplitTri, plane))
		return false; //selecting triangles
	
	vector<Vec2f> SplitEdges; //edges on the cutting plane in 2d coords of plane
	SplitEdges.reserve(SplitTri.size()*2);
	SplitTriangles(SplitTri, WholeTri, SplitEdges, plane);
	float CutArea = PolygonArea(SplitEdges);

	//-----------------------
	//now there is all data to compute volume
	//--------------------------
	double volume = 0.0f; 

	for (uint i = 0; i < WholeTri.size(); i++)
	{
		Vec3f v1 = WholeTri[i].v[1]-WholeTri[i].v[0], v2 = WholeTri[i].v[2]-WholeTri[i].v[0]; 

		Vec3f normal; //plane a,b,c
		double planeD; //plane d, also pyramid's height
		normal = v1%v2; //plane normal and also used in area finding
		double S = 0.5f*((normal).length());
		normal.normalize();
		planeD = -(WholeTri[i].v[0][0]*normal[0]+WholeTri[i].v[0][1]*normal[1]+WholeTri[i].v[0][2]*normal[2]);
		volume -= 1.0f/3.0f*S*planeD;
		
	}
	//consider cut face too
	volume += 1.0f/3.0f*CutArea*plane[3];
	vol = volume;
	return true;
}
}