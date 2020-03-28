//----------------------------------------------------------------------------
inline Box3::Box3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
inline Box3::Box3 (const Vector3& rkCenter, const Vector3* akAxis,
    const Real* afExtent)
    :
    Center(rkCenter)
{
    for (int i = 0; i < 3; i++)
    {
        Axis[i] = akAxis[i];
        Extent[i] = afExtent[i];
    }
}
//----------------------------------------------------------------------------
inline Box3::Box3 (const Vector3& rkCenter, const Vector3& rkAxis0,
    const Vector3& rkAxis1, const Vector3& rkAxis2, Real fExtent0,
    Real fExtent1, Real fExtent2)
    :
    Center(rkCenter)
{
    Axis[0] = rkAxis0;
    Axis[1] = rkAxis1;
    Axis[2] = rkAxis2;
    Extent[0] = fExtent0;
    Extent[1] = fExtent1;
    Extent[2] = fExtent2;
}
//----------------------------------------------------------------------------
inline void Box3::ComputeVertices (Vector3 akVertex[8]) const
{
    Vector3 akEAxis[3] =
    {
        Extent[0]*Axis[0],
        Extent[1]*Axis[1],
        Extent[2]*Axis[2]
    };

    akVertex[0] = Center - akEAxis[0] - akEAxis[1] - akEAxis[2];
    akVertex[1] = Center + akEAxis[0] - akEAxis[1] - akEAxis[2];
    akVertex[2] = Center + akEAxis[0] + akEAxis[1] - akEAxis[2];
    akVertex[3] = Center - akEAxis[0] + akEAxis[1] - akEAxis[2];
    akVertex[4] = Center - akEAxis[0] - akEAxis[1] + akEAxis[2];
    akVertex[5] = Center + akEAxis[0] - akEAxis[1] + akEAxis[2];
    akVertex[6] = Center + akEAxis[0] + akEAxis[1] + akEAxis[2];
    akVertex[7] = Center - akEAxis[0] + akEAxis[1] + akEAxis[2];
}
//----------------------------------------------------------------------------
