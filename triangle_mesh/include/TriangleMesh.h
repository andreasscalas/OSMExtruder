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
    Vertex* addNewVertex();
    Vertex* addNewVertex(double, double, double);
    Vertex* addNewVertex(Point);
    Vertex* addNewVertex(Vertex* );
    Vertex* getVertex(unsigned int);
    Edge* getEdge(unsigned int);
    Triangle* getTriangle(unsigned int);
    Edge* addNewEdge();
    Edge* addNewEdge(Vertex*, Vertex*);
    Edge* addNewEdge(Edge*);
    Triangle* addNewTriangle();
    Triangle* addNewTriangle(Edge*,Edge*, Edge*);
    Triangle* addNewTriangle(Triangle*);
    int load(std::string filename);
    int save(std::string filename, unsigned int precision = 5);
    unsigned int removeIsolatedVertices();
    int triangulate(std::vector<std::vector<Point*> > &boundaries, std::vector<std::vector<Point*> > &constraints);
    Edge* searchEdgeContainingVertex(std::vector<Edge*> list, Vertex* v);
protected:
    std::vector<Vertex* > vertices;
    std::vector<Edge* > edges;
    std::vector<Triangle* > triangles;

    int loadPLY(std::string filename);
};

#endif // TRIANGLEMESH_H
