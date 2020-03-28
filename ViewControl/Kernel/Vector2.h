#ifndef VECTOR2_H
#define VECTOR2_H

#include "Math.h"

class Vector2
{
public:

	union
	{
		struct
		{
			Real x,y;
		};
		Real m_afTuple[2];
	};

    // construction
    Vector2 ();  // uninitialized
    Vector2 (Real fX, Real fY);
    Vector2 (const Real* afTuple);
    Vector2 (const Vector2& rkV);

    // coordinate access
    inline operator const Real* () const;
    inline operator Real* ();
    inline Real operator[] (int i) const;
    inline Real& operator[] (int i);

    // assignment
    inline Vector2& operator= (const Vector2& rkV);

    // comparison
    bool operator== (const Vector2& rkV) const;
    bool operator!= (const Vector2& rkV) const;
    bool operator<  (const Vector2& rkV) const;
    bool operator<= (const Vector2& rkV) const;
    bool operator>  (const Vector2& rkV) const;
    bool operator>= (const Vector2& rkV) const;

    // arithmetic operations
    inline Vector2 operator+ (const Vector2& rkV) const;
    inline Vector2 operator- (const Vector2& rkV) const;
    inline Vector2 operator* (Real fScalar) const;
    inline Vector2 operator/ (Real fScalar) const;
    inline Vector2 operator- () const;

    // arithmetic updates
    inline Vector2& operator+= (const Vector2& rkV);
    inline Vector2& operator-= (const Vector2& rkV);
    inline Vector2& operator*= (Real fScalar);
    inline Vector2& operator/= (Real fScalar);

    // vector operations
    inline Real Length () const;
    inline Real SquaredLength () const;
    inline Real Dot (const Vector2& rkV) const;
    inline Real Normalize ();

    // returns (y,-x)
    inline Vector2 Perp () const;

    // returns (y,-x)/sqrt(x*x+y*y)
    inline Vector2 UnitPerp () const;

    // returns DotPerp((x,y),(V.x,V.y)) = x*V.y - y*V.x
    inline Real DotPerp (const Vector2& rkV) const;

	/** Calculates the 2 dimensional cross-product of 2 vectors, which results
		in a single floating point value which is 2 times the area of the triangle.
    */
    inline Real Cross( const Vector2& rkVector ) const
    {
        return x * rkVector.y - y * rkVector.x;
    }

    // Compute the barycentric coordinates of the point with respect to the
    // triangle <V0,V1,V2>, P = b0*V0 + b1*V1 + b2*V2, where b0 + b1 + b2 = 1.
    void GetBarycentrics (const Vector2& rkV0, const Vector2& rkV1,
        const Vector2& rkV2, Real afBary[3]) const;

    // Gram-Schmidt orthonormalization.  Take linearly independent vectors U
    // and V and compute an orthonormal set (unit length, mutually
    // perpendicular).
    static void Orthonormalize (Vector2& rkU, Vector2& rkV);

    // Input V must be a nonzero vector.  The output is an orthonormal basis
    // {U,V}.  The input V is normalized by this function.  If you know V is
    // already unit length, use U = V.Perp().
    static void GenerateOrthonormalBasis (Vector2& rkU, Vector2& rkV);

    // Compute the extreme values.
    static void ComputeExtremes (int iVQuantity, const Vector2* akPoint,
        Vector2& rkMin, Vector2& rkMax);

    // special vectors
    static const Vector2 ZERO;    // (0,0)
    static const Vector2 UNIT_X;  // (1,0)
    static const Vector2 UNIT_Y;  // (0,1)
    static const Vector2 ONE;     // (1,1)

private:
    // support for comparisons
    int CompareArrays (const Vector2& rkV) const;
};

// arithmetic operations

Vector2 operator* (Real fScalar, const Vector2& rkV);

#include "Vector2.inl"


#endif
