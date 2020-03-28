//----------------------------------------------------------------------------
inline Vector4::Vector4 ()
{
    // uninitialized for performance in array construction
}
//----------------------------------------------------------------------------
inline Vector4::Vector4 (Real fX, Real fY, Real fZ, Real fW)
{
    m_afTuple[0] = fX;
    m_afTuple[1] = fY;
    m_afTuple[2] = fZ;
    m_afTuple[3] = fW;
}
//----------------------------------------------------------------------------
inline Vector4::Vector4 (const Real* afTuple)
{
    m_afTuple[0] = afTuple[0];
    m_afTuple[1] = afTuple[1];
    m_afTuple[2] = afTuple[2];
    m_afTuple[3] = afTuple[3];
}
//----------------------------------------------------------------------------
inline Vector4::Vector4 (const Vector4& rkV)
{
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
    m_afTuple[2] = rkV.m_afTuple[2];
    m_afTuple[3] = rkV.m_afTuple[3];
}
//----------------------------------------------------------------------------
inline Vector4::operator const Real* () const
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
inline Vector4::operator Real* ()
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
inline Real Vector4::operator[] (int i) const
{
    return m_afTuple[i];
}
//----------------------------------------------------------------------------
inline Real& Vector4::operator[] (int i)
{
    return m_afTuple[i];
}
//----------------------------------------------------------------------------
inline Vector4& Vector4::operator= (const Vector4& rkV)
{
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
    m_afTuple[2] = rkV.m_afTuple[2];
    m_afTuple[3] = rkV.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
inline int Vector4::CompareArrays (const Vector4& rkV) const
{
    return memcmp(m_afTuple,rkV.m_afTuple,4*sizeof(Real));
}
//----------------------------------------------------------------------------
inline bool Vector4::operator== (const Vector4& rkV) const
{
    return CompareArrays(rkV) == 0;
}
//----------------------------------------------------------------------------
inline bool Vector4::operator!= (const Vector4& rkV) const
{
    return CompareArrays(rkV) != 0;
}
//----------------------------------------------------------------------------
inline bool Vector4::operator< (const Vector4& rkV) const
{
    if( x < rkV.x && y < rkV.y && z < rkV.z && w < rkV.w )
		return true;
    return false;
}
//----------------------------------------------------------------------------
inline bool Vector4::operator<= (const Vector4& rkV) const
{
    if( x <= rkV.x && y <= rkV.y && z <= rkV.z && w <= rkV.w )
		return true;
    return false;
}
//----------------------------------------------------------------------------
inline bool Vector4::operator> (const Vector4& rkV) const
{
    if( x > rkV.x && y > rkV.y && z > rkV.z && w > rkV.w )
		return true;
    return false;
}
//----------------------------------------------------------------------------
inline bool Vector4::operator>= (const Vector4& rkV) const
{
    if( x >= rkV.x && y >= rkV.y && z >= rkV.z && w >= rkV.w )
		return true;
    return false;
}
//----------------------------------------------------------------------------
inline Vector4 Vector4::operator+ (const Vector4& rkV) const
{
    return Vector4(
        m_afTuple[0]+rkV.m_afTuple[0],
        m_afTuple[1]+rkV.m_afTuple[1],
        m_afTuple[2]+rkV.m_afTuple[2],
        m_afTuple[3]+rkV.m_afTuple[3]);
}
//----------------------------------------------------------------------------
inline Vector4 Vector4::operator- (const Vector4& rkV) const
{
    return Vector4(
        m_afTuple[0]-rkV.m_afTuple[0],
        m_afTuple[1]-rkV.m_afTuple[1],
        m_afTuple[2]-rkV.m_afTuple[2],
        m_afTuple[3]-rkV.m_afTuple[3]);
}
//----------------------------------------------------------------------------
inline Vector4 Vector4::operator* (Real fScalar) const
{
    return Vector4(
        fScalar*m_afTuple[0],
        fScalar*m_afTuple[1],
        fScalar*m_afTuple[2],
        fScalar*m_afTuple[3]);
}
//----------------------------------------------------------------------------
inline Vector4 Vector4::operator/ (Real fScalar) const
{
    Vector4 kQuot;

    if (fScalar != (Real)0.0)
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        kQuot.m_afTuple[0] = fInvScalar*m_afTuple[0];
        kQuot.m_afTuple[1] = fInvScalar*m_afTuple[1];
        kQuot.m_afTuple[2] = fInvScalar*m_afTuple[2];
        kQuot.m_afTuple[3] = fInvScalar*m_afTuple[3];
    }
    else
    {
        kQuot.m_afTuple[0] = Math::MAX_REAL;
        kQuot.m_afTuple[1] = Math::MAX_REAL;
        kQuot.m_afTuple[2] = Math::MAX_REAL;
        kQuot.m_afTuple[3] = Math::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
inline Vector4 Vector4::operator- () const
{
    return Vector4(
        -m_afTuple[0],
        -m_afTuple[1],
        -m_afTuple[2],
        -m_afTuple[3]);
}
//----------------------------------------------------------------------------
inline Vector4 operator* (Real fScalar, const Vector4& rkV)
{
    return Vector4(
        fScalar*rkV[0],
        fScalar*rkV[1],
        fScalar*rkV[2],
        fScalar*rkV[3]);
}
//----------------------------------------------------------------------------
inline Vector4 operator/ (Real fScalar, const Vector4& rkV)
{
    return Vector4(
        fScalar/rkV[0],
        fScalar/rkV[1],
        fScalar/rkV[2],
        fScalar/rkV[3]);
}
//----------------------------------------------------------------------------
inline Vector4& Vector4::operator+= (const Vector4& rkV)
{
    m_afTuple[0] += rkV.m_afTuple[0];
    m_afTuple[1] += rkV.m_afTuple[1];
    m_afTuple[2] += rkV.m_afTuple[2];
    m_afTuple[3] += rkV.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
inline Vector4& Vector4::operator-= (const Vector4& rkV)
{
    m_afTuple[0] -= rkV.m_afTuple[0];
    m_afTuple[1] -= rkV.m_afTuple[1];
    m_afTuple[2] -= rkV.m_afTuple[2];
    m_afTuple[3] -= rkV.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
inline Vector4& Vector4::operator*= (Real fScalar)
{
    m_afTuple[0] *= fScalar;
    m_afTuple[1] *= fScalar;
    m_afTuple[2] *= fScalar;
    m_afTuple[3] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
inline Vector4& Vector4::operator/= (Real fScalar)
{
    if (fScalar != (Real)0.0)
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        m_afTuple[0] *= fInvScalar;
        m_afTuple[1] *= fInvScalar;
        m_afTuple[2] *= fInvScalar;
        m_afTuple[3] *= fInvScalar;
    }
    else
    {
        m_afTuple[0] = Math::MAX_REAL;
        m_afTuple[1] = Math::MAX_REAL;
        m_afTuple[2] = Math::MAX_REAL;
        m_afTuple[3] = Math::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
inline Real Vector4::Length () const
{
    return Math::Sqrt(
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] +
        m_afTuple[3]*m_afTuple[3]);
}
//----------------------------------------------------------------------------
inline Real Vector4::SquaredLength () const
{
    return
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] +
        m_afTuple[3]*m_afTuple[3];
}
//----------------------------------------------------------------------------
inline Real Vector4::Dot (const Vector4& rkV) const
{
    return
        m_afTuple[0]*rkV.m_afTuple[0] +
        m_afTuple[1]*rkV.m_afTuple[1] +
        m_afTuple[2]*rkV.m_afTuple[2] +
        m_afTuple[3]*rkV.m_afTuple[3];
}
//----------------------------------------------------------------------------
inline Real Vector4::Normalize ()
{
    Real fLength = Length();

    if (fLength > Math::ZERO_TOLERANCE)
    {
        Real fInvLength = ((Real)1.0)/fLength;
        m_afTuple[0] *= fInvLength;
        m_afTuple[1] *= fInvLength;
        m_afTuple[2] *= fInvLength;
        m_afTuple[3] *= fInvLength;
    }
    else
    {
        fLength = (Real)0.0;
        m_afTuple[0] = (Real)0.0;
        m_afTuple[1] = (Real)0.0;
        m_afTuple[2] = (Real)0.0;
        m_afTuple[3] = (Real)0.0;
    }

    return fLength;
}
//----------------------------------------------------------------------------