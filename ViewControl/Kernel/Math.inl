//----------------------------------------------------------------------------
inline Real Math::ACos (Real fValue)
{
    if (-(Real)1.0 < fValue)
    {
        if (fValue < (Real)1.0)
        {
            return (Real)acos(fValue);
        }
        else
        {
            return (Real)0.0;
        }
    }
    else
    {
        return PI;
    }
}
//----------------------------------------------------------------------------
inline Real Math::ASin (Real fValue)
{
    if (-(Real)1.0 < fValue)
    {
        if (fValue < (Real)1.0)
        {
            return (Real)asin(fValue);
        }
        else
        {
            return HALF_PI;
        }
    }
    else
    {
        return -HALF_PI;
    }
}
//----------------------------------------------------------------------------
inline Real Math::ATan (Real fValue)
{
    return (Real)atan(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::ATan2 (Real fY, Real fX)
{
    return (Real)atan2(fY,fX);
}
//----------------------------------------------------------------------------
inline Real Math::Ceil (Real fValue)
{
    return (Real)ceil(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::Cos (Real fValue)
{
    return (Real)cos(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::Exp (Real fValue)
{
    return (Real)exp(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::FAbs (Real fValue)
{
    return (Real)fabs(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::Floor (Real fValue)
{
    return (Real)floor(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::FMod (Real fX, Real fY)
{
    return (Real)fmod(fX,fY);
}
//----------------------------------------------------------------------------
inline Real Math::InvSqrt (Real fValue)
{
    return (Real)(1.0/sqrt(fValue));
}
//----------------------------------------------------------------------------
inline Real Math::Log (Real fValue)
{
    return (Real)log(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::Log2 (Real fValue)
{
    return Math::INV_LN_2 * (Real)log(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::Log10 (Real fValue)
{
    return Math::INV_LN_10 * (Real)log(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::Pow (Real fBase, Real fExponent)
{
    return (Real)pow(fBase,fExponent);
}
//----------------------------------------------------------------------------
inline Real Math::Sin (Real fValue)
{
    return (Real)sin(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::Sqr (Real fValue)
{
    return fValue*fValue;
}
//----------------------------------------------------------------------------
inline Real Math::Sqrt (Real fValue)
{
    return (Real)sqrt(fValue);
}
//----------------------------------------------------------------------------
inline Real Math::Tan (Real fValue)
{
    return (Real)tan(fValue);
}
//----------------------------------------------------------------------------
inline int Math::Sign (int iValue)
{
    if (iValue > 0)
    {
        return 1;
    }

    if (iValue < 0)
    {
        return -1;
    }

    return 0;
}
//----------------------------------------------------------------------------
inline Real Math::Sign (Real fValue)
{
    if (fValue > (Real)0.0)
    {
        return (Real)1.0;
    }

    if (fValue < (Real)0.0)
    {
        return -(Real)1.0;
    }

    return (Real)0.0;
}
//----------------------------------------------------------------------------
inline Real Math::UnitRandom (unsigned int uiSeed )
{
    if (uiSeed > 0)
    {
        srand(uiSeed);
    }

    Real dRatio = ((Real)rand())/((Real)(RAND_MAX));
    return (Real)dRatio;
}
//----------------------------------------------------------------------------
inline Real Math::SymmetricRandom (unsigned int uiSeed)
{
    if (uiSeed > 0.0)
    {
        srand(uiSeed);
    }

    Real dRatio = ((Real)rand())/((Real)(RAND_MAX));
    return (Real)(2.0*dRatio - 1.0);
}
//----------------------------------------------------------------------------
inline Real Math::IntervalRandom (Real fMin, Real fMax, unsigned int uiSeed)
{
    if (uiSeed > 0)
    {
        srand(uiSeed);
    }

    Real dRatio = ((Real)rand())/((Real)(RAND_MAX));
    return fMin+(fMax-fMin)*((Real)dRatio);
}
//----------------------------------------------------------------------------
inline int Math::Random (int fMax, int fSeed)
{
    if ( fSeed > 0 )
        srand(fSeed);

    return (fMax*rand())/RAND_MAX;
}
//----------------------------------------------------------------------------
inline Real Math::FastSin0 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = (Real)7.61e-03;
    fResult *= fASqr;
    fResult -= (Real)1.6605e-01;
    fResult *= fASqr;
    fResult += (Real)1.0;
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastSin1 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = -(Real)2.39e-08;
    fResult *= fASqr;
    fResult += (Real)2.7526e-06;
    fResult *= fASqr;
    fResult -= (Real)1.98409e-04;
    fResult *= fASqr;
    fResult += (Real)8.3333315e-03;
    fResult *= fASqr;
    fResult -= (Real)1.666666664e-01;
    fResult *= fASqr;
    fResult += (Real)1.0;
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastCos0 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = (Real)3.705e-02;
    fResult *= fASqr;
    fResult -= (Real)4.967e-01;
    fResult *= fASqr;
    fResult += (Real)1.0;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastCos1 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = -(Real)2.605e-07;
    fResult *= fASqr;
    fResult += (Real)2.47609e-05;
    fResult *= fASqr;
    fResult -= (Real)1.3888397e-03;
    fResult *= fASqr;
    fResult += (Real)4.16666418e-02;
    fResult *= fASqr;
    fResult -= (Real)4.999999963e-01;
    fResult *= fASqr;
    fResult += (Real)1.0;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastTan0 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = (Real)2.033e-01;
    fResult *= fASqr;
    fResult += (Real)3.1755e-01;
    fResult *= fASqr;
    fResult += (Real)1.0;
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastTan1 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = (Real)9.5168091e-03;
    fResult *= fASqr;
    fResult += (Real)2.900525e-03;
    fResult *= fASqr;
    fResult += (Real)2.45650893e-02;
    fResult *= fASqr;
    fResult += (Real)5.33740603e-02;
    fResult *= fASqr;
    fResult += (Real)1.333923995e-01;
    fResult *= fASqr;
    fResult += (Real)3.333314036e-01;
    fResult *= fASqr;
    fResult += (Real)1.0;
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastInvSin0 (Real fValue)
{
    Real fRoot = Math::Sqrt(((Real)1.0)-fValue);
    Real fResult = -(Real)0.0187293;
    fResult *= fValue;
    fResult += (Real)0.0742610;
    fResult *= fValue;
    fResult -= (Real)0.2121144;
    fResult *= fValue;
    fResult += (Real)1.5707288;
    fResult = HALF_PI - fRoot*fResult;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastInvSin1 (Real fValue)
{
    Real fRoot = Math::Sqrt(FAbs(((Real)1.0)-fValue));
    Real fResult = -(Real)0.0012624911;
    fResult *= fValue;
    fResult += (Real)0.0066700901;
    fResult *= fValue;
    fResult -= (Real)0.0170881256;
    fResult *= fValue;
    fResult += (Real)0.0308918810;
    fResult *= fValue;
    fResult -= (Real)0.0501743046;
    fResult *= fValue;
    fResult += (Real)0.0889789874;
    fResult *= fValue;
    fResult -= (Real)0.2145988016;
    fResult *= fValue;
    fResult += (Real)1.5707963050;
    fResult = HALF_PI - fRoot*fResult;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastInvCos0 (Real fValue)
{
    Real fRoot = Math::Sqrt(((Real)1.0)-fValue);
    Real fResult = -(Real)0.0187293;
    fResult *= fValue;
    fResult += (Real)0.0742610;
    fResult *= fValue;
    fResult -= (Real)0.2121144;
    fResult *= fValue;
    fResult += (Real)1.5707288;
    fResult *= fRoot;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastInvCos1 (Real fValue)
{
    Real fRoot = Math::Sqrt(FAbs(((Real)1.0)-fValue));
    Real fResult = -(Real)0.0012624911;
    fResult *= fValue;
    fResult += (Real)0.0066700901;
    fResult *= fValue;
    fResult -= (Real)0.0170881256;
    fResult *= fValue;
    fResult += (Real)0.0308918810;
    fResult *= fValue;
    fResult -= (Real)0.0501743046;
    fResult *= fValue;
    fResult += (Real)0.0889789874;
    fResult *= fValue;
    fResult -= (Real)0.2145988016;
    fResult *= fValue;
    fResult += (Real)1.5707963050;
    fResult *= fRoot;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastInvTan0 (Real fValue)
{
    Real fVSqr = fValue*fValue;
    Real fResult = (Real)0.0208351;
    fResult *= fVSqr;
    fResult -= (Real)0.085133;
    fResult *= fVSqr;
    fResult += (Real)0.180141;
    fResult *= fVSqr;
    fResult -= (Real)0.3302995;
    fResult *= fVSqr;
    fResult += (Real)0.999866;
    fResult *= fValue;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastInvTan1 (Real fValue)
{
    Real fVSqr = fValue*fValue;
    Real fResult = (Real)0.0028662257;
    fResult *= fVSqr;
    fResult -= (Real)0.0161657367;
    fResult *= fVSqr;
    fResult += (Real)0.0429096138;
    fResult *= fVSqr;
    fResult -= (Real)0.0752896400;
    fResult *= fVSqr;
    fResult += (Real)0.1065626393;
    fResult *= fVSqr;
    fResult -= (Real)0.1420889944;
    fResult *= fVSqr;
    fResult += (Real)0.1999355085;
    fResult *= fVSqr;
    fResult -= (Real)0.3333314528;
    fResult *= fVSqr;
    fResult += (Real)1.0;
    fResult *= fValue;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastNegExp0 (Real fValue)
{
    Real fResult = (Real)0.0038278;
    fResult *= fValue;
    fResult += (Real)0.0292732;
    fResult *= fValue;
    fResult += (Real)0.2507213;
    fResult *= fValue;
    fResult += (Real)1.0;
    fResult *= fResult;
    fResult *= fResult;
    fResult = ((Real)1.0)/fResult;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastNegExp1 (Real fValue)
{
    Real fResult = (Real)0.00026695;
    fResult *= fValue;
    fResult += (Real)0.00227723;
    fResult *= fValue;
    fResult += (Real)0.03158565;
    fResult *= fValue;
    fResult += (Real)0.24991035;
    fResult *= fValue;
    fResult += (Real)1.0;
    fResult *= fResult;
    fResult *= fResult;
    fResult = ((Real)1.0)/fResult;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastNegExp2 (Real fValue)
{
    Real fResult = (Real)0.000014876;
    fResult *= fValue;
    fResult += (Real)0.000127992;
    fResult *= fValue;
    fResult += (Real)0.002673255;
    fResult *= fValue;
    fResult += (Real)0.031198056;
    fResult *= fValue;
    fResult += (Real)0.250010936;
    fResult *= fValue;
    fResult += (Real)1.0;
    fResult *= fResult;
    fResult *= fResult;
    fResult = ((Real)1.0)/fResult;
    return fResult;
}
//----------------------------------------------------------------------------
inline Real Math::FastNegExp3 (Real fValue)
{
    Real fResult = (Real)0.0000006906;
    fResult *= fValue;
    fResult += (Real)0.0000054302;
    fResult *= fValue;
    fResult += (Real)0.0001715620;
    fResult *= fValue;
    fResult += (Real)0.0025913712;
    fResult *= fValue;
    fResult += (Real)0.0312575832;
    fResult *= fValue;
    fResult += (Real)0.2499986842;
    fResult *= fValue;
    fResult += (Real)1.0;
    fResult *= fResult;
    fResult *= fResult;
    fResult = ((Real)1.0)/fResult;
    return fResult;
}
//----------------------------------------------------------------------------
inline bool Math::RealEqual( Real a, Real b, Real tolerance )
{
    if (fabs(b-a) <= tolerance)
        return true;
    else
        return false;
}
//----------------------------------------------------------------------------