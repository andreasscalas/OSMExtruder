#include "trianglehelper.h"

#ifndef ANSI_DECLARATORS
#define ANSI_DECLARATORS
#endif
#ifndef TRILIBRARY
#define TRILIBRARY
#endif
#ifndef REAL
#define REAL double
#endif
#ifndef VOID
#define VOID void
#endif
#include TRIANGLE_HEADER

#include <map>
using namespace std;
using namespace TriHelper;

TriangleHelper::TriangleHelper(std::vector<Point3D> boundary, std::vector<std::vector<Point3D> > holes){

    this->boundary.insert(this->boundary.end(), boundary.begin(), boundary.end());
    this->holes.insert(this->holes.end(), holes.begin(), holes.end());

    launchTriangle();
}

const std::vector<unsigned int> &TriangleHelper::getTriangles() const
{
    return triangles;
}

void TriangleHelper::launchTriangle(){

    triangulateio in, out;

    vector<pair<unsigned int, unsigned int> > segments;

    int numberOfPoints = static_cast<int>(boundary.size());

    for(unsigned int i = 0; i < holes.size(); i++)
        numberOfPoints += holes[i].size();

    in.numberofpoints = numberOfPoints;
    in.pointlist      = static_cast<double*>(calloc(static_cast<size_t>(2 * in.numberofpoints), sizeof(double)));
    in.numberofpointattributes = 0;
    in.pointmarkerlist  = static_cast<int*>(calloc(static_cast<size_t>(in.numberofpoints), sizeof(int)));
    for(int vid = 0; vid < in.numberofpoints; vid++)
       in.pointmarkerlist[vid] = 1;

    unsigned int reachedID;
    for(unsigned int vid = 0; vid < static_cast<unsigned int>(boundary.size()); vid++){

        in.pointlist[vid * 2  ]     = boundary[vid].getX();
        in.pointlist[vid * 2 + 1]   = boundary[vid].getY();
        if(vid > 0)
            segments.push_back(make_pair(vid - 1, vid));
        reachedID = vid;
    }

    segments.push_back(make_pair(reachedID, 0));
    reachedID++;
    for(vector<vector<Point3D> >::iterator bit = holes.begin(); bit != holes.end(); bit++){
        unsigned int vid = reachedID;
        vector<Point3D> outline = (*bit);
        for(vector<Point3D>::iterator vit = outline.begin(); vit != outline.end(); vit++){
            in.pointlist[vid * 2  ]     = (*vit).getX();
            in.pointlist[vid * 2 + 1]   = (*vit).getY();
            if(vid > reachedID)
                segments.push_back(make_pair(vid - 1, vid));

            vid++;
        }
        segments.push_back(make_pair(vid - 1, reachedID));
        reachedID = vid;
    }

    in.numberofsegments = static_cast<int>(segments.size());
    in.segmentlist      = static_cast<int*>(calloc(static_cast<size_t>(2 * in.numberofsegments), sizeof(int)));

    for(unsigned int i = 0; i < static_cast<unsigned int>(2 * in.numberofsegments); i += 2){
        in.segmentlist[i] = static_cast<int>(segments[i / 2].first);
        in.segmentlist[i + 1] = static_cast<int>(segments[i / 2].second);
    }

    in.segmentmarkerlist = nullptr;

    in.numberofholes = static_cast<int>(holes.size());
    in.holelist      = static_cast<double*>(calloc(static_cast<size_t>(2 * in.numberofholes), sizeof(double)));

    for(unsigned int hid = 0; hid < static_cast<unsigned int>(in.numberofholes); ++hid){

        vector<Point3D> outline = holes[hid];
        Point3D v = (outline[1] - outline[0]) * 1E-3;
        double vec[2] = {-v.getY(), v.getX()};
        Point3D middle = (outline[1] + outline[0]) / 2;
        double innerPoint[2] = {vec[0] + middle.getX(), vec[1] + middle.getY()};
        double turningSign =    (outline[1].getY() - outline[0].getY()) * (innerPoint[0] - outline[0].getX()) -
                                (outline[1].getX() - outline[0].getX()) * (innerPoint[1] - outline[0].getY());
        if(turningSign < 0){
            innerPoint[0] = -vec[0] + middle.getX();
            innerPoint[1] = -vec[1] + middle.getY();
        }

        in.holelist[hid * 2  ]      = innerPoint[0];
        in.holelist[hid * 2 + 1]    = innerPoint[1];
    }

    in.numberoftriangles          = 0;
    in.numberofcorners            = 0;
    in.numberoftriangleattributes = 0;
    in.trianglelist               = nullptr;
    in.triangleattributelist      = nullptr;
    in.numberofregions = 0;

    out.pointlist      = nullptr;
    out.trianglelist   = nullptr;
    out.segmentlist    = nullptr;

    std::string s = "pzNPQ";

    triangulate(const_cast<char*>(s.c_str()), &in, &out, nullptr);


    for(int tid = 0; tid < out.numberoftriangles; tid++){
        triangles.push_back(out.trianglelist[tid * 3]);
        triangles.push_back(out.trianglelist[tid * 3 + 1]);
        triangles.push_back(out.trianglelist[tid * 3 + 2]);
    }
    free(in.pointlist);
    free(in.pointmarkerlist);
    free(in.segmentlist);
    free(in.holelist);
    free(out.pointlist);
    free(out.trianglelist);
    free(out.segmentlist);

}

#undef ANSI_DECLARATORS
#undef REAL
#undef VOID
#undef TRILIBRARY
