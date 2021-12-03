#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <Vertex.h>
#include <Edge.h>
#include <Triangle.h>
#include <memory>

class TriangleMesh
{
public:
    TriangleMesh();
    ~TriangleMesh();
    unsigned int getVerticesNumber();
    unsigned int getEdgesNumber();
    unsigned int getTrianglesNumber();
    std::shared_ptr<Vertex> addNewVertex();
    std::shared_ptr<Vertex> addNewVertex(double, double, double);
    std::shared_ptr<Vertex> addNewVertex(Point);
    std::shared_ptr<Vertex> addNewVertex(std::shared_ptr<Vertex> );
    std::shared_ptr<Vertex> getVertex(unsigned int);
    std::shared_ptr<Edge> getEdge(unsigned int);
    std::shared_ptr<Triangle> getTriangle(unsigned int);
    std::shared_ptr<Edge> addNewEdge();
    std::shared_ptr<Edge> addNewEdge(std::shared_ptr<Vertex>, std::shared_ptr<Vertex>);
    std::shared_ptr<Edge> addNewEdge(std::shared_ptr<Edge>);
    std::shared_ptr<Triangle> addNewTriangle();
    std::shared_ptr<Triangle> addNewTriangle(std::shared_ptr<Edge>,std::shared_ptr<Edge>, std::shared_ptr<Edge>);
    std::shared_ptr<Triangle> addNewTriangle(std::shared_ptr<Triangle>);
    int load(std::string filename);
    int save(std::string filename, unsigned int precision = 5);
    unsigned int removeIsolatedVertices();
    int triangulate(std::vector<std::vector<std::shared_ptr<Point> > > &boundaries, std::vector<std::vector<std::shared_ptr<Point> > > &constraints);
    std::shared_ptr<Edge> searchEdgeContainingVertex(std::vector<std::shared_ptr<Edge> > list, std::shared_ptr<Vertex> v);

protected:
    std::vector<std::shared_ptr<Vertex> > vertices;
    std::vector<std::shared_ptr<Edge> > edges;
    std::vector<std::shared_ptr<Triangle> > triangles;

    int loadPLY(std::string filename);
};

#endif // TRIANGLEMESH_H
