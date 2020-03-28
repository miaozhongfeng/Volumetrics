
#pragma once

#include "Kernel\Kernel.h"
#include "Kernel\Trimeshes.h"

struct BurrPlane
{
    Real radius;
    OM_Vector3 normal;
    OM_Vector3 center;
};

/*
    BurrPlaneFinder automatically determines burr ring planes.

    After creating BurrPlaneFinder object (a valid pointer to mesh should be passed
    into constructor), method FindBurrPlanes should be called to initiate burr planes
    finding algorithm. When it is finished it's possible to grab the results
    using Get* methods.

    WORKFLOW: First observation are is determined by computing skull bounds and
    extending little bit to have skull and beginning of antlers be included in observation area.
    Then centerline is computed for points in the observation area.
    Assuming that direction of centerline in each point is normal of the "normal cut" plane,
    cut mesh on rings and find average mean curvature of points in the cut
    Final burr plane determination is made based on least squares approximation of first N points 
    with maximal mean curvature in a ring with a maximal average mean curvature
*/
class BurrPlaneFinder
{
public:
    // Constructors/destructor
    BurrPlaneFinder(CTriMesh* m_triMesh);
    ~BurrPlaneFinder();
    
    // Performs burr plane finding. 
    // \return false in case of error, true - otherwise.
    bool FindBurrPlanes();
    
    const BurrPlane* GetBurrPlanes() const;
    void GetLeftBurrPlane(BurrPlane& burrPlane) const;
    void GetRightBurrPlane(BurrPlane& burrPlane) const;

protected:
    bool _computeCenterLine(const VertVector& skullGrids, VertVector& centerLinePts, const OM_Vector3& offset, Real minDist);
    bool _detBurPlane(VertVector& centerLinePts, const VertVector& skullGrids, const HandlesVector& skullGridsHdls, BurrPlane& burrPlane);
    void _computeAverageCutCurvatures(const OM_Vector3& normal, const OM_Vector3& origin, const VertVector& skullGrids, const HandlesVector& skullGridsHdls, const OM_Property& curvProp, Real& curvature, bool bCut = true);
    bool _computeAverageBurrPlane(const OM_Vector3& normal, const OM_Vector3& origin, const VertVector& skullGrids, const HandlesVector& skullGridsHdls, const OM_Property& curvProp, BurrPlane& burrPlane, bool bCut = true);

    bool _testLeftRight(const VertVector& skullGrids, const HandlesVector& skullGridsHdls);
    bool _testRightLeft(const VertVector& skullGrids, const HandlesVector& skullGridsHdls);
private:
    CTriMesh* m_triMesh;    // geometry to operate with

    Real m_delta;          // the epsilon value for discretization 
    Real m_radius;         // the approximate radius of burr rings

    Real m_minRd, m_maxRd;     // observation area's bonds 
    Real m_minRdSk, m_maxRdSk; // skull's bounds

    BurrPlane m_burrPlane[2];   // burr planes [0] - for Left, [1] - for Right

    OM_Property m_gaussCurvP; // OpenMesh property for gauss curvature value in each point
    OM_Property m_meanCurvP;  // OpenMesh property for mean curvature value in each point
};
