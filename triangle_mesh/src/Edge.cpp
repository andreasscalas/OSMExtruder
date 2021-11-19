#include "Edge.h"
#include <algorithm>

Edge::Edge()
{
    setId(-1);
    v1 = nullptr;
    v2 = nullptr;
    t1 = nullptr;
    t2 = nullptr;
}

Edge::Edge(Vertex* v1, Vertex* v2)
{
    setId(-1);
    setV1(v1);
    setV2(v2);
    t1 = nullptr;
    t2 = nullptr;
    v1->setE0(this);
    v2->setE0(this);
}

Edge::Edge(Edge* e)
{
    setId(e->getId());
    setV1(e->getV1());
    setV2(e->getV2());
    setT1(e->getT1());
    setT2(e->getT2());
    this->associated_flags.insert(this->associated_flags.end(), e->getAssociatedFlags().begin(), e->getAssociatedFlags().end());
    this->information.insert(this->information.end(), e->getInformation().begin(), e->getInformation().end());
}

Edge::~Edge()
{
    if(v1 != nullptr && v1->getE0() == this)
    {
        std::vector<Edge*> ve = v1->getVE();
        unsigned int i = 0;
        while(ve.at(i) == this) i++;
        v1->setE0(ve.at(i));
    }
    if(v2 != nullptr && v2->getE0() == this)
    {
        std::vector<Edge*> ve = v2->getVE();
        unsigned int i = 0;
        while(ve.at(i) == this) i++;
        v2->setE0(ve.at(i));
    }
    if(t1 != nullptr)
        if(t1->getE1() == this)
            t1->setE1(nullptr);
        else if(t1->getE2() == this)
            t1->setE2(nullptr);
        else if(t1->getE3() == this)
            t1->setE3(nullptr);

    if(t2 != nullptr)
        if(t2->getE1() == this)
            t2->setE1(nullptr);
        else if(t2->getE2() == this)
            t2->setE2(nullptr);
        else if(t2->getE3() == this)
            t2->setE3(nullptr);


}

Vertex* Edge::getV1() const
{
    return v1;
}

void Edge::setV1(Vertex* newV1)
{
    v1 = newV1;
}

Vertex* Edge::getV2() const
{
    return v2;
}

void Edge::setV2(Vertex* newV2)
{
    v2 = newV2;
}

const std::vector<void *> &Edge::getInformation() const
{
    return information;
}

void Edge::setInformation(const std::vector<void *> &newInformation)
{
    information = newInformation;
}

const std::vector<FlagType> &Edge::getAssociatedFlags() const
{
    return associated_flags;
}

void Edge::setAssociatedFlags(const std::vector<FlagType> &newAssociatedFlags)
{
    associated_flags = newAssociatedFlags;
}

Vertex* Edge::getCommonVertex(Edge* e)
{
    if(e->hasVertex(v1))
        return v1;
    if(e->hasVertex(v2))
        return v2;
    return nullptr;

}

Vertex* Edge::getOppositeVertex(Vertex* v)
{
    if(v1 == v)
        return v2;
    if(v2 != nullptr && v2 == v)
        return v1;

    return nullptr;
}

Triangle* Edge::getLeftTriangle(Vertex*  v)
{
    Vertex* v_;
    if(v1 == v)
        v_ = v2;
    else if(v2 == v)
        v_ = v1;
    else
        return nullptr;
    if(t1 != nullptr)
    {
        Vertex* v__ = t1->getOppositeVertex(this);
        Point vec1 = *v_ - *v;
        Point vec2 = *v__ - *v;
        Point cross = vec1 & vec2;
        Point normal = t1->computeNormal();
        if(cross * normal >= 0)
        {
            return t1;
        }
    }
    if(t2 != nullptr)
    {
        Vertex* v__ = t2->getOppositeVertex(this);
        Point vec1 = *v_ - *v;
        Point vec2 = *v__ - *v;
        Point cross = vec1 & vec2;
        Point normal = t2->computeNormal();
        if(normal * cross >= 0)
        {
            return t2;
        }
    }
    return nullptr;
}

Triangle* Edge::getRightTriangle(Vertex*  v)
{
    Vertex* v_;
    if(v1 == v)
        v_ = v2;
    else if(v2 == v)
        v_ = v1;
    else
        return nullptr;
    if(t1 != nullptr)
    {
        Vertex* v__ = t1->getOppositeVertex(this);
        Point vec1 = *v_ - *v;
        Point vec2 = *v__ - *v;
        Point cross = vec1 & vec2;
        Point normal = t1->computeNormal();
        if(cross * normal <= 0)
        {
            return t1;
        }
    }
    if(t2 != nullptr)
    {
        Vertex* v__ = t2->getOppositeVertex(this);
        Point vec1 = *v_ - *v;
        Point vec2 = *v__ - *v;
        Point cross = vec1 & vec2;
        Point normal = t2->computeNormal();
        if(normal * cross <= 0)
        {
            return t2;
        }
    }
    return nullptr;
}

bool Edge::hasVertex(Vertex*  v)
{
    return (v1 == v || v2 == v);
}


Triangle* Edge::getT1() const
{
    return t1;
}

void Edge::setT1(Triangle* newT1)
{
    t1 = newT1;
}

Triangle* Edge::getT2() const
{
    return t2;
}

void Edge::setT2(Triangle* newT2)
{
    t2 = newT2;
}


bool Edge::addFlag(FlagType f)
{
    if(std::find(associated_flags.begin(), associated_flags.end(), f) == associated_flags.end())
    {
        associated_flags.push_back(f);
        return true;
    }
    return false;
}

int Edge::searchFlag(FlagType f)
{
    std::vector<FlagType>::iterator it = std::find(associated_flags.begin(), associated_flags.end(), f);
    if(it != associated_flags.end())
        return  it - associated_flags.begin();
    return -1;
}

bool Edge::removeFlag(FlagType f)
{
    int flagPosition = searchFlag(f);
    if(flagPosition >= 0)
    {
        associated_flags.erase(associated_flags.begin() + flagPosition);
        return true;
    }
    return false;

}

bool Edge::removeFlag(unsigned int position)
{
    if(position < associated_flags.size())
    {
        associated_flags.erase(associated_flags.begin() + position);
        return true;
    }
    return false;

}

bool Edge::addInformation(void *info)
{
    if(std::find(information.begin(), information.end(), info) == information.end())
    {
        information.push_back(info);
        return true;
    }
    return false;
}

int Edge::searchInfo(void * info)
{
    std::vector<void*>::iterator it = std::find(information.begin(), information.end(), info);
    if(it != information.end())
        return  it - information.begin();
    return -1;
}

bool Edge::removeInfo(void *info)
{
    int flagPosition = searchInfo(info);
    if(flagPosition >= 0)
    {
        information.erase(information.begin() + flagPosition);
        return true;
    }
    return false;
}

bool Edge::removeInfo(unsigned int position)
{
    if(position < information.size())
    {
        information.erase(information.begin() + position);
        return true;
    }
    return false;
}

unsigned int Edge::getId() const
{
    return id;
}

void Edge::setId(unsigned int newId)
{
    id = newId;
}

double Edge::computeLength() const
{
    return ((*v1) - (*v2)).norm();
}

void Edge::print(std::ostream &stream)
{
    stream << "Edge with id: " << id << " connecting vertices with ids: " << v1->getId() << " and " << v2->getId()<< ". Incident triangles have ids: "  << std::endl;
    if(t1 != nullptr)
        stream << t1->getId();
    else
        stream << "not available";
    stream << " and ";
    if(t2 != nullptr)
        stream << t2->getId();
    else
        stream << "not available";
    stream << std::endl;
}
