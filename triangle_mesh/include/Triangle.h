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
    Triangle(std::shared_ptr<Edge>, std::shared_ptr<Edge>, std::shared_ptr<Edge>);
    Triangle(std::shared_ptr<Triangle>);
    ~Triangle();
    unsigned int getId() const;
    void setId(unsigned int newId);
    std::shared_ptr<Vertex>  getV1() const;
    std::shared_ptr<Vertex>  getV2() const;
    std::shared_ptr<Vertex>  getV3() const;
    std::shared_ptr<Edge> getE1() const;
    void setE1(std::shared_ptr<Edge> newE1);
    std::shared_ptr<Edge> getE2() const;
    void setE2(std::shared_ptr<Edge> newE2);
    std::shared_ptr<Edge> getE3() const;
    void setE3(std::shared_ptr<Edge> newE3);
    std::shared_ptr<Vertex> getNextVertex(std::shared_ptr<Vertex> v);
    const std::vector<void *> &getInformation() const;
    void setInformation(const std::vector<void *> &newInformation);
    const std::vector<FlagType> &getAssociatedFlags() const;
    void setAssociatedFlags(const std::vector<FlagType> &newAssociatedFlags);

    std::shared_ptr<Edge> getOppositeEdge(std::shared_ptr<Vertex> ) const;
    std::shared_ptr<Vertex> getOppositeVertex(std::shared_ptr<Edge>) const;
    bool hasEdge(std::shared_ptr<Edge>);
    std::shared_ptr<Edge> getCommonEdge(std::shared_ptr<Triangle> ) const;
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
    std::shared_ptr<Edge> e1;
    std::shared_ptr<Edge> e2;
    std::shared_ptr<Edge> e3;
    std::vector<void*> information;
    std::vector<FlagType> associated_flags;
};



#endif // TRIANGLE_H
