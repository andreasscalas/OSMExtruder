#include <iostream>
#include <utilities.h>
#include <map>
#include <string>
#include <sstream>
#include <trianglehelper.h>
#include <Point.h>
#include <geotiff.h>
#include <coordsconverter.h>

using namespace std;
const double OFFSET = 25e-7;


int main(int argc, char *argv[])
{

    Point p1 (1.0,0.0,0.0);
    p1 /= p1.norm();
    Point p2 (0.8,0.5,0.0);
    p2 /= -p2.norm();
    std::cout << p2.computeAngle(p1) << std::endl << std::flush;

    /*if(argc < 2)
    {
        std::cerr << "Missing buildings shapefile parameter" << std::endl;
        return 1;
    }
    std::vector<std::vector<std::shared_ptr<Point> > > boundaries;
    Utilities::load_shapefile_shp(argv[1], boundaries);
    CoordsConverter converter(32633);

    for(unsigned int i = 0; i < boundaries.size(); i++)
        for(unsigned int j = 0; j < boundaries.at(i).size(); j++)
        {
            std::cout << "New point" << std::endl;
            boundaries.at(i).at(j)->print(std::cout, BracketsType::NONE, " ");
            double x, y, u1, u2;
            converter.convertToWgs84(boundaries.at(i).at(j)->getX(), boundaries.at(i).at(j)->getY(), x, y);
            std::cout << x << " " << y << std::endl << std::flush;
            converter.convertToUTM(x, y, u1, u2);
            std::cout << u1 << " " << u2 << std::endl << std::flush;
        }*/
    return 0;

}
