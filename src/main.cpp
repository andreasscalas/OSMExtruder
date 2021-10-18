#include <iostream>
#include <utilities.h>
#include <map>
#include <string>
#include <trianglehelper.h>
#include <Point3D.h>

using namespace std;
const double OFFSET = 1e-4;


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "Missing shapefile parameter" << std::endl;
        return 1;
    }
    std::vector<std::vector<Point3D> > boundaries;
    std::vector<std::vector<unsigned int> > triangles;
    Utilities::load_shapefile_shp(argv[1], boundaries);
    Point3D mean(0,0,0);
    unsigned int points_number = 0;
    unsigned int triangles_number = 0;
    for(unsigned int i = 0; i < boundaries.size(); i++)
    {
        for(unsigned int j = 0; j < boundaries[i].size(); j++)
        {
            mean = mean + boundaries[i][j];
            points_number++;
        }
    }
    mean = mean / points_number;
    for(unsigned int i = 0; i < boundaries.size(); i++)
        for(unsigned int j = 0; j < boundaries[i].size(); j++)
            boundaries[i][j] = boundaries[i][j] - mean;
    Utilities::fix_polygons(boundaries);


    points_number = 0;
    for(unsigned int i = 0; i < boundaries.size(); i++)
    {
        std::vector<unsigned int> shell_triangles;
        if(boundaries[i][0] == boundaries[i][boundaries[i].size() - 1])
            boundaries[i].pop_back();
        unsigned int size = boundaries[i].size();

        for(unsigned int j = 0; j < size; j++)
            boundaries[i].push_back(Point3D(boundaries[i][j].getX(), boundaries[i][j].getY(), boundaries[i][j].getZ() + OFFSET));

        points_number += boundaries[i].size();
        for(unsigned int j = 0; j < size; j++)
        {
            shell_triangles.push_back(Utilities::mod(j - 1, size));
            shell_triangles.push_back(size + j);
            shell_triangles.push_back(size + Utilities::mod(j - 1, size));

            shell_triangles.push_back(Utilities::mod(j - 1, size));
            shell_triangles.push_back(j);
            shell_triangles.push_back(size + j);
        }

        std::vector<Point3D> polygon_base(boundaries[i].begin(), boundaries[i].begin() + size);
        std::vector<std::vector<Point3D> > polygon_base_holes;
        TriHelper::TriangleHelper helper(polygon_base, polygon_base_holes);
        std::vector<unsigned int> enclosing_triangles = helper.getTriangles();

        for(unsigned int j = 0; j < enclosing_triangles.size() / 3; j++)
        {
            shell_triangles.push_back(enclosing_triangles[j * 3]);
            shell_triangles.push_back(enclosing_triangles[j * 3 + 1]);
            shell_triangles.push_back(enclosing_triangles[j * 3 + 2]);

            shell_triangles.push_back(size + enclosing_triangles[j * 3]);
            shell_triangles.push_back(size + enclosing_triangles[j * 3 + 1]);
            shell_triangles.push_back(size + enclosing_triangles[j * 3 + 2]);
        }
        triangles_number += shell_triangles.size();
        triangles.push_back(shell_triangles);
    }

    triangles_number /= 3;
    for(unsigned int i = 0; i < boundaries.size(); i++)
        for(unsigned int j = 0; j < boundaries[i].size(); j++)
            boundaries[i][j] = boundaries[i][j] + mean;
    ofstream city("city.ply");
    city.precision(20);
    if(city.is_open())
    {
        city << "ply" << std::endl;
        city << "format ascii 1.0" << std::endl;
        city << "element vertex " << points_number << std::endl;
        city << "property double x" << std::endl;
        city << "property double y" << std::endl;
        city << "property double z" << std::endl;
        city << "element face " << triangles_number <<std::endl;
        city << "property list uchar int vertex_indices" << std::endl;
        city << "end_header" << std::endl;

        for(unsigned int i = 0; i < boundaries.size(); i++)
            for(unsigned int j = 0; j < boundaries[i].size(); j++)
                city << boundaries[i][j].getX() << " "  << boundaries[i][j].getY() << " "  << boundaries[i][j].getZ() << " " << std::endl;

        unsigned int triangles_index_offset = 0;
        for(unsigned int i = 0; i < triangles.size(); i++)
        {
            if(i > 0)
                triangles_index_offset += boundaries[i - 1].size();
            for(unsigned int j = 0; j < triangles[i].size() / 3; j++)
                city << "3 " << triangles_index_offset + triangles[i][j * 3] << " "
                             << triangles_index_offset + triangles[i][j * 3 + 1] << " "
                             << triangles_index_offset + triangles[i][j * 3 + 2] << std::endl;
        }

        city.close();
    }
    return 0;

}
