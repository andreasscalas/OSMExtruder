
#include <annotationfilemanager.h>
#include <KDTreeVectorOfVectorsAdaptor.h>
#include <document.h>
#include <filereadstream.h>
#include <coordsconverter.h>
#include <lineannotation.h>

typedef std::vector<std::vector<double> > my_vector_of_vectors_t;
typedef KDTreeVectorOfVectorsAdaptor< my_vector_of_vectors_t, double > my_kd_tree_t;
static unsigned char BLUE[3] =   {0,0,255};
static const int BUFFER_SIZE = 65536;
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        std::cerr << "Missing one or more file parameters" << std::endl;
        return 1;
    }

    std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>();
    mesh->load(argv[1]);

    my_vector_of_vectors_t points_vector;
    for(unsigned int i = 0; i < mesh->getVerticesNumber(); i++)
    {

        std::vector<double> point = { mesh->getVertex(i)->getX(),
                                      mesh->getVertex(i)->getY(),
                                      0};
        points_vector.push_back(point);
    }
    my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
    mat_index->index->buildIndex();

    std::string shortestPathJsonFile = argv[2];
    FILE* fp = fopen(shortestPathJsonFile.c_str(),"r");

    std::string shortestPathAnnotationFile = shortestPathJsonFile.substr(0,shortestPathJsonFile.find_last_of(".") + 1) + "ant";


    std::shared_ptr<LineAnnotation> annotation = std::make_shared<LineAnnotation>();

    if(fp != nullptr)
    {
        char buffer[BUFFER_SIZE];
        rapidjson::FileReadStream frs(fp, buffer, sizeof (buffer));

        rapidjson::Document document;
        if(!(document.ParseStream(frs).HasParseError())){

            assert(document.HasMember("Attr value") && document["Attr value"].IsDouble());
            assert(document.HasMember("Attribute") && document["Attribute"].IsString());
            assert(document.HasMember("geometry") && document["geometry"].IsArray());

            double value = document["Attr value"].GetDouble();
            std::string attributeName = document["Attribute"].GetString();
            annotation->setId(0);
            annotation->setTag(attributeName + "-based shortest path");
            annotation->setColor(BLUE);
            annotation->setHierarchyLevel(-1);
            annotation->setMesh(mesh.get());
            rapidjson::Value& path = document["geometry"].GetArray();
            std::vector<std::vector<std::shared_ptr<Vertex> > > polylines;
            std::vector<std::shared_ptr<Vertex> > polyline;
            CoordsConverter converter(32633);
            std::shared_ptr<Vertex> prev;
            for (rapidjson::SizeType i = 0; i < path.Size(); i++)
            {
                assert(path[i].IsArray());
                rapidjson::Value& obj = path[i].GetArray();
                assert(obj.Size() == 2 && obj[0].IsArray() && obj[1].IsArray());
                rapidjson::Value& source = obj[0].GetArray();
                rapidjson::Value& destination = obj[1].GetArray();
                std::shared_ptr<Vertex> sourceVertex, destinationVertex;
                assert(source.Size() == 2 && source[0].IsDouble() && source[1].IsDouble());
                assert(destination.Size() == 2 && destination[0].IsDouble() && destination[1].IsDouble());
                double lon = source[0].GetDouble();
                double lat = source[1].GetDouble();
                double x, y;
                converter.convertToUTM(lat, lon, x, y);

                double query_point[3] = {x, y, 0};
                std::vector<size_t> ret_index(1);
                std::vector<double> out_dist_sqr(1);
                mat_index->index->knnSearch(query_point, 1, &ret_index[0], &out_dist_sqr[0]);
                sourceVertex = mesh->getVertex(ret_index[0]);

                lon = destination[0].GetDouble();
                lat = destination[1].GetDouble();
                converter.convertToUTM(lat, lon, x, y);

                query_point[0] = x;
                query_point[1] = y;
                mat_index->index->knnSearch(query_point, 1, &ret_index[0], &out_dist_sqr[0]);
                destinationVertex = mesh->getVertex(ret_index[0]);

                if(prev == nullptr)
                    polyline.push_back(sourceVertex);
                else
                    assert(prev->getId() == sourceVertex->getId());
                polyline.push_back(destinationVertex);
                prev = destinationVertex;
            }
            polylines.push_back(polyline);
            annotation->setPolyLines(polylines);
            mesh->addAnnotation(annotation);
        }
        fclose(fp);
    }



    AnnotationFileManager manager;
    manager.setMesh(mesh.get());
    manager.writeAnnotations(shortestPathAnnotationFile);
    mesh.reset();
    std::cout << "Ended" << std::endl;
    return 0;
}
