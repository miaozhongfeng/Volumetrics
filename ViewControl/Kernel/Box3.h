#ifndef __Box3_H_
#define __Box3_H_

#include "Vector3.h"
/*
 A box has center C, axis directions U[0], U[1], and U[2] (all
 unit-length vectors), and extents e[0], e[1], and e[2] (all nonnegative
 numbers).  A point X = C+y[0]*U[0]+y[1]*U[1]+y[2]*U[2] is inside or
 on the box whenever |y[i]| <= e[i] for all i.
*/
class Box3
{
public:
    // construction
    Box3 ();  // uninitialized
    Box3 (const Vector3& rkCenter, const Vector3* akAxis,
        const Real* afExtent);
    Box3 (const Vector3& rkCenter, const Vector3& rkAxis0,
        const Vector3& rkAxis1, const Vector3& rkAxis2,
        Real fExtent0, Real fExtent1, Real fExtent2);

    void ComputeVertices (Vector3 akVertex[8]) const;

    Vector3 Center;
    Vector3 Axis[3];  // must be an orthonormal set of vectors
    Real Extent[3];         // must be nonnegative
};

#include "Box3.inl"

#endif