#ifndef EDGE_H
#define EDGE_H

#include <Vertex.h>
#include <Triangle.h>
#include <CommonDefinitions.h>
#include <vector>
#include <memory>

class Vertex;
class Triangle;

class Edge
{
public:
    Edge();
    Edge(Vertex*, Vertex*);
    Edge(Edge*);
    ~Edge();

    Vertex*  getCommonVertex(Edge* e2);
    Vertex*  getOppositeVertex(Vertex*  v);
    Triangle* getLeftTriangle(Vertex* );
    Triangle* getRightTriangle(Vertex* v);
    bool hasVertex(Vertex* );

    Vertex* getV1() const;
    void setV1(Vertex* newV1);
    Vertex* getV2() const;
    void setV2(Vertex* newV2);
    Triangle* getT1() const;
    void setT1(Triangle* newT1);
    Triangle* getT2() const;
    void setT2(Triangle* newT2);
    const std::vector<void *> &getInformation() const;
    void setInformation(const std::vector<void *> &newInformation);
    const std::vector<FlagType> &getAssociatedFlags() const;
    void setAssociatedFlags(const std::vector<FlagType> &newAssociatedFlags);

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

    double computeLength() const;
    void print(std::ostream&);

protected:
    unsigned int id;
    Vertex*  v1;
    Vertex*  v2;
    Triangle* t1;
    Triangle* t2;
    std::vector<void*> information;
    std::vector<FlagType> associated_flags;
};

#endif // EDGE_H
