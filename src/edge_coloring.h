/**
 *
 * This code is based on the code of Maciej Piechotka whom implemented
 * the algorithm for edge coloring of Misra & Gries (1992).
 *
 * Reference:
 *
 * Misra, J., & Gries, D. (1992)
 * A constructive proof of Vizing’s theorem. In Information Processing Letters.
 *
 */

#ifndef EDGE_COLORING_H
#define EDGE_COLORING_H

#include <vector>

#include "mesh.h"

class edge_coloring
{
    int *n_vertex_sides;
    side ** *vertex_sides;

    typedef std::vector<vertex *> vertexvec;

    void construct_vertex_sides(mesh &m, bool boundary);
    vertex *other_vertex(side *s, vertex *v);
    side *get_side(vertex *u, vertex *v);

    bool is_free(vertex *v, int color);
    vertexvec maximal_fan(vertex *x, vertex *y);
    int find_free_color(vertex *v);
    void invert_cd_path(vertex *x, side *eold, int c, int d);
    void invert_cd_path(vertex *x, int c, int d);
    void rotate_fan(vertex *x, vertexvec::iterator begin, vertexvec::iterator end);
    int color_edge(side *e);

    class bind_is_free
    {
        edge_coloring *ec;
        int d;
    public:
        bind_is_free(edge_coloring *_ec, int _d): ec(_ec), d(_d) {}
        bool operator()(vertex *v)
        {
            return ec->is_free(v, d);
        }
    };
public:
    static int do_edge_coloring(mesh &m, bool boundary);
};

#endif