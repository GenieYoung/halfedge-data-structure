#include "halfedge.h"
#include <queue>

void build_cube()
{
    halfedge::Graph<> g;
    halfedge::VertexHandle v0 = g.add_vertex({0,0,0});
    halfedge::VertexHandle v1 = g.add_vertex({1,0,0});
    halfedge::VertexHandle v2 = g.add_vertex({1,1,0});
    halfedge::VertexHandle v3 = g.add_vertex({0,1,0});
    halfedge::VertexHandle v4 = g.add_vertex({0,0,1});
    halfedge::VertexHandle v5 = g.add_vertex({1,0,1});
    halfedge::VertexHandle v6 = g.add_vertex({1,1,1});
    halfedge::VertexHandle v7 = g.add_vertex({0,1,1});
    g.add_face(v0, v3, v2, v1);
    g.add_face(v1, v2, v6, v5);
    g.add_face(v4, v5, v6, v7);
    g.add_face(v0, v4, v7, v3);
    g.add_face(v0, v1, v5, v4);
    g.add_face(v3, v7, v6, v2);
    g.write_obj("cube.obj");
}

void read_obj()
{
    halfedge::Graph<> g;
    g.read_obj("leaf.obj");
    g.write_obj("leaf_after_read.obj");
}

int main()
{
    build_cube();
    read_obj();
}