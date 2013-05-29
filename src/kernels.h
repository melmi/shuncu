#include "mesh.h"
#include "domain.h"

#ifndef KERNELS_H
#define KERNELS_H

struct device_mesh
{
	int ne, nv, ns;
	int nbv, nbs;

	double *a, *dl, *dn, *nx, *ny, *nbx, *nby;
	int *v1, *v2, *bsides, *bvertices;
	double *px, *py, *h, *ux, *uy, *xi, *d;
};

void init_device_mesh(device_mesh& dm, mesh m);
void free_device_mem(device_mesh& dm);
void copy_mesh_data(device_mesh& dm, mesh m);
void copy_to_device(device_mesh dm, domain d);
void copy_to_host(device_mesh dm, domain d);

void get_grad(device_mesh m, double *phi, double*& gradx, double*& grady);

#endif