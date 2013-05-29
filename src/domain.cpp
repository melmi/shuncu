#include "domain.h"

#include <fstream>
#include <algorithm>

struct variable
{
    double **data;
    std::string name;
    bool write;
};

domain::domain(std::string vfile_name, std::string sfile_name, std::string efile_name)
    : m(vfile_name, sfile_name, efile_name)
{
    register_var(&px, "px", false);
    register_var(&py, "py", false);
    register_var(&ux, "ux", true);
    register_var(&uy, "uy", true);
    register_var(&xi, "xi", true);
    register_var(&h , "h" , false);
    register_var(&d , "d" , false);

    init_vars();
}

domain::~domain(void)
{
    clear_vars();
}

void domain::register_var(double **data, std::string name, bool write)
{
    variable v;
    v.data = data;
    v.name = name;
    v.write = write;
    variables.push_back(v);
}

void domain::init_vars()
{
    for (std::vector<variable>::iterator i = variables.begin(); i != variables.end(); ++i)
    {
        *i->data = new double[m.nv];
        std::fill_n(*i->data, m.nv, 0);
    }
}

void domain::clear_vars()
{
    for (std::vector<variable>::iterator i = variables.begin(); i != variables.end(); ++i)
        delete[] (*i->data);
}

void domain::write_file(std::string fname)
{
    std::ofstream file(fname.c_str());

    file << "# vtk DataFile Version 3.0" << std::endl ;
    file << "Habib's Data" << std::endl ;
    file << "ASCII" << std::endl ;
    file << "DATASET UNSTRUCTURED_GRID" << std::endl ;
    file << "POINTS " << m.nv << " float" << std::endl ;
    for (int i = 0 ; i < m.nv ; ++i)
        file << m.v[i].r.x << " " << m.v[i].r.y << "  0"  << std::endl;
    file << "CELLS " << m.ne << " " << m.ne * 4 << std::endl;
    for (int i = 0 ; i < m.ne ; ++i)
        file << "3  " << m.e[i].v1->no << " " << m.e[i].v2->no << " " << m.e[i].v3->no << std::endl;
    file << "CELL_TYPES " << m.ne << std::endl ;
    for (int  i = 0 ; i < m.ne ; ++i)
        file << "5" << std::endl;

    file << "POINT_DATA " << m.nv << std::endl ;

    for (std::vector<variable>::iterator i = variables.begin(); i != variables.end(); ++i)
        if (i->write)
        {
            file << "SCALARS " << i->name << " float" << std::endl ;
            file << "LOOKUP_TABLE default" << std::endl ;
            for (int j = 0 ; j < m.nv ; ++j)
                file << (*i->data)[j] << std::endl;
        }

    file.close();
}
