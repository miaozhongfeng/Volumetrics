
#ifndef PLANE_H
#define PLANE_H

#include "Vector3.h"
#include "AxisAlignedBox.h"
#include "Matrix4.h"

// The "positive side" of the plane is the half space to which the plane
// normal points.  The "negative side" is the other half space.  The flag
// "no side" indicates the plane itself.
enum Side
{
	NO_SIDE,
	POSITIVE_SIDE,
	NEGATIVE_SIDE,
	BOTH_SIDE
};

class Plane
{
public:
    // The plane is represented as -Dot(N,X) = c where N is a unit-length
    // normal vector, c is the plane constant, and X is any point on the
    // plane.  The user must ensure that the normal vector satisfies this
    // condition.

	// ax+by+cz+d=0

    Plane ();  // uninitialized
    Plane (const Plane& rkPlane);

    // specify N and c directly
    Plane (const Vector3& rkNormal, Real fConstant);

    // N is specified, c = -Dot(N,P) where P is on the plane
    Plane (const Vector3& rkNormal, const Vector3& rkP);

    // N = Cross(P1-P0,P2-P0)/Length(Cross(P1-P0,P2-P0)), c = -Dot(N,P0) where
    // P0, P1, P2 are points on the plane.
    Plane (const Vector3& rkP0, const Vector3& rkP1,
        const Vector3& rkP2);

    // assignment
    Plane& operator= (const Plane& rkPlane);

	//return Returns a member point of the plane.
	Vector3 GetMemberPoint() const;

    // access plane P as P[0] = N.x, P[1] = N.y, P[2] = N.z, P[3] = c
    operator const Real* () const;
    operator Real* ();
    Real operator[] (int i) const;
    Real& operator[] (int i);

	Side GetSide (const Vector3& rkPoint) const;

	/**
	returns the side where the aligneBox is. the flag BOTH_SIDE indicates an intersecting box.
	one corner ON the plane is sufficient to consider the box and the plane intersecting.
	*/
	Side GetSide (const AxisAlignedBox& rkBox) const;

	/** Returns which side of the plane that the given box lies on.
		The box is defined as centre/half-size pairs for effectively.
	@param centre The centre of the box.
	@param halfSize The half-size of the box.
	@returns
		POSITIVE_SIDE if the box complete lies on the "positive side" of the plane,
		NEGATIVE_SIDE if the box complete lies on the "negative side" of the plane,
		and BOTH_SIDE if the box intersects the plane.
	*/
	Side GetSide (const Vector3& centre, const Vector3& halfSize) const;

    // Compute d = Dot(N,Q)+c where N is the plane normal and c is the plane
    // constant.  This is a signed distance.  The sign of the return value is
    // positive if the point is on the positive side of the plane, negative if
    // the point is on the negative side, and zero if the point is on the
    // plane.
    Real DistanceTo (const Vector3& rkQ) const;

	/** Project a vector onto the plane. 
	@remarks This gives you the element of the input vector that is perpendicular 
		to the normal of the plane. You can get the element which is parallel
		to the normal of the plane by subtracting the result of this method
		from the original vector, since parallel + perpendicular = original.
	@param v The input vector
	*/
	Vector3 ProjectVector(const Vector3& v);

	// If the plane is Dot(N,X) = c, arrange for the normal vector to be
    // unit length.  The new equation is Dot(N/|N|,X) = c/|N|.
    void Normalize ();

    /** Redefine this plane based on 3 points. */
    void Redefine(const Vector3& rkPoint0, const Vector3& rkPoint1,
        const Vector3& rkPoint2);

	/** Redefine this plane based on a normal and a point. */
	void Redefine(const Vector3& rkNormal, const Vector3& rkPoint);

    Vector3 getImageRv(const Vector3& rv);
    Vector3 getImageVector(const Vector3& vec);

	static const Plane XY;
	static const Plane XZ;
	static const Plane YZ;

    Vector3 Normal;
	union {
		Real Constant;
		Real D;
	};
};

typedef std::vector<Plane> PlaneList;

#include "Plane.inl"

inline Plane operator * (const Matrix4& m, const Plane& p)
{
    Plane ret;
	Matrix4 invTrans = m.Inverse();//.Transpose();
	Vector4 v4( p.Normal.x, p.Normal.y, p.Normal.z, p.D );
	v4 = invTrans * v4;
	ret.Normal.x = v4.x; 
	ret.Normal.y = v4.y; 
	ret.Normal.z = v4.z;
	ret.D = -v4.w / ret.Normal.Normalize();

    return ret;
}

#endif
