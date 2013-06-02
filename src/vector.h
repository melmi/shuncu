#ifndef VECTOR_H
#define VECTOR_H

class vector
{
public:
	float x,y;
	vector(void);
	vector(float _x, float _y);
	float length();
	void normalize();
};

float operator*(vector v1, vector v2);
vector operator+(vector v1, vector v2);
vector operator-(vector v1, vector v2);
vector operator*(float r, vector v2);

#endif