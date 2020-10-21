#pragma once

class Vec2f
{
public:
	Vec2f(float _x, float _y);
	float x;
	float y;

	Vec2f operator+(const Vec2f& v);
	Vec2f operator-(const Vec2f& v);
	
	Vec2f operator*(const float& f);
	Vec2f operator/(const float& f);

	float Length();
	void Normalize();
};

