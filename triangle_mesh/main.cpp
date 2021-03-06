#include <iostream>
#include <fstream>
#include <TriangleMesh.h>
#include <trianglehelper.h>
int main(int argc, char* argv[])
{
    if(argc < 2)
        return -1;

    std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>();
//    mesh->load(argv[1]);
//    mesh->save("prova.ply");

    std::vector<std::vector<std::shared_ptr<Point>> > boundaries;
    std::vector<std::vector<std::shared_ptr<Point>> > constraints;
    std::vector<std::shared_ptr<Point>> boundary = {
        std::make_shared<Point>(0,-3,0),
        std::make_shared<Point>(3,0,0),
        std::make_shared<Point>(0,3,0),
        std::make_shared<Point>(-3,0,0)
    };
    boundaries.push_back(boundary);
    std::vector<std::shared_ptr<Point>> hole = {
        std::make_shared<Point>(-1,-1,0),
        std::make_shared<Point>(-1,1,0),
        std::make_shared<Point>(1,1,0),
        std::make_shared<Point>(1,-1,0)
    };
    boundaries.push_back(hole);

    std::vector<std::shared_ptr<Point>> constraint = {
        std::make_shared<Point>(-0.5,0.5,0),
        std::make_shared<Point>(-0.5,-0.5,0)
    };
    constraints.push_back(constraint);
    mesh->triangulate(boundaries, constraints);
//    TriHelper::TriangleHelper helper(boundary, boundaries, constraints);
//    std::vector<unsigned int> triangles = helper.getTriangles();
//    for(unsigned int i = 0; i < triangles.size() / 3; i++)
//        std::cout << triangles[i * 3] << " " << triangles[i * 3 + 1] << " " << triangles[i * 3 + 2] << std::endl;
    mesh->save("prova_triangulation.ply");
    mesh.reset();
    return 0;
}
