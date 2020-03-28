//----------------------------------------------------------------------------
inline Quaternion::Quaternion ()
{
    // uninitialized for performance in array construction
}
//----------------------------------------------------------------------------
inline Quaternion::Quaternion (Real fW, Real fX, Real fY, Real fZ)
{
    m_afTuple[0] = fW;
    m_afTuple[1] = fX;
    m_afTuple[2] = fY;
    m_afTuple[3] = fZ;
}
//----------------------------------------------------------------------------
inline Quaternion::Quaternion (const Quaternion& rkQ)
{
    m_afTuple[0] = rkQ.m_afTuple[0];
    m_afTuple[1] = rkQ.m_afTuple[1];
    m_afTuple[2] = rkQ.m_afTuple[2];
    m_afTuple[3] = rkQ.m_afTuple[3];
}
//----------------------------------------------------------------------------
inline Quaternion::Quaternion (const Real* afTuple)
{
    m_afTuple[0] = afTuple[0];
    m_afTuple[1] = afTuple[1];
    m_afTuple[2] = afTuple[2];
    m_afTuple[3] = afTuple[3];
}
//----------------------------------------------------------------------------
inline Quaternion::Quaternion (const Matrix3& rkRot)
{
    FromRotationMatrix(rkRot);
}
//----------------------------------------------------------------------------
inline Quaternion::Quaternion (const Vector3& rkAxis, Real fAngle)
{
    FromAxisAngle(rkAxis,fAngle);
}
//----------------------------------------------------------------------------
inline Quaternion::Quaternion (const Vector3 akRotColumn[3])
{
    FromRotationMatrix(akRotColumn);
}
//----------------------------------------------------------------------------
inline Quaternion::operator const Real* () const
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
inline Quaternion::operator Real* ()
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
inline Real Quaternion::operator[] (int i) const
{
    return m_afTuple[i];
}
//----------------------------------------------------------------------------
inline Real& Quaternion::operator[] (int i)
{
    return m_afTuple[i];
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::operator= (const Quaternion& rkQ)
{
    m_afTuple[0] = rkQ.m_afTuple[0];
    m_afTuple[1] = rkQ.m_afTuple[1];
    m_afTuple[2] = rkQ.m_afTuple[2];
    m_afTuple[3] = rkQ.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
inline int Quaternion::CompareArrays (const Quaternion& rkQ) const
{
    return memcmp(m_afTuple,rkQ.m_afTuple,4*sizeof(Real));
}
//----------------------------------------------------------------------------
inline bool Quaternion::operator== (const Quaternion& rkQ) const
{
    return CompareArrays(rkQ) == 0;
}
//----------------------------------------------------------------------------
inline bool Quaternion::operator!= (const Quaternion& rkQ) const
{
    return CompareArrays(rkQ) != 0;
}
//----------------------------------------------------------------------------
inline bool Quaternion::operator< (const Quaternion& rkQ) const
{
    return CompareArrays(rkQ) < 0;
}
//----------------------------------------------------------------------------
inline bool Quaternion::operator<= (const Quaternion& rkQ) const
{
    return CompareArrays(rkQ) <= 0;
}
//----------------------------------------------------------------------------
inline bool Quaternion::operator> (const Quaternion& rkQ) const
{
    return CompareArrays(rkQ) > 0;
}
//----------------------------------------------------------------------------
inline bool Quaternion::operator>= (const Quaternion& rkQ) const
{
    return CompareArrays(rkQ) >= 0;
}
//----------------------------------------------------------------------------
inline Quaternion Quaternion::operator+ (
    const Quaternion& rkQ) const
{
    Quaternion kSum;
    for (int i = 0; i < 4; i++)
    {
        kSum.m_afTuple[i] = m_afTuple[i] + rkQ.m_afTuple[i];
    }
    return kSum;
}
//----------------------------------------------------------------------------
inline Quaternion Quaternion::operator- (
    const Quaternion& rkQ) const
{
    Quaternion kDiff;
    for (int i = 0; i < 4; i++)
    {
        kDiff.m_afTuple[i] = m_afTuple[i] - rkQ.m_afTuple[i];
    }
    return kDiff;
}
//----------------------------------------------------------------------------
inline Quaternion Quaternion::operator* (
    const Quaternion& rkQ) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

    Quaternion kProd;

    kProd.m_afTuple[0] =
        m_afTuple[0]*rkQ.m_afTuple[0] -
        m_afTuple[1]*rkQ.m_afTuple[1] -
        m_afTuple[2]*rkQ.m_afTuple[2] -
        m_afTuple[3]*rkQ.m_afTuple[3];

    kProd.m_afTuple[1] =
        m_afTuple[0]*rkQ.m_afTuple[1] +
        m_afTuple[1]*rkQ.m_afTuple[0] +
        m_afTuple[2]*rkQ.m_afTuple[3] -
        m_afTuple[3]*rkQ.m_afTuple[2];

    kProd.m_afTuple[2] =
        m_afTuple[0]*rkQ.m_afTuple[2] +
        m_afTuple[2]*rkQ.m_afTuple[0] +
        m_afTuple[3]*rkQ.m_afTuple[1] -
        m_afTuple[1]*rkQ.m_afTuple[3];

    kProd.m_afTuple[3] =
        m_afTuple[0]*rkQ.m_afTuple[3] +
        m_afTuple[3]*rkQ.m_afTuple[0] +
        m_afTuple[1]*rkQ.m_afTuple[2] -
        m_afTuple[2]*rkQ.m_afTuple[1];

    return kProd;
}
//----------------------------------------------------------------------------
inline Quaternion Quaternion::operator* (Real fScalar) const
{
    Quaternion kProd;
    for (int i = 0; i < 4; i++)
    {
        kProd.m_afTuple[i] = fScalar*m_afTuple[i];
    }
    return kProd;
}
//----------------------------------------------------------------------------
inline Quaternion Quaternion::operator/ (Real fScalar) const
{
    Quaternion kQuot;
    int i;

    if (fScalar != (Real)0.0)
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        for (i = 0; i < 4; i++)
        {
            kQuot.m_afTuple[i] = fInvScalar*m_afTuple[i];
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            kQuot.m_afTuple[i] = Math::MAX_REAL;
        }
    }

    return kQuot;
}
//----------------------------------------------------------------------------
inline Quaternion Quaternion::operator- () const
{
    Quaternion kNeg;
    for (int i = 0; i < 4; i++)
    {
        kNeg.m_afTuple[i] = -m_afTuple[i];
    }
    return kNeg;
}
//----------------------------------------------------------------------------
inline Quaternion operator* (Real fScalar, const Quaternion& rkQ)
{
    Quaternion kProd;
    for (int i = 0; i < 4; i++)
    {
        kProd[i] = fScalar*rkQ[i];
    }
    return kProd;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::operator+= (const Quaternion& rkQ)
{
    for (int i = 0; i < 4; i++)
    {
        m_afTuple[i] += rkQ.m_afTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::operator-= (const Quaternion& rkQ)
{
    for (int i = 0; i < 4; i++)
    {
        m_afTuple[i] -= rkQ.m_afTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::operator*= (Real fScalar)
{
    for (int i = 0; i < 4; i++)
    {
        m_afTuple[i] *= fScalar;
    }
    return *this;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::operator/= (Real fScalar)
{
    int i;

    if (fScalar != (Real)0.0)
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        for (i = 0; i < 4; i++)
        {
            m_afTuple[i] *= fInvScalar;
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            m_afTuple[i] = Math::MAX_REAL;
        }
    }

    return *this;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::FromRotationMatrix (
    const Matrix3& rkRot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    Real fTrace = rkRot(0,0) + rkRot(1,1) + rkRot(2,2);
    Real fRoot;

    if (fTrace > (Real)0.0)
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = Math::Sqrt(fTrace + (Real)1.0);  // 2w
        m_afTuple[0] = ((Real)0.5)*fRoot;
        fRoot = ((Real)0.5)/fRoot;  // 1/(4w)
        m_afTuple[1] = (rkRot(2,1)-rkRot(1,2))*fRoot;
        m_afTuple[2] = (rkRot(0,2)-rkRot(2,0))*fRoot;
        m_afTuple[3] = (rkRot(1,0)-rkRot(0,1))*fRoot;
    }
    else
    {
        // |w| <= 1/2
        int i = 0;
        if ( rkRot(1,1) > rkRot(0,0) )
        {
            i = 1;
        }
        if ( rkRot(2,2) > rkRot(i,i) )
        {
            i = 2;
        }
        int j = ms_iNext[i];
        int k = ms_iNext[j];

        fRoot = Math::Sqrt(rkRot(i,i)-rkRot(j,j)-rkRot(k,k)+(Real)1.0);
        Real* apfQuat[3] = { &m_afTuple[1], &m_afTuple[2], &m_afTuple[3] };
        *apfQuat[i] = ((Real)0.5)*fRoot;
        fRoot = ((Real)0.5)/fRoot;
        m_afTuple[0] = (rkRot(k,j)-rkRot(j,k))*fRoot;
        *apfQuat[j] = (rkRot(j,i)+rkRot(i,j))*fRoot;
        *apfQuat[k] = (rkRot(k,i)+rkRot(i,k))*fRoot;
    }

    return *this;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::FromRotationMatrix (
    const Matrix4& rkRot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    Real fTrace = rkRot(0,0) + rkRot(1,1) + rkRot(2,2);
    Real fRoot;

    if (fTrace > (Real)0.0)
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = Math::Sqrt(fTrace + (Real)1.0);  // 2w
        m_afTuple[0] = ((Real)0.5)*fRoot;
        fRoot = ((Real)0.5)/fRoot;  // 1/(4w)
        m_afTuple[1] = (rkRot(2,1)-rkRot(1,2))*fRoot;
        m_afTuple[2] = (rkRot(0,2)-rkRot(2,0))*fRoot;
        m_afTuple[3] = (rkRot(1,0)-rkRot(0,1))*fRoot;
    }
    else
    {
        // |w| <= 1/2
        int i = 0;
        if ( rkRot(1,1) > rkRot(0,0) )
        {
            i = 1;
        }
        if ( rkRot(2,2) > rkRot(i,i) )
        {
            i = 2;
        }
        int j = ms_iNext[i];
        int k = ms_iNext[j];

        fRoot = Math::Sqrt(rkRot(i,i)-rkRot(j,j)-rkRot(k,k)+(Real)1.0);
        Real* apfQuat[3] = { &m_afTuple[1], &m_afTuple[2], &m_afTuple[3] };
        *apfQuat[i] = ((Real)0.5)*fRoot;
        fRoot = ((Real)0.5)/fRoot;
        m_afTuple[0] = (rkRot(k,j)-rkRot(j,k))*fRoot;
        *apfQuat[j] = (rkRot(j,i)+rkRot(i,j))*fRoot;
        *apfQuat[k] = (rkRot(k,i)+rkRot(i,k))*fRoot;
    }

    return *this;
}
//----------------------------------------------------------------------------
inline void Quaternion::ToRotationMatrix (Matrix3& rkRot) const
{
    Real fTx  = ((Real)2.0)*m_afTuple[1];
    Real fTy  = ((Real)2.0)*m_afTuple[2];
    Real fTz  = ((Real)2.0)*m_afTuple[3];
    Real fTwx = fTx*m_afTuple[0];
    Real fTwy = fTy*m_afTuple[0];
    Real fTwz = fTz*m_afTuple[0];
    Real fTxx = fTx*m_afTuple[1];
    Real fTxy = fTy*m_afTuple[1];
    Real fTxz = fTz*m_afTuple[1];
    Real fTyy = fTy*m_afTuple[2];
    Real fTyz = fTz*m_afTuple[2];
    Real fTzz = fTz*m_afTuple[3];

    rkRot(0,0) = (Real)1.0-(fTyy+fTzz);
    rkRot(0,1) = fTxy-fTwz;
    rkRot(0,2) = fTxz+fTwy;
    rkRot(1,0) = fTxy+fTwz;
    rkRot(1,1) = (Real)1.0-(fTxx+fTzz);
    rkRot(1,2) = fTyz-fTwx;
    rkRot(2,0) = fTxz-fTwy;
    rkRot(2,1) = fTyz+fTwx;
    rkRot(2,2) = (Real)1.0-(fTxx+fTyy);
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::FromRotationMatrix (
    const Vector3 akRotColumn[3])
{
    Matrix3 kRot;
    for (int iCol = 0; iCol < 3; iCol++)
    {
        kRot(0,iCol) = akRotColumn[iCol][0];
        kRot(1,iCol) = akRotColumn[iCol][1];
        kRot(2,iCol) = akRotColumn[iCol][2];
    }
    return FromRotationMatrix(kRot);
}
//----------------------------------------------------------------------------
inline void Quaternion::ToRotationMatrix (Vector3 akRotColumn[3]) const
{
    Matrix3 kRot;
    ToRotationMatrix(kRot);
    for (int iCol = 0; iCol < 3; iCol++)
    {
        akRotColumn[iCol][0] = kRot(0,iCol);
        akRotColumn[iCol][1] = kRot(1,iCol);
        akRotColumn[iCol][2] = kRot(2,iCol);
    }
}
//-----------------------------------------------------------------------
inline void Quaternion::FromAxes (const Vector3* akAxis)
{
    Matrix3 kRot;

    for (size_t iCol = 0; iCol < 3; iCol++)
    {
        kRot[0][iCol] = akAxis[iCol].x;
        kRot[1][iCol] = akAxis[iCol].y;
        kRot[2][iCol] = akAxis[iCol].z;
    }

    FromRotationMatrix(kRot);
}
//-----------------------------------------------------------------------
inline void Quaternion::FromAxes (const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
{
    Matrix3 kRot;

    kRot[0][0] = xaxis.x;
    kRot[1][0] = xaxis.y;
    kRot[2][0] = xaxis.z;

    kRot[0][1] = yaxis.x;
    kRot[1][1] = yaxis.y;
    kRot[2][1] = yaxis.z;

    kRot[0][2] = zaxis.x;
    kRot[1][2] = zaxis.y;
    kRot[2][2] = zaxis.z;

    FromRotationMatrix(kRot);

}
//-----------------------------------------------------------------------
inline void Quaternion::ToAxes (Vector3* akAxis) const
{
	Real fTx  = ((Real)2.0)*x;
    Real fTy  = ((Real)2.0)*y;
    Real fTz  = ((Real)2.0)*z;
    Real fTwx = fTx*w;
    Real fTwy = fTy*w;
    Real fTwz = fTz*w;
    Real fTxx = fTx*x;
    Real fTxy = fTy*x;
    Real fTxz = fTz*x;
    Real fTyy = fTy*y;
    Real fTyz = fTz*y;
    Real fTzz = fTz*z;

    akAxis[0].x = (Real)1.0-(fTyy+fTzz);
    akAxis[1].x = fTxy-fTwz;
    akAxis[2].x = fTxz+fTwy;
    akAxis[0].y = fTxy+fTwz;
    akAxis[1].y = (Real)1.0-(fTxx+fTzz);
    akAxis[2].y = fTyz-fTwx;
    akAxis[0].z = fTxz-fTwy;
    akAxis[1].z = fTyz+fTwx;
    akAxis[2].z = (Real)1.0-(fTxx+fTyy);
}
//-----------------------------------------------------------------------
inline Vector3 Quaternion::xAxis(void) const
{
    //Real fTx  = 2.0*x;
    Real fTy  = ((Real)2.0)*y;
    Real fTz  = ((Real)2.0)*z;
    Real fTwy = fTy*w;
    Real fTwz = fTz*w;
    Real fTxy = fTy*x;
    Real fTxz = fTz*x;
    Real fTyy = fTy*y;
    Real fTzz = fTz*z;

    return Vector3((Real)1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
}
//-----------------------------------------------------------------------
inline Vector3 Quaternion::yAxis(void) const
{
    Real fTx  = ((Real)2.0)*x;
    Real fTy  = ((Real)2.0)*y;
    Real fTz  = ((Real)2.0)*z;
    Real fTwx = fTx*w;
    Real fTwz = fTz*w;
    Real fTxx = fTx*x;
    Real fTxy = fTy*x;
    Real fTyz = fTz*y;
    Real fTzz = fTz*z;

    return Vector3(fTxy-fTwz, (Real)1.0-(fTxx+fTzz), fTyz+fTwx);
}
//-----------------------------------------------------------------------
inline Vector3 Quaternion::zAxis(void) const
{
    Real fTx  = ((Real)2.0)*x;
    Real fTy  = ((Real)2.0)*y;
    Real fTz  = ((Real)2.0)*z;
    Real fTwx = fTx*w;
    Real fTwy = fTy*w;
    Real fTxx = fTx*x;
    Real fTxz = fTz*x;
    Real fTyy = fTy*y;
    Real fTyz = fTz*y;

    return Vector3(fTxz+fTwy, fTyz-fTwx, (Real)1.0-(fTxx+fTyy));
}
//-----------------------------------------------------------------------
inline void Quaternion::ToAxes (Vector3& xaxis, Vector3& yaxis, Vector3& zaxis) const
{
	Real fTx  = ((Real)2.0)*x;
    Real fTy  = ((Real)2.0)*y;
    Real fTz  = ((Real)2.0)*z;
    Real fTwx = fTx*w;
    Real fTwy = fTy*w;
    Real fTwz = fTz*w;
    Real fTxx = fTx*x;
    Real fTxy = fTy*x;
    Real fTxz = fTz*x;
    Real fTyy = fTy*y;
    Real fTyz = fTz*y;
    Real fTzz = fTz*z;

    xaxis.x = (Real)1.0-(fTyy+fTzz);
    yaxis.x = fTxy-fTwz;
    zaxis.x = fTxz+fTwy;
    xaxis.y = fTxy+fTwz;
    yaxis.y = (Real)1.0-(fTxx+fTzz);
    zaxis.y = fTyz-fTwx;
    xaxis.z = fTxz-fTwy;
    yaxis.z = fTyz+fTwx;
    zaxis.z = (Real)1.0-(fTxx+fTyy);
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::FromAxisAngle (
    const Vector3& rkAxis, Real fAngle)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Real fHalfAngle = ((Real)0.5)*fAngle;
    Real fSin = Math::Sin(fHalfAngle);
    m_afTuple[0] = Math::Cos(fHalfAngle);
    m_afTuple[1] = fSin*rkAxis[0];
    m_afTuple[2] = fSin*rkAxis[1];
    m_afTuple[3] = fSin*rkAxis[2];

    return *this;
}
//----------------------------------------------------------------------------
inline void Quaternion::ToAxisAngle (Vector3& rkAxis, Real& rfAngle)
    const
{
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Real fSqrLength = m_afTuple[1]*m_afTuple[1] + m_afTuple[2]*m_afTuple[2]
        + m_afTuple[3]*m_afTuple[3];

    if (fSqrLength > Math::ZERO_TOLERANCE)
    {
        rfAngle = ((Real)2.0)*Math::ACos(m_afTuple[0]);
        Real fInvLength = Math::InvSqrt(fSqrLength);
        rkAxis[0] = m_afTuple[1]*fInvLength;
        rkAxis[1] = m_afTuple[2]*fInvLength;
        rkAxis[2] = m_afTuple[3]*fInvLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        rfAngle = (Real)0.0;
        rkAxis[0] = (Real)1.0;
        rkAxis[1] = (Real)0.0;
        rkAxis[2] = (Real)0.0;
    }
}
//----------------------------------------------------------------------------
inline Real Quaternion::Length () const
{
    return Math::Sqrt(
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] +
        m_afTuple[3]*m_afTuple[3]);
}
//----------------------------------------------------------------------------
inline Real Quaternion::SquaredLength () const
{
    return
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] +
        m_afTuple[3]*m_afTuple[3];
}
//----------------------------------------------------------------------------
inline Real Quaternion::Dot (const Quaternion& rkQ) const
{
    Real fDot = (Real)0.0;
    for (int i = 0; i < 4; i++)
    {
        fDot += m_afTuple[i]*rkQ.m_afTuple[i];
    }
    return fDot;
}
//----------------------------------------------------------------------------
inline Real Quaternion::Normalize ()
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
inline Quaternion Quaternion::Inverse () const
{
    Quaternion kInverse;

    Real fNorm = (Real)0.0;
    int i;
    for (i = 0; i < 4; i++)
    {
        fNorm += m_afTuple[i]*m_afTuple[i];
    }

    if (fNorm > (Real)0.0)
    {
        Real fInvNorm = ((Real)1.0)/fNorm;
        kInverse.m_afTuple[0] = m_afTuple[0]*fInvNorm;
        kInverse.m_afTuple[1] = -m_afTuple[1]*fInvNorm;
        kInverse.m_afTuple[2] = -m_afTuple[2]*fInvNorm;
        kInverse.m_afTuple[3] = -m_afTuple[3]*fInvNorm;
    }
    else
    {
        // return an invalid result to flag the error
        for (i = 0; i < 4; i++)
        {
            kInverse.m_afTuple[i] = (Real)0.0;
        }
    }

    return kInverse;
}
//----------------------------------------------------------------------------
inline Quaternion Quaternion::Conjugate () const
{
    return Quaternion(m_afTuple[0],-m_afTuple[1],-m_afTuple[2],
        -m_afTuple[3]);
}
//----------------------------------------------------------------------------
inline Quaternion Quaternion::Exp () const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    Quaternion kResult;

    Real fAngle = Math::Sqrt(m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] + m_afTuple[3]*m_afTuple[3]);

    Real fSin = Math::Sin(fAngle);
    kResult.m_afTuple[0] = Math::Cos(fAngle);

    int i;

    if (Math::FAbs(fSin) >= Math::ZERO_TOLERANCE)
    {
        Real fCoeff = fSin/fAngle;
        for (i = 1; i <= 3; i++)
        {
            kResult.m_afTuple[i] = fCoeff*m_afTuple[i];
        }
    }
    else
    {
        for (i = 1; i <= 3; i++)
        {
            kResult.m_afTuple[i] = m_afTuple[i];
        }
    }

    return kResult;
}
//----------------------------------------------------------------------------
inline Quaternion Quaternion::Log () const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    Quaternion kResult;
    kResult.m_afTuple[0] = (Real)0.0;

    int i;

    if (Math::FAbs(m_afTuple[0]) < (Real)1.0)
    {
        Real fAngle = Math::ACos(m_afTuple[0]);
        Real fSin = Math::Sin(fAngle);
        if (Math::FAbs(fSin) >= Math::ZERO_TOLERANCE)
        {
            Real fCoeff = fAngle/fSin;
            for (i = 1; i <= 3; i++)
            {
                kResult.m_afTuple[i] = fCoeff*m_afTuple[i];
            }
            return kResult;
        }
    }

    for (i = 1; i <= 3; i++)
    {
        kResult.m_afTuple[i] = m_afTuple[i];
    }
    return kResult;
}
//----------------------------------------------------------------------------
inline Vector3 Quaternion::Rotate (const Vector3& rkVector)
    const
{
    // Given a vector u = (x0,y0,z0) and a unit length quaternion
    // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
    // rotation of u by q is v = q*u*q^{-1} where * indicates quaternion
    // multiplication and where u is treated as the quaternion <0,x0,y0,z0>.
    // Note that q^{-1} = <w,-x,-y,-z>, so no real work is required to
    // invert q.  Now
    //
    //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
    //     = q*(x0*i+y0*j+z0*k)*q^{-1}
    //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
    //
    // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
    // of the rotation matrix computed in Quaternion::ToRotationMatrix.
    // The vector v is obtained as the product of that rotation matrix with
    // vector u.  As such, the quaternion representation of a rotation
    // matrix requires less space than the matrix and more time to compute
    // the rotated vector.  Typical space-time tradeoff...

    Matrix3 kRot;
    ToRotationMatrix(kRot);
    return kRot*rkVector;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::Slerp (Real fT, const Quaternion& rkP,
    const Quaternion& rkQ, bool shortestPath)
{
    Real fCos = rkP.Dot(rkQ);

	// Problem 1:  given any two quaternions, there exist two possible arcs, along which one can move. 
	// One of them goes around the long way and this is the one that we wish to avoid. 
	if( fCos < 0.0 && shortestPath ) { fCos=-fCos;	(*this)=-rkQ; } else (*this) = rkQ;

	Real fAngle = Math::ACos(fCos);

	//Problem 2:  we explore the special cases where the both quaternions are very close together, 
	//in which case we approximate using the more economical LERP and avoid "divisions by zero" since sin(Angle) = 0  as   Angle=0
    if (Math::FAbs(fAngle) >= Math::ZERO_TOLERANCE)
    {
        Real fSin = Math::Sin(fAngle);
        Real fInvSin = ((Real)1.0)/fSin;
        Real fCoeff0 = Math::Sin(((Real)1.0-fT)*fAngle);
        Real fCoeff1 = Math::Sin(fT*fAngle);
        *this = (fCoeff0*rkP + fCoeff1*(*this))*fInvSin;
    }
    else
    {
        //*this = rkP;
		this->NLerp(fT, rkP, *this);
    }

    return *this;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::SlerpExtraSpins (Real fT,
    const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
{
    Real fCos = rkP.Dot(rkQ);
    Real fAngle = Math::ACos(fCos);

    if (Math::FAbs(fAngle) >= Math::ZERO_TOLERANCE)
    {
        Real fSin = Math::Sin(fAngle);
        Real fPhase = Math::PI*iExtraSpins*fT;
        Real fInvSin = ((Real)1.0)/fSin;
        Real fCoeff0 = Math::Sin(((Real)1.0-fT)*fAngle-fPhase)*fInvSin;
        Real fCoeff1 = Math::Sin(fT*fAngle + fPhase)*fInvSin;
        *this = fCoeff0*rkP + fCoeff1*rkQ;
    }
    else
    {
        *this = rkP;
    }

    return *this;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::Intermediate (const Quaternion& rkQ0,
    const Quaternion& rkQ1, const Quaternion& rkQ2)
{
    // assert:  Q0, Q1, Q2 all unit-length
    Quaternion kQ1Inv = rkQ1.Conjugate();
    Quaternion kP0 = kQ1Inv*rkQ0;
    Quaternion kP2 = kQ1Inv*rkQ2;
    Quaternion kArg = -((Real)0.25)*(kP0.Log()+kP2.Log());
    Quaternion kA = rkQ1*kArg.Exp();
    *this = kA;

    return *this;
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::Squad (Real fT, const Quaternion& rkQ0,
    const Quaternion& rkA0, const Quaternion& rkA1, const Quaternion& rkQ1, bool shortestPath)
{
    Real fSlerpT = ((Real)2.0)*fT*((Real)1.0-fT);
    Quaternion kSlerpP = Slerp(fT,rkQ0,rkQ1,shortestPath);
    Quaternion kSlerpQ = Slerp(fT,rkA0,rkA1);
    return Slerp(fSlerpT,kSlerpP,kSlerpQ);
}
//-----------------------------------------------------------------------
inline Quaternion& Quaternion::NLerp(float fT, const Quaternion& rkP, 
    const Quaternion& rkQ, bool shortestPath)
{
	// original
	//*this = rkP + fT * (rkQ - rkP);

	// from ogre

	Quaternion result;
    Real fCos = rkP.Dot(rkQ);
	if (fCos < 0.0f && shortestPath)
	{
		*this = rkP + fT * ((-rkQ) - rkP);
	}
	else
	{
		*this = rkP + fT * (rkQ - rkP);
	}

    Normalize();

	// end

    return (*this);
}
//----------------------------------------------------------------------------
inline Quaternion& Quaternion::Align (const Vector3& rkV1,
    const Vector3& rkV2)
{
    // If V1 and V2 are not parallel, the axis of rotation is the unit-length
    // vector U = Cross(V1,V2)/Length(Cross(V1,V2)).  The angle of rotation,
    // A, is the angle between V1 and V2.  The quaternion for the rotation is
    // q = cos(A/2) + sin(A/2)*(ux*i+uy*j+uz*k) where U = (ux,uy,uz).
    //
    // (1) Rather than extract A = acos(Dot(V1,V2)), multiply by 1/2, then
    //     compute sin(A/2) and cos(A/2), we reduce the computational costs by
    //     computing the bisector B = (V1+V2)/Length(V1+V2), so cos(A/2) =
    //     Dot(V1,B).
    //
    // (2) The rotation axis is U = Cross(V1,B)/Length(Cross(V1,B)), but
    //     Length(Cross(V1,B)) = Length(V1)*Length(B)*sin(A/2) = sin(A/2), in
    //     which case sin(A/2)*(ux*i+uy*j+uz*k) = (cx*i+cy*j+cz*k) where
    //     C = Cross(V1,B).
    //
    // If V1 = V2, then B = V1, cos(A/2) = 1, and U = (0,0,0).  If V1 = -V2,
    // then B = 0.  This can happen even if V1 is approximately -V2 using
    // floating point arithmetic, since Vector3::Normalize checks for
    // closeness to zero and returns the zero vector accordingly.  The test
    // for exactly zero is usually not recommend for floating point
    // arithmetic, but the implementation of Vector3::Normalize guarantees
    // the comparison is robust.  In this case, the A = pi and any axis
    // perpendicular to V1 may be used as the rotation axis.

    Vector3 kBisector = rkV1 + rkV2;
    kBisector.Normalize();

    Real fCosHalfAngle = rkV1.Dot(kBisector);
    Vector3 kCross;

    m_afTuple[0] = fCosHalfAngle;

    if (fCosHalfAngle != (Real)0.0)
    {
        kCross = rkV1.Cross(kBisector);
        m_afTuple[1] = kCross[0];
        m_afTuple[2] = kCross[1];
        m_afTuple[3] = kCross[2];
    }
    else
    {
        Real fInvLength;
        if (Math::FAbs(rkV1[0]) >= Math::FAbs(rkV1[1]))
        {
            // V1.x or V1.z is the largest magnitude component
            fInvLength = Math::InvSqrt(rkV1[0]*rkV1[0] +
                rkV1[2]*rkV1[2]);

            m_afTuple[1] = -rkV1[2]*fInvLength;
            m_afTuple[2] = (Real)0.0;
            m_afTuple[3] = +rkV1[0]*fInvLength;
        }
        else
        {
            // V1.y or V1.z is the largest magnitude component
            fInvLength = Math::InvSqrt(rkV1[1]*rkV1[1] +
                rkV1[2]*rkV1[2]);

            m_afTuple[1] = (Real)0.0;
            m_afTuple[2] = +rkV1[2]*fInvLength;
            m_afTuple[3] = -rkV1[1]*fInvLength;
        }
    }

    return *this;
}
//----------------------------------------------------------------------------
inline void Quaternion::DecomposeTwistTimesSwing (
    const Vector3& rkV1, Quaternion& rkTwist, Quaternion& rkSwing)
{
    Vector3 kV2 = Rotate(rkV1);
    rkSwing = Align(rkV1,kV2);
    rkTwist = (*this)*rkSwing.Conjugate();
}
//----------------------------------------------------------------------------
inline void Quaternion::DecomposeSwingTimesTwist (
    const Vector3& rkV1, Quaternion& rkSwing, Quaternion& rkTwist)
{
    Vector3 kV2 = Rotate(rkV1);
    rkSwing = Align(rkV1,kV2);
    rkTwist = rkSwing.Conjugate()*(*this);
}
//----------------------------------------------------------------------------
inline bool Quaternion::Equals(const Quaternion& rhs, const Real& tolerance) const
{
    Real fCos = Dot(rhs);
    Real angle = Math::ACos(fCos);

	return (Math::FAbs(angle) <= tolerance)
        || Math::RealEqual(angle, Math::PI, tolerance);

	
}
//-----------------------------------------------------------------------
inline Vector3 Quaternion::operator* (const Vector3& v) const
{
	// nVidia SDK implementation
	Vector3 uv, uuv;
	Vector3 qvec(x, y, z);
	uv = qvec.Cross(v);
	uuv = qvec.Cross(uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;

}