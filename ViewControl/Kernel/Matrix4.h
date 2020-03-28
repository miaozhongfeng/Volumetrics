#ifndef MATRIX4_H
#define MATRIX4_H

// Matrix operations are applied on the left.  For example, given a matrix M
// and a vector V, matrix-times-vector is M*V.  That is, V is treated as a
// column vector.  Some graphics APIs use V*M where V is treated as a row
// vector.  In this context the "M" matrix is really a transpose of the M as
// represented in Wild Magic.  Similarly, to apply two matrix operations M0
// and M1, in that order, you compute M1*M0 so that the transform of a vector
// is (M1*M0)*V = M1*(M0*V).  Some graphics APIs use M0*M1, but again these
// matrices are the transpose of those as represented in Wild Magic.  You
// must therefore be careful about how you interface the transformation code
// with graphics APIS.
//
// For memory organization it might seem natural to chose Real[N][N] for the
// matrix storage, but this can be a problem on a platform/console that
// chooses to store the data in column-major rather than row-major format.
// To avoid potential portability problems, the matrix is stored as Real[N*N]
// and organized in row-major order.  That is, the entry of the matrix in row
// r (0 <= r < N) and column c (0 <= c < N) is stored at index i = c+N*r
// (0 <= i < N*N).

#include "Vector4.h"
#include "Matrix3.h"

class Matrix4
{
public:
	Matrix4 ();

    // If bZero is true, create the zero matrix.  Otherwise, create the
    // identity matrix.
    Matrix4 (bool bZero);

    // copy constructor
    Matrix4 (const Matrix4& rkM);

    // input Mrc is in row r, column c.
    Matrix4 (Real fM00, Real fM01, Real fM02, Real fM03,
             Real fM10, Real fM11, Real fM12, Real fM13,
             Real fM20, Real fM21, Real fM22, Real fM23,
             Real fM30, Real fM31, Real fM32, Real fM33);

    // Create a matrix from an array of numbers.  The input array is
    // interpreted based on the Boolean input as
    //   true:  entry[0..15]={m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,
    //                        m23,m30,m31,m32,m33} [row major]
    //   false: entry[0..15]={m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,
    //                        m32,m03,m13,m23,m33} [col major]
    Matrix4 (const Real afEntry[16], bool bRowMajor);

    void MakeZero ();
    void MakeIdentity ();

    // member access
    inline operator const Real* () const;
    inline operator Real* ();
    inline const Real* operator[] (int iRow) const;
    inline Real* operator[] (int iRow);
    inline Real operator() (int iRow, int iCol) const;
    inline Real& operator() (int iRow, int iCol);
    void SetRow (int iRow, const Vector4& rkV);
    Vector4 GetRow (int iRow) const;
    void SetColumn (int iCol, const Vector4& rkV);
    Vector4 GetColumn (int iCol) const;
    void GetColumnMajor (Real* afCMajor) const;

    // assignment
    inline Matrix4& operator= (const Matrix4& rkM);

    /** Assignment from 3x3 matrix.
    */
    inline void operator = ( const Matrix3& mat3 )
    {
        m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2];
        m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2];
        m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2];
    }

    // comparison
    bool operator== (const Matrix4& rkM) const;
    bool operator!= (const Matrix4& rkM) const;
    bool operator<  (const Matrix4& rkM) const;
    bool operator<= (const Matrix4& rkM) const;
    bool operator>  (const Matrix4& rkM) const;
    bool operator>= (const Matrix4& rkM) const;

    // arithmetic operations
    inline Matrix4 operator+ (const Matrix4& rkM) const;
    inline Matrix4 operator- (const Matrix4& rkM) const;
    inline Matrix4 operator* (const Matrix4& rkM) const;
    inline Matrix4 operator* (Real fScalar) const;
    inline Matrix4 operator/ (Real fScalar) const;
    inline Matrix4 operator- () const;

    // arithmetic updates
    inline Matrix4& operator+= (const Matrix4& rkM);
    inline Matrix4& operator-= (const Matrix4& rkM);
	inline Matrix4& operator*= (const Matrix4& rkM);
    inline Matrix4& operator*= (Real fScalar);
    inline Matrix4& operator/= (Real fScalar);

    // matrix times vector
    inline Vector4 operator* (const Vector4& rkV) const;  // M * v

    /** Vector transformation using '*'.
        @remarks
            Transforms the given 3-D vector by the matrix, projecting the 
            result back into <i>w</i> = 1.
        @note
            This means that the initial <i>w</i> is considered to be 1.0,
            and then all the tree elements of the resulting 3-D vector are
            divided by the resulting <i>w</i>.
    */
    inline Vector3 operator * ( const Vector3 &v ) const
    {
        Vector3 r;

        Real fInvW = Real(1.0) / ( m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] );

        r.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] ) * fInvW;
        r.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] ) * fInvW;
        r.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] ) * fInvW;

        return r;
    }

    // other operations
    Matrix4 Transpose () const;  // M^T
    Matrix4 TransposeTimes (const Matrix4& rkM) const;  // this^T * M
    Matrix4 TimesTranspose (const Matrix4& rkM) const;  // this * M^T
    Matrix4 Inverse () const;
    Matrix4 Adjoint () const;
    Real Determinant () const;
    Real QForm (const Vector4& rkU,
        const Vector4& rkV) const;  // u^T*M*v

    // projection matrices onto a specified plane
    void MakeObliqueProjection (const Vector3& rkNormal,
        const Vector3& rkPoint, const Vector3& rkDirection);
    void MakePerspectiveProjection (const Vector3& rkNormal,
        const Vector3& rkPoint, const Vector3& rkEye);

    // reflection matrix through a specified plane
    void MakeReflection (const Vector3& rkNormal,
        const Vector3& rkPoint);

    // special matrices
    static const Matrix4 ZERO;
    static const Matrix4 IDENTITY;
	        /** Useful little matrix which takes 2D clipspace {-1, 1} to {0,1}
            and inverts the Y. */
    static const Matrix4 CLIPSPACE2DTOIMAGESPACE;

    inline Matrix4 Concatenate(const Matrix4 &m2) const
    {
        Matrix4 r;
        r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
        r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
        r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
        r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

        r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
        r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
        r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
        r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

        r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
        r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
        r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
        r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

        r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
        r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
        r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
        r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

        return r;
    }

    /** Building a Matrix4 from orientation / scale / position.
    @remarks
        Transform is performed in the order scale, rotate, translation, i.e. translation is independent
        of orientation axes, scale does not affect size of translation, rotation and scaling are always
        centered on the origin.
    */
    void makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);

    /** Building an inverse Matrix4 from orientation / scale / position.
    @remarks
        As makeTransform except it build the inverse given the same data as makeTransform, so
        performing -translation, -rotate, 1/scale in that order.
    */
    void makeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);

	/** Sets the translation transformation part of the matrix.
    */
    inline void SetTranslation( const Vector3& v )
    {
        m[0][3] = v.x;
        m[1][3] = v.y;
        m[2][3] = v.z;
    }

	/** Sets the translation transformation part of the matrix.
    */
    inline void SetTranslation( Real x, Real y, Real z )
    {
        m[0][3] = x;
        m[1][3] = y;
        m[2][3] = z;
    }

    /** Extracts the translation transformation part of the matrix.
     */
    inline Vector3 GetTranslation() const
    {
      return Vector3(m[0][3], m[1][3], m[2][3]);
    }

    /** Sets the scale part of the matrix.
    */
    inline void SetScale( const Vector3& v )
    {
        m[0][0] = v.x;
        m[1][1] = v.y;
        m[2][2] = v.z;
    }

	inline void BuildReflectionMatrix ( const Vector3& p, const Vector3& n )
	{
		Real vxy   = Real(-2.0) * n.x * n.y;
		Real vxz   = Real(-2.0) * n.x * n.z;
		Real vyz   = Real(-2.0) * n.y * n.z;
		Real pdotn = Real(2.0) * p.Dot(n);

		m[0][0]=Real(1.0-2.0*n.x*n.x);	m[0][1]=vxy;    				m[0][2]=vxz;    				m[0][3]=pdotn*n.x;
		m[1][0]=vxy;  					m[1][1]=Real(1.0-2.0*n.y*n.y); 	m[1][2]=vyz;    				m[1][3]=pdotn*n.y;
		m[2][0]=vxz;  					m[2][1]=vyz;   					m[2][2]=Real(1.0-2.0*n.z*n.z); 	m[2][3]=pdotn*n.z;
		m[3][0]=0;						m[3][1]=0;						m[3][2]=0;						m[3][3]=1;
	}
       
    /** Extracts the rotation / scaling part of the Matrix as a 3x3 matrix. 
    @param m3x3 Destination Matrix3
    */
    inline void Extract3x3Matrix(Matrix3& m3x3) const
    {
        m3x3.m[0][0] = m[0][0];
        m3x3.m[0][1] = m[0][1];
        m3x3.m[0][2] = m[0][2];
        m3x3.m[1][0] = m[1][0];
        m3x3.m[1][1] = m[1][1];
        m3x3.m[1][2] = m[1][2];
        m3x3.m[2][0] = m[2][0];
        m3x3.m[2][1] = m[2][1];
        m3x3.m[2][2] = m[2][2];

    }

    /** Check whether or not the matrix is affine matrix.
        @remarks
            An affine matrix is a 4x4 matrix with row 3 equal to (0, 0, 0, 1),
            e.g. no projective coefficients.
    */
    inline bool IsAffine(void) const
    {
        return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
    }

    /** Returns the inverse of the affine matrix.
        @note
            The matrix must be an affine matrix. @see Matrix4::isAffine.
    */
    Matrix4 InverseAffine(void) const;

    /** Concatenate two affine matrix.
        @note
            The matrices must be affine matrix. @see Matrix4::isAffine.
    */
    inline Matrix4 ConcatenateAffine(const Matrix4 &m2) const
    {
        assert(IsAffine() && m2.IsAffine());

        return Matrix4(
            m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0],
            m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1],
            m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2],
            m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3],

            m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0],
            m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1],
            m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2],
            m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3],

            m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0],
            m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1],
            m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2],
            m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3],

            0, 0, 0, 1);
    }

    /** 3-D Vector transformation specially for affine matrix.
        @remarks
            Transforms the given 3-D vector by the matrix, projecting the 
            result back into <i>w</i> = 1.
        @note
            The matrix must be an affine matrix. @see Matrix4::isAffine.
    */
    inline Vector3 TransformAffine(const Vector3& v) const
    {
        assert(IsAffine());

        return Vector3(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3], 
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
    }

    /** 4-D Vector transformation specially for affine matrix.
        @note
            The matrix must be an affine matrix. @see Matrix4::isAffine.
    */
    inline Vector4 TransformAffine(const Vector4& v) const
    {
        assert(IsAffine());

        return Vector4(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            v.w);
    }


private:
    // support for comparisons
    int CompareArrays (const Matrix4& rkM) const;

	union
	{
		Real m[4][4];
		Real m_afEntry[16];
	};
};

// c * M

inline Matrix4 operator* (Real fScalar, const Matrix4& rkM);

// v^T * M

inline Vector4 operator* (const Vector4& rkV,
    const Matrix4& rkM);
inline Vector3 operator* (const Vector3& v,
    const Matrix4& m)
{
    Vector3 r;

    Real fInvW = Real(1.0) / ( m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] );

    r.x = ( m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[0][3] ) * fInvW;
    r.y = ( m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[1][3] ) * fInvW;
    r.z = ( m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[2][3] ) * fInvW;

	return r;
}

#include "Matrix4.inl"

#endif
