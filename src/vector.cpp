#include "vector.h"

#include<cmath>

vector::vector(void)
{
	x=y=0;
}

vector::vector(float _x, float _y)
{
	x=_x;
	y=_y;
}

float operator*(vector v1, vector v2)
{
	return v1.x*v2.x+v1.y*v2.y;
}

vector operator+(vector v1, vector v2)
{
	return vector(v1.x+v2.x,v1.y+v2.y);
}

vector operator-(vector v1, vector v2)
{
	return vector(v1.x-v2.x,v1.y-v2.y);
}

vector operator*(float r, vector v2)
{
	return vector(r*v2.x,r*v2.y);
}

float vector::length()
{
	return std::sqrt(x*x+y*y);
}
void vector::normalize()
{
	float l=length();
	x/=l;
	y/=l;
}


