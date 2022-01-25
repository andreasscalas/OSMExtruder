#include <iostream>
#include <utilities.h>
#include <map>
#include <string>
#include <sstream>
#include <trianglehelper.h>
#include <Point.h>
#include <geotiff.h>
#include <annotationfilemanager.h>
#include <surfaceannotation.h>
#include <coordsconverter.h>
#include <KDTreeVectorOfVectorsAdaptor.h>
#include <pointannotation.h>

#include <document.h>
#include <filereadstream.h>

using namespace std;

static const int BUFFER_SIZE = 65536;


int main(int argc, char *argv[])
{

    const unsigned char RED[3] =    {255,0,0};
    const unsigned char GREEN[3] =  {0,255,0};
    const unsigned char GRAY[3] =   {220,220,220};
    const unsigned char BLUE[3] =   {0,0,255};
    const unsigned char WHITE[3] =  {255,255,255};
    const unsigned char BLACK[3] =  {0,0,0};
    const unsigned char BROWN[3] =  {98,76,54};
    const unsigned char BEIGE[3] =  {245,245,220};
    if(argc < 6)
    {
        std::cerr << "Missing one or more file parameters" << std::endl;
        return 1;
    }

    std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>();
    mesh->load(argv[1]);
    AnnotationFileManager manager;
    manager.setMesh(mesh.get());
    manager.readAnnotations(argv[2]);
    std::vector<std::shared_ptr<Annotation> > annotations = mesh->getAnnotations();
    std::vector<std::shared_ptr<SurfaceAnnotation> > buildings;
    for(unsigned int i = 0; i < annotations.size(); i++)
    {
        std::shared_ptr<SurfaceAnnotation> a = dynamic_pointer_cast<SurfaceAnnotation>(annotations.at(i));
        if(a != nullptr && a->getTag().find("building") != std::string::npos)
            buildings.push_back(a);
    }

    std::string delim = ",";
    std::ifstream istream(argv[3]);
    std::string folder = argv[3];
    folder = folder.substr(0, folder.find_last_of("/") + 1);


    std::vector<std::vector<std::string> > pois;
    std::vector<double> pois_occupancy;
    if (istream.is_open())
    {
        std::string line;
        while ( getline (istream, line) )
        {
            std::vector<std::string> line_fields;
            unsigned long start = 0;
            unsigned long end = line.find(delim);
            while(end != std::string::npos)
            {
                line_fields.push_back(line.substr(start, end - start));
                start = end + delim.length();
                end = line.find(delim, start);
            }
            pois.push_back(line_fields);
        }
        istream.close();
    }

    std::vector<std::string> occupancy_files = Utilities::globVector(folder + "*.json");
    for(unsigned int i = 0; i < occupancy_files.size(); i++)
        pois_occupancy.push_back(0);

    for(unsigned int i = 0; i < occupancy_files.size(); i++)
    {
        int poi_pos = std::atoi(occupancy_files.at(i).substr(
                                             occupancy_files.at(i).find_last_of("_") + 1,
                                             occupancy_files.at(i).find_last_of(".")).c_str());

        FILE* fp = fopen(occupancy_files.at(i).c_str(),"r");
        if(fp != nullptr)
        {
            char buffer[BUFFER_SIZE];
            rapidjson::FileReadStream frs(fp, buffer, sizeof (buffer));

            rapidjson::Document document;
            if(!(document.ParseStream(frs).HasParseError())){
                std::string member = "POI_occupancy_" + std::to_string(poi_pos);
                assert(document.HasMember("time [minutes past 00:00]") &&
                       document["time [minutes past 00:00]"].IsArray() &&
                       document.HasMember(member.c_str()) &&
                       document[member.c_str()].IsArray());

                rapidjson::Value& timeList = document["time [minutes past 00:00]"];
                rapidjson::Value& occupancyList = document[member.c_str()];
                assert(timeList.Size() == occupancyList.Size());
                for (rapidjson::SizeType i = 0; i < occupancyList.Size(); i++)
                {
                    if(timeList[i].GetInt() / 60 == atoi(argv[4]) && timeList[i].GetInt() % 60 == atoi(argv[5]))
                        pois_occupancy.at(poi_pos - 1) = occupancyList[i].GetDouble();
                }

            }
            fclose(fp);
        }
    }
    Utilities::my_vector_of_vectors_t points_vector;
//    for(unsigned int i = 0; i < mesh->getVerticesNumber(); i++)
//    {
//        std::vector<double> point = { mesh->getVertex(i)->getX(), mesh->getVertex(i)->getY(), 0};
//        points_vector.push_back(point);
//    }
    unsigned int counter = 0;
    std::map<unsigned int, unsigned int> index_to_annotation;
    for(unsigned int i = 0; i < buildings.size(); i++)
    {
        std::vector<std::shared_ptr<Vertex> > involved = buildings.at(i)->getInvolvedVertices();
        for(unsigned int j = 0; j < involved.size(); j++)
        {
            std::vector<double> point = { involved.at(j)->getX(), involved.at(j)->getY(), 0};
            points_vector.push_back(point);
            index_to_annotation.insert(std::make_pair(counter++, i));
        }
    }
    Utilities::my_kd_tree_t* mat_index = new Utilities::my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
    mat_index->index->buildIndex();

    mesh->clearAnnotations();
    CoordsConverter converter(32633);
    for(unsigned int i = 1; i < pois.size(); i++)
    {
        bool found = false;
        unsigned int associated_building = -1;
        double x, y;
        converter.convertToUTM(std::stod(pois.at(i).at(3)), std::stod(pois.at(i).at(4)), x, y);
        std::shared_ptr<Point> poi_location = std::make_shared<Point>(x, y, 0);

        for(unsigned int j = 0; j < buildings.size(); j++)
        {
            std::vector<std::vector<std::shared_ptr<Vertex> > > outlines = buildings.at(j)->getOutlines();
            if(outlines.size() != 1)
            {
                std::cerr << "Buildings' annotation can have only one boundary, while annotation n° " << j << "have more" << std::endl;
                return 1;
            }

            std::vector<std::shared_ptr<Point> > projected_outline;
            for(unsigned int k = 0; k < outlines.at(0).size() - 1; k++)
                projected_outline.push_back(std::make_shared<Point>(outlines.at(0).at(k)->getX(),outlines.at(0).at(k)->getY(), 0));
            if(Utilities::isPointInsidePolygon(poi_location, projected_outline))
            {
                std::cout << "Found! POI n° " << i - 1 << " is inside building n° " << j << std::endl;
                associated_building = j;
                found = true;
                break;
            }
        }
        std::cout.precision(15);
        if(!found)
        {
            double lat, lon;
            converter.convertToWgs84(x,y,lat,lon);
//            std::cout << x << " " << y << " " << lat << " " << lon << std::endl;

            double query_point[3] = {x, y, 0};
            std::vector<size_t> ret_index(1);
            std::vector<double> out_dist_sqr(1);
            mat_index->index->knnSearch(query_point, 1, &ret_index[0], &out_dist_sqr[0]);

            associated_building = index_to_annotation.at(ret_index[0]);
            std::cout << "POI n° " << i - 1 << " is outside any building. The closest one is building n°: " << associated_building << std::endl;

//            mesh->getVertex(ret_index[0])->print(std::cout);
//            std::shared_ptr<PointAnnotation> ann = std::make_shared<PointAnnotation>();
//            ann->setId(id++);
//            ann->addPoint(mesh->getVertex(ret_index[0]));
//            ann->setMesh(mesh.get());
//            unsigned char color[3] = {0,0,0};
//            ann->setColor(color);
//            ann->setTag("Closest");
//            mesh->addAnnotation(ann);
        }

        if(found)
        {
            unsigned char color[3];
            Utilities::interpolate(WHITE, RED, pois_occupancy.at(i - 1), color);
            std::cout << pois_occupancy.at(i - 1) << " " << static_cast<unsigned int>(color[0]) << " " << static_cast<unsigned int>(color[1]) << " " << static_cast<unsigned int>(color[2]) << std::endl;
            buildings.at(associated_building)->setColor(color);
            mesh->addAnnotation(buildings.at(associated_building));
        }
        poi_location.reset();
    }
    if(!manager.writeAnnotations("pois.ant"))
        std::cerr << "Error writing annotations" << std::endl;
//    if(!manager.writeAnnotations("errors.ant"))
//        std::cerr << "Error writing annotations" << std::endl;

    std::cout << "Ended!" << std::endl;

    return 0;

}
