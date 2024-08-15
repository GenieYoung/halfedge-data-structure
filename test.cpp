#include "halfedge.h"
#include <queue>

void build_cube()
{
    // you can use your own point instead array by override the default traits
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

void smooth()
{
    // an example of iterator, the algorithm itself should be modified
    halfedge::Graph<> g;
    g.read_obj("leaf.obj");
    for(auto vit = g.vertices_begin(); vit != g.vertices_end(); ++vit)
    {
        double x_sum = 0, y_sum = 0, z_sum = 0;
        unsigned valence = 0;
        for(auto vvit = g.vv_begin(*vit); vvit != g.vv_end(*vit); ++vvit)
        {
            x_sum += g.point(*vvit)[0];
            y_sum += g.point(*vvit)[1];
            z_sum += g.point(*vvit)[2];
            ++valence;
        }
        g.point(*vit) = {x_sum/valence, y_sum/valence, z_sum/valence};
    }
    g.write_obj("leaf_after_smooth.obj");
}

int main()
{
    build_cube();
    read_obj();
    smooth();
}