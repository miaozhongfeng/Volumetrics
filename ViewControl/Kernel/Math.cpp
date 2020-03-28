
#include "stdafx.h"
#include "Math.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Ray.h"
#include "Sphere.h"
#include "AxisAlignedBox.h"
#include "Plane.h"

#undef max 

const Real Math::POS_INFINITY = std::numeric_limits<Real>::infinity();
const Real Math::NEG_INFINITY = -std::numeric_limits<Real>::infinity();
const Real Math::EPSILON = FLT_EPSILON;
const Real Math::ZERO_TOLERANCE = 1e-06f;
const Real Math::MAX_REAL = std::numeric_limits<Real>::max();//FLT_MAX;
const Real Math::PI = Real (4.0*atan(1.0));
const Real Math::TWO_PI = Real (2.0*Math::PI);
const Real Math::HALF_PI = Real (0.5*Math::PI);
const Real Math::INV_PI = Real (1.0/Math::PI);
const Real Math::INV_TWO_PI = Real (1.0/Math::TWO_PI);
const Real Math::DEG_TO_RAD = Math::PI/Real(180.0);
const Real Math::RAD_TO_DEG = Real(180.0)/Math::PI;
const Real Math::LN_2 = Real (Math::Log(2.0));
const Real Math::LN_10 = Real (Math::Log(10.0));
const Real Math::INV_LN_2 = Real (1.0/Math::LN_2);
const Real Math::INV_LN_10 = Real (1.0/Math::LN_10);

const float Math::MAX_FLOAT = FLT_MAX;
const double Math::MAX_DOUBLE = DBL_MAX;

//----------------------------------------------------------------------------
Real Math::FastInvSqrt (Real fValue)
{
	Real fHalf = 0.5f*fValue;
	int i  = *(int*)&fValue;
	i = 0x5f3759df - (i >> 1);
	fValue = *(Real*)&i;
	fValue = fValue*(1.5f - fHalf*fValue*fValue);
	return fValue;
}
//----------------------------------------------------------------------------*/
bool Math::pointInTri2D(const Vector2& p, const Vector2& a, 
	const Vector2& b, const Vector2& c)
{
	// Winding must be consistent from all edges for point to be inside
	Vector2 v1, v2;
	Real dot[3];
	bool zeroDot[3];

	v1 = b - a;
	v2 = p - a;

	// Note we don't care about normalisation here since sign is all we need
	// It means we don't have to worry about magnitude of cross products either
	dot[0] = v1.Cross(v2);
	zeroDot[0] = Math::RealEqual(dot[0], 0.0f, Real(1e-3));


	v1 = c - b;
	v2 = p - b;

	dot[1] = v1.Cross(v2);
	zeroDot[1] = Math::RealEqual(dot[1], 0.0f, Real(1e-3));

	// Compare signs (ignore colinear / coincident points)
	if(!zeroDot[0] && !zeroDot[1] 
	&& Math::Sign(dot[0]) != Math::Sign(dot[1]))
	{
		return false;
	}

	v1 = a - c;
	v2 = p - c;

	dot[2] = v1.Cross(v2);
	zeroDot[2] = Math::RealEqual(dot[2], 0.0f, Real(1e-3));
	// Compare signs (ignore colinear / coincident points)
	if((!zeroDot[0] && !zeroDot[2] 
		&& Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
		(!zeroDot[1] && !zeroDot[2] 
		&& Math::Sign(dot[1]) != Math::Sign(dot[2])))
	{
		return false;
	}


	return true;
}
//-----------------------------------------------------------------------
bool Math::pointInTri3D(const Vector3& p, const Vector3& a, 
	const Vector3& b, const Vector3& c, const Vector3& normal)
{
	// Winding must be consistent from all edges for point to be inside
	Vector3 v1, v2;
	Real dot[3];
	bool zeroDot[3];

	v1 = b - a;
	v2 = p - a;

	// Note we don't care about normalisation here since sign is all we need
	// It means we don't have to worry about magnitude of cross products either
	dot[0] = v1.Cross(v2).Dot(normal);
	zeroDot[0] = Math::RealEqual(dot[0], 0.0f, Real(1e-3));


	v1 = c - b;
	v2 = p - b;

	dot[1] = v1.Cross(v2).Dot(normal);
	zeroDot[1] = Math::RealEqual(dot[1], 0.0f, Real(1e-3));

	// Compare signs (ignore colinear / coincident points)
	if(!zeroDot[0] && !zeroDot[1] 
		&& Math::Sign(dot[0]) != Math::Sign(dot[1]))
	{
		return false;
	}

	v1 = a - c;
	v2 = p - c;

	dot[2] = v1.Cross(v2).Dot(normal);
	zeroDot[2] = Math::RealEqual(dot[2], 0.0f, Real(1e-3));
	// Compare signs (ignore colinear / coincident points)
	if((!zeroDot[0] && !zeroDot[2] 
		&& Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
		(!zeroDot[1] && !zeroDot[2] 
		&& Math::Sign(dot[1]) != Math::Sign(dot[2])))
	{
		return false;
	}


	return true;
}
//-----------------------------------------------------------------------
std::pair<bool, Real> Math::intersects(const Ray& ray, const Plane& plane)
{

	Real denom = plane.Normal.Dot(ray.getDirection());
	if (Math::FAbs(denom) < Math::EPSILON)
	{
		// Parallel
		return std::pair<bool, Real>(false, 0);
	}
	else
	{
		Real nom = plane.Normal.Dot(ray.getOrigin()) + plane.Constant;
		Real t = -(nom/denom);
		return std::pair<bool, Real>(t >= 0, t);
	}
    
}
//-----------------------------------------------------------------------
std::pair<bool, Real> Math::intersects(const Ray& ray, 
	const std::vector<Plane>& planes, bool normalIsOutside)
{
	std::vector<Plane>::const_iterator planeit, planeitend;
	planeitend = planes.end();
	bool allInside = true;
	std::pair<bool, Real> ret;
	ret.first = false;
	ret.second = 0.0f;

	// derive side
	// NB we don't pass directly since that would require Plane::Side in 
	// interface, which results in recursive includes since Math is so fundamental
	Side outside = normalIsOutside ? POSITIVE_SIDE : NEGATIVE_SIDE;

	for (planeit = planes.begin(); planeit != planeitend; ++planeit)
	{
		const Plane& plane = *planeit;
		// is origin outside?
		if (plane.GetSide(ray.getOrigin()) == outside)
		{
			allInside = false;
			// Test single plane
			std::pair<bool, Real> planeRes = 
				ray.intersects(plane);
			if (planeRes.first)
			{
				// Ok, we intersected
				ret.first = true;
				// Use the most distant result since convex volume
				ret.second = std::max(ret.second, planeRes.second);
			}
		}
	}

	if (allInside)
	{
		// Intersecting at 0 distance since inside the volume!
		ret.first = true;
		ret.second = 0.0f;
	}

	return ret;
}
//-----------------------------------------------------------------------
std::pair<bool, Real> Math::intersects(const Ray& ray, 
	const std::list<Plane>& planes, bool normalIsOutside)
{
	std::list<Plane>::const_iterator planeit, planeitend;
	planeitend = planes.end();
	bool allInside = true;
	std::pair<bool, Real> ret;
	ret.first = false;
	ret.second = 0.0f;

	// derive side
	// NB we don't pass directly since that would require Plane::Side in 
	// interface, which results in recursive includes since Math is so fundamental
	Side outside = normalIsOutside ? POSITIVE_SIDE : NEGATIVE_SIDE;

	for (planeit = planes.begin(); planeit != planeitend; ++planeit)
	{
		const Plane& plane = *planeit;
		// is origin outside?
		if (plane.GetSide(ray.getOrigin()) == outside)
		{
			allInside = false;
			// Test single plane
			std::pair<bool, Real> planeRes = 
				ray.intersects(plane);
			if (planeRes.first)
			{
				// Ok, we intersected
				ret.first = true;
				// Use the most distant result since convex volume
				ret.second = std::max(ret.second, planeRes.second);
			}
		}
	}

	if (allInside)
	{
		// Intersecting at 0 distance since inside the volume!
		ret.first = true;
		ret.second = 0.0f;
	}

	return ret;
}
//-----------------------------------------------------------------------
std::pair<bool, Real> Math::intersects(const Ray& ray, const Sphere& sphere, 
	bool discardInside)
{
	const Vector3& raydir = ray.getDirection();
	// Adjust ray origin relative to sphere center
	const Vector3& rayorig = ray.getOrigin() - sphere.getCenter();
	Real radius = sphere.getRadius();

	// Check origin inside first
	if (rayorig.SquaredLength() <= radius*radius && discardInside)
	{
		return std::pair<bool, Real>(true, 0);
	}

	// Mmm, quadratics
	// Build coeffs which can be used with std quadratic solver
	// ie t = (-b +/- sqrt(b*b + 4ac)) / 2a
	Real a = raydir.Dot(raydir);
	Real b = 2 * rayorig.Dot(raydir);
	Real c = rayorig.Dot(rayorig) - radius*radius;

	// Calc determinant
	Real d = (b*b) - (4 * a * c);
	if (d < 0)
	{
		// No intersection
		return std::pair<bool, Real>(false, 0);
	}
	else
	{
		// BTW, if d=0 there is one intersection, if d > 0 there are 2
		// But we only want the closest one, so that's ok, just use the 
		// '-' version of the solver
		Real t = ( -b - Math::Sqrt(d) ) / (2 * a);
		if (t < 0)
			t = ( -b + Math::Sqrt(d) ) / (2 * a);
		return std::pair<bool, Real>(true, t);
	}


}
//-----------------------------------------------------------------------
std::pair<bool, Real> Math::intersects(const Ray& ray, const AxisAlignedBox& box)
{
	if (box.isNull()) return std::pair<bool, Real>(false, 0);
	if (box.isInfinite()) return std::pair<bool, Real>(true, 0);

	Real lowt = 0.0f;
	Real t;
	bool hit = false;
	Vector3 hitpoint;
	const Vector3& min = box.getMinimum();
	const Vector3& max = box.getMaximum();
	const Vector3& rayorig = ray.getOrigin();
	const Vector3& raydir = ray.getDirection();

	// Check origin inside first
	if ( rayorig > min && rayorig < max )
	{
		return std::pair<bool, Real>(true, 0);
	}

	// Check each face in turn, only check closest 3
	// Min x
	if (rayorig.x < min.x && raydir.x > 0)
	{
		t = (min.x - rayorig.x) / raydir.x;
		if (t > 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max x
	if (rayorig.x > max.x && raydir.x < 0)
	{
		t = (max.x - rayorig.x) / raydir.x;
		if (t > 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min y
	if (rayorig.y < min.y && raydir.y > 0)
	{
		t = (min.y - rayorig.y) / raydir.y;
		if (t > 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max y
	if (rayorig.y > max.y && raydir.y < 0)
	{
		t = (max.y - rayorig.y) / raydir.y;
		if (t > 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min z
	if (rayorig.z < min.z && raydir.z > 0)
	{
		t = (min.z - rayorig.z) / raydir.z;
		if (t > 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max z
	if (rayorig.z > max.z && raydir.z < 0)
	{
		t = (max.z - rayorig.z) / raydir.z;
		if (t > 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	return std::pair<bool, Real>(hit, lowt);

}
//-----------------------------------------------------------------------
bool Math::intersects(const Ray& ray, const AxisAlignedBox& box,
	Real* d1, Real* d2)
{
	if (box.isNull())
		return false;

	if (box.isInfinite())
	{
		if (d1) *d1 = 0;
		if (d2) *d2 = Math::POS_INFINITY;
		return true;
	}

	const Vector3& min = box.getMinimum();
	const Vector3& max = box.getMaximum();
	const Vector3& rayorig = ray.getOrigin();
	const Vector3& raydir = ray.getDirection();

	Vector3 absDir;
	absDir[0] = Math::FAbs(raydir[0]);
	absDir[1] = Math::FAbs(raydir[1]);
	absDir[2] = Math::FAbs(raydir[2]);

	// Sort the axis, ensure check minimise floating error axis first
	int imax = 0, imid = 1, imin = 2;
	if (absDir[0] < absDir[2])
	{
		imax = 2;
		imin = 0;
	}
	if (absDir[1] < absDir[imin])
	{
		imid = imin;
		imin = 1;
	}
	else if (absDir[1] > absDir[imax])
	{
		imid = imax;
		imax = 1;
	}

	Real start = 0, end = Math::POS_INFINITY;

#define _CALC_AXIS(i)                                       \
do {                                                    \
	Real denom = 1 / raydir[i];                         \
	Real newstart = (min[i] - rayorig[i]) * denom;      \
	Real newend = (max[i] - rayorig[i]) * denom;        \
	if (newstart > newend) std::swap(newstart, newend); \
	if (newstart > end || newend < start) return false; \
	if (newstart > start) start = newstart;             \
	if (newend < end) end = newend;                     \
} while(0)

	// Check each axis in turn

	_CALC_AXIS(imax);

	if (absDir[imid] < Math::EPSILON)
	{
		// Parallel with middle and minimise axis, check bounds only
		if (rayorig[imid] < min[imid] || rayorig[imid] > max[imid] ||
			rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
			return false;
	}
	else
	{
		_CALC_AXIS(imid);

		if (absDir[imin] < Math::EPSILON)
		{
			// Parallel with minimise axis, check bounds only
			if (rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
				return false;
		}
		else
		{
			_CALC_AXIS(imin);
		}
	}
#undef _CALC_AXIS

	if (d1) *d1 = start;
	if (d2) *d2 = end;

	return true;
}
//-----------------------------------------------------------------------
std::pair<bool, Real> Math::intersects(const Ray& ray, const Vector3& a,
	const Vector3& b, const Vector3& c, const Vector3& normal,
	bool positiveSide, bool negativeSide)
{
	//
	// Calculate intersection with plane.
	//
	Real t;
	{
		Real denom = normal.Dot(ray.getDirection());

		// Check intersect side
		if (denom > + Math::EPSILON)
		{
			if (!negativeSide)
				return std::pair<bool, Real>(false, 0);
		}
		else if (denom < - Math::EPSILON)
		{
			if (!positiveSide)
				return std::pair<bool, Real>(false, 0);
		}
		else
		{
			// Parallel or triangle area is close to zero when
			// the plane normal not Normalized.
			return std::pair<bool, Real>(false, 0);
		}

		t = normal.Dot(a - ray.getOrigin()) / denom;

		if (t < 0)
		{
			// Intersection is behind origin
			return std::pair<bool, Real>(false, 0);
		}
	}

	//
	// Calculate the largest area projection plane in X, Y or Z.
	//
	int i0, i1;
	{
		Real n0 = Math::FAbs(normal[0]);
		Real n1 = Math::FAbs(normal[1]);
		Real n2 = Math::FAbs(normal[2]);

		i0 = 1; i1 = 2;
		if (n1 > n2)
		{
			if (n1 > n0) i0 = 0;
		}
		else
		{
			if (n2 > n0) i1 = 0;
		}
	}

	//
	// Check the intersection point is inside the triangle.
	//
	{
		Real u1 = b[i0] - a[i0];
		Real v1 = b[i1] - a[i1];
		Real u2 = c[i0] - a[i0];
		Real v2 = c[i1] - a[i1];
		Real u0 = t * ray.getDirection()[i0] + ray.getOrigin()[i0] - a[i0];
		Real v0 = t * ray.getDirection()[i1] + ray.getOrigin()[i1] - a[i1];

		Real alpha = u0 * v2 - u2 * v0;
		Real beta  = u1 * v0 - u0 * v1;
		Real area  = u1 * v2 - u2 * v1;

		// epsilon to avoid float precision error
		const Real EPSILON = 1e-3f;

		Real tolerance = - EPSILON * area;

		if (area > 0)
		{
			if (alpha < tolerance || beta < tolerance || alpha+beta > area-tolerance)
				return std::pair<bool, Real>(false, 0);
		}
		else
		{
			if (alpha > tolerance || beta > tolerance || alpha+beta < area-tolerance)
				return std::pair<bool, Real>(false, 0);
		}
	}

	return std::pair<bool, Real>(true, t);
}
//-----------------------------------------------------------------------
std::pair<bool, Real> Math::intersects(const Ray& ray, const Vector3& a,
	const Vector3& b, const Vector3& c,
	bool positiveSide, bool negativeSide)
{
	Vector3 normal = calculateBasicFaceNormalWithoutNormalize(a, b, c);
	return intersects(ray, a, b, c, normal, positiveSide, negativeSide);
}
//-----------------------------------------------------------------------
bool Math::intersects(const Sphere& sphere, const AxisAlignedBox& box)
{
	if (box.isNull()) return false;
	if (box.isInfinite()) return true;

	// Use splitting planes
	const Vector3& center = sphere.getCenter();
	Real radius = sphere.getRadius();
	const Vector3& min = box.getMinimum();
	const Vector3& max = box.getMaximum();

	// just test facing planes, early fail if sphere is totally outside
	if (min.x - center.x > radius)
	{
		return false;
	}
	if (center.x - max.x > radius)
	{
		return false;
	}

	if (min.y - center.y > radius)
	{
		return false;
	}
	if (center.y - max.y > radius)
	{
		return false;
	}

	if (min.z - center.z > radius)
	{
		return false;
	}
	if (center.z - max.z > radius)
	{
		return false;
	}

	// Must intersect
	return true;

}
//-----------------------------------------------------------------------
bool Math::intersects(const Plane& plane, const AxisAlignedBox& box)
{
	return (plane.GetSide(box) == BOTH_SIDE);
}
//-----------------------------------------------------------------------
bool Math::intersects(const Sphere& sphere, const Plane& plane)
{
	return (
		Math::FAbs(plane.Normal.Dot(sphere.getCenter()))
		<= sphere.getRadius() );
}
//-----------------------------------------------------------------------
Vector3 Math::calculateTangentSpaceVector(
	const Vector3& position1, const Vector3& position2, const Vector3& position3,
	Real u1, Real v1, Real u2, Real v2, Real u3, Real v3)
{
	//side0 is the vector along one side of the triangle of vertices passed in, 
	//and side1 is the vector along another side. Taking the cross product of these returns the normal.
	Vector3 side0 = position1 - position2;
	Vector3 side1 = position3 - position1;
	//Calculate face normal
	Vector3 normal = side1.Cross(side0);
	normal.Normalize();
	//Now we use a formula to calculate the tangent. 
	Real deltaV0 = v1 - v2;
	Real deltaV1 = v3 - v1;
	Vector3 tangent = deltaV1 * side0 - deltaV0 * side1;
	tangent.Normalize();
	//Calculate binormal
	Real deltaU0 = u1 - u2;
	Real deltaU1 = u3 - u1;
	Vector3 binormal = deltaU1 * side0 - deltaU0 * side1;
	binormal.Normalize();
	//Now, we take the cross product of the tangents to get a vector which 
	//should point in the same direction as our normal calculated above. 
	//If it points in the opposite direction (the dot product between the normals is less than zero), 
	//then we need to reverse the s and t tangents. 
	//This is because the triangle has been mirrored when going from tangent space to object space.
	//reverse tangents if necessary
	Vector3 tangentCross = tangent.Cross(binormal);
	if (tangentCross.Dot(normal) < 0.0f)
	{
		tangent = -tangent;
		binormal = -binormal;
	}

	return tangent;

}
//-----------------------------------------------------------------------
Matrix4 Math::buildReflectionMatrix(const Plane& p)
{
	return Matrix4(
		-2 * p.Normal.x * p.Normal.x + 1,   -2 * p.Normal.x * p.Normal.y,       -2 * p.Normal.x * p.Normal.z,       -2 * p.Normal.x * p.Constant, 
		-2 * p.Normal.y * p.Normal.x,       -2 * p.Normal.y * p.Normal.y + 1,   -2 * p.Normal.y * p.Normal.z,       -2 * p.Normal.y * p.Constant, 
		-2 * p.Normal.z * p.Normal.x,       -2 * p.Normal.z * p.Normal.y,       -2 * p.Normal.z * p.Normal.z + 1,   -2 * p.Normal.z * p.Constant, 
		0,                                  0,                                  0,                                  1);
}
//-----------------------------------------------------------------------
Vector4 Math::calculateFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
	Vector3 normal = calculateBasicFaceNormal(v1, v2, v3);
	// Now set up the w (distance of tri from origin
	return Vector4(normal.x, normal.y, normal.z, -(normal.Dot(v1)));
}
//-----------------------------------------------------------------------
Vector3 Math::calculateBasicFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
	Vector3 normal = (v2 - v1).Cross(v3 - v1);
	normal.Normalize();
	return normal;
}
//-----------------------------------------------------------------------
Vector4 Math::calculateFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
	Vector3 normal = calculateBasicFaceNormalWithoutNormalize(v1, v2, v3);
	// Now set up the w (distance of tri from origin)
	return Vector4(normal.x, normal.y, normal.z, -(normal.Dot(v1)));
}
//-----------------------------------------------------------------------
Vector3 Math::calculateBasicFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
	Vector3 normal = (v2 - v1).Cross(v3 - v1);
	return normal;
}
//-----------------------------------------------------------------------
Real Math::gaussianDistribution(Real x, Real offset, Real scale)
{
	Real nom = Math::Exp(
		-Math::Sqr(x - offset) / (2 * Math::Sqr(scale)));
	Real denom = scale * Math::Sqrt(2 * Math::PI);

	return nom / denom;

}
