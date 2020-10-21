#include "Vec2f.h"
#include "cmath"

Vec2f::Vec2f(float _x, float _y) :
	x(_x), y(_y)
{
}

Vec2f Vec2f::operator+(const Vec2f &v)
{
	return Vec2f(x + v.x, y + v.y);
}

Vec2f Vec2f::operator-(const Vec2f & v)
{
	return Vec2f(x - v.x, y - v.y);
}

Vec2f Vec2f::operator*(const float & f)
{
	return Vec2f(x*f, y*f);
}

Vec2f Vec2f::operator/(const float & f)
{
	return Vec2f(x / f, y / f);
}

float Vec2f::Length()
{
	if (x == 0.0f && y == 0.0f)
		return 0;
	return std::sqrt(x*x + y * y);
}

void Vec2f::Normalize()
{
	float length = Length();
	x /= length;
	y /= length;
}
