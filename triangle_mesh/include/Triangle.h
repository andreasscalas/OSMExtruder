#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <Vertex.h>
#include <Edge.h>
#include <CommonDefinitions.h>
#include <vector>
#include <memory>

class Vertex;
class Edge;
class Triangle {
public:

    static constexpr double EPSILON_BC = 1e-7;
    Triangle();
    Triangle(Edge*, Edge*, Edge*);
    Triangle(Triangle*);
    ~Triangle();
    unsigned int getId() const;
    void setId(unsigned int newId);
    Vertex*  getV1() const;
    Vertex*  getV2() const;
    Vertex*  getV3() const;
    Edge* getE1() const;
    void setE1(Edge* newE1);
    Edge* getE2() const;
    void setE2(Edge* newE2);
    Edge* getE3() const;
    void setE3(Edge* newE3);
    Vertex* getNextVertex(Vertex* v);
    const std::vector<void *> &getInformation() const;
    void setInformation(const std::vector<void *> &newInformation);
    const std::vector<FlagType> &getAssociatedFlags() const;
    void setAssociatedFlags(const std::vector<FlagType> &newAssociatedFlags);

    Edge* getOppositeEdge(Vertex* ) const;
    Vertex* getOppositeVertex(Edge*) const;
    bool hasEdge(Edge*);
    Edge* getCommonEdge(Triangle* ) const;
    Point project(Point p) const;
    Point computeNormal() const;
    void computeBC(Point p, double &alpha, double &beta, double &gamma);
    bool isPointInside(Point p);
    double squaredDistanceFromPoint(Point p) const;
    double computeDistanceFromPoint(Point p, Point *c = nullptr);


    bool addFlag(FlagType);
    int searchFlag(FlagType);
    bool removeFlag(FlagType);
    bool removeFlag(unsigned int);
    bool addInformation(void*);
    int searchInfo(void*);
    bool removeInfo(void*);
    bool removeInfo(unsigned int);

    void print(std::ostream&);

protected:
    unsigned int id;
    Edge* e1;
    Edge* e2;
    Edge* e3;
    std::vector<void*> information;
    std::vector<FlagType> associated_flags;
};



#endif // TRIANGLE_H
