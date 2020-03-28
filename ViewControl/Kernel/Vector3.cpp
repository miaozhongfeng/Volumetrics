
#include "stdafx.h"
#include "Vector3.h"
#include "Quaternion.h"

const Vector3 Vector3::ZERO(0.0,0.0,0.0);
const Vector3 Vector3::UNIT_X(1.0,0.0,0.0);
const Vector3 Vector3::UNIT_Y(0.0,1.0,0.0);
const Vector3 Vector3::UNIT_Z(0.0,0.0,1.0);
const Vector3 Vector3::NEGATIVE_UNIT_X(-1.0, 0.0, 0.0);
const Vector3 Vector3::NEGATIVE_UNIT_Y( 0.0,-1.0, 0.0);
const Vector3 Vector3::NEGATIVE_UNIT_Z( 0.0, 0.0,-1.0);
const Vector3 Vector3::ONE(1.0,1.0,1.0);
const Vector3 Vector3::UNIT_SCALE(1.0,1.0,1.0);

//----------------------------------------------------------------------------
Quaternion Vector3::GetRotationTo(const Vector3& dest,
	const Vector3& fallbackAxis) const
{
    // Based on Stan Melax's article in Game Programming Gems
    Quaternion q;
    // Copy, since cannot modify local
    Vector3 v0 = *this;
    Vector3 v1 = dest;
    v0.Normalize();
    v1.Normalize();

    Real d = v0.Dot(v1);
    // If dot == 1, vectors are the same
    if (d >= 1.0f)
    {
        return Quaternion::IDENTITY;
    }
	if (d < (1e-6f - 1.0f))
	{
		if (fallbackAxis != Vector3::ZERO)
		{
			// rotate 180 degrees about the fallback axis
			q.FromAxisAngle(fallbackAxis, Real(Math::PI));
		}
		else
		{
			// Generate an axis
			Vector3 axis = Vector3::UNIT_X.Cross(*this);
			if (axis.IsZeroLength()) // pick another if colinear
				axis = Vector3::UNIT_Y.Cross(*this);
			axis.Normalize();
			q.FromAxisAngle(axis, Real(Math::PI));
		}
	}
	else
	{
        Real s = Math::Sqrt( (1+d)*2 );
        Real invs = 1 / s;

		Vector3 c = v0.Cross(v1);

        q.x = c.x * invs;
	    q.y = c.y * invs;
    	q.z = c.z * invs;
    	q.w = s * (Real)0.5;
		q.Normalize();
	}
    return q;
}
