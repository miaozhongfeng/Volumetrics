
#pragma once

#include <iostream>

class CColor
{
public:
	float r, g, b, a;
	CColor() : r(0.0), g(0.0), b(0.0), a(1.0) {}
	CColor(float r, float g, float b, float a = 1.0) : r(r), g(g), b(b), a(a) {}
};

inline std::ostream& operator << (std::ostream& lhs, const CColor &rhs)
{
	lhs << "(" << rhs.r << ", " << rhs.g << ", " << rhs.b << ", " << rhs.a << ")";
	return lhs;
}

inline std::istream& operator >> (std::istream& lhs, CColor &rhs)
{
	char aux;
	lhs >> aux >> rhs.r >> aux >> rhs.g >> aux >> rhs.b >> aux >> rhs.a >> aux;
	return lhs;
}

