
#pragma once

#include <vector>
#include "../Kernel/trimeshes.h"

class Vector3;

/*
    GeoTreeLine object is a geodesic tree line, which can be used for geodesic tree
    visualization.
*/
class GeoTreeLine
{
public:
    Vector3 m_p1;
    Vector3 m_p2;
};

/*
    CriticalPointsFinder implements critical points finding algorithm described in next papers:
    "Decomposing Polygon Meshes by Means of Critical Points" by Yinan Zhou and Zhiyong Huang
    (http://www1.i2r.a-star.edu.sg/~zyhuang/review/research/others/mmm04_shape.pdf)
    "3D Mesh Skeleton Extraction Using Topological and Geometrical Analyses" (only section 4.1)
    by Julien Tierny, Jean-Philippe Vandeborre and Mohamed Daoudi.

    After creating CriticalPointsFinder object (a valid pointer to mesh should be passed
    into constructor), method FindCriticalPoints should be called to initiate critical
    points finding algorithm. When it is finished it's possible to grab the results
    using Get* methods.

    NOTE: currently only extremum points (i.e. local maximuns) are calculated accurately.
    CriticalPointsFinder provides access to saddles (i.e. local minimums) points but one
    should not rely that saddle points are correct.

    TODO: calculate saddle points accurately, in the same way as critical points or
    do not calculate them at all. This is an open question as it's not clear whether
    saddles are needed.
*/
class CriticalPointsFinder
{
public:
    // Constructor.
    // \param mesh The pointer to valid mesh object.
    CriticalPointsFinder(MeshType* mesh);
    // Destructor.
    ~CriticalPointsFinder();

    // Performs critical points finding. 
    // \return false in case of error, true - otherwise.
    // \param bUseTwoRoots when true, critical points are found as intersection of cp, found from different root points 
    bool FindCriticalPoints(bool bUseTwoRoots = true);
    
    // Returns an array of root vertexes.
    // \param roots a vector to save results.
    void GetRootPoints(std::vector<Vector3>& roots);
    // Returns an array of extremum vertexes.
    // \param points a vector to save results.
    void GetExtremumPoints(std::vector<Vector3>& points);
    // Returns an array of saddle vertexes.
    // \param points a vector to save results.
    void GetSaddlePoints(std::vector<Vector3>& points);
    // Returns an array of geodesic tree line, which can be used for geodesic tree visualization.
    // For debugging purposes only.
    // \param points a vector to save results.
    void GetGeoTreeLines(std::vector<GeoTreeLine>& lines);

    // Returns an array of extremum vertex handlers.
    // \param verts a vector to save results.
    void GetExtremumVertexes(std::vector<MeshType::VertexHandle>& verts);

private:

    // Performs root finding.
    void FindRoots();

    // Build geodesic tree.
    // \param root A root vertex to start building the tree from.
    // \param normalize Defines whether to normalize (i.e. to clamp into [0,1]) geodesic
    //          distances of vertexes.
    // \param colectLines Defines whether to collect geodesic tree lines during processing.
    //          Geodesic tree lines will be stored in m_geoTreeLines vector and may be used for 
    //          geodesic tree visualization.
    void BuildGeoTree(MeshType::VertexHandle root, bool normalize, bool colectLines = false);

    // Return maximum geodesic distance of all vertexes.
    float GetMaxGeoDist();

    // Finds extremums and saddles analyzing geodesic tree. 
    // \param extremums A vector where extremums will be saved. 
    // \param saddles A vector where saddles will be saved.
    void AnalyzeGeoTree(std::vector<MeshType::VertexHandle>& extremums,
                        std::vector<MeshType::VertexHandle>& saddles);

    // Checks whether vertex v2 is situated in "radius" points neighborhood from vertex v1.
    bool CheckVertsNear(MeshType::VertexHandle v1, MeshType::VertexHandle v2, int radius = 2);

    // Return the number of triangles the vertex belongs to.
    int CountFacesVertexBelongsTo(MeshType::VertexHandle vertex);

    // Checks if the vertex satisfies additional condition to be counted as extremum point.
    // The additional condition is that most of its neighbor vertexes must also be maximums
    // among other points around.
    bool ExtremumAdditionalCondition(MeshType::VertexHandle v);

    // Coverts VertexHandle to Vector3.
    Vector3 HandleToVector3(MeshType::VertexHandle vertex);

    // Logs vertexes distance property. For debugging only.
    void LogDistanceProp(int maxVerts = 400);

private:

    MeshType* m_mesh;

    OpenMesh::VPropHandleT<float> m_geoDistProp;

    std::vector<MeshType::VertexHandle> m_roots;
    std::vector<MeshType::VertexHandle> m_extremums;
    std::vector<MeshType::VertexHandle> m_saddles;

    std::vector<GeoTreeLine> m_geoTreeLines;
};