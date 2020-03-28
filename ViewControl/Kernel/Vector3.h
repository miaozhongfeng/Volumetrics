#ifndef VECTOR3_H
#define VECTOR3_H

#include "Math.h"

class Vector3
{
public:

	union
	{
		struct
		{
			Real x,y,z;
		};
		Real m_afTuple[3];
	};

    // construction
    Vector3 ();  // uninitialized
    Vector3 (Real fX, Real fY, Real fZ);
    Vector3 (const Real* afTuple);
    Vector3 (const Vector3& rkV);

    // coordinate access
    /*inline operator const Real* () const;
    inline operator Real* ();*/
    inline Real operator[] (const int i) const;
    inline Real& operator[] (const int i);

    // assignment
    inline Vector3& operator= (const Vector3& rkV);

    // comparison
    bool operator== (const Vector3& rkV) const;
    bool operator!= (const Vector3& rkV) const;
    bool operator<  (const Vector3& rkV) const;
    bool operator<= (const Vector3& rkV) const;
    bool operator>  (const Vector3& rkV) const;
    bool operator>= (const Vector3& rkV) const;

    // arithmetic operations
    inline Vector3 operator+ (const Vector3& rkV) const;
    inline Vector3 operator- (const Vector3& rkV) const;
	inline Vector3 operator* (const Vector3& rkV) const;
	inline Vector3 operator/ (const Vector3& rkV) const;
    inline Vector3 operator* (Real fScalar) const;
    inline Vector3 operator/ (Real fScalar) const;
    inline Vector3 operator- () const;

    // arithmetic updates
    inline Vector3& operator+= (const Vector3& rkV);
    inline Vector3& operator-= (const Vector3& rkV);
	inline Vector3& operator*= (const Vector3& rkV);
    inline Vector3& operator*= (Real fScalar);
    inline Vector3& operator/= (Real fScalar);

	inline Vector3 operator% (const Vector3& rkV) const;

	// arithmetic operations
	inline friend Vector3 operator* (Real fScalar, const Vector3& rkV);

    // vector operations
    inline Real Length () const;
    inline Real SquaredLength () const;
    inline Real Dot (const Vector3& rkV) const;
    /** Calculates the absolute dot (scalar) product of this vector with another.
        @remarks
            This function work similar dotProduct, except it use absolute value
            of each component of the vector to computing.
        @param
            vec Vector with which to calculate the absolute dot product (together
            with this one).
        @returns
            A Real representing the absolute dot product value.
    */
    inline Real AbsDot(const Vector3& vec) const
    {
        return Math::FAbs(x * vec.x) + Math::FAbs(y * vec.y) + Math::FAbs(z * vec.z);
    }

    inline Real Normalize ();

    // The cross products are computed using the right-handed rule.  Be aware
    // that some graphics APIs use a left-handed rule.  If you have to compute
    // a cross product with these functions and send the result to the API
    // that expects left-handed, you will need to change sign on the vector
    // (replace each component value c by -c).
    inline Vector3 Cross (const Vector3& rkV) const;
    inline Vector3 UnitCross (const Vector3& rkV) const;

    // Compute the barycentric coordinates of the point with respect to the
    // tetrahedron <V0,V1,V2,V3>, P = b0*V0 + b1*V1 + b2*V2 + b3*V3, where
    // b0 + b1 + b2 + b3 = 1.
    void GetBarycentrics (const Vector3& rkV0, const Vector3& rkV1,
        const Vector3& rkV2, const Vector3& rkV3, Real afBary[4]) const;

    // Gram-Schmidt orthonormalization.  Take linearly independent vectors
    // U, V, and W and compute an orthonormal set (unit length, mutually
    // perpendicular).
    static void Orthonormalize (Vector3& rkU, Vector3& rkV, Vector3& rkW);
    static void Orthonormalize (Vector3* akV);

    // Input W must be a nonzero vector. The output is an orthonormal basis
    // {U,V,W}.  The input W is normalized by this function.  If you know
    // W is already unit length, use GenerateComplementBasis to compute U
    // and V.
    static void GenerateOrthonormalBasis (Vector3& rkU, Vector3& rkV,
        Vector3& rkW);

    // Input W must be a unit-length vector.  The output vectors {U,V} are
    // unit length and mutually perpendicular, and {U,V,W} is an orthonormal
    // basis.
    static void GenerateComplementBasis (Vector3& rkU, Vector3& rkV,
        const Vector3& rkW);

    // Compute the extreme values.
    static void ComputeExtremes (int iVQuantity, const Vector3* akPoint,
        Vector3& rkMin, Vector3& rkMax);

	/** Returns whether this vector is within a positional tolerance
		of another vector.
	@param rhs The vector to compare with
	@param tolerance The amount that each element of the vector may vary by 
		and still be considered equal
	*/
	inline bool PositionEquals(const Vector3& rhs, Real tolerance = 1e-03) const
	{
		return Math::RealEqual(x, rhs.x, tolerance) &&
			Math::RealEqual(y, rhs.y, tolerance) &&
			Math::RealEqual(z, rhs.z, tolerance);
		
	}

	/** Returns whether this vector is within a directional tolerance
		of another vector.
	@param rhs The vector to compare with
	@param tolerance The maximum angle by which the vectors may vary and
		still be considered equal
	@note Both vectors should be normalised.
	*/
	inline bool DirectionEquals(const Vector3& rhs,
		const Real& tolerance) const
	{
		Real dot = Dot(rhs);
		Real angle = Math::ACos(dot);

		return Math::FAbs(angle) <= tolerance;

	}

    /** Sets this vector's components to the minimum of its own and the
        ones of the passed in vector.
        @remarks
            'Minimum' in this case means the combination of the lowest
            value of x, y and z from both vectors. Lowest is taken just
            numerically, not magnitude, so -1 < 0.
    */
    inline void MakeFloor( const Vector3& cmp )
    {
        if( cmp.x < x ) x = cmp.x;
        if( cmp.y < y ) y = cmp.y;
        if( cmp.z < z ) z = cmp.z;
    }

    /** Sets this vector's components to the maximum of its own and the
        ones of the passed in vector.
        @remarks
            'Maximum' in this case means the combination of the highest
            value of x, y and z from both vectors. Highest is taken just
            numerically, not magnitude, so 1 > -3.
    */
    inline void MakeCeil( const Vector3& cmp )
    {
        if( cmp.x > x ) x = cmp.x;
        if( cmp.y > y ) y = cmp.y;
        if( cmp.z > z ) z = cmp.z;
    }

    /** As normalise, except that this vector is unaffected and the
        normalised vector is returned as a copy. */
    inline Vector3 NormalizedCopy(void) const
    {
        Vector3 ret = *this;
        ret.Normalize();
        return ret;
    }

    /** Calculates a reflection vector to the plane with the given normal .
		@remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
		*/
    inline Vector3 Reflect(const Vector3& normal) const
    {
        return Vector3( *this - ( 2 * this->Dot(normal) * normal ) );
    }

    /** Returns true if this vector is zero length. */
    inline bool IsZeroLength(void) const
    {
        Real sqlen = (x * x) + (y * y) + (z * z);
        return (sqlen < (1e-06 * 1e-06));

    }

    /** Returns a vector at a point half way between this and the passed
        in vector.
    */
    inline Vector3 MidPoint( const Vector3& vec ) const
    {
        return Vector3(
            ( x + vec.x ) * (Real)0.5,
            ( y + vec.y ) * (Real)0.5,
            ( z + vec.z ) * (Real)0.5 );
    }

	/** Gets the shortest arc quaternion to rotate this vector to the destination
        vector.
    @remarks
        If you call this with a dest vector that is close to the inverse
        of this vector, we will rotate 180 degrees around the 'fallbackAxis'
		(if specified, or a generated axis if not) since in this case
		ANY axis of rotation is valid.
    */
    Quaternion GetRotationTo(const Vector3& dest,
		const Vector3& fallbackAxis = Vector3::ZERO) const;

    // special vectors
    static const Vector3 ZERO;    // (0,0,0)
    static const Vector3 UNIT_X;  // (1,0,0)
    static const Vector3 UNIT_Y;  // (0,1,0)
    static const Vector3 UNIT_Z;  // (0,0,1)
    static const Vector3 NEGATIVE_UNIT_X;  // (-1,0,0)
    static const Vector3 NEGATIVE_UNIT_Y;  // (0,-1,0)
    static const Vector3 NEGATIVE_UNIT_Z;  // (0,0,-1)
    static const Vector3 ONE;     // (1,1,1)
	static const Vector3 UNIT_SCALE; // (1,1,1)

private:
    // support for comparisons
    int CompareArrays (const Vector3& rkV) const;
};

#include "Vector3.inl"


#endif
