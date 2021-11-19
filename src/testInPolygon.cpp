#include <iostream>
#include <utilities.h>
#include <map>
#include <string>
#include <sstream>
#include <trianglehelper.h>
#include <Point.h>
#include <geotiff.h>

using namespace std;
const double OFFSET = 25e-7;


int main(int argc, char *argv[])
{

    if(argc < 2)
    {
        std::cerr << "Missing buildings shapefile parameter" << std::endl;
        return 1;
    }
    std::vector<std::vector<Point*> > boundaries;
    Utilities::load_shapefile_shp(argv[1], boundaries);

    for(unsigned int i = 0; i < boundaries.size(); i++)
        for(unsigned int j = 0; j < boundaries.at(i).size(); j++)
            boundaries.at(i).at(j)->print(std::cout, BracketsType::NONE, " ");
    return 0;

}
