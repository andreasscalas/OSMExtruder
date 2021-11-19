
#include <TriangleMesh.h>
#include <shapefil.h>

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

namespace Utilities {

//    std::ofstream file("prova.txt");

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

    /**
     * @brief load_shapefile_shp reader di shapefile (per gentile concessione di Daniela)
     * @param filename il nome (path + nome) dello shapefile
     * @param boundaries vettore che fa da contenitore ai poligoni nello shapefile
     * @return codice d'errore
     */
    int load_shapefile_shp(const std::string filename, std::vector<std::vector<Point*>> &boundaries)
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

            boundaries.push_back(std::vector<Point*> ());

            for (int j = 0; j < (obj->nVertices - 1); ++j)
            {
                Point* point = new Point(obj->padfX[j], obj->padfY[j], obj->padfZ[j]);
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
//    int load_xyz_file(const std::string filename, std::vector<Point*> &points)
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


    std::vector<Point*>::iterator findPointInList(std::vector<Point*>::iterator begin, std::vector<Point*>::iterator end, Point* p)
    {
        for(std::vector<Point*>::iterator it = begin; it != end; it++)
            if(**it == *p)
                return it;
        return end;
    }

    /**
     * @brief fix_polygon pulisce un poligono da vertici doppi e spikes.
     * @param boundary il poligono
     */
    void fix_polygon(std::vector<Point*> &boundary)
    {
        for (unsigned int i = 0; i < boundary.size() - 1; i++) {

            while(i < boundary.size() - 1 && (*boundary[i]) == (*boundary[i + 1]))
            {
                Point* tmp = boundary.at(i + 1);
                boundary.erase(boundary.begin() + i + 1);
                delete tmp;
            }

            std::vector<Point*>::iterator it = findPointInList(boundary.begin() + i + 1, boundary.end(), boundary[i]);
            while(it != boundary.end())
            {
                for(std::vector<Point*>::iterator it1 = boundary.begin() + i + 1; it1 <= it; it1++)
                    delete *it1;
                boundary.erase(boundary.begin() + i + 1, it + 1);
                it = findPointInList(boundary.begin() + i + 1, boundary.end(), boundary[i]);
            }
            if(i > 0)
            {
                if((*boundary[i]) == (*boundary[0]))
                {
                    for(unsigned int j = i + 1; j < boundary.size(); j++)
                        delete boundary.at(j);
                    boundary.erase(boundary.begin() + i + 1, boundary.end());
                }
            }

        }


    }

    void fix_line(std::vector<Point*> &line)
    {
        for (int i = 0; i < static_cast<int>(line.size()) - 1; i++) {

            while(i < line.size() - 1 && (*line[i]) == (*line[i + 1]))
            {
                Point* tmp = line.at(i + 1);
                line.erase(line.begin() + i + 1);
                delete tmp;
            }

            std::vector<Point*>::iterator it = findPointInList(line.begin() + i + 1, line.end(), line[i]);
            while(it != line.end())
            {
                for(std::vector<Point*>::iterator it1 = line.begin() + i + 1; it1 <= it; it1++)
                    delete *it1;
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
    bool segmentsIntersection(Point* p1, Point* p2, Point* p3, Point* p4, Point &pout)
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

        bool result = isPointInSegment(p1, p2, &pout) && isPointInSegment(p3, p4, &pout);
        return result;

    }

    /**
     * @brief polygonsIntersect metodo per il check di intersezione tra poligoni
     * @param polygon1 poligono numero 1
     * @param polygon2 poligono numero 2
     * @return true se i poligoni si intersecano, false altrimenti
     */
    bool polygonsIntersect(std::vector<Point*> polygon1, std::vector<Point*> polygon2)
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
    std::vector<Point*> bbExtraction(std::vector<Point*> points)
    {
        std::vector<Point*> bb;
        bb.push_back(new Point(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 0));
        bb.push_back(new Point(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 0));
        bb.push_back(new Point(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), 0));
        bb.push_back(new Point(std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), 0));

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
    void mergeTouchingPolygons(std::vector<std::vector<Point*> > &boundaries)
    {
        using namespace boost::geometry;

        typedef model::d2::point_xy<double> point_2d;
        typedef model::polygon<point_2d> polygon_2d;
        typedef model::box<point_2d> box_2d;
        for(int i = 0; i < boundaries.size(); i++)
        {
            std::vector<Point*> bb1 = bbExtraction(boundaries[i]);
            for(int j = 0; j < boundaries.size(); j++)
            {
                if(i == j)
                    continue;
                std::vector<Point*> bb2 = bbExtraction(boundaries[j]);
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
                            boundaries.at(i).push_back(new Point(tmp.x(), tmp.y(), 0));
                        }

                        Point* tmp = boundaries.at(i).at(j);
                        boundaries.at(i).erase(boundaries.at(i).begin() + j);
                        delete tmp;
                        if(i > j) i--;
                        break;

                    }

                }
                for(unsigned int k = 0; k < 4; k++)
                    delete bb2[k];
            }

            for(unsigned int j = 0; j < 4; j++)
                delete bb1[j];
        }
    }

    void fix_polygons(std::vector<std::vector<Point*> > &boundaries)
    {

        for (unsigned int i = 0; i < boundaries.size(); i++) {
            //First run: remove repeated vertices (successive pairs)

            fix_polygon(boundaries[i]);
            if(boundaries[i].size() < 4){
                for(unsigned int j = 0; j < boundaries[i].size() - 1; j++)
                    delete boundaries.at(i).at(j);
                boundaries.erase(boundaries.begin() + i);
                i--;
            }

            /*for (unsigned int j = 0; i < boundaries[i].size() - 1; j++)
                while(boundaries[i].size() > j && boundaries[i][j] == boundaries[i][j + 1])
                {
                    std::vector<Point3D>::iterator it = boundaries[i].begin() + j + 1;
                    boundaries[i].erase(it);
                }
            std::stringstream wkt;

            wkt << "POLYGON((";
            for(unsigned int j = 0; j < boundaries[i].size() - 1; j++)
            {
                wkt << boundaries[i][j].getX() << " "  << boundaries[i][j].getY();
                if(j < boundaries[i].size() - 2)
                    wkt << ",";
                else
                    wkt << "))";

            }
            OGRGeometry *geometry;
            std::string input = wkt.str();
            const char* inputWKT = input.c_str();
            OGRErr err = OGRGeometryFactory::createFromWkt(&inputWKT, NULL, &geometry);
            PolygonRepair prepair;
            if (err != OGRERR_NONE) {
                switch (err) {
                  case OGRERR_UNSUPPORTED_GEOMETRY_TYPE:
                    std::cerr << "Error: geometry must be Polygon or MultiPolygon" << std::endl;
                    break;
                  case OGRERR_NOT_ENOUGH_DATA:
                  case OGRERR_CORRUPT_DATA:
                    std::cerr << "Error: corrupted input" << std::endl;
                    break;
                  default:
                    std::cerr << "Error: corrupted input" << std::endl;
                    break;
                }
                return;
            }
            if (geometry->IsEmpty() == 1) {
                std::cerr << "Error: empty geometry" << std::endl;
                return;
            }
            if ( (geometry->getGeometryType() != wkbPolygon) &&
                   (geometry->getGeometryType() != wkbMultiPolygon) ) {
                std::cerr << "Error: geometry must be Polygon or MultiPolygon" << std::endl;
                return;
            }

            boundaries.erase(boundaries.begin() + i);
            OGRMultiPolygon *outPolygons;

            time_t startTime = time(NULL);
            startTime = 0;
            outPolygons = prepair.repairOddEven(geometry, startTime);
            for (unsigned int j = 0; j < outPolygons->getNumGeometries(); ++j)
            {
                OGRPolygon* ogr_polygon;
                ogr_polygon = static_cast<OGRPolygon*>(outPolygons->getGeometryRef(j));
                OGRLinearRing* boundary = ogr_polygon->getExteriorRing();
                std::vector<Point3D> newBoundary;
                for(unsigned int k = 0; k < boundary->getNumPoints(); k++)
                {
                    Point3D p;
                    p.setX(boundary->getX(k));
                    p.setY(boundary->getY(k));
                    p.setZ(boundary->getZ(k));
                    newBoundary.push_back(p);
                }
                if(newBoundary.size() < 4)
                    continue;
                boundaries.insert(boundaries.begin() + i, newBoundary);
                i++;
            }
            i--;*/


        }
    }

    void fix_lines(std::vector<std::vector<Point*> > &boundaries)
    {

        for (unsigned int i = 0; i < boundaries.size(); i++) {
            //First run: remove repeated vertices (successive pairs)

            fix_line(boundaries[i]);

            if(boundaries.at(i).size() < 2)
            {
                for(unsigned int j = 0; j < boundaries.at(i).size(); j++)
                    delete boundaries.at(i).at(j);
                boundaries.erase(boundaries.begin() + i);
                i--;
            }
        }
    }

    bool isPointInsidePolygon(Point* v, std::vector<Point*> boundary){

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

    bool isPolygonInsidePolygon(std::vector<Point*> boundary1, std::vector<Point*> boundary2){

        return isPointInsidePolygon(boundary1[0], boundary2);
    }


    /**
     * @brief extractPolygonsHeights1 assolutamente da ottimizzare
     * @param boundaries
     * @param dhm_tiff
     * @return
     */
    std::vector<std::vector<std::pair<unsigned int, unsigned int> > > extractPolygonsHeights(std::vector<std::vector<Point*> > boundaries, GeoTiff* dhm_tiff, double scale_factor, Point origin)
    {
        std::vector<std::vector<std::pair<unsigned int, unsigned int> > > heights;
        unsigned int counter = 0;
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

        #pragma omp parallel for num_threads(31)
        for(unsigned int i = 0; i < boundaries.size(); i++)
        {
            std::vector<Point*> bb = bbExtraction(boundaries[i]);
            for(unsigned int j = 0; j < dhm_tiff->GetDimensions()[0]; j++)
                for(unsigned int k = 0; k < dhm_tiff->GetDimensions()[1]; k++)
                {
                    double min_x = geoTransform[0] + k * geoTransform[1] + j * geoTransform[2];
                    double min_y = geoTransform[3] + k * geoTransform[4] + j * geoTransform[5];
                    std::vector<Point*> frame;
                    frame.push_back(new Point(min_x, min_y, 0));
                    frame.push_back(new Point(min_x + geoTransform[1], min_y, 0));
                    frame.push_back(new Point(frame.back()->getX(), min_y + geoTransform[5], 0));
                    frame.push_back(new Point(min_x, frame.back()->getY(), 0));
                    frame.push_back(frame[0]);
                    if(((*frame[0]) - (*boundaries[i][0])).norm() < std::max(((*bb[0]) - (*bb[2])).norm(), ((*frame[0]) - (*frame[2])).norm()))
                    {
                        if(isPointInsidePolygon(boundaries[i][0], frame) ||                                         //Se un punto del poligono è dentro al pixel del geotiff...
                           (isPointInsidePolygon(frame[0], bb) && isPointInsidePolygon(frame[0], boundaries[i])) || //o il pixel è contenuto nel poligono...
                           polygonsIntersect(frame, boundaries[i]))                                                 //o ancora poligono e pixel si intersecano
                        {
                            heights[i].push_back(std::make_pair(j, k));
                        }
                    }

                    for(unsigned int l = 0; l < 4; l++)
                        delete frame.at(l);


                }
            if(heights[i].size() == 0)
            {
                std::cerr << "Impossible case: boundary does not include any pixel while no pixel wholly include the boundary and boundary intersects no pixel";
                for(unsigned int j = 0; j < boundaries[i].size(); j++)
                    boundaries[i][j]->print(std::cerr);
                exit(6);
            }
            #pragma omp critical
            {
                counter++;
                std::cout << counter * 100 / boundaries.size() << "%\r" << std::flush;
            }

        }
        std::cout << std::endl;

        geoTransform[0] = (geoTransform[0] * scale_factor + origin.getX());  //x del punto in alto a sinistra
        geoTransform[3] = (geoTransform[3] * scale_factor + origin.getY());  //y del punto in alto a sinistra
        geoTransform[1] *= scale_factor;
        geoTransform[5] *= scale_factor;
        return heights;
    }

    /**
     * @brief extractPolygonsHeights1 assolutamente da ottimizzare
     * @param boundaries
     * @param dhm_tiff
     * @return
     */
    std::vector<std::vector<std::pair<unsigned int, unsigned int> > > extractPolygonsHeights1(std::vector<std::vector<Point*> > boundaries, GeoTiff* dhm_tiff, double scale_factor, Point origin)
    {
        std::vector<std::vector<std::pair<unsigned int, unsigned int> > > heights;
        unsigned int counter = 0;
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
            }
        }
        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
        mat_index->index->buildIndex();

        #pragma omp parallel for num_threads(31)
        for(unsigned int i = 0; i < boundaries.size(); i++)
        {
            std::vector<Point*> bb = bbExtraction(boundaries[i]);
            double sphere_radius = std::max(pixel_diagonal_length, ((*bb[0]) - (*bb[2])).norm()) / 2;
            Point middle = ((*bb[0]) + (*bb[1]) + (*bb[2]) + (*bb[3])) / 4;

            std::vector<std::pair<size_t, double> > neighbors_distances;
            std::vector<std::pair<unsigned int, unsigned int> > neighbors;
            nanoflann::SearchParams params;
            double query_point[3] = {middle.getX(), middle.getY(), 0};
            mat_index->index->radiusSearch(query_point, sphere_radius, neighbors_distances, params);
            for(std::vector<std::pair<size_t,double> >::iterator it = neighbors_distances.begin(); it != neighbors_distances.end(); it++){
                std::pair<size_t, double> p = static_cast<std::pair<long int, double> >(*it);
                neighbors.push_back(std::make_pair(p.first / dhm_tiff->GetDimensions()[0], p.first % dhm_tiff->GetDimensions()[0]));
            }
            for(unsigned int j = 0; j < neighbors.size(); j++)
            {
                double min_x = geoTransform[0] + neighbors.at(j).second * geoTransform[1] + neighbors.at(j).first * geoTransform[2];
                double min_y = geoTransform[3] + neighbors.at(j).second * geoTransform[4] + neighbors.at(j).first * geoTransform[5];
                std::vector<Point*> frame;
                frame.push_back(new Point(min_x, min_y, 0));
                frame.push_back(new Point(min_x + geoTransform[1], min_y, 0));
                frame.push_back(new Point(frame.back()->getX(), min_y + geoTransform[5], 0));
                frame.push_back(new Point(min_x, frame.back()->getY(), 0));
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
                    delete frame.at(l);
            }
            if(heights[i].size() == 0)
            {
                std::cerr << "Impossible case: boundary does not include any pixel while no pixel wholly include the boundary and boundary intersects no pixel";
                for(unsigned int j = 0; j < boundaries[i].size(); j++)
                    boundaries[i][j]->print(std::cerr);
                exit(6);
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



//    std::vector<std::vector<std::pair<unsigned int, unsigned int> > > extractPolygonsHeights3(std::vector<std::vector<Point3D> > boundaries, GeoTiff* dhm_tiff, Point3D origin,  double scale_factor)
//    {
//        std::vector<std::vector<std::pair<unsigned int, unsigned int> > > heights;
//        my_vector_of_vectors_t points_vector;

//        for(unsigned int i = 0; i < boundaries.size(); i++)
//        {
//            std::vector<std::pair<unsigned int, unsigned int> > boundary_heights;
//            boundary_heights.clear();
//            heights.push_back(boundary_heights);
//        }
//        float** dhm_heights = dhm_tiff->GetRasterBand(1);
//        double* geoTransform = dhm_tiff->GetGeoTransform();
//        //Apply scaling to increase the robustness of the method (coordinates of geographic positions are huge).
//        geoTransform[0] = (geoTransform[0] - origin.getX()) / scale_factor;  //x del punto in alto a sinistra
//        geoTransform[3] = (geoTransform[3] - origin.getY()) / scale_factor;  //y del punto in alto a sinistra
//        geoTransform[1] /= scale_factor;
//        geoTransform[5] /= scale_factor;

//        for(unsigned int i = 0; i < dhm_tiff->GetDimensions()[0]; i++)
//        {
//            for(unsigned int j = 0; j < dhm_tiff->GetDimensions()[1]; j++)
//            {
//                std::vector<double> point = { geoTransform[0] + j * geoTransform[1] + i * geoTransform[2],
//                                              geoTransform[3] + j * geoTransform[4] + i * geoTransform[5],
//                                              0};
//                points_vector.push_back(point);
//            }
//        }
//        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
//        mat_index->index->buildIndex();

//        #pragma omp parallel for num_threads(31)
//        for(unsigned int i = 0; i < boundaries.size(); i++)
//        {
//            double min = std::numeric_limits<double>::max();
//            double max = -std::numeric_limits<double>::max();
//            double mean = 0;
//            unsigned int assigned_points_number = 0;
//            std::vector<Point3D> bb = bbExtraction(boundaries[i]);

//            for(unsigned int j = 0; j < dhm_tiff->GetDimensions()[0]; j++)
//            {
//                for(unsigned int k = 0; k < dhm_tiff->GetDimensions()[1]; k++)
//                {
//                    Point3D p(geoTransform[0] + k * geoTransform[1] + j * geoTransform[2],
//                              geoTransform[3] + k * geoTransform[4] + j * geoTransform[5], 0);

//                    if(isPointInsidePolygon(p, bb) && isPointInsidePolygon(p, boundaries[i]))
//                    {
//                        //std::cout << j << " " << k << std::endl;
//                        if(j >= dhm_tiff->GetDimensions()[0] || k >= dhm_tiff->GetDimensions()[1])
//                            std::cerr<< "Point does not exist" << std::endl;
//                        heights[i].push_back(std::make_pair(j,k));
//                    }

//                }
//            }
//            if(heights[i].size() == 0)
//            {
//                std::cout << "Height not found for polygon " << i << std::endl;
//                for(unsigned int j = 0; j < boundaries[i].size() - 1; j++)
//                {
//                    double point[3] = {boundaries[i][j].getX(), boundaries[i][j].getY(), 0};
//                    std::vector<size_t> ret_index(4);
//                    std::vector<double> out_dist_sqr(4);
//                    mat_index->index->knnSearch(point, 4, &ret_index[0], &out_dist_sqr[0]);
//                    unsigned int maxPos = -1;
//                    double max = -std::numeric_limits<double>::max();
//                    for(unsigned int k = 0; k < 4; k++)
//                    {
//                        unsigned int row = ret_index[k] / dhm_tiff->GetDimensions()[1];
//                        unsigned int col = ret_index[k] % dhm_tiff->GetDimensions()[1];
//                        if(dhm_heights[row][col] > max)
//                        {
//                            maxPos = k;
//                            max = dhm_heights[row][col];
//                        }
//                    }
//                    if(ret_index[0] > dhm_tiff->GetDimensions()[0] * dhm_tiff->GetDimensions()[1] ||
//                       ret_index[1] > dhm_tiff->GetDimensions()[0] * dhm_tiff->GetDimensions()[1] ||
//                       ret_index[2] > dhm_tiff->GetDimensions()[0] * dhm_tiff->GetDimensions()[1] ||
//                       ret_index[3] > dhm_tiff->GetDimensions()[0] * dhm_tiff->GetDimensions()[1])
//                    {
//                        std::cerr << "Impossible case. Knn search returns some out-of-bounds index. Maybe there are less than 4 points?" << std::endl
//                                  << "The knn search return indices: " << std::endl;
//                        std::cerr << ret_index[0] << std::endl;
//                        std::cerr << ret_index[1] << std::endl;
//                        std::cerr << ret_index[2] << std::endl;
//                        std::cerr << ret_index[3] << std::endl;
//                        exit(6);
//                    }
//                    if(maxPos > 3 || maxPos < 0)
//                    {
//                        std::cerr << "Impossible case. There is no neighbour with height greater than -infinite." << std::endl;
//                        exit(7);
//                    }
//                    unsigned int row = ret_index[maxPos] / dhm_tiff->GetDimensions()[0];
//                    unsigned int col = ret_index[maxPos] % dhm_tiff->GetDimensions()[0];

//                    auto p = std::make_pair(row, col);
//                    std::vector<std::pair<unsigned int, unsigned int> >::iterator it = std::find(heights[i].begin(), heights[i].end(), p);
//                    heights[i].push_back(std::make_pair(row,col));

//                }
//            }



//        }

//        delete mat_index;
//        geoTransform[0] = (geoTransform[0] * scale_factor + origin.getX());  //x del punto in alto a sinistra
//        geoTransform[3] = (geoTransform[3] * scale_factor + origin.getY());  //y del punto in alto a sinistra
//        geoTransform[1] *= scale_factor;
//        geoTransform[5] *= scale_factor;

//        return heights;
//    }

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

    void refineLines(std::vector<std::vector<Point*> > &lines, GeoTiff* dhm_tiff, double scale_factor, Point origin)
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
                std::vector<Point*> intersections;
                Point* v1 = lines.at(i).at(j - 1);
                Point* v2 = lines.at(i).at(j);
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
                    std::vector<Point*> frame;
                    frame.push_back(new Point(min_x, min_y, 0));
                    frame.push_back(new Point(min_x + geoTransform[1], min_y, 0));
                    frame.push_back(new Point(frame.back()->getX(), min_y + geoTransform[5], 0));
                    frame.push_back(new Point(min_x, frame.back()->getY(), 0));
                    frame.push_back(frame[0]);

                    for(unsigned int l = 1; l < 5; l++)
                    {
                        Point* pout = new Point;
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
                                delete pout;
                        } else if(pout != nullptr)
                            delete pout;
                    }

                    for(unsigned int l = 1; l < 4; l++)
                        delete frame.at(l);
                }
                neighbors.clear();
                neighbors_distances.clear();
                std::sort(intersections.begin(), intersections.end(), [v1](Point* a, Point* b) {
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
                std::cout << counter * 100 / dhm_tiff->GetDimensions()[0] << "%\r" << std::flush;
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

    Vertex* extractNearestVertex(std::vector<Vertex*> &frontier, std::map<Vertex*, double> distances){

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

        Vertex* nearest = frontier[minPos];

        frontier.erase(frontier.begin() + minPos);

        return nearest;

    }

    std::vector<Vertex*> dijkstra(Vertex* v1, Vertex* v2){

        std::vector<Vertex*> frontier;
        std::map<Vertex*, double> distances = {{v1, 0}};
        std::map<Vertex*, Vertex*> predecessors = {{v1, nullptr}};
        std::set<Vertex*> v21RingNeighbors;
        std::vector<Vertex*> shortestPath;
        Vertex* v;
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

            std::vector<Vertex*> neighbors = v->getVV();
            for(std::vector<Vertex*>::iterator n = neighbors.begin(); n != neighbors.end(); n++){
                Vertex* x = *n;

                if(x == v2 && v21RingNeighbors.find(v) == v21RingNeighbors.end())
                    v21RingNeighbors.insert(v);

                std::map<Vertex*, Vertex*>::iterator pit = predecessors.find(x);
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
            if(v == v2)
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

    void findFaces(std::vector<Triangle*> &faces, std::vector<Vertex*> vertices){

        //For each vertex, we find the triangle ring and we insert in the array all the faces that aren't in it
        for(unsigned long i = 0; i < vertices.size(); i++){

            std::vector<Triangle*> vt = vertices[i]->getVT();
            for(unsigned int j = 0; j < vt.size(); j++){
                Triangle* t = vt.at(j);

                std::vector<Triangle*>::iterator it = find(faces.begin(), faces.end(), t);
                if(it == faces.end())
                    faces.push_back(t);
            }
        }
    }

    Triangle* findCorrespondingTriangle(Point* v, std::vector<Triangle*> neighbors){

        bool found = false;
        double bestDistance = DBL_MAX;
        Triangle* t = nullptr;
        for(std::vector<Triangle*>::iterator it = neighbors.begin(); it != neighbors.end(); it++){
            Triangle* t_ = static_cast<Triangle*>(*it) ;
            Vertex* tv1 = t_->getV1(), *tv2 = t_->getV2(), *tv3 = t_->getV3();
            Point p = linePlaneIntersection(*v, (*v) - t_->computeNormal(), *tv1, *tv2, *tv3);
            if(t_->isPointInside(p)){
                double actualDistance = t_->computeDistanceFromPoint(*v);
                if(!found){
                    t = t_;
                    bestDistance = actualDistance;
                    found = true;
                } else if(actualDistance < bestDistance){
                    t = t_;
                    bestDistance = actualDistance;
                }
            }

        }

        return t;
    }

    Vertex* findCorrespondingVertex(Vertex* v, std::vector<Triangle*> neighbors){

        double bestDistance = DBL_MAX;
        Vertex* r = nullptr;
        Triangle* t = findCorrespondingTriangle(v, neighbors);
        if(t != nullptr){
            Vertex* tv1 = t->getV1(), *tv2 = t->getV2(), *tv3 = t->getV3();
            Point p = linePlaneIntersection(*v, (*v) + v->computeNormal(), *tv1, *tv2, *tv3);
            Vertex* v_ = t->getV1();

            for(int i = 0; i < 3; i++){
                double actualDistance = ((p)-(*v_)).norm();
                if(actualDistance < bestDistance){
                    bestDistance = actualDistance;
                    r = v_;
                }
                v_ = t->getNextVertex(v_);
            }

        }
        return r;

    }

    std::vector<std::vector<Vertex*> > projectLines(std::vector<std::vector<Point*> > polyLines, TriangleMesh *otherMesh, double neighbourhoodSize)
    {

            Vertex* v, *initialVertex;                      //Some support variable
            std::vector<std::vector<Vertex*> > otherPolyLines;

            my_vector_of_vectors_t points_vector;
            for(unsigned int i = 0; i < otherMesh->getVerticesNumber(); i++)
            {
                std::vector<double> point = { otherMesh->getVertex(i)->getX(),
                                              otherMesh->getVertex(i)->getY(),
                                              otherMesh->getVertex(i)->getZ()};
                points_vector.push_back(point);
            }

            my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
            mat_index->index->buildIndex();

            for(std::vector<std::vector<Point*> >::iterator lit = polyLines.begin(); lit != polyLines.end(); lit++){

                std::vector<Vertex*> otherPolyLine;
                std::vector<Point*> polyLine = *lit;
                std::vector<Point*>::iterator vit = polyLine.begin();
                Vertex* v1, *v2;

                do{
                    v = static_cast<Vertex*>(*vit);
                    std::vector<std::pair<size_t, double> > neighbors_distances;
                    std::vector<Vertex*> neighbors;
                    nanoflann::SearchParams params;
                    double query_point[3] = {v->getX(), v->getY(), v->getZ()};
                    mat_index->index->radiusSearch(query_point, neighbourhoodSize, neighbors_distances, params);
                    for(std::vector<std::pair<size_t,double> >::iterator it = neighbors_distances.begin(); it != neighbors_distances.end(); it++){
                        std::pair<size_t, double> p = static_cast<std::pair<long int, double> >(*it);
                        neighbors.push_back(otherMesh->getVertex(p.first));
                    }
                    std::vector<Triangle*> toCheckTriangles;
                    Utilities::findFaces(toCheckTriangles, neighbors);
                    v1 = Utilities::findCorrespondingVertex(v, toCheckTriangles);
                    if(v1 == nullptr)
                        neighbourhoodSize *= 2;
                }while(v1 == nullptr);
                initialVertex = v1;

                for(; vit != polyLine.end(); vit++){
                    do{
                        v = static_cast<Vertex*>(*vit);
                        std::vector<std::pair<size_t, double> > neighbors_distances;
                        std::vector<Vertex*> neighbors;
                        nanoflann::SearchParams params;
                        double query_point[3] = {v->getX(), v->getY(), v->getZ()};
                        mat_index->index->radiusSearch(query_point, neighbourhoodSize, neighbors_distances, params);
                        for(std::vector<std::pair<size_t,double> >::iterator it = neighbors_distances.begin(); it != neighbors_distances.end(); it++){
                            std::pair<size_t, double> p = static_cast<std::pair<long int, double> >(*it);
                            neighbors.push_back(otherMesh->getVertex(p.first));
                        }
                        std::vector<Triangle*> toCheckTriangles;
                        Utilities::findFaces(toCheckTriangles, neighbors);
                        v2 = Utilities::findCorrespondingVertex(v, toCheckTriangles);
                        if(v2 == nullptr)
                            neighbourhoodSize *= 2;
                    }while(v2 == nullptr);
                    std::vector<Vertex*> path = dijkstra(v1, v2);
                    otherPolyLine.insert(otherPolyLine.end(), path.begin(), path.end());
                    v1 = v2;
                    otherPolyLines.push_back(otherPolyLine);
                }

            }

            return otherPolyLines;
    }

    void removeLinePointsInPolygons(std::vector<std::vector<Point*> > &lines, std::vector<std::vector<Point*> > polygons)
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
            std::vector<Point*> bb = bbExtraction(polygons[i]);
            //double diagMeasure = (*bb[2] - *bb[0]).norm();
            meanDiagonal += (*bb[2] - *bb[0]).norm();/*
            if(diagMeasure > meanDiagonal)
                meanDiagonal = diagMeasure;*/
        }
        meanDiagonal /= polygons.size();

        my_kd_tree_t* mat_index = new my_kd_tree_t(3, points_vector, 10 /* max leaf */ );
        mat_index->index->buildIndex();
        unsigned int counter = 0;

        #pragma omp parallel for num_threads(31)
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            for(unsigned int j = 0; j < lines[i].size(); j++){
                Point* v = lines.at(i).at(j);
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
                    if(isPointInsidePolygon(v, polygons.at(neighboringPolygons.at(k))))
                    {
                        lines.at(i).erase(lines.at(i).begin() + j);
                        delete v;
                        j--;
                        break;
                    }
            }

            #pragma omp critical
            {
                counter++;
                std::cout << counter * 100 / lines.size() << "%\r" << std::flush;
            }
        }
        for(unsigned int i = 0; i < lines.size(); i++)
            if(lines.at(i).size() == 0)
                lines.erase(lines.begin() + i);
        std::cout << std::endl;

    }
}
