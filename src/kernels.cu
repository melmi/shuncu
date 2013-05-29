#include "kernels.h"

void HandleError(cudaError_t err, const char *file, int line) 
{
	if (err != cudaSuccess) 
	{
		printf( "%s in %s at line %d\n" , 
			    cudaGetErrorString( err ),
				file, line );
		exit( EXIT_FAILURE );
	}
}

void HandleError( cudaError_t err ) 
{
	HandleError( err, __FILE__, __LINE__ );
}


void init_device_mesh(device_mesh& dm, mesh m)
{
	dm.ne  = m.ne;
	dm.nv  = m.nv;
	dm.ns  = m.ns;
	dm.nbv = m.nbv;
	dm.nbs = m.nbs;

	HandleError( cudaMalloc( (void**)&dm.a , m.nv * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.dl, m.ns * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.dn, m.ns * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.nx, m.ns * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.ny, m.ns * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.v1, m.ns * sizeof(int   ) ) );
	HandleError( cudaMalloc( (void**)&dm.v2, m.ns * sizeof(int   ) ) );
	
	HandleError( cudaMalloc( (void**)&dm.nbx, m.nbs * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.nby, m.nbs * sizeof(double) ) );

	HandleError( cudaMalloc( (void**)&dm.bsides   , m.nbs * sizeof(int) ) );
	HandleError( cudaMalloc( (void**)&dm.bvertices, m.nbv * sizeof(int) ) );

	HandleError( cudaMalloc( (void**)&dm.px, m.nv * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.py, m.nv * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.ux, m.nv * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.uy, m.nv * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.xi, m.nv * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.h , m.nv * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&dm.d , m.nv * sizeof(double) ) );
}

void free_device_mem(device_mesh dm)
{
	HandleError( cudaFree( dm.a  ) );
	HandleError( cudaFree( dm.dl ) );
	HandleError( cudaFree( dm.dn ) );
	HandleError( cudaFree( dm.nx ) );
	HandleError( cudaFree( dm.ny ) );
	HandleError( cudaFree( dm.v1 ) );
	HandleError( cudaFree( dm.v2 ) );

	HandleError( cudaFree( dm.px ) );
	HandleError( cudaFree( dm.py ) );
	HandleError( cudaFree( dm.ux ) );
	HandleError( cudaFree( dm.uy ) );
	HandleError( cudaFree( dm.xi ) );
	HandleError( cudaFree( dm.h  ) );
	HandleError( cudaFree( dm.d  ) );
}

void copy_mesh_data(device_mesh& dm, mesh m)
{
	double *a  = new double[m.nv];
	double *dl = new double[m.ns];
	double *dn = new double[m.ns];
	double *nx = new double[m.ns];
	double *ny = new double[m.ns];
	double *nbx = new double[m.nbs];
	double *nby = new double[m.nbs];
	int *v1 = new int[m.ns];
	int *v2 = new int[m.ns];

	for (int i=0;i<m.nv;++i) a[i]=m.v[i].area;
	for (int i=0;i<m.ns;++i)
	{
		dl[i] = m.s[i].dl;
		dn[i] = m.s[i].dn;
		nx[i] = m.s[i].n.x;
		ny[i] = m.s[i].n.y;
		v1[i] = m.s[i].v1->no;
		v2[i] = m.s[i].v2->no;
	}
	for (int i=0;i<m.nbs;++i)
	{
		nbx[i]=m.bsnormals[i].x;
		nby[i]=m.bsnormals[i].y;
	}

	HandleError( cudaMemcpy( dm.a , a , m.nv * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.dl, dl, m.ns * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.dn, dn, m.ns * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.nx, nx, m.ns * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.ny, ny, m.ns * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.v1, v1, m.ns * sizeof(int   ), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.v2, v2, m.ns * sizeof(int   ), cudaMemcpyHostToDevice ) );

	HandleError( cudaMemcpy( dm.nbx, nbx, m.nbs * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.nby, nby, m.nbs * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.bsides   , m.bsides   , m.nbs * sizeof(int), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.bvertices, m.bvertices, m.nbv * sizeof(int), cudaMemcpyHostToDevice ) );

	delete[] a ;
	delete[] dl;
	delete[] dn;
	delete[] nx;
	delete[] ny;
	delete[] v1;
	delete[] v2;
	delete[] nbx;
	delete[] nby;
}
__global__ void _grad_normal_sides(device_mesh m, double *phi, double *gradx, double *grady)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i >= m.ns) return;

	int v1 = m.v1[i];
	int v2 = m.v2[i];

	double phif = ( phi[v1] + phi[v2] ) / 2;

	double gx= phif * m.nx[i] * m.dn[i];
	double gy= phif * m.ny[i] * m.dn[i];

	gradx[v1] += gx; grady[v1] += gy;
	gradx[v2] -= gx; grady[v2] -= gy;
}

__global__ void _grad_bsides(device_mesh m, double *phi, double *gradx, double *grady)
{
	int j = blockDim.x * blockIdx.x + threadIdx.x;
	if (j >= m.nbs) return;
	int i = m.bsides[j];

	int v1 = m.v1[i];
	int v2 = m.v2[i];

	double phif1 = 0.75 * phi[v1] + 0.25 * phi[v2];
	double phif2 = 0.25 * phi[v1] + 0.75 * phi[v2];

	gradx[v1] += phif1 * m.nbx[j] * m.dl[i] /2; grady[v1] += phif1 * m.nbx[j] * m.dl[i] /2;
	gradx[v2] += phif2 * m.nbx[j] * m.dl[i] /2; grady[v2] += phif2 * m.nbx[j] * m.dl[i] /2;
}

__global__ void _devide_by_a(device_mesh m, double *phi)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i >= m.nv) return;

	phi[i] /= m.a[i];
}
void get_grad(device_mesh m, double *phi, double*& gradx, double*& grady)
{
	HandleError( cudaMalloc( (void**)&gradx , m.nv * sizeof(double) ) );
	HandleError( cudaMalloc( (void**)&grady , m.nv * sizeof(double) ) );

	int nthreads = 512;

	_grad_normal_sides<<<m.ns/nthreads+1,nthreads>>>(m, phi, gradx, grady);
	_grad_bsides<<<m.nbs/nthreads+1,nthreads>>>(m, phi, gradx, grady);
	_devide_by_a<<<m.nv/nthreads+1,nthreads>>>(m, phi);
}

void copy_to_device(device_mesh dm, domain d)
{
	HandleError( cudaMemcpy( dm.px , d.px , dm.nv * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.py , d.py , dm.nv * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.ux , d.ux , dm.nv * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.uy , d.uy , dm.nv * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.xi , d.xi , dm.nv * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.h  , d.h  , dm.nv * sizeof(double), cudaMemcpyHostToDevice ) );
	HandleError( cudaMemcpy( dm.d  , d.d  , dm.nv * sizeof(double), cudaMemcpyHostToDevice ) );
}
void copy_to_host(device_mesh dm, domain d)
{
	HandleError( cudaMemcpy( d.px , dm.px , dm.nv * sizeof(double), cudaMemcpyDeviceToHost ) );
	HandleError( cudaMemcpy( d.py , dm.py , dm.nv * sizeof(double), cudaMemcpyDeviceToHost ) );
	HandleError( cudaMemcpy( d.ux , dm.ux , dm.nv * sizeof(double), cudaMemcpyDeviceToHost ) );
	HandleError( cudaMemcpy( d.uy , dm.uy , dm.nv * sizeof(double), cudaMemcpyDeviceToHost ) );
	HandleError( cudaMemcpy( d.xi , dm.xi , dm.nv * sizeof(double), cudaMemcpyDeviceToHost ) );
	HandleError( cudaMemcpy( d.h  , dm.h  , dm.nv * sizeof(double), cudaMemcpyDeviceToHost ) );
	// HandleError( cudaMemcpy( d.m.d  , dm.d  , dm.nv * sizeof(double), cudaMemcpyDeviceToHost ) );
}