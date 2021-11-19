#include <Triangle.h>
#include <algorithm>

Triangle::Triangle()
{
    id = -1;
    e1 = nullptr;
    e2 = nullptr;
    e3 = nullptr;
}

Triangle::Triangle(Edge* e1, Edge* e2, Edge* e3)
{
    setId(-1);
    setE1(e1);
    setE2(e2);
    setE3(e3);
}

Triangle::Triangle(Triangle* t)
{
    setId(t->getId());
    setE1(e1);
    setE2(e2);
    setE3(e3);
    this->associated_flags.insert(this->associated_flags.end(), t->getAssociatedFlags().begin(), t->getAssociatedFlags().end());
    this->information.insert(this->information.end(), t->getInformation().begin(), t->getInformation().end());
}

Triangle::~Triangle()
{
    if(e1 != nullptr)
        if(e1->getT1() == this)
            e1->setT1(nullptr);
        else if(e1->getT2() == this)
            e1->setT2(nullptr);
    if(e2 != nullptr)
        if(e2->getT1() == this)
            e2->setT1(nullptr);
        else if(e2->getT2() == this)
            e2->setT2(nullptr);
    if(e3 != nullptr)
        if(e3->getT1() == this)
            e3->setT1(nullptr);
        else if(e3->getT2() == this)
            e3->setT2(nullptr);
}

Edge* Triangle::getE1() const
{
    return e1;
}

void Triangle::setE1(Edge* newE1)
{
    e1 = newE1;
}

Edge* Triangle::getE2() const
{
    return e2;
}

void Triangle::setE2(Edge* newE2)
{
    e2 = newE2;
}

Edge* Triangle::getE3() const
{
    return e3;
}

void Triangle::setE3(Edge* newE3)
{
    e3 = newE3;
}

Vertex *Triangle::getNextVertex(Vertex *v)
{
    if(e1->hasVertex(v))
        return e1->getOppositeVertex(v);
    if(e2->hasVertex(v))
        return e2->getOppositeVertex(v);
    if(e3->hasVertex(v))
        return e3->getOppositeVertex(v);
    return nullptr;
}

const std::vector<void *> &Triangle::getInformation() const
{
    return information;
}

void Triangle::setInformation(const std::vector<void *> &newInformation)
{
    information = newInformation;
}

const std::vector<FlagType> &Triangle::getAssociatedFlags() const
{
    return associated_flags;
}

void Triangle::setAssociatedFlags(const std::vector<FlagType> &newAssociatedFlags)
{
    associated_flags = newAssociatedFlags;
}

bool Triangle::hasEdge(Edge* e)
{
    return e1 == e || e2 == e || e3 == e;
}

Vertex* Triangle::getV1() const
{
    return e3->getCommonVertex(e1);
}

Vertex*  Triangle::getV2() const
{
    return e1->getCommonVertex(e2);
}

Vertex* Triangle::getV3() const
{
    return e2->getCommonVertex(e3);
}

Edge* Triangle::getOppositeEdge(Vertex* v) const
{
    if(!e1->hasVertex(v) && (e2->hasVertex(v) || e3->hasVertex(v)))
        return e1;
    else if (!e2->hasVertex(v) && (e1->hasVertex(v) || e3->hasVertex(v)))
        return e2;
    else if (!e3->hasVertex(v) && (e1->hasVertex(v) || e2->hasVertex(v)))
        return e3;
    return nullptr;
}

Vertex *Triangle::getOppositeVertex(Edge * e) const
{
    if(e1 == e)
        return e2->getCommonVertex(e3);
    else if(e2 == e)
        return e1->getCommonVertex(e3);
    else if(e3 == e)
        return e1->getCommonVertex(e2);
    return nullptr;
}

Edge*  Triangle::getCommonEdge(Triangle*  t) const
{
    if(t->hasEdge(e1))
        return e1;
    else if(t->hasEdge(e2))
        return e2;
    else if(t->hasEdge(e3))
        return e3;
    return nullptr;
}

Point Triangle::project(Point p) const
{
    Point normal = computeNormal();
    Point origin = (*getV1() + *getV2() + *getV3()) / 3;
    Point po = p - origin;
    return p - normal * (po * normal);
}


Point Triangle::computeNormal() const
{
    Point p2p1 = (*getV2()) - (*getV1());
    Point p3p1 = (*getV3()) - (*getV2());
    Point normal = p2p1 & p3p1;
    normal /= normal.norm();
    return normal;
}

void Triangle::computeBC(Point p, double &alpha, double &beta, double &gamma)
{
    Point n = computeNormal();
    Point AB = *getV2() - *getV1();
    Point AC = *getV3() - *getV1();
    Point PA = *getV1() - p;
    Point PB = *getV2() - p;
    Point PC = *getV3() - p;
    double areaABC = (AB & AC).norm() / 2.0;
    Point a1 = PB & PC;
    Point a2 = PC & PA;
    Point a3 = PA & PB;

    alpha = static_cast<long double>((n * a1 >= 0 ? a1.norm() : -a1.norm()) / (2.0 * areaABC));
    beta = static_cast<long double>((n * a2 >= 0 ? a2.norm() : -a2.norm()) / (2.0 * areaABC));
    gamma = static_cast<long double>((n * a3 >= 0 ? a3.norm() : -a3.norm()) / (2.0 * areaABC));
}

bool Triangle::isPointInside(Point p)
{
    double alfa, beta, gamma;
    computeBC(p, alfa, beta, gamma);
    long double aboutOne = static_cast<long double>(1.0 + EPSILON_BC);
    return (alfa >= 0 && alfa <= aboutOne) && (beta >= 0 && beta <= aboutOne) &&
       (gamma >= 0 && gamma <= aboutOne) && (alfa + beta + gamma <= aboutOne);
}

double Triangle::computeDistanceFromPoint(Point p, Point *c)
{
    Point projected = project(p), closest;
    double alfa, beta, gamma;

    computeBC(projected, alfa, beta, gamma);

    long double aboutOne = static_cast<long double>(1.0 + EPSILON_BC);
    if((alfa >= 0 && alfa <= aboutOne) && (beta >= 0 && beta <= aboutOne) &&
            (gamma >= 0 && gamma <= aboutOne) && (alfa + beta + gamma <= aboutOne))
        closest = projected;
    else
    {
        if(alfa >= 0.0 && beta < 0.0 && gamma < 0.0)
        {
            closest = *getV1();
        } else if(alfa < 0.0 && beta >= 0.0 && gamma < 0.0)
        {
            closest = *getV2();
        } else if(alfa < 0.0 && beta < 0.0 && gamma >= 0.0)
        {
            closest = *getV3();
        } else if (alfa >= 0.0 && beta >= 0.0 && gamma < 0.0)
        {
            closest = projected.computeProjectionOnLine(*getV1(), *getV2());
        } else if (alfa < 0.0 && beta >= 0.0 && gamma >= 0.0)
        {
            closest = projected.computeProjectionOnLine(*getV2(), *getV3());
        } else if (alfa >= 0.0 && beta < 0.0 && gamma >= 0.0)
        {
            closest = projected.computeProjectionOnLine(*getV3(), *getV1());
        } else
            closest = Point(std::numeric_limits<double>::max(),
                            std::numeric_limits<double>::max(),
                            std::numeric_limits<double>::max());
    }

    return (p - closest).norm();
}


bool Triangle::addFlag(FlagType f)
{
    if(std::find(associated_flags.begin(), associated_flags.end(), f) == associated_flags.end())
    {
        associated_flags.push_back(f);
        return true;
    }
    return false;
}

int Triangle::searchFlag(FlagType f)
{
    std::vector<FlagType>::iterator it = std::find(associated_flags.begin(), associated_flags.end(), f);
    if(it != associated_flags.end())
        return  it - associated_flags.begin();
    return -1;
}

bool Triangle::removeFlag(FlagType f)
{
    int flagPosition = searchFlag(f);
    if(flagPosition >= 0)
    {
        associated_flags.erase(associated_flags.begin() + flagPosition);
        return true;
    }
    return false;

}

bool Triangle::removeFlag(unsigned int position)
{
    if(position < associated_flags.size())
    {
        associated_flags.erase(associated_flags.begin() + position);
        return true;
    }
    return false;

}

bool Triangle::addInformation(void *info)
{
    if(std::find(information.begin(), information.end(), info) == information.end())
    {
        information.push_back(info);
        return true;
    }
    return false;
}

int Triangle::searchInfo(void * info)
{
    std::vector<void*>::iterator it = std::find(information.begin(), information.end(), info);
    if(it != information.end())
        return  it - information.begin();
    return -1;
}

bool Triangle::removeInfo(void *info)
{
    int flagPosition = searchInfo(info);
    if(flagPosition >= 0)
    {
        information.erase(information.begin() + flagPosition);
        return true;
    }
    return false;
}

bool Triangle::removeInfo(unsigned int position)
{
    if(position < information.size())
    {
        information.erase(information.begin() + position);
        return true;
    }
    return false;
}

void Triangle::print(std::ostream &stream)
{
    stream << "Triangle with id: " << id << std::endl <<
              "Connects vertices with ids: " << getV1()->getId() << ", " << getV2()->getId() << " and " << getV3()->getId() << std::endl <<
              " using edges with ids: " << getE1()->getId() << ", " << getE2()->getId() << " and " << getE3()->getId() << std::endl;
}

unsigned int Triangle::getId() const
{
    return id;
}

void Triangle::setId(unsigned int newId)
{
    id = newId;
}
