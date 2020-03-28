
#pragma once

#include "Kernel\TriMeshes.h"
#include "Kernel\Plane.h"

#define ROUND2(x) ((double)((int)((x + 0.005)*100)))/100.0;


//--------------------------------------------------------------------------------------------------------
struct DebugPoint
{
	OpenMesh::Vec3f m_position;
	float m_radius;
	int m_type;
	bool m_criticalPoint;

	DebugPoint(OpenMesh::Vec3f vec, float radius, int type, bool critical = false): 
		m_position(vec), 
		m_type(type),
		m_radius(radius),
		m_criticalPoint(critical) {};
};

//--------------------------------------------------------------------------------------------------------
struct CapPoint
{
	int m_capIndex;				// -1 = not set
	std::vector<OpenMesh::Vec3f> m_intPoints;	// intersection points
	bool m_used;				// is used

	CapPoint():
	m_used(false),
	m_capIndex(-1) {};

	CapPoint& operator=(OpenMesh::Vec3f intP)
	{
		m_intPoints.push_back(intP);
		return *this;
	}
};

//--------------------------------------------------------------------------------------------------------
struct Cap
{
	std::vector<OpenMesh::Vec3f> m_intPoints;
	bool	m_marked;
	bool	m_hasCenter;
	float	m_distance;	// outermost distance
	OpenMesh::Vec3f m_center;

	MeshType::VertexHandle	m_vHandle;
	//std::vector<MeshType::VertexHandle>	m_handles;

	Cap():
	m_marked(false),
	m_hasCenter(false),
	m_distance(0) {/*m_intPoints.reserve(250);*/};
};

//--------------------------------------------------------------------------------------------------------
struct SphereUnit
{
	OpenMesh::Vec3f	m_origin;
	float	m_radius;
	int		m_index;
	bool	m_valid;
	bool	m_used;

	bool	m_hasConnectPoints;
	Vector3 m_connectPoint[2];

	SphereUnit(int index, OpenMesh::Vec3f vec, float radius, bool valid = true): 
		m_origin(vec), 
		m_radius(radius),
		m_valid(valid),
		m_index(index),
		m_used(false) {};

	SphereUnit(OpenMesh::Vec3f vec, float radius, int type, bool valid = false): 
		m_origin(vec), 
		m_radius(radius),
		m_valid(valid),
		m_used(false) {};

	SphereUnit(bool valid = false): 
		m_valid(valid),
		m_used(false) {};

	bool operator == (SphereUnit* sphere)
	{
		return (this == sphere);
	}
};

//--------------------------------------------------------------------------------------------------------
struct Branch
{
	std::vector<Branch>		m_childs;
	std::vector<Vector3>	m_points;
	std::vector<Vector3>	m_extPoints; // points that were removed from main list for length computation 
	std::vector<Vector3>	m_worldPoints;

	Vector3					m_begPoint; // in renormalized space

	Branch* m_parent;
	int		m_index;
	int		m_depthIndex;
	bool	m_used;
	bool	m_isE;

	float m_length;
	float m_lengthBurr;

	Branch(): m_used(false), m_isE(false), m_parent(NULL) {};
	void fromBranch(Branch& br)
	{
		m_childs = br.m_childs;
		m_points = br.m_points;
		m_depthIndex = br.m_depthIndex;
		m_index = br.m_index;
	}
		
};

//--------------------------------------------------------------------------------------------------------
struct DimUnit
{
	Vector3 m_point[2]; // start, end point
	double	m_distance;

	DimUnit(Vector3& p1, Vector3& p2, double dist):
	m_distance(dist)
	{
		m_point[0] = p1;
		m_point[1] = p2;
	}

	DimUnit() 
	{
	}
};
//--------------------------------------------------------------------------------------------------------
struct AntlerTip
{
    AntlerTip(const Vector3 pt, double length, std::string name, Branch* br):
        m_point(pt), m_length(length), m_name(name), m_branch(br)
    {}
    Vector3 m_point;    // antler's tip
    double m_length;    // tip's length
    std::string m_name; // name F->Gn
	Branch*	m_branch;

    static const int maxTips;
    static std::string nameList[21];
};
//--------------------------------------------------------------------------------------------------------
class CenterLine
{
public:
	typedef		std::vector<Vector3>						LineStrip;
	typedef		std::map<MeshType::VertexHandle, CapPoint>	HandleMap;
	typedef		std::pair<MeshType::VertexHandle, CapPoint>	HandlePair;
	typedef		OpenMesh::VPropHandleT< int > Property_Tag;
	typedef		OpenMesh::VPropHandleT< int > Property_Mark;

	bool					m_isValidPlane[2];
	Plane					m_burPlane[2];
	Vector3					*m_burCenter;
	std::vector<Vector3>	m_annotations[2];
	std::vector<Branch*>	m_sortedTips[2];		// here we will have sorted tips from F to Gn
	std::vector<DimUnit>	m_dims;					// here we will have D, C, B etc in future
    
    std::vector<AntlerTip>  m_tips[2];
private:
	float		m_radius;					
	CTriMesh	*m_triMesh;
	MeshType	*m_mesh;

	Property_Tag						m_meshTag;
	Property_Mark						m_markTag;
	std::vector<OpenMesh::Vec3f>		m_extremumPoints;
	std::vector<MeshType::VertexHandle>	m_stack;
	std::vector<DebugPoint>				m_debugPoints;
	
	std::vector<LineStrip>				m_centerLines;		
	std::list<SphereUnit>				m_spheres;

	std::vector<Branch>					m_branches;
	std::vector<std::vector<Branch*>>	m_tipList;
	std::vector<std::vector<int>>		m_mergeList;

	std::map<int, bool>					m_isMergePoint;
	std::map<int, bool>					m_isIntersected;

	HandleMap m_capPoints;
	std::vector<Cap> m_capsList;
	
	void markVerticesFromList(int index, std::vector<int> &lst);
	void markLastUsedVertices(int index);

	size_t getCapsCount(HandleMap &vlist);
	std::pair<bool, Real> GetIntersection(const Vector3 &origin, const Vector3 &direction, float radius);

	OpenMesh::Vec3f GetCenterPoint(const Plane &pl, OpenMesh::Vec3f origin, float radius);
	OpenMesh::Vec3f GetCenterPoint(const std::vector<OpenMesh::Vec3f> &vlist);
	
	// p1 must be inside sphere and p2 - outside, otherwise result will be incorrect
	OpenMesh::Vec3f GetIntersectionPoint(const OpenMesh::Vec3f &origin, float radius, 
					const OpenMesh::Vec3f &p1, const OpenMesh::Vec3f &p2);
	Vector3 GetIntersectionPoint(const Vector3 &origin, float radius, 
					const Vector3 &p1, const Vector3 &p2);
	//bool GetIntersection(OpenMesh::Vec3f &origin, float radius, MeshType::Edge edge, OpenMesh::Vec3f &res);

	int CheckAntlerPartType(const OpenMesh::Vec3f &origin, float radius, bool &bHasCenter, 
		OpenMesh::Vec3f &center, int index, bool useIndex = true, std::vector<int> &lst = std::vector<int>());

	float lineLineDist(const Vector3 &lorigin1, const Vector3 &ldir1, const Vector3 &lorigin2, const Vector3 &ldir2,
		Vector3& kClosestPoint0, Vector3& kClosestPoint1);
	float lineSegmentDist(const Vector3 &lorigin, const Vector3 &ldir,
		const Vector3 &sorigin, const Vector3 &sdir, float sextent, Vector3& kClosestPoint0, Vector3& kClosestPoint1);
	double pointEdgeDist(const Vector3 &a, const Vector3 &b1, const Vector3 &b2);
	DimUnit hausdorffDistBetweenMainBeams();


	void reconstructBranch(Branch& br);
	void generateBranchDetails(Branch& br, int depth = 0, float remainingLen = 0.0f, Branch* parent = NULL);

	void convertLengthToInches(bool bLeftBranch = true, bool bRightBranch = true, bool bInit = true);
	void calcBranchLength(Branch& br, float remainingLen = 0.0f);
	void calcWorldPoints(Branch& br, Matrix3 &mat);
	// curBranch, prevBranch = Branches to which we will trim childs of curBranch
	void TrimBranchToMeasurementPoint(Branch* curBranch, Branch* prevBranch, bool bReloadCenterlines);
	void markAsE(Branch* br, Branch* exclude, bool bMark = true);

	bool intersectPlane(const Plane &pl, const Vector3 &p1, const Vector3 &p2, Vector3 &res);
	bool intersectBurrPlane(Vector3 p1, Vector3 p2, Vector3 &res);

	Vector3 getBranchDirection(std::vector<Vector3> &br);
	void getBranchesVsSphere(const Vector3 &origin, float radius, Branch* br, std::vector<std::pair<Vector3, Branch*>> &res);


	// inline
	inline bool IsInsideSphere(const Vector3 &origin, float radius, const Vector3 &point);
	inline bool IsInsideSphere(const OpenMesh::Vec3f &origin, float radius, const OpenMesh::Vec3f &point);
	inline int IsInsideSphere2x(const OpenMesh::Vec3f &origin, float radius1, float radius2, const OpenMesh::Vec3f &point);

	inline const Vector3& toVector3(const OpenMesh::Vec3f &src);
	inline const OpenMesh::Vec3f& toVec(const Vector3 &src);

	inline void splitBranch(Branch& parent, Vector3& splitPoint);
	inline Branch fromBranch(Branch& parent, Vector3& splitPoint);
	inline Branch fromCenterline(int index, Vector3& splitPoint);
	inline void parentFromCenterline(Branch& br, int index, Vector3& splitPoint);

	inline float angleBetweenVectors(Vector3 &a, Vector3 &b);
	inline float angleBetweenLines(int index, Vector3 lastPoint);
	inline bool isBetterSegment(int index, size_t pop, Vector3 lastPoint);

	inline void smoothPoints(Vector3 &v1, Vector3& v2, Vector3& v3);
	inline void smoothPoints(Vector3 &v1, Vector3& v2, Vector3& v3, Vector3& v4);

	inline void subdivideBranchDepth(Branch& br, float div = 0.3, bool bSkipFirstLastSegment = false);
	inline void subdivideBranch(Branch& br, float div = 0.3, bool bSkipFirstLastSegment = false);
	inline void subdivideBranchInSphere(Branch& br, Vector3 &origin, float radius, float div = 0.3);

public:
	void updateBranchLength(Branch* mainBeam_l, Branch* mainBeam_r, bool bInit = true);
	void Render();
	void Compute();

	CenterLine(CTriMesh* mesh, std::vector<Vector3> exPoints, float radius, 
				Plane *burrPlane1, Plane *burrPlane2, Vector3 *burCenters);
	~CenterLine();
};


#include "CenterLine.inl"
