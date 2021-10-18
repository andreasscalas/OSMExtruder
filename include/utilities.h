#include <shapefil.h>

#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stack>
#include <map>
#include <boost/geometry.hpp>
#include <fstream>
#include <Point3D.h>

namespace Utilities {

//    std::ofstream file("prova.txt");
    const double EPSILON_DETERMINANT = 1e-10;
    const double EPSILON_CONTAINMENT = 1e-4;
    bool bastardo_trovato = false;

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
    int load_shapefile_shp(const std::string filename, std::vector<std::vector<Point3D>> &boundaries)
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

        if ((nShapeType != SHPT_POLYGON) && (nShapeType != SHPT_POLYGONZ))
        {
            std::cerr << "Error: no polygon in the shapefile " << filename << std::endl;
            return 1;
        }

        for (int i = 0; i < nEntities; ++i)
        {
            SHPObject *obj = SHPReadObject(hSHP, i);

            boundaries.push_back(std::vector<Point3D> ());

            for (int j = 0; j < (obj->nVertices - 1); ++j)
            {
                Point3D point;
                point.setX(obj->padfX[j]);
                point.setY(obj->padfY[j]);
                point.setZ(obj->padfZ[j]);

                boundaries.at(boundaries.size()-1).push_back(point);
            }

            SHPDestroyObject(obj);
        }

        SHPClose(hSHP);

        std::cout << "Loaded " << boundaries.size() << " polygons" << std::endl;

        return 0;
    }

    /**
     * @brief fix_polygon pulisce un poligono da vertici doppi e spikes.
     * @param boundary il poligono
     */
    void fix_polygon(std::vector<Point3D> &boundary)
    {
        for (unsigned int i = 0; i < boundary.size() - 1; i++) {

            while(boundary.size() > i && boundary[i] == boundary[i + 1])
            {
                std::vector<Point3D>::iterator it = boundary.begin() + i + 1;
                boundary.erase(it);
            }

            if(i > 0)
            {
                std::vector<Point3D>::iterator it = std::find(boundary.begin() + i + 1, boundary.end(), boundary[i]);
                while(it != boundary.end())
                {
                    boundary.erase(boundary.begin() + i + 1, it + 1);
                    it = std::find(boundary.begin() + i + 1, boundary.end(), boundary[i]);
                }

                if(boundary[i] == boundary[0])
                    boundary.erase(boundary.begin() + i + 1, boundary.end());
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
    bool isPointInSegment(Point3D a, Point3D b, Point3D p)
    {
        if(p == a || p == b)
            return true;
        double l = (b - a).norm();
        double l1 = (p - a).norm();
        double l2 = (b - p).norm();
        double w1 = l1 / l;
        double w2 = l2 / l;
        return w1 + w2 <= 1.0 + EPSILON_CONTAINMENT;
    }


    inline double det(double a, double b, double c, double d)
    {
        return a * d - b * c;
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
    bool segmentsIntersection(Point3D p1, Point3D p2, Point3D p3, Point3D p4, Point3D &pout)
    {

        double detL1 = det(p1.getX(), p1.getY(), p2.getX(), p2.getY());
        double detL2 = det(p3.getX(), p3.getY(), p4.getX(), p4.getY());
        double x1mx2 = p1.getX() - p2.getX();
        double x3mx4 = p3.getX() - p4.getX();
        double y1my2 = p1.getY() - p2.getY();
        double y3my4 = p3.getY() - p4.getY();

        double xnom = det(detL1, x1mx2, detL2, x3mx4);
        double ynom = det(detL1, y1my2, detL2, y3my4);
        double denom = det(x1mx2, y1my2, x3mx4, y3my4);
        if(abs(denom) < EPSILON_DETERMINANT)//Lines don't seem to cross
        {
            pout.setX(std::numeric_limits<double>::quiet_NaN());
            pout.setY(std::numeric_limits<double>::quiet_NaN());
            pout.setZ(0);
            return false;
        }

        pout.setX(xnom / denom);
        pout.setY(ynom / denom);
        pout.setZ(p1.getZ());
        if(!isfinite(pout.getX()) || !isfinite(pout.getY())) //Probably a numerical issue
            return false;

        return isPointInSegment(p1, p2, pout) && isPointInSegment(p3, p4, pout);

    }

    /**
     * @brief polygonsIntersect metodo per il check di intersezione tra poligoni
     * @param polygon1 poligono numero 1
     * @param polygon2 poligono numero 2
     * @return true se i poligoni si intersecano, false altrimenti
     */
    bool polygonsIntersect(std::vector<Point3D> polygon1, std::vector<Point3D> polygon2)
    {
        for(unsigned int i = 1; i < polygon1.size(); i++)
        {
            for(unsigned int j = 1; j < polygon2.size(); j++)
            {
                Point3D intersection;
                if(segmentsIntersection(polygon1[i - 1], polygon1[i], polygon2[j - 1], polygon2[j], intersection))
                    return true;
            }
        }
        return false;
    }

    /**
     * @brief bbExtraction metodo per estrarre la AABB dai poligoni (riduzione costo computazionale del calcolo di intersezione)
     * @param points il poligono
     * @return array di 4 elementi (5 in realtà, il primo e l'ultimo sono uguali) codificante la AABB. Seguo questa codifica per compatibilità con gli altri poligoni.
     */
    std::vector<Point3D> bbExtraction(std::vector<Point3D> points)
    {
        std::vector<Point3D> bb;
        bb.push_back(Point3D(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 0));
        bb.push_back(Point3D(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 0));
        bb.push_back(Point3D(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), 0));
        bb.push_back(Point3D(std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), 0));

        for(unsigned int j = 0; j < points.size(); j++)
        {

            if(points[j].getX() < bb[0].getX())
            {
                bb[0].setX(points[j].getX());
                bb[3].setX(points[j].getX());
            }
            if(points[j].getX() > bb[1].getX())
            {
                bb[1].setX(points[j].getX());
                bb[2].setX(points[j].getX());
            }
            if(points[j].getY() < bb[0].getY())
            {
                bb[0].setY(points[j].getY());
                bb[1].setY(points[j].getY());
            }
            if(points[j].getY() > bb[2].getY())
            {
                bb[2].setY(points[j].getY());
                bb[3].setY(points[j].getY());
            }
        }
        bb.push_back(bb[0]);

        return bb;
    }

    /**
     * @brief mergeTouchingPolygons metodo per fondere poligoni a contatto. Correntemente non in funzione
     * @param boundaries insieme dei poligoni
     */
    void mergeTouchingPolygons(std::vector<std::vector<Point3D> > &boundaries)
    {
        using namespace boost::geometry;

        typedef model::d2::point_xy<double> point_2d;
        typedef model::polygon<point_2d> polygon_2d;
        typedef model::box<point_2d> box_2d;
        Point3D bastardo(-0.002199, -0.007460, 0.000000);
        for(int i = 0; i < boundaries.size(); i++)
        {
            std::vector<Point3D> bb1 = bbExtraction(boundaries[i]);
            for(int j = 0; j < boundaries.size(); j++)
            {
                if(i == j)
                    continue;
                std::vector<Point3D> bb2 = bbExtraction(boundaries[j]);
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
                            tmp.x(boundaries[i][k].getX());
                            tmp.y(boundaries[i][k].getY());
                            append(first_polygon, tmp);
                        }
                        for(unsigned int k = 0; k < boundaries[j].size(); k++)
                        {
                            point_2d tmp;
                            tmp.x(boundaries[j][k].getX());
                            tmp.y(boundaries[j][k].getY());
                            append(second_polygon, tmp);
                        }
                        std::vector<polygon_2d> output;
                        boost::geometry::union_(first_polygon, second_polygon, output);
                        boundaries[i].clear();
                        for(unsigned int k = 0; k < output[0].outer().size(); k++)
                        {
                            point_2d tmp = output[0].outer()[k];
                            boundaries[i].push_back(Point3D(tmp.x(), tmp.y(), 0));
                        }

                        boundaries.erase(boundaries.begin() + j);
                        if(i > j) i--;
                        break;

                    }

                }
            }
        }
    }

    void fix_polygons(std::vector<std::vector<Point3D> > &boundaries)
    {

        for (unsigned int i = 0; i < boundaries.size(); i++) {
            //First run: remove repeated vertices (successive pairs)

            fix_polygon(boundaries[i]);
            if(boundaries[i].size() < 4)
            {
                boundaries.erase(boundaries.begin() + i);
                i--;
            }
        }
    }


    std::vector<unsigned int*> ele_file_reader(std::string filename)
    {
        std::ifstream ele_file(filename);
        std::vector<unsigned int*> triangles;

        std::string line;
        std::getline(ele_file, line);

        while(std::getline(ele_file, line))
            if(line.find("#") == std::string::npos)
            {
                std::stringstream stream(line);
                unsigned int t_index;
                unsigned int* triangle = new unsigned int[3];
                stream >> t_index;
                stream >> triangle[0];
                stream >> triangle[1];
                stream >> triangle[2];
                triangles.push_back(triangle);
            }

        ele_file.close();

        return triangles;

    }

}
