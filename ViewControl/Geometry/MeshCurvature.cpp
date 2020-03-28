#include "stdafx.h"
#include "..\Kernel\Vector4.h"
#include "..\Kernel\Matrix4.h"
#include "..\Kernel\Kernel.h"
#include "..\Kernel\TriMeshes.h"

#include "..\PRST_3D.h"

typedef OpenMesh::VPropHandleT<float> OM_Property;

typedef std::vector<OpenMesh::VertexHandle> HandlesVector;

inline Vector3 HandleToVector3(MeshType* mesh, MeshType::VertexHandle vertex)
{
    OpenMesh::Vec3f v = mesh->point(vertex);
    return Vector3(v[0], v[1], v[2]);
}

#define DOT |
#define CROSS %

#if 0
    int savePTZ(const wchar_t* filename, unsigned int rows, unsigned int cols, MeshType* mesh, OM_Property& curvP);
    int savePTZ(const wchar_t* filename, unsigned int rows, unsigned int cols, MeshType* mesh, const HandlesVector& hvec, OM_Property& curvP);
#endif

void ComputeCurvature(MeshType* mesh, const HandlesVector& hvec, OM_Property& gausCurvP, OM_Property&meanCurvP)
{
    // Add curvature properties to vertexes before any calculations.
 //   mesh->add_property(gausCurvP);
 //   mesh->add_property(meanCurvP);

    size_t vertCount = hvec.size();

    MeshType::VertexVertexIter vv_it;  
    HandlesVector::const_iterator v_it = hvec.begin();
    HandlesVector::const_iterator vEnd = hvec.end();
    float maxDst = 0;
    size_t cnt = 0;
    for (v_it; v_it != vEnd; ++v_it)
    {
        // Set zero as mean and Gaussian curvature for all vertices.

        mesh->property(gausCurvP,*v_it) = 0.0;
        mesh->property(meanCurvP,*v_it) = 0.0;

        // TODO: is this really needed?
        if(mesh->is_boundary(*v_it)){
            continue;
        }

        Vector p0 = mesh->point(*v_it);

        float A = 0.0;
        float alphas = 0.0;
        float betaIs = 0.0;

        MeshType::VertexFaceIter vf_it = mesh->vf_iter(*v_it);

        for (vv_it = mesh->vv_iter(*v_it); vv_it; ++vv_it, ++vf_it)
        {
            MeshType::VertexVertexIter next_vit = vv_it;
            ++next_vit;
            if(!next_vit)
                next_vit = mesh->vv_iter(*v_it);

            MeshType::VertexFaceIter next_fit = vf_it;
            ++next_fit;
            if(!next_fit)
                next_fit = mesh->vf_iter(*v_it);

            Vector p1 = mesh->point(vv_it);
            Vector p2 = mesh->point(next_vit);

            Vector e1 = p1-p0;
            Vector e2 = p2-p0;

            Vector crossT = e1 CROSS e2 ;
            float square = crossT.length();

            if(square == 0)
                continue;

            float alpha = square / (e1.length()*e2.length());

            A += square;
            alphas += alpha;

            if(((MeshType::FaceHandle)vf_it).idx() == -1 || ((MeshType::FaceHandle)next_fit).idx() == -1)
                continue;

            Vector n1 = mesh->normal(vf_it);
            Vector n2 = mesh->normal(next_fit);

            betaIs += e1.length()* Math::ACos(n1 DOT n2);
        }

        if(A != 0)
        {
            float gauss = 3*(Math::TWO_PI - alphas)/A;
            float mean = 0.75*betaIs/A;

            mesh->property(gausCurvP,*v_it) = gauss;
            mesh->property(meanCurvP,*v_it) = mean;

            if(cnt % 6 == 0)
            {
                float ro = p0.length();
            }
            cnt ++;
        }
    }
#if 0
    unsigned int rows = vertCount;
    unsigned int columns = 1;
    savePTZ(L"meanCurvature.ptz", rows, columns, mesh, hvec, meanCurvP);
    savePTZ(L"gaussCurvature.ptz", rows, columns, mesh, hvec, gausCurvP);
#endif
}

void ComputeCurvature(MeshType* mesh, OM_Property& gausCurvP, OM_Property&meanCurvP)
{
    // Add curvature properties to vertexes before any calculations.
    mesh->add_property(gausCurvP);
    mesh->add_property(meanCurvP);

    size_t vertCount = mesh->n_vertices();

    MeshType::VertexVertexIter vv_it;  
    MeshType::VertexIter v_it = mesh->vertices_begin();
    MeshType::VertexIter vEnd = mesh->vertices_end();
    float maxDst = 0;
    size_t cnt = 0;
    for (v_it; v_it != vEnd; ++v_it)
    {
        // Set zero as mean and Gaussian curvature for all vertices.
        
        mesh->property(gausCurvP,v_it) = 0.0;
        mesh->property(meanCurvP,v_it) = 0.0;

        Vector p0 = mesh->point(v_it);

        float A = 0.0;
        float alphas = 0.0;
        float betaIs = 0.0;

        MeshType::VertexFaceIter vf_it = mesh->vf_iter(v_it);

        for (vv_it = mesh->vv_iter(v_it); vv_it; ++vv_it, ++vf_it)
        {
            MeshType::VertexVertexIter next_vit = vv_it;
            ++next_vit;
            if(!next_vit)
                next_vit = mesh->vv_iter(v_it);

            MeshType::VertexFaceIter next_fit = vf_it;
            ++next_fit;
            if(!next_fit)
                next_fit = mesh->vf_iter(v_it);

            Vector p1 = mesh->point(vv_it);
            Vector p2 = mesh->point(next_vit);

            Vector e1 = p1-p0;
            Vector e2 = p2-p0;

            Vector crossT = e1 CROSS e2 ;
            float square = crossT.length();
            
            if(square == 0)
                continue;

            float alpha = square / (e1.length()*e2.length());
            
            A += square;
            alphas += alpha;

            Vector n1 = mesh->normal(vf_it);
            Vector n2 = mesh->normal(next_fit);

            betaIs += e1.length()* Math::ACos(n1 DOT n2);
        }

        if(A != 0)
        {
            float gauss = 3*(Math::TWO_PI - alphas)/A;
            float mean = 0.75*betaIs/A;

            mesh->property(gausCurvP,v_it) = gauss;
            mesh->property(meanCurvP,v_it) = mean;

            if(cnt % 6 == 0)
            {
                float ro = p0.length();
            }
            cnt ++;
        }
    }
#if 0
    unsigned int rows = vertCount;
    unsigned int columns = 1;
    savePTZ(L"meanCurvature.ptz", rows, columns, mesh, meanCurvP);
    savePTZ(L"gaussCurvature.ptz", rows, columns, mesh, gausCurvP);
#endif
}

#if 0

#include "ptz.h"
int savePTZ(const wchar_t* filename, unsigned int rows, unsigned int cols, MeshType* mesh, OM_Property& curvP)
{
    if(!mesh) return 0;

    MeshType::VertexVertexIter vv_it;  
    MeshType::VertexIter v_it = mesh->vertices_begin();
    MeshType::VertexIter vEnd = mesh->vertices_end();

    int width = cols;
    int height = rows;
    double* pnts = new double[width*height*4];
    ZeroMemory(pnts, sizeof(double)*width*height*4);
    int cnt = 0;
    unsigned int pt = 0;
    for (v_it; v_it != vEnd; ++v_it)
    {
    
            pnts[cnt+0] = mesh->point(v_it)[0];
            pnts[cnt+1] = mesh->point(v_it)[1];
            pnts[cnt+2] = mesh->point(v_it)[2];
            pnts[cnt+3] = mesh->property(curvP,v_it);
            cnt += 4;
    }
  
    FILE* f ;
    _wfopen_s(&f, filename, L"wb");
    if(!f)
    {
        // Can't open the file!
        return 0;
    }

    if(ptz_fileSave(f, pnts, width, height))
    {
        delete[] pnts;
        return false;
    }
    fclose(f);

    delete[] pnts;
    return true;
}

int savePTZ(const wchar_t* filename, unsigned int rows, unsigned int cols, MeshType* mesh, const HandlesVector& hvec, OM_Property& curvP)
{
    if(!mesh) return 0;

    MeshType::VertexVertexIter vv_it;  
    HandlesVector::const_iterator v_it = hvec.begin();
    HandlesVector::const_iterator vEnd = hvec.end();

    int width = cols;
    int height = rows;
    double* pnts = new double[width*height*4];
    ZeroMemory(pnts, sizeof(double)*width*height*4);
    int cnt = 0;
    unsigned int pt = 0;
    for (v_it; v_it != vEnd; ++v_it)
    {

        pnts[cnt+0] = mesh->point(*v_it)[0];
        pnts[cnt+1] = mesh->point(*v_it)[1];
        pnts[cnt+2] = mesh->point(*v_it)[2];
        pnts[cnt+3] = mesh->property(curvP,*v_it);
        cnt += 4;
    }

    FILE* f ;
    _wfopen_s(&f, filename, L"wb");
    if(!f)
    {
        // Can't open the file!
        return 0;
    }

    if(ptz_fileSave(f, pnts, width, height))
    {
        delete[] pnts;
        return false;
    }
    fclose(f);

    delete[] pnts;
    return true;
}
#endif