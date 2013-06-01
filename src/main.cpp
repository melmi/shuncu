#include "domain.h"
#include <iostream>
using namespace std;

int main()
{
    domain d("./mesh/n0.dat", "./mesh/s0.dat", "./mesh/e0.dat");
    d.write_file("./output/test.vtk");

    for (int i = 0; i < d.m.ns; ++i)
    {
        std::cout << d.m.s[i].color << std::endl;
    }
}