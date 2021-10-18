#ifndef POINT3D_H
#define POINT3D_H

#include <limits>
#include <math.h>
#include <iostream>
class Point3D
{
public:

    Point3D(){
        x = std::numeric_limits<double>::max();
        y = std::numeric_limits<double>::max();
        z = std::numeric_limits<double>::max();
    }

    Point3D(double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    double norm() { return std::sqrt(x * x + y * y + z * z); }

    void print(std::ostream &stream) { stream << "(" << x << "," << y << "," << z << ")" << std::endl; }

    inline double getX() const { return x; }
    inline void setX(double newX){ x = newX; }
    inline double getY() const { return y; }
    inline void setY(double newY) { y = newY; }
    inline double getZ() const { return z; }
    inline void setZ(double newZ) { z = newZ; }


    inline bool operator==(Point3D other){
        return ((*this) - other).norm() < 1e-6;
    }

    inline bool operator!=(Point3D other){
        return !(*this==other);
    }

    inline Point3D operator+(Point3D other)
    {
        return Point3D(x + other.getX(), y + other.getY(), z + other.getZ());
    }

    inline Point3D operator-(Point3D other)
    {
        return Point3D(x - other.getX(), y - other.getY(), z - other.getZ());
    }

    inline double operator*(Point3D other)
    {
        return x * other.getX() + y * other.getY() + z * other.getZ();
    }


    inline Point3D operator*(double val)
    {
        return Point3D(x * val, y * val, z * val);
    }

    inline Point3D operator&(Point3D other)
    {
        return Point3D(y * other.z - z * other.getY(), z * other.getX() - x * other.z, x * other.getY() - y * other.getX());
    }

    inline Point3D operator/(double val)
    {
        return Point3D(x / val, y / val, z / val);
    }

    inline void operator=(Point3D other)
    {
        this->x = other.getX();
        this->y = other.getY();
        this->z = other.getZ();
    }

protected:
        double x;
        double y;
        double z;

};
#endif // POINT3D_H
