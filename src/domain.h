#include <string>
#include <vector>

#include "mesh.h"

#ifndef DOMAIN_H
#define DOMAIN_H

struct variable;

class domain
{
	std::vector<variable> variables;
	void register_var(float **data, std::string name, bool write);
	void init_vars();
	void clear_vars();

public:
	mesh m;
	float *px, *py, *ux, *uy, *xi, *h, *d;

	void write_file(std::string fname);

	domain(std::string vfile_name, std::string sfile_name, std::string efile_name);
	~domain();
};

#endif
