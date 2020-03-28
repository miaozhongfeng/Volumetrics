
#include "stdafx.h"
#include "Plane.h"
#include "Matrix3.h"

const Plane Plane::XY(Vector3(0.0f,0.0f,1.0f),0.0f);
const Plane Plane::XZ(Vector3(0.0f,1.0f,0.0f),0.0f);
const Plane Plane::YZ(Vector3(1.0f,0.0f,0.0f),0.0f);

//----------------------------------------------------------------------------
Vector3 Plane::ProjectVector(const Vector3& p)
{
	// We know plane normal is unit length, so use simple method
	Matrix3 xform;
	xform[0][0] = Normal.x * Normal.x - 1.0f;
	xform[0][1] = Normal.x * Normal.y;
	xform[0][2] = Normal.x * Normal.z;
	xform[1][0] = Normal.y * Normal.x;
	xform[1][1] = Normal.y * Normal.y - 1.0f;
	xform[1][2] = Normal.y * Normal.z;
	xform[2][0] = Normal.z * Normal.x;
	xform[2][1] = Normal.z * Normal.y;
	xform[2][2] = Normal.z * Normal.z - 1.0f;
	return xform * p;

}
