
inline bool CenterLine::IsInsideSphere(const Vector3 &origin, float radius, const Vector3 &point)
{
	if((origin - point).Length() < radius)
		return true;
	else
		return false;
}
//--------------------------------------------------------------------------------------------------------
inline bool CenterLine::IsInsideSphere(const OpenMesh::Vec3f &origin, float radius, const OpenMesh::Vec3f &point)
{
	if((origin - point).length() < radius)
		return true;
	else
		return false;
}
//--------------------------------------------------------------------------------------------------------
inline int CenterLine::IsInsideSphere2x(const OpenMesh::Vec3f &origin, float radius1, float radius2, const OpenMesh::Vec3f &point)
{
	float len = (origin - point).length();
	if(len < radius1)
		return 2;
	else
	if(len < radius2)
		return 1;
	else
		return 0;
}
//--------------------------------------------------------------------------------------------------------
inline const Vector3& CenterLine::toVector3(const OpenMesh::Vec3f &src)
{
	return *((Vector3*)&src[0]);//Vector3(0,0,0);//(float*)&src[0]);
};
//--------------------------------------------------------------------------------------------------------
inline const OpenMesh::Vec3f& CenterLine::toVec(const Vector3 &src)
{
	return *((OpenMesh::Vec3f*)&src.x);//Vector3(0,0,0);//(float*)&src[0]);
};
//--------------------------------------------------------------------------------------------------------

inline void CenterLine::splitBranch(Branch& parent, Vector3& splitPoint)
{
	Branch br;
	br.fromBranch(parent);
	std::vector<Vector3>::iterator it = br.m_points.begin();
	std::vector<Vector3>::iterator iend = br.m_points.end();

	for(;it != iend; ++it)
		if(*it == splitPoint)
		{
			++it;
			if(it != iend)
				br.m_points.erase(it, iend);

			break;
		}

	it = parent.m_points.begin();
	iend = parent.m_points.end();

	for(;it != iend; ++it)
		if(*it == splitPoint)
		{
			parent.m_points.erase(parent.m_points.begin(), it);
			break;
		}

	parent.m_childs.clear();
	parent.m_childs.push_back(br);
}
//--------------------------------------------------------------------------------------------------------
inline Branch CenterLine::fromBranch(Branch& parent, Vector3& splitPoint)
{
	Branch br;
	br.fromBranch(parent);
	std::vector<Vector3>::iterator it = br.m_points.begin();
	std::vector<Vector3>::iterator iend = br.m_points.end();

	bool bFind = false;
	for(;it != iend; ++it)
		if(*it == splitPoint)
		{
			bFind = true;
			++it;
			if(it != iend)
				br.m_points.erase(it, iend);

			break;
		}

	if(!bFind)
	{
		MessageBox(0,L"Can't find split point",L"Some",0);
	}

	return br;
}
//--------------------------------------------------------------------------------------------------------
inline Branch CenterLine::fromCenterline(int index, Vector3& splitPoint)
{
	Branch br;
	br.m_index = index;
	br.m_points = m_centerLines[index];
	//br.fromBranch(parent);
	std::vector<Vector3>::iterator it = br.m_points.begin();
	std::vector<Vector3>::iterator iend = br.m_points.end();

	bool bFind = false;
	for(;it != iend; ++it)
		if(*it == splitPoint)
		{
			bFind = true;
			++it;
			if(it != iend)
				br.m_points.erase(it, iend);

			break;
		}

	if(!bFind)
	{
		MessageBox(0,L"Can't find split point",L"Some",0);
	}

	return br;
}
//--------------------------------------------------------------------------------------------------------
inline void CenterLine::parentFromCenterline(Branch& br, int index, Vector3& splitPoint)
{
	br.m_index = index;
	br.m_points = m_centerLines[index];
	//br.fromBranch(parent);
	std::vector<Vector3>::iterator it = br.m_points.begin();
	std::vector<Vector3>::iterator iend = br.m_points.end();

	bool bFind = false;
	for(;it != iend; ++it)
		if(*it == splitPoint)
		{
			bFind = true;
			br.m_points.erase(br.m_points.begin(), it);
			break;
		}

	if(!bFind)
	{
		MessageBox(0,L"Can't find split point",L"Some",0);
	}
}
//--------------------------------------------------------------------------------------------------------
inline float CenterLine::angleBetweenVectors(Vector3 &a, Vector3 &b)
{
	return abs(acos(a.Dot(b)/(a.Length()*b.Length())) * Math::RAD_TO_DEG);
}
//--------------------------------------------------------------------------------------------------------
inline float CenterLine::angleBetweenLines(int index, Vector3 lastPoint)
{
	if(m_centerLines[index].size() < 2)
		return 0;

	Vector3 a = *m_centerLines[index].rbegin() - *(m_centerLines[index].rbegin() + 1);
	Vector3 b = lastPoint - *m_centerLines[index].rbegin();

	return abs(acos(a.Dot(b)/(a.Length()*b.Length())) * Math::RAD_TO_DEG);
}
//--------------------------------------------------------------------------------------------------------
inline bool CenterLine::isBetterSegment(int index, size_t pop, Vector3 lastPoint)
{
	if(pop < 1 || pop >= m_centerLines[index].size())
		return false;

	if(m_centerLines[index].size() < 2)
		return 0;

	Vector3 a = *m_centerLines[index].rbegin() - *(m_centerLines[index].rbegin() + pop);
	Vector3 b = lastPoint - *(m_centerLines[index].rbegin() + pop);
	if(pop + 1 == m_centerLines[index].size())
		return a.Length() < b.Length();

	float lenA = a.Length();
	float lenB = b.Length();

	if(lenA > lenB)
		return false;

	Vector3 c = *(m_centerLines[index].rbegin() + pop) - *(m_centerLines[index].rbegin() + pop + 1);
	float lenC = c.Length();

	float angAC = abs(acos(a.Dot(c)/(lenA * lenC)));
	//float angAB = abs(acos(b.Dot(c)/(lenB * lenC)));
	//return angAC > angAB;

	return angAC * Math::RAD_TO_DEG < 20;
}
//--------------------------------------------------------------------------------------------------------
inline void CenterLine::smoothPoints(Vector3 &v1, Vector3& v2, Vector3& v3)
{
	Vector3 dir32 = (v3 - v2);
	float len32 = dir32.Length(); 
	Vector3 dir21 = (v2 - v1);
	float len21 = dir21.Length(); 
	Vector3 dir = (v3 - v2) + (v2 - v1);
	dir.Normalize();
	dir32.Normalize();
	dir21.Normalize();
	v3 = v2 + (dir + dir32*10).NormalizedCopy() * len32;
	v2 = v1 + (dir + dir21*10).NormalizedCopy() * len21;
	//v2 = v1 + (dir).NormalizedCopy() * len21;

}
//--------------------------------------------------------------------------------------------------------
inline void CenterLine::smoothPoints(Vector3 &v1, Vector3& v2, Vector3& v3, Vector3& v4)
{
	Vector3 dir43 = (v4 - v3);
	float len43 = dir43.Length(); 
	Vector3 dir32 = (v3 - v2);
	float len32 = dir32.Length(); 
	Vector3 dir21 = (v2 - v1);
	float len21 = dir21.Length(); 
	Vector3 dir = (v4 - v3) + (v3 - v2) + (v2 - v1);
	dir.Normalize();
	dir43.Normalize();
	dir32.Normalize();
	dir21.Normalize();
	v4 = v3 + (dir + dir43).NormalizedCopy() * len43;
	v3 = v2 + (dir + dir32).NormalizedCopy() * len32;
	v2 = v1 + (dir + dir21).NormalizedCopy() * len21;
}
//--------------------------------------------------------------------------------------------------------
inline void CenterLine::subdivideBranchDepth(Branch& br, float div, bool bSkipFirstLastSegment)
{
	subdivideBranch(br, div);
	
	std::vector<Branch>::iterator bit = br.m_childs.begin();
	std::vector<Branch>::iterator bend = br.m_childs.end();

	for(; bit != bend; ++bit)
		subdivideBranchDepth(*bit, bSkipFirstLastSegment);
}
//--------------------------------------------------------------------------------------------------------
inline void CenterLine::subdivideBranch(Branch& br, float div, bool bSkipFirstLastSegment)
{
	std::vector<Vector3>::iterator it = br.m_points.begin();
	std::vector<Vector3>::iterator iend = br.m_points.end();

	if(br.m_points.size() > 1)
	{
		Vector3 prev, next, dir, res;
		float sqRad = Math::Sqr(m_radius * div);
		float len;
		int divCount;
		next = br.m_points[0];
		for(size_t j = 1; j < br.m_points.size(); j++)
		{
			prev = next;
			next = br.m_points[j];
			if(bSkipFirstLastSegment && (j == 1 || j == br.m_points.size() - 1))
				continue;
			dir = next - prev;
			len = dir.SquaredLength();
			if(len > sqRad)	
			{
				divCount = floor(sqrt(len / sqRad)) + 1;
				len = sqrt(len) / divCount;
				dir.Normalize();
				for(int k = 1; k < divCount; k++)
				{
					res = prev + dir * len * k;
					br.m_points.insert(br.m_points.begin() + j, res);
					j++;
				}
				next = br.m_points[j];
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------

inline void CenterLine::subdivideBranchInSphere(Branch& br, Vector3 &origin, float radius, float div)
{
	std::vector<Vector3>::iterator it = br.m_points.begin();
	std::vector<Vector3>::iterator iend = br.m_points.end();

	if(br.m_points.size() > 1)
	{
		Vector3 prev, next, dir, res;
		float sqRad = Math::Sqr(m_radius * div);
		float len;
		int divCount;
		next = br.m_points[0];
		for(size_t j = 1; j < br.m_points.size(); j++)
		{
			prev = next;
			next = br.m_points[j];
			if(!IsInsideSphere(origin, radius, prev) && !IsInsideSphere(origin, radius, next))
				continue;
			dir = next - prev;
			len = dir.SquaredLength();
			if(len > sqRad)	
			{
				divCount = floor(sqrt(len / sqRad)) + 1;
				len = sqrt(len) / divCount;
				dir.Normalize();
				for(int k = 1; k < divCount; k++)
				{
					res = prev + dir * len * k;
					br.m_points.insert(br.m_points.begin() + j, res);
					j++;
				}
				next = br.m_points[j];
			}
		}
	}
}
