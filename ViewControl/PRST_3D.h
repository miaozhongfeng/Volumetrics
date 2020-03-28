
#pragma once

typedef std::vector<OpenMesh::Vec3f> VertVector;
typedef OpenMesh::Vec3f Vector;

struct HLINE
{
    VertVector pairs;
	float value ; // Symmetry strength of the plane
	double Ro;
	int theta, alpha ; // Ro, theta - parameters of planes's normal equation 
	//with considering center of bounding box as origin of coordinate system 

	friend bool operator < (const HLINE& lh, const HLINE& rh){
		return lh.value < rh.value ;
	}
	friend bool operator > (const HLINE& lh, const HLINE& rh){
		return lh.value > rh.value ;
	}
};

typedef std::vector<HLINE> HVector;
typedef std::vector<HLINE>::const_iterator HVecIter;

class PRST_3
{
public:
	PRST_3();
	~PRST_3();

	void run(const VertVector& grid, HVector& H);

	int d_min;
	int d_max;

	OpenMesh::Vec3f origin;
	double			max_width;

	int binsT;
	int binsA;

	int nRo;
};