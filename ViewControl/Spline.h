#pragma once 

typedef std::vector<OpenMesh::Vec3f> VertVector;
typedef OpenMesh::Vec3f Vector;

class NaturalCubicSpline
{
public:
	NaturalCubicSpline(const VertVector& points) :
	  m_Controls(points) {
		  L = m_Controls.size() - 1 ;
	  } ;
	  ~NaturalCubicSpline() {} ;

	// Computes derivations in each spline node:
	void ComputeSUseExisting(VertVector& S_) ;
	// Computes coefficients of 3rd degree polynomial for each spline patch: 
	// use only with computed derivations
	void ComputeCoefficients() ;
	// Returns points of k-th splined patch,
	// which correspond to the parameter value beginning from t_s and ending with t_e 
	void GetPoints(float t_s, float t_e, float step, int k_patch, VertVector& points);
public:
	void DrawCurve(int nPoints) ;
	void DrawExistingCurve(VertVector& S_,int nPoints) ;
	void GenCurve(int nPoints, VertVector& points) ;
	void GenExistingCurve(VertVector& S_, int nPoints, VertVector& points) ;
private:
	void ComputeDerivatives() ;
	void ComputeDerivativesC() ;
private:
public:
	VertVector m_Controls ; // контрольные точки -  
	VertVector S ;			// производные в контрольных точках
	VertVector a, b, c, d ;// коэфециэнты полинома 3 степени для каждого отрезка
	int L ; // m_Controls.size() - 1 = L ;
protected:
};
