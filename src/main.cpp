#include "domain.h"
#include "edge_coloring.h"
#include <iostream>
using namespace std;

int main()
{
    domain d("./mesh/n0.dat", "./mesh/s0.dat", "./mesh/e0.dat");
    d.write_file("./output/test.vtk");
    edge_coloring::do_edge_coloring(d.m);

    for (int i = 0; i < d.m.ns; ++i)
    {
        std::cout << d.m.s[i].color << std::endl;
    }
}