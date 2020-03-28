
#include "stdafx.h"
#include "Kernel\Vector4.h"
#include "Kernel\Matrix4.h"
#include "Kernel\Kernel.h"
#include "Kernel\TriMeshes.h"
#include "Spline.h"

void NaturalCubicSpline::ComputeSUseExisting(VertVector& S_) 
{
	int Existing = S_.size() ;
	if (Existing == L+1 ) {
		return ;
	}
	if(Existing == 0){
		ComputeDerivativesC() ;
		S_.resize(L+1) ;
		S_ = S ;
		return ;
	}
	S.resize(L+1) ;
	S_.resize(L+1) ;
	S = S_ ; 
	for(int i = Existing ; i < L ; ++i){
		S[i] = (m_Controls[i+1]-m_Controls[i-1])*0.5 ;	
		S_[i] = S[i] ;
	}

	S[L] = ( (m_Controls[L] - m_Controls[L-1])*3.0 - S[L-1] ) * 0.5 ;
	S_[L] = S[L] ; // !
}
void NaturalCubicSpline::ComputeCoefficients()
{
	a.resize(L) ;
	b.resize(L) ;
	c.resize(L) ;
	d.resize(L) ;
	for(int k = 0 ; k < L ; ++k){ 
		a[k] = S[k+1] + S[k] - (m_Controls[k+1] - m_Controls[k])*2.0 ;
		b[k] = (m_Controls[k+1] - m_Controls[k])*3.0 - S[k]*2.0 - S[k+1] ;
		c[k] = S[k] ;
		d[k] = m_Controls[k] ;
	}
}
/*
2*S[0] + S[1] = 3*(y[1]-y[0]) ;
S[k-1] + 4*S[k] + S[k+1] = 3*(y[k+1]-y[k-1]) ;
2*S[L] + S[L-1] = 3*(y[L]-y[L-1]) ;
*/
void NaturalCubicSpline::ComputeDerivatives()
{
	VertVector v(L) ;
	VertVector q(L+1) ;
	v[0] = Vector(0.25, 0.25, 0.25) ;
	q[0][0] = 3*(m_Controls[1][0] - m_Controls[0][0])*v[0][0] ;
	q[0][1] = 3*(m_Controls[1][1] - m_Controls[0][1])*v[0][1] ;
	q[0][2] = 3*(m_Controls[1][2] - m_Controls[0][2])*v[0][2] ;
	for(int i = 1 ; i < L ; ++i){
		v[i][0] = 1.0/(4.0 - v[i-1][0]) ;
		v[i][1] = 1.0/(4.0 - v[i-1][1]) ;
		v[i][2] = 1.0/(4.0 - v[i-1][2]) ;
		q[i][0] = (3.0*(m_Controls[i][0] - m_Controls[i-1][0]) - q[i-1][0])*v[i][0] ;
		q[i][1] = (3.0*(m_Controls[i][1] - m_Controls[i-1][1]) - q[i-1][1])*v[i][1] ;
		q[i][2] = (3.0*(m_Controls[i][2] - m_Controls[i-1][2]) - q[i-1][2])*v[i][2] ;
	}
	q[L] = (m_Controls[L]-m_Controls[L-1])*3.0 - q[L-1] ; // ??????????
	S.reserve(L+1) ;
	S[L] = q[L] ;
	for(int i = L-1 ; i >= 0 ; --i){
		S[i][0] = q[i][0] - v[i][0]*S[i+1][0] ;
		S[i][1] = q[i][1] - v[i][1]*S[i+1][1] ;
		S[i][2] = q[i][2] - v[i][2]*S[i+1][2] ;
	}
}
void NaturalCubicSpline::ComputeDerivativesC() 
{
	S.resize(L+1) ;
	for(int i = 1 ; i < L ; ++i){
		S[i] = (m_Controls[i+1]-m_Controls[i-1])*0.5 ;	
	}
	S[0] = ( (m_Controls[1]-m_Controls[0])*3.0 - S[1] ) * 0.5 ;
	S[L] = ( (m_Controls[L] - m_Controls[L-1])*3.0 - S[L-1] ) * 0.5 ;
}
void NaturalCubicSpline::DrawCurve( int nPoints )
{
	ComputeDerivativesC() ;
	ComputeCoefficients() ;
	FILE* f = fopen("Spline.plr", "w") ;
	if(!f){
		return ;
	}
	fprintf(f,"Spline\n6\n%d\n", nPoints) ;
	int perPice = nPoints/L ;
	Vector tmp ;
	float dt = 1.0/(perPice-1) ;
	float t ; 
	for(int k = 0 ; k < L ; ++k)
	{
		for(int i = 0 ; i < perPice ; ++i){
			t = dt*i ;
			tmp = d[k] + t*(c[k] + t*( b[k] + t*a[k]) ) ;
			fprintf(f,"%f %f %f\n", tmp[0], tmp[1], tmp[2]) ;
		}
	}
	fclose(f) ;
}
void NaturalCubicSpline::DrawExistingCurve(VertVector& S_,int nPoints)
{
	ComputeSUseExisting (S_) ;
	ComputeCoefficients() ;
	FILE* f = fopen("Spline.plr", "w") ;
	if(!f){
		return ;
	}
	fprintf(f,"Spline\n6\n%d\n", nPoints) ;
	int perPice = nPoints/L ;
	Vector tmp ;
	float dt = 1.0/(perPice-1) ;
	float t ; 
	for(int k = 0 ; k < L ; ++k)
	{
		for(int i = 0 ; i < perPice ; ++i){
			t = dt*i ;
			tmp = d[k] + t*(c[k] + t*( b[k] + t*a[k]) ) ;
			fprintf(f,"%f %f %f\n", tmp[0], tmp[1], tmp[2]) ;
		}
	}
	fclose(f) ;
}

void NaturalCubicSpline::GenCurve(int nPoints, VertVector& points) 
{
	points.reserve(nPoints) ; 
	points.clear() ;
	ComputeDerivativesC() ;
	ComputeCoefficients() ;

    int perPice = (int)(double(nPoints)/L + 0.5) ;
    float dt = 1.0/(perPice);//-1) ;
    float t ; 
    Vector tmp ;
    for(int k = 0 ; k < L ; ++k)
    {
        for(int i = 0 ; i < perPice; ++i){
			t = dt*i ;
			tmp = d[k] + t*(c[k] + t*( b[k] + t*a[k]) ) ;
			points.push_back(tmp) ;
		}
	}
}
void NaturalCubicSpline::GenExistingCurve(VertVector& S_, int PerPice, VertVector& points) 
{
	points.reserve(PerPice*L) ; 
	if ( !points.empty() ){
		points.clear() ;
	}
	ComputeSUseExisting (S_) ;
	ComputeCoefficients() ;

//	int perPice = nPoints/L ;
	float dt = 1.0/(PerPice-1) ;
	float t ; 
	Vector tmp ;
	for(int k = 0 ; k < L ; ++k)
	{
		for(int i = 0 ; i < PerPice ; ++i){
			t = dt*i ;
			tmp = d[k] + t*(c[k] + t*( b[k] + t*a[k]) ) ;
			points.push_back(tmp) ;
		}
	}
}
// Returns points of k-th splined patch,
// which correspond to the parameter value beginning from t_s and ending with t_e 
void NaturalCubicSpline::GetPoints(float t_s, float t_e, float step, int k_patch, VertVector& points)
{
	Vector tmp ;
	float t = t_s;
	while(t <= t_e)
	{
		tmp = d[k_patch] + t*(c[k_patch] + t*( b[k_patch] + t*a[k_patch]) ) ;
		points.push_back(tmp) ;
		t += step ;
	}
}