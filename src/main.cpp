#include <iostream>
#include <utilities.h>
#include <imatistl.h>
#include <map>
#include <string>

using namespace std;
const double OFFSET = 1e-4;

/**
 * @brief exec chiamata shell con output in formato testuale
 * @param cmd il comadno da eseguire nella shell
 * @return l'output del comando
 */
std::string exec(const char* cmd);

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "Missing shapefile parameter" << std::endl;
        return 1;
    }
    std::vector<std::vector<Utilities::Point3D> > boundaries;
    Utilities::load_shapefile_shp(argv[1], boundaries);
    Utilities::Point3D mean(0,0,0);
    unsigned int points_num = 0;
    for(unsigned int i = 0; i < boundaries.size(); i++)
    {
        for(unsigned int j = 0; j < boundaries[i].size(); j++)
        {
            mean = mean + boundaries[i][j];
            points_num++;
        }
    }
    mean = mean / points_num;
    for(unsigned int i = 0; i < boundaries.size(); i++)
        for(unsigned int j = 0; j < boundaries[i].size(); j++)
            boundaries[i][j] = boundaries[i][j] - mean;
    Utilities::fix_polygons(boundaries);
    std::ofstream remaining_polys_file;
    IMATI_STL::TriMesh* city = new IMATI_STL::TriMesh();

    for(unsigned int i = 0; i < boundaries.size(); i++)
    {
        std::vector<IMATI_STL::Vertex*> shell_vertices;
        std::vector<IMATI_STL::Triangle*> shell_triangles;
        if(boundaries[i][0] == boundaries[i][boundaries[i].size() - 1])
            boundaries[i].pop_back();

        for(unsigned int j = 0; j < boundaries[i].size(); j++)
        {
            shell_vertices.push_back(city->newVertex(boundaries[i][j].getX(), boundaries[i][j].getY(), boundaries[i][j].getZ()));
            city->V.appendTail(shell_vertices.back());
        }

        for(unsigned int j = 0; j < boundaries[i].size(); j++)
        {
            shell_vertices.push_back(city->newVertex(boundaries[i][j].getX(), boundaries[i][j].getY(), boundaries[i][j].getZ() + OFFSET));
            city->V.appendTail(shell_vertices.back());
        }

        IMATI_STL::ExtVertex **var = (IMATI_STL::ExtVertex **) malloc(sizeof(IMATI_STL::ExtVertex *) * shell_vertices.size());
        for(unsigned int j = 0; j < shell_vertices.size(); j++)
            var[j] = new IMATI_STL::ExtVertex(shell_vertices[j]);
        for(unsigned int j = 0; j < boundaries[i].size(); j++)
        {
            shell_triangles.push_back(city->CreateIndexedTriangle(var,
                                                                  Utilities::mod(j - 1, boundaries[i].size()),
                                                                  boundaries[i].size() + j,
                                                                  boundaries[i].size() + Utilities::mod(j - 1, boundaries[i].size())));

            shell_triangles.push_back(city->CreateIndexedTriangle(var,
                                                                  Utilities::mod(j - 1, boundaries[i].size()),
                                                                  j,
                                                                  boundaries[i].size() + j));
        }



        ofstream polygon_file("a.poly");
        if(polygon_file.is_open())
        {
            polygon_file << boundaries[i].size() << " 2 1 0" << std::endl;

            polygon_file.precision(17);
            for(unsigned int j = 0; j < boundaries[i].size(); j++)
            {
                polygon_file << j + 1 << " " << shell_vertices[j]->x << " " << shell_vertices[j]->y << std::endl;
            }

            polygon_file << boundaries[i].size() << " 0" << std::endl;
            for(unsigned int j = 0; j < boundaries[i].size() - 1; j++)
                    polygon_file << j + 1 << " " << j + 1 << " " << j + 2 << std::endl;
            polygon_file << boundaries[i].size() << " " << boundaries[i].size() << " 1"<< std::endl;
            polygon_file << "0"<< std::endl;
            polygon_file.close();
            std::cout << exec("./../triangle/triangle a.poly -pQ") << std::flush;
            std::vector<unsigned int*> triangles = Utilities::ele_file_reader("a.1.ele");
            for(unsigned int j = 0; j < triangles.size(); j++)
            {
                shell_triangles.push_back(city->CreateIndexedTriangle(var,
                                                                      triangles[j][0] - 1,
                                                                      triangles[j][1] - 1,
                                                                      triangles[j][2] - 1));

                shell_triangles.push_back(city->CreateIndexedTriangle(var,
                                                                      boundaries[i].size() + triangles[j][0] - 1,
                                                                      boundaries[i].size() + triangles[j][1] - 1,
                                                                      boundaries[i].size() + triangles[j][2] - 1));
                delete[] triangles[j];
            }
        }
        if (var != nullptr)
        {
            for (unsigned int j = 0; j < shell_vertices.size(); j++)
                delete var[j];
            free(var);
        }
    }
    city->save("city.ply");
    return 0;

}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
