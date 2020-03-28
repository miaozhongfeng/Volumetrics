#ifndef VECTOR4_H
#define VECTOR4_H

#include "Math.h"
#include "Vector3.h"

class Vector4
{
public:

	union 
	{
        struct 
		{
            float x, y, z, w;        
        };
        float m_afTuple[4];
    };

    // construction
    Vector4 ();  // uninitialized
    Vector4 (Real fX, Real fY, Real fZ, Real fW);
    Vector4 (const Real* afTuple);
    Vector4 (const Vector4& rkV);

	inline explicit Vector4(const Vector3& rhs)
		: x(rhs.x), y(rhs.y), z(rhs.z), w(1.0f) { }

    // coordinate access
    inline operator const Real* () const;
    inline operator Real* ();
    inline Real operator[] (int i) const;
    inline Real& operator[] (int i);

    // assignment
    inline Vector4& operator= (const Vector4& rkV);
    inline Vector4& operator= (const Vector3& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = 1.0f;
        return *this;
    }

    // comparison
    bool operator== (const Vector4& rkV) const;
    bool operator!= (const Vector4& rkV) const;
    bool operator<  (const Vector4& rkV) const;
    bool operator<= (const Vector4& rkV) const;
    bool operator>  (const Vector4& rkV) const;
    bool operator>= (const Vector4& rkV) const;

    // arithmetic operations
    inline Vector4 operator+ (const Vector4& rkV) const;
    inline Vector4 operator- (const Vector4& rkV) const;
    inline Vector4 operator* (Real fScalar) const;
    inline Vector4 operator/ (Real fScalar) const;
    inline Vector4 operator- () const;

    // arithmetic updates
    inline Vector4& operator+= (const Vector4& rkV);
    inline Vector4& operator-= (const Vector4& rkV);
    inline Vector4& operator*= (Real fScalar);
    inline Vector4& operator/= (Real fScalar);

    // vector operations
    inline Real Length () const;
    inline Real SquaredLength () const;
    inline Real Dot (const Vector4& rkV) const;
    inline Real Normalize ();

    // special vectors
    static const Vector4 ZERO;
    static const Vector4 UNIT_X;  // (1,0,0,0)
    static const Vector4 UNIT_Y;  // (0,1,0,0)
    static const Vector4 UNIT_Z;  // (0,0,1,0)
    static const Vector4 UNIT_W;  // (0,0,0,1)
    static const Vector4 ONE;     // (1,1,1,1)

private:
    // support for comparisons
    int CompareArrays (const Vector4& rkV) const;
};

// arithmetic operations

Vector4 operator* (Real fScalar, const Vector4& rkV);
Vector4 operator/ (Real fScalar, const Vector4& rkV);


#include "Vector4.inl"


#endif
