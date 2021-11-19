#include "TriangleMesh.h"
#include <fstream>
#include <sstream>
#include <map>
#include <trianglehelper.h>
#include <exception>

TriangleMesh::TriangleMesh()
{

}

TriangleMesh::~TriangleMesh()
{

    for(unsigned int i = 0; i < vertices.size(); i++)
        if(vertices.at(i) != nullptr)
            delete vertices.at(i);
    for(unsigned int i = 0; i < edges.size(); i++)
        if(edges.at(i) != nullptr)
            delete edges.at(i);
    for(unsigned int i = 0; i < triangles.size(); i++)
        if(triangles.at(i) != nullptr)
            delete triangles.at(i);
}

unsigned int TriangleMesh::getVerticesNumber()
{
    return vertices.size();
}

unsigned int TriangleMesh::getEdgesNumber()
{
    return edges.size();
}

unsigned int TriangleMesh::getTrianglesNumber()
{
    return triangles.size();
}

Vertex* TriangleMesh::addNewVertex()
{
    vertices.push_back(new Vertex());
    return vertices.back();
}

Vertex* TriangleMesh::addNewVertex(double x, double y, double z)
{
    vertices.push_back(new Vertex(x, y, z));
    return vertices.back();
}

Vertex* TriangleMesh::addNewVertex(Point p)
{
    return addNewVertex(p.getX(), p.getY(), p.getZ());
}

Vertex* TriangleMesh::addNewVertex(Vertex*  v)
{
    vertices.push_back(new Vertex(v));
    return vertices.back();
}

Vertex *TriangleMesh::getVertex(unsigned int pos)
{
    if(pos > vertices.size())
        return nullptr;
    return vertices.at(pos);
}

Edge *TriangleMesh::getEdge(unsigned int pos)
{
    if(pos > edges.size())
        return nullptr;
    return edges.at(pos);
}

Triangle *TriangleMesh::getTriangle(unsigned int pos)
{
    if(pos > triangles.size())
        return nullptr;
    return triangles.at(pos);
}

Edge* TriangleMesh::addNewEdge()
{
    edges.push_back(new Edge());
    return edges.back();
}

Edge* TriangleMesh::addNewEdge(Vertex* v1, Vertex* v2)
{
    edges.push_back(new Edge(v1, v2));
    return edges.back();
}

Edge* TriangleMesh::addNewEdge(Edge* e)
{
    edges.push_back(new Edge(e));
    return edges.back();
}

Triangle* TriangleMesh::addNewTriangle()
{
    triangles.push_back(new Triangle());
    return triangles.back();
}

Triangle* TriangleMesh::addNewTriangle(Edge* e1, Edge* e2, Edge* e3)
{
    triangles.push_back(new Triangle(e1, e2, e3));
    return triangles.back();
}

Triangle* TriangleMesh::addNewTriangle(Triangle* t)
{
    triangles.push_back(new Triangle(t));
    return triangles.back();
}

int TriangleMesh::load(std::string filename)
{
    return loadPLY(filename);
}

int TriangleMesh::save(std::string filename, unsigned int precision)
{

    unsigned int return_code;
    std::ofstream meshStream(filename);
    std::string format = filename.substr(filename.find(".") + 1);
    meshStream.precision(precision);
    if(meshStream.is_open())
    {
        if(format.compare("ply") == 0)
        {
            meshStream << "ply" << std::endl;
            meshStream << "format ascii 1.0" << std::endl;
            meshStream << "element vertex " << getVerticesNumber() << std::endl;
            meshStream << "property double x" << std::endl;
            meshStream << "property double y" << std::endl;
            meshStream << "property double z" << std::endl;
            meshStream << "element face " << getTrianglesNumber() << std::endl;
            meshStream << "property list uchar int vertex_indices" << std::endl;
            meshStream << "end_header" << std::endl;
            std::cout << "Writing vertices:" << std::endl;
            for(unsigned int i = 0; i < getVerticesNumber(); i++)
            {
                meshStream << vertices.at(i)->getX() << " " << vertices.at(i)->getY() << " " << vertices.at(i)->getZ() << std::endl;
                if(i % 100 == 0)
                    std::cout << i * 100 / getVerticesNumber() << "%\r" << std::flush;
            }

            std::cout << "Ended! Written " << getVerticesNumber() << " vertices." << std::endl << "Writing triangles:" << std::endl;
            for(unsigned int i = 0; i < getTrianglesNumber(); i++)
            {
                meshStream << "3 " << triangles.at(i)->getV1()->getId() << " " << triangles.at(i)->getV2()->getId() << " " << triangles.at(i)->getV3()->getId() << std::endl;
                if(i % 100 == 0)
                    std::cout << i * 100 / getTrianglesNumber() << "%\r" << std::flush;
            }
            std::cout << "Ended! Written " << getTrianglesNumber() << " triangles." << std::endl;
            return_code = 0;
        } else
            return_code = 1;

        meshStream.close();
    } else
        return_code = -1;
    return return_code;
}

unsigned int TriangleMesh::removeIsolatedVertices()
{
    unsigned int removed = 0;
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        if(vertices.at(i)->getE0() == nullptr)
        {
            Vertex* v = vertices.at(i);
            for(unsigned int j = i + 1; j < vertices.size(); j++)
                vertices.at(j)->setId(vertices.at(j)->getId() - 1);
            vertices.erase(vertices.begin() + i);
            delete v;
            removed++;
            i--;
        }
    }
    return removed;
}

Edge* TriangleMesh::searchEdgeContainingVertex(std::vector<Edge*> list, Vertex* v)
{
    for(unsigned int i = 0; i < list.size(); i++)
        if(list.at(i)->hasVertex(v))
            return list.at(i);

    return nullptr;
}


int TriangleMesh::triangulate(std::vector<std::vector<Point *> > &boundaries, std::vector<std::vector<Point *> > &constraints)
{
    if(boundaries.size() < 1)
        return -1;
    std::vector<std::vector<Point *> > holes(boundaries.begin() + 1, boundaries.end());
    std::vector<double*> exterior_boundary_double_array;
    std::vector<std::vector<double*> > holes_double_arrays;
    std::map<Vertex*, std::vector<Edge*> > vertices_edges;
    unsigned int vertices_id = vertices.size();
    unsigned int edges_id = edges.size();
    unsigned int triangles_id = triangles.size();
    /**The first boundary is the exterior boundary**/
    for(unsigned int i = 0; i < boundaries.at(0).size(); i++)
    {
        std::vector<Edge*> incident;
        Point* tmp = boundaries.at(0).at(i);
        exterior_boundary_double_array.push_back(tmp->toDoubleArray());
        boundaries.at(0).at(i) = addNewVertex(tmp->getX(), tmp->getY(), tmp->getZ());
        delete tmp;
        vertices.back()->setId(vertices_id++);
        vertices_edges.insert(std::make_pair(vertices.back(), incident));
    }
    /**The remaining boundaries are holes**/
    for(unsigned int i = 1; i < boundaries.size(); i++)
    {
        std::vector<double*> hole_double_array;
        for(unsigned int j = 0; j < boundaries.at(i).size(); j++)
        {
            std::vector<Edge*> incident;
            Point* tmp = boundaries.at(i).at(j);
            hole_double_array.push_back(tmp->toDoubleArray());
            boundaries.at(i).at(j) = addNewVertex(tmp->getX(), tmp->getY(), tmp->getZ());
            delete tmp;
            vertices.back()->setId(vertices_id++);
            vertices_edges.insert(std::make_pair(vertices.back(), incident));
        }
        holes_double_arrays.push_back(hole_double_array);
    }

    std::vector<double*> constraint_vertices;
    std::vector<std::pair<unsigned int, unsigned int> > constraint_segments;
    unsigned int constraints_vertices_first_index = vertices.size();
    for(unsigned int i = 0; i < constraints.size(); i++)
    {
        int last_pos = -1;
        for(int j = 0; j < constraints.at(i).size(); j++)
        {
            int pos = -1;
            for(unsigned int k = constraints_vertices_first_index; k < vertices.size(); k++)
                if((*constraints.at(i).at(j)) == (*vertices.at(k)))
                {
                    //std::cout << i << " " << j << " " << k << std::endl << "Point before ";
                    Point* tmp = static_cast<Vertex*>(constraints.at(i).at(j));
                    //tmp->print(std::cout);
                    //std::cout << "Point after ";
                    //vertices.at(k)->print(std::cout);
                    constraints.at(i).at(j) = vertices.at(k);
                    delete tmp;
                    tmp = nullptr;
                    pos = k;
                    break;
                }
            if(pos < 0)
            {
                std::vector<Edge*> incident;
                Point* tmp = constraints.at(i).at(j);
                Vertex* v = addNewVertex(tmp->getX(), tmp->getY(), tmp->getZ());
                constraints.at(i).at(j) = v;
                delete tmp;
                v->setId(vertices_id++);
                vertices_edges.insert(std::make_pair(v, incident));
                constraint_vertices.push_back(v->toDoubleArray());
                pos = vertices_id - 1;
            }

            if(j > 0)
                constraint_segments.push_back(std::make_pair(last_pos, pos));

            last_pos = pos;
        }
    }

    std::cout << "Starting triangulation " << std::endl;
    TriHelper::TriangleHelper helper(exterior_boundary_double_array, holes_double_arrays, constraint_vertices, constraint_segments);
    std::vector<unsigned int> generated_triangles = helper.getTriangles();
    std::vector<double*> generated_points = helper.getAddedPoints();
    for(unsigned int i = 0; i < generated_points.size(); i++)
    {
        std::vector<Edge*> incident;
        addNewVertex(generated_points.at(i)[0], generated_points.at(i)[1], 0);
        vertices.back()->setId(vertices_id++);
        vertices_edges.insert(std::make_pair(vertices.back(), incident));
    }

    std::cout << "Ended! Created " << generated_triangles.size() / 3 << " triangles." << std::endl << "Building mesh structure:" << std::endl;
    for(unsigned int i = 0; i < generated_triangles.size() / 3; i++)
    {
        unsigned int polyVertNum;
        unsigned int v1 = generated_triangles.at(i * 3), v2 = generated_triangles.at(i * 3 + 1), v3 = generated_triangles.at(i * 3 + 2);
        Edge* e1, *e2, *e3;

        e1 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v1)), vertices.at(v2));
        if(e1 == nullptr)
            e1 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v2)), vertices.at(v1));
        if(e1 == nullptr)
        {
            addNewEdge(vertices.at(v1), vertices.at(v2));
            e1 = edges.back();
            e1->setId(edges_id++);
            vertices_edges.at(vertices.at(v1)).push_back(e1);
        }
        e2 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v2)), vertices.at(v3));
        if(e2 == nullptr)
            e2 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v3)), vertices.at(v2));
        if(e2 == nullptr)
        {
            addNewEdge(vertices.at(v2), vertices.at(v3));
            e2 = edges.back();
            e1->setId(edges_id++);
            vertices_edges.at(vertices.at(v2)).push_back(e2);
        }
        e3 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v3)), vertices.at(v1));
        if(e3 == nullptr)
            e3 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v1)), vertices.at(v3));
        if(e3 == nullptr)
        {
            addNewEdge(vertices.at(v3), vertices.at(v1));
            e3 = edges.back();
            e1->setId(edges_id++);
            vertices_edges.at(vertices.at(v3)).push_back(e3);
        }

        addNewTriangle(e1, e2, e3);
        triangles.back()->setId(triangles_id++);
        if(e1->getT1() == nullptr)
           e1->setT1(triangles.back());
        else if(e1->getT2() == nullptr)
            e1->setT2(triangles.back());
        else
            return 9;
        if(e2->getT1() == nullptr)
            e2->setT1(triangles.back());
        else if(e2->getT2() == nullptr)
            e2->setT2(triangles.back());
        else
            return 9;
        if(e3->getT1() == nullptr)
            e3->setT1(triangles.back());
        else if(e3->getT2() == nullptr)
            e3->setT2(triangles.back());
        else
            return 9;

        if(i % 100 == 0)
            std::cout << i * 100 / generated_triangles.size() << "%\r" << std::flush;;

    }
    unsigned int removed_number = removeIsolatedVertices();
    if(removed_number > 0)
        std::cout << "Removed " << removed_number << " isolated vertices (constraint segment/s may have been outside area to be triangulated)" << std::endl;

//    for(unsigned int i = 0; i < boundaries.size(); i++)
//    {
//        for(unsigned int j = 0; j < boundaries.at(i).size(); j++)
//            if(boundaries.at(i).at(j) == nullptr || static_cast<Vertex*>(boundaries.at(i).at(j)) == nullptr)
//            {
//                boundaries.at(i).erase(boundaries.at(i).begin() + j);
//                j--;
//            }
//    }

//    for(unsigned int i = 0; i < constraints.size(); i++)
//    {
//        for(unsigned int j = 0; j < constraints.at(i).size(); j++)
//            if(constraints.at(i).at(j) == nullptr || static_cast<Vertex*>(constraints.at(i).at(j)) == nullptr)
//            {
//                constraints.at(i).erase(constraints.at(i).begin() + j);
//                j--;
//            }
//    }
    std::cout << "Ended!" << std::endl;
    return 0;

}

int TriangleMesh::loadPLY(std::string filename)
{
    std::ifstream fileStream(filename);

    if(fileStream.is_open())
    {
        unsigned int vertices_number = 0;
        unsigned int triangles_number = 0;
        unsigned int reached_vertex_id = 0;
        unsigned int reached_edge_id = 0;
        unsigned int reached_triangle_id = 0;
        std::map<Vertex*, std::vector<Edge*> > vertices_edges;
        std::string line;
        std::getline(fileStream, line);
        if(line.compare("ply") != 0)
            return 1;
        std::getline(fileStream, line);
        if(line.compare("format ascii 1.0") == 0)
        {
            while(std::getline(fileStream, line) && line.substr(0, 7).compare("comment") == 0); //removing all comments
            if(line.substr(0, 14).compare("element vertex") != 0)
                return 2;
            vertices_number = stoi(line.substr(15, line.size() - 15));
            while(std::getline(fileStream, line) && line.substr(0, 7).compare("comment") == 0); //removing all comments
            if(line.substr(0, 8).compare("property") != 0)
                return 3;
            while(std::getline(fileStream, line) && line.substr(0, 7).compare("comment") == 0); //removing all comments
            if(line.substr(0, 8).compare("property") != 0)
                return 3;
            while(std::getline(fileStream, line) && line.substr(0, 7).compare("comment") == 0); //removing all comments
            if(line.substr(0, 8).compare("property") != 0)
                return 3;
            while(std::getline(fileStream, line) && line.substr(0, 7).compare("comment") == 0); //removing all comments
            if(line.substr(0, 12).compare("element face") != 0)
                return 4;
            triangles_number = stoi(line.substr(13, line.size() - 13));
            while(std::getline(fileStream, line) && line.substr(0, 7).compare("comment") == 0); //removing all comments
            if(line.compare("property list uchar int vertex_indices") != 0)
                return 5;
            while(std::getline(fileStream, line) && line.substr(0, 7).compare("comment") == 0); //removing all comments
            if(line.compare("end_header") != 0)
                return 6;
            std::cout << "Loading vertices: " << std::endl;
            for(unsigned int i = 0; i < vertices_number; i++)
            {
                if(!std::getline(fileStream, line))
                    return 7;
                std::stringstream sstream(line);
                double x, y, z;
                sstream >> x;
                sstream >> y;
                sstream >> z;
                Vertex* v = addNewVertex(x, y, z);
                v->setId(reached_vertex_id++);
                std::vector<Edge*> incident;
                vertices_edges.insert(std::make_pair(v, incident));
                if(i % 100 == 0)
                    std::cout << i * 100 / vertices_number << "%\r" << std::flush;;
            }
            std::cout << "Ended! Loaded " << vertices_number << " vertices." << std::endl << "Loading triangles:" << std::endl;
            for(unsigned int i = 0; i < triangles_number; i++)
            {
                if(!std::getline(fileStream, line))
                    return 8;
                std::stringstream sstream(line);
                unsigned int polyVertNum;
                unsigned int v1, v2, v3;
                Edge* e1, *e2, *e3;
                sstream >> polyVertNum;
                sstream >> v1;
                sstream >> v2;
                sstream >> v3;

                e1 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v1)), vertices.at(v2));
                if(e1 == nullptr)
                    e1 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v2)), vertices.at(v1));
                if(e1 == nullptr)
                {
                    addNewEdge(vertices.at(v1), vertices.at(v2));
                    e1 = edges.back();
                    e1->setId(reached_edge_id++);
                    vertices_edges.at(vertices.at(v1)).push_back(e1);
                }
                e2 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v2)), vertices.at(v3));
                if(e2 == nullptr)
                    e2 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v3)), vertices.at(v2));
                if(e2 == nullptr)
                {
                    addNewEdge(vertices.at(v2), vertices.at(v3));
                    e2 = edges.back();
                    e2->setId(reached_edge_id++);
                    vertices_edges.at(vertices.at(v2)).push_back(e2);
                }
                e3 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v3)), vertices.at(v1));
                if(e3 == nullptr)
                    e3 = searchEdgeContainingVertex(vertices_edges.at(vertices.at(v1)), vertices.at(v3));
                if(e3 == nullptr)
                {
                    addNewEdge(vertices.at(v3), vertices.at(v1));
                    e3 = edges.back();
                    e3->setId(reached_edge_id++);
                    vertices_edges.at(vertices.at(v3)).push_back(e3);
                }

                addNewTriangle(e1, e2, e3);
                triangles.back()->setId(reached_triangle_id++);
                if(e1->getT1() == nullptr)
                   e1->setT1(triangles.back());
                else if(e1->getT2() == nullptr)
                    e1->setT2(triangles.back());
                else
                {
                    e1->getT1()->print(std::cerr);
                    e1->getT1()->getV1()->print(std::cerr);
                    e1->getT1()->getV2()->print(std::cerr);
                    e1->getT1()->getV3()->print(std::cerr);
                    e1->getT2()->print(std::cerr);
                    e1->getT2()->getV1()->print(std::cerr);
                    e1->getT2()->getV2()->print(std::cerr);
                    e1->getT2()->getV3()->print(std::cerr);
                    triangles.back()->print(std::cerr);
                    triangles.back()->getV1()->print(std::cerr);
                    triangles.back()->getV2()->print(std::cerr);
                    triangles.back()->getV3()->print(std::cerr);
                    return 9;
                }
                if(e2->getT1() == nullptr)
                    e2->setT1(triangles.back());
                else if(e2->getT2() == nullptr)
                    e2->setT2(triangles.back());
                else
                    return 9;
                if(e3->getT1() == nullptr)
                    e3->setT1(triangles.back());
                else if(e3->getT2() == nullptr)
                    e3->setT2(triangles.back());
                else
                    return 9;

                if(i % 100 == 0)
                    std::cout << i * 100 / triangles_number << "%\r" << std::flush;;

            }
            std::cout << "Ended! Loaded " << triangles_number << " triangles." << std::endl;

        }
        fileStream.close();
        return 0;
    }
    else
        return -1;

}
