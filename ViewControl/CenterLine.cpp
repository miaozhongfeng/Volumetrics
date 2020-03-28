#include "stdafx.h"
#include "Kernel\Vector4.h"
#include "Kernel\Matrix4.h"
#include "Kernel\Ray.h"
#include "Kernel\Kernel.h"
#include "Kernel\TriMeshes.h"
#include "Kernel\Materials.h"

#include "CenterLine.h"

#include <sstream>

using namespace std;

//------
std::string AntlerTip::nameList[21] = { "E", "F", "G1", "G2", "G3", "G4", 
                                        "G5", "G6", "G7", "G8", "G9",
                                        "G10", "G11", "G12", "G13", "G14",
                                        "G15", "G16", "G17", "G18", "G19" };

const int AntlerTip::maxTips = 19;
float g_dist = 0;

//--------------------------------------------------------------------------------------------------------
CenterLine::CenterLine(CTriMesh* mesh, vector<Vector3> exPoints, float radius, Plane *burrPlane1, Plane *burrPlane2, Vector3 *burCenters):
m_triMesh(mesh),
m_radius(radius)
{ 
	vector<Vector3>::iterator it = exPoints.begin();
	vector<Vector3>::iterator end = exPoints.end();
	for(; it != end; ++it)
		m_extremumPoints.push_back(OpenMesh::Vec3f((float*)&(*it)));

	if(burrPlane1)
	{
		m_isValidPlane[0] = true;
		m_burPlane[0] = *burrPlane1;
	}
	else
		m_isValidPlane[0] = false;

	if(burrPlane2)
	{
		m_isValidPlane[1] = true;
		m_burPlane[1] = *burrPlane2;
	}
	else
		m_isValidPlane[1] = false;

	m_burCenter = burCenters;
}
//--------------------------------------------------------------------------------------------------------
CenterLine::~CenterLine()
{
	m_triMesh = NULL;
	m_extremumPoints.clear();
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::Render()
{
	glPushMatrix();
		glMultMatrixf(m_triMesh->_worldMatrix.Transpose());

		vector<DebugPoint>::iterator it = m_debugPoints.begin();
		vector<DebugPoint>::iterator end = m_debugPoints.end();

		//glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        if(false)
		for(; it != end; ++it)
		//if(it->m_type == 10)
		//if(it->m_criticalPoint)
		{
			if(it->m_type == 1)
				MatFBGreen.Apply();
			else
			if(it->m_type == 2)
				MatFBGold.Apply();
			else
			if(it->m_type == 3)
				MatFBBrass.Apply();
			else
			if(it->m_type == 4)
				MatFBRed.Apply();
			else
			if(it->m_type == 0)
				MatFBViolet.Apply();
			else
				MatFBPink.Apply();

			glPushMatrix();
				glTranslatef(it->m_position[0], it->m_position[1], it->m_position[2]);
				glutSolidSphere(it->m_radius,16,16);
			glPopMatrix();
		}

		glDisable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT_AND_BACK);
		glDisable(GL_CULL_FACE);

		glEnable(GL_LINE_SMOOTH);
		glDisable(GL_DEPTH_TEST);
		glLineWidth(2.0);
		MatFBPolishedCopper.Apply();

		for(size_t i = 0; i < m_centerLines.size(); i++)
			if(m_centerLines[i].size() > 1)
			{
				glBegin(GL_LINE_STRIP);

				LineStrip::iterator it = m_centerLines[i].begin();
				LineStrip::iterator endi = m_centerLines[i].end();
				for(; it != endi; ++it)
					glVertex3f((*it)[0], (*it)[1], (*it)[2]);

				glEnd();
			}

		glLineWidth(1.0);
		glDisable(GL_LINE_SMOOTH);
		glEnable(GL_DEPTH_TEST);

	glPopMatrix();
}
//--------------------------------------------------------------------------------------------------------
OpenMesh::Vec3f CenterLine::GetCenterPoint(const Plane &pl, OpenMesh::Vec3f origin, float radius)
{
	vector<OpenMesh::Vec3f> list;

	MeshType::VertexIter vEnd =  m_mesh->vertices_end();
	for (MeshType::VertexIter it = m_mesh->vertices_begin(); it != vEnd; ++it)
		m_mesh->status(it).set_tagged(false);

	Vector3 p1;
	Vector3 res;

	MeshType::VertexVertexIter vv_it;
	for (MeshType::VertexIter it = m_mesh->vertices_begin(); it != vEnd; ++it)
		if(!m_mesh->status(it).tagged() && IsInsideSphere(origin, radius, m_mesh->point(it)))
		{
			p1 = toVector3(m_mesh->point(it));
			m_mesh->status(it).set_tagged(true);
			vv_it = m_mesh->vv_iter(it);
			for (; vv_it; ++vv_it)
				if(!m_mesh->status(vv_it).tagged())
					if(intersectPlane(pl, p1, toVector3(m_mesh->point(vv_it)), res))
						list.push_back(toVec(res));
		}


	// get distance between outermost intersection points 
	float dist, dist2, mdist;
	OpenMesh::Vec3f v1,v2,v3, cp;
	dist = 0;
	vector<OpenMesh::Vec3f>::iterator it = list.begin();
	vector<OpenMesh::Vec3f>::iterator end = list.end();
	for(; it != end; ++it)
	{
		vector<OpenMesh::Vec3f>::iterator it2 = list.begin();
		for(; it2 != end; ++it2)
			if(it != it2)
			{
				dist2 = ((*it) - (*it2)).sqrnorm();
				if(dist < dist2)
				{
					v1 = *it;
					v2 = *it2;
					dist = dist2;
				}
			}
	}
	
	mdist = sqrt(dist) / 2;
	cp = (v1 + v2) / 2;

	dist = 0;
	vector<OpenMesh::Vec3f>::iterator it2 = list.begin();
	for(; it2 != end; ++it2)
	{
		dist2 = (cp - (*it2)).sqrnorm();
		if(dist < dist2)
		{
			v3 = *it2;
			dist = dist2;
		}
	}

	dist = sqrt(dist);
	if(mdist > dist)
		return cp;
	else
	{
		return (v1 + v2 + v3) / 3;
	}

	//m_debugPoints.push_back(DebugPoint(origin, radius, 10, false));

	//return GetCenterPoint(list);
}
//--------------------------------------------------------------------------------------------------------
inline OpenMesh::Vec3f CenterLine::GetCenterPoint(const vector<OpenMesh::Vec3f> &vlist)
{
	if(vlist.empty())
		return OpenMesh::Vec3f(0,0,0);

	vector<OpenMesh::Vec3f>::const_iterator it = vlist.begin();
	vector<OpenMesh::Vec3f>::const_iterator end = vlist.end();

	float invCount = 1.0f/vlist.size();
	OpenMesh::Vec3f res(0,0,0);

	for(; it != end; ++it)
		res += (*it) * invCount;

	return res;
}
//--------------------------------------------------------------------------------------------------------
inline void addList(vector<OpenMesh::Vec3f> &dest, vector<OpenMesh::Vec3f> &source)
{
	dest.insert(
		dest.end(), 
		source.begin(),
		source.end());
}
//--------------------------------------------------------------------------------------------------------
size_t CenterLine::getCapsCount(HandleMap &vlist/*, int depth*/)
{
	// TODO: eliminate incorrect gaps (inside sphere maybe maybe different,
	// not connected mesh parts and we should leave only one main part)
	// maybe by checking vert count in every part ?!
	m_capsList.clear();
	m_stack.clear();
	HandleMap::iterator it = vlist.begin();
	HandleMap::iterator _it;
	MeshType::VertexVertexIter vv_it;

	vector<pair<int,int>> merge_lst;

	while(it != vlist.end())
	{
		if(!it->second.m_used)
		{
			it->second.m_used = true;
			if(m_stack.empty())
			{
				m_stack.push_back(it->first);
				while(m_stack.rbegin() != m_stack.rend())
				{
					vv_it = m_mesh->vv_iter(*m_stack.rbegin());
					// we was here already :)
					m_mesh->property(m_meshTag, *m_stack.rbegin()) = 0xff;

					m_stack.pop_back();

					for (; vv_it; ++vv_it)
						if(it->first != vv_it)
						{
							// is vertex in first outside ring
							if(m_mesh->status(vv_it).tagged())
							{
								_it = vlist.find(vv_it);
								// TODO: maybe it'ld be better to remove handle from main list ?!
								//if(!_it->second.m_used)
								{
									_it->second.m_used = true;

									if(_it->second.m_capIndex < 0)
									{
										if(it->second.m_capIndex < 0)
										{
											Cap cap;
											it->second.m_capIndex = m_capsList.size();
											_it->second.m_capIndex = m_capsList.size();

											cap.m_vHandle = vv_it;
											addList(cap.m_intPoints, it->second.m_intPoints);
											addList(cap.m_intPoints, _it->second.m_intPoints);

											m_capsList.push_back(cap);
										}
										else
										{
											_it->second.m_capIndex = it->second.m_capIndex;

											addList(m_capsList[it->second.m_capIndex].m_intPoints, _it->second.m_intPoints);
										}
									}
									else
									{
										if(it->second.m_capIndex < 0)
										{
											it->second.m_capIndex = _it->second.m_capIndex;

											addList(m_capsList[_it->second.m_capIndex].m_intPoints, it->second.m_intPoints);
										}
										else
											if(it->second.m_capIndex != _it->second.m_capIndex)
												// this occurs 'cause we using volume inside 2nd sphere and outside 1st
												// to find gaps in hi poly mesh, but our mesh maybe hi poly in one part 
												// and lo poly in other (( 
												merge_lst.push_back(pair<int,int>(it->second.m_capIndex, _it->second.m_capIndex));
									}
								}
							}

							// is inside 2nd sphere and outside 1st
							if(m_mesh->property(m_meshTag, vv_it) == 1)
								m_stack.push_back(vv_it);
						}
				}
			}
		}

		it++;
	}

	if(!merge_lst.empty())
	{
		for(int i=0; i<3; i++)
		{
			vector<pair<int,int>>::iterator itm = merge_lst.begin();
			vector<pair<int,int>>::iterator iend = merge_lst.end();
			for(; itm != iend; ++itm)
			if(m_capsList[itm->second].m_intPoints.size() > 0)
			{
				addList(m_capsList[itm->first].m_intPoints, m_capsList[itm->second].m_intPoints);
				m_capsList[itm->second].m_intPoints.clear();
			}
		}

		vector<Cap>::iterator itc = m_capsList.begin();

		while(itc != m_capsList.end())
		{
			if(itc->m_intPoints.empty())
			{
				m_capsList.erase(itc);
				itc = m_capsList.begin();
			}
			else
				itc++;
		}

		/*wostringstream str;
		str<<"Caps: "<<currCap;
		MessageBox(0,str.str().c_str(),L"Some",0);*/
	}

	return m_capsList.size();
}
//--------------------------------------------------------------------------------------------------------
int CenterLine::CheckAntlerPartType(const OpenMesh::Vec3f &origin, float radius, bool &bHasCenter, 
									OpenMesh::Vec3f &center, int index, bool useIndex, std::vector<int> &lst)
{
	m_capPoints.clear();
	bHasCenter = false;

	MeshType::VertexIter vEnd =  m_mesh->vertices_end();
	for (MeshType::VertexIter it = m_mesh->vertices_begin(); it != vEnd; ++it)
	{
		//m_Mesh->status(it).set_tagged(IsInsideSphere(origin, radius, m_Mesh->point(it)));
		// status used to mark first outside sphere ring of vertices
		m_mesh->status(it).set_tagged(false);
		// to speed up search process
		// if 0 than outside 2nd sphere
		// if 1 than outside 1st sphere and inside 2nd sphere
		// if 2 than inside 1st sphere
		m_mesh->property(m_meshTag, it) = IsInsideSphere2x(origin, radius, 2*radius, m_mesh->point(it)) + 10;
	}



	{
		MeshType::VertexVertexIter vv_it;
		int iC = 0;
		int iCMax = 0;
		map<MeshType::VertexHandle, int> partMap;

		m_stack.clear();
		for (MeshType::VertexIter it = m_mesh->vertices_begin(); it != vEnd; ++it)
		{
			int& prop = m_mesh->property(m_meshTag, it);
			if(prop == 12)
			{
				iC = 0;
				m_stack.push_back(it);
				while(m_stack.rbegin() != m_stack.rend())
				{
					iC++;
					vv_it = m_mesh->vv_iter(*m_stack.rbegin());
					// we was here already :)
					m_mesh->property(m_meshTag, *m_stack.rbegin()) = 2;

					m_stack.pop_back();

					for (; vv_it; ++vv_it)
						// is inside 1st sphere
						if(m_mesh->property(m_meshTag, vv_it) == 12)
							m_stack.push_back(vv_it);
				}
				if(iCMax < iC)
					iCMax = iC;
				partMap[it] = iC;
			}
			else
				if(prop>=10)
					prop -= 10;
		}

		map<MeshType::VertexHandle, int>::iterator pit = partMap.begin();
		map<MeshType::VertexHandle, int>::iterator piend = partMap.end();
		for(; pit != piend; ++pit)
			if(pit->second != iCMax)
			{
				m_stack.push_back(pit->first);
				while(m_stack.rbegin() != m_stack.rend())
				{
					vv_it = m_mesh->vv_iter(*m_stack.rbegin());
					m_mesh->property(m_meshTag, *m_stack.rbegin()) = 0;

					m_stack.pop_back();

					for (; vv_it; ++vv_it)
						// is inside 1st sphere
						if(m_mesh->property(m_meshTag, vv_it) == 2)
							m_stack.push_back(vv_it);
				}
			}

		/*if(iC>1)
		{
			m_debugPoints.push_back(DebugPoint(origin, radius, 7, true));

			wostringstream str;
			str<<"iC: "<<iC;
			MessageBox(0,str.str().c_str(),L"Some",0);
		}*/
	}



	// get intersection points
	// mark first outside ring of vertices
	MeshType::VertexVertexIter vv_it;
	OpenMesh::Vec3f intPoint;
	for (MeshType::VertexIter it = m_mesh->vertices_begin(); it != vEnd; ++it)
		if(m_mesh->property(m_meshTag, it) == 2)
		{
			vv_it = m_mesh->vv_iter(it);
			for (; vv_it; ++vv_it)		
				if(m_mesh->property(m_meshTag, vv_it) < 2)
				{
					m_capPoints[vv_it] = GetIntersectionPoint(origin, radius, m_mesh->point(it), m_mesh->point(vv_it));
					m_mesh->status(vv_it).set_tagged(true);
				}
		}

	// get caps count
	size_t capsCount = getCapsCount(m_capPoints);
	
	// get distance between outermost intersection points 
	float mdist = 0, dist, dist2;
	for(size_t i = 0; i < m_capsList.size(); i++)
	{
		dist = 0;
		vector<OpenMesh::Vec3f>::iterator it = m_capsList[i].m_intPoints.begin();
		vector<OpenMesh::Vec3f>::iterator end = m_capsList[i].m_intPoints.end();
		for(; it != end; ++it)
		{
			vector<OpenMesh::Vec3f>::iterator it2 = m_capsList[i].m_intPoints.begin();
			for(; it2 != end; ++it2)
				if(it != it2)
				{
					dist2 = ((*it) - (*it2)).sqrnorm();
					if(dist < dist2)
						dist = dist2;
				}
		}

		dist = sqrt(dist);
		m_capsList[i].m_distance = dist;
		if(mdist < dist)
			mdist = dist;
	}

	int res;
	res = 0;
	float chkDist = 0.8f * radius * 2.f;
	if(mdist < chkDist)
	{
		if(capsCount == 1)
		{
			// TODO: check normal direction
			// we have 'a' type (tip of branch)
			res = 1;
			center = GetCenterPoint(m_capsList[0].m_intPoints);
			bHasCenter = true;
		}
		else
		if(capsCount == 2)
		{
			// we have 'b' type (tube part of branch)
			res = 2;
		}
		else
		{
			// we have 'c' type (connection unit)
			res = 3;
		}
		
		if(res == 2 || res == 3)
		{
			int unmarkedCount = 0;
			if(useIndex)
			for(size_t i = 0; i < m_capsList.size(); i++)
			{
				//if(m_mesh->property(m_markTag, m_capsList[i].m_vHandle) < 0)
				if(m_mesh->property(m_markTag, m_capsList[i].m_vHandle) != index)
					//&& m_capsList[i].m_distance > 0.2f * radius)
					unmarkedCount++;
				else
					m_capsList[i].m_marked = true;
				/*int uCount = 0;
				vector<OpenMesh::Vec3f>::iterator it = m_capsList[i].m_intPoints.begin();
				vector<OpenMesh::Vec3f>::iterator end = m_capsList[i].m_intPoints.end();
				for(; it != end; ++it)
					if(m_mesh->property(m_markTag, it->first) < 0)
						uCount++;

				m_capsList[i].m_marked = (uCount * 2.f >  m_capsList[i].m_intPoints.size)? true: false;
				if(!m_capsList[i].m_marked)
					unmarkedCount++;*/
			}
			else
			for(size_t i = 0; i < m_capsList.size(); i++)
			{
				int n = 0;
				for(size_t j = 0; j < lst.size(); j++)
					if(m_mesh->property(m_markTag, m_capsList[i].m_vHandle) != lst[j])
						n++;

				if(n == lst.size())
					unmarkedCount++;
				else
					m_capsList[i].m_marked = true;

				/*if(m_mesh->property(m_markTag, m_capsList[i].m_vHandle) < 0)
					unmarkedCount++;
				else
					m_capsList[i].m_marked = true;*/
			}
			
			if(unmarkedCount == 1)
			{
				for(size_t i = 0; i < m_capsList.size(); i++)
					if(!m_capsList[i].m_marked)// && m_capsList[i].m_distance < chkDist)
					//if(!m_capsList[i].m_marked && m_capsList[i].m_distance < 2.0 * m_radius)
					{
						/*if(m_capsList[i].m_distance > 1.5 * m_radius)
						{
							m_debugPoints.push_back(DebugPoint(origin, radius, 1, true));
							//wostringstream str;
							//str<<"m_distance: "<<m_capsList[i].m_distance / m_radius
							//	<<"m_distance: "<<radius / m_radius;
							//MessageBox(0,str.str().c_str(),L"Some",0);
						}*/
						g_dist = m_capsList[i].m_distance;
						center = GetCenterPoint(m_capsList[i].m_intPoints);
						m_capsList[i].m_center = center;
						m_capsList[i].m_hasCenter = true;
						bHasCenter = true;
						break;
					}
			}
		}
	}
	else
	{
		// we have 'd' type (unknown part of branch)
		res = 4;

		/*int unmarkedCount = 0;
		if(useIndex)
		for(size_t i = 0; i < m_capsList.size(); i++)
		{
			if(m_mesh->property(m_markTag, m_capsList[i].m_vHandle) != index)
				unmarkedCount++;
			else
				m_capsList[i].m_marked = true;
		}
		else
		for(size_t i = 0; i < m_capsList.size(); i++)
		{
			int n = 0;
				for(size_t j = 0; j < lst.size(); j++)
					if(m_mesh->property(m_markTag, m_capsList[i].m_vHandle) != lst[j])
						n++;

				if(n == lst.size())
					unmarkedCount++;
				else
					m_capsList[i].m_marked = true;
		}

		if(capsCount >= 2)
			if(unmarkedCount == 1)
			{
				for(size_t i = 0; i < m_capsList.size(); i++)
					if(!m_capsList[i].m_marked)
					{
						center = GetCenterPoint(m_capsList[i].m_intPoints);
						bHasCenter = true;
						break;
					}
			}*/
	}
	// if res == 0 than some error occurs

	wostringstream str;
	str//<<"Intersection points: "<<allIntPoints.size()
		<<"\nDiameter: "<<radius * 2.0f
		<<"\nDistance: "<<mdist
		<<"\nCaps count: "<<m_capsList.size()
		<<"\nType: "<<res;
	/*if(res == 3)
	{
		for(size_t i = 0; i < m_capsList.size(); i++)
			str<<"\nDistance: "<<m_capsList[i].m_distance;
		for(size_t i = 0; i < m_capsList.size(); i++)
			str<<"\nSize: "<<m_capsList[i].m_intPoints.size();
		MessageBox(0,str.str().c_str(),L"Some",0);
	}*/

	return res;
}
//--------------------------------------------------------------------------------------------------------
Vector3 CenterLine::GetIntersectionPoint(const Vector3 &origin, float radius, 
					const Vector3 &p1, const Vector3 &p2)
{
	Vector3 kDiff = p1 - origin;
    Real fA0 = kDiff.Dot(kDiff) - radius*radius;
    Real fA1;

	if(fA0 <= 0)
	{
		Vector3 vDir = p2 - p1;
		vDir.Normalize();

		// P1 is inside the sphere
		fA1 = vDir.Dot(kDiff);
		return p1 + (-fA1 + Math::Sqrt(fA1*fA1 - fA0)) * vDir;
	}
	else
		return p1; // didn't check for intesection simply return p1
}
//--------------------------------------------------------------------------------------------------------
OpenMesh::Vec3f CenterLine::GetIntersectionPoint(const OpenMesh::Vec3f &origin, float radius, 
					const OpenMesh::Vec3f &p1, const OpenMesh::Vec3f &p2)
{
	// TODO: uncomment this and fix some code in centerline connection
	// + improve smoothing
	OpenMesh::Vec3f kDiff = p1 - origin;
    Real fA0 = dot(kDiff, kDiff) - radius*radius;
    Real fA1;

	if(fA0 <= 0)
	{
		OpenMesh::Vec3f vDir = p2 - p1;
		vDir.normalize();

		// P1 is inside the sphere
		fA1 = dot(vDir, kDiff);
		return p1 + (-fA1 + Math::Sqrt(fA1*fA1 - fA0)) * vDir;
	}
	else
		return p1; // didn't check for intesection simply return p1/**/
	/*OpenMesh::Vec3f vDir = p2 - p1;
	vDir.normalize();

	return p1 - dot(vDir, p1 - origin) * vDir;/**/
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::markLastUsedVertices(int index)
{
	MeshType::VertexIter vEnd =  m_mesh->vertices_end();
	for (MeshType::VertexIter it = m_mesh->vertices_begin(); it != vEnd; ++it)
	{
		if(m_mesh->property(m_meshTag, it) == 2)
			m_mesh->property(m_markTag, it) = index;
	}
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::markVerticesFromList(int index, std::vector<int> &lst)
{
	MeshType::VertexIter vEnd =  m_mesh->vertices_end();
	vector<int>::iterator lit;
	vector<int>::iterator ibeg = lst.begin() + 1;
	vector<int>::iterator iend = lst.end();
	for (MeshType::VertexIter it = m_mesh->vertices_begin(); it != vEnd; ++it)
	{
		for(lit = ibeg; lit != iend; ++lit)
			if(m_mesh->property(m_markTag, it) == *lit)
			{
				m_mesh->property(m_markTag, it) = index;
				break;
			}
	}
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::Compute()
{
	if(!m_triMesh || !m_triMesh->_loaded)
		return;

	m_mesh = &m_triMesh->_mesh;
	m_mesh->add_property( m_meshTag, "tag" );
	m_mesh->add_property( m_markTag, "mark" );

	// set all vectices in mesh to unmarked state
	MeshType::VertexIter vEnd =  m_mesh->vertices_end();
	for(MeshType::VertexIter it = m_mesh->vertices_begin(); it != vEnd; ++it)
		m_mesh->property(m_markTag, it) = -1;

	// clear all stuff
	m_dims.clear();
	m_stack.clear();
	m_spheres.clear();
	m_debugPoints.clear();
	m_centerLines.clear();
	m_mergeList.clear();
	m_tipList.clear();
	m_branches.clear();
	m_isMergePoint.clear();
	m_isIntersected.clear();
	for(int i = 0; i < 2; i++)
	{
		m_annotations[i].clear();
		m_sortedTips[i].clear();
	}
	m_stack.reserve(m_mesh->n_faces());	// for speed performance )

	OpenMesh::Vec3f centerPoint;	// current center point
	OpenMesh::Vec3f retCenterPoint;	// result center point
	float radius;					// current radius
	float step = m_radius * 0.1;	// step for inc/dec radius
	float angle = 15;				// 
	int index = 0;					// ?? )
	int incRadCount = 0;
	bool bHasCenter;
	bool bInvalid;

	vector<OpenMesh::Vec3f>::iterator iend = m_extremumPoints.end();
	vector<OpenMesh::Vec3f>::iterator it = m_extremumPoints.begin();
	vector<OpenMesh::Vec3f>::iterator it2;
	for(; it != iend; ++it)
	{
		// skip invalid points
		bInvalid = false;

		it2 = it + 1;
		for(; it2 != iend; ++it2)
			if((*it - *it2).length() < m_radius * 0.5f)
			{
				bInvalid = true;
				break;
			}

		if(bInvalid)
			continue;

		radius = 0.5 * m_radius;
		int type = CheckAntlerPartType(*it, radius, bHasCenter, retCenterPoint, index);
		//m_debugPoints.push_back(DebugPoint(*it, radius, type, true));

		if(type != 1)
		{
			radius *= 2;
			type = CheckAntlerPartType(*it, radius, bHasCenter, retCenterPoint, index);
			//m_debugPoints.push_back(DebugPoint(*it, radius, type, true));

			// skip invalid points 2nd pass
			it2 = m_extremumPoints.begin();
			//it2 = it + 1;
			for(; it2 != iend; ++it2)
				if((*it - *it2).length() < radius)
				{
					type = CheckAntlerPartType(*it2, 0.5 * m_radius, bHasCenter, retCenterPoint, index);
					if(type == 1)
					{
						bInvalid = true;
						break;
					}
				}

			if(!bInvalid)
				type = CheckAntlerPartType(*it, radius, bHasCenter, retCenterPoint, index);
		}

		if(bInvalid)
			continue;

		m_centerLines.push_back(LineStrip());
		m_centerLines[index].push_back(toVector3(*it));
		if(bHasCenter)
			m_centerLines[index].push_back(toVector3(retCenterPoint));
		
		if(type == 1)
		{
			radius = 0.5 * m_radius;
			centerPoint = retCenterPoint;

			int tri = 0;
			markLastUsedVertices(index);
			while(true)
			{
				type = CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, index);

				if(type == 4)
				{
					//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
					radius += step;
					if(radius > 3*m_radius || tri > 0)
					{
						radius -= 3*step;
						m_isMergePoint[index] = false;
						m_spheres.push_back(SphereUnit(index, centerPoint, radius));
						//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
						break;
					}
					continue;
				}
				else
				if(bHasCenter && type < 3)
				{
					if(tri > 0)
					{
						radius -= 2*step;
						tri = 0;
					}
					{
						radius -= step;
						//float chkAngle = min(angle, angleBetweenLines(index, toVector3(retCenterPoint)));
						if(radius >= m_radius * 0.5)
						{
							int steps = 0;
							type = CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, index);
							while(type < 3 && type > 0 && bHasCenter 
								&& angleBetweenLines(index, toVector3(retCenterPoint))<angle)
							{
								radius -= step;
								steps++;
								if(radius < m_radius * 0.5 || steps > 3)
									break;
								type = CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, index);
							}
						}
						radius += step;
						CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, index);
						if (angleBetweenLines(index, toVector3(retCenterPoint))<angle)
						{
							incRadCount = 0;
							angle = 15;
							Vector3 resV;
							if(intersectBurrPlane(*m_centerLines[index].rbegin(), toVector3(retCenterPoint), resV))
							{
								//MessageBox(0,L"",L"",0);
								m_isIntersected[index] = true;
								m_centerLines[index].push_back(resV);
								break;
							}

							if(m_centerLines[index].size() > 0)
							{
								Vector3 dir = toVector3(retCenterPoint) - *m_centerLines[index].rbegin();
								float len = dir.Length();
								if(len > step)
								{
									dir.Normalize();
									centerPoint = toVec(*m_centerLines[index].rbegin() + dir * len * 0.5);
								}
								else
									centerPoint = retCenterPoint;
							}
							else
								centerPoint = retCenterPoint;

							//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
							m_centerLines[index].push_back(toVector3(retCenterPoint));
							markLastUsedVertices(index);
						}
						else
						{
							incRadCount++;
							if(incRadCount >= 2)
							{
								radius -= step;
								incRadCount = 0;
								angle += 2.5;
							}
							else
								radius += step;
						}
					}
				}
				else
				{
					if(tri == 0 && type == 3)
					{
						tri++;
						radius += 4*step;
						continue;
					}
					else
					{
						if(tri > 0)
							radius -= 2*step;
						if(type == 3 || type == 2)
						{
							m_isMergePoint[index] = false;
							m_spheres.push_back(SphereUnit(index, centerPoint, radius));

							if(m_centerLines[index].size() > 1)
							{
								// find most closest center for tri
								Vector3 a = *m_centerLines[index].rbegin() - *(m_centerLines[index].rbegin() + 1);
								a.Normalize();

								float newRad = radius;
								centerPoint = toVec(*m_centerLines[index].rbegin() + a * step);

								while(true)
								{
									type = CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, index);
									int ccnt = 0;
									while(type <= 3 && type > 0)
									{
										ccnt++;
										radius -= step;
										type = CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, index);
									}	
									radius += step;

									if(newRad <= radius || ccnt == 0)
									{
										radius = newRad;
										centerPoint -= toVec(a * step);
										//m_centerLines.rbegin()->push_back(toVector3(centerPoint));
										break;
									}
									
									centerPoint += toVec(a * step);
									newRad = radius;
								}
							}

							type = CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, index);

							int ccnt = 0; 
							Vector3 p[2];
							for(size_t i = 0; i < m_capsList.size(); i++)
								if(!m_capsList[i].m_hasCenter)// && m_capsList[i].m_distance < chkDist)
								{
									m_capsList[i].m_center = GetCenterPoint(m_capsList[i].m_intPoints);
									m_capsList[i].m_hasCenter = true;

									if(!m_capsList[i].m_marked)
									{
										if(ccnt < 2)
											p[ccnt] = toVector3(m_capsList[i].m_center);
										ccnt++;
									}
								}
							if(ccnt == 2 && m_capsList.size() <= 3)
							{
								//if(angleBetweenVectors(a, b) > 25 && angleBetweenVectors(a, c) > 25)
								{
									m_spheres.rbegin()->m_hasConnectPoints = true;
									m_spheres.rbegin()->m_connectPoint[0] = p[0];
									m_spheres.rbegin()->m_connectPoint[1] = p[1];
								}
							}
								/*//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
								//Vector3 dir = toVector3(m_capsList[0].m_center - m_capsList[1].m_center);
								//Vector3 ct = toVector3((m_capsList[0].m_center + m_capsList[1].m_center) / 2
								
								Vector3 dir = p[0] - p[1];
								Vector3 ct = (p[0] + p[1]) / 2;
								dir.Normalize();
								if(m_centerLines[index].size() > 1)
								{
									//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
									Vector3 a = *m_centerLines[index].rbegin() - *(m_centerLines[index].rbegin() + 1);
									a.Normalize();
									float ang = angleBetweenVectors(a, dir);
									if(ang > 90 && ang < 180) 
										ang = 180 - ang;

									if(ang > 55)
									{
										m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
										float dist = (ct - *m_centerLines[index].rbegin()).Length();
										Vector3 newPos = *m_centerLines[index].rbegin() + a * dist / sin(ang * Math::DEG_TO_RAD) * 0.5f;
										m_centerLines.rbegin()->push_back(newPos);
									}
								}
							}*/
							//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
						}
						//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
						break;
					}
				}
			}
		}

		index++;
	}

	//return;

	int pass = 0;
	float scaleRad = 0.3f;
	int isEmptyPass = 0;
	vector<SphereUnit*> intersectSpheres;
	vector<SphereUnit*> closestSpheres;
	vector<pair<float, SphereUnit*>> lengthList;

	float avRadius;
	OpenMesh::Vec3f avPos;
	int iCounter;

	angle = 25;

	while(m_spheres.size() > 0 && pass < 25)
	{
		list<SphereUnit>::iterator ibeg = m_spheres.begin();
		list<SphereUnit>::iterator iend = m_spheres.end();
		list<SphereUnit>::iterator it2;
		list<SphereUnit>::iterator it = ibeg;
		for(; it != iend; ++it)
			it->m_used = false;	
			
		intersectSpheres.clear();
		closestSpheres.clear();
		lengthList.clear();

		/*it = ibeg;
		intersectSpheres.push_back(&(*it));
		for(; it != iend; ++it)
			if(std::find(intersectSpheres.begin(), intersectSpheres.end(), &(*it)) != intersectSpheres.end())
			{
				it->m_used = true;
				it2 = it;
				it2++;
				for(; it2 != iend; ++it2)
					if(!it2->m_used)
					{
						float length = (it2->m_origin - it->m_origin).length();
						if( length < (it2->m_radius + it->m_radius) * 0.7)
						{
							lengthList.push_back(pair<float, SphereUnit*>(length, &(*it)));
							if(find(intersectSpheres.begin(), intersectSpheres.end(), &(*it2)) == intersectSpheres.end())
								intersectSpheres.push_back(&(*it2));
						}
					}
			}

		if(intersectSpheres.size() > 2)
		{
			pass++;
			int ic = 0;
			int icl = 0;
			bool bFind;
			float closestLen = lengthList[0].first;
			SphereUnit* sphere = lengthList[0].second;

			for(size_t i = 1; i < lengthList.size(); i++)
				if(closestLen > lengthList[i].first && lengthList[i].second == *intersectSpheres.begin())
				{
					closestLen = lengthList[i].first;
					sphere = lengthList[i].second;
				}

			for(size_t i = 0; i < lengthList.size(); i++)
			{
				bFind = (lengthList[i].second == *intersectSpheres.begin());
				icl = 0;
				//if(lengthList[i].first <= closestLen * 1.4)
					for(size_t j = 0; j < lengthList.size(); j++)
						if(j != i)
						{
							//if(lengthList[j].first > closestLen * 1.4)
							//	continue;
							if(abs(lengthList[i].first - lengthList[j].first) < closestLen * 0.15)
							{
								icl++;
								if(!bFind)
									bFind = (lengthList[j].second == *intersectSpheres.begin());
							}
						}

				if(icl > ic && bFind)
				{
					ic = icl;
					sphere = lengthList[i].second;
				}
			}
			closestSpheres.push_back(sphere);
			for(size_t i = 0; i < intersectSpheres.size(); i++)
				if(sphere != intersectSpheres[i])
				{
					float len = (sphere->m_origin - intersectSpheres[i]->m_origin).length();
					if( abs(len - closestLen) < closestLen * 0.15)
						closestSpheres.push_back(intersectSpheres[i]);
				}

			intersectSpheres = closestSpheres;
		}*/

		{
			if(isEmptyPass >= 2)
			{
				//angle += 2.5;
				scaleRad += 0.1f;
				if(scaleRad > 0.8)
					break;
				isEmptyPass = 0;
			}

			// find which branch to connect
			for(it = ibeg; it != iend; ++it)
			{
				intersectSpheres.push_back(&(*it));
				for(it2 = ibeg; it2 != iend; ++it2)
					if(it != it2)
						if((it2->m_origin - it->m_origin).length() < 
							(it2->m_radius + it->m_radius) * scaleRad)
						{
							intersectSpheres.push_back(&(*it2));
							it2->m_used = true;
						}

				if(intersectSpheres.size() > 1)
					break;

				intersectSpheres.clear();
			}

			if(intersectSpheres.size() > 2)
			{
				closestSpheres.clear();

				for(it = ibeg; it != iend; ++it)
				{
					closestSpheres.push_back(&(*it));
					for(it2 = ibeg; it2 != iend; ++it2)
						if(it != it2)
							if((it2->m_origin - it->m_origin).length() < 
								(it2->m_radius + it->m_radius) * scaleRad)
							{
								closestSpheres.push_back(&(*it2));
								it2->m_used = true;
							}

					if(closestSpheres.size() == 2)
					{
						bool bFind = false;
						for(size_t cs = 0; cs < closestSpheres.size(); cs++)
							if(closestSpheres[cs] == *intersectSpheres.begin())
							{
								bFind = true;
								break;
							}
						if(bFind)
						{
							intersectSpheres = closestSpheres;
							break;
						}
					}

					closestSpheres.clear();
				}
			}

			/*if(intersectSpheres.size() == 2)
				closestSpheres = intersectSpheres;

			for(it = ibeg; it != iend; ++it)
			{
				intersectSpheres.push_back(&(*it));
				for(it2 = ibeg; it2 != iend; ++it2)
					if(it != it2)
						if((it2->m_origin - it->m_origin).length() < 
							(it2->m_radius + it->m_radius) * scaleRad)
						{
							intersectSpheres.push_back(&(*it2));
							it2->m_used = true;
						}

				if(intersectSpheres.size() == 2)
					break;

				intersectSpheres.clear();
			}

			if(intersectSpheres.size() != 2 || closestSpheres == intersectSpheres || pass > 5)
			{
				intersectSpheres.clear();
				for(it = ibeg; it != iend; ++it)
				{
					intersectSpheres.push_back(&(*it));
					for(it2 = ibeg; it2 != iend; ++it2)
						if(it != it2)
							if((it2->m_origin - it->m_origin).length() < 
								(it2->m_radius + it->m_radius) * scaleRad)
							{
								intersectSpheres.push_back(&(*it2));
								it2->m_used = true;
							}

					if(intersectSpheres.size() > 2)
						break;

					intersectSpheres.clear();
				}
			}

			if(intersectSpheres.size() < 2)
			{
				intersectSpheres.clear();
				for(it = ibeg; it != iend; ++it)
				{
					intersectSpheres.push_back(&(*it));
					for(it2 = ibeg; it2 != iend; ++it2)
						if(it != it2)
							if((it2->m_origin - it->m_origin).length() < 
								(it2->m_radius + it->m_radius) * scaleRad)
							{
								intersectSpheres.push_back(&(*it2));
								it2->m_used = true;
							}

					if(intersectSpheres.size() > 1)
						break;

					intersectSpheres.clear();
				}
			}*/
		}

		if(intersectSpheres.size() < 2)
		{
			scaleRad += 0.1f;
			if(scaleRad > 1)
				break;
			continue;
		}

		avPos = OpenMesh::Vec3f(0,0,0);
		avRadius = 0;
		iCounter = 0;
		for(size_t i = 0; i < intersectSpheres.size(); i++, iCounter++)
		{
			avPos += intersectSpheres[i]->m_origin;
			avRadius += intersectSpheres[i]->m_radius;
		}
		avPos /= iCounter;
		avRadius /= iCounter;

		// ------------------------
		if(avRadius < m_radius)
			avRadius = m_radius;
		
		vector<int> lst;
		vector<SphereUnit*>::iterator iit = intersectSpheres.begin();
		vector<SphereUnit*>::iterator iiend = intersectSpheres.end();
		int curIndex = (*iit)->m_index;

		for(; iit != iiend; ++iit)
			lst.push_back((*iit)->m_index);

		int type;
		if(intersectSpheres.size() == 2)
		{
			bHasCenter = false;
			bool bHasCenter1, bHasCenter2;
			OpenMesh::Vec3f p1, p2;

			avPos = intersectSpheres[0]->m_origin;
			avRadius = intersectSpheres[0]->m_radius;
			type = CheckAntlerPartType(avPos, avRadius, bHasCenter1, p1, curIndex, false, lst);
			//if(bHasCenter)
			{
				avPos = intersectSpheres[1]->m_origin;
				avRadius = intersectSpheres[1]->m_radius;
				type = CheckAntlerPartType(avPos, avRadius, bHasCenter2, p2, curIndex, false, lst);
				//if(bHasCenter)
				{
					Vector3 lorigin1, ldir1;
					Vector3 lorigin2, ldir2;

				for(int _pass = 0; _pass < 2; _pass++)
				{
					int conType = 0;
					lorigin1 = *(m_centerLines[intersectSpheres[0]->m_index].rbegin() + 1);
					ldir1 = (lorigin1 - *m_centerLines[intersectSpheres[0]->m_index].rbegin());
					ldir1.Normalize();
					lorigin2 = *(m_centerLines[intersectSpheres[1]->m_index].rbegin() + 1);
					ldir2 = (lorigin2 - *m_centerLines[intersectSpheres[1]->m_index].rbegin());
					ldir2.Normalize();

					if(_pass == 0)
					if((m_isMergePoint[intersectSpheres[0]->m_index] || m_isMergePoint[intersectSpheres[1]->m_index])&&
						(intersectSpheres[0]->m_hasConnectPoints || intersectSpheres[1]->m_hasConnectPoints) 
						&& angleBetweenVectors(ldir1, ldir2) > 60)
					{
						if(intersectSpheres[0]->m_hasConnectPoints)
						{
							lorigin1 = intersectSpheres[0]->m_connectPoint[0];
							ldir1 = lorigin1 - intersectSpheres[0]->m_connectPoint[1];
							ldir1.Normalize();
							lorigin2 = *(m_centerLines[intersectSpheres[0]->m_index].rbegin() + 1);
							ldir2 = (lorigin2 - *m_centerLines[intersectSpheres[0]->m_index].rbegin());
							ldir2.Normalize();

							conType = 1;
						}						
						else
						{
							lorigin1 = intersectSpheres[1]->m_connectPoint[0];
							ldir1 = lorigin1 - intersectSpheres[1]->m_connectPoint[1];
							ldir1.Normalize();
							lorigin2 = *(m_centerLines[intersectSpheres[1]->m_index].rbegin() + 1);
							ldir2 = (lorigin2 - *m_centerLines[intersectSpheres[1]->m_index].rbegin());
							ldir2.Normalize();

							conType = 2;
						}
					}

					//if(angleBetweenVectors(ldir1, ldir2) > 30)
					{
						Vector3 clp1, clp2;
						lineLineDist(lorigin1, ldir1, lorigin2, ldir2, clp1, clp2);
						clp2 = (clp2 + clp1)/2;
						
						if(!bHasCenter1 && !bHasCenter2)
						{
							avRadius = (intersectSpheres[0]->m_radius + intersectSpheres[1]->m_radius) / 2;
							type = CheckAntlerPartType(toVec(clp2), avRadius, bHasCenter, retCenterPoint, curIndex, false, lst);
							//m_debugPoints.push_back(DebugPoint(toVec(clp2), avRadius, type, false));
							if(bHasCenter)
							{
								avRadius = avRadius - step;
								while(CheckAntlerPartType(toVec(clp2), avRadius, bHasCenter, retCenterPoint, curIndex, false, lst) <= type && bHasCenter)
								{
									avRadius -= step;
								}
								avRadius += step;
								CheckAntlerPartType(toVec(clp2), avRadius, bHasCenter, retCenterPoint, curIndex, false, lst);
							}
							else
							{
								avRadius = avRadius + step;
								while(CheckAntlerPartType(toVec(clp2), avRadius, bHasCenter, retCenterPoint, curIndex, false, lst) <= type && !bHasCenter)
								{
									avRadius += step;
									if(avRadius > m_radius * 5)
										break;
								}
							}
								//bHasCenter = false;
						}
						else
						{
							if(bHasCenter1 && !bHasCenter2)
							{
								retCenterPoint = p1;
								
								avPos = intersectSpheres[0]->m_origin;
								avRadius = intersectSpheres[0]->m_radius;
								type = CheckAntlerPartType(avPos, avRadius, bHasCenter, p1, curIndex, false, lst);
							}
							else
							if(bHasCenter2 && !bHasCenter1)
							{
								retCenterPoint = p2;
									
								avPos = intersectSpheres[1]->m_origin;
								avRadius = intersectSpheres[1]->m_radius;
								type = CheckAntlerPartType(avPos, avRadius, bHasCenter, p2, curIndex, false, lst);
							}
							else
								if(intersectSpheres[0]->m_radius < intersectSpheres[1]->m_radius)
								{
									avPos = intersectSpheres[0]->m_origin;
									avRadius = intersectSpheres[0]->m_radius;
									type = CheckAntlerPartType(avPos, avRadius, bHasCenter, p1, curIndex, false, lst);

									retCenterPoint = p1;
								}
								else
									retCenterPoint = p2;

							bHasCenter = true;
						}

						if(bHasCenter)
						{
							Vector3 resV;
							if(intersectBurrPlane(*m_centerLines[intersectSpheres[0]->m_index].rbegin(), clp2, resV) || 
								intersectBurrPlane(*m_centerLines[intersectSpheres[1]->m_index].rbegin(), clp2, resV) ||
								intersectBurrPlane(clp2, toVector3(retCenterPoint), resV))
							{
								bHasCenter = false;
								// connection invalid
							}
							else
							{
								if((*m_centerLines[intersectSpheres[0]->m_index].rbegin() - toVector3(retCenterPoint)).Length()
									< (*m_centerLines[intersectSpheres[0]->m_index].rbegin() - clp2).Length() ||
									(*m_centerLines[intersectSpheres[1]->m_index].rbegin() - toVector3(retCenterPoint)).Length()
									< (*m_centerLines[intersectSpheres[1]->m_index].rbegin() - clp2).Length())
								{
									//m_centerLines[intersectSpheres[0]->m_index].push_back(clp2);
									//m_centerLines[intersectSpheres[1]->m_index].push_back(clp2);
									/*if(m_isMergePoint[intersectSpheres[0]->m_index])
										m_centerLines[intersectSpheres[0]->m_index].pop_back();
									if(m_isMergePoint[intersectSpheres[1]->m_index])
										m_centerLines[intersectSpheres[1]->m_index].pop_back();*/

									m_centerLines[intersectSpheres[0]->m_index].push_back(toVector3(retCenterPoint));	
									m_centerLines[intersectSpheres[1]->m_index].push_back(toVector3(retCenterPoint));	
								}
								else
								{
									if(m_centerLines[intersectSpheres[0]->m_index].size() > 1 &&
										(*m_centerLines[intersectSpheres[0]->m_index].rbegin() - 
										*(m_centerLines[intersectSpheres[0]->m_index].rbegin() + 1)).Length() >
										(*(m_centerLines[intersectSpheres[0]->m_index].rbegin() + 1) - clp2).Length())
										m_centerLines[intersectSpheres[0]->m_index].pop_back();

									if(m_centerLines[intersectSpheres[1]->m_index].size() > 1 &&
										(*m_centerLines[intersectSpheres[1]->m_index].rbegin() - 
										*(m_centerLines[intersectSpheres[1]->m_index].rbegin() + 1)).Length() >
										(*(m_centerLines[intersectSpheres[1]->m_index].rbegin() + 1) - clp2).Length())
										m_centerLines[intersectSpheres[1]->m_index].pop_back();
									
									if(conType > 0)
									{
										ldir1 = *m_centerLines[intersectSpheres[0]->m_index].rbegin() 
											- *(m_centerLines[intersectSpheres[0]->m_index].rbegin() + 1);
										ldir1.Normalize();
										ldir2 = (toVector3(retCenterPoint) - *m_centerLines[intersectSpheres[0]->m_index].rbegin());
										ldir2.Normalize();

										if(m_isMergePoint[intersectSpheres[0]->m_index] && angleBetweenVectors(ldir1, ldir2) > 25)
											m_centerLines[intersectSpheres[0]->m_index].pop_back();

										ldir1 = *m_centerLines[intersectSpheres[1]->m_index].rbegin() 
											- *(m_centerLines[intersectSpheres[1]->m_index].rbegin() + 1);
										ldir1.Normalize();
										ldir2 = (toVector3(retCenterPoint) - *m_centerLines[intersectSpheres[1]->m_index].rbegin());
										ldir2.Normalize();

										if(m_isMergePoint[intersectSpheres[1]->m_index] && angleBetweenVectors(ldir1, ldir2) > 25)
											m_centerLines[intersectSpheres[1]->m_index].pop_back();
									}

									m_centerLines[intersectSpheres[0]->m_index].push_back(clp2);
									m_centerLines[intersectSpheres[1]->m_index].push_back(clp2);
									m_centerLines[intersectSpheres[0]->m_index].push_back(toVector3(retCenterPoint));	
								}

								break;
							}
						}
					}
				}
				}
			}
		}
		else
		//if(false)
		//if(intersectSpheres.size() > 2)
		{
			
			//bHasCenter = false;
			//MessageBox(0,L"",L"",0);
			type = CheckAntlerPartType(avPos, avRadius, bHasCenter, retCenterPoint, curIndex, false, lst);

			if(bHasCenter)
			{
				radius = avRadius - step;
				while((CheckAntlerPartType(avPos, radius, bHasCenter, retCenterPoint, curIndex, false, lst) == type) && bHasCenter)
				{
					radius -= step;
				}
				radius += step;
				CheckAntlerPartType(avPos, radius, bHasCenter, retCenterPoint, curIndex, false, lst);
				//m_debugPoints.push_back(DebugPoint(avPos, radius, type, false));
				avRadius = radius;
			}
		
			bool bChecked = true;
			int times = 10;
			angle = 45;

			//while(!bChecked && times > 0)
			{
				bool bFind = false;
				vector<int>::iterator lit = lst.begin();
				while(lit != lst.end())
				{
					if(!m_isMergePoint[*lit] && angleBetweenLines(*lit, toVector3(retCenterPoint)) > angle)
					{
						bFind = true;
						break;
					}
					lit++;
				}
				if(bFind)
				{
					avRadius += step;
					type = CheckAntlerPartType(avPos, avRadius, bHasCenter, retCenterPoint, curIndex, false, lst);
					if(!bHasCenter)
					{
						avRadius -= step;
						CheckAntlerPartType(avPos, avRadius, bHasCenter, retCenterPoint, curIndex, false, lst);
					}
					{
						lit = lst.begin();
						iit = intersectSpheres.begin();
						while(lit != lst.end())
						{
							if(!m_isMergePoint[*lit] && angleBetweenLines(*lit, toVector3(retCenterPoint)) > angle)
							{
								lst.erase(lit);
								intersectSpheres.erase(iit);
								lit = lst.begin();	
								iit = intersectSpheres.begin();
							}
							else
							{
								lit++;
								iit++;
							}
						}

						bChecked = lst.size() > 1;
						times = 0;
					}
				}
				else
					bChecked = true;
				times--;
			}

			if(!bChecked)
			{
				bHasCenter = false;
				//isEmptyPass++;
			}
		}

		if(bHasCenter)// && bChecked)
		{
			//markVerticesFromList(curIndex, lst);
			//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
			pass = 0;

			vector<int>::iterator lit = lst.begin();
			vector<int>::iterator liend = lst.end();
			int n = 0;
			while(lit != liend)
			{
				m_isMergePoint[*lit] = true;
				Vector3 resV;

				if(intersectSpheres.size() > 2)
				{
					if(intersectBurrPlane(*m_centerLines[*lit].rbegin(), toVector3(retCenterPoint), resV))
					{
						//MessageBox(0,L"",L"",0);
						m_isIntersected[curIndex] = true;
						m_centerLines[*lit].push_back(resV);
						lst.erase(lit);
						lit = lst.begin() + n;
						liend = lst.end();
						continue;
					}
					else
						m_centerLines[*lit].push_back(toVector3(retCenterPoint));
				}

				++lit;
				++n;
			}

			while(intersectSpheres.size() > 0)
			{
				it = std::find(m_spheres.begin(), m_spheres.end(), *intersectSpheres.rbegin());
				if(it != m_spheres.end())
					m_spheres.erase(it);
				else
					;// report error
				intersectSpheres.pop_back();
			}

			if(lst.size() > 1)
			{
				m_mergeList.push_back(lst);
				scaleRad = 0.3f;
				isEmptyPass = 0;
			}
			else
			{
				isEmptyPass++;
				m_spheres.push_back(*m_spheres.begin());
				m_spheres.pop_front();
				//scaleRad += 0.1f;
				continue;
			}
			markVerticesFromList(curIndex, lst);
			markLastUsedVertices(curIndex);

			centerPoint = retCenterPoint;

			//float retRad = avRadius;

			//radius = avRadius - 10*step;
			//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
			//avRadius = m_radius * 0.5;
			CheckAntlerPartType(centerPoint, avRadius, bHasCenter, retCenterPoint, curIndex);
			if(!bHasCenter)
			{
				radius = avRadius + step;
				while((CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, curIndex) <= 4) && !bHasCenter)
				{
					radius += step;
					if(radius > m_radius * 5)
						break;
				}
			}
			else
			{
				radius = avRadius - step;
				while((CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, curIndex) <= 4) && bHasCenter)
				{
					radius -= step;
				}
				radius += step;
				CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, curIndex);
			}

			bool bSkip = false;
			if(bHasCenter)
			{
				Vector3 resV;
				if(intersectBurrPlane(*m_centerLines[curIndex].rbegin(), toVector3(retCenterPoint), resV))
				{
					//MessageBox(0,L"",L"",0);
					m_isIntersected[curIndex] = true;
					m_centerLines[curIndex].push_back(resV);
					bSkip = true;
				}
				else
				{
					m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
					centerPoint = retCenterPoint;
					m_centerLines[curIndex].push_back(toVector3(retCenterPoint));
					markLastUsedVertices(curIndex);
				}
			}
			else
			{
				m_spheres.push_back(SphereUnit(curIndex, centerPoint, avRadius));
				bSkip = true;
			}

			//radius = avRadius;//scaleRad;
				//min(avRad, m_radius * 1.2f);
			//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
			if(!bSkip)
			while(true)
			{
				type = CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, curIndex);
				//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));

				if(bHasCenter)
				{
					if(type <= 4)
					{
						radius -= step;
						//if(radius >= m_radius * 0.5)
						{
							type = CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, curIndex);
							while(type <= 4 && type > 0 && bHasCenter 
								&& angleBetweenLines(curIndex, toVector3(retCenterPoint))<25)
							{
								radius -= step;
								/*if(radius < m_radius * 0.5)
									break;*/
								type = CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, curIndex);
							}
						}
						radius += step;
						CheckAntlerPartType(centerPoint, radius, bHasCenter, retCenterPoint, curIndex);
					}
					m_isMergePoint[lst[0]] = false;

					Vector3 resV;
					if(intersectBurrPlane(*m_centerLines[lst[0]].rbegin(), toVector3(retCenterPoint), resV))
					{
						//MessageBox(0, L"", L"", 0);
						m_isIntersected[lst[0]] = true;
						m_centerLines[lst[0]].push_back(resV);
						break;
					}
					if(m_centerLines[lst[0]].size() > 0)
					{
						Vector3 dir = toVector3(retCenterPoint) - *m_centerLines[lst[0]].rbegin();
						float len = dir.Length();
						if(len > step)
						{
							dir.Normalize();
							centerPoint = toVec(*m_centerLines[lst[0]].rbegin() + dir * len * 0.5);
						}
						else
							centerPoint = retCenterPoint;
					}
					else
						centerPoint = retCenterPoint;

					m_centerLines[lst[0]].push_back(toVector3(retCenterPoint));
					//centerPoint = retCenterPoint;
					//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
					markLastUsedVertices(lst[0]);
				}
				else
				{
					if(type == 4)
					{
						radius += step;
						if(radius > 4*m_radius)
						{
							//retRad = max(retRad, radius);
							m_spheres.push_back(SphereUnit(curIndex, centerPoint, radius));
							//m_debugPoints.push_back(DebugPoint(centerPoint, radius, 7, false));

							break;
						}

						continue;
					}else
					if(type == 3)
					{
						//retRad = max(retRad, radius);
						m_spheres.push_back(SphereUnit(curIndex, centerPoint, radius));
						//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));

						break;
					}
					else
					{
						//m_debugPoints.push_back(DebugPoint(centerPoint, radius, type, false));
						break;
					}
					
					//else
						//break;
				}
			}
		}
		else
		{
			m_spheres.push_back(*m_spheres.begin());
			m_spheres.pop_front();
			isEmptyPass++;
			pass++;
			continue;
		}

		pass++;
	}

	list<SphereUnit>::iterator ibeg = m_spheres.begin();
	for(; ibeg != m_spheres.end(); ++ibeg)
		m_debugPoints.push_back(DebugPoint(ibeg->m_origin, ibeg->m_radius, 10, false));

	//-----------------------------------
	// organize centerlines into branches
	vector<vector<int>>::iterator mbeg = m_mergeList.begin();
	vector<vector<int>>::iterator mend = m_mergeList.end();
	vector<vector<int>>::iterator mit = mbeg;

	for(; mit != mend; ++mit)
	{
		vector<int> &vlist = *mit;
		vector<int>::reverse_iterator vit = vlist.rbegin();
		vector<int>::reverse_iterator vend = vlist.rend();
		Vector3 &mergePoint = *m_centerLines[*vit].rbegin();
		//Branch *mbranch = NULL;
		bool bFind;
		Branch mainBranch;
		for(; vit != vend; ++vit)
		{
			bFind = false;
			vector<Branch>::iterator bit = m_branches.begin();
			vector<Branch>::iterator bend = m_branches.end();
			for(; bit != bend; ++bit)
				if(bit->m_index == *vit)
				{
					bFind = true;
					mainBranch.m_childs.push_back(fromBranch(*bit, mergePoint));
					m_branches.erase(bit);
					break;
				}

			if(!bFind)
			{
				mainBranch.m_childs.push_back(fromCenterline(*vit, mergePoint));
			}
		}

		parentFromCenterline(mainBranch, *vlist.begin(), mergePoint);
		m_branches.push_back(mainBranch);
	}

	// clear merge list )
	m_mergeList.clear();

	if(m_branches.size() == 2)
	{
		// extend centerline to burr plane
		vector<Branch>::iterator bit = m_branches.begin();
		vector<Branch>::iterator bend = m_branches.end();
		for(; bit != bend; ++bit, ++index)
		{

			if(m_isIntersected.find(bit->m_index) == m_isIntersected.end())
			{
				int plNum;
				if((m_burCenter[0] - *bit->m_points.rbegin()).Length() < 
					(m_burCenter[1] - *bit->m_points.rbegin()).Length())
					plNum = 0;
				else
					plNum = 1;
				Vector3 res = toVector3(GetCenterPoint(m_burPlane[plNum], toVec(m_burCenter[plNum]), m_radius * 1.5));
				//m_debugPoints.push_back(DebugPoint(toVec(m_burCenter[plNum]), m_radius * 1.5, 10, false));
				
				Vector3 a, b;
				if(bit->m_points.size() > 1)
				{
					a = *bit->m_points.rbegin() - *(bit->m_points.rbegin() + 1);
					b = res - *bit->m_points.rbegin();
				}
				else
					a = b = Vector3::UNIT_X;

				if(angleBetweenVectors(a, b) < 45)
					bit->m_points.push_back(res);
				else
				{
					a.Normalize();
					a = *bit->m_points.rbegin() + a * m_radius * 2;
					if(intersectBurrPlane(a, *bit->m_points.rbegin(), res))
						bit->m_points.push_back(res);
				}
			}
		}

		// swap branches if needed
		if((m_burCenter[0] - *m_branches[0].m_points.rbegin()).Length() > 
			(m_burCenter[0] - *m_branches[1].m_points.rbegin()).Length())
			std::swap(m_branches[0], m_branches[1]);

		// connect unused branches
		{
			vector<pair<Vector3, Branch*>> connectList;

			list<SphereUnit>::iterator ibeg = m_spheres.begin();
			list<SphereUnit>::iterator iend = m_spheres.end();
			for(; ibeg != iend; ++ibeg)
				if(m_branches[0].m_index != ibeg->m_index && m_branches[1].m_index != ibeg->m_index)
				{
					//m_debugPoints.push_back(DebugPoint(ibeg->m_origin, ibeg->m_radius, 0, false));
					Vector3 origin = toVector3(ibeg->m_origin);
					connectList.clear();
					getBranchesVsSphere(origin, ibeg->m_radius, &m_branches[0], connectList);
					if(connectList.empty())
						getBranchesVsSphere(origin, ibeg->m_radius, &m_branches[1], connectList);

					if(connectList.size() > 0)
					{
						Vector3 lastPoint = *m_centerLines[ibeg->m_index].rbegin();
						Vector3 dir = lastPoint - *(m_centerLines[ibeg->m_index].rbegin() + 1);
						dir.Normalize();

						float angle = 180;
						float angle2;
						float minAngle = 180;
						float minLen = LONG_MAX;
						Vector3 minV;
						Vector3 point;
						Vector3 resV;

						for(size_t branch_index = 0; branch_index < connectList.size(); branch_index++)
						{
							if(angleBetweenVectors(dir,  -connectList[branch_index].first) > 100)
								continue;

							subdivideBranchInSphere(*connectList[branch_index].second, origin, ibeg->m_radius, 0.3);

							vector<Vector3> &points = connectList[branch_index].second->m_points;
							for(size_t i = 0; i < points.size(); i++)
								if(IsInsideSphere(origin, ibeg->m_radius, points[i]))
								{
									angle2 = angleBetweenVectors(dir,  points[i] - lastPoint);
									if(!intersectBurrPlane(points[i], lastPoint, resV))
									{
										float len = (points[i] - lastPoint).Length();
										if(len < minLen)
										{
											minAngle = angle2;
											minLen = len;
											minV = points[i];
										}

										if(angle > angle2)
										{
											index = branch_index;
											angle = angle2;
											point = points[i];
										}
									}
								}

							// second pass :)
							for(size_t i = 0; i < points.size(); i++)
								if(IsInsideSphere(origin, ibeg->m_radius, points[i]))
								{
									angle2 = angleBetweenVectors(dir,  points[i] - lastPoint);
									if(!intersectBurrPlane(points[i], lastPoint, resV))
									{
										float len = (points[i] - lastPoint).Length();
										if(len < minLen * 1.2 && angle2 < minAngle)
										{
											minAngle = angle2;
											minV = points[i];
										}
									}

								}
						}

						if(angle < 100)
						{
							angle2 = angleBetweenVectors(dir,  minV - lastPoint);
							if(angle2 < angle * 1.2)
								point = minV;

							m_centerLines[ibeg->m_index].push_back(point);
							splitBranch(*connectList[index].second, point);
							connectList[index].second->m_childs.push_back(fromCenterline(ibeg->m_index, point));

							//m_debugPoints.push_back(DebugPoint(ibeg->m_origin, ibeg->m_radius, 0, false));
						}
					}
				}
		}

		//------------------------
		// generate branch details (depth, length, etc)
		// prerape vectors<>
		index = 0;
		bit = m_branches.begin();
		bend = m_branches.end();
		for(; bit != bend; ++bit, ++index)
		{
			bit->m_index = index;
			m_tipList.push_back(vector<Branch*>());
			generateBranchDetails(*bit);
		}

		//-------------------------------------------------------------------
		// find symmetry plane + up, right, forward vectors + rotation matrix
		if(!m_branches[0].m_childs.empty() && !m_branches[1].m_childs.empty())
		{
			Vector3 center = (*m_branches[0].m_points.rbegin() + *m_branches[1].m_points.rbegin())/2.0f;
			Vector3 left = *m_branches[0].m_points.rbegin() - *m_branches[1].m_points.rbegin();
			left.Normalize();

			Vector3 up;
			//up.Normalize();
			Plane leftPl(left, center);
			up = (*m_branches[0].m_points.begin() - center).NormalizedCopy();
			up = (*m_branches[1].m_points.begin() - center).NormalizedCopy();
			up.Normalize();
			up = leftPl.ProjectVector(up);
			up.Normalize(); // ??

			Vector3 front = up.UnitCross(left);
			Plane frontPl(front, center);
			int sideOfPlane = 0;

			for(int i = 0; i < 2; i++)
				for(size_t j = 0; j < m_tipList[i].size(); j++)
				{
					Side sd = frontPl.GetSide(*m_tipList[i][j]->m_points.begin());
					if(sd == POSITIVE_SIDE)
						sideOfPlane++;
					else
						if(sd == NEGATIVE_SIDE)
							sideOfPlane--;
				}

			if(sideOfPlane < 0)
			{
				front = -front;
				left = up.UnitCross(front);
			}


			// show antler axes
			/*m_centerLines.push_back(LineStrip());
			m_centerLines.rbegin()->push_back(center - up * 1000);
			m_centerLines.rbegin()->push_back(center);
			m_centerLines.rbegin()->push_back(center + left * 100);
			
			m_centerLines.push_back(LineStrip());
			m_centerLines.rbegin()->push_back(center + front * 100);
			m_centerLines.rbegin()->push_back(center);*/
			

			Matrix3 mat;
			mat.SetRow(0, left);
			mat.SetRow(1, up);
			mat.SetRow(2, front);
			Matrix3 rotX;
			rotX.FromAxisAngle(Vector3::UNIT_X, Math::HALF_PI / 4.0f);
			mat = rotX * mat;

			/*Matrix4 m4 = Matrix4::IDENTITY;
			m4 = mat;
			m_triMesh->SetNormalMatrix(m4);*/
			calcWorldPoints(m_branches[0], mat);
			calcWorldPoints(m_branches[1], mat);
		}

		// --------------------
		// identify antler tips
		for(int i = 0; i < 2; i++)
		{
			size_t ind = 0;
			//Vector3 nc = *(m_tipList[i][0]->m_worldPoints.begin());
			Vector3 nc = m_tipList[i][0]->m_begPoint;
			//Vector3 worldPos = *(m_tipList[i][0]->m_worldPoints.begin());
			Vector3 worldPos = *(m_tipList[i][0]->m_points.begin());
			Vector3 worldPos2;
			Vector3 np;

			// find main beam
			// pass 1: find closest one to viewer
			for(size_t j = 1; j < m_tipList[i].size(); j++)
			{
				//np = *(m_tipList[i][j]->m_worldPoints.begin());
				np = m_tipList[i][j]->m_begPoint;
				if(np.z > nc.z)
				{
					nc = np;
					//worldPos = *(m_tipList[i][j]->m_worldPoints.begin());
					worldPos = *(m_tipList[i][j]->m_points.begin());
					ind = j;
				}
			}

			// pass 2: find closest to symmetry plane and most bottom one )
			for(size_t j = 0; j < m_tipList[i].size(); j++)
			{
				//np = *(m_tipList[i][j]->m_worldPoints.begin());
				np = m_tipList[i][j]->m_begPoint;
				//if(np.z > nc.z || abs(np.z - nc.z) < m_radius * 8 && np.y < nc.y)
				//if( ((min(np.z, nc.z) * 1.4 >= max(np.z, nc.z)) && (abs(np.x) < abs(nc.x)))&&(np.y > nc.y) )
				if( (min(np.z, nc.z) * 1.4 >= max(np.z, nc.z)) && 
					(abs(np.x) < abs(nc.x))&&
					np.y > nc.y)
				{
					//if(np.y < 0 && nc.y > 0 && np)
					{
						nc = np;
						//worldPos = *(m_tipList[i][j]->m_worldPoints.begin());
						worldPos = *(m_tipList[i][j]->m_points.begin());
						ind = j;
					}
				}
			}

			m_tipList[i][ind]->m_used = true;
			m_sortedTips[i].push_back(m_tipList[i][ind]);
			//m_annotations[i].push_back(nc);
			m_annotations[i].push_back(worldPos);


			// identify other tips G1, G2 etc....
			// if length < 1.5 inches than ignore tip
			Branch* curBranch = m_tipList[i][ind]->m_parent;
			while(curBranch)
			{
				curBranch->m_used = true;
				//curBranch = curBranch->m_parent;
				vector<Branch>::iterator ibeg = curBranch->m_childs.begin();
				vector<Branch>::iterator iend = curBranch->m_childs.end();
				vector<Branch>::iterator it = ibeg;
				vector<Branch>::iterator itused;
				int iUsedCount = 0;
				bool bFirst = true;
				while(true)
				{
					if(!it->m_used)
					{
						if(bFirst)
						{
							//nc = *(it->m_worldPoints.begin());
							nc = it->m_begPoint;
							//worldPos = *(it->m_worldPoints.begin());
							worldPos = *(it->m_points.begin());
							itused = it;
							bFirst = false;
						}
						else
						{
							np = it->m_begPoint;
							//worldPos2 = *(it->m_worldPoints.begin());
							worldPos2 = *(it->m_points.begin());
							//np = *(it->m_worldPoints.begin());
							if(np.z > nc.z)
							{
								nc = np;
								worldPos = worldPos2;
								itused = it;
							}
						}
					}
					else
						iUsedCount++;

					it++;

					if(iUsedCount >= curBranch->m_childs.size())
						break;
					if(it == iend)
					{
						if(bFirst)
							break;
						if(itused != iend)
						{
							if(!itused->m_childs.empty())
							{
								curBranch = &itused->m_childs[0];
								break;
							}
							m_sortedTips[i].push_back(&(*itused));
							itused->m_used = true;
						}
						//m_annotations[i].push_back(nc);
						m_annotations[i].push_back(worldPos);
						iUsedCount = 0;
						bFirst = true;
						it = ibeg;
					}
				}
				curBranch = curBranch->m_parent;
			}
		}


		// ----------------------------------
		// smooth centerlines

		subdivideBranchDepth(m_branches[0], 0.3);
		subdivideBranchDepth(m_branches[1], 0.3);
		subdivideBranch(m_branches[0], 0.3);
		subdivideBranch(m_branches[1], 0.3);

		for(int p = 0; p < 2; p++)
		{
			for(int i = 0; i < 2; i++)
			{
				for(size_t k = 0; k <= m_sortedTips[i].size(); k++)
				{
					int pointCount;
					Branch* curBranch;
					if(k == m_sortedTips[i].size())
						curBranch = m_sortedTips[i][0];
					else
						curBranch = m_sortedTips[i][k];

					while (curBranch)
					{
						if(curBranch->m_parent == NULL)
							pointCount = 2;
						else 
							pointCount = 3;

						for(size_t j = 0; j + pointCount < curBranch->m_points.size(); j++)
							smoothPoints(curBranch->m_points[j], curBranch->m_points[j+1], curBranch->m_points[j+2]);

						if(curBranch->m_parent != NULL)
						{
							Vector3* parentPoint[4];
							Branch* dublPoint[4];
							int curPoint = 0;
							int curDublPoint = 0;
							int j = (int)curBranch->m_points.size() - 3;
							if(j < 0) 
								j = 0;

							for(; j + 1 < curBranch->m_points.size(); j++)
							{
								parentPoint[curPoint] = &curBranch->m_points[j];
								curPoint++;
							}

							Branch* parent = curBranch->m_parent;
							while(parent && curPoint < 4)
							{
								if(parent->m_parent == NULL)
									pointCount = 0;
								else 
									pointCount = 1;

								dublPoint[curDublPoint] = parent;
								curDublPoint++;
								for(j = 0; j + pointCount < parent->m_points.size(); j++)
								{
									parentPoint[curPoint] = &parent->m_points[j];
									curPoint++;
									if(curPoint == 4)
										break;
								}
								parent = parent->m_parent;
							}
							
							if(curPoint == 4)
							{
								smoothPoints(*parentPoint[0], *parentPoint[1], *parentPoint[2]);
								smoothPoints(*parentPoint[1], *parentPoint[2], *parentPoint[3]);
								for(int cr = 0; cr < curDublPoint; cr++)
									for(size_t cc = 0; cc < dublPoint[cr]->m_childs.size(); cc++)
									{
										(*dublPoint[cr]->m_childs[cc].m_points.rbegin()) =
											dublPoint[cr]->m_points[0];
									}
							}
						}
						curBranch = curBranch->m_parent;
					}
				}
			}

			if(p == 0)
			{
				subdivideBranchDepth(m_branches[0], 0.6);
				subdivideBranchDepth(m_branches[1], 0.6);
			}
			if(p == 1)
			{
				subdivideBranchDepth(m_branches[0], 0.3);
				subdivideBranchDepth(m_branches[1], 0.3);
			}
		}/**/

		updateBranchLength(m_sortedTips[0][0], m_sortedTips[1][0]);

		// reorder G branches for whitetail
		for(int i = 0; i < 2; i++)
		{
			std::vector<Vector3> tmpList = m_annotations[i];
			m_tipList[i] = m_sortedTips[i];
			m_sortedTips[i].clear();
			m_annotations[i].clear();
			m_sortedTips[i].push_back(m_tipList[i][0]);
			m_annotations[i].push_back(tmpList[0]);

			for(size_t k = m_tipList[i].size() - 1; k > 0; k--)
			{
				m_sortedTips[i].push_back(m_tipList[i][k]);
				m_annotations[i].push_back(tmpList[k]);
			}
		}

		// calculcate widest spread (D value)
		m_dims.push_back(hausdorffDistBetweenMainBeams());

		// calculcate C value
		Vector3 p1 = Vector3::ZERO;
		Vector3 p2 = Vector3::ZERO;
		Vector3 p1_ = Vector3::ZERO;
		Vector3 p2_ = Vector3::ZERO;
		Vector3 p;
		float len;

		for(int i = 0; i < 2; i++)
		{
			for(size_t k = 0; k < m_sortedTips[i].size(); k++)
			{
				Branch* curBranch = m_sortedTips[i][k];
				while (curBranch)
				{
					for(size_t j = 0; j < curBranch->m_points.size(); j++)
					{
						p = m_triMesh->_worldMatrix.TransformAffine(curBranch->m_points[j]);
						if(p1.x > p.x)
						{
							p1 = p;
							p1_ = curBranch->m_points[j];
						}

						if(p2.x < p.x)
						{
							p2 = p;
							p2_ = curBranch->m_points[j];
						}
					}
					curBranch = curBranch->m_parent;
				}
			}
		}

		len = abs(p2.x - p1.x) / m_triMesh->_ratio;
		//m_dims.push_back(DimUnit(p1, p2, len));
		//len = abs(p2_.x - p1_.x);
		m_dims.push_back(DimUnit(p1_, p2_, len));

		// calculate B value
		p1 = m_tipList[0][0]->m_points[0];
		p2 = m_tipList[1][0]->m_points[0];
		len = (p2 - p1).Length();
		//p1 = m_triMesh->_worldMatrix.TransformAffine(p1);
		//p2 = m_triMesh->_worldMatrix.TransformAffine(p2);
		m_dims.push_back(DimUnit(p1, p2, len));

        // convert to inches
        convertLengthToInches();

		for(size_t i = 0; i < m_dims.size(); i++)
		{
			m_dims[i].m_distance /= 25.4;
			//round
			m_dims[i].m_distance = ROUND2(m_dims[i].m_distance);
		}

	}

	m_mesh->remove_property( m_meshTag );
	m_mesh->remove_property( m_markTag );
}

std::pair<bool, Real> CenterLine::GetIntersection(const Vector3 &origin, const Vector3 &direction, float radius)
{
	MeshType::VertexIter vEnd =  m_mesh->vertices_end();
	for (MeshType::VertexIter it = m_mesh->vertices_begin(); it != vEnd; ++it)
	{
		m_mesh->status(it).set_tagged(IsInsideSphere(toVec(origin), radius, m_mesh->point(it)));
	}
	
	Ray ray(origin, direction);
	Vector3 v[3];
	bool bContinue;
	std::pair<bool, Real> result;
	float len = Math::POS_INFINITY;

	MeshType::FaceIter fi; //iterator for faces
	MeshType::FaceIter fe = m_mesh->faces_end(); //faces end
	for (fi = m_mesh->faces_begin(); fi != fe; ++fi)
	{
		bContinue = false;
		MeshType::FaceVertexIter fvi = m_mesh->fv_iter(fi);
		for (int i = 0; i < 3; i++, ++fvi)
		{
			if(!m_mesh->status(fvi).tagged())
			{
				bContinue = true;
				break;
			}

			v[i] = toVector3(m_mesh->point(fvi));
		}

		if(bContinue)
			continue;

		result = Math::intersects(ray, v[0], v[1], v[2]);
		if(result.first && result.second < len)
			len = result.second;
	}

	if(len < Math::POS_INFINITY)
		return std::pair<bool, Real>(true, len);
	else
		return std::pair<bool, Real>(false, 0);
}

void CenterLine::TrimBranchToMeasurementPoint(Branch* curBranch, Branch* prevBranch, bool bReloadCenterlines)
{
	for(size_t i = 0; i < curBranch->m_childs.size(); i++)
		if(&curBranch->m_childs[i] != prevBranch)	
		{
			float len = 0;
			vector<Vector3>::reverse_iterator cit = curBranch->m_childs[i].m_points.rbegin();
			vector<Vector3>::reverse_iterator cend = curBranch->m_childs[i].m_points.rend();
			vector<Vector3>::reverse_iterator cit2 = cit;
			++cit2;
			Vector3 dir;
			Vector3 resDir = Vector3::ZERO;

			while(cit2 != cend && len < m_radius * 2)
			{
				dir = *cit2 - *cit;
				resDir += dir.NormalizedCopy();
				len += (*cit - *cit2).Length();

				cit = cit2;
				++cit2;
			}

			resDir.Normalize();

			/*m_centerLines.push_back(LineStrip());
			m_centerLines.rbegin()->push_back(*curBranch->m_childs[i].m_points.rbegin());
			m_centerLines.rbegin()->push_back(*curBranch->m_childs[i].m_points.rbegin() + resDir * 100);*/

			// -------------------------------
			cit = prevBranch->m_points.rbegin();
			cend = prevBranch->m_points.rend();
			cit2 = cit;
			++cit2;
			std::pair<bool, Real> res;
			std::pair<bool, Vector3> res1, res2;
			float len1 = Math::POS_INFINITY;
			len = 0;

			while(cit2 != cend && len < m_radius * 3)
			{
				dir = *cit2 - *cit;
				len += (*cit - *cit2).Length();

				res = GetIntersection(*cit2, resDir, 3 * m_radius);
				if(res.first && res.second < len1)
				{
					len1 = res.second;
					res1 = std::pair<bool, Vector3>(true, *cit2 + resDir * len1);
				}

				cit = cit2;
				++cit2;
			}

			if(len1 == Math::POS_INFINITY)
				res1 = std::pair<bool, Vector3>(false, Vector3::ZERO);


			// -------------------------------
			vector<Vector3>::iterator citf = curBranch->m_points.begin();
			vector<Vector3>::iterator cendf = curBranch->m_points.end();
			vector<Vector3>::iterator citf2 = citf;
			citf2 = citf;
			++citf2;
			float len2 = Math::POS_INFINITY;
			len = 0;

			while(citf2 != cendf && len < m_radius * 3)
			{
				dir = *citf2 - *citf;
				len += (*citf - *citf2).Length();

				res = GetIntersection(*citf2, resDir, 3 * m_radius);
				if(res.first && res.second < len2)
				{
					len2 = res.second;
					res2 = std::pair<bool, Vector3>(true, *citf2 + resDir * len2);
				}

				citf = citf2;
				++citf2;
			}

			if(len2 == Math::POS_INFINITY)
				res2 = std::pair<bool, Vector3>(false, Vector3::ZERO);

			if(res1.first && !res2.first)
			{
				res2 = res1;
				len2 = len1;
			}
			else
			if(!res1.first && res2.first)
			{
				res1 = res2;
				len1 = len2;
			}

			if(res1.first && res2.first)
			{
				/*m_centerLines.push_back(LineStrip());
				m_centerLines.rbegin()->push_back(res1.second);
				m_centerLines.rbegin()->push_back(res2.second);*/

				Vector3 origin = *curBranch->m_childs[i].m_points.rbegin();
				float radius = (len1 + len2) / 2;
				//m_debugPoints.push_back(DebugPoint(toVec(origin), radius, 10, false));

				cit = curBranch->m_childs[i].m_points.rbegin();
				cend = curBranch->m_childs[i].m_points.rend();
				cit2 = cit;
				++cit2;
				len = 0;

				Vector3 v1 = *cit;
				Vector3 v2 = *cit2;

				if(bReloadCenterlines)
				{
					m_centerLines.push_back(LineStrip());
					m_centerLines.rbegin()->push_back(v1);
				}
				curBranch->m_childs[i].m_extPoints.push_back(v1);

				while(cit2 != cend && len < m_radius * 3)
				{
					len += (*cit - *cit2).Length();

					if(IsInsideSphere(origin, radius, v2))
					{
						if(bReloadCenterlines)
							m_centerLines.rbegin()->push_back(v2);
						curBranch->m_childs[i].m_extPoints.push_back(v2);

						curBranch->m_childs[i].m_points.pop_back();
						cit = curBranch->m_childs[i].m_points.rbegin();
						cend = curBranch->m_childs[i].m_points.rend();
						++cit2;
						if(cit2 == cend)
							break;

						v1 = v2;
						v2 = *cit2;

						continue;
					}
					else
					{
						Vector3 res = GetIntersectionPoint(origin, radius, v1, v2);
						curBranch->m_childs[i].m_points.pop_back();
						curBranch->m_childs[i].m_points.push_back(res);

						if(bReloadCenterlines)
							m_centerLines.rbegin()->push_back(v2);
						curBranch->m_childs[i].m_extPoints.push_back(v2);
						break;
					}

					cit = cit2;
					++cit2;

					v1 = v2;
					v2 = *cit2;
				}

				curBranch->m_childs[i].m_extPoints.pop_back();
			}
		}
}

void CenterLine::updateBranchLength(Branch* mainBeam_l, Branch* mainBeam_r, bool bInit)
{
	bool bReloadCenterlines = mainBeam_l && mainBeam_r;

	if(bReloadCenterlines)
	{
		for(size_t i = 0; i < m_centerLines.size(); i++)
			m_centerLines[i].clear();

		m_centerLines.clear();
	}

	//curBranch->m_childs[i].m_extPoints.push_back(v1);

	Branch* mainBeam[2] = {mainBeam_l, mainBeam_r};
	Branch* startBranch[2] = {mainBeam_l, mainBeam_r};
	for(int i = 0; i < 2; i++)
		if(startBranch[i])
		{
			while(startBranch[i]->m_parent)
				startBranch[i] = startBranch[i]->m_parent;

			reconstructBranch(*startBranch[i]);
		}

	for(int index = 0; index < 2; ++index)
	{
		if(!mainBeam[index])
			continue;

		Branch* prevBranch = mainBeam[index]; 
		Branch* curBranch = prevBranch->m_parent;
		if(curBranch)
		{
			//MessageBox(0, L"", L"", 0);
			//Branch* nextBranch = curBranch->m_parent;
			while(curBranch && prevBranch)
			{
				TrimBranchToMeasurementPoint(curBranch, prevBranch, bReloadCenterlines);
				prevBranch = curBranch;
				curBranch = curBranch->m_parent;
			}
		}
		// previous calculation... 
		calcBranchLength(*prevBranch);

		prevBranch = mainBeam[index]; 
		curBranch = prevBranch->m_parent;

		// set mark E to false
		markAsE(startBranch[index], NULL, false);

		// find E branches
		while(curBranch && prevBranch)
		{
			for(size_t i = 0; i < curBranch->m_childs.size(); i++)
				if(&curBranch->m_childs[i] != prevBranch)
				{
					Branch* cur = &curBranch->m_childs[i];
					while(cur)
					{
						Vector3 mainDir;
						if(cur->m_points.size() <= 1)
						{
							if(cur->m_childs.size() > 0)
								mainDir = Vector3(0,0,0); // just ... set something
							else
								break;
						}
						else
							mainDir = *(cur->m_points.begin() + 1) - *cur->m_points.begin();
						mainDir.Normalize();

						Branch* notEbr = NULL;
						float angle = 90;
						float length = 0;
						for(size_t j = 0; j < cur->m_childs.size(); j++)
						{
							Branch* child = &cur->m_childs[j];
							Vector3 childDir = *child->m_points.rbegin() - *(child->m_points.rbegin() + 1);
							childDir.Normalize();

							float ang = angleBetweenVectors(mainDir, childDir);
							if(!notEbr)
							{
								notEbr = child;
								angle = ang;
							}
							else
								if((ang < angle && 1.3 * child->m_length > notEbr->m_length ||
									child->m_length > 1.3 * notEbr->m_length) && notEbr->m_childs.size() <= 1 ||
									//child->m_length > notEbr->m_length && child->m_childs.size() > 1 ||
									child->m_childs.size() > 1 && notEbr->m_childs.size() <= 1 ||
									ang < angle && child->m_childs.size() > 1 && notEbr->m_childs.size() > 1
									)
								{
									notEbr = child;
									angle = ang;
								}
						}
					
						if(notEbr)
							markAsE(cur, notEbr);
						cur = notEbr;
					}
				}

			prevBranch = curBranch;
			curBranch = curBranch->m_parent;
		}

		// trim to measurement point
		curBranch = mainBeam[index];
		while(curBranch)
		{
			curBranch->m_length = curBranch->m_lengthBurr;
			curBranch->m_used = true;
			curBranch = curBranch->m_parent;
		}

		// for not E branches
		for(size_t tip = 0; tip < m_tipList[index].size(); tip++)
			if(m_tipList[index][tip] != mainBeam[index] && !m_tipList[index][tip]->m_isE)
			{
				//curBranch = m_tipList[index][tip]->m_parent;
				prevBranch = m_tipList[index][tip];
				curBranch = prevBranch->m_parent;
				while(curBranch && !curBranch->m_used)
				{
					TrimBranchToMeasurementPoint(curBranch, prevBranch, bReloadCenterlines);

					curBranch->m_used = true;
					prevBranch = curBranch;
					curBranch = curBranch->m_parent;
				}

			}

		// for E branches
		for(size_t tip = 0; tip < m_tipList[index].size(); tip++)
			if(m_tipList[index][tip] != mainBeam[index] && m_tipList[index][tip]->m_isE)
			{
				//curBranch = m_tipList[index][tip]->m_parent;
				prevBranch = m_tipList[index][tip];
				curBranch = prevBranch->m_parent;
				while(curBranch && !curBranch->m_used)
				{
					TrimBranchToMeasurementPoint(curBranch, prevBranch, bReloadCenterlines);

					curBranch->m_used = true;
					prevBranch = curBranch;
					curBranch = curBranch->m_parent;
				}

			}

		// final calculation
		calcBranchLength(*prevBranch);

		curBranch = mainBeam[index];
		while(curBranch)
		{
			curBranch->m_length = curBranch->m_lengthBurr;
			curBranch->m_used = true;
			curBranch = curBranch->m_parent;
		}

		// adjust length
		// for not E branches
		for(size_t tip = 0; tip < m_tipList[index].size(); tip++)
			if(m_tipList[index][tip] != mainBeam[index] && !m_tipList[index][tip]->m_isE)
			{
				curBranch = m_tipList[index][tip]->m_parent;
				while(curBranch && !curBranch->m_used)
				{
					m_tipList[index][tip]->m_length += curBranch->m_length;
					curBranch->m_used = true;
					curBranch = curBranch->m_parent;
				}

			}

		// for E branches
		for(size_t tip = 0; tip < m_tipList[index].size(); tip++)
			if(m_tipList[index][tip] != mainBeam[index] && m_tipList[index][tip]->m_isE)
			{
				curBranch = m_tipList[index][tip]->m_parent;
				while(curBranch && !curBranch->m_used)
				{
					m_tipList[index][tip]->m_length += curBranch->m_length;
					curBranch->m_used = true;
					curBranch = curBranch->m_parent;
				}

			}
	}

	// ----------------------------------
	// fill centerline list for rendering
	/*for(size_t i = 0; i < m_centerLines.size(); i++)
		m_centerLines[i].clear();

	m_centerLines.clear();*/
	if(bReloadCenterlines)
	{
		m_centerLines.push_back(LineStrip());

		int index = m_centerLines.size() - 1;
		for(int i = 0; i < 2; i++)
		{
			for(size_t k = 0; k < m_sortedTips[i].size(); k++)
			{
				Branch* curBranch = m_sortedTips[i][k];
				while (curBranch)
				{
					for(size_t j = 0; j + 1 < curBranch->m_points.size(); j++)
					{
						m_centerLines[index].push_back(curBranch->m_points[j]);
					}

					//if(curBranch->m_parent == NULL || *curBranch->m_points.rbegin() == *curBranch->m_parent->m_points.begin())
						m_centerLines[index].push_back(*curBranch->m_points.rbegin());

					m_centerLines.push_back(LineStrip());
					index++;

					curBranch = curBranch->m_parent;
				}
			}
		}
	}
    if(!bInit)
	    convertLengthToInches(mainBeam_l != NULL, mainBeam_r != NULL, bInit);
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::reconstructBranch(Branch& br)
{
	std::vector<Vector3>::reverse_iterator it = br.m_extPoints.rbegin();
	std::vector<Vector3>::reverse_iterator iend = br.m_extPoints.rend();
	
	for(;it != iend; ++it)
		br.m_points.push_back(*it);

	br.m_extPoints.clear();
	br.m_used = false;

	std::vector<Branch>::iterator bit = br.m_childs.begin();
	std::vector<Branch>::iterator bend = br.m_childs.end();

	for(; bit != bend; ++bit)
		reconstructBranch(*bit);
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::convertLengthToInches(bool bLeftBranch, bool bRightBranch, bool bInit)
{
	bool bConvert[2] = { bLeftBranch, bRightBranch };
	for(int i = 0; i < 2; i++)
	{
        if(!bConvert[i])
            continue;

        if(bInit)
            m_tips[i].clear();

		int eCount = 0;
		for(size_t j = 0; j < m_sortedTips[i].size(); j++)
		{
			m_sortedTips[i][j]->m_length /= 25.4;
			m_sortedTips[i][j]->m_lengthBurr /= 25.4;

			
			// round
			m_sortedTips[i][j]->m_lengthBurr = ROUND2(m_sortedTips[i][j]->m_lengthBurr);
			m_sortedTips[i][j]->m_length = ROUND2(m_sortedTips[i][j]->m_length);

            double length = 0;
            if(bInit)
            {
                length = (j != 0 ? m_sortedTips[i][j]->m_length : m_sortedTips[i][j]->m_lengthBurr);
                if (j < AntlerTip::maxTips)
                {
					if(j != 0 && (length <= 1.5 || m_sortedTips[i][j]->m_isE))
					{
						m_tips[i].push_back(AntlerTip(m_annotations[i][j], length, AntlerTip::nameList[0], m_sortedTips[i][j]));
						eCount++;
					}
					else
						m_tips[i].push_back(AntlerTip(m_annotations[i][j], length, AntlerTip::nameList[j+1-eCount], m_sortedTips[i][j]));
                }
            }
            else
            {
                length = (m_tips[i][j].m_name != "F" ? m_sortedTips[i][j]->m_length : m_sortedTips[i][j]->m_lengthBurr);
                m_tips[i][j].m_length = length;
            }
        }
    }
}
//--------------------------------------------------------------------------------------------------------
double CenterLine::pointEdgeDist(const Vector3 &a, const Vector3 &b1, const Vector3 &b2) 
{
	Vector3 v12, v21, v1a, v2a;
	double dotProduct1, dotProduct2;
	v12 = b2 - b1; // vector from b1 to b2
	v21 = b1 - b2; // vector from b2 to b1
	v1a = a - b1;  // vector from v1 to a
	v2a = a - b2; // vector from v2 to a

	// dotProduct1 is between v12 and v1a
	//dotProduct1 = v12._x * v1a._x + v12._y * v1a._y
	dotProduct1 = v12.Dot(v1a);

	// dotProduct2 is between v21 and v2a
	//dotProduct2 = v21._x * v2a._x + v21._y * v2a._y;
	dotProduct2 = v21.Dot(v2a);

	if(dotProduct1 > 0 && dotProduct2 > 0) { //they're in the line segment
		double costheta, theta, sintheta;
		//costheta = dotProduct1/(sqrt(v12._x * v12._x+ v12._y * v12._y) * sqrt( v1a._x * v1a._x+ v1a._y * v1a._y));
		costheta = dotProduct1/(v12.Length() * v1a.Length());
		theta = acos(costheta);
		sintheta = sin(theta);
		return v1a.Length() * sintheta;
		//return sqrt( v1a._x * v1a._x+ v1a._y * v1a._y) * sintheta;
	} else if(dotProduct1 >= 0) {
		//return sqrt(v2a._x * v2a._x + v2a._y * v2a._y);
		return v2a.Length();
	} else if(dotProduct2 >= 0) {
		//return sqrt(v1a._x * v1a._x + v1a._y * v1a._y);
		return v1a.Length();
	} else
		return LONG_MAX; // can never happen, but put in here just in case
}

DimUnit CenterLine::hausdorffDistBetweenMainBeams() 
{
	double dist;
	size_t i, j;
	 
	Vector3* a;
	Vector3* v1;
	Vector3* v2;

	DimUnit res, tmp;
	res.m_distance = LONG_MIN;

	if(m_sortedTips[0].empty() || m_sortedTips[1].empty())
		return res;

	vector<Vector3> curve[2];

	subdivideBranchDepth(m_branches[0], 0.1);
	subdivideBranchDepth(m_branches[1], 0.1);
	float minZ = LONG_MAX;
	float maxZ = 0;
	float minY = LONG_MAX;
	float maxY = 0;

	for(int i = 0; i < 2; i++)
	{
		Branch* curBranch = m_sortedTips[i][0];
		while (curBranch)
		{
			for(size_t j = 0; j + 1 < curBranch->m_points.size(); j++)
			{
				curve[i].push_back(curBranch->m_points[j]);

				if(curBranch->m_worldPoints[i].z < minZ)
					minZ = curBranch->m_worldPoints[i].z;
				if(curBranch->m_worldPoints[i].z > maxZ)
					maxZ = curBranch->m_worldPoints[i].z;
				
				if(curBranch->m_worldPoints[i].y < minY)
					minY = curBranch->m_worldPoints[i].y;
				if(curBranch->m_worldPoints[i].y > maxY)
					maxY = curBranch->m_worldPoints[i].y;
			}

			if(curBranch->m_parent == NULL || *curBranch->m_points.rbegin() != *curBranch->m_parent->m_points.begin())
				curve[i].push_back(*curBranch->m_points.rbegin());

			curBranch = curBranch->m_parent;
		}
	}


	//cout << "\nComputing Hausdorff distance...\n";
	size_t len = curve[1].size();

	Vector3 world[2];
	float stepZ = abs(maxZ - minZ) * 0.05;
	float stepY = abs(maxY - minY) * 0.05;
	
	double deltaZ = stepZ * 1; 
	double deltaY = stepY * 4; 
	
	while(res.m_distance == LONG_MIN)
	{
		deltaZ += stepZ;
		deltaY += stepY;

		for(i=0; i<curve[0].size();i++) 
		{
			tmp.m_distance = LONG_MAX;
			for(j=0; j<curve[1].size(); j++) 
			{
				a = &curve[0][i];
				v1 = &(curve[1][j%len]);
				v2 = &(curve[1][(j+1)%len]);
				dist = pointEdgeDist(*a, *v1, *v2);

				world[0] = m_triMesh->_worldMatrix.TransformAffine(*a);
				world[1] = m_triMesh->_worldMatrix.TransformAffine(*v1);

				if(dist < tmp.m_distance && (abs(world[0].z - world[1].z) < deltaZ)
					&& (abs(world[0].y - world[1].y) < deltaY))
				{
					tmp.m_distance = dist;
					tmp.m_point[0] = *a;
					tmp.m_point[1] = *v1;
				}
			}
			if (tmp.m_distance != LONG_MAX && tmp.m_distance > res.m_distance) 
			{
				res = tmp;
			}
		}
	}

	/*for(int i = 0; i < 2; i++)
	{
		res.m_point[i] = m_triMesh->_worldMatrix.TransformAffine(res.m_point[i]);
	}

	if(res.m_point[0].x > res.m_point[1].x)
	{
		Vector3 swap = res.m_point[0];
		res.m_point[0] = res.m_point[1];
		res.m_point[1] = swap;
	}*/

	return res;
}
//--------------------------------------------------------------------------------------------------------
float CenterLine::lineLineDist(const Vector3 &lorigin1, const Vector3 &ldir1, const Vector3 &lorigin2, const Vector3 &ldir2,
	Vector3& kClosestPoint0, Vector3& kClosestPoint1)
{
	Vector3 kDiff = lorigin1 - lorigin2;
	Real fA01 = -ldir1.Dot(ldir2);
	Real fB0 = kDiff.Dot(ldir1);
	Real fC = kDiff.SquaredLength();
	Real fDet = Math::FAbs((Real)1.0 - fA01*fA01);
	Real fB1, fS0, fS1, fSqrDist;

	if (fDet >= Math::ZERO_TOLERANCE)
	{
		// lines are not parallel
		fB1 = -kDiff.Dot(ldir2);
		Real fInvDet = ((Real)1.0)/fDet;
		fS0 = (fA01*fB1-fB0)*fInvDet;
		fS1 = (fA01*fB0-fB1)*fInvDet;
		fSqrDist = fS0*(fS0+fA01*fS1+((Real)2.0)*fB0) +
			fS1*(fA01*fS0+fS1+((Real)2.0)*fB1)+fC;
	}
	else
	{
		// lines are parallel, select any closest pair of points
		fS0 = -fB0;
		fS1 = (Real)0.0;
		fSqrDist = fB0*fS0+fC;
	}

	kClosestPoint0 = lorigin1 + fS0*ldir1;
	kClosestPoint1 = lorigin2 + fS1*ldir2;
	/*m_fLine0Parameter = fS0;
	m_fLine1Parameter = fS1;*/
	return Math::Sqrt(Math::FAbs(fSqrDist));
}
//--------------------------------------------------------------------------------------------------------
float CenterLine::lineSegmentDist(const Vector3 &lorigin, const Vector3 &ldir,
		const Vector3 &sorigin, const Vector3 &sdir, float sextent, Vector3& kClosestPoint0, Vector3& kClosestPoint1)
{
    Vector3 kDiff = lorigin - sorigin;
    Real fA01 = -ldir.Dot(sdir);
    Real fB0 = kDiff.Dot(ldir);
    Real fC = kDiff.SquaredLength();
    Real fDet = Math::FAbs((Real)1.0 - fA01*fA01);
    Real fB1, fS0, fS1, fSqrDist, fExtDet;

    if (fDet >= Math::ZERO_TOLERANCE)
    {
        // The line and segment are not parallel.
        fB1 = -kDiff.Dot(sdir);
        fS1 = fA01*fB0-fB1;
        fExtDet = sextent*fDet;

        if (fS1 >= -fExtDet)
        {
            if (fS1 <= fExtDet)
            {
                // Two interior points are closest, one on the line and one
                // on the segment.
                Real fInvDet = ((Real)1.0)/fDet;
                fS0 = (fA01*fB1-fB0)*fInvDet;
                fS1 *= fInvDet;
                fSqrDist = fS0*(fS0+fA01*fS1+((Real)2.0)*fB0) +
                    fS1*(fA01*fS0+fS1+((Real)2.0)*fB1)+fC;
            }
            else
            {
                // The end point e1 of the segment and an interior point of
                // the line are closest.
                fS1 = sextent;
                fS0 = -(fA01*fS1+fB0);
                fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
            }
        }
        else
        {
            // The end point e0 of the segment and an interior point of the
            // line are closest.
            fS1 = -sextent;
            fS0 = -(fA01*fS1+fB0);
            fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
        }
    }
    else
    {
        // The line and segment are parallel.  Choose the closest pair so that
        // one point is at segment origin.
        fS1 = (Real)0.0;
        fS0 = -fB0;
        fSqrDist = fB0*fS0+fC;
    }

    kClosestPoint0 = lorigin + fS0*ldir;
    kClosestPoint1 = sorigin + fS1*sdir;
    //m_fLineParameter = fS0;
    //m_fSegmentParameter = fS1;
    return Math::Sqrt(Math::FAbs(fSqrDist));
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::generateBranchDetails(Branch& br, int depth, float remainingLen, Branch* parent)
{
	br.m_depthIndex = depth;
	br.m_parent = parent;
	br.m_length = 0;
	if(parent)
		br.m_index = parent->m_index;

	std::vector<Vector3>::iterator it = br.m_points.begin();
	std::vector<Vector3>::iterator it2 = it;
	std::vector<Vector3>::iterator iend = br.m_points.end();
	
	it2++;
	for(;it2 != iend; ++it, ++it2)
	{
		br.m_length += (*it - *it2).Length();
	}

	br.m_lengthBurr = br.m_length + remainingLen;

	Vector3 vec;
	for(it = br.m_points.begin(); it != iend; ++it)
	{
		vec = *it;
		//vec = (vec + m_triMesh->_translation) * m_triMesh->_ratio;
		//vec = m_triMesh->_rotationMatrix * vec;
		vec = m_triMesh->_worldMatrix.TransformAffine(vec);
		br.m_worldPoints.push_back(vec);
	}

	std::vector<Branch>::iterator bit = br.m_childs.begin();
	std::vector<Branch>::iterator bend = br.m_childs.end();

	for(; bit != bend; ++bit)
		generateBranchDetails(*bit, depth + 1, br.m_lengthBurr, &br);

	if(br.m_childs.empty())
		m_tipList[br.m_index].push_back(&br);
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::markAsE(Branch* br, Branch* exclude, bool bMark)
{
	std::vector<Branch>::iterator bit = br->m_childs.begin();
	std::vector<Branch>::iterator bend = br->m_childs.end();

	for(; bit != bend; ++bit)
	{
		if(&(*bit) != exclude)
		{
			(*bit).m_isE = bMark;
			markAsE(&(*bit), exclude, bMark);
		}
		else
			(*bit).m_isE = !bMark;
	}
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::calcBranchLength(Branch& br, float remainingLen)
{
	br.m_length = 0;
	br.m_used = false;

	std::vector<Vector3>::iterator it = br.m_points.begin();
	std::vector<Vector3>::iterator it2 = it;
	std::vector<Vector3>::iterator iend = br.m_points.end();
	
	it2++;
	for(;it2 != iend; ++it, ++it2)
	{
		br.m_length += (*it - *it2).Length();
	}

	br.m_lengthBurr = br.m_length + remainingLen;

	std::vector<Branch>::iterator bit = br.m_childs.begin();
	std::vector<Branch>::iterator bend = br.m_childs.end();

	for(; bit != bend; ++bit)
		calcBranchLength(*bit, br.m_lengthBurr);
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::calcWorldPoints(Branch& br, Matrix3 &mat)
{
	//std::vector<Vector3>::iterator it = br.m_points.begin();
	//std::vector<Vector3>::iterator iend = br.m_points.end();

	//br.m_worldPoints.clear();
	//it = br.m_points.begin();
	//Vector3 vec;
	/*for(;it != iend; ++it)
	{
		vec = *it;
		vec = (vec + m_triMesh->_translation) * m_triMesh->_ratio;
		vec = m_triMesh->_rotationMatrix * vec;
		br.m_worldPoints.push_back(vec);
	}*/

	br.m_begPoint = *br.m_points.begin();
	br.m_begPoint = (br.m_begPoint + m_triMesh->_translation) * m_triMesh->_ratio;
	br.m_begPoint = mat * br.m_begPoint;

	std::vector<Branch>::iterator bit = br.m_childs.begin();
	std::vector<Branch>::iterator bend = br.m_childs.end();

	for(; bit != bend; ++bit)
		calcWorldPoints(*bit, mat);
}
//--------------------------------------------------------------------------------------------------------
bool CenterLine::intersectPlane(const Plane &pl, const Vector3 &p1, const Vector3 &p2, Vector3 &res)
{
	Side sd1, sd2;
	sd1 = pl.GetSide(p1);
	sd2 = pl.GetSide(p2);

	if(sd1 == NO_SIDE)
	{
		res = p1;
		return true;
	}
	else
	if(sd2 == NO_SIDE)
	{
		res = p2;
		return true;
	}
	else
		if(sd1 != sd2)
		{
			Vector3 dir = p2 - p1;
			dir.Normalize();
			Real denom = pl.Normal.Dot(dir);
			Real nom = pl.Normal.Dot(p1) + pl.Constant;
			res = p1 - dir * (nom/denom);
			return true;
		}

	return false;
}
//--------------------------------------------------------------------------------------------------------
bool CenterLine::intersectBurrPlane(Vector3 p1, Vector3 p2, Vector3 &res)
{
	bool ret = false;
	for(int i = 0; i < 2; i++)
		if(m_isValidPlane[i])
		{
			/*if((m_burCenter[i] - p1).Length() >= m_radius && 
				(m_burCenter[i] - p2).Length() >= m_radius)
				continue;*/

			Side sd1, sd2;
			sd1 = m_burPlane[i].GetSide(p1);
			sd2 = m_burPlane[i].GetSide(p2);

			if(sd1 == NO_SIDE)
			{
				res = p1;
				ret = true;
			}
			else
			if(sd2 == NO_SIDE)
			{
				res = p2;
				ret = true;
			}
			else
				if(sd1 != sd2)
				{
					Vector3 dir = p2 - p1;
					dir.Normalize();
					Real denom = m_burPlane[i].Normal.Dot(dir);
					Real nom = m_burPlane[i].Normal.Dot(p1) + m_burPlane[i].Constant;
					res = p1 - dir * (nom/denom);
					ret = true;
				}

			if(ret)
			{
				if((res - m_burCenter[i]).Length() >= m_radius * 2)
					ret = false;
				else
					return ret;
			}
		}

	return ret;
}
//--------------------------------------------------------------------------------------------------------
Vector3 CenterLine::getBranchDirection(std::vector<Vector3> &br)
{
	std::vector<Vector3>::reverse_iterator cit = br.rbegin();
	std::vector<Vector3>::reverse_iterator cend = br.rend();
	std::vector<Vector3>::reverse_iterator cit2 = cit;
	++cit2;
	Vector3 resDir = Vector3::ZERO;

	while(cit2 != cend/* && len < m_radius * 2*/)
	{
		resDir += (*cit2 - *cit).NormalizedCopy();
		//len += (*cit - *cit2).Length();

		cit = cit2;
		++cit2;
	}

	resDir.Normalize();

	return resDir;
}
//--------------------------------------------------------------------------------------------------------
void CenterLine::getBranchesVsSphere(const Vector3 &origin, float radius, Branch* br, std::vector<std::pair<Vector3, Branch*>> &res)
{
	if(br)
	{
		for(size_t i = 0; i < br->m_points.size(); i++)
			if(IsInsideSphere(origin, radius, br->m_points[i]))
			{
				res.push_back(std::pair<Vector3, Branch*>(getBranchDirection(br->m_points), br));
				break;
			}

		for(size_t i = 0; i < br->m_childs.size(); i++)
			getBranchesVsSphere(origin, radius, &br->m_childs[i], res);
	}
}
