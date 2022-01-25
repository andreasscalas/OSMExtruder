
#include "coordsconverter.h"
#include "osmway.h"

#include <TriangleMesh.h>
#include <osmnode.h>
#include <osmrelation.h>
#include <shapefil.h>
#include <tinyxml2.h>

#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stack>
#include <map>
#include <boost/geometry.hpp>
#include <fstream>
#include <Point.h>
#include <geotiff.h>
#include <KDTreeVectorOfVectorsAdaptor.h>
#include <glob.h>

namespace Utilities {

    typedef std::vector<std::vector<double> > my_vector_of_vectors_t;
    typedef KDTreeVectorOfVectorsAdaptor< my_vector_of_vectors_t, double > my_kd_tree_t;
    typedef boost::geometry::model::d2::point_xy<double> boost_point;
    typedef boost::geometry::model::polygon<boost_point> boost_polygon;
    const double EPSILON_DETERMINANT = 1e-12;
    const double EPSILON_CONTAINMENT = 1e-8;

    int mod(int val, int m)
    {
        return (val % m + m) % m;
    }

    /**
     * @brief The Point3D class. Defines a 3D point
     */


    enum ERRORS {
        IOERROR
    };

    int loadOSM(std::string filename, std::vector<std::shared_ptr<OSMNode> > &nodes, std::vector<std::shared_ptr<OSMWay> > &ways, std::vector<std::shared_ptr<OSMRelation> > &relations)
    {
        tinyxml2::XMLDocument document;
        document.LoadFile(filename.c_str());

        tinyxml2::XMLElement* osmRoot = document.FirstChildElement();
        tinyxml2::XMLElement* xmlNode = osmRoot->FirstChildElement("node");
        std::map<unsigned int, unsigned int> osmid_pos;
        unsigned int counter = 0;
        CoordsConverter converter(32633);

        while(xmlNode != NULL){

            char *value;
            std::shared_ptr<OSMNode> node = std::make_shared<OSMNode>();
            std::vector<std::pair<std::string, std::string> > tags;
            const char* id = xmlNode->Attribute("id");
            if(id == NULL)
                return -1;
            node->setId(id);
            osmid_pos.insert(std::make_pair(atoi(id), counter++));
            const char* userName = xmlNode->Attribute("user");
            if(userName == NULL)
                return -2;
            node->setUserName(userName);
            const char* userId = xmlNode->Attribute("uid");
            if(userId == NULL)
                return -3;
            node->setUserId(userId);
            const char* latitude = xmlNode->Attribute("lat");
            const char* longitude = xmlNode->Attribute("lon");
            if(latitude == NULL || longitude == NULL)
                return -4;


            double lat = strtod(latitude, NULL), lon = strtod(longitude, NULL), x, y;
            converter.convertToUTM(lat, lon, x, y);
            std::shared_ptr<Point> coordinates = std::make_shared<Point>(x, y, 0);
            coordinates->setInfo(static_cast<void*>(node.get()));
            node->setCoordinates(coordinates);

            const char* isVisible = xmlNode->Attribute("visible");
            if(isVisible == NULL)
                return -5;
            node->setIsVisible(strcmp(isVisible, "true") == 0);

            const char* version = xmlNode->Attribute("version");
            if(version == NULL)
                return -6;
            node->setVersion(version);

            const char* changeset = xmlNode->Attribute("changeset");
            if(changeset == NULL)
                return -7;
            node->setChangeset(changeset);


            const char* timestamp = xmlNode->Attribute("timestamp");
            if(timestamp == NULL)
                return -8;
            node->setTimestamp(timestamp);

            tinyxml2::XMLElement* xmlTag = xmlNode->FirstChildElement("tag");
            while(xmlTag != NULL){
                const char* key = xmlTag->Attribute("k");
                const char* value = xmlTag->Attribute("v");
                if(key == NULL || value == NULL)
                    return -8;

                tags.push_back(std::make_pair(key, value));
                xmlTag = xmlTag->NextSiblingElement("tag");
            }

            node->setTags(tags);
            xmlNode = xmlNode->NextSiblingElement("node");

            nodes.push_back(node);
        }

        tinyxml2::XMLElement* xmlWay = osmRoot->FirstChildElement("way");
        counter = 0;

        while(xmlWay != NULL){

            char *value;
            std::shared_ptr<OSMWay> way = std::make_shared<OSMWay>();
            std::vector<std::pair<std::string, std::string> > tags;
            std::vector<std::shared_ptr<OSMNode> > connected;
            const char* id = xmlWay->Attribute("id");
            if(id == NULL)
                return -9;
            way->setId(id);
            osmid_pos.insert(std::make_pair(atoi(id), counter++));
            const char* userName = xmlWay->Attribute("user");
            if(userName == NULL)
                return -10;
            way->setUserName(userName);
            const char* userId = xmlWay->Attribute("uid");
            if(userId == NULL)
                return -11;
            way->setUserId(userId);

            const char* isVisible = xmlWay->Attribute("visible");
            if(isVisible == NULL)
                return -12;
            way->setIsVisible(strcmp(isVisible, "true") == 0);

            const char* version = xmlWay->Attribute("version");
            if(version == NULL)
                return -13;
            way->setVersion(version);

            const char* changeset = xmlWay->Attribute("changeset");
            if(changeset == NULL)
                return -14;
            way->setChangeset(changeset);

            const char* timestamp = xmlWay->Attribute("timestamp");
            if(timestamp == NULL)
                return -15;
            way->setTimestamp(timestamp);

            tinyxml2::XMLElement* xmlConnected = xmlWay->FirstChildElement("nd");
            while(xmlConnected != NULL){
                const char* ref = xmlConnected->Attribute("ref");
                if(ref == NULL)
                    return -16;

                connected.push_back(nodes.at(osmid_pos.at(atoi(ref))));
                xmlConnected = xmlConnected->NextSiblingElement("nd");
            }

            way->setNodes(connected);
            tinyxml2::XMLElement* xmlTag = xmlWay->FirstChildElement("tag");
            while(xmlTag != NULL){
                const char* key = xmlTag->Attribute("k");
                const char* value = xmlTag->Attribute("v");
                if(key == NULL || value == NULL)
                    return -17;

                tags.push_back(std::make_pair(key, value));
                xmlTag = xmlTag->NextSiblingElement("tag");
            }

            way->setTags(tags);
            xmlWay = xmlWay->NextSiblingElement("way");

            ways.push_back(way);
        }


        tinyxml2::XMLElement* xmlRelation = osmRoot->FirstChildElement("relation");
        counter = 0;

        while(xmlRelation != NULL){

            char *value;
            std::shared_ptr<OSMRelation> relation = std::make_shared<OSMRelation>();
            std::vector<std::pair<std::string, std::string> > tags;
            const char* id = xmlRelation->Attribute("id");
            if(id == NULL)
                return -18;
            relation->setId(id);
            osmid_pos.insert(std::make_pair(atoi(id), counter++));
            const char* userName = xmlRelation->Attribute("user");
            if(userName == NULL)
                return -19;
            relation->setUserName(userName);
            const char* userId = xmlRelation->Attribute("uid");
            if(userId == NULL)
                return -20;
            relation->setUserId(userId);

            const char* isVisible = xmlRelation->Attribute("visible");
            if(isVisible == NULL)
                return -21;
            relation->setIsVisible(strcmp(isVisible, "true") == 0);

            const char* version = xmlRelation->Attribute("version");
            if(version == NULL)
                return -22;
            relation->setVersion(version);

            const char* changeset = xmlRelation->Attribute("changeset");
            if(changeset == NULL)
                return -23;
            relation->setChangeset(changeset);

            const char* timestamp = xmlRelation->Attribute("timestamp");
            if(timestamp == NULL)
                return -24;
            relation->setTimestamp(timestamp);

            tinyxml2::XMLElement* xmlConnected = xmlRelation->FirstChildElement("member");
            while(xmlConnected != NULL){
                const char* type = xmlConnected->Attribute("type");
                const char* member_ref = xmlConnected->Attribute("ref");
                const char* role_pointer = xmlConnected->Attribute("role");
                if(type == NULL || member_ref == NULL || role_pointer == NULL)
                    return -25;
                std::string role(role_pointer);

                relation->setType(type);
                if(strcmp(type, "node") == 0)
                    if(osmid_pos.find(atoi(member_ref)) != osmid_pos.end())
                        relation->addNode(std::make_pair(nodes.at(osmid_pos.at(atoi(member_ref))), role));
                    else
                        relation->addNode(std::make_pair(nullptr, role));
                else if(strcmp(type, "way") == 0)
                    if(osmid_pos.find(atoi(member_ref)) != osmid_pos.end())
                        relation->addWay(std::make_pair(ways.at(osmid_pos.at(atoi(member_ref))), role));
                    else
                        relation->addWay(std::make_pair(nullptr, role));
                else if(strcmp(type, "relation") == 0)
                    if(osmid_pos.find(atoi(member_ref)) != osmid_pos.end())
                    {
                        relation->addRelation(std::make_pair(relations.at(osmid_pos.at(atoi(member_ref))), role));
                    } else
                    {
                        relation->addRelation(std::make_pair(nullptr, role));
                    }
                else
                    return -26;

                xmlConnected = xmlConnected->NextSiblingElement("members");
            }


            tinyxml2::XMLElement* xmlTag = xmlRelation->FirstChildElement("tag");
            while(xmlTag != NULL){
                const char* key = xmlTag->Attribute("k");
                const char* value = xmlTag->Attribute("v");
                if(key == NULL || value == NULL)
                    return -27;

                tags.push_back(std::make_pair(key, value));
                xmlTag = xmlTag->NextSiblingElement("tag");
            }

            relation->setTags(tags);
            xmlRelation = xmlRelation->NextSiblingElement("relation");

            relations.push_back(relation);
        }

        return 0;
    }
    /**
     * @brief load_shapefile_shp reader di shapefile (per gentile concessione di Daniela)
     * @param filename il nome (path + nome) dello shapefile
     * @param boundaries vettore che fa da contenitore ai poligoni nello shapefile
     * @return codice d'errore
     */
    int load_shapefile_shp(const std::string filename, std::vector<std::vector<std::shared_ptr<Point> >> &boundaries)
    {
        boundaries.clear();

        SHPHandle hSHP = SHPOpen(filename.c_str(), "rb");

        if (hSHP == NULL)
        {
            std::cerr << "Error while loading shapefile " << filename << std::endl;
            return IOERROR;
        }

        int nShapeType, nEntities;
        double adfBndsMin[4], adfBndsMax[4];

        SHPGetInfo(hSHP, &nEntities, &nShapeType, adfBndsMin, adfBndsMax);

        if ((nShapeType != SHPT_POLYGON) && (nShapeType != SHPT_POLYGONZ) &&
            (nShapeType != SHPT_ARC) && (nShapeType != SHPT_ARCZ))
        {
            std::cerr << "Error: no polygon nor line in the shapefile " << filename << std::endl;
            return 1;
        }

        for (int i = 0; i < nEntities; ++i)
        {
            SHPObject *obj = SHPReadObject(hSHP, i);

            boundaries.push_back(std::vector<std::shared_ptr<Point> > ());

            unsigned int size = obj->nVertices;
            if(nShapeType == SHPT_POLYGON || nShapeType == SHPT_POLYGONZ)
                size--;
            for (int j = 0; j < size; ++j)
            {
                std::shared_ptr<Point> point = std::make_shared<Point>(obj->padfX[j], obj->padfY[j], obj->padfZ[j]);
                if(j > 0 && *point == *boundaries.at(boundaries.size()-1).at(0))
                    break;
                boundaries.at(boundaries.size() - 1).push_back(point);
            }

            SHPDestroyObject(obj);
        }

        SHPClose(hSHP);

        return 0;
    }


    /**
     * @brief load_xyz_file reader di file xyz (nuvole di punti)
     * @param filename il nome (path + nome) del file xyz
     * @param points vettore che fa da contenitore ai punti della nuvola
     * @return codice d'errore
     */
//    int load_xyz_file(const std::string filename, std::vector<std::shared_ptr<Point> > &points)
//    {
//        points.clear();
//        std::ifstream file_stream(filename);

//        if(file_stream.is_open())
//        {
//            std::string line;
//            while(std::getline(file_stream, line))
//            {
//                std::stringstream stream(line);
//                double x, y, z;
//                bool bad_format = false;
//                if(!(stream >> x) || stream.fail())
//                    bad_format = true;
//                if(!(stream >> y) || stream.fail())
//                    bad_format = true;
//                if(!(stream >> z) || stream.fail())
//                    bad_format = true;

//                if(!bad_format)
//                {
//                    Point point(x, y, z);
//                    points.push_back(point);
//                } else {
//                    std::cerr << "File has bad format, reading is impossible" << std::endl << std::flush;
//                    return -1;
//                }
//            }
//        }
//        std::cout << "Loaded " << points.size() << " points" << std::endl;

//        return 0;
//    }


    unsigned int findNodeInList(std::vector<std::shared_ptr<OSMNode> > list, unsigned int begin, unsigned int end, std::shared_ptr<OSMNode> p)
    {
        for(unsigned int i = begin; i < end; i++)
            if(list.at(i)->getId() == p->getId())
                return i;
        return end;
    }

    std::vector<std::shared_ptr<Point> >::iterator findPointInList(std::vector<std::shared_ptr<Point> >::iterator begin, std::vector<std::shared_ptr<Point> >::iterator end, std::shared_ptr<Point> p)
    {
        for(std::vector<std::shared_ptr<Point> >::iterator it = begin; it != end; it++)
            if(**it == *p)
                return it;
        return end;
    }

    /**
     * @brief fix_polygon pulisce un poligono da vertici doppi e spikes.
     * @param boundary il poligono
     */
    void fix_polygon(std::vector<std::shared_ptr<Point> > &boundary)
    {
        for (unsigned int i = 0; i < boundary.size(); i++) {

            while(i < boundary.size() - 1 && (*boundary[i]) == (*boundary[i + 1]))
            {
                std::shared_ptr<Point> tmp = boundary.at(i + 1);
                boundary.erase(boundary.begin() + i + 1);
                tmp.reset();
            }

            std::vector<std::shared_ptr<Point> >::iterator it = findPointInList(boundary.begin() + i + 1, boundary.end(), boundary[i]);
            while(it != boundary.end())
            {
                for(std::vector<std::shared_ptr<Point> >::iterator it1 = boundary.begin() + i + 1; it1 <= it; it1++)
                    it1->reset();
                boundary.erase(boundary.begin() + i + 1, it + 1);
                it = findPointInList(boundary.begin() + i + 1, boundary.end(), boundary[i]);
            }
            if(i > 0)
            {
                if((*boundary[i]) == (*boundary[0]))
                {
                    for(unsigned int j = i + 1; j < boundary.size(); j++)
                        boundary.at(j).reset();
                    boundary.erase(boundary.begin() + i + 1, boundary.end());
                }
            }

        }


    }

    void fix_line(std::shared_ptr<OSMWay > way)
    {
        for (int i = 1; i < static_cast<int>(way->getNodes().size()); i++) {
            while(i < way->getNodes().size() - 1 && *way->getNodes().at(i)->getCoordinates() == *way->getNodes().at(i - 1)->getCoordinates())
            {
                std::shared_ptr<OSMNode> tmp = way->getNodes().at(i);
                way->removeNode(way->getNodes().at(i)->getId());
                tmp.reset();
            }

            unsigned int it = findNodeInList(way->getNodes(), i + 1, way->getNodes().size(), way->getNodes().at(i));
            while(it < way->getNodes().size() && way->getNodes().at(it)->getId() != way->getNodes().at(i)->getId())
            {
                for(unsigned int it1 = i + 1; it1 <= it; it1++)
                    way->getNodes().at(it1).reset();
                std::shared_ptr<OSMNode> tmp = way->getNodes().at(i);
                way->removeNode(way->getNodes().at(i)->getId());
                tmp.reset();
                it = findNodeInList(way->getNodes(), i + 1, way->getNodes().size(), way->getNodes().at(i));
            }

        }
    }

    void fix_line(std::vector<std::shared_ptr<Point> > &line)
    {
        for (int i = 0; i < static_cast<int>(line.size()) - 1; i++) {

            while(i < line.size() - 1 && (*line[i]) == (*line[i + 1]))
            {
                std::shared_ptr<Point> tmp = line.at(i + 1);
                line.erase(line.begin() + i + 1);
                tmp.reset();
            }

            std::vector<std::shared_ptr<Point> >::iterator it = findPointInList(line.begin() + i + 1, line.end(), line[i]);
            while(i > 0 && it != line.end())
            {
                for(std::vector<std::shared_ptr<Point> >::iterator it1 = line.begin() + i + 1; it1 <= it; it1++)
                    it1->reset();
                line.erase(line.begin() + i + 1, it + 1);
                it = findPointInList(line.begin() + i + 1, line.end(), line[i]);
            }

        }


    }

    /**
     * @brief isPointInSegment check di contenimento di un punto in un segmento
     * @param a punto di inizio del segmento
     * @param b punto di fine del segmento
     * @param p punto query
     * @return true se p è contenuto in ab, false altrimenti
     */
    bool isPointInSegment(Point* a, Point* b, Point* p)
    {
        if((*p) == (*a) || (*p) == (*b))
            return true;
        double l = ((*b) - (*a)).norm();
        double l1 = ((*p) - (*a)).norm();
        double l2 = ((*b) - (*p)).norm();
        double w1 = l1 / l;
        double w2 = l2 / l;
        return w1 + w2 <= 1.0 + EPSILON_CONTAINMENT;
    }


    inline double det(double a, double b, double c, double d)
    {
        return a * d - b * c;
    }

    double det3x3(double a11, double a12, double a13, double a21, double a22, double a23, double a31, double a32, double a33)
    {
        return (a11)*((a22)*(a33) - (a23)*(a32)) - (a12)*((a21)*(a33) - (a23)*(a31)) + (a13)*((a21)*(a32) - (a22)*(a31));
    }

    /**
     * @brief segmentsIntersection metodo per il check di intersezione tra segmenti (al momento funziona solo in 2D, deov ancora estenderlo).
     * @param p1 punto di inizio del segmento numero 1
     * @param p2 punto di fine del segmento numero 1
     * @param p3 punto di inizio del segmento numero 2
     * @param p4 punto di fine del segmento numero 2
     * @param pout il punto di intersezione (per riferimento)
     * @return true se i segmenti si intersecano, false altrimenti
     */
    bool segmentsIntersection(std::shared_ptr<Point> p1, std::shared_ptr<Point> p2, std::shared_ptr<Point> p3, std::shared_ptr<Point> p4, Point &pout)
    {

        double detL1 = det(p1->getX(), p1->getY(), p2->getX(), p2->getY());
        double detL2 = det(p3->getX(), p3->getY(), p4->getX(), p4->getY());
        double x1mx2 = p1->getX() - p2->getX();
        double x3mx4 = p3->getX() - p4->getX();
        double y1my2 = p1->getY() - p2->getY();
        double y3my4 = p3->getY() - p4->getY();

        double xnom = det(detL1, x1mx2, detL2, x3mx4);
        double ynom = det(detL1, y1my2, detL2, y3my4);
        double denom = det(x1mx2, y1my2, x3mx4, y3my4);
        if(abs(denom) < EPSILON_DETERMINANT)//Lines don't seem to cross
        {
            pout.setX(std::numeric_limits<double>::max());
            pout.setY(std::numeric_limits<double>::max());
            pout.setZ(0);
            return false;
        }

        pout.setX(xnom / denom);
        pout.setY(ynom / denom);
        pout.setZ(p1->getZ());
        if(!isfinite(pout.getX()) || !isfinite(pout.getY())) //Probably a numerical issue
            return false;

        bool result = isPointInSegment(p1.get(), p2.get(), &pout) && isPointInSegment(p3.get(), p4.get(), &pout);
        return result;

    }

    /**
     * @brief polygonsIntersect metodo per il check di intersezione tra poligoni
     * @param polygon1 poligono numero 1
     * @param polygon2 poligono numero 2
     * @return true se i poligoni si intersecano, false altrimenti
     */
    bool polygonsIntersect(std::vector<std::shared_ptr<Point> > polygon1, std::vector<std::shared_ptr<Point> > polygon2)
    {
        for(unsigned int i = 1; i < polygon1.size(); i++)
        {
            for(unsigned int j = 1; j < polygon2.size(); j++)
            {
                Point intersection;
                if(segmentsIntersection(polygon1[i - 1], polygon1[i], polygon2[j - 1], polygon2[j], intersection))
                    return true;
            }
        }
        return false;
    }

    Point linePlaneIntersection(const Point& p, const Point& q, const Point& r, const Point& s, const Point& t)
    {
        Point intersection(std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max());
        double den = det3x3(p.getX() - q.getX(),
                            p.getY() - q.getY(),
                            p.getZ() - q.getZ(),
                            s.getX() - r.getX(),
                            s.getY() - r.getY(),
                            s.getZ() - r.getZ(),
                            t.getX() - r.getX(),
                            t.getY() - r.getY(),
                            t.getZ() - r.getZ());
        if (den == 0) return intersection;
        double num = det3x3( p.getX() - r.getX(),
                             p.getY() - r.getY(),
                             p.getZ() - r.getZ(),
                             s.getX() - r.getX(),
                             s.getY() - r.getY(),
                             s.getZ() - r.getZ(),
                             t.getX() - r.getX(),
                             t.getY() - r.getY(),
                             t.getZ() - r.getZ());
        double gamma = num / den;
        return p + ((q - p) * gamma);
    }

    /**
     * @brief bbExtraction metodo per estrarre la AABB dai poligoni (riduzione costo computazionale del calcolo di intersezione)
     * @param points il poligono
     * @return array di 4 elementi (5 in realtà, il primo e l'ultimo sono uguali) codificante la AABB. Seguo questa codifica per compatibilità con gli altri poligoni.
     */
    std::vector<std::shared_ptr<Point> > bbExtraction(std::vector<std::shared_ptr<Point> > points)
    {
        std::vector<std::shared_ptr<Point> > bb;
        bb.push_back(std::make_shared<Point>(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 0));
        bb.push_back(std::make_shared<Point>(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 0));
        bb.push_back(std::make_shared<Point>(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), 0));
        bb.push_back(std::make_shared<Point>(std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), 0));

        for(unsigned int j = 0; j < points.size(); j++)
        {

            if(points[j]->getX() < bb[0]->getX())
            {
                bb[0]->setX(points[j]->getX());
                bb[3]->setX(points[j]->getX());
            }
            if(points[j]->getX() > bb[1]->getX())
            {
                bb[1]->setX(points[j]->getX());
                bb[2]->setX(points[j]->getX());
            }
            if(points[j]->getY() < bb[0]->getY())
            {
                bb[0]->setY(points[j]->getY());
                bb[1]->setY(points[j]->getY());
            }
            if(points[j]->getY() > bb[2]->getY())
            {
                bb[2]->setY(points[j]->getY());
                bb[3]->setY(points[j]->getY());
            }
        }
        bb.push_back(bb[0]);

        return bb;
    }

    /**
     * @brief mergeTouchingPolygons metodo per fondere poligoni a contatto. Correntemente non in funzione
     * @param boundaries insieme dei poligoni
     */
    void mergeTouchingPolygons(std::vector<std::vector<std::shared_ptr<Point> > > &boundaries)
    {
        using namespace boost::geometry;

        typedef model::d2::point_xy<double> point_2d;
        typedef model::polygon<point_2d> polygon_2d;
        typedef model::box<point_2d> box_2d;
        for(int i = 0; i < boundaries.size(); i++)
        {
            std::vector<std::shared_ptr<Point> > bb1 = bbExtraction(boundaries[i]);
            for(int j = 0; j < boundaries.size(); j++)
            {
                if(i == j)
                    continue;
                std::vector<std::shared_ptr<Point> > bb2 = bbExtraction(boundaries[j]);
                if(polygonsIntersect(bb1, bb2)){

                    if(polygonsIntersect(boundaries[i], boundaries[j]))
                    {
                        polygon_2d first_polygon;
                        polygon_2d second_polygon;
                        if(boundaries[i][0] != boundaries[i][boundaries[i].size() - 1])
                            boundaries[i].push_back(boundaries[i][0]);
                        if(boundaries[j][0] != boundaries[j][boundaries[j].size() - 1])
                            boundaries[j].push_back(boundaries[j][0]);
                        for(unsigned int k = 0; k < boundaries[i].size(); k++)
                        {
                            point_2d tmp;
                            tmp.x(boundaries[i][k]->getX());
                            tmp.y(boundaries[i][k]->getY());
                            append(first_polygon, tmp);
                        }
                        for(unsigned int k = 0; k < boundaries[j].size(); k++)
                        {
                            point_2d tmp;
                            tmp.x(boundaries[j][k]->getX());
                            tmp.y(boundaries[j][k]->getY());
                            append(second_polygon, tmp);
                        }
                        std::vector<polygon_2d> output;
                        boost::geometry::union_(first_polygon, second_polygon, output);
                        boundaries[i].clear();
                        for(unsigned int k = 0; k < output[0].outer().size(); k++)
                        {
                            point_2d tmp = output[0].outer()[k];
                            boundaries.at(i).push_back(std::make_shared<Point>(tmp.x(), tmp.y(), 0));
                        }

                        std::shared_ptr<Point> tmp = boundaries.at(i).at(j);
                        boundaries.at(i).erase(boundaries.at(i).begin() + j);
                        tmp.reset();
                        if(i > j) i--;
                        break;

                    }

                }
                for(unsigned int k = 0; k < 4; k++)
                    bb2[k].reset();
            }

            for(unsigned int j = 0; j < 4; j++)
                bb1[j].reset();
        }
    }

    void fix_polygons(std::vector<std::vector<std::shared_ptr<Point> > > &boundaries)
    {

        for (unsigned int i = 0; i < boundaries.size(); i++) {
            //First run: remove repeated vertices (successive pairs)

            fix_polygon(boundaries[i]);
            if(boundaries[i].size() < 4){
                for(unsigned int j = 0; j < boundaries[i].size() - 1; j++)
                    boundaries.at(i).at(j).reset();
                boundaries.erase(boundaries.begin() + i);
                i--;
            }

        }
    }

    void fix_lines(std::vector<std::shared_ptr<OSMWay> > &lines)
    {

        for (unsigned int i = 0; i < lines.size(); i++) {
            //First run: remove repeated vertices (successive pairs)

            fix_line(lines[i]);
            if(lines.at(i)->getNodes().size() < 2)
            {
                std::shared_ptr<OSMWay> tmp = lines.at(i);
                lines.erase(lines.begin() + i);
                tmp.reset();
                i--;
            }

        }
    }

    void fix_lines(std::vector<std::vector<std::shared_ptr<Point> > > &lines)
    {

        for (unsigned int i = 0; i < lines.size(); i++) {
            //First run: remove repeated vertices (successive pairs)

            fix_line(lines[i]);
            if(lines.at(i).size() < 2)
            {
                lines.erase(lines.begin() + i);
                i--;
            }

        }
    }

    bool isPointInsidePolygon(std::shared_ptr<Point> v, std::vector<std::shared_ptr<Point> > boundary){

        unsigned int c = 0;
        boundary.push_back(boundary.at(0));
        for ( unsigned int i = 1; i < boundary.size(); i++) {
            if ( ((boundary[i]->getY() >= v->getY()) != (boundary[i - 1]->getY() >= v->getY())) &&
                 (v->getX() <= (boundary[i - 1]->getX() - boundary[i]->getX()) * (v->getY() - boundary[i]->getY())
                    / (boundary[i - 1]->getY() - boundary[i]->getY()) + boundary[i]->getX()) )
               c = !c;
        }
        return c;
    }

    bool isPolygonInsidePolygon(std::vector<std::shared_ptr<Point> > boundary1, std::vector<std::shared_ptr<Point> > boundary2){

        return isPointInsidePolygon(boundary1[0], boundary2);
    }

    /**
     * @brief extractPolygonsHeights1 assolutamente da ottimizzare
     * @param boundaries
     * @param dhm_tiff
     * @return
     */
    std::vector<std::vector<std::pair<unsigned int, unsigned int> > > extractPolygonsHeights(std::vector<std::vector<std::shared_ptr<Point> > > boundaries, GeoTiff* dhm_tiff, double scale_factor, Point origin)
    {
        std::vector<std::vector<std::pair<unsigned int, unsigned int> > > heights;
        unsigned int counter = 0;
        std::map<unsigned int, std::pair<unsigned int, unsigned int> > index_to_rowcol;
        for(unsigned int i = 0; i < boundaries.size(); i++)
        {
            std::vector<std::pair<unsigned int, unsigned int> > boundary_heights;
            heights.push_back(boundary_heights);
        }
        float** dhm_heights = dhm_tiff->GetRasterBand(1);
        double* geoTransform = dhm_tiff->GetGeoTransform();
        geoTransform[0] = (geoTransform[0] - origin.getX()) / scale_factor;  //x del punto in alto a sinistra
        geoTransform[3] = (geoTransform[3] - origin.getY()) / scale_factor;  //y del punto in alto a sinistra
        geoTransform[1] /= scale_factor;
        geoTransform[5] /= scale_factor;
        double pixel_diagonal_length = sqrt(pow(geoTransform[1], 2) + pow(geoTransform[5], 2));
        my_vector_of_vectors_t points_vector;
        for(unsigned int i = 0; i < dhm_tiff->GetDimensions()[0]; i++)
        {
            for(unsigned int j = 0; j < dhm_tiff->GetDimensions()[1]; j++)
            {
                std::vector<double> point = { geoTransform[0] + j * geoTransform[1] + i * geoTransform[2],
                                              geoTransform[3] + j * geoTransform[4] + i * geoTransform[5],
                                              0};
                points_vector.push_back(point);
                index_to_rowcol.insert(std::make_pair(counter++, std::make_pair(i,j)));
            }
        }
        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
        mat_index->index->buildIndex();
        for(unsigned int i = 0; i < boundaries.at(0).size(); i++)
        {
            double query_point[3] = {boundaries.at(0).at(i)->getX(), boundaries.at(0).at(i)->getY(), 0};
            std::vector<size_t> ret_index(1);
            std::vector<double> out_dist_sqr(1);
            mat_index->index->knnSearch(query_point, 1, &ret_index[0], &out_dist_sqr[0]);
            heights.at(0).push_back(index_to_rowcol.at(ret_index[0]));
        }

        counter = 0;

        #pragma omp parallel for num_threads(31)
        for(unsigned int i = 1; i < boundaries.size(); i++)
        {
            std::vector<std::shared_ptr<Point> > bb = bbExtraction(boundaries[i]);
            double sphere_radius = std::max(pixel_diagonal_length, ((*bb[0]) - (*bb[2])).norm()) / 2;
            Point middle = ((*bb[0]) + (*bb[1]) + (*bb[2]) + (*bb[3])) / 4;

            std::vector<std::pair<size_t, double> > neighbors_distances;
            std::vector<std::pair<unsigned int, unsigned int> > neighbors;
            nanoflann::SearchParams params;
            double query_point[3] = {middle.getX(), middle.getY(), 0};
            mat_index->index->radiusSearch(query_point, sphere_radius, neighbors_distances, params);
            for(std::vector<std::pair<size_t,double> >::iterator it = neighbors_distances.begin(); it != neighbors_distances.end(); it++){
                std::pair<size_t, double> p = static_cast<std::pair<long int, double> >(*it);

                neighbors.push_back(index_to_rowcol.at(p.first));
            }

            for(unsigned int j = 0; j < neighbors.size(); j++)
            {
                double min_x = geoTransform[0] + neighbors.at(j).second * geoTransform[1] + neighbors.at(j).first * geoTransform[2];
                double min_y = geoTransform[3] + neighbors.at(j).second * geoTransform[4] + neighbors.at(j).first * geoTransform[5];
                std::vector<std::shared_ptr<Point> > frame;
                frame.push_back(std::make_shared<Point>(min_x, min_y, 0));
                frame.push_back(std::make_shared<Point>(min_x + geoTransform[1], min_y, 0));
                frame.push_back(std::make_shared<Point>(frame.back()->getX(), min_y + geoTransform[5], 0));
                frame.push_back(std::make_shared<Point>(min_x, frame.back()->getY(), 0));
                frame.push_back(frame[0]);
                if(((*frame[0]) - (*boundaries[i][0])).norm() < std::max(((*bb[0]) - (*bb[2])).norm(), ((*frame[0]) - (*frame[2])).norm()))
                {
                    if(isPointInsidePolygon(boundaries[i][0], frame) ||                                         //Se un punto del poligono è dentro al pixel del geotiff...
                       (isPointInsidePolygon(frame[0], bb) && isPointInsidePolygon(frame[0], boundaries[i])) || //o il pixel è contenuto nel poligono...
                       polygonsIntersect(frame, boundaries[i]))                                                 //o ancora poligono e pixel si intersecano
                    {
                        heights[i].push_back(std::make_pair(neighbors.at(j).first, neighbors.at(j).second));
                    }
                }
                for(unsigned int l = 0; l < 4; l++)
                    frame.at(l).reset();
            }
            if(heights[i].size() == 0)
            {
                #pragma omp critical
                {
                    std::cerr << "Impossible case: boundary does not include any pixel while no pixel wholly include the boundary and boundary intersects no pixel" << std::endl;
                    std::cerr << geoTransform[0] << " " << geoTransform[1] << " " << geoTransform[2] << " " << std::endl;
                    std::cerr << geoTransform[3] << " " << geoTransform[4] << " " << geoTransform[5] << " " << std::endl << std::flush;
                    std::cerr << dhm_tiff->GetDimensions()[0] << " " << dhm_tiff->GetDimensions()[1] << std::endl;
                    for(unsigned int j = 0; j < boundaries[i].size(); j++)
                        boundaries[i][j]->print(std::cerr);
                    exit(6);
                }
            }
            #pragma omp critical
            {
                counter++;
                std::cout << counter * 100 / boundaries.size() << "%\r" << std::flush;
            }

        }
        std::cout << std::endl;
        delete mat_index;
        for(unsigned int i = 0; i < dhm_tiff->GetDimensions()[0]; i++)
            if(dhm_heights[i] != nullptr)
                delete[] dhm_heights[i];

        geoTransform[0] = (geoTransform[0] * scale_factor + origin.getX());  //x del punto in alto a sinistra
        geoTransform[3] = (geoTransform[3] * scale_factor + origin.getY());  //y del punto in alto a sinistra
        geoTransform[1] *= scale_factor;
        geoTransform[5] *= scale_factor;
        return heights;
    }

    void associateHeights(TriangleMesh* mesh, GeoTiff* dhm_tiff, double scale_factor, Point origin)
    {
        unsigned int counter = 0;
        float** dhm_heights = dhm_tiff->GetRasterBand(1);
        double* geoTransform = dhm_tiff->GetGeoTransform();
        geoTransform[0] = (geoTransform[0] - origin.getX()) / scale_factor;  //x del punto in alto a sinistra
        geoTransform[3] = (geoTransform[3] - origin.getY()) / scale_factor;  //y del punto in alto a sinistra
        geoTransform[1] /= scale_factor;
        geoTransform[5] /= scale_factor;
        my_vector_of_vectors_t points_vector;
        for(unsigned int i = 0; i < dhm_tiff->GetDimensions()[0]; i++)
        {
            for(unsigned int j = 0; j < dhm_tiff->GetDimensions()[1]; j++)
            {
                std::vector<double> point = { geoTransform[0] + j * geoTransform[1] + i * geoTransform[2],
                                              geoTransform[3] + j * geoTransform[4] + i * geoTransform[5],
                                              0};
                points_vector.push_back(point);
            }
        }
        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
        mat_index->index->buildIndex();

        #pragma omp parallel for num_threads(31)
        for(unsigned int i = 0; i < mesh->getVerticesNumber(); i++)
        {
            double point[3] = {mesh->getVertex(i)->getX(), mesh->getVertex(i)->getY(), 0};
            std::vector<size_t> ret_index(1);
            std::vector<double> out_dist_sqr(1);
            mat_index->index->knnSearch(point, 1, &ret_index[0], &out_dist_sqr[0]);
            unsigned int maxPos = -1;
            unsigned int row = ret_index[0] / dhm_tiff->GetDimensions()[1];
            unsigned int col = ret_index[0] % dhm_tiff->GetDimensions()[1];
            mesh->getVertex(i)->setZ(dhm_heights[row][col]);
            #pragma omp critical
            {
                counter++;
                std::cout << counter * 100 / mesh->getVerticesNumber() << "%\r" << std::flush;
            }

        }

        std::cout << std::endl;

        for(unsigned int i = 0; i < dhm_tiff->GetDimensions()[0]; i++)
                if(dhm_heights[i] != nullptr)
                    delete[] dhm_heights[i];
        delete mat_index;
        geoTransform[0] = (geoTransform[0] * scale_factor + origin.getX());  //x del punto in alto a sinistra
        geoTransform[3] = (geoTransform[3] * scale_factor + origin.getY());  //y del punto in alto a sinistra
        geoTransform[1] *= scale_factor;
        geoTransform[5] *= scale_factor;

    }

    void refineLines(std::vector<std::vector<std::shared_ptr<Point> > > &lines, GeoTiff* dhm_tiff, double scale_factor, Point origin)
    {

        float** dhm_heights = dhm_tiff->GetRasterBand(1);
        double* geoTransform = dhm_tiff->GetGeoTransform();
        geoTransform[0] = (geoTransform[0] - origin.getX()) / scale_factor;  //x del punto in alto a sinistra
        geoTransform[3] = (geoTransform[3] - origin.getY()) / scale_factor;  //y del punto in alto a sinistra
        geoTransform[1] /= scale_factor;
        geoTransform[5] /= scale_factor;
        double pixel_diagonal_length = sqrt(pow(geoTransform[1], 2) + pow(geoTransform[5], 2));
        my_vector_of_vectors_t points_vector;
        unsigned int counter = 0;

        for(unsigned int i = 0; i < dhm_tiff->GetDimensions()[0]; i++)
        {
            for(unsigned int j = 0; j < dhm_tiff->GetDimensions()[1]; j++)
            {
                std::vector<double> point = { geoTransform[0] + j * geoTransform[1] + i * geoTransform[2],
                                              geoTransform[3] + j * geoTransform[4] + i * geoTransform[5],
                                              0};
                points_vector.push_back(point);

            }
        }
        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
        mat_index->index->buildIndex();

        #pragma omp parallel for num_threads(31)
        for(unsigned int i = 0; i < lines.size(); i++)
        {

            for(unsigned int j = 1; j < lines.at(i).size(); j++)
            {
                std::vector<std::shared_ptr<Point> > intersections;
                std::shared_ptr<Point> v1 = lines.at(i).at(j - 1);
                std::shared_ptr<Point> v2 = lines.at(i).at(j);
                Point middle(((*v1) + (*v2))  / 2);
                double sphere_radius = std::max(pixel_diagonal_length, ((*v1) - (*v2)).norm() / 2);

                std::vector<std::pair<size_t, double> > neighbors_distances;
                std::vector<std::pair<unsigned int, unsigned int> > neighbors;
                nanoflann::SearchParams params;
                double query_point[3] = {middle.getX(), middle.getY(), 0};
                mat_index->index->radiusSearch(query_point, sphere_radius, neighbors_distances, params);
                for(std::vector<std::pair<size_t,double> >::iterator it = neighbors_distances.begin(); it != neighbors_distances.end(); it++){
                    std::pair<size_t, double> p = static_cast<std::pair<long int, double> >(*it);
                    neighbors.push_back(std::make_pair(p.first / dhm_tiff->GetDimensions()[0], p.first % dhm_tiff->GetDimensions()[0]));
                }
                for(unsigned int k = 0; k < neighbors.size(); k++)
                {
                    double min_x = geoTransform[0] + neighbors.at(k).second * geoTransform[1] + neighbors.at(k).first * geoTransform[2];
                    double min_y = geoTransform[3] + neighbors.at(k).second * geoTransform[4] + neighbors.at(k).first * geoTransform[5];
                    std::vector<std::shared_ptr<Point> > frame;
                    frame.push_back(std::make_shared<Point>(min_x, min_y, 0));
                    frame.push_back(std::make_shared<Point>(min_x + geoTransform[1], min_y, 0));
                    frame.push_back(std::make_shared<Point>(frame.back()->getX(), min_y + geoTransform[5], 0));
                    frame.push_back(std::make_shared<Point>(min_x, frame.back()->getY(), 0));
                    frame.push_back(frame[0]);

                    for(unsigned int l = 1; l < 5; l++)
                    {
                        std::shared_ptr<Point> pout = std::make_shared<Point>();
                        bool intersects = Utilities::segmentsIntersection(v1, v2, frame.at(l - 1), frame.at(l), *pout);

                        if(intersects)
                        {
                            int pos = -1;
                            for(unsigned int m = 0; m < intersections.size(); m++)
                                if(*intersections.at(m) == *pout)
                                    pos = m;
                            if(pos < 0 && *pout != *v1 && *pout != *v2)
                                intersections.push_back(pout);
                            else if(pout != nullptr)
                                pout.reset();
                        } else if(pout != nullptr)
                            pout.reset();
                    }

                    for(unsigned int l = 1; l < 4; l++)
                        frame.at(l).reset();
                }
                neighbors.clear();
                neighbors_distances.clear();
                std::sort(intersections.begin(), intersections.end(), [v1](std::shared_ptr<Point> a, std::shared_ptr<Point> b) {
                    return ((*a) - (*v1)).norm() < ((*b) - (*v1)).norm();
                });

                #pragma omp critical
                {
                    lines.at(i).insert(lines.at(i).begin() + j, intersections.begin(), intersections.end());
                }
                j += intersections.size();

            }

            #pragma omp critical
            {
                counter++;
                std::cout << counter * 100 / lines.size() << "%\r" << std::flush;
            }
        }

        delete mat_index;
        std::cout << std::endl;
        for(unsigned int i = 0; i < dhm_tiff->GetDimensions()[0]; i++)
                if(dhm_heights[i] != nullptr)
                    delete[] dhm_heights[i];
        geoTransform[0] = (geoTransform[0] * scale_factor + origin.getX());  //x del punto in alto a sinistra
        geoTransform[3] = (geoTransform[3] * scale_factor + origin.getY());  //y del punto in alto a sinistra
        geoTransform[1] *= scale_factor;
        geoTransform[5] *= scale_factor;
    }

    std::shared_ptr<Vertex> extractNearestVertex(std::vector<std::shared_ptr<Vertex>> &frontier, std::map<std::shared_ptr<Vertex>, double> distances){

        double minDist = DBL_MAX;
        int minPos = -1;
        for(unsigned int i = 0; i < frontier.size(); i++){
            if(distances[frontier[i]] < minDist){
                minPos = i;
                minDist = distances[frontier[i]];
            }
        }

        if(minPos == -1)
            return nullptr;

        std::shared_ptr<Vertex> nearest = frontier[minPos];

        frontier.erase(frontier.begin() + minPos);

        return nearest;

    }

    std::vector<std::shared_ptr<Vertex>> dijkstra(std::shared_ptr<Vertex> v1, std::shared_ptr<Vertex> v2){

        std::vector<std::shared_ptr<Vertex>> frontier;
        std::map<std::shared_ptr<Vertex>, double> distances = {{v1, 0}};
        std::map<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>> predecessors = {{v1, nullptr}};
        std::set<Vertex*> v21RingNeighbors;
        std::vector<std::shared_ptr<Vertex>> shortestPath;
        std::shared_ptr<Vertex> v;
        bool v2visited = false;

        if((*v1) == (*v2)){
            return shortestPath;
        }

        frontier.push_back(v1);

        do{

            v = extractNearestVertex(frontier, distances);
            if(v == nullptr)
            {
                return shortestPath;
            }

            std::vector<std::shared_ptr<Vertex>> neighbors = v->getVV();
            for(std::vector<std::shared_ptr<Vertex> >::iterator n = neighbors.begin(); n != neighbors.end(); n++){
                std::shared_ptr<Vertex> x = *n;

                if(x->getId() == v2->getId() && v21RingNeighbors.find(&(*v)) == v21RingNeighbors.end())
                    v21RingNeighbors.insert(&(*v));

                std::map<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>::iterator pit = predecessors.find(x);
                double distanceVX;

                distanceVX = distances[v] + x->computePointSegmentDistance(*v1, *v2);


                if(pit != predecessors.end()){
                    if(distances[x] > distanceVX){
                        distances[x] = distanceVX;
                        predecessors[x] = v;
                    }
                } else {
                    distances.insert(std::make_pair(x, distanceVX));
                    predecessors.insert(std::make_pair(x, v));
                    frontier.push_back(x);
                }
            }
            if(v->getId() == v2->getId())
                v2visited = true;

        } while(!v2visited);

        shortestPath.push_back(v2);
        v = predecessors[v2];
        while(v != v1){
            shortestPath.insert(shortestPath.begin(), v);
            v = predecessors[v];
        }

        return shortestPath;
    }

    void removeOSMWayPointsInPolygons(std::vector<std::shared_ptr<OSMWay>> &lines, std::vector<std::vector<std::shared_ptr<Point> > > polygons)
    {
        my_vector_of_vectors_t points_vector;
        std::vector<unsigned int> sizes(lines.size());
        std::map<unsigned int, unsigned int > pointPolygonLink;
        unsigned int id = 0;
        double meanDiagonal = 0;
        for(unsigned int i = 0; i < polygons.size(); i++)
        {
            for(unsigned int j = 0; j < polygons[i].size() - 1; j++){
                std::vector<double> point = { polygons.at(i).at(j)->getX(),
                                              polygons.at(i).at(j)->getY(),
                                              polygons.at(i).at(j)->getZ()};
                points_vector.push_back(point);
                pointPolygonLink.insert(std::make_pair(id++, i));
            }
            std::vector<std::shared_ptr<Point> > bb = bbExtraction(polygons[i]);
            double diagMeasure = (*bb[2] - *bb[0]).norm();
            meanDiagonal += diagMeasure;
        }
        meanDiagonal /= polygons.size();

        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
        mat_index->index->buildIndex();
        unsigned int counter = 0;

        #pragma omp parallel for num_threads(31)
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            std::vector<std::shared_ptr<OSMNode> > nodes = lines.at(i)->getNodes();
            for(unsigned int j = 0; j < nodes.size(); j++){
                std::vector<std::shared_ptr<Point> > intersections;
                std::shared_ptr<Point> v = nodes.at(j)->getCoordinates();
                std::vector<std::pair<size_t, double> > neighbors_distances;
                std::vector<unsigned int> neighboringPolygons;
                nanoflann::SearchParams params;
                double query_point[3] = {v->getX(), v->getY(), v->getZ()};
                neighbors_distances.clear();
                mat_index->index->radiusSearch(query_point, meanDiagonal / 2, neighbors_distances, params);
                for(std::vector<std::pair<size_t,double> >::iterator it = neighbors_distances.begin(); it != neighbors_distances.end(); it++){
                    std::pair<size_t, double> p = static_cast<std::pair<long int, double> >(*it);
                    std::vector<unsigned int>::iterator pit = std::find(neighboringPolygons.begin(), neighboringPolygons.end(), pointPolygonLink.at(p.first));
                    if(pit == neighboringPolygons.end())
                        neighboringPolygons.push_back(pointPolygonLink.at(p.first));
                }
                for(unsigned int k = 0; k < neighboringPolygons.size(); k++)
                {
                    if(isPointInsidePolygon(v, polygons.at(neighboringPolygons.at(k))))
                    {
                        bool onBoundary = false;
                        for(unsigned int l = 1; l < polygons.at(neighboringPolygons.at(k)).size(); l++)
                        {
                            std::shared_ptr<Point> p1 = polygons.at(neighboringPolygons.at(k)).at(l - 1);
                            std::shared_ptr<Point> p2 = polygons.at(neighboringPolygons.at(k)).at(l);
                            if(Utilities::isPointInSegment(p1.get(), p2.get(), v.get()))
                            {
                                onBoundary = true;
                                break;
                            }
                        }
                        if(!onBoundary)
                        {
                            lines.at(i)->removeNode(nodes.at(j)->getId());
                            nodes.erase(nodes.begin() + j);
                            v.reset();
                            j--;
                            break;
                        }
                    }
                }
            }

            #pragma omp critical
            {
                counter++;
                std::cout << counter * 100 / lines.size() << "%\r" << std::flush;
            }
            sizes.at(i) = nodes.size();
        }

        std::cout << std::endl;
    }


    void removeLinePointsInPolygons(std::vector<std::vector<std::shared_ptr<Point> > > &lines, std::vector<std::vector<std::shared_ptr<Point> > > polygons)
    {
        my_vector_of_vectors_t points_vector;
        std::map<unsigned int, unsigned int > pointPolygonLink;
        unsigned int id = 0;
        double meanDiagonal = 0;
        for(unsigned int i = 0; i < polygons.size(); i++)
        {
            for(unsigned int j = 0; j < polygons[i].size() - 1; j++){
                std::vector<double> point = { polygons.at(i).at(j)->getX(),
                                              polygons.at(i).at(j)->getY(),
                                              polygons.at(i).at(j)->getZ()};
                points_vector.push_back(point);
                pointPolygonLink.insert(std::make_pair(id++, i));
            }
            std::vector<std::shared_ptr<Point> > bb = bbExtraction(polygons[i]);
            double diagMeasure = (*bb[2] - *bb[0]).norm();
            meanDiagonal += diagMeasure;
        }
        meanDiagonal /= polygons.size();

        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
        mat_index->index->buildIndex();
        unsigned int counter = 0;

        #pragma omp parallel for num_threads(31)
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            for(unsigned int j = 0; j < lines[i].size(); j++){
                std::vector<std::shared_ptr<Point> > intersections;
                std::shared_ptr<Point> v = lines.at(i).at(j);
                std::vector<std::pair<size_t, double> > neighbors_distances;
                std::vector<unsigned int> neighboringPolygons;
                nanoflann::SearchParams params;
                double query_point[3] = {v->getX(), v->getY(), v->getZ()};
                neighbors_distances.clear();
                mat_index->index->radiusSearch(query_point, meanDiagonal / 2, neighbors_distances, params);
                for(std::vector<std::pair<size_t,double> >::iterator it = neighbors_distances.begin(); it != neighbors_distances.end(); it++){
                    std::pair<size_t, double> p = static_cast<std::pair<long int, double> >(*it);
                    std::vector<unsigned int>::iterator pit = std::find(neighboringPolygons.begin(), neighboringPolygons.end(), pointPolygonLink.at(p.first));
                    if(pit == neighboringPolygons.end())
                        neighboringPolygons.push_back(pointPolygonLink.at(p.first));
                }
                for(unsigned int k = 0; k < neighboringPolygons.size(); k++)
                {
                    if(isPointInsidePolygon(v, polygons.at(neighboringPolygons.at(k))))
                    {
                        bool onBoundary = false;
                        for(unsigned int l = 1; l < polygons.at(neighboringPolygons.at(k)).size(); l++)
                        {
                            std::shared_ptr<Point> p1 = polygons.at(neighboringPolygons.at(k)).at(l - 1);
                            std::shared_ptr<Point> p2 = polygons.at(neighboringPolygons.at(k)).at(l);
                            if(Utilities::isPointInSegment(p1.get(), p2.get(), v.get()))
                            {
                                onBoundary = true;
                                break;
                            }
                        }
                        if(!onBoundary)
                        {
                            lines.at(i).erase(lines.at(i).begin() + j);
                            v.reset();
                            j--;
                            break;
                        }
                    }
                }
            }

            #pragma omp critical
            {
                counter++;
                std::cout << counter * 100 / lines.size() << "%\r" << std::flush;
            }
        }
        for(unsigned int i = 0; i < lines.size(); i++)
            if(lines.at(i).size() < 2)
            {
                lines.erase(lines.begin() + i);
                i--;
            }
        std::cout << std::endl;

    }

    std::vector<std::pair<unsigned int, unsigned int> > removeSegmentsIntersectingPolygons(std::vector<std::vector<std::shared_ptr<Point> > > &lines, std::vector<std::vector<std::shared_ptr<Point> > > &polygons)
    {
        std::vector<std::pair<unsigned int, unsigned int>> keptClippings;
        my_vector_of_vectors_t points_vector;
        std::map<unsigned int, unsigned int > pointPolygonLink;
        unsigned int id = 0, counter = 0;
        double meanDiagonal = 0;
        for(unsigned int i = 0; i < polygons.size(); i++)
        {
            for(unsigned int j = 0; j < polygons[i].size() - 1; j++){
                std::vector<double> point = { polygons.at(i).at(j)->getX(),
                                              polygons.at(i).at(j)->getY(),
                                              polygons.at(i).at(j)->getZ()};
                points_vector.push_back(point);
                pointPolygonLink.insert(std::make_pair(id++, i));
            }
            std::vector<std::shared_ptr<Point> > bb = bbExtraction(polygons[i]);
            double diagMeasure = (*bb[2] - *bb[0]).norm();
            meanDiagonal += diagMeasure;
        }
        meanDiagonal /= polygons.size();

        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
        mat_index->index->buildIndex();

        for(unsigned int i = 0; i < lines.size(); i++)
        {
            unsigned int deviation = 0;
            for(unsigned int j = 1; j < lines[i].size(); j++){
                std::vector<std::shared_ptr<Point> > intersections;
                std::shared_ptr<Point> v = lines.at(i).at(j);
                std::vector<std::pair<size_t, double> > neighbors_distances;
                std::vector<unsigned int> neighboringPolygons;
                nanoflann::SearchParams params;
                double query_point[3] = {v->getX(), v->getY(), v->getZ()};
                neighbors_distances.clear();
                mat_index->index->radiusSearch(query_point, meanDiagonal / 2, neighbors_distances, params);
                for(std::vector<std::pair<size_t,double> >::iterator it = neighbors_distances.begin(); it != neighbors_distances.end(); it++){
                    std::pair<size_t, double> p = static_cast<std::pair<long int, double> >(*it);
                    std::vector<unsigned int>::iterator pit = std::find(neighboringPolygons.begin(), neighboringPolygons.end(), pointPolygonLink.at(p.first));
                    if(pit == neighboringPolygons.end())
                        neighboringPolygons.push_back(pointPolygonLink.at(p.first));
                }

                for(unsigned int k = 0; k < neighboringPolygons.size(); k++)
                {
                    std::vector<std::shared_ptr<Point> > intersections;
                    for(unsigned int l = 0; l < polygons.at(neighboringPolygons.at(k)).size(); l++)
                    {
                        std::shared_ptr<Point> pout = std::make_shared<Point>();
                        if(segmentsIntersection(lines.at(i).at(j - 1), lines.at(i).at(j), polygons.at(neighboringPolygons.at(k)).at(l), polygons.at(neighboringPolygons.at(k)).at((l + 1) % polygons.at(neighboringPolygons.at(k)).size()), *pout))
                        {
                            polygons.at(neighboringPolygons.at(k)).insert(polygons.at(neighboringPolygons.at(k)).begin() + l + 1, pout);
                            l++;
                            intersections.push_back(pout);
                        }

                    }
                    if(intersections.size() > 1)
                    {

                        if(intersections.size() % 2 != 0)
                        {
                            //Da gestire caso in cui segmento traversa uno "spuntone" di poligono e poi "tocca" una punta (tecnicamente sono 3 punti).
                            //Al momento si ignora il caso, eliminando l'ultima intersezione e considerandolo come un caso con numero pari di intersezioni
                            intersections.pop_back();
                        }

                        unsigned int firstPos = intersections.size() - 1, lastPos = 0;
                        if((*intersections.at(intersections.size() - 1) - *lines.at(i).at(j - 1)).norm() < (*intersections.at(0) - *lines.at(i).at(j - 1)).norm())
                        {
                            firstPos = 0;
                            lastPos = intersections.size() - 1;
                        }
                        std::vector<std::shared_ptr<Point> > remaining(lines.at(i).begin() + j, lines.at(i).end());
                        lines.insert(lines.begin() + i + 1, remaining);
                        lines.at(i + 1).insert(lines.at(i + 1).begin(), intersections.at(firstPos));
                        lines.at(i).erase(lines.at(i).begin() + j, lines.at(i).end());
                        lines.at(i).push_back(intersections.at(lastPos));
                        deviation = intersections.size() / 2;
                        keptClippings.push_back(std::make_pair(i, i + deviation));
                        for(unsigned int l = 1; l < intersections.size() / 2; l++)
                        {
                            std::vector<std::shared_ptr<Point> > exteriorSegment = {intersections.at(l * 2 - 1), intersections.at(l * 2)};
                            lines.insert(lines.begin() + i + 1, exteriorSegment);
                        }

                    }
                }

                i += deviation;
            }

            counter++;
            std::cout << counter * 100 / lines.size() << "%\r" << std::flush;
        }

        std::cout << std::endl;
        return keptClippings;
    }

    std::vector<std::vector<std::shared_ptr<Point> > > getClosest(std::vector<std::vector<std::shared_ptr<Point> > > list, std::vector<std::vector<std::shared_ptr<Point> > > p)
    {
        my_vector_of_vectors_t points_vector;
        std::vector<std::vector<std::shared_ptr<Point> > > closest_points;
        std::map<unsigned int, std::shared_ptr<Point> > points_pos;
        unsigned int counter = 0;
        for(unsigned int i = 0; i < list.size(); i++)
        {
            for(unsigned int j = 0; j < list[i].size() - 1; j++){
                std::vector<double> point = { list.at(i).at(j)->getX(),
                                              list.at(i).at(j)->getY(),
                                              list.at(i).at(j)->getZ()};
                points_vector.push_back(point);
                points_pos.insert(std::make_pair(counter++, list.at(i).at(j)));
            }
        }

        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
        mat_index->index->buildIndex();
        for(unsigned int i = 0; i < p.size(); i++)
        {
            std::vector<std::shared_ptr<Point> > closest;
            for(unsigned int j = 0; j < p.at(i).size(); j++)
            {
                nanoflann::SearchParams params;
                double point[3] = {p.at(i).at(j)->getX(), p.at(i).at(j)->getY(), p.at(i).at(j)->getZ()};
                std::vector<size_t> ret_index(1);
                std::vector<double> out_dist_sqr(1);
                mat_index->index->knnSearch(point, 1, &ret_index[0], &out_dist_sqr[0]);
                closest.push_back(points_pos.at(ret_index[0]));
            }
            closest_points.push_back(closest);
        }
        return closest_points;
    }

    void interpolate(const unsigned char color1[], const unsigned char color2[], float fraction, unsigned char (&color)[3])
    {
        unsigned char r1 = color1[0];
        unsigned char r2 = color2[0];
        unsigned char g1 = color1[1];
        unsigned char g2 = color2[1];
        unsigned char b1 = color1[2];
        unsigned char b2 = color2[2];
        color[0] = (r2 - r1) * fraction + r1;
        color[1] = (g2 - g1) * fraction + g1;
        color[2] = (b2 - b1) * fraction + b1;
    }


    std::vector<std::string> globVector(const std::string& pattern){
        glob_t glob_result;
        glob(pattern.c_str(),GLOB_TILDE,nullptr, &glob_result);
        std::vector<std::string> files;
        for(unsigned int i = 0; i < glob_result.gl_pathc;++i)
            files.push_back(std::string(glob_result.gl_pathv[i]));

        globfree(&glob_result);
        return files;
    }
}
