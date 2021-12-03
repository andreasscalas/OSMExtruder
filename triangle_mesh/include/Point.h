#ifndef POINT_H
#define POINT_H

#include <limits>
#include <math.h>
#include <iostream>

enum class BracketsType
{
    ROUND,
    SQUARE,
    NONE
};

class Point
{
public:

    static constexpr double EPSILON = 1e-8;
    Point(){
        x = std::numeric_limits<double>::max();
        y = std::numeric_limits<double>::max();
        z = std::numeric_limits<double>::max();
        info = nullptr;
    }

    Point(double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
        info = nullptr;
    }


    double norm() { return std::sqrt(x * x + y * y + z * z); }

    void print(std::ostream &stream, BracketsType brackets_type = BracketsType::ROUND, std::string separator = ",") {
        stream.precision(15);
        std::string opening_bracket;
        std::string closing_bracket;
        switch(brackets_type)
        {
            case BracketsType::ROUND:
            {
                opening_bracket = "(";
                closing_bracket = ")";
                break;
            }
            case BracketsType::SQUARE:
            {
                opening_bracket = "[";
                closing_bracket = "]";
                break;
            }
            case BracketsType::NONE:
            {
                opening_bracket = "";
                closing_bracket = "";
                break;
            }
        }

        stream << opening_bracket << x << separator << y << separator << z << closing_bracket << std::endl;
    }

    inline double getX() const { return x; }
    inline void setX(double newX){ x = newX; }
    inline double getY() const { return y; }
    inline void setY(double newY) { y = newY; }
    inline double getZ() const { return z; }
    inline void setZ(double newZ) { z = newZ; }


    inline bool operator==(const Point other) const
    {
        return ((*this) - other).norm() < EPSILON;
    }

    inline bool operator!=(const Point other) const
    {
        return !(*this==other);
    }

    inline Point operator+(const Point other) const
    {
        return Point(x + other.getX(), y + other.getY(), z + other.getZ());
    }

    inline void operator+=(const Point other)
    {
        this->x = x + other.getX();
        this->y = y + other.getY();
        this->z = z + other.getZ();
    }

    inline Point operator-(const Point other) const
    {
        return Point(x - other.getX(), y - other.getY(), z - other.getZ());
    }

    inline void operator-=(const Point other)
    {
        this->x = x - other.getX();
        this->y = y - other.getY();
        this->z = z - other.getZ();
    }

    inline double operator*(const Point other) const
    {
        return x * other.getX() + y * other.getY() + z * other.getZ();
    }

    inline Point operator*(const double val) const
    {
        return Point(x * val, y * val, z * val);
    }

    inline void operator*=(const double val)
    {
        x = x * val;
        y = y * val;
        z = z * val;
    }

    inline Point operator&(const Point other) const
    {
        return Point(y * other.z - z * other.getY(), z * other.getX() - x * other.z, x * other.getY() - y * other.getX());
    }

    inline void operator&=(const Point other)
    {
        this->x = y * other.z - z * other.getY();
        this->y = z * other.getX() - x * other.z;
        this->z = x * other.getY() - y * other.getX();
    }

    inline Point operator/(const double val) const
    {
        return Point(x / val, y / val, z / val);
    }

    inline void operator/=(const double val)
    {
        x = x / val;
        y = y / val;
        z = z / val;
    }

    inline void operator=(const Point other)
    {
        this->x = other.getX();
        this->y = other.getY();
        this->z = other.getZ();
        this->info = other.info;
    }

    inline double* toDoubleArray() const
    {
        double* doublePoint = new double(3);
        doublePoint[0] = x;
        doublePoint[1] = y;
        doublePoint[2] = z;
        return doublePoint;
    }

    inline double computeAngle(Point p)
    {
        double angle;
        angle = atan2(((*this) & p).norm(), (*this) * p);
        return angle;
    }

    inline double distanceFromLine(const Point A, const Point B) const
    {
        Point BA = B - A;
        double ba_length = BA.norm();

        if (ba_length == 0.0) std::cerr << "distanceFromLine : Degenerate line passed !" << std::endl;

        return ((((*this) - A) & BA).norm()) / (ba_length);
    }

    inline double computePointSegmentDistance(const Point A, const Point B) const
    {
        Point p(this->getX(), this->getY(), this->getZ());
        Point AP = (A) - p;
        Point BP = (B) - p;
        double ap_length = AP.norm();
        double bp_length = BP.norm();

        if (ap_length == 0 || bp_length == 0.0) return 0.0;

        Point AB = (A) - (B);
        double ab_length = AP.norm();
        Point BA = (B) - (A);

        if (ab_length * ap_length == 0.0 || ab_length * bp_length == 0.0) return ap_length;

        if (AB.computeAngle(AP) > M_PI / 2.0) return ap_length;
        else if (BA.computeAngle(BP) > M_PI / 2.0) return bp_length;

        return distanceFromLine(A,B);
    }

    inline Point computeProjectionOnLine(Point p1, Point p2)
    {
        Point n = p2 - p1;
        n /= n.norm();
        Point v = (*this) - p1;
        return p1 + n * (v * n);

    }

    inline double computePointPlaneDistance(Point origin, Point normal)
    {
        Point po = (*this) - origin;
        return po * normal;
    }

    void *getInfo() const;
    void setInfo(void *value);

protected:
    double x;
    double y;
    double z;
    void* info; //AAAAAARGH IL RITORNO


};

inline void *Point::getInfo() const
{
    return info;
}

inline void Point::setInfo(void *value)
{
    info = value;
}
#endif // POINT_H
