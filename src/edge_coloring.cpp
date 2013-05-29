#include "edge_coloring.h"

#include <algorithm>
#include <functional>

vertex *edge_coloring::other_vertex(side *s, vertex *v)
{
    if (s->v1 == v)
        return s->v2;
    else
        return s->v1;
}

bool edge_coloring::is_free(vertex *v, int color)
{
    if (color == -1) return false;

    int nsides = n_vertex_sides[v->no];
    for (int i = 0; i < nsides; ++i)
    {
        if (vertex_sides[v->no][i]->color == color)
            return false;
    }
    return true;
}

edge_coloring::vertexvec edge_coloring::maximal_fan(vertex *x, vertex *y)
{

    vertexvec fan;
    fan.push_back(y);
    bool extended;
    do
    {
        extended = false;

        int nsides = n_vertex_sides[x->no];
        for (int i = 0; i < nsides; ++i)
        {
            vertex *v = other_vertex(vertex_sides[x->no][i], x);
            if (is_free(fan.back(), vertex_sides[x->no][i]->color) &&
                    std::find(fan.begin(), fan.end(), v) == fan.end())
            {
                fan.push_back(v);
                extended = true;
            }
        }
    }
    while (extended);

    return fan;
}

int edge_coloring::find_free_color(vertex *v)
{
    int c = 0;
    while (!is_free(v, c)) c++;

    return c;
}

void edge_coloring::invert_cd_path(vertex *x, side *eold, int c, int d)
{
    eold->color = d;

    int nsides = n_vertex_sides[x->no];
    for (int i = 0; i < nsides; ++i)
    {
        side *e = vertex_sides[x->no][i];
        if ( e->color == d && e != eold)
        {
            invert_cd_path(other_vertex(e, x), e, d, c);
            return;
        }
    }
}

void edge_coloring::invert_cd_path(vertex *x, int c, int d)
{
    int nsides = n_vertex_sides[x->no];
    for (int i = 0; i < nsides; ++i)
    {
        side *e = vertex_sides[x->no][i];
        if (e->color == d)
        {
            invert_cd_path(other_vertex(e, x) , e, d, c);
            return;
        }
    }
}

side *edge_coloring::get_side(vertex *u, vertex *v)
{
    int nsides;

    nsides = n_vertex_sides[u->no];
    for (int i = 0; i < nsides; ++i)
    {
        side *e = vertex_sides[u->no][i];
        if (e->v2 == v)
            return e;
    }

    nsides = n_vertex_sides[v->no];
    for (int i = 0; i < nsides; ++i)
    {
        side *e = vertex_sides[v->no][i];
        if (e->v2 == u)
            return e;
    }

    return NULL;
}

void edge_coloring::rotate_fan(vertex *x, vertexvec::iterator begin, vertexvec::iterator end)
{
    if (begin == end) return;

    side *previous = get_side(x, *begin);
    for (begin++; begin != end; begin++)
    {
        side *current = get_side(x, *begin);
        previous->color = current->color;
        previous = current;
    }
}

int edge_coloring::color_edge(side *e)
{
    // namespace ph = boost::phoenix;

    vertex *x = e->v1, *y = e->v2;
    vertexvec fan = maximal_fan(x, y);
    int c = find_free_color(x);
    int d = find_free_color(fan.back());
    invert_cd_path(x, c, d);

    vertexvec::iterator w = std::find_if(fan.begin(), fan.end(), bind_is_free(this, d));

    // vertexvec::iterator w = std::find_if(fan.begin(),
    //                                      fan.end(),
    //                                      std::bind1st(std::mem_fun(&edge_coloring::is_free), this));
    // ph::bind(&is_free,
    //         ph::arg_names::arg1,
    //         ph::val(d)));

    rotate_fan(x, fan.begin(), w + 1);
    get_side(x, *w)->color = d;

    return std::max(c, d);
}

void edge_coloring::construct_vertex_sides(mesh &m)
{
    n_vertex_sides = new int[m.nv];
    vertex_sides = new side **[m.nv];
    std::fill_n(n_vertex_sides, m.nv, 0);

    //counting
    for (int i = 0; i < m.ns; ++i)
    {
        ++n_vertex_sides[m.s[i].v1->no];
        ++n_vertex_sides[m.s[i].v2->no];
    }

    for (int i = 0; i < m.nv; ++i)
        vertex_sides[i] = new side*[n_vertex_sides[i]];

    // inserting
    std::fill_n(n_vertex_sides, m.nv, 0);
    for (int i = 0; i < m.ns; ++i)
    {
        int vno;

        vno = m.s[i].v1->no;
        vertex_sides[vno][n_vertex_sides[vno]++] = m.s + i;

        vno = m.s[i].v2->no;
        vertex_sides[vno][n_vertex_sides[vno]++] = m.s + i;
    }
}

int edge_coloring::do_edge_coloring(mesh &m)
{
    edge_coloring ec;

    ec.construct_vertex_sides(m);

    for (int i = 0; i < m.ns; ++i)
        m.s[i].color = -1;

    int colors = 0;
    for (int i = 0; i < m.ns; ++i)
        colors = std::max(colors, ec.color_edge(m.s + i) + 1);

    delete[] ec.n_vertex_sides;
    for (int i = 0; i < m.nv; ++i)
        delete[] ec.vertex_sides[i];
    delete[] ec.vertex_sides;

    return colors;
}

