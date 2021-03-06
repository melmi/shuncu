#include <fstream>
#include "vector.h"

#ifndef MESH_H
#define MESH_H

struct vertex
{
    int no , marker;
    vector r;
    float area;
};

struct element;

struct side
{
    int no, marker;
    vertex *v1, *v2;
    element *e1, *e2;
    int color;

    vector n;
    float dn, dl;

    int ibc;
};

struct element
{
    int no, marker;
    vertex *v1, *v2, *v3;
    element *e1, *e2, *e3;
    side *s1, *s2, *s3;
    vector r;
};

class mesh
{
    void read_vertices(std::ifstream &vfile);
    void read_sides(std::ifstream &sfile);
    void read_elems(std::ifstream &efile);
    
    void compute_geom_props();
    
    void init_boundary_vertices();
    void init_boundary_sides();

    void init_color_sides();
    void init_b_color_sides();
public:
    vertex *v;
    side *s;
    element *e;
    int ne, nv, ns;

    int *bsides, *bvertices; //boundary sides and vertices
    vector *bsnormals; //normal vectors of boundary sides to the out of mesh
    int nbv, nbs;

    int ncolors, nbcolors;
    int *ncolor_sides, *nbcolor_sides;
    int **color_sides, **bcolor_sides;

    mesh(std::string vfile_name, std::string sfile_name, std::string efile_name);
    ~mesh();
};

#endif