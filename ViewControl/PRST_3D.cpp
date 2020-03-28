#include "stdafx.h"
#include "Kernel\Vector4.h"
#include "Kernel\Matrix4.h"
#include "Kernel\Kernel.h"
#include "Kernel\TriMeshes.h"

#include "PRST_3D.h"

#include "WildMagic/Wm5ContBox3.h"

//TODO: use Wm5::Math<double>::PI etc ?! 
// - No, PRST should be independent from Wm, since PRST does not use it and 
//		Wm part may be replaced with something else!
//Liana: I just want double precision 
const double pi = 3.141592653589793238;
const double half_pi = 1.570796326794896619;
const double RadToDegree = 180.0 / pi ;
const double DegreeToRad = pi / 180.0 ;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//#define DUMP_SYM_PLANE
//#define FORWARD_UP_BY_CUTS

#define DOT |

OpenMesh::Vec3f RotateVectorAroundVector(OpenMesh::Vec3f& axe, const OpenMesh::Vec3f& v, double angle)
{
	double a[3][3];
	double c,s;
	OpenMesh::Vec3f res;

	c = cos(angle);
	s = sin(angle);

	a[0][0] = c + (1-c)*axe[0]*axe[0];
	a[0][1] = (1-c)*axe[1]*axe[0] - s*axe[2];
	a[0][2] = (1-c)*axe[2]*axe[0] + s*axe[1];
	a[1][0] = (1-c)*axe[0]*axe[1] + s*axe[2];
	a[1][1] = c + (1-c)*axe[1]*axe[1];
	a[1][2] = (1-c)*axe[2]*axe[1] - s*axe[0];
	a[2][0] = (1-c)*axe[0]*axe[2] - s*axe[1];
	a[2][1] = (1-c)*axe[1]*axe[2] + s*axe[0];
	a[2][2] = c + (1-c)*axe[2]*axe[2];

	res[0] = v[0]*a[0][0] + v[1]*a[0][1] + v[2]*a[0][2];
	res[1] = v[0]*a[1][0] + v[1]*a[1][1] + v[2]*a[1][2];
	res[2] = v[0]*a[2][0] + v[1]*a[2][1] + v[2]*a[2][2];
	return res;
}

PRST_3::PRST_3():
	max_width(100),
	origin(0.0, 0.0, 0.0),
	d_min(0.0),
	d_max(100),
	binsT(8),
	binsA(8),
	nRo(300)
{}
PRST_3::~PRST_3()
{}

void PRST_3::run(const VertVector& grid, HVector& H)
{
	double RoD = max_width*sqrt(2.0);

//	int dmin = max_width/50.0;
//	int dmax = max_width*2;

//	int nRo = 300;
	int nAlpha = 180;
	int nTheta = 180;

	// BinsR - how many units will be considered as one ( along Ro ) - for averaging
	double binsR = RoD/(double)nRo;
	// BinsT - planes in what angular diapason will be considered as one planes - for averaging
//	int binsT = 8; // 10
//	int binsA = 8; // 10

	int half_binsA = binsA/2;
	int half_binsT = binsT/2;

	double invBinsT = 1.0/binsT;
	double invBinsA = 1.0/binsA;
	double invBinsR = 1.0/binsR;

	nRo = (int)(RoD/binsR+0.5); // JIC

	// Hough accumulator nRo x nTheta x nAlpha - Not all voxels are filled 
	H.clear(); 
	H.resize(nRo*nTheta*nAlpha) ; 

	// Fill H with lines of zero strength
	for(int i = 0 ; i < nRo/2 ; ++i)
	{
		for(int j = 0 ; j < nTheta ; ++j)
		{
			for(int k = 0 ; k < nAlpha; ++k)
			{
				H[i*nTheta*nAlpha + j*nAlpha + k].value = 0.0f;
				H[i*nTheta*nAlpha + j*nAlpha + k].Ro = i*binsR;
				H[i*nTheta*nAlpha + j*nAlpha + k].theta = j;
				H[i*nTheta*nAlpha + j*nAlpha + k].alpha = k;
			}
		}
	}
	for(int i = nRo/2 ; i < nRo ; ++i)
	{
		for(int j = 0 ; j < nTheta ; ++j)
		{
			for(int k = 0 ; k < nAlpha; ++k)
			{
				H[i*nTheta*nAlpha + j*nAlpha + k].value = 0.0f;
				H[i*nTheta*nAlpha + j*nAlpha + k].Ro = (i-nRo)*binsR; //- RoD;
				H[i*nTheta*nAlpha + j*nAlpha + k].theta = j;
				H[i*nTheta*nAlpha + j*nAlpha + k].alpha = k;
			}
		}
	}

	int Hindex;
	int ThetaIndex; // from 0 to 179 in degrees
	int AlphaIndex; // from 0 to 179 in degrees
	int RoIndex;	// from 0 to length of diagonal in units binsR
	double dx, dy, dz;
	double x, y, z;
	double d2;
	double Alpha, Theta, AlphaRad, ThetaRad, Ro; 

	double dTheta, dAlpha, dRo;

	double x0, y0,z0;
	x0 = origin[0];
	y0 = origin[1];
	z0 = origin[2];

	const size_t nVert = grid.size();
	// for each pair(pt1,pt2) in grids
	for(size_t i = 0; i < nVert; ++i)
	{
		OpenMesh::Vec3f ptI = grid[i];
		for(size_t j = i+1; j < nVert; ++j)
		{
			OpenMesh::Vec3f ptJ = grid[j];

            double dx = ptJ[0] - ptI[0];
            double dy = ptJ[1] - ptI[1];
            double dz = ptJ[2] - ptI[2];

            double d2 = sqrt(dx*dx + dy*dy + dz*dz) ;

            // Central point of the segment which connects two pixels in the pair
            double x =  ( (ptJ[0] + ptI[0])*0.5 ) - x0 ;
            double y =  ( (ptJ[1] + ptI[1])*0.5 ) - y0 ;
            double z =  ( (ptJ[2] + ptI[2])*0.5 ) - z0 ;
            // tilt angle  
            double AlphaRad = atan(dy/dx);
            double ThetaRad = atan(sqrt(dx*dx+dy*dy)/dz);

            // since atan return angle in (-pi/2, pi/2)
            // make Alpha and theta to a range [0, 359]

            if(fabs(dx) < FLT_EPSILON)
                AlphaRad = pi/2;
            else
            {
                if(dx < 0)
                    AlphaRad += pi;
                else
                {
                    if(AlphaRad < 0)
                        AlphaRad += 2*pi;
                }
            }

            if(fabs(dz) < FLT_EPSILON)
                ThetaRad = pi/2;
            else
            {
                if(dz < 0)
                    ThetaRad += pi;
            }
/*
            // 
            if(AlphaRad >= pi)
            {
                AlphaRad -= pi;
                ThetaRad = pi - ThetaRad;
            }
*/
/*
            // handle poles
            if(ThetaRad < binsT*DegreeToRad)
            {
                ThetaRad = 0;
                AlphaRad = 0;
            }
            if(fabs(ThetaRad - pi) < binsT*DegreeToRad)
            {
                ThetaRad = 0;
                AlphaRad = 0;
            }
*/
            Ro = x*cos(AlphaRad) + y*sin(AlphaRad) + z*cos(ThetaRad);

            if(fabs(Ro) < binsR){
                Ro = 0;
            }

            double Alpha = AlphaRad * RadToDegree ;
            double Theta = ThetaRad * RadToDegree ;

            int AlphaIndex = (int)(Alpha + 0.5) + half_binsA;
            int ThetaIndex = (int)(Theta + 0.5) + half_binsT;
          
            // Merging close located planes considering angular diapason
            AlphaIndex = AlphaIndex - AlphaIndex%binsA;
            ThetaIndex = ThetaIndex - ThetaIndex%binsT;
                // Mistake of averaging - Voice of this pair would be less with moving away from averaged line
            dTheta = ( 1 - fabs(Theta - ThetaIndex) * invBinsT ) ;
            dAlpha = ( 1 - fabs(Alpha - AlphaIndex) * invBinsA ) ;

            if(AlphaIndex >= 180)
            {
                AlphaIndex -= 180;
                ThetaIndex = 180 - ThetaIndex;

                AlphaIndex = AlphaIndex - AlphaIndex%binsA;
                ThetaIndex = ThetaIndex - ThetaIndex%binsT;

                Ro = -Ro;
            }

            // handle poles
            if(ThetaIndex == 0)
            {
                ThetaIndex = 0;
                AlphaIndex = 0;
            }
            if(ThetaIndex >= nTheta-binsT)
            {
                ThetaIndex = 0;
                AlphaIndex = 0;
                Ro = -Ro;
            }

            ThetaIndex = min(ThetaIndex, nTheta-1);
            AlphaIndex = min(AlphaIndex, nAlpha-1);

            AlphaIndex = max(0, AlphaIndex);
            ThetaIndex = max(0, ThetaIndex);

            int RoIndex = (int)(Ro/binsR+0.5);
                // Mistake of averaging 
            dRo = ( 1 - fabs(Ro - RoIndex*binsR) * invBinsR ) ;

            if (RoIndex < 0){
                RoIndex += nRo ;
            }

            RoIndex = min(RoIndex, nRo) ;

            ThetaRad = fabs(half_pi - ThetaRad) ; 
            float w = dAlpha + dTheta + dRo;

            w += ( 1.0/( d2*( 2 - sin(ThetaRad) ) ) ); 

            int Hindex = nAlpha*nTheta*RoIndex + ThetaIndex*nAlpha + AlphaIndex ;

			H[Hindex].value += 1 ;//w;// add voice of pair
#ifdef DUMP_SYM_PLANE
            H[Hindex].pairs.push_back(ptI);
            H[Hindex].pairs.push_back(ptJ);
#endif
		}
	}
}

void WritePlane(const OpenMesh::Vec3f& normal, const OpenMesh::Vec3f& origin, double max_width, const char* filename, bool bAppend = false);

void findMassCenters(const VertVector& normals, const VertVector& offsets, const CTriMesh* pObj, VertVector& centers, double max_width)
{
    centers.resize(normals.size(), OpenMesh::Vec3f(0.0, 0.0, 0.0));

    std::vector<int> cnts (normals.size(), 0);
    
    MeshType::ConstVertexIter vItB(pObj->_mesh.vertices_begin());
    MeshType::ConstVertexIter vItE(pObj->_mesh.vertices_end());    
    MeshType::ConstVertexIter iterI;

    for(iterI = vItB; iterI != vItE; ++iterI)
    {
        OpenMesh::Vec3f pt = OpenMesh::vector_cast<OpenMesh::Vec3f>(pObj->_mesh.point(iterI));
        for(int i = 0; i < offsets.size(); ++i)
        {
            OpenMesh::Vec3f dir = pt - offsets[i];

            if(fabs(dir|normals[i]) < max_width/100.0)
            {
                centers[i] +=  pt;
                cnts[i] ++;
            }
        }
    }
    for(int i = 0; i < offsets.size(); ++i)
    {
        if(cnts[i] == 0)
            centers[i] = OpenMesh::Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
        else
            centers[i] /= (double)cnts[i];
    }
}

void calcNormalizedView(CTriMesh* pObj, Matrix4& normView)
{
	OpenMesh::Vec3f Origin ((pObj->_xmax + pObj->_xmin)*0.5f,
							(pObj->_ymax + pObj->_ymin)*0.5f,
							(pObj->_zmax + pObj->_zmin)*0.5f);

	double max_width = max((pObj->_xmax-pObj->_xmin), 
							max((pObj->_ymax-pObj->_ymin), (pObj->_zmax-pObj->_zmin)));

	// Find first axis
		// Sample points for PRST - use app 2000 from full amount of mesh vertices
	MeshType::ConstVertexIter vItB(pObj->_mesh.vertices_begin());
	MeshType::ConstVertexIter vItE(pObj->_mesh.vertices_end());    
	MeshType::ConstVertexIter iterI;
	
	int cnt = 0;
	int nJump = pObj->_mesh.n_vertices()/2000;

#ifdef DUMP_SYM_PLANE
    FILE* f11 = fopen("grids.txt", "w");
#endif

	VertVector grids; grids.reserve(3000); 
	for(iterI = vItB; iterI != vItE; ++iterI, ++cnt)
	{
        if(cnt == 0){
			grids.push_back(OpenMesh::vector_cast<OpenMesh::Vec3f>(pObj->_mesh.point(iterI)));
#ifdef DUMP_SYM_PLANE
            fprintf(f11, "%lg %lg %lg\n", grids.back()[0], grids.back()[1], grids.back()[2]);
#endif
        }
		if(cnt == (nJump-1))
			cnt = -1;
	}
#ifdef DUMP_SYM_PLANE
    fclose(f11);
#endif
		// Setup PRST params
	PRST_3 prstObj;
	prstObj.nRo		= 300;
	prstObj.binsA	= 8;
	prstObj.binsT	= 8;
	prstObj.d_max	= 2*max_width;
	prstObj.d_min	= max_width/100.0;
	prstObj.origin	= Origin;
	prstObj.max_width = max_width;
		// run PRST
	HVector H;
	prstObj.run(grids, H);
		// Determine plane with maximal strength of symmetry
	std::vector<HLINE>::iterator max_iter, m_i ;
	max_iter = max_element( H.begin(), H.end() ) ;

#ifdef DUMP_SYM_PLANE
   FILE* f = fopen("SymPlane1.txt", "w");
   FILE* f1 = fopen("SymPlane2.txt", "w");
    for(size_t i = 0; i < max_iter->pairs.size()/2; i+=2)
    {
        fprintf(f, "%lg %lg %lg\n", max_iter->pairs[i][0], max_iter->pairs[i][1], max_iter->pairs[i][2]);
        fprintf(f1, "%lg %lg %lg\n", max_iter->pairs[i+1][0], max_iter->pairs[i+1][1], max_iter->pairs[i+1][2]);
    }
    fclose(f);
    fclose(f1);
#endif

		// find plane's normal and origin
	OpenMesh::Vec3f symPlaneNormal, symPlaneOrigin;
	symPlaneNormal = OpenMesh::Vec3f(cos(max_iter->alpha*DegreeToRad)*sin(max_iter->theta*DegreeToRad),
									 sin(max_iter->alpha*DegreeToRad)*sin(max_iter->theta*DegreeToRad),
									 cos(max_iter->theta*DegreeToRad));
	symPlaneNormal.normalize();
	symPlaneOrigin = symPlaneNormal*(double)max_iter->Ro + Origin;

	// Find second axis
		//Compute center of mass for middle points within thin layer, created by two planes, parallel with symmetry plane
	OpenMesh::Vec3f MassC (0.0, 0.0, 0.0);

	cnt = 0;
	std::vector< Wm5::Vector3<double> > vertWm;
	for(iterI = vItB; iterI != vItE; ++iterI)
	{
		OpenMesh::Vec3f pt = OpenMesh::vector_cast<OpenMesh::Vec3f>(pObj->_mesh.point(iterI));
		OpenMesh::Vec3f dir = pt - symPlaneOrigin;

		if(fabs(dir|symPlaneNormal) < max_width/100.0)
		{
			vertWm.push_back(Wm5::Vector3<double>(pt[0], pt[1], pt[2]));
			MassC +=  pt;
			cnt ++;
		}
	}
	MassC /= (double)cnt;

	Wm5::Box3<double> box = Wm5::ContOrientedBox(vertWm.size(), &vertWm[0]);
	// Fill rotation matrix:
		// Translation components
	normView.SetRow(3, Vector4(MassC[0], MassC[1], MassC[2], 1.0));
	//normView[3][0] = MassC.x;
	//normView[3][1] = MassC.y;
	//normView[3][2] = MassC.z;
	//normView[3][3] = 1.0;
		// Rotation components
			//right direction - symPlaneNormal
			//up direction - smallest side in oriented box, except of symPlaneNormal
			//forward direction - remaining one
	
	int xAxis = -1, yAxis = -1, zAxis =-1; // x - right, y - up, z - forward
	for(int i = 0; i < 3; ++i)
	{
		if( fabs(1 - fabs(symPlaneNormal|OpenMesh::Vec3f(box.Axis[i][0], box.Axis[i][1], box.Axis[i][2]))) < 0.01)
			xAxis = i;
	}
	if(xAxis != -1)
	{
		double minExtends = max_width;
		for(int i = 0; i < 3; ++i)
		{
			if(i == xAxis)
				continue;
			if(box.Extent[i] < minExtends)
			{
				minExtends = box.Extent[i];
				yAxis = i;
			}
		}
		zAxis = 3 - xAxis - yAxis;

		// Find bottom vs top and forward vs back
		//	MassC always will be in bottom back part of the bounding box
		OpenMesh::Vec3f dir = MassC - Origin; 
		OpenMesh::Vec3f rightDir(box.Axis[xAxis][0], box.Axis[xAxis][1], box.Axis[xAxis][2]);
		OpenMesh::Vec3f upDir(box.Axis[yAxis][0], box.Axis[yAxis][1], box.Axis[yAxis][2]);
		OpenMesh::Vec3f forwardDir(box.Axis[zAxis][0], box.Axis[zAxis][1], box.Axis[zAxis][2]);
        OpenMesh::Vec3f boxC(box.Center[0], box.Center[1], box.Center[2]);

#ifdef FORWARD_UP_BY_CUTS
        VertVector normals, offsets, centers;
        normals.push_back(upDir);
        offsets.push_back(OpenMesh::Vec3f(box.Center[0] + box.Axis[yAxis][0]*(box.Extent[yAxis]+max_width/100),
                                            box.Center[1] + box.Axis[yAxis][1]*(box.Extent[yAxis]+max_width/100),
                                            box.Center[2] + box.Axis[yAxis][2]*(box.Extent[yAxis]+max_width/100)));
        normals.push_back(forwardDir);
        offsets.push_back(OpenMesh::Vec3f(box.Center[0] + box.Axis[zAxis][0]*(box.Extent[zAxis]+max_width/100),
                                            box.Center[1] + box.Axis[zAxis][1]*(box.Extent[zAxis]+max_width/100),
                                            box.Center[2] + box.Axis[zAxis][2]*(box.Extent[zAxis]+max_width/100)));

        findMassCenters(normals, offsets, pObj, centers, max_width);
        bool bNeedRecalc = false;
       
            if(centers[0][0] == FLT_MAX)
            {
                bNeedRecalc = true;
                offsets[0] = OpenMesh::Vec3f(box.Center[0] - box.Axis[yAxis][0]*(box.Extent[yAxis]+max_width/100),
                                             box.Center[1] - box.Axis[yAxis][1]*(box.Extent[yAxis]+max_width/100),
                                             box.Center[2] - box.Axis[yAxis][2]*(box.Extent[yAxis]+max_width/100));
            }
            if(centers[1][0] == FLT_MAX)
            {
                bNeedRecalc = true;
                offsets[1] = OpenMesh::Vec3f(box.Center[0] - box.Axis[zAxis][0]*(box.Extent[zAxis]+max_width/100),
                                             box.Center[1] - box.Axis[zAxis][1]*(box.Extent[zAxis]+max_width/100),
                                             box.Center[2] - box.Axis[zAxis][2]*(box.Extent[zAxis]+max_width/100));
            }
      
        if(bNeedRecalc)
            findMassCenters(normals, offsets, pObj, centers, max_width);

        WritePlane(normals[0], offsets[0], max_width, "UpCut.stl");
        WritePlane(normals[1], offsets[1], max_width, "ForwardCut.stl");

        if((boxC-centers[0]).length() > (boxC-centers[1]).length())
        {
            yAxis = zAxis;
            zAxis = 3 - xAxis - yAxis;

            upDir = OpenMesh::Vec3f (box.Axis[yAxis][0], box.Axis[yAxis][1], box.Axis[yAxis][2]);
            forwardDir = OpenMesh::Vec3f(box.Axis[zAxis][0], box.Axis[zAxis][1], box.Axis[zAxis][2]);
        }
#endif
        if( (dir|upDir) > 0)
		{
			upDir = RotateVectorAroundVector(rightDir, upDir, pi);
			forwardDir = RotateVectorAroundVector(rightDir, forwardDir, pi);
		}

        OpenMesh::Vec3f offsetOr(boxC[0] + forwardDir[0]*(max_width/100),
                                boxC[1] + forwardDir[1]*(max_width/100),
                                boxC[2] + forwardDir[2]*(max_width/100));

		int cntLeft = 0, cntRight = 0;
		for(iterI = vItB; iterI != vItE; ++iterI)
		{
			OpenMesh::Vec3f pt = OpenMesh::vector_cast<OpenMesh::Vec3f>(pObj->_mesh.point(iterI));
			OpenMesh::Vec3f dir = pt - offsetOr;// symPlaneOrigin;//boxC;//

			if( (dir|forwardDir) < 0)
				cntLeft ++;
			else
				cntRight ++;
		}

		if(cntLeft < cntRight)
		{
			rightDir = RotateVectorAroundVector(upDir, rightDir, pi);
			forwardDir = RotateVectorAroundVector(upDir, forwardDir, pi);
		}

		normView.SetRow(0, Vector4(rightDir[0], rightDir[1], rightDir[2], 0.0));
		normView.SetRow(1, Vector4(upDir[0], upDir[1], upDir[2], 0.0));
		normView.SetRow(2, Vector4(forwardDir[0], forwardDir[1], forwardDir[2], 0.0));

// Find center of skull more exactly 
        double delta = max_width/50;
        double minRdSk = FLT_MAX, maxRdSk = -FLT_MAX;
        for(iterI = vItB; iterI != vItE; ++iterI)
        {
            OM_Vector3 pt = pObj->_mesh.point(iterI);
            OM_Vector3 dir = pt - boxC;

            Real Rd = (dir DOT rightDir);
            // cut points far from the skull - possible antlers growing down
            if(fabs(Rd) > max_width/3.0)
                continue;

            Real RdUp = (dir DOT upDir);

            if( RdUp < -box.Extent[yAxis]*0.0)
            {
                // cut points above middle of the skull
                minRdSk = min(Rd, minRdSk);
                maxRdSk = max(Rd, maxRdSk);
            }
        }
        OpenMesh::Vec3f newCenter = boxC + rightDir*(maxRdSk+minRdSk)*0.5;

        Box3 skullBox;
        skullBox.Axis[0] = Vector3(rightDir[0], rightDir[1], rightDir[2]);
        skullBox.Axis[1] = Vector3(upDir[0], upDir[1], upDir[2]);
        skullBox.Axis[2] = Vector3(forwardDir[0], forwardDir[1], forwardDir[2]);

        skullBox.Extent[0] = box.Extent[xAxis];
        skullBox.Extent[1] = box.Extent[yAxis];
        skullBox.Extent[2] = box.Extent[zAxis];

        skullBox.Center = Vector3(newCenter[0], newCenter[1], newCenter[2]);//Vector3(box.Center[0], box.Center[1], box.Center[2]);

        pObj->SetSkullBox(skullBox);
#if 0
        WritePlane(symPlaneNormal, symPlaneOrigin, max_width, "PlaneSym.stl");
        WritePlane(rightDir, boxC+rightDir*maxRdSk, max_width, "PlaneMiddleMax.stl");
        WritePlane(rightDir, boxC+rightDir*minRdSk, max_width, "PlaneMiddleMin.stl");
        WritePlane(rightDir, newCenter, max_width, "PlaneMiddle.stl");
        WritePlane(rightDir, boxC, max_width, "PlaneMiddleOld.stl");
        WritePlane(rightDir, newCenter+rightDir*max_width/8.0, max_width, "PlaneMiddleCutL.stl");
        WritePlane(rightDir, newCenter-rightDir*max_width/8.0, max_width, "PlaneMiddleCutR.stl");
        WritePlane(upDir, boxC, max_width, "PlaneMiddleCutUp.stl");
#endif
#ifdef DUMP_SYM_PLANE
		FILE* f;
		fopen_s(&f, "Axes.txt", "w");
		if(f)
		{
            fprintf(f, "%d %d %lg\n", max_iter->alpha, max_iter->theta, max_iter->Ro);
			fprintf(f, "%d %d %d %d\n", cntLeft, cntRight, cntLeftOff, cntRightOff);
			for(int i = 0 ; i < 3; ++i)
			{
				Vector4 row = normView.GetRow(i);
				fprintf(f, "%lg %lg %lg\n", row.x, row.y,row.z);
			}
			fclose(f);
		}

        WritePlane(symPlaneNormal, symPlaneOrigin, max_width, "PlaneSym.stl");
        WritePlane(forwardDir, symPlaneOrigin, max_width, "PlaneWmX.stl");
        WritePlane(rightDir, symPlaneOrigin, max_width, "PlaneWmY.stl");
        WritePlane(upDir, symPlaneOrigin, max_width, "PlaneWmZ.stl");
#endif 
 	}
}

void setNormalizedView(CTriMesh* pObj, Matrix4& normView)
{
    OpenMesh::Vec3f Origin ((pObj->_xmax + pObj->_xmin)*0.5f,
        (pObj->_ymax + pObj->_ymin)*0.5f,
        (pObj->_zmax + pObj->_zmin)*0.5f);

    double max_width = max((pObj->_xmax-pObj->_xmin), 
        max((pObj->_ymax-pObj->_ymin), (pObj->_zmax-pObj->_zmin)));

    size_t xAxis = 0, yAxis = 1, zAxis = 2;

    Vector3 rightDir   = normView.GetRow(xAxis);
    Vector3 upDir      = normView.GetRow(yAxis);
    Vector3 forwardDir = normView.GetRow(zAxis);

    Real scale = rightDir.Length();

    upDir.Normalize();
    rightDir.Normalize();
    forwardDir.Normalize();

    OM_Vector3 rightDir_(rightDir[0], rightDir[1], rightDir[2]);
    OM_Vector3 upDir_(upDir[0], upDir[1], upDir[2]);
    OM_Vector3 forwardDir_(forwardDir[0], forwardDir[1], forwardDir[2]);

    // Find second axis
    //Compute center of mass for middle points within thin layer, created by two planes, parallel with symmetry plane
    OM_Vector3 MassC (0.0, 0.0, 0.0);

    MeshType::ConstVertexIter vItB(pObj->_mesh.vertices_begin());
    MeshType::ConstVertexIter vItE(pObj->_mesh.vertices_end());    
    MeshType::ConstVertexIter iterI;

    size_t cnt = 0;
    std::vector< Wm5::Vector3<double> > vertWm;
    for(iterI = vItB; iterI != vItE; ++iterI)
    {
        OM_Vector3 pt = pObj->_mesh.point(iterI);
        OM_Vector3 dir = pt - Origin;

        if(fabs(dir DOT rightDir_) < max_width/100.0)
        {
            vertWm.push_back(Wm5::Vector3<double>(pt[0], pt[1], pt[2]));
            MassC +=  pt;
            cnt ++;
        }
    }
    MassC /= (double)cnt;

    Wm5::Box3<double> box = Wm5::ContOrientedBox(vertWm.size(), &vertWm[0]);
    OM_Vector3 boxC (box.Center[0], box.Center[1], box.Center[2]);

    // Translation components // ??????
    normView.SetRow(xAxis, Vector4(rightDir));
    normView.SetRow(yAxis, Vector4(upDir));
    normView.SetRow(zAxis, Vector4(forwardDir));
    normView.SetRow(3, Vector4(MassC[0], MassC[1], MassC[2], 1.0));
   
        // Find center of skull more exactly 
    double delta = max_width/50;
    double minRdSk = FLT_MAX, maxRdSk = -FLT_MAX;
    for(iterI = vItB; iterI != vItE; ++iterI)
    {
        OM_Vector3 pt = pObj->_mesh.point(iterI);
        OM_Vector3 dir = pt - boxC;

        Real Rd = (dir DOT rightDir_);
        // cut points far from the skull - possible antlers growing down
        if(fabs(Rd) > max_width/3.0)
            continue;

        Real RdUp = (dir DOT upDir_);

        if( RdUp < -box.Extent[yAxis]*0.75)
        {
            // cut points above middle of the skull
            minRdSk = min(Rd, minRdSk);
            maxRdSk = max(Rd, maxRdSk);
        }
    }
    OM_Vector3 newCenter = boxC + rightDir_*(maxRdSk+minRdSk)*0.5;

    Box3 skullBox;
    skullBox.Axis[0] = Vector3(rightDir[0], rightDir[1], rightDir[2]);
    skullBox.Axis[1] = Vector3(upDir[0], upDir[1], upDir[2]);
    skullBox.Axis[2] = Vector3(forwardDir[0], forwardDir[1], forwardDir[2]);

    skullBox.Extent[0] = box.Extent[xAxis];
    skullBox.Extent[1] = box.Extent[yAxis];
    skullBox.Extent[2] = box.Extent[zAxis];

    skullBox.Center = Vector3(newCenter[0], newCenter[1], newCenter[2]);

    pObj->SetSkullBox(skullBox);
}

void WritePlane(const OpenMesh::Vec3f& normal, const OpenMesh::Vec3f& origin, double max_width, const char* filename, bool bAppend)
{
    OpenMesh::Vec3f xVec = OpenMesh::Vec3f(1.0, 0.0, 0.0);
    OpenMesh::Vec3f yVec = normal%xVec;
    if(yVec.length() < 0.001)
        yVec = normal%OpenMesh::Vec3f(0.0, 1.0, 0.0);
    OpenMesh::Vec3f u = yVec%normal; u.normalize();
    OpenMesh::Vec3f v = normal%u; v.normalize();

    OpenMesh::Vec3f pt1, pt2, pt3, pt4;

    pt1 = origin + u*(max_width/2.0) + v*(max_width/2.0);
    pt2 = origin - u*(max_width/2.0) + v*(max_width/2.0);
    pt3 = origin - u*(max_width/2.0) - v*(max_width/2.0);
    pt4 = origin + u*(max_width/2.0) - v*(max_width/2.0);

    FILE* f = NULL;
    if(!bAppend)
    {
        if( !( f = fopen( filename , "w") ) )
            return;
    }
    else
    {
        if( !( f = fopen( filename , "a") ) )
            return;
    }

    if(!bAppend)
        fprintf (f , "solid Object01\n") ;

    fprintf ( f , "  facet normal %lg %lg %lg\n" , normal[0] , normal[1] , normal[2] ) ;
    fprintf( f , "    outer loop\n") ;

    fprintf( f , "      vertex %lg %lg %lg\n" , pt1[0], pt1[1], pt1[2]) ;
    fprintf( f , "      vertex %lg %lg %lg\n" , pt2[0], pt2[1], pt2[2]) ;
    fprintf( f , "      vertex %lg %lg %lg\n" , pt3[0], pt3[1], pt3[2]) ;

    fprintf( f , "    endloop\n") ;
    fprintf( f , "  endfacet\n") ;

    fprintf ( f , "  facet normal %lg %lg %lg\n" , normal[0] , normal[1] , normal[2] ) ;
    fprintf( f , "    outer loop\n") ;

    fprintf( f , "      vertex %lg %lg %lg\n" , pt1[0], pt1[1], pt1[2]) ;
    fprintf( f , "      vertex %lg %lg %lg\n" , pt3[0], pt3[1], pt3[2]) ;
    fprintf( f , "      vertex %lg %lg %lg\n" , pt4[0], pt4[1], pt4[2]) ;

    fprintf( f , "    endloop\n") ;
    fprintf( f , "  endfacet\n") ;

    if(!bAppend)
        fprintf ( f , "endsolid Object01\n") ;
    fclose(f) ;
}
