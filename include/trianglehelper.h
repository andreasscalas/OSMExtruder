#ifndef TRIANGLEHELPER_H
#define TRIANGLEHELPER_H

#include <vector>
#include <string>
#include <Point3D.h>

class ExtendedTrimesh;

namespace TriHelper {
    class TriangleHelper
    {
    public:
        TriangleHelper(std::vector<Point3D> boundary, std::vector<std::vector<Point3D> > holes);

        const std::vector<unsigned int> &getTriangles() const;

    private:
        std::vector<Point3D> boundary;
        std::vector<std::vector<Point3D> > holes;
        std::vector<unsigned int> triangles;
        const std::string filename = "tmp";
        void launchTriangle();
    };

}

#endif // TRIANGLEHELPER_H
