//--
#include "stdafx.h"
#include "Debug\Debugger.h"
#include "BurrPlaneFinder.h"

#include "Spline.h"
#include "WildMagic/Wm5ApprPlaneFit3.h"

#include "Kernel/Plane.h"

#define DOT |
#define CROSS %

#ifndef max
    #define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// Constructor/destructor
BurrPlaneFinder::BurrPlaneFinder(CTriMesh* triMesh):
    m_triMesh(triMesh)
{}
BurrPlaneFinder::~BurrPlaneFinder()
{}

extern void WritePlane(const OpenMesh::Vec3f& normal, const OpenMesh::Vec3f& origin, double max_width, const char* filename, bool bAppend = false);
bool BurrPlaneFinder::FindBurrPlanes()
{
    const uint xAxis = 0, yAxis = 1, zAxis = 2 ;

    Real max_width = m_triMesh->_maxWidth;

    OM_Vector3 rightDir  (m_triMesh->_skullBox.Axis[xAxis][0], m_triMesh->_skullBox.Axis[xAxis][1], m_triMesh->_skullBox.Axis[xAxis][2]);
    OM_Vector3 upDir     (m_triMesh->_skullBox.Axis[yAxis][0], m_triMesh->_skullBox.Axis[yAxis][1], m_triMesh->_skullBox.Axis[yAxis][2]);
    OM_Vector3 forwardDir(m_triMesh->_skullBox.Axis[zAxis][0], m_triMesh->_skullBox.Axis[zAxis][1], m_triMesh->_skullBox.Axis[zAxis][2]);
    OM_Vector3 boxC      (m_triMesh->_skullBox.Center[0], m_triMesh->_skullBox.Center[1], m_triMesh->_skullBox.Center[2]);

    MeshType::ConstVertexIter vItB(m_triMesh->_mesh.vertices_begin());
    MeshType::ConstVertexIter vItE(m_triMesh->_mesh.vertices_end());    
    MeshType::ConstVertexIter iterI;

    m_delta = max_width/50;
    m_radius = max_width/15;
    
    Real step = m_radius * 0.25;

// Step I.
//        Find width of the skull in the right direction
//        Use this width to determines the "skull area", where burr rings are located
//        For left and right branches cut antler tips if possible and save other points in the skull area

    double minRd = FLT_MAX, maxRd = -FLT_MAX;
    double minRdSk = FLT_MAX, maxRdSk = -FLT_MAX;
        // find skull width
    for(iterI = vItB; iterI != vItE; ++iterI)
    {
        OM_Vector3 pt = m_triMesh->_mesh.point(iterI);
        OM_Vector3 dir = pt - boxC;

        Real RdUp = (dir DOT upDir);

        if( RdUp > m_triMesh->_skullBox.Extent[yAxis] + m_delta)
            continue;

        Real Rd = (dir DOT rightDir);
        // cut points far from the skull - possible antlers growing down
        if(fabs(Rd) > max_width/3.0)
            continue;
        
        // cut points far from the skull - possible antlers growing to the down-middle
        double Rf = (dir|forwardDir);
        if(abs(Rf) > max_width/3.0)
            continue;

        if( RdUp < -m_triMesh->_skullBox.Extent[yAxis]*0.5)
        {
            // cut points above middle of the skull
            minRdSk = min(Rd, minRdSk);
            maxRdSk = max(Rd, maxRdSk);
        }

        if( fabs(RdUp - m_triMesh->_skullBox.Extent[yAxis]) > max_width/200)
            continue;

        minRd = min(Rd, minRd);
        maxRd = max(Rd, maxRd);
    }
        // from where begin searching burr rings
    if(fabs(minRd - minRdSk) < m_radius)
        minRd = minRdSk - m_radius;

    if(fabs(maxRd - maxRdSk) < m_radius)
        maxRd = maxRdSk + m_radius;

    m_minRd   = minRd;   m_maxRd   = maxRd;
    m_minRdSk = minRdSk; m_maxRdSk = maxRdSk;
#if 0
    WritePlane(rightDir, boxC+rightDir*minRd, max_width, "MinRigth.stl");
    WritePlane(rightDir, boxC+rightDir*maxRd, max_width, "MaxRight.stl");

    WritePlane(rightDir, boxC+rightDir*minRdSk, max_width, "MinRigthSk.stl");
    WritePlane(rightDir, boxC+rightDir*maxRdSk, max_width, "MaxRightSk.stl");

    WritePlane(upDir, boxC+upDir*m_triMesh->_skullBox.Extent[yAxis], max_width, "UpCut.stl");
#endif

    // Right side - side with positive "right" values, Left - with negative
    size_t discr = 10;
    std::vector<Real> minValsLeft(discr, FLT_MAX);
    std::vector<Real> minValsRight(discr, FLT_MAX);

    Real right_max = maxRd + m_delta;
    Real right_min = maxRdSk*0.1;//maxRdSk*0.1;

    Real right_width = right_max - right_min;
    Real right_delta = right_width/discr;

    Real left_max = minRd - m_delta;
    Real left_min = minRdSk*0.1;//minRdSk*0.1;

    Real left_width = -left_max + left_min;
    Real left_delta = left_width/discr;
    // Find minimal up values for all cuts along the antlers
    size_t cntL = 0, cntR = 0;
    for(iterI = vItB; iterI != vItE; ++iterI)
    {
        OM_Vector3 pt = m_triMesh->_mesh.point(iterI);
        OM_Vector3 dir = pt - boxC;

        // cut points far from the skull - possible antlers growing to the down-middle
        double Rf = (dir|forwardDir);
        if(abs(Rf) > max_width/3.0)
            continue;

        double Rd = (dir|rightDir);
        // cut all point beyond right bound
        if(Rd > right_max)
            continue;
        // cut all point beyond left bound
        if(Rd < left_max)
            continue;

        Real RdUp = (dir|upDir);
        if( RdUp < m_triMesh->_skullBox.Extent[yAxis]*0.25){
            continue;
        }

        int minInd = -1;
        if(Rd > right_min)
        {
            cntR++;
            minInd = min(Rd/right_delta, discr-1);
            minValsRight[minInd] = min(minValsRight[minInd], RdUp);
        }
        else if(Rd < left_min)
        {
            cntL++;
            minInd = min(fabs(Rd)/fabs(left_delta), discr-1);
            minValsLeft[minInd] = min(minValsLeft[minInd], RdUp);
        }
    }

    // Find points, admitted for burr ring finding
    VertVector skullGridsR, skullGridsL;
    HandlesVector skullGridsHdlsR, skullGridsHdlsL;

    skullGridsR.reserve(cntR);     skullGridsL.reserve(cntL);
    skullGridsHdlsR.reserve(cntL); skullGridsHdlsL.reserve(cntL);

    for(iterI = vItB; iterI != vItE; ++iterI)
    {
        OM_Vector3 pt = m_triMesh->_mesh.point(iterI);
        OM_Vector3 dir = pt - boxC;

        double Rf = (dir|forwardDir);
        if(abs(Rf) > max_width/3.0)
            continue;

        double Rd = (dir|rightDir);

        // cut all point beyond right bound
        if(Rd > right_max)
            continue;
        // cut all point beyond left bound
        if(Rd < left_max)
            continue;
        // cut all middle points
        if(Rd < right_min && Rd > left_min)
            continue;

        double RdUp = (dir|upDir);
        
        size_t minInd = -1;
        if(Rd > right_min)
        {
            minInd = min(Rd/right_delta, discr-1);
            if(RdUp > minValsRight[minInd] + 2.3*m_radius) // diameter + little bit
                continue;

            skullGridsR.push_back(pt);
            skullGridsHdlsR.push_back(iterI);
        }
        else if(Rd < left_min)
        {
            minInd = min(fabs(Rd)/left_delta, discr-1);
            if(RdUp > minValsLeft[minInd] + 2.3*m_radius) // diameter + little bit
                continue;
            skullGridsL.push_back(pt);
            skullGridsHdlsL.push_back(iterI);
        }
    }
#if 0
    FILE* f = fopen("skullGridsLeft.plr", "w");
    fprintf(f, "skullGridsLeft\n6\n%d\n", skullGridsL.size());
    for(size_t i = 0 ; i < skullGridsHdlsL.size(); ++i)
        fprintf(f, "%lg %lg %lg\n", skullGridsL[i][0], skullGridsL[i][1], skullGridsL[i][2]);
    fclose(f);

    f = fopen("skullGridsRight.plr", "w");
    fprintf(f, "skullGridsLeft\n6\n%d\n", skullGridsR.size());
    for(size_t i = 0 ; i < skullGridsHdlsR.size(); ++i)
        fprintf(f, "%lg %lg %lg\n", skullGridsR[i][0], skullGridsR[i][1], skullGridsR[i][2]);
    fclose(f);
#endif
// Step II. Find the centerlines for left and right branches
    
    OM_Vector3 offsetLeft(boxC + rightDir*m_minRd);
    OM_Vector3 offsetRight(boxC + rightDir*m_maxRd);
    
    VertVector centerLinePtsL, centerLinePtsR;
    if(!_computeCenterLine(skullGridsL, centerLinePtsL, offsetLeft, max_width/14.0)){//fabs(m_minRdSk)*0.6)){
        AntlersDebugger::logMessage("ERROR: BurrPlaneFinder: Could not find centerline for left branch");
        return false;
    }
    if(!_computeCenterLine(skullGridsR, centerLinePtsR, offsetRight, max_width/14.0)){//fabs(m_maxRdSk)*0.6)){
        AntlersDebugger::logMessage("ERROR: BurrPlaneFinder: Could not find centerline for right branch");
        return false;
    }
#if 0
    f = fopen("centerlineLeft.plr", "w");
    fprintf(f, "centerline\n6\n%d\n", centerLinePtsL.size());
    for(size_t i = 0 ; i < centerLinePtsL.size(); ++i)
        fprintf(f, "%lg %lg %lg\n", centerLinePtsL[i][0], centerLinePtsL[i][1], centerLinePtsL[i][2]);
    fclose(f);

    f = fopen("centerlineRight.plr", "w");
    fprintf(f, "centerline\n6\n%d\n", centerLinePtsR.size());
    for(size_t i = 0 ; i < centerLinePtsR.size(); ++i)
        fprintf(f, "%lg %lg %lg\n", centerLinePtsR[i][0], centerLinePtsR[i][1], centerLinePtsR[i][2]);
    fclose(f);
#endif
// Step III. Cut mesh on rings and find ring with maximal average curvature
//           Perform least squares approximation for first N points with
//           maximal mean curvature in a ring with maximal average curvature

    m_triMesh->_mesh.add_property(m_gaussCurvP);
    m_triMesh->_mesh.add_property(m_meanCurvP);

    if(!_detBurPlane(centerLinePtsL, skullGridsL, skullGridsHdlsL, m_burrPlane[0])){
        AntlersDebugger::logMessage("ERROR: BurrPlaneFinder: Could not determine burr plane for left branch");
        return false;
    }

    if(!_detBurPlane(centerLinePtsR, skullGridsR, skullGridsHdlsR, m_burrPlane[1])){
        AntlersDebugger::logMessage("ERROR: BurrPlaneFinder: Could not determine burr plane for right branch");
        return false;
    }

#if 0
    WritePlane(m_burrPlane[0].normal, m_burrPlane[0].center, m_burrPlane[0].radius, "burrPlaneL.stl", false);
    WritePlane(m_burrPlane[1].normal, m_burrPlane[1].center, m_burrPlane[1].radius, "burrPlaneR.stl", false);
#endif

//    if(!_testLeftRight(skullGridsL, skullGridsHdlsL))
//        _testRightLeft(skullGridsR, skullGridsHdlsR);

    m_triMesh->_mesh.remove_property(m_gaussCurvP);
    m_triMesh->_mesh.remove_property(m_meanCurvP);

    return true;
}

//tests weather it is better to take reflected right burr plane instead of left
bool BurrPlaneFinder::_testLeftRight(const VertVector& skullGrids, const HandlesVector& skullGridsHdls)
{
    const uint xAxis = 0, yAxis = 1, zAxis = 2 ;

    OM_Vector3 rightDir  (m_triMesh->_skullBox.Axis[xAxis][0], m_triMesh->_skullBox.Axis[xAxis][1], m_triMesh->_skullBox.Axis[xAxis][2]);
    OM_Vector3 upDir     (m_triMesh->_skullBox.Axis[yAxis][0], m_triMesh->_skullBox.Axis[yAxis][1], m_triMesh->_skullBox.Axis[yAxis][2]);
    OM_Vector3 boxC      (m_triMesh->_skullBox.Center[0], m_triMesh->_skullBox.Center[1], m_triMesh->_skullBox.Center[2]);

    Plane rightPl ( m_triMesh->_skullBox.Axis[xAxis], m_triMesh->_skullBox.Center);

    Vector3 reflNormR = rightPl.getImageVector(Vector3(m_burrPlane[1].normal[0], m_burrPlane[1].normal[1], m_burrPlane[1].normal[2]));
    Vector3 reflCenterR = rightPl.getImageRv(Vector3(m_burrPlane[1].center[0], m_burrPlane[1].center[1], m_burrPlane[1].center[2]));

    OM_Vector3 normRefl(reflNormR[0], reflNormR[1], reflNormR[2]);
    OM_Vector3 centerRefl(reflCenterR[0], reflCenterR[1], reflCenterR[2]);

    centerRefl = centerRefl + normRefl*(fabs(m_minRdSk)-fabs(m_maxRdSk))*0.5;

    WritePlane(m_burrPlane[1].normal, m_burrPlane[1].center, m_triMesh->_maxWidth, "Right.stl", false);
    WritePlane(normRefl, centerRefl, m_triMesh->_maxWidth, "ReflectedRight.stl", false);
    WritePlane(rightDir, boxC, m_triMesh->_maxWidth, "planeCenterY.stl", false);

    Real curvatureOld, curvatureNew;
    _computeAverageCutCurvatures(m_burrPlane[0].normal, m_burrPlane[0].center, skullGrids, skullGridsHdls, m_meanCurvP, curvatureOld);
    _computeAverageCutCurvatures(normRefl, centerRefl, skullGrids, skullGridsHdls, m_meanCurvP, curvatureNew);
    AntlersDebugger::logMessage("old curvature %lg new curvature %lg", curvatureOld, curvatureNew);
    if(curvatureNew > curvatureOld)
    {
        _computeAverageBurrPlane(normRefl, centerRefl, skullGrids, skullGridsHdls, m_meanCurvP, m_burrPlane[0]);
        return true;
    }
    return false;
}
//tests weather it is better to take reflected right burr plane instead of left
bool BurrPlaneFinder::_testRightLeft(const VertVector& skullGrids, const HandlesVector& skullGridsHdls)
{
    const uint xAxis = 0, yAxis = 1, zAxis = 2 ;

    OM_Vector3 rightDir  (m_triMesh->_skullBox.Axis[xAxis][0], m_triMesh->_skullBox.Axis[xAxis][1], m_triMesh->_skullBox.Axis[xAxis][2]);
    OM_Vector3 upDir     (m_triMesh->_skullBox.Axis[yAxis][0], m_triMesh->_skullBox.Axis[yAxis][1], m_triMesh->_skullBox.Axis[yAxis][2]);
    OM_Vector3 boxC      (m_triMesh->_skullBox.Center[0], m_triMesh->_skullBox.Center[1], m_triMesh->_skullBox.Center[2]);

    Plane rightPl ( m_triMesh->_skullBox.Axis[xAxis], m_triMesh->_skullBox.Center);

    Vector3 reflNormR = rightPl.getImageVector(Vector3(m_burrPlane[0].normal[0], m_burrPlane[0].normal[1], m_burrPlane[0].normal[2]));
    Vector3 reflCenterR = rightPl.getImageRv(Vector3(m_burrPlane[0].center[0], m_burrPlane[0].center[1], m_burrPlane[0].center[2]));

    OM_Vector3 normRefl(reflNormR[0], reflNormR[1], reflNormR[2]);
    OM_Vector3 centerRefl(reflCenterR[0], reflCenterR[1], reflCenterR[2]);

//    centerRefl = centerRefl - normRefl*(fabs(m_maxRdSk)-fabs(m_minRdSk))*0.75;

    Real curvatureOld, curvatureNew = -FLT_MAX;
    _computeAverageCutCurvatures(m_burrPlane[1].normal, m_burrPlane[1].center, skullGrids, skullGridsHdls, m_meanCurvP, curvatureOld);
  
    centerRefl = centerRefl - normRefl*(fabs(m_maxRdSk)-fabs(m_minRdSk))*1.0;
    for(int i = 0 ; i < 6; ++i)
    {
        Real curvature;
        _computeAverageCutCurvatures(normRefl, centerRefl, skullGrids, skullGridsHdls, m_meanCurvP, curvature);
        curvatureNew = max(curvatureNew, curvature);

        centerRefl = centerRefl + normRefl*(fabs(m_maxRdSk)-fabs(m_minRdSk))*0.25;
    }

    WritePlane(m_burrPlane[0].normal, m_burrPlane[0].center, m_triMesh->_maxWidth, "Left.stl", false);
    WritePlane(normRefl, centerRefl, m_triMesh->_maxWidth, "ReflectedLeft.stl", false);

    AntlersDebugger::logMessage("old curvature %lg new curvature %lg", curvatureOld, curvatureNew);
    if(curvatureNew > curvatureOld || 
        -fabs(centerRefl|rightDir) + fabs(m_burrPlane[1].center|rightDir) > m_radius*0.5)
    {
        _computeAverageBurrPlane(normRefl, centerRefl, skullGrids, skullGridsHdls, m_meanCurvP, m_burrPlane[1]);
        return true;
    }
    return false;
}

bool BurrPlaneFinder::_computeCenterLine(const VertVector& skullGrids, VertVector& centerLinePts, const OM_Vector3& offset, Real minDist)
{
    const uint xAxis = 0, yAxis = 1, zAxis = 2 ;

    OM_Vector3 rightDir  (m_triMesh->_skullBox.Axis[xAxis][0], m_triMesh->_skullBox.Axis[xAxis][1], m_triMesh->_skullBox.Axis[xAxis][2]);
    OM_Vector3 upDir     (m_triMesh->_skullBox.Axis[yAxis][0], m_triMesh->_skullBox.Axis[yAxis][1], m_triMesh->_skullBox.Axis[yAxis][2]);
    OM_Vector3 boxC      (m_triMesh->_skullBox.Center[0], m_triMesh->_skullBox.Center[1], m_triMesh->_skullBox.Center[2]);

    size_t cnt = 0;
    size_t nPts = skullGrids.size();

    VertVector::const_iterator vItB(skullGrids.begin());
    VertVector::const_iterator vItE(skullGrids.end());    
    VertVector::const_iterator iterI;

    OM_Vector3 startPoint(0.0, 0.0, 0.0);
    for(iterI = vItB; iterI != vItE; ++iterI)
    {
        OM_Vector3 pt = *iterI;
        OM_Vector3 dir = pt - offset;

        if(fabs(dir|rightDir) < m_delta*0.5)
        {
            startPoint +=  pt;
            cnt ++;
        }
    }
    if(cnt == 0)
    {
        AntlersDebugger::logMessage("ERROR: BurrPlaneFinder: Could not find start point");
        return false;
    }
    else
        startPoint /= (double)cnt;

    Real step = m_radius*0.25;
    OM_Vector3 nextDir(boxC - startPoint); nextDir.normalize();

    centerLinePts.clear();
    centerLinePts.push_back(startPoint);

    Real radius_2 = m_radius*m_radius;

    int cntIter = 0;
    while(fabs((startPoint - boxC) DOT rightDir) > minDist)
    {
        //TODO: if this really needed ?
        if(centerLinePts.size() > 3)
        {
            // find next dir - i.e. normal in current dir
            VertVector splinedCenterLine;
            NaturalCubicSpline ncSp (centerLinePts);
            ncSp.GenCurve(centerLinePts.size()*3, splinedCenterLine);

            size_t nNewPt = splinedCenterLine.size();
            if(nNewPt > 2)
                nextDir = splinedCenterLine[nNewPt-1] - splinedCenterLine[nNewPt-2] ;
        }

        nextDir.normalize();

        int cnt = 0;
        OM_Vector3 newStart (0.0, 0.0, 0.0);

        for(size_t i = 0 ; i < nPts; ++i)
        {
            OM_Vector3 offset = startPoint + nextDir*step;
            if((offset - skullGrids[i]).sqrnorm() < radius_2)   //IsInsideSphere(offset, m_radius, skullGrids[i]))
            {
                newStart += skullGrids[i];
                cnt ++;
            }
        }

        if(cnt !=  0)
            newStart /= cnt;

        // ?????
        Real rV = (newStart - boxC) DOT rightDir;
        Real uV = (newStart - boxC) DOT upDir;
        if(cnt == 0 || (fabs(rV) < m_triMesh->_maxWidth/8.0 &&//fabs(m_minRdSk)*0.8 &&
            fabs(uV) < m_triMesh->_skullBox.Extent[yAxis]*1.1))
        {
            nextDir = boxC - upDir*m_triMesh->_skullBox.Extent[yAxis] - startPoint;
            nextDir.normalize();
            newStart = startPoint + nextDir*step;
        }

        if(cnt < 20)
        {
            AntlersDebugger::logMessage("stopping centerline computation: cnt < 20");
            break;
        }

        cntIter++;
        if(cntIter > 50) //  if we are in a dead loop
        {
            AntlersDebugger::logMessage("stopping centerline computation: cntIter > 50");
            break;
        }

      //  newStart /= cnt;

        startPoint = newStart;
        centerLinePts.push_back(startPoint);
    }
    AntlersDebugger::logMessage("centerline computation completed");
    return true;
}

extern void ComputeCurvature(MeshType* mesh, const HandlesVector& hvec, OM_Property& gausCurvP, OM_Property&meanCurvP);

bool BurrPlaneFinder::_detBurPlane(VertVector& centerLinePts, const VertVector& skullGrids, const HandlesVector& skullGridsHdls, BurrPlane& burrPlane)
{
    if(centerLinePts.size() < 2) // 3 ?
        return false;

    //TODO: if this really needed ?
    //spline centerline to have homogeneous distribution and optimal amount of points

    size_t nPt = centerLinePts.size();
    NaturalCubicSpline sp(centerLinePts);

    centerLinePts.clear();
    sp.GenCurve(nPt*3, centerLinePts);
    nPt = centerLinePts.size();

    //Compute mean and gauss curvature for each point
//    OM_Property gausCurvP, meanCurvP;
    ComputeCurvature(&m_triMesh->_mesh, skullGridsHdls, m_gaussCurvP, m_meanCurvP);

    // Cut mesh with planes perpendicular to center line.
    std::vector<Real> curvatures;
    curvatures.resize(nPt-1, 0.0);

    for(size_t i = 0; i < nPt-1; ++i)
    {
        OM_Vector3 normal = centerLinePts[i+1] - centerLinePts[i];
        OM_Vector3 offset = centerLinePts[i]; //(centerLinePts[i+1] + centerLinePts[i])*0.5;
        normal.normalize();

        _computeAverageCutCurvatures(normal, offset, skullGrids, skullGridsHdls, m_meanCurvP, curvatures[i]);
    }

    // Analyze curvatures
    // Find first from the end curvature peak
    size_t iMax = -1;
    double maxCurv = -FLT_MAX;
    for(int i = curvatures.size()-1 ; i >= 0; --i)
    {
        if(maxCurv < curvatures[i]){
            iMax = i;
            maxCurv = curvatures[i];
        }
    }
    if(iMax == -1)
        return false;
#if 0
    AntlersDebugger::logMessage("maximal curvature is in the %d layer out of %d", iMax, curvatures.size());
    AntlersDebugger::logMessage("%lg %lg %lg", centerLinePts[iMax][0], centerLinePts[iMax][1], centerLinePts[iMax][2]);
#endif
    OM_Vector3 offset = (centerLinePts[iMax]);

    bool bLast = (iMax == centerLinePts.size()-1);
    if(bLast)
        iMax--;

    OM_Vector3 normal = centerLinePts[iMax+1] - centerLinePts[iMax];
    normal.normalize();

    return _computeAverageBurrPlane(normal, offset, skullGrids, skullGridsHdls, m_meanCurvP, burrPlane);
}

void BurrPlaneFinder::_computeAverageCutCurvatures(const OM_Vector3& normal, const OM_Vector3& origin, const VertVector& skullGrids, const HandlesVector& skullGridsHdls, const OM_Property& curvProp, Real& curvature, bool bCut)
{
    Real minCurv = FLT_MAX, maxCurv = -FLT_MAX, avCurv = 0.0;

    size_t cnt = 0;
    size_t nPts = skullGrids.size();

    for(size_t i = 0 ; i < nPts; ++i)
    {
        OM_Vector3 dir = skullGrids[i] - origin;
        Real dot = dir DOT normal;
        if(fabs(dot) < m_delta)
        {
            Real curv = m_triMesh->_mesh.property(curvProp, skullGridsHdls[i]);
            Real rad = (dir - normal*dot).length();
            if(bCut)
            if(rad > m_delta*4) //m_radius
                continue;

            minCurv = min(minCurv, curv);
            maxCurv = max(maxCurv, curv);
            avCurv += curv;
            cnt ++;
        }
    }
    if(cnt > 30) //&& minCurv != 0 && (fabs(maxCurv)-fabs(minCurv))/fabs(minCurv) < 2.0)
        curvature = avCurv/cnt;
    else
        curvature = 0;
}

bool BurrPlaneFinder::_computeAverageBurrPlane(const OM_Vector3& normal, const OM_Vector3& origin, const VertVector& skullGrids, const HandlesVector& skullGridsHdls, const OM_Property& curvProp, BurrPlane& burrPlane, bool bCut)
{
    typedef std::map<Real, OM_Vector3> CurvaturePointsMap;
    CurvaturePointsMap map;

    size_t nPts = skullGrids.size();
    for(size_t i = 0 ; i < nPts; ++i)
    {
        OM_Vector3 dir = skullGrids[i] - origin;
        
        Real dot = dir DOT normal;
        if(fabs(dot) < m_delta)
        {
            Real rad = (dir - normal*dot).length();
            if(bCut)
            if(rad > m_delta*4) // m_radius
                continue;

            Real curv = m_triMesh->_mesh.property(curvProp, skullGridsHdls[i]);
            map[fabs(curv)] = skullGrids[i];
        }
    }
    
    size_t nMax = min(map.size(),map.size()/10);
    if(nMax < 3)
    {
        AntlersDebugger::logMessage("stopping burr plane computation. nMax < 3 %d", nMax);
        return false;
    }

    //Perform least squares approximation for first nPts/10 points with maximal mean curvature
    std::vector< Wm5::Vector3<double> > pts;
    
    CurvaturePointsMap::iterator iter = map.end(); iter--;

    for(size_t i = 0 ; i < nMax; ++i, --iter){
        pts.push_back(Wm5::Vector3<double>(iter->second[0], iter->second[1], iter->second[2]));
    }
    
    Wm5::Plane3<double> wm_plane = Wm5::OrthogonalPlaneFit3(pts.size(), &pts[0]);

    burrPlane.normal = OM_Vector3(wm_plane.Normal[0], wm_plane.Normal[1],wm_plane.Normal[2]);
    burrPlane.normal.normalize();

    OM_Vector3 planeOff = burrPlane.normal*wm_plane.Constant;

    OM_Vector3 dir = origin - planeOff;
    Real dot = dir DOT burrPlane.normal;

    burrPlane.center = planeOff + (dir - burrPlane.normal*dot);
    
    // find burr ring's radius
    Real maxRad = -FLT_MAX;
    for(size_t i = 0 ; i < nPts; ++i)
    {
        OM_Vector3 dir = skullGrids[i] - burrPlane.center;

        Real dot = dir DOT burrPlane.normal;
        if(fabs(dot) < m_delta)
        {
            Real rad = (dir - burrPlane.normal*dot).length();
            if(rad > m_delta*4) // m_radius
                continue;

            maxRad = max(maxRad, rad);
        }
    }
    
    burrPlane.radius = maxRad;

    return true;
}

const BurrPlane* BurrPlaneFinder::GetBurrPlanes() const
{
    return &m_burrPlane[0];
}
void BurrPlaneFinder::GetLeftBurrPlane(BurrPlane& burrPlane) const
{
    burrPlane = m_burrPlane[0];
}
void BurrPlaneFinder::GetRightBurrPlane(BurrPlane& burrPlane) const
{
    burrPlane = m_burrPlane[1];
}