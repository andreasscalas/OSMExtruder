#include <iostream>
#include <utilities.h>
#include <map>
#include <string>
#include <trianglehelper.h>
#include <Point.h>
#include <geotiff.h>
#include <TriangleMesh.h>
#include <rapidjson/prettywriter.h>

using namespace std;
const double OFFSET = 5e-3;


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "Missing buildings shapefile parameter" << std::endl;
        return 1;
    }
    if(argc < 3)
    {
        std::cerr << "Missing streets shapefile parameter" << std::endl;
        return 2;
    }
    if(argc < 4)
    {
        std::cerr << "Missing DTM file parameter" << std::endl;
        return 3;
    }
    if(argc < 5)
    {
        std::cerr << "Missing DSM file parameter" << std::endl;
        return 4;
    }

    /****Loading and preprocessing base polygons of buildings****/
    std::vector<std::vector<Point*> > boundaries, streets;
    std::vector<std::vector<unsigned int> > triangles;
    Utilities::load_shapefile_shp(argv[1], boundaries);
    std::cout << "Loaded " << boundaries.size() << " buildings' bases" << std::endl;
    Utilities::load_shapefile_shp(argv[2], streets);
    std::cout << "Loaded " << streets.size() << " streets' segments" << std::endl;
    std::cout << "Loading DTM" << std::endl;
    GeoTiff* dtm_tiff = new GeoTiff(argv[3]);
    std::cout << "Loading DSM" << std::endl;
    GeoTiff* dsm_tiff = new GeoTiff(argv[4]);


    double* geoTransform = dtm_tiff->GetGeoTransform();
    Point min(geoTransform[0],
              geoTransform[3] + dtm_tiff->GetDimensions()[1] * geoTransform[5],
              0);
    Point max(geoTransform[0] + dtm_tiff->GetDimensions()[0] * geoTransform[1],
              geoTransform[3],
              0);
    Point* origin = new Point(0,0,0);
    unsigned int points_number = 0;

    std::cout << "Cleaning polygons/lines and normalizing points" << std::endl;
    for(unsigned int i = 0; i < boundaries.size(); i++)
    {
        for(unsigned int j = 0; j < boundaries.at(i).size(); j++)
        {
            (*origin) = (*origin) + (*boundaries.at(i).at(j));
            points_number++;
        }
    }

    for(unsigned int j = 0; j < dtm_tiff->GetDimensions()[0]; j++)
    {
        for(unsigned int k = 0; k < dtm_tiff->GetDimensions()[1]; k++)
        {

            Point p(geoTransform[0] + (k + 0.5) * geoTransform[1] + j * geoTransform[2],
                      geoTransform[3] + k * geoTransform[4] + (j + 0.5) * geoTransform[5], 0);

            (*origin) = (*origin) + p;
            points_number++;
        }
    }
    double city_size = (max - min).norm();
    (*origin) /= points_number;

    for(unsigned int i = 0; i < boundaries.size(); i++)
        for(unsigned int j = 0; j < boundaries.at(i).size(); j++)
            *boundaries.at(i).at(j) = ((*boundaries.at(i).at(j)) - (*origin)) / city_size;

    for(unsigned int i = 0; i < streets.size(); i++)
        for(unsigned int j = 0; j < streets.at(i).size(); j++)
            *streets.at(i).at(j) = ((*streets.at(i).at(j)) - (*origin)) / city_size;

    Utilities::fix_polygons(boundaries);
    for(unsigned int i = 0; i < boundaries.size(); i++)
    {
        for(unsigned int j = 0; j < boundaries.size(); j++)
        {
            if(i==j)
                continue;
            if(Utilities::isPolygonInsidePolygon(boundaries.at(i), boundaries.at(j)))
            {
                for(unsigned int k = 0; k < boundaries.at(i).size(); k++)
                    delete boundaries.at(i).at(k);
                boundaries.erase(boundaries.begin() + i);
                i--;
                if(j > i)
                    j--;
                break;
            }
        }
    }

    Utilities::fix_lines(streets);

    std::cout << "Refining lines:" << std::endl;
    //Utilities::refineLines(streets, dtm_tiff, city_size, *origin);
    std::cout << "Removing inserted critical vertices:" << std::endl;
    Utilities::removeLinePointsInPolygons(streets, boundaries);
    Utilities::fix_lines(streets);

    TriangleMesh* mesh = new TriangleMesh();
    std::vector<std::vector<Point*> > main_triangulation_boundaries;
    std::vector<Point*> external_bb = {
        new Point(min.getX(), min.getY(), 0),
        new Point(max.getX(), min.getY(), 0),
        new Point(max.getX(), max.getY(), 0),
        new Point(min.getX(), max.getY(), 0)};
    *external_bb[0] = ((*external_bb[0]) - (*origin)) / city_size;
    *external_bb[1] = ((*external_bb[1]) - (*origin)) / city_size;
    *external_bb[2] = ((*external_bb[2]) - (*origin)) / city_size;
    *external_bb[3] = ((*external_bb[3]) - (*origin)) / city_size;

    main_triangulation_boundaries.push_back(external_bb);
    for(unsigned int i = 0; i < boundaries.size(); i++)
    {
        std::vector<Point*> hole;
        unsigned int size = boundaries.at(i).size();
        if(boundaries.at(i)[0] == boundaries.at(i).back())
            size--;
        for(unsigned int j = 0; j < size; j++)
            hole.push_back(boundaries.at(i).at(j));

        main_triangulation_boundaries.push_back(hole);
    }


    std::cout << "Associating heights to buildings." << std::endl;
    std::vector<std::vector<std::pair<unsigned int, unsigned int> > > bases_elevations = Utilities::extractPolygonsHeights1(main_triangulation_boundaries, dtm_tiff, city_size, *origin);

    std::cout << "Ended" << std::endl << "Creating triangulation:" << std::endl;

    mesh->triangulate(main_triangulation_boundaries, streets);
    mesh->save("blublu.ply");


    //std::vector<std::vector<Vertex*> > projectedStreets = Utilities::projectLines(streets, mesh, ((min - *origin) / city_size).norm() / 100);

    for(unsigned int i = 1; i < main_triangulation_boundaries.size(); i++)
    {

        std::vector<unsigned int> shell_triangles;
        unsigned int size = main_triangulation_boundaries.at(i).size();
        std::map<Vertex*, std::vector<Edge*> > vertices_edges;

        for(unsigned int j = 0; j < size; j++)
        {
            Vertex* v1 = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(j));
            Vertex* v2 = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at((j + 1) % size));
            std::vector<Vertex*> shortestPath = Utilities::dijkstra(v1, v2);
            if(shortestPath.size() > 1)
            {
                std::cout << "Non-existent vertex catched: adding to polygon " << i << " " << j<< std::endl;
                main_triangulation_boundaries.at(i).insert(main_triangulation_boundaries.at(i).begin() + (j + 1) % size,
                                                           shortestPath.begin(),
                                                           shortestPath.begin() + shortestPath.size() - 1);
                size += shortestPath.size() - 1;
            } else if(shortestPath.size() < 1){

                std::cerr << "Impossible case" << std::endl;
                v1->print(std::cerr);
                v2->print(std::cerr);
                for(unsigned int k = 0; k < main_triangulation_boundaries.at(i).size(); k++)
                {
                    std::cerr << "Vertex " << k << " on boundary" << std::endl;
                    Vertex* v =  static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(k));
                    if(v!= nullptr)
                    {
                        v->print(std::cerr);
                        std::vector<Edge*> ve = v->getVE();
                        for(unsigned int l = 0; l < ve.size(); l++)
                        {
                            ve[l]->print(std::cerr);
                            Vertex* v_ = ve[l]->getOppositeVertex(v);
                            if(v_ != nullptr)
                                v_->print(std::cerr);
                            else
                                std::cerr << "Opposite vertex not available!!!" << std::endl;
                        }
                        std::cerr << std::endl;
                    } else
                    {
                        std::cerr << "Vertex not available!!!" << std::endl;
                    }

                }
                std::cerr << std::flush;
                delete mesh;
//                for(unsigned int i = 0; i < main_triangulation_boundaries.size(); i++)
//                    for(unsigned int j = 0; j < main_triangulation_boundaries[i].size(); j++)
//                        if(main_triangulation_boundaries.at(i).at(j) != nullptr)
//                            if(static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(j)) != nullptr)
//                                delete static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(j));
//                            else
//                                delete main_triangulation_boundaries.at(i).at(j);
//                for(unsigned int i = 0; i < streets.size(); i++)
//                    for(unsigned int j = 0; j < streets[i].size(); j++)
//                        if(streets.at(i).at(j) != nullptr)
//                            if(static_cast<Vertex*>(streets.at(i).at(j)) != nullptr)
//                                delete static_cast<Vertex*>(streets.at(i).at(j));
//                            else
//                                delete streets.at(i).at(j);
                return(5);
            }
        }

        for(unsigned int j = 0; j < size; j++)
        {
            std::vector<Edge*> connected, ve = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(j))->getVE();
            //main_triangulation_boundaries.at(i).at(j)->print(std::cout);

            vertices_edges.insert(std::make_pair(static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(j)), ve));
            main_triangulation_boundaries.at(i).push_back(mesh->addNewVertex(main_triangulation_boundaries.at(i).at(j)->getX(),
                                                                          main_triangulation_boundaries.at(i).at(j)->getY(),
                                                                          main_triangulation_boundaries.at(i).at(j)->getZ() + OFFSET));
            vertices_edges.insert(std::make_pair(static_cast<Vertex*>(main_triangulation_boundaries.at(i).back()), connected));

            static_cast<Vertex*>(main_triangulation_boundaries.at(i).back())->setId(mesh->getVerticesNumber() - 1);

        }

        for(int j = 0; j < size; j++)
        {
            Vertex* v1 = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(Utilities::mod(j - 1, size)));
            Vertex* v2 = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(size + j));
            Vertex* v3 = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(size + Utilities::mod(j - 1, size)));
            Vertex* v4 = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(j));

            Edge* e1 = mesh->addNewEdge(v1, v2);
            vertices_edges.at(v1).push_back(e1);
            vertices_edges.at(v2).push_back(e1);
            Edge* e2 = mesh->addNewEdge(v2, v3);
            vertices_edges.at(v2).push_back(e2);
            vertices_edges.at(v3).push_back(e2);
            Edge* e3 = mesh->addNewEdge(v3, v1);
            vertices_edges.at(v3).push_back(e3);
            vertices_edges.at(v1).push_back(e3);
            Triangle *t1 = mesh->addNewTriangle(e1, e2, e3);
            e1->setT1(t1);
            e2->setT1(t1);
            if(e3->getT1() == nullptr)
                e3->setT1(t1);
            else
                e3->setT2(t1);
            Edge* e4 = mesh->searchEdgeContainingVertex(vertices_edges.at(v1), v4);
            if(e4 == nullptr)
                e4 = mesh->searchEdgeContainingVertex(vertices_edges.at(v4), v1);
            Edge* e5 = mesh->addNewEdge(v4, v2);
            vertices_edges.at(v4).push_back(e5);
            vertices_edges.at(v2).push_back(e5);
            Edge* e6 = mesh->searchEdgeContainingVertex(vertices_edges.at(v2), v1);
            Triangle *t2 = mesh->addNewTriangle(e4, e5, e6);
            e4->setT1(t2);
            if(e5->getT1() == nullptr)
                e5->setT1(t2);
            else
                e5->setT2(t2);
            e6->setT1(t2);
            e1->setT2(t2);

        }

        std::vector<double*> boundary;
        std::vector<std::vector<double*> > holes;
        std::vector<double*> constraints;
        std::vector<std::pair<unsigned int, unsigned int> > constraint_segments;
        for(unsigned int j = 0; j < size; j++)
        {
            boundary.push_back(main_triangulation_boundaries.at(i)[size + j]->toDoubleArray());
        }
        TriHelper::TriangleHelper helper(boundary, holes, constraints, constraint_segments);
        std::vector<unsigned int> enclosing_triangles = helper.getTriangles();

        for(unsigned int j = 0; j < enclosing_triangles.size() / 3; j++)
        {
            Vertex* v1 = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(size + enclosing_triangles[j * 3]));
            Vertex* v2 = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(size + enclosing_triangles[j * 3 + 1]));
            Vertex* v3 = static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(size + enclosing_triangles[j * 3 + 2]));
            Edge* e1 = mesh->searchEdgeContainingVertex(vertices_edges.at(v1), v2);
            if(e1 == nullptr)
            {
                e1 = mesh->addNewEdge(v1, v2);
                vertices_edges.at(v1).push_back(e1);
                vertices_edges.at(v2).push_back(e1);
            }
            Edge* e2 = mesh->searchEdgeContainingVertex(vertices_edges.at(v2), v3);
            if(e2 == nullptr)
            {
                e2 = mesh->addNewEdge(v2, v3);
                vertices_edges.at(v2).push_back(e2);
                vertices_edges.at(v3).push_back(e2);
            }
            Edge* e3 = mesh->searchEdgeContainingVertex(vertices_edges.at(v3), v1);
            if(e3 == nullptr)
            {
                e3 = mesh->addNewEdge(v3, v1);
                vertices_edges.at(v3).push_back(e3);
                vertices_edges.at(v1).push_back(e3);
            }
            Triangle *t = mesh->addNewTriangle(e1, e2, e3);
            if(e1->getT1() == nullptr)
                e1->setT1(t);
            else if(e1->getT2() == nullptr)
                e1->setT2(t);
            else
                std::cerr << "Non manifold configuration!" << std::endl;

            if(e2->getT1() == nullptr)
                e2->setT1(t);
            else if(e2->getT2() == nullptr)
                e2->setT2(t);
            else
                std::cerr << "Non manifold configuration!" << std::endl;

            if(e3->getT1() == nullptr)
                e3->setT1(t);
            else if(e3->getT2() == nullptr)
                e3->setT2(t);
            else
                std::cerr << "Non manifold configuration!" << std::endl;
        }
    }

    float** elevations = dtm_tiff->GetRasterBand(1);
    float** heights = dsm_tiff->GetRasterBand(1);

    std::cout << "Ended" << std::endl << "Adapting heights to DTM" << std::endl;
    Utilities::associateHeights(mesh, dtm_tiff, city_size, *origin);
    std::cout << "Ended" << std::endl;
//    delete dtm_tiff;
//    delete dsm_tiff;
    //De-normalise boundaries points
    for(unsigned int i = 0; i < mesh->getVerticesNumber(); i++)
    {
        Vertex* v = mesh->getVertex(i);
        v->setX(v->getX() * city_size + origin->getX());
        v->setY(v->getY() * city_size + origin->getY());
    }

    delete origin;
    for(unsigned int i = 0; i < main_triangulation_boundaries.size(); i++)
    {

        unsigned int row_min = -1, col_min = -1, row_max = 0, col_max = 0;
        double elevation_min = std::numeric_limits<double>::max(), elevation_max = -std::numeric_limits<double>::max(), elevation_mean = 0;
        double height_min = std::numeric_limits<double>::max(), height_max = -std::numeric_limits<double>::max(), height_mean = 0;
        //Computing min-max-mean height values
        for(unsigned int k = 0; k < bases_elevations.at(i).size(); k++)
        {
            if(row_min > bases_elevations.at(i)[k].first)
                row_min = bases_elevations.at(i)[k].first;
            if(row_max < bases_elevations.at(i)[k].first)
                row_max = bases_elevations.at(i)[k].first;
            if(col_min > bases_elevations.at(i)[k].second)
                col_min = bases_elevations.at(i)[k].second;
            if(col_max < bases_elevations.at(i)[k].second)
                col_max = bases_elevations.at(i)[k].second;

            elevation_mean += elevations[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second];
            height_mean += heights[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second];
            if(elevation_min > elevations[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second])
                elevation_min = elevations[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second];

            if(elevation_max < elevations[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second])
                elevation_max = elevations[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second];

            if(height_min > heights[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second])
                height_min = heights[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second];

            if(height_max < heights[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second])
                height_max = heights[bases_elevations.at(i)[k].first][bases_elevations.at(i)[k].second];

        }
        elevation_mean /= bases_elevations.at(i).size();
        height_mean /= bases_elevations.at(i).size();
        for(unsigned int j = 0; j < main_triangulation_boundaries.at(i).size(); j++)
        {
            if(j < main_triangulation_boundaries.at(i).size() / 2)
            {
                main_triangulation_boundaries.at(i).at(j)->setZ(elevation_mean);
            } else
                main_triangulation_boundaries.at(i).at(j)->setZ(height_mean);
        }
    }
    mesh->save("base_triangulated.ply", 15);
    for(unsigned int i = 0; i < dtm_tiff->GetDimensions()[0]; i++)
        if(heights[i] != nullptr)
            delete[] elevations[i];
    for(unsigned int i = 0; i < dtm_tiff->GetDimensions()[0]; i++)
        if(heights[i] != nullptr)
            delete[] heights[i];


    std::ofstream meshstream("meshPointsIds");
    meshstream.precision(15);
    if(meshstream.is_open())
    {
        ofstream annotationFile("streets.ant");
        rapidjson::StringBuffer s;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("annotations");
        writer.StartArray();
        unsigned char building_color[] = {255, 215, 0};
        unsigned char street_color[] = {123, 117, 107};
        for(unsigned int i = 1; i < main_triangulation_boundaries.size(); i++)
        {
            std::vector<Point*> boundary(main_triangulation_boundaries.at(i).begin(),
                                          main_triangulation_boundaries.at(i).begin() + main_triangulation_boundaries.at(i).size() / 2);
            std::reverse(boundary.begin(), boundary.end());
            writer.StartObject();
            writer.Key("id");
            writer.Uint(i - 1);
            writer.Key("tag");
            writer.String(("building n° " + std::to_string(i - 1)).c_str());
            writer.Key("father");
            writer.Int(-1);
            writer.Key("level");
            writer.Uint(0);
            writer.Key("color");
            writer.StartArray();
            writer.Int(building_color[0]);
            writer.Int(building_color[1]);
            writer.Int(building_color[2]);
            writer.EndArray();
            writer.Key("type");
            writer.String("Area");
            writer.Key("boundaries");
            writer.StartArray();

            writer.StartArray();
            for(unsigned int j = 0; j < boundary.size(); j++)
            {
                Vertex* v = static_cast<Vertex*>(boundary.at(j));
                if(v->getId() < 0 || v->getId() > mesh->getVerticesNumber())
                    std::cout << "Found deleted vertex in building: " << i << " " << j << std::endl;
                else
                {
                    static_cast<Vertex*>(boundary.at(j))->print(std::cout);
                    meshstream << boundary.at(j)->getX() << " " << boundary.at(j)->getY() << std::endl;
                    writer.Int(static_cast<int>(v->getId()));
                }
            }
            writer.Int(static_cast<int>(static_cast<Vertex*>(boundary.at(0))->getId()));
            writer.EndArray();
            writer.EndArray();
            writer.Key("attributes");
            writer.StartArray();
            //Add here attributes
            writer.EndArray();
            writer.EndObject();
        }
        for(unsigned int i = 0; i < streets.size(); i++)
        {

            writer.StartObject();
            writer.Key("id");
            writer.Uint(/*main_triangulation_boundaries.size() - 1 +*/ i);
            writer.Key("tag");
            writer.String(("street n° " + std::to_string(i)).c_str());
            writer.Key("father");
            writer.Int(-1);
            writer.Key("level");
            writer.Uint(0);
            writer.Key("color");
            writer.StartArray();
            writer.Int(street_color[0]);
            writer.Int(street_color[1]);
            writer.Int(street_color[2]);
            writer.EndArray();
            writer.Key("type");
            writer.String("Line");
            writer.Key("polylines");
            for(unsigned int j = 0; j < streets.at(i).size(); j++)
                if(streets.at(i).at(j) == nullptr || static_cast<Vertex*>(streets.at(i).at(j)) == nullptr)
                {
                    streets.at(i).erase(streets.at(i).begin() + j);
                    j--;
                }
            if(streets.at(i).size() <= 1)
            {
                streets.erase(streets.begin() + i);
                i--;
                break;
            }
            writer.StartArray();
            writer.StartArray();
            for(unsigned int j = 0; j < streets.at(i).size(); j++)
                if(static_cast<Vertex*>(streets.at(i).at(j))->getId() < 0 || static_cast<Vertex*>(streets.at(i).at(j))->getId() > mesh->getVerticesNumber())
                    std::cout << "Found deleted vertex in street: " << i << " " << j << std::endl;
                else
                    writer.Int(static_cast<int>(static_cast<Vertex*>(streets.at(i).at(j))->getId()));

            writer.EndArray();
            writer.EndArray();
            writer.Key("attributes");
            writer.StartArray();
            //Add here attributes
            writer.EndArray();
            writer.EndObject();

        }
        writer.EndArray();
        writer.EndObject();
        if(annotationFile.is_open())
        {
            annotationFile << s.GetString();
            annotationFile.close();
        }
        meshstream.close();
    }



    delete mesh;
    std::cout << "Process completed" << std::endl;
//    for(unsigned int i = 0; i < main_triangulation_boundaries.size(); i++)
//        for(unsigned int j = 0; j < main_triangulation_boundaries[i].size(); j++)
//            if(main_triangulation_boundaries.at(i).at(j) != nullptr)
//                if(static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(j)) != nullptr)
//                    delete static_cast<Vertex*>(main_triangulation_boundaries.at(i).at(j));
//                else
//                    delete main_triangulation_boundaries.at(i).at(j);
//    for(unsigned int i = 0; i < streets.size(); i++)
//        for(unsigned int j = 0; j < streets[i].size(); j++)
//            if(streets.at(i).at(j) != nullptr)
//                if(static_cast<Vertex*>(streets.at(i).at(j)) != nullptr)
//                    delete static_cast<Vertex*>(streets.at(i).at(j));
//                else
//                    delete streets.at(i).at(j);

    return 0;

}
