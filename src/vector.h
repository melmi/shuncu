#ifndef VECTOR_H
#define VECTOR_H

class vector
{
public:
	double x,y;
	vector(void);
	vector(double _x, double _y);
	double length();
	void normalize();
};

double operator*(vector v1, vector v2);
vector operator+(vector v1, vector v2);
vector operator-(vector v1, vector v2);
vector operator*(double r, vector v2);

#endif