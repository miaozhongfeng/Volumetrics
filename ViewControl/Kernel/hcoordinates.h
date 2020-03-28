#pragma once

#include <iostream>
#include <cmath>

class CHCoordinate
{
public:

	float x, y, z, w;

	CHCoordinate(): x(0.0), y(0.0), z(0.0), w(1.0) {}

	CHCoordinate(float x, float y, float z, float w = 1.0): x(x), y(y), z(z), w(w) {}
	
	CHCoordinate operator + (const CHCoordinate &hc)
	{
		float 
			x1 = x / w,
			y1 = y / w,
			z1 = z / w,
			x2 = hc.x / hc.w,
			y2 = hc.y / hc.w,
			z2 = hc.z / hc.w;

		return CHCoordinate( x1 + x2, y1 + y2, z1 + z2 );
	}

	CHCoordinate operator - (const CHCoordinate &hc)
	{
		float 
			x1 = x / w,
			y1 = y / w,
			z1 = z / w,
			x2 = hc.x / hc.w,
			y2 = hc.y / hc.w,
			z2 = hc.z / hc.w;

		return CHCoordinate( x1 - x2, y1 - y2, z1 - z2 );
	}

	CHCoordinate& operator += (const CHCoordinate &hc)
	{
		x = x / w + hc.x / hc.w,
		y = y / w + hc.y / hc.w,
		z = z / w + hc.z / hc.w;
		w = 1.0;

		return *this;
	}

	CHCoordinate& operator -= (const CHCoordinate &hc)
	{
		x = x / w - hc.x / hc.w,
		y = y / w - hc.y / hc.w,
		z = z / w - hc.z / hc.w;
		w = 1.0;

		return *this;
	}

	float operator * (const CHCoordinate &hc)
	{
		float 
			x1 = x / w,
			y1 = y / w,
			z1 = z / w,
			x2 = hc.x / hc.w,
			y2 = hc.y / hc.w,
			z2 = hc.z / hc.w;

		return x1 * x2 + y1 * y2 + z1 * z2;
	}

	CHCoordinate& operator *= (float c_xyz)
	{
		x *= c_xyz,
		y *= c_xyz,
		z *= c_xyz;

		return *this;
	}

	CHCoordinate& operator /= (float c_xyz)
	{
		x /= c_xyz,
		y /= c_xyz,
		z /= c_xyz;

		return *this;
	}

	CHCoordinate operator % (const CHCoordinate &hc)
	{
		float 
			x1 = x / w,
			y1 = y / w,
			z1 = z / w,
			x2 = hc.x / hc.w,
			y2 = hc.y / hc.w,
			z2 = hc.z / hc.w;

		return CHCoordinate( y1 * z2 - z1 * y2, z1 * x2 - x1 * z2, x1 * y2 - y1 * x2 );
	}

	void Normalize()
	{
		float length = (float) sqrt((*this) * (*this));
		if (length != 1.0f)
			*this /= length;
	}

};

inline std::ostream& operator << (std::ostream &lhs, const CHCoordinate &hc)
{
	lhs << "(" << hc.x << ", " << hc.y << ", " << hc.z << ", " << hc.w << ")";
	return lhs;
}

inline std::istream& operator >> (std::istream &lhs, CHCoordinate &hc)
{
	char aux;
	lhs >> aux >> hc.x >> aux >> hc.y >> aux >> hc.z >> aux >> hc.w >> aux;
	return lhs;
}