#ifndef QUATERNION_H
#define QUATERNION_H

#include "Matrix3.h"
#include "Matrix4.h"

class Quaternion
{
public:

	union
	{
		struct
		{
			Real w,x,y,z;
		};
		Real m_afTuple[4];
	};

    // A quaternion is q = w + x*i + y*j + z*k where (w,x,y,z) is not
    // necessarily a unit length vector in 4D.

    // construction
    Quaternion ();  // uninitialized
    Quaternion (Real fW, Real fX, Real fY, Real fZ);
    Quaternion (const Quaternion& rkQ);
	Quaternion (const Real* afTuple);

    // quaternion for the input rotation matrix
    Quaternion (const Matrix3& rkRot);

    // quaternion for the rotation of the axis-angle pair
    Quaternion (const Vector3& rkAxis, Real fAngle);

    // quaternion for the rotation matrix with specified columns
    Quaternion (const Vector3 akRotColumn[3]);

    /// Construct a quaternion from 3 orthonormal local axes
    Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
    {
        this->FromAxes(xaxis, yaxis, zaxis);
    }

    // member access:  0 = w, 1 = x, 2 = y, 3 = z
    inline operator const Real* () const;
    inline operator Real* ();
    inline Real operator[] (int i) const;
    inline Real& operator[] (int i);

    // assignment
    inline Quaternion& operator= (const Quaternion& rkQ);

    // comparison
    bool operator== (const Quaternion& rkQ) const;
    bool operator!= (const Quaternion& rkQ) const;
    bool operator<  (const Quaternion& rkQ) const;
    bool operator<= (const Quaternion& rkQ) const;
    bool operator>  (const Quaternion& rkQ) const;
    bool operator>= (const Quaternion& rkQ) const;

    // arithmetic operations
    inline Quaternion operator+ (const Quaternion& rkQ) const;
    inline Quaternion operator- (const Quaternion& rkQ) const;
    inline Quaternion operator* (const Quaternion& rkQ) const;
    inline Quaternion operator* (Real fScalar) const;
    inline Quaternion operator/ (Real fScalar) const;
    inline Quaternion operator- () const;
	inline Vector3 operator* (const Vector3& v) const;

    // arithmetic updates
    inline Quaternion& operator+= (const Quaternion& rkQ);
    inline Quaternion& operator-= (const Quaternion& rkQ);
    inline Quaternion& operator*= (Real fScalar);
    inline Quaternion& operator/= (Real fScalar);

    // conversion between quaternions, matrices, and axis-angle
    Quaternion& FromRotationMatrix (const Matrix3& rkRot);
	Quaternion& FromRotationMatrix (const Matrix4& rkRot);  //porting issues
    void ToRotationMatrix (Matrix3& rkRot) const;
    Quaternion& FromRotationMatrix (const Vector3 akRotColumn[3]);
    void ToRotationMatrix (Vector3 akRotColumn[3]) const;
    Quaternion& FromAxisAngle (const Vector3& rkAxis, Real fAngle);
    void ToAxisAngle (Vector3& rkAxis, Real& rfAngle) const;

	void FromAxes (const Vector3* akAxis);
	void FromAxes (const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis);
	void ToAxes (Vector3* akAxis) const;
	Vector3 xAxis(void) const;
	Vector3 yAxis(void) const;
	Vector3 zAxis(void) const;
	void ToAxes (Vector3& xaxis, Vector3& yaxis, Vector3& zaxis) const;

    // functions of a quaternion
    inline Real Length () const;  // length of 4-tuple
    inline Real SquaredLength () const;  // squared length of 4-tuple
    inline Real Dot (const Quaternion& rkQ) const;  // dot product of 4-tuples
    inline Real Normalize ();  // make the 4-tuple unit length
    Quaternion Inverse () const;  // apply to non-zero quaternion
    Quaternion Conjugate () const;
    Quaternion Exp () const;  // apply to quaternion with w = 0
    Quaternion Log () const;  // apply to unit-length quaternion

    // rotation of a vector by a quaternion
    Vector3 Rotate (const Vector3& rkVector) const;

    // spherical linear interpolation
    Quaternion& Slerp (Real fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath = false);

    Quaternion& SlerpExtraSpins (Real fT, const Quaternion& rkP,
        const Quaternion& rkQ, int iExtraSpins);

    // intermediate terms for spherical quadratic interpolation
    Quaternion& Intermediate (const Quaternion& rkQ0,
        const Quaternion& rkQ1, const Quaternion& rkQ2);

    // spherical quadratic interpolation
    Quaternion& Squad (Real fT, const Quaternion& rkQ0,
        const Quaternion& rkA0, const Quaternion& rkA1,
        const Quaternion& rkQ1, bool shortestPath = false);

	// normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
	Quaternion& NLerp(float fT, const Quaternion& rkP, 
		const Quaternion& rkQ, bool shortestPath = false);

    // Compute a quaternion that rotates unit-length vector V1 to unit-length
    // vector V2.  The rotation is about the axis perpendicular to both V1 and
    // V2, with angle of that between V1 and V2.  If V1 and V2 are parallel,
    // any axis of rotation will do, such as the permutation (z2,x2,y2), where
    // V2 = (x2,y2,z2).
    Quaternion& Align (const Vector3& rkV1, const Vector3& rkV2);

    // Decompose a quaternion into q = q_twist * q_swing, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    void DecomposeTwistTimesSwing (const Vector3& rkV1,
        Quaternion& rkTwist, Quaternion& rkSwing);

    // Decompose a quaternion into q = q_swing * q_twist, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    void DecomposeSwingTimesTwist (const Vector3& rkV1,
        Quaternion& rkSwing, Quaternion& rkTwist);

    // special values
    static const Quaternion IDENTITY;
    static const Quaternion ZERO;

	/// Equality with tolerance (tolerance is max angle difference)
	bool Equals(const Quaternion& rhs, const Real& tolerance) const;

	//-----------------------------------------------------------------------
    inline Quaternion UnitInverse () const
    {
        // assert:  'this' is unit length
        return Quaternion(w,-x,-y,-z);
    }

private:
    // support for comparisons
    int CompareArrays (const Quaternion& rkQ) const;

    // support for FromRotationMatrix
    static int ms_iNext[3];
};


inline Quaternion operator* (Real fScalar, const Quaternion& rkQ);

#include "Quaternion.inl"


#endif
