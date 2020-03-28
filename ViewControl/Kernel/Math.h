#ifndef MATH_H
#define MATH_H

#include "Kernel.h"

class Math
{
public:
    // Wrappers to hide implementations of functions.  The ACos and ASin
    // functions clamp the input argument to [-1,1] to avoid NaN issues
    // when the input is slightly larger than 1 or slightly smaller than -1.
    // Other functions have the potential for using a fast and approximate
    // algorithm rather than calling the standard math library functions.
    static Real ACos (Real fValue);
    static Real ASin (Real fValue);
    static Real ATan (Real fValue);
    static Real ATan2 (Real fY, Real fX);
    static Real Ceil (Real fValue);
    static Real Cos (Real fValue);
    static Real Exp (Real fValue);
    static Real FAbs (Real fValue);
    static Real Floor (Real fValue);
    static Real FMod (Real fX, Real fY);
    static Real InvSqrt (Real fValue);
    static Real Log (Real fValue);
    static Real Log2 (Real fValue);
    static Real Log10 (Real fValue);
    static Real Pow (Real fBase, Real fExponent);
    static Real Sin (Real fValue);
    static Real Sqr (Real fValue);
    static Real Sqrt (Real fValue);
    static Real Tan (Real fValue);

	static inline int IAbs (int iValue) { return ( iValue >= 0 ? iValue : -iValue ); }
	static inline int ICeil (float fValue) { return int(ceil(fValue)); }
	static inline int IFloor (float fValue) { return int(floor(fValue)); }
    static int ISign (int iValue) { return ( iValue > 0 ? +1 : ( iValue < 0 ? -1 : 0 ) ); }

	// Generate a random number in [0,max).  The random number generator may
    // be seeded by a first call to IntervalRandom with a positive seed.
	static int Random (int fMax, int fSeed);

    // Return -1 if the input is negative, 0 if the input is zero, and +1
    // if the input is positive.
    static int Sign (int iValue);
    static Real Sign (Real fValue);

    // Generate a random number in [0,1).  The random number generator may
    // be seeded by a first call to UnitRandom with a positive seed.
    static Real UnitRandom (unsigned int uiSeed = 0);

    // Generate a random number in [-1,1).  The random number generator may
    // be seeded by a first call to SymmetricRandom with a positive seed.
    static Real SymmetricRandom (unsigned int uiSeed = 0);

    // Generate a random number in [min,max).  The random number generator may
    // be seeded by a first call to IntervalRandom with a positive seed.
    static Real IntervalRandom (Real fMin, Real fMax,
        unsigned int uiSeed = 0);

    // Fast evaluation of trigonometric and inverse trigonometric functions
    // using polynomial approximations.  The speed ups were measured on an
    // AMD 2800 (2.08 GHz) processor using Visual Studion .NET 2003 with a
    // release build.

    // The input must be in [0,pi/2].
    // max error sin0 = 1.7e-04, speed up = 4.0
    // max error sin1 = 1.9e-08, speed up = 2.8
    static Real FastSin0 (Real fAngle);
    static Real FastSin1 (Real fAngle);

    // The input must be in [0,pi/2]
    // max error cos0 = 1.2e-03, speed up = 4.5
    // max error cos1 = 6.5e-09, speed up = 2.8
    static Real FastCos0 (Real fAngle);
    static Real FastCos1 (Real fAngle);

    // The input must be in [0,pi/4].
    // max error tan0 = 8.1e-04, speed up = 5.6
    // max error tan1 = 1.9e-08, speed up = 3.4
    static Real FastTan0 (Real fAngle);
    static Real FastTan1 (Real fAngle);

    // The input must be in [0,1].
    // max error invsin0 = 6.8e-05, speed up = 7.5
    // max error invsin1 = 1.4e-07, speed up = 5.5
    static Real FastInvSin0 (Real fValue);
    static Real FastInvSin1 (Real fValue);

    // The input must be in [0,1].
    // max error invcos0 = 6.8e-05, speed up = 7.5
    // max error invcos1 = 1.4e-07, speed up = 5.7
    static Real FastInvCos0 (Real fValue);
    static Real FastInvCos1 (Real fValue);

    // The input must be in [-1,1]. 
    // max error invtan0 = 1.2e-05, speed up = 2.8
    // max error invtan1 = 2.3e-08, speed up = 1.8
    static Real FastInvTan0 (Real fValue);
    static Real FastInvTan1 (Real fValue);

    // A fast approximation to 1/sqrt.
    static Real FastInvSqrt (Real fValue);

    // Fast approximations to exp(-x).  The input x must be in [0,infinity).
    // max error negexp0 = 0.00024, speed up = 25.4
    // max error negexp1 = 0.000024, speed up = 25.4
    // max error negexp2 = 0.0000024, speed up = 20.5
    // max error negexp3 = 0.00000025, speed up = 17.3
    static Real FastNegExp0 (Real fValue);
    static Real FastNegExp1 (Real fValue);
    static Real FastNegExp2 (Real fValue);
    static Real FastNegExp3 (Real fValue);

	// Compare 2 floats, using tolerance for inaccuracies.
	static bool RealEqual(Real a, Real b,
		Real tolerance = Math::EPSILON);


   /** Checks whether a given point is inside a triangle, in a
        2-dimensional (Cartesian) space.
        @remarks
            The vertices of the triangle must be given in either
            trigonometrical (anticlockwise) or inverse trigonometrical
            (clockwise) order.
        @param
            p The point.
        @param
            a The triangle's first vertex.
        @param
            b The triangle's second vertex.
        @param
            c The triangle's third vertex.
        @returns
            If the point resides in the triangle, <b>true</b> is
            returned.
        @par
            If the point is outside the triangle, <b>false</b> is
            returned.
    */
    static bool pointInTri2D(const Vector2& p, const Vector2& a, 
		const Vector2& b, const Vector2& c);

   /** Checks whether a given 3D point is inside a triangle.
   @remarks
        The vertices of the triangle must be given in either
        trigonometrical (anticlockwise) or inverse trigonometrical
        (clockwise) order, and the point must be guaranteed to be in the
		same plane as the triangle
    @param
        p The point.
    @param
        a The triangle's first vertex.
    @param
        b The triangle's second vertex.
    @param
        c The triangle's third vertex.
	@param 
		normal The triangle plane's normal (passed in rather than calculated
			on demand since the callermay already have it)
    @returns
        If the point resides in the triangle, <b>true</b> is
        returned.
    @par
        If the point is outside the triangle, <b>false</b> is
        returned.
    */
    static bool pointInTri3D(const Vector3& p, const Vector3& a, 
		const Vector3& b, const Vector3& c, const Vector3& normal);
    /** Ray / plane intersection, returns boolean result and distance. */
    static std::pair<bool, Real> intersects(const Ray& ray, const Plane& plane);

    /** Ray / sphere intersection, returns boolean result and distance. */
    static std::pair<bool, Real> intersects(const Ray& ray, const Sphere& sphere, 
        bool discardInside = true);
    
    /** Ray / box intersection, returns boolean result and distance. */
    static std::pair<bool, Real> intersects(const Ray& ray, const AxisAlignedBox& box);

    /** Ray / box intersection, returns boolean result and two intersection distance.
    @param
        ray The ray.
    @param
        box The box.
    @param
        d1 A real pointer to retrieve the near intersection distance
            from the ray origin, maybe <b>null</b> which means don't care
            about the near intersection distance.
    @param
        d2 A real pointer to retrieve the far intersection distance
            from the ray origin, maybe <b>null</b> which means don't care
            about the far intersection distance.
    @returns
        If the ray is intersects the box, <b>true</b> is returned, and
        the near intersection distance is return by <i>d1</i>, the
        far intersection distance is return by <i>d2</i>. Guarantee
        <b>0</b> <= <i>d1</i> <= <i>d2</i>.
    @par
        If the ray isn't intersects the box, <b>false</b> is returned, and
        <i>d1</i> and <i>d2</i> is unmodified.
    */
    static bool intersects(const Ray& ray, const AxisAlignedBox& box,
        Real* d1, Real* d2);

    /** Ray / triangle intersection, returns boolean result and distance.
    @param
        ray The ray.
    @param
        a The triangle's first vertex.
    @param
        b The triangle's second vertex.
    @param
        c The triangle's third vertex.
	@param 
		normal The triangle plane's normal (passed in rather than calculated
			on demand since the callermay already have it), doesn't need
            normalised since we don't care.
    @param
        positiveSide Intersect with "positive side" of the triangle
    @param
        negativeSide Intersect with "negative side" of the triangle
    @returns
        If the ray is intersects the triangle, a pair of <b>true</b> and the
        distance between intersection point and ray origin returned.
    @par
        If the ray isn't intersects the triangle, a pair of <b>false</b> and
        <b>0</b> returned.
    */
    static std::pair<bool, Real> intersects(const Ray& ray, const Vector3& a,
        const Vector3& b, const Vector3& c, const Vector3& normal,
        bool positiveSide = true, bool negativeSide = true);

    /** Ray / triangle intersection, returns boolean result and distance.
    @param
        ray The ray.
    @param
        a The triangle's first vertex.
    @param
        b The triangle's second vertex.
    @param
        c The triangle's third vertex.
    @param
        positiveSide Intersect with "positive side" of the triangle
    @param
        negativeSide Intersect with "negative side" of the triangle
    @returns
        If the ray is intersects the triangle, a pair of <b>true</b> and the
        distance between intersection point and ray origin returned.
    @par
        If the ray isn't intersects the triangle, a pair of <b>false</b> and
        <b>0</b> returned.
    */
    static std::pair<bool, Real> intersects(const Ray& ray, const Vector3& a,
        const Vector3& b, const Vector3& c,
        bool positiveSide = true, bool negativeSide = true);

    /** Sphere / box intersection test. */
    static bool intersects(const Sphere& sphere, const AxisAlignedBox& box);

    /** Plane / box intersection test. */
    static bool intersects(const Plane& plane, const AxisAlignedBox& box);

    /** Ray / convex plane list intersection test. 
    @param ray The ray to test with
    @param plaeList List of planes which form a convex volume
    @param normalIsOutside Does the normal point outside the volume
    */
    static std::pair<bool, Real> intersects(
        const Ray& ray, const std::vector<Plane>& planeList, 
        bool normalIsOutside);
    /** Ray / convex plane list intersection test. 
    @param ray The ray to test with
    @param plaeList List of planes which form a convex volume
    @param normalIsOutside Does the normal point outside the volume
    */
    static std::pair<bool, Real> intersects(
        const Ray& ray, const std::list<Plane>& planeList, 
        bool normalIsOutside);

    /** Sphere / plane intersection test. 
    @remarks NB just do a plane.getDistance(sphere.getCenter()) for more detail!
    */
    static bool intersects(const Sphere& sphere, const Plane& plane);

    /** Calculates the tangent space vector for a given set of positions / texture coords. */
    static Vector3 calculateTangentSpaceVector(
        const Vector3& position1, const Vector3& position2, const Vector3& position3,
        Real u1, Real v1, Real u2, Real v2, Real u3, Real v3);

    /** Build a reflection matrix for the passed in plane. */
    static Matrix4 buildReflectionMatrix(const Plane& p);
    /** Calculate a face normal, including the w component which is the offset from the origin. */
    static Vector4 calculateFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3);
    /** Calculate a face normal, no w-information. */
    static Vector3 calculateBasicFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3);
    /** Calculate a face normal without normalize, including the w component which is the offset from the origin. */
    static Vector4 calculateFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3);
    /** Calculate a face normal without normalize, no w-information. */
    static Vector3 calculateBasicFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3);

	/** Generates a value based on the gaussian (normal) distribution function
		with the given offset and scale parameters.
	*/
	static Real gaussianDistribution(Real x, Real offset = 0.0f, Real scale = 1.0f);

    // common constants
	static const Real POS_INFINITY;
	static const Real NEG_INFINITY;
    static const Real EPSILON;
    static const Real ZERO_TOLERANCE;
    static const Real MAX_REAL;
	static const float MAX_FLOAT;
	static const double MAX_DOUBLE;
    static const Real PI;
    static const Real TWO_PI;
    static const Real HALF_PI;
    static const Real INV_PI;
    static const Real INV_TWO_PI;
    static const Real DEG_TO_RAD;
    static const Real RAD_TO_DEG;
    static const Real LN_2;
    static const Real LN_10;
    static const Real INV_LN_2;
    static const Real INV_LN_10;
};

#include "Math.inl"

#endif
