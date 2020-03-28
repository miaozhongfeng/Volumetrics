#ifndef __Ray_H_
#define __Ray_H_

#include "Vector3.h"
//#include "PlaneBoundedVolume.h"

/** Representation of a ray in space, ie a line with an origin and direction. */
class Ray
{
protected:
    Vector3 mOrigin;
    Vector3 mDirection;
public:
    Ray():mOrigin(Vector3::ZERO), mDirection(Vector3::UNIT_Z) {}
    Ray(const Vector3& origin, const Vector3& direction)
        :mOrigin(origin), mDirection(direction) {}

	// distance from Ray to Point in 3d
	Real squaredDistanceTo(const Vector3& rkVector, Vector3& kClosestPoint0, Vector3& kClosestPoint1)
	{ 
		Vector3 kDiff = rkVector - mOrigin;
		Real fRayParameter = mDirection.Dot(kDiff);
		if (fRayParameter > (Real)0.0)
		{
			kClosestPoint1 = mOrigin + fRayParameter * mDirection;
		}
		else
		{
			kClosestPoint1 = mOrigin;
			fRayParameter = (Real)0.0;
		}

		kClosestPoint0 = rkVector;
		kDiff = kClosestPoint1 - kClosestPoint0;
		return kDiff.SquaredLength();
	}

	Real distanceTo(const Vector3& rkVector, Vector3& kClosestPoint0, Vector3& kClosestPoint1)
	{ 
		return Math::Sqrt(squaredDistanceTo(rkVector, kClosestPoint0, kClosestPoint1));
	}

    /** Sets the origin of the ray. */
    void setOrigin(const Vector3& origin) {mOrigin = origin;} 
    /** Gets the origin of the ray. */
    const Vector3& getOrigin(void) const {return mOrigin;} 

    /** Sets the direction of the ray. */
    void setDirection(const Vector3& dir) {mDirection = dir;} 
    /** Gets the direction of the ray. */
    const Vector3& getDirection(void) const {return mDirection;} 

	/** Gets the position of a point t units along the ray. */
	Vector3 getPoint(Real t) const { 
		return Vector3(mOrigin + (mDirection * t));
	}
	
	/** Gets the position of a point t units along the ray. */
	Vector3 operator*(Real t) const { 
		return getPoint(t);
	};

	/** Tests whether this ray intersects the given plane. 
	@returns A pair structure where the first element indicates whether
		an intersection occurs, and if true, the second element will
		indicate the distance along the ray at which it intersects. 
		This can be converted to a point in space by calling getPoint().
	*/
	std::pair<bool, Real> intersects(const Plane& p) const
	{
		return Math::intersects(*this, p);
	}
    /** Tests whether this ray intersects the given plane bounded volume. 
    @returns A pair structure where the first element indicates whether
    an intersection occurs, and if true, the second element will
    indicate the distance along the ray at which it intersects. 
    This can be converted to a point in space by calling getPoint().
    */
    /*std::pair<bool, Real> intersects(const PlaneBoundedVolume& p) const
    {
        return Math::intersects(*this, p.planes, p.outside == POSITIVE_SIDE);
    }*/
	/** Tests whether this ray intersects the given sphere. 
	@returns A pair structure where the first element indicates whether
		an intersection occurs, and if true, the second element will
		indicate the distance along the ray at which it intersects. 
		This can be converted to a point in space by calling getPoint().
	*/
	std::pair<bool, Real> intersects(const Sphere& s) const
	{
		return Math::intersects(*this, s);
	}
	/** Tests whether this ray intersects the given box. 
	@returns A pair structure where the first element indicates whether
		an intersection occurs, and if true, the second element will
		indicate the distance along the ray at which it intersects. 
		This can be converted to a point in space by calling getPoint().
	*/
	std::pair<bool, Real> intersects(const AxisAlignedBox& box) const
	{
		return Math::intersects(*this, box);
	}

};

#endif
