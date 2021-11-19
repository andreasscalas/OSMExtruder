#ifndef VERTEX_H
#define VERTEX_H

#include <Point.h>
#include <Edge.h>
#include <Triangle.h>
#include <CommonDefinitions.h>
#include <memory>
#include <vector>

class Edge;
class Triangle;


class Vertex : public Point
{
public:
    Vertex();
    Vertex(double x, double y, double z);
    Vertex(double* p);
    Vertex(Point p);
    Vertex(Point* p);
    Vertex(Vertex*  p);
    ~Vertex();

    Edge* getE0() const;
    void setE0(Edge* newE0);
    const std::vector<void *> &getInformation() const;
    void setInformation(const std::vector<void *> &newInformation);
    const std::vector<FlagType> &getAssociatedFlags() const;
    void setAssociatedFlags(const std::vector<FlagType> &newAssociatedFlags);
    std::vector<Vertex* > getVV();
    std::vector<Edge* > getVE();
    std::vector<Triangle* > getVT();
    Edge* getCommonEdge(Vertex*  v);
    Point computeNormal();

    bool addFlag(FlagType);
    int searchFlag(FlagType);
    bool removeFlag(FlagType);
    bool removeFlag(unsigned int);
    bool addInformation(void*);
    int searchInfo(void*);
    bool removeInfo(void*);
    bool removeInfo(unsigned int);

    unsigned int getId() const;
    void setId(unsigned int newId);
    void print(std::ostream&);

protected:
    unsigned int id;
    Edge* e0;
    std::vector<void*> information;
    std::vector<FlagType> associated_flags;
};

#endif // VERTEX_H
