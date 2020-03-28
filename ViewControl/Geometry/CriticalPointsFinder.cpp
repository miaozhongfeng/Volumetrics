
#include <vector>
#include <list>
#include <assert.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <map>

#include "..\Debug\Debugger.h"
#include "..\Kernel\trimeshes.h"
#include "..\Kernel\Vector3.h"
#include "..\Kernel\Math.h"

#include "CriticalPointsFinder.h"

using namespace std; 

CriticalPointsFinder::CriticalPointsFinder(MeshType* mesh):
m_mesh(mesh)
{
    assert(m_mesh);
}

CriticalPointsFinder::~CriticalPointsFinder()
{
}

bool CriticalPointsFinder::CheckVertsNear(MeshType::VertexHandle v1, MeshType::VertexHandle v2, int radius)
{
    if (v1 == v2){
        return true;
    }

    MeshType::VertexVertexIter vv_it;  
    MeshType::VertexHandle vh;
    for (vv_it = m_mesh->vv_iter(v1); vv_it; ++vv_it){
        vh = vv_it;
        
        if (v2 == vh){
            return true;
        }

        // Recursion.
        if (radius >= 1){
            if (CheckVertsNear(vh, v2, radius - 1)){
                return true;
            }
        }
    }

    return false;
}

bool CriticalPointsFinder::FindCriticalPoints(bool bUseTwoRoots)
{
    // Add geodesic distance property to vertexes before any calculations.
    m_mesh->add_property(m_geoDistProp);

    if (!m_mesh){
        return false;
    }

    AntlersDebugger::logMessage("Finding critical points...");

    // First step: find root vertexes.
    FindRoots();

    vector<MeshType::VertexHandle> extremums1;
    vector<MeshType::VertexHandle> saddles1;
    vector<float> geoDst1;

    vector<MeshType::VertexHandle> extremums2;
    vector<MeshType::VertexHandle> saddles2;
    vector<float> geoDst2;

    // Second step: create geodesic trees for both roots. Get extremums and
    // saddles for these two cases.
    // TODO: possible optimization - use tree, which was built in FindRoots

    BuildGeoTree(m_roots[0], true, false);
    AnalyzeGeoTree(extremums1, saddles1);

    for (unsigned i = 0; i < extremums1.size(); ++i){
        geoDst1.push_back(m_mesh->property(m_geoDistProp, extremums1[i]));
    }

    if(bUseTwoRoots)
    {
        BuildGeoTree(m_roots[1], true, false);
        AnalyzeGeoTree(extremums2, saddles2);
    }
    
    for (unsigned i = 0; i < extremums2.size(); ++i){
        geoDst2.push_back(m_mesh->property(m_geoDistProp, extremums2[i]));
    }

    // The third step: find the intersection of two sets of extremums.
    vector<MeshType::VertexHandle> intersecExtremums;
    for (unsigned i = 0; i < extremums1.size(); ++i){

        // TODO: is this really needed?
        if(m_mesh->is_boundary(extremums1[i])){
            continue;
        }

        // TODO: is this really needed?
        if (CountFacesVertexBelongsTo(extremums1[i]) < 3){
            continue;
        }

        if(!bUseTwoRoots)
            intersecExtremums.push_back(extremums1[i]);
        else
        {
            for (unsigned j = 0; j < extremums2.size(); ++j){
                if (extremums1[i] == extremums2[j]){
                    intersecExtremums.push_back(extremums1[i]);
                    break;
                }

                float dst = m_mesh->property(m_geoDistProp, extremums1[i]);
                float distEuc = (HandleToVector3(extremums1[i])-HandleToVector3(extremums2[j])).Length();
                if (/*fabsf(geoDst2[j] - dst) < 0.005f &&*/ CheckVertsNear(extremums1[i], extremums2[j], 3)){
                    intersecExtremums.push_back(extremums1[i]);
                    break;
                }
            }
        }
    }

    //m_extremums = extremums2;
    //m_saddles = extremums1;

    m_saddles = saddles1;
    m_extremums = intersecExtremums;

    // Root points are extremums too.
    m_extremums.push_back(m_roots[0]);
    m_extremums.push_back(m_roots[1]);
        
    // Remove geodesic distance property. We do not need it now.
    m_mesh->remove_property(m_geoDistProp);

    return true;
}

void CriticalPointsFinder::FindRoots()
{
    m_roots.clear();

    // Pick vertex x at random from the pool of mesh vertices.
    unsigned vertCount = m_mesh->n_vertices();
    unsigned rootId = Math::Random(vertCount, -1);
    MeshType::VertexIter v_it = m_mesh->vertices_begin();
    for (unsigned i = 0; i < rootId; i++){
        ++v_it;
    }
    MeshType::VertexHandle x = v_it;
    
    // Build shortest-path tree root at x and find y such that
    // disx(y) is maximal.
    BuildGeoTree(x, false, false);
    float maxDst = 0;
    MeshType::VertexHandle y;
    MeshType::VertexIter vEnd = m_mesh->vertices_end();
    for (v_it = m_mesh->vertices_begin(); v_it != vEnd; ++v_it){
        float dst = m_mesh->property(m_geoDistProp,v_it);
        if(dst > maxDst){
            maxDst = dst;
            y = v_it;
        }
    }

    // Build shortest-path tree root at y and find z such that
    // disy(z) is maximal.
    BuildGeoTree(y, false, false);
    MeshType::VertexHandle z;
    maxDst = 0;
    for (v_it = m_mesh->vertices_begin(); v_it != vEnd; ++v_it){
        float dst = m_mesh->property(m_geoDistProp,v_it);
        if(dst > maxDst){
            maxDst = dst;
            z = v_it;
        }
    }

    // y and z will be suitable root vertexes.
    m_roots.push_back(z);
    m_roots.push_back(y);
}

void CriticalPointsFinder::GetRootPoints(vector<Vector3>& roots)
{
    roots.clear();
    for (unsigned rootNum = 0; rootNum < m_roots.size(); ++rootNum){
        roots.push_back(HandleToVector3(m_roots[rootNum]));
    }
}

// Sorting predicate for creating geodesic tree.
struct VertSortPred{
    bool operator() (MeshType::VertexHandle v1, MeshType::VertexHandle v2) const {
        return  (s_mesh->property(s_geoDistProp, v1) < s_mesh->property(s_geoDistProp, v2));
    }

    static MeshType* s_mesh;
    static OpenMesh::VPropHandleT<float> s_geoDistProp;
};
MeshType* VertSortPred::s_mesh = 0;
OpenMesh::VPropHandleT<float> VertSortPred::s_geoDistProp;

void CriticalPointsFinder::BuildGeoTree(MeshType::VertexHandle root, bool normalize, bool colectLines)
{
    // Local types for building geodesic tree visual presentation.
    typedef map<MeshType::VertexHandle, GeoTreeLine> GeoTreeMap;
    typedef pair<MeshType::VertexHandle, GeoTreeLine> GeoTreePair;
    GeoTreeMap geoTreeMap;

    // Initialize sorting predicate.
    VertSortPred::s_geoDistProp = m_geoDistProp;
    VertSortPred::s_mesh = m_mesh;

    // Create mesh iterators.
    MeshType::VertexIter v_it = m_mesh->vertices_begin();
    MeshType::VertexVertexIter vv_it;  
    MeshType::VertexIter vEnd = m_mesh->vertices_end();

    // Set infinity as geodesic distance for all vertices.
    for (; v_it != vEnd; ++v_it){
        m_mesh->property(m_geoDistProp,v_it) = Math::POS_INFINITY;
    }

    // Create vertex queue.
    typedef vector<MeshType::VertexHandle> VertsQueue;
    VertsQueue vertsQueue;
    
    // Initialize the queue with the first vertex - root vertex. Set its geodesic distance to 0.
    vertsQueue.reserve(1000); 
    m_mesh->property(m_geoDistProp, root) = 0;
    vertsQueue.push_back(root);

    // Process while the queue is not empty.
    while (vertsQueue.size()){
        // Dequeue the vertex v with smallest dis(v) from Q. This is the first vertex always.
        MeshType::VertexHandle frontVertHandle = vertsQueue.front();
        vertsQueue.erase(vertsQueue.begin()); // remove the vertex
        OpenMesh::Vec3f v = m_mesh->point(frontVertHandle);
        float dis_v = m_mesh->property(m_geoDistProp, frontVertHandle);

        // For each vertex vi adjacent to v, if dis(v) + length(v, vi) < dis(vi),
        // update dis(vi) = dis(v) + length(v, vi) and insert (or update) vi to Q.
        for (vv_it = m_mesh->vv_iter(frontVertHandle); vv_it; ++vv_it){
            OpenMesh::Vec3f vi = m_mesh->point(vv_it);
            float dis_vi = m_mesh->property(m_geoDistProp, vv_it);
            float len = Math::Sqrt( (v[0]-vi[0])*(v[0]-vi[0]) + 
                                    (v[1]-vi[1])*(v[1]-vi[1]) +
                                    (v[2]-vi[2])*(v[2]-vi[2]));

            float new_dis_v = dis_v + len;
            if(new_dis_v < dis_vi){
                // Set new dist.
                m_mesh->property(m_geoDistProp, vv_it) = new_dis_v;
                
                // Look if the vertex is in the queue already.
                MeshType::VertexHandle handle = vv_it;
                VertsQueue::iterator foundIt = find(vertsQueue.begin(), vertsQueue.end(), handle);
                
                // Line for geodesic tree visual presentation.
                GeoTreeLine line = {Vector3(v[0], v[1], v[2]), Vector3(vi[0], vi[1], vi[2])};
                
                // Add the vertex to the queue if it is not here yet.
                if(foundIt == vertsQueue.end()){
                    vertsQueue.push_back(handle);   
                    if(colectLines){
                        geoTreeMap.insert(GeoTreePair(handle, line));
                    }
                } 
                else
                {
                    if(colectLines){
                        GeoTreeMap::iterator it = geoTreeMap.find(handle);
                        if(it != geoTreeMap.end()){
                            it->second = line;
                        }
                    }
                }
            } // if (new_dis_v < dis_vi)
        } // for each vertex vi adjacent to v
        
        // Sort the vertexes in the queue by theirs geodesic distance.
        sort(vertsQueue.begin(), vertsQueue.end(), VertSortPred());
    } // while (vertsQueue.size())

    // Normalize geodesic distance for all vertexes.
    if(normalize){
        float maxGeoDst = GetMaxGeoDist();
        for (v_it = m_mesh->vertices_begin(); v_it != vEnd; ++v_it){
            if (m_mesh->property(m_geoDistProp, v_it) != Math::POS_INFINITY){
                m_mesh->property(m_geoDistProp, v_it) /= maxGeoDst;
            }
        }
    }

    // Save geodesic tree lines if requested.
    if (colectLines){
        m_geoTreeLines.clear();
        for (GeoTreeMap::iterator it = geoTreeMap.begin(); it != geoTreeMap.end(); ++it){
            m_geoTreeLines.push_back(it->second);
        }
    }
}

float CriticalPointsFinder::GetMaxGeoDist()
{
    MeshType::VertexIter v_it;
    MeshType::VertexIter vEnd = m_mesh->vertices_end();
    float maxDst = 0;
    for (v_it = m_mesh->vertices_begin(); v_it != vEnd; ++v_it){
        float dst = m_mesh->property(m_geoDistProp, v_it);
        if(dst > maxDst){
            maxDst = dst;
        }
    }

    return maxDst;
}

void CriticalPointsFinder::AnalyzeGeoTree(vector<MeshType::VertexHandle>& extremums,
                                          vector<MeshType::VertexHandle>& saddles)
{
    extremums.clear();
    saddles.clear();

    MeshType::VertexIter v_it;
    MeshType::VertexVertexIter vv_it;  
    MeshType::VertexIter vEnd = m_mesh->vertices_end();

    vector<float> distDiffs;

    int invalidPointsCount = 0;
    for (v_it = m_mesh->vertices_begin(); v_it != vEnd; ++v_it){
        float dis_v = m_mesh->property(m_geoDistProp, v_it);
        // If geodesic distance == infinity this means that the distance was never calculated 
        // for the point (the point was not visited on the previous step). 
        if (dis_v == Math::POS_INFINITY){
            invalidPointsCount++;
            continue;
        }

        // Create (v - vi) sequence.
        distDiffs.clear();
        distDiffs.reserve(10);
        
        for (vv_it = m_mesh->vv_iter(v_it); vv_it; ++vv_it){
            float dis_vi = m_mesh->property(m_geoDistProp, vv_it);
            float dis_diff = dis_vi - dis_v;
            if (dis_diff != 0){
                distDiffs.push_back(dis_diff);
            }
        }

        if(distDiffs.size() == 0){
            continue;
        }

        // Calculate sign changes count.
        int sgc = 0;
        bool lastSign = (distDiffs[0] > 0);

        for (unsigned i = 1; i < distDiffs.size(); ++i){
            bool sign = (distDiffs[i] > 0);
            if (sign != lastSign){
                sgc++;
                lastSign = sign;
            }
        }

        // Decide on the point type.
        if (sgc == 0 && distDiffs[0] < 0 && ExtremumAdditionalCondition(v_it)){
            OpenMesh::Vec3f v = m_mesh->point(v_it);
            extremums.push_back(v_it);
        }
        else if ((sgc % 2 == 0) && (sgc >= 4)){
            OpenMesh::Vec3f v = m_mesh->point(v_it);
            saddles.push_back(v_it);
        }
    } // for (v_it = m_mesh->vertices_begin(); v_it != vEnd; ++v_it)

    if (invalidPointsCount > 0){
        AntlersDebugger::logMessage("Warning: some vertexes (%d) were not reached during the geodesic tree building.", 
                                    invalidPointsCount);
    }
}

bool CriticalPointsFinder::ExtremumAdditionalCondition(MeshType::VertexHandle v)
{
    MeshType::VertexVertexIter vv_it;  
    MeshType::VertexHandle vh;

    vector<MeshType::VertexHandle> radiusOneVerts;
    radiusOneVerts.push_back(v);    
    for (vv_it = m_mesh->vv_iter(v); vv_it; ++vv_it){
        radiusOneVerts.push_back(vv_it);
    }

    int failurCount = 0;
    int pointsAroundTotalCount = 0;
    for (vv_it = m_mesh->vv_iter(v); vv_it; ++vv_it){
        float dis_vvi = m_mesh->property(m_geoDistProp, vv_it);
        MeshType::VertexVertexIter vvv_it;  
        for (vvv_it = m_mesh->vv_iter(vv_it); vvv_it; ++vvv_it){
            vh = vvv_it;
            if (find(radiusOneVerts.begin(), radiusOneVerts.end(), vh) == radiusOneVerts.end()){
                pointsAroundTotalCount++;
                float dis_vvvi = m_mesh->property(m_geoDistProp, vvv_it);
                if (dis_vvvi > dis_vvi){
                    failurCount++;
                }
            }
        }
    }

    // The number of "failures" should be low (no more than 15% is set now).
    return ((((float)pointsAroundTotalCount * 0.15f) - failurCount) > 0);
}

void CriticalPointsFinder::GetExtremumPoints(vector<Vector3>& points)
{
    points.clear();
    for (unsigned extremumNum = 0; extremumNum < m_extremums.size(); ++extremumNum){
        points.push_back(HandleToVector3(m_extremums[extremumNum]));
    }
}

void CriticalPointsFinder::GetSaddlePoints(vector<Vector3>& points)
{
    points.clear();
    for (unsigned saddleNum = 0; saddleNum < m_saddles.size(); ++saddleNum){
        points.push_back(HandleToVector3(m_saddles[saddleNum]));
    }
}

void CriticalPointsFinder::GetGeoTreeLines(vector<GeoTreeLine>& lines)
{
    lines = m_geoTreeLines;
}

void CriticalPointsFinder::GetExtremumVertexes(vector<MeshType::VertexHandle>& verts)
{
    verts = m_extremums;
}

int CriticalPointsFinder::CountFacesVertexBelongsTo(MeshType::VertexHandle vertex)
{
    int count = 0;
    for (MeshType::VertexFaceIter vf_it = m_mesh->vf_iter(vertex); vf_it; ++vf_it){
        count++;
    }
    return count;
}

Vector3 CriticalPointsFinder::HandleToVector3(MeshType::VertexHandle vertex)
{
    OpenMesh::Vec3f v = m_mesh->point(vertex);
    return Vector3(v[0], v[1], v[2]);
}

// For debugging only.
void CriticalPointsFinder::LogDistanceProp(int maxVerts)
{
    MeshType::VertexIter v_it = m_mesh->vertices_begin();
    string str;
    int n = 0;
    for (; v_it != m_mesh->vertices_end(); ++v_it){
        float dist = m_mesh->property(m_geoDistProp,v_it);
        ostringstream os;
        os << dist;
        str += os.str();
        str += "   ";
        if (n % 8 == 0){
            AntlersDebugger::logMessage(str.c_str());
            str = "";
            if (n > maxVerts){
                return;
            }
        }
        n++;
    }
}
