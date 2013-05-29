#include "mesh.h"

#include <fstream>

void mesh::read_vertices(std::ifstream &vfile)
{
    for (int i = 0 ; i < nv; ++i)
    {
        vfile >> v[i].no;
        vfile.ignore(100, ':');
        vfile >> v[i].r.x >> v[i].r.y >> v[i].marker;
    }
}

void mesh::read_sides(std::ifstream &sfile)
{
    for (int i = 0 ; i < ns ; ++i)
    {
        int idx1, idx2;
        sfile >> s[i].no;
        sfile.ignore(100, ':');
        sfile >> idx1 >> idx2;
        s[i].v1 = idx1 == -1 ? NULL : &v[idx1]; s[i].v2 = idx2 == -1 ? NULL : &v[idx2];
        sfile >> idx1 >> idx2;
        s[i].e1 = idx1 == -1 ? NULL : &e[idx1]; s[i].e2 = idx2 == -1 ? NULL : &e[idx2];
        sfile >> s[i].marker;
    }
}

void mesh::read_elems(std::ifstream &efile)
{
    for (int i = 0 ; i < ne ; ++i)
    {
        int idx1, idx2, idx3;
        efile >> e[i].no;
        efile.ignore(100, ':');
        efile >> idx1 >> idx2 >> idx3;
        e[i].v1 = idx1 == -1 ? NULL : &v[idx1]; e[i].v2 = idx2 == -1 ? NULL : &v[idx2]; e[i].v3 = idx3 == -1 ? NULL : &v[idx3];
        efile >> idx1 >> idx2 >> idx3;
        e[i].e1 = idx1 == -1 ? NULL : &e[idx1]; e[i].e2 = idx2 == -1 ? NULL : &e[idx2]; e[i].e3 = idx3 == -1 ? NULL : &e[idx3];
        efile >> idx1 >> idx2 >> idx3;
        e[i].s1 = idx1 == -1 ? NULL : &s[idx1]; e[i].s2 = idx2 == -1 ? NULL : &s[idx2]; e[i].s3 = idx3 == -1 ? NULL : &s[idx3];
        efile >> e[i].r.x >> e[i].r.y >> e[i].marker;
    }
}

mesh::mesh(std::string vfile_name, std::string sfile_name, std::string efile_name)
{
    std::ifstream vfile(vfile_name.c_str()), sfile(sfile_name.c_str()), efile(efile_name.c_str());

    vfile >> nv;
    sfile >> ns;
    efile >> ne;

    v = new vertex[nv];
    s = new side[ns];
    e = new element[ne];

    read_vertices(vfile);
    read_sides(sfile);
    read_elems(efile);

    vfile.close();
    sfile.close();
    efile.close();

   compute_geom_props();
   init_boundary_vertices();
   init_boundary_sides();
}

void mesh::compute_geom_props()
{
    for (int i = 0 ; i < ns ; ++i)
    {
        s[i].n = s[i].v2->r - s[i].v1->r;
        s[i].n.normalize();

        vector p1 = s[i].e1 == NULL ? 0.5 * (s[i].v1->r + s[i].v2->r) : s[i].e1->r;
        vector p2 = s[i].e2 == NULL ? 0.5 * (s[i].v1->r + s[i].v2->r) : s[i].e2->r;
        s[i].dn = (p1 - p2).length();

        s[i].dl = (s[i].v1->r - s[i].v2->r).length();
    }

    for (int i = 0 ; i < ns ; ++i)
    {
        double da = s[i].dl * s[i].dn / 4 ;
        if (s[i].v1 != NULL) s[i].v1->area += da;
        if (s[i].v2 != NULL) s[i].v2->area += da;
    }
}

void mesh::init_boundary_vertices()
{
    nbv = 0;
    for (int i = 0; i < nv; ++i) if (v[i].marker != 0) ++nbv;
    bvertices = new int[nbv];
    for (int i = 0, idx = 0; i < nv; ++i) if (v[i].marker != 0) bvertices[idx++] = i;
}

void mesh::init_boundary_sides()
{
    nbs = 0;
    for (int i = 0; i < ns; ++i)
        if (s[i].e1 == NULL || s[i].e2 == NULL)
            ++nbs;
    bsides = new int[nbs];
    bsnormals = new vector[nbs];

    for (int i = 0, idx = 0; i < ns; ++i)
        if (s[i].e1 == NULL || s[i].e2 == NULL)
        {
            bsides[idx] = i;

            vector side_center = 0.5 * ( s[i].v1->r + s[i].v2->r );
            vector r_e = s[i].e1 != NULL ? s[i].e1->r : s[i].e2->r;
            bsnormals[idx] = side_center - r_e;
            bsnormals[idx].normalize();

            ++idx;
        }
}

mesh::~mesh()
{
    delete[] v;
    delete[] s;
    delete[] e;

    delete[] bsides;
    delete[] bvertices;
    delete[] bsnormals;
}

