#include <iostream>
#include <map>
#include <string>
#include <utilities.h>
#include <trianglehelper.h>
#include <Point.h>
#include <geotiff.h>
#include <TriangleMesh.h>
#include <rapidjson/prettywriter.h>
#include <tinyxml2.h>

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

    std::vector<std::shared_ptr<OSMNode> > nodes;
    std::vector<std::shared_ptr<OSMWay> > arcs;
    std::vector<std::shared_ptr<OSMRelation> > relations;
    int out = Utilities::loadOSM("/home/andreas/Scrivania/Matera/map.osm", nodes, arcs, relations);

    std::vector<std::shared_ptr<OSMWay> > buildings_arcs;
    std::vector<std::shared_ptr<OSMWay> > streets_arcs;
    //FILTRARE ARCHI IN BOUNDARY E STRADE
    /****Loading and preprocessing base polygons of buildings****/
    std::vector<std::vector<std::shared_ptr<Point> > > boundaries, streets;

    if(out < 0)
        return out;

    for(unsigned int i = 0; i < arcs.size(); i++)
    {
        if(arcs.at(i)->checkTag(std::make_pair("building","")))
            buildings_arcs.push_back(arcs.at(i));

        if(arcs.at(i)->checkTag(std::make_pair("highway","")))
            streets_arcs.push_back(arcs.at(i));
    }


    std::vector<std::vector<std::shared_ptr<Point> > > arcs_points;
    for(unsigned int i = 0; i < streets_arcs.size(); i++)
    {
        std::vector<std::shared_ptr<Point> > arc_points;
        std::vector<std::shared_ptr<OSMNode> > arc_nodes = streets_arcs.at(i)->getNodes();
        for(unsigned int j = 0; j < arc_nodes.size(); j++)
            arc_points.push_back(arc_nodes.at(j)->getCoordinates());
        arcs_points.push_back(arc_points);
    }

    for(unsigned int i = 0; i < buildings_arcs.size(); i++)
    {

        std::vector<std::shared_ptr<Point> > boundary;
        std::vector<std::shared_ptr<OSMNode> > nodes = buildings_arcs.at(i)->getNodes();
        for(unsigned int j = 0; j < nodes.size() - 1; j++)
            boundary.push_back(nodes.at(j)->getCoordinates());
        boundaries.push_back(boundary);

    }

    boundaries.clear();
    std::vector<std::vector<unsigned int> > triangles;
    Utilities::load_shapefile_shp(argv[1], boundaries);
    std::cout << "Loaded " << boundaries.size() << " buildings' bases" << std::endl;
    /*Utilities::load_shapefile_shp(argv[2], streets);
    std::cout << "Loaded " << streets.size() << " streets' segments" << std::endl;*/


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
    std::shared_ptr<Point> origin = std::make_shared<Point>(0,0,0);
    unsigned int points_number = 0;

    std::cout << "Cleaning polygons/lines and normalizing points" << std::endl;
    for(unsigned int i = 0; i < boundaries.size(); i++)
    {
        for(unsigned int j = 0; j < boundaries.at(i).size(); j++)
        {
            (*origin) += (*boundaries.at(i).at(j));
            points_number++;
        }
    }

    for(unsigned int j = 0; j < dtm_tiff->GetDimensions()[0]; j++)
    {
        for(unsigned int k = 0; k < dtm_tiff->GetDimensions()[1]; k++)
        {

            Point p(geoTransform[0] + (k + 0.5) * geoTransform[1] + j * geoTransform[2],
                      geoTransform[3] + k * geoTransform[4] + (j + 0.5) * geoTransform[5], 0);

            (*origin) += p;
            points_number++;
        }
    }
    double city_size = (max - min).norm();
    (*origin) /= points_number;

    for(unsigned int i = 0; i < boundaries.size(); i++)
        for(unsigned int j = 0; j < boundaries.at(i).size(); j++)
        {
            *boundaries.at(i).at(j) -= (*origin);
            *boundaries.at(i).at(j) /= city_size;
        }

    for(unsigned int i = 0; i < nodes.size(); i++)
    {
        std::shared_ptr<OSMNode> n = nodes.at(i);
        std::shared_ptr<Point> pos = n->getCoordinates();
        *pos -= *origin;
        *pos /= city_size;
    }

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
                    boundaries.at(i).at(k).reset();
                boundaries.erase(boundaries.begin() + i);
                i--;
                if(j > i)
                    j--;
                break;
            }
        }
    }

    Utilities::fix_lines(arcs_points);

    std::cout << "Refining lines:" << std::endl;
    Utilities::refineLines(arcs_points, dtm_tiff, city_size, *origin);

    std::cout << "Removing inserted critical vertices:" << std::endl;
    Utilities::removeLinePointsInPolygons(arcs_points, boundaries);


    std::vector<std::pair<unsigned int, unsigned int> > keptClippings = Utilities::removeSegmentsIntersectingPolygons(arcs_points, boundaries);

    TriangleMesh* mesh = new TriangleMesh();
    std::vector<std::vector<std::shared_ptr<Point> > > main_triangulation_boundaries;
    std::vector<std::shared_ptr<Point> > external_bb = {
        std::make_shared<Point>(min.getX(), min.getY(), 0),
        std::make_shared<Point>(max.getX(), min.getY(), 0),
        std::make_shared<Point>(max.getX(), max.getY(), 0),
        std::make_shared<Point>(min.getX(), max.getY(), 0)};
    *external_bb[0] = ((*external_bb[0]) - (*origin)) / city_size;
    *external_bb[1] = ((*external_bb[1]) - (*origin)) / city_size;
    *external_bb[2] = ((*external_bb[2]) - (*origin)) / city_size;
    *external_bb[3] = ((*external_bb[3]) - (*origin)) / city_size;

    main_triangulation_boundaries.push_back(external_bb);
    for(unsigned int i = 0; i < boundaries.size(); i++)
    {
        std::vector<std::shared_ptr<Point> > hole;
        unsigned int size = boundaries.at(i).size();
        if(boundaries.at(i)[0] == boundaries.at(i).back())
            size--;
        for(unsigned int j = 0; j < size; j++)
            hole.push_back(boundaries.at(i).at(j));

        main_triangulation_boundaries.push_back(hole);
    }
    std::cout << "Ended" << std::endl << "Creating triangulation:" << std::endl;

    mesh->triangulate(main_triangulation_boundaries, arcs_points);
    mesh->save("blublu.ply");

    std::cout << "Associating height to buildings" << std::endl;
    std::vector<std::vector<std::pair<unsigned int, unsigned int> > > bases_elevations = Utilities::extractPolygonsHeights1(main_triangulation_boundaries, dtm_tiff, city_size, *origin);

    for(unsigned int i = 0; i < keptClippings.size(); i++)
    {
        unsigned int numberOfNewLines = keptClippings.at(i).second - keptClippings.at(i).first;
        for(unsigned int j = 0; j < numberOfNewLines; j++)
        {
            std::vector<std::shared_ptr<Point> > clipping = arcs_points.at(keptClippings.at(i).first + j + 1);
            arcs_points.at(keptClippings.at(i).first).insert(arcs_points.at(keptClippings.at(i).first).end(), clipping.begin(), clipping.end());
        }
        arcs_points.erase(arcs_points.begin() + keptClippings.at(i).first + 1, arcs_points.begin() + keptClippings.at(i).second + 1);
        for(unsigned int j = i + 1; j < keptClippings.size(); j++)
        {
            keptClippings.at(j).first -= numberOfNewLines;
            keptClippings.at(j).second -= numberOfNewLines;
        }
    }

    unsigned int reachedEdgeId = mesh->getEdgesNumber();
    unsigned int reachedTriangleId = mesh->getTrianglesNumber();


    for(unsigned int i = 1; i < main_triangulation_boundaries.size(); i++)
    {

        std::vector<unsigned int> shell_triangles;
        unsigned int size = main_triangulation_boundaries.at(i).size();
        std::map<std::shared_ptr<Vertex>, std::vector<std::shared_ptr<Edge> > > vertices_edges;

        for(unsigned int j = 0; j < size; j++)
        {
            std::shared_ptr<Vertex> v1 = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(j));
            std::shared_ptr<Vertex> v2 = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at((j + 1) % size));
            std::vector<std::shared_ptr<Vertex> > shortestPath = Utilities::dijkstra(v1, v2);
            if(shortestPath.size() > 1)
            {
                std::cout << "Non-existent vertex catched: adding to polygon " << i << " " << j<< std::endl;
                main_triangulation_boundaries.at(i).insert(main_triangulation_boundaries.at(i).begin() + (j + 1) % size,
                                                           shortestPath.begin(),
                                                           shortestPath.begin() + shortestPath.size() - 1);
                size += shortestPath.size() - 1;
            } else if(shortestPath.size() < 1){

                for(unsigned int j = 0; j < size; j++)
                    main_triangulation_boundaries.at(i).at(j)->print(std::cerr,BracketsType::NONE, " ");
                std::cerr << "Impossible case" << std::endl;
                v1->print(std::cerr);
                v2->print(std::cerr);
                for(unsigned int k = 0; k < main_triangulation_boundaries.at(i).size(); k++)
                {
                    std::cerr << "Vertex " << k << " on boundary" << std::endl;
                    std::shared_ptr<Vertex> v =  static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(k));
                    if(v!= nullptr)
                    {
                        v->print(std::cerr);
                        std::vector<std::shared_ptr<Edge> > ve = v->getVE();
                        for(unsigned int l = 0; l < ve.size(); l++)
                        {
                            ve[l]->print(std::cerr);
                            std::shared_ptr<Vertex> v_ = ve[l]->getOppositeVertex(v);
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
                for(unsigned int i = 0; i < main_triangulation_boundaries.size(); i++)
                    for(unsigned int j = 0; j < main_triangulation_boundaries[i].size(); j++)
                        if(main_triangulation_boundaries.at(i).at(j) != nullptr)
                            main_triangulation_boundaries.at(i).at(j).reset();

                for(unsigned int i = 0; i < arcs_points.size(); i++)
                    for(unsigned int j = 0; j < arcs_points[i].size(); j++)
                        if(arcs_points.at(i).at(j) != nullptr)
                            if(static_pointer_cast<Vertex>(arcs_points.at(i).at(j)) != nullptr)
                                arcs_points.at(i).at(j).reset();
                return(5);
            }
        }

        for(unsigned int j = 0; j < size; j++)
        {
            std::vector<std::shared_ptr<Edge> > connected, ve = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(j))->getVE();

            vertices_edges.insert(std::make_pair(static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(j)), ve));
            main_triangulation_boundaries.at(i).push_back(mesh->addNewVertex(main_triangulation_boundaries.at(i).at(j)->getX(),
                                                                          main_triangulation_boundaries.at(i).at(j)->getY(),
                                                                          main_triangulation_boundaries.at(i).at(j)->getZ() + OFFSET));
            vertices_edges.insert(std::make_pair(static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).back()), connected));

            static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).back())->setId(mesh->getVerticesNumber() - 1);

        }

        for(int j = 0; j < size; j++)
        {
            unsigned int pos1 = Utilities::mod(j - 1, size);
            unsigned int pos2 = size + j;
            unsigned int pos3 = size + Utilities::mod(j - 1, size);
            unsigned int pos4 = j;
            std::shared_ptr<Vertex> v1 = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(pos1));
            std::shared_ptr<Vertex> v2 = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(pos2));
            std::shared_ptr<Vertex> v3 = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(pos3));
            std::shared_ptr<Vertex> v4 = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(pos4));

            std::shared_ptr<Edge> e1 = mesh->addNewEdge(v1, v2);
            vertices_edges.at(v1).push_back(e1);
            vertices_edges.at(v2).push_back(e1);
            e1->setId(reachedEdgeId++);
            std::shared_ptr<Edge> e2 = mesh->addNewEdge(v2, v3);
            vertices_edges.at(v2).push_back(e2);
            vertices_edges.at(v3).push_back(e2);
            e2->setId(reachedEdgeId++);
            std::shared_ptr<Edge> e3 = mesh->addNewEdge(v3, v1);
            vertices_edges.at(v3).push_back(e3);
            vertices_edges.at(v1).push_back(e3);
            e3->setId(reachedEdgeId++);
            std::shared_ptr<Triangle> t1 = mesh->addNewTriangle(e1, e2, e3);
            t1->setId(reachedTriangleId++);
            e1->setT1(t1);
            e2->setT1(t1);
            if(e3->getT1() == nullptr)
                e3->setT1(t1);
            else if(e3->getT2() == nullptr)
                e3->setT2(t1);
            else
                std::cout << "Found" << std::endl;
            std::shared_ptr<Edge> e4 = mesh->searchEdgeContainingVertex(vertices_edges.at(v1), v4);
            if(e4 == nullptr)
                e4 = mesh->searchEdgeContainingVertex(vertices_edges.at(v4), v1);
            std::shared_ptr<Edge> e5 = mesh->searchEdgeContainingVertex(vertices_edges.at(v4), v2);
            if(e5 == nullptr)
                e5 = mesh->searchEdgeContainingVertex(vertices_edges.at(v2), v4);
            if(e5 == nullptr)
            {
                e5 = mesh->addNewEdge(v4, v2);
                vertices_edges.at(v4).push_back(e5);
                vertices_edges.at(v2).push_back(e5);
                e5->setId(reachedEdgeId++);
            }
            std::shared_ptr<Triangle> t2 = mesh->addNewTriangle(e4, e5, e1);
            t2->setId(reachedTriangleId++);

            if(e4->getT1() == nullptr)
                e4->setT1(t2);
            else if(e4->getT2() == nullptr)
                e4->setT2(t2);
            else
                std::cout << "Found" << std::endl;
            if(e5->getT1() == nullptr)
                e5->setT1(t2);
            else if(e5->getT2() == nullptr)
                e5->setT2(t2);
            else
                std::cout << "Found" << std::endl;
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
            std::shared_ptr<Vertex> v1 = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(size + enclosing_triangles[j * 3]));
            std::shared_ptr<Vertex> v2 = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(size + enclosing_triangles[j * 3 + 1]));
            std::shared_ptr<Vertex> v3 = static_pointer_cast<Vertex>(main_triangulation_boundaries.at(i).at(size + enclosing_triangles[j * 3 + 2]));
            std::shared_ptr<Edge> e1 = mesh->searchEdgeContainingVertex(vertices_edges.at(v1), v2);
            if(e1 == nullptr)
            {
                e1 = mesh->addNewEdge(v1, v2);
                e1->setId(reachedEdgeId++);
                vertices_edges.at(v1).push_back(e1);
                vertices_edges.at(v2).push_back(e1);
            }
            std::shared_ptr<Edge> e2 = mesh->searchEdgeContainingVertex(vertices_edges.at(v2), v3);
            if(e2 == nullptr)
            {
                e2 = mesh->addNewEdge(v2, v3);
                e2->setId(reachedEdgeId++);
                vertices_edges.at(v2).push_back(e2);
                vertices_edges.at(v3).push_back(e2);
            }
            std::shared_ptr<Edge> e3 = mesh->searchEdgeContainingVertex(vertices_edges.at(v3), v1);
            if(e3 == nullptr)
            {
                e3 = mesh->addNewEdge(v3, v1);
                e3->setId(reachedEdgeId++);
                vertices_edges.at(v3).push_back(e3);
                vertices_edges.at(v1).push_back(e3);
            }
            std::shared_ptr<Triangle> t = mesh->addNewTriangle(e1, e2, e3);
            t->setId(reachedTriangleId++);
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

    std::cout << "Associating heights to buildings." << std::endl;
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
        std::shared_ptr<Vertex> v = mesh->getVertex(i);
        v->setX(v->getX() * city_size + origin->getX());
        v->setY(v->getY() * city_size + origin->getY());
    }

    origin.reset();
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


    ofstream annotationFile("streets.ant");
    rapidjson::StringBuffer s;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
    s.Clear();
    writer.Reset(s);
    writer.StartObject();
    writer.Key("annotations");
    writer.StartArray();
    unsigned char building_color[] = {255, 215, 0};
    unsigned char street_color[] = {123, 117, 107};
    for(unsigned int i = 1; i < main_triangulation_boundaries.size(); i++)
    {
        std::vector<std::shared_ptr<Point> > boundary(main_triangulation_boundaries.at(i).begin(),
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
            std::shared_ptr<Vertex> v = static_pointer_cast<Vertex>(boundary.at(j));
            if(v->getId() < 0 || v->getId() > mesh->getVerticesNumber())
                std::cout << "Found deleted vertex in building: " << i << " " << j << std::endl;
            else
            {
                writer.Int(static_cast<int>(v->getId()));
            }
        }
        writer.Int(static_cast<int>(static_pointer_cast<Vertex>(boundary.at(0))->getId()));
        writer.EndArray();
        writer.EndArray();
        writer.Key("attributes");
        writer.StartArray();
        //Add here attributes
        writer.EndArray();
        writer.EndObject();
    }
    for(unsigned int i = 0; i < arcs_points.size(); i++)
    {

        writer.StartObject();
        writer.Key("id");
        writer.Uint(main_triangulation_boundaries.size() - 1 + i);
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

        writer.StartArray();
        writer.StartArray();
        double slope = 0.0;
        std::cout.precision(15);
        for(unsigned int j = 0; j < arcs_points.at(i).size(); j++)
            if(static_pointer_cast<Vertex>(arcs_points.at(i).at(j))->getId() > mesh->getVerticesNumber())
                std::cout << "Found deleted vertex in street: " << i << " " << j << std::endl;
            else
            {
                writer.Int(static_cast<int>(static_pointer_cast<Vertex>(arcs_points.at(i).at(j))->getId()));
                if(j > 0)
                {
                    Point v = *arcs_points.at(i).at(j) - *arcs_points.at(i).at(j - 1);
                    Point projected(v.getX(), v.getY(), 0);
                    v /= v.norm();
                    projected /= projected.norm();
                    slope += v.computeAngle(projected);
                }
            }

        writer.EndArray();
        writer.EndArray();
        writer.Key("attributes");
        writer.StartArray();

        if(arcs_points.at(i).size() > 1)
        {
            slope /= arcs_points.at(i).size() - 1.0;
            writer.StartObject();
            writer.Key("id");
            writer.Uint(0);
            writer.Key("name");
            writer.String("slope");
            writer.Key("type");
            writer.String("Semantic");
            writer.Key("value");
            writer.String(std::to_string(slope).c_str());
            writer.EndObject();
        }

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

    std::cout << "OSMID-Points association" << std::endl;

    std::map<unsigned int, std::shared_ptr<Point> > osmid_point_association;
    for(unsigned int i = 0; i < arcs_points.size(); i++)
        for(unsigned int j = 0; j < arcs_points.at(i).size(); j++)
            if(arcs_points.at(i).at(j)->getInfo() != nullptr)
            {
                OSMNode* n = static_cast<OSMNode*>(arcs_points.at(i).at(j)->getInfo());
                if(n != nullptr)
                    osmid_point_association.insert(std::make_pair(std::atoi(n->getId().c_str()), arcs_points.at(i).at(j)));
                else
                    std::cout << "nullptr: " << i << " " << j << std::endl;
            } else
                std::cout << "no info: " << i << " " << j << std::endl;

    std::cout << "OSMID-Points associations size: " << osmid_point_association.size() << std::endl;
    std::cout << "Beginning slopes computation" << std::endl;
    ofstream slopesFile("attributes.json");
    s.Clear();
    writer.Reset(s);
    writer.StartObject();
    writer.Key("Arcs_attributes");
    writer.StartArray();
    unsigned int counter = 0;
    for(unsigned int i = 0; i < streets_arcs.size(); i++)
    {
        std::vector<std::shared_ptr<OSMNode> > arc_nodes = streets_arcs.at(i)->getNodes();

        for(unsigned int j = 1; j < arc_nodes.size(); j++)
        {
            double min_slope = std::numeric_limits<double>::max(), mean_slope = 0.0, max_slope = -std::numeric_limits<double>::max();
            std::shared_ptr<OSMNode> n1 = arc_nodes.at(j - 1);
            std::shared_ptr<OSMNode> n2 = arc_nodes.at(j);
            unsigned int id1;
            stringstream sstream(n1->getId());
            sstream >> id1;
            unsigned int id2;
            sstream.clear();
            sstream.str(n2->getId());
            sstream >> id2;


            std::map<unsigned int, std::shared_ptr<Point> >::iterator it1 = osmid_point_association.find(id1);
            std::map<unsigned int, std::shared_ptr<Point> >::iterator it2 = osmid_point_association.find(id2);
            std::shared_ptr<Vertex> v1;
            std::shared_ptr<Vertex> v2;
            if(it1 != osmid_point_association.end() && it2 != osmid_point_association.end())
            {
                v1 = static_pointer_cast<Vertex>(osmid_point_association.at(id1));
                v2 = static_pointer_cast<Vertex>(osmid_point_association.at(id2));
            } else
            {
                std::cerr << "Arc id " << streets_arcs.at(i)->getId() << std::endl;
                std::cerr << "Pair (" << id1 << "," << id2 << ")" << std::endl;
                std::cerr << "No id vertex association" << std::endl;
                std::cerr << "Position: " << i << " " << j << std::endl;
                if(it1 == osmid_point_association.end())
                    std::cerr << 1 << std::endl;
                if(it2 == osmid_point_association.end())
                    std::cerr << 2 << std::endl;
                counter++;
                continue;
            }
            if(v1 == nullptr || v2 == nullptr)
            {
                std::cerr << "Pair (" << id1 << "," << id2 << ")" << std::endl;
                std::cerr << "Position: " << i << " " << j << std::endl;
                std::cerr << "Could not find vertex" << std::endl;
                counter++;
                continue;
            }
            writer.StartObject();
            writer.Key("Node1");
            writer.Uint(id1);
            writer.Key("Node2");
            writer.Uint(id2);
            std::vector<std::shared_ptr<Vertex> > path = Utilities::dijkstra(v1, v2);
            path.insert(path.begin(), v1);
            for(unsigned int k = 1; k < path.size(); k++)
            {
                Point v = *path.at(k) - *path.at(k - 1);
                Point projected(v.getX(), v.getY(), 0);
                v /= v.norm();
                projected /= projected.norm();
                double angle = v.computeAngle(projected);
                mean_slope += angle;
                if(angle < min_slope)
                    min_slope = angle;
                if(angle > max_slope)
                    max_slope = angle;

            }
            mean_slope /= path.size() - 1;

            writer.Key("Attributes");
            writer.StartArray();
            writer.StartObject();
            writer.Key("type");
            writer.String("min_slope");
            writer.Key("value");
            writer.Double(min_slope);
            writer.EndObject();
            writer.StartObject();
            writer.Key("type");
            writer.String("max_slope");
            writer.Key("value");
            writer.Double(max_slope);
            writer.EndObject();
            writer.StartObject();
            writer.Key("type");
            writer.String("mean_slope");
            writer.Key("value");
            writer.Double(mean_slope);
            writer.EndObject();
            writer.EndArray();
            writer.EndObject();

        }
    }
    std::cout << counter << " motherfucker" << std::endl;
    std::cout << std::endl;
    writer.EndArray();
    writer.EndObject();
    if(slopesFile.is_open())
    {
        slopesFile << s.GetString();
        slopesFile.close();
    }

    delete mesh;
    for(unsigned int i = 0; i < main_triangulation_boundaries.size(); i++)
        for(unsigned int j = 0; j < main_triangulation_boundaries[i].size(); j++)
            if(main_triangulation_boundaries.at(i).at(j).unique())
                main_triangulation_boundaries.at(i).at(j).reset();
    for(unsigned int i = 0; i < arcs_points.size(); i++)
        for(unsigned int j = 0; j < arcs_points[i].size(); j++)
            if(arcs_points.at(i).at(j).unique())
                arcs_points.at(i).at(j).reset();
    std::cout << "Process completed" << std::endl;


    return 0;

}
