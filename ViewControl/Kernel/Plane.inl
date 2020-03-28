//----------------------------------------------------------------------------
inline Plane::Plane ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
inline Plane::Plane (const Plane& rkPlane)
    :
    Normal(rkPlane.Normal)
{
    Constant = rkPlane.Constant;
}
//----------------------------------------------------------------------------
inline Plane::Plane (const Vector3& rkNormal, Real fConstant)
    :
    Normal(rkNormal)
{
    Constant = fConstant;
}
//----------------------------------------------------------------------------
inline Plane::Plane (const Vector3& rkNormal, const Vector3& rkP)
    :
    Normal(rkNormal)
{
    Constant = -rkNormal.Dot(rkP);
}
//----------------------------------------------------------------------------
inline Plane::Plane (const Vector3& rkP0, const Vector3& rkP1,
    const Vector3& rkP2)
{
    Vector3 kEdge1 = rkP1 - rkP0;
    Vector3 kEdge2 = rkP2 - rkP0;
    Normal = kEdge1.UnitCross(kEdge2);
    Constant = -Normal.Dot(rkP0);
}
//----------------------------------------------------------------------------
inline Plane& Plane::operator= (const Plane& rkPlane)
{
    Normal = rkPlane.Normal;
    Constant = rkPlane.Constant;
    return *this;
}
//----------------------------------------------------------------------------
inline Real Plane::DistanceTo (const Vector3& rkP) const
{
    return Normal.Dot(rkP) + Constant;
}
//----------------------------------------------------------------------------
inline Side Plane::GetSide (const Vector3& rkPoint) const
{
	Real fDistance = DistanceTo(rkPoint);

	if ( fDistance < 0.0 )
		return NEGATIVE_SIDE;

	if ( fDistance > 0.0 )
		return POSITIVE_SIDE;

	return NO_SIDE;
}
//----------------------------------------------------------------------------
inline Side Plane::GetSide (const AxisAlignedBox& box) const
{
	if (box.isNull()) 
		return NO_SIDE;
	if (box.isInfinite())
		return BOTH_SIDE;

    return GetSide(box.getCenter(), box.getHalfSize());
}
//----------------------------------------------------------------------------
inline Side Plane::GetSide (const Vector3& centre, const Vector3& halfSize) const
{
    // Calculate the distance between box centre and the plane
    Real dist = DistanceTo(centre);

    // Calculate the maximise allows absolute distance for
    // the distance between box centre and plane
    Real maxAbsDist = Normal.AbsDot(halfSize);

    if (dist < -maxAbsDist)
        return NEGATIVE_SIDE;

    if (dist > +maxAbsDist)
        return POSITIVE_SIDE;

    return BOTH_SIDE;
}
//----------------------------------------------------------------------------
inline Real Plane::operator[] (int i) const
{
    assert( 0 <= i && i < 4 );
    return ((Real*)this)[i];
}
//----------------------------------------------------------------------------
inline Real& Plane::operator[] (int i)
{
    assert( 0 <= i && i < 4 );
    return ((Real*)this)[i];
}
//----------------------------------------------------------------------------
inline void Plane::Normalize ()
{
    // assert:  |N| > 0
    Real fSqrLength = Normal.SquaredLength();
	Real fInvLength = Math::InvSqrt(fSqrLength);
    for (int i = 0; i < 4; i++)
        ((Real*)this)[i] *= fInvLength;
}
//----------------------------------------------------------------------------
inline Plane::operator const Real* () const
{
    return (Real*)this;
}
//----------------------------------------------------------------------------
inline Plane::operator Real* ()
{
    return (Real*)this;
}
//----------------------------------------------------------------------------
inline Vector3 Plane::GetMemberPoint() const
{
	return Normal * -Constant;
}
//-----------------------------------------------------------------------
inline void Plane::Redefine(const Vector3& rkPoint0, const Vector3& rkPoint1,
	const Vector3& rkPoint2)
{
    Vector3 kEdge1 = rkPoint1 - rkPoint0;
    Vector3 kEdge2 = rkPoint2 - rkPoint0;
    Normal = kEdge1.UnitCross(kEdge2);
    Constant = -Normal.Dot(rkPoint0);
}
//-----------------------------------------------------------------------
inline void Plane::Redefine(const Vector3& rkNormal, const Vector3& rkPoint)
{
	Normal = rkNormal;
	Constant = -rkNormal.Dot(rkPoint);
}
//-----------------------------------------------------------------------
inline Vector3 Plane::getImageRv(const Vector3& rv)
{
    Real d = (Normal.Dot(rv - GetMemberPoint()));
    return ( rv - 2*d*Normal ) ;
}
inline Vector3 Plane::getImageVector(const Vector3& vec)
{
    return ( vec - 2.0 * Normal *(Normal.Dot(vec))); 
}