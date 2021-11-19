#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <Point.h>
#include <utilities.h>

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPropAssembly.h>
#include <vtkLine.h>
#include <vtkTriangle.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleJoystickCamera.h>
#include <vtkInteractorStyle3D.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>

using namespace std;
const double OFFSET = 25e-7;


int main(int argc, char *argv[])
{

    if(argc < 2)
    {
        std::cerr << "Missing buildings shapefile parameter" << std::endl;
        return 1;
    }
    TriangleMesh* mesh = new TriangleMesh();
    int retValue = mesh->load(argv[1]);
    if(retValue == 0)
    {
        vtkSmartPointer<vtkPoints> meshPoints = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> meshEdges = vtkSmartPointer<vtkCellArray>::New();
        vtkSmartPointer<vtkCellArray> meshTriangles = vtkSmartPointer<vtkCellArray>::New();
        vtkSmartPointer<vtkUnsignedCharArray> meshVColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        meshVColors->SetNumberOfComponents(3);
        meshVColors->SetName("PColors");
        vtkSmartPointer<vtkUnsignedCharArray> meshEColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        meshEColors->SetNumberOfComponents(3);
        meshEColors->SetName("LColors");
        vtkSmartPointer<vtkUnsignedCharArray> meshTColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        meshTColors->SetNumberOfComponents(3);
        meshTColors->SetName ("TColors");
        vtkSmartPointer<vtkActor> meshSurfaceActor = vtkSmartPointer<vtkActor>::New();
        vtkSmartPointer<vtkActor> meshPointsActor = vtkSmartPointer<vtkActor>::New();
        vtkSmartPointer<vtkActor> meshWireframeActor = vtkSmartPointer<vtkActor>::New();
        vtkSmartPointer<vtkPropAssembly> canvas = vtkSmartPointer<vtkPropAssembly>::New();

        //std::vector<Annotation*> annotations;

        const unsigned char RED[3] =    {255,0,0};
        const unsigned char GREEN[3] =  {0,255,0};
        const unsigned char GRAY[3] =   {220,220,220};
        const unsigned char BLUE[3] =   {0,0,255};
        const unsigned char BLACK[3] =  {0,0,0};
        double minEdgeLength = std::numeric_limits<double>::max();
        for(unsigned int i = 0; i < mesh->getVerticesNumber(); i++){
            Vertex* v = mesh->getVertex(i);
            meshPoints->InsertNextPoint(v->getX(), v->getY(), v->getZ());
            meshVColors->InsertNextTypedTuple(BLUE);
        }


        for(unsigned int i = 0; i < mesh->getEdgesNumber(); i++){
            Edge* e = mesh->getEdge(i);
            vtkSmartPointer<vtkLine> edge = vtkSmartPointer<vtkLine>::New();
            edge->GetPointIds()->SetNumberOfIds(2);
            edge->GetPointIds()->SetId(0, static_cast<vtkIdType>(e->getV1()->getId()));
            edge->GetPointIds()->SetId(1, static_cast<vtkIdType>(e->getV2()->getId()));
            meshEdges->InsertNextCell(edge);
            meshEColors->InsertNextTypedTuple(BLACK);
            double length = e->computeLength();
            if(length < minEdgeLength)
                minEdgeLength = length;
        }


        for(unsigned int i = 0; i < mesh->getTrianglesNumber(); i++){
            Triangle* t = mesh->getTriangle(i);
            Vertex *v1, *v2, *v3;
            v1 = t->getV1();
            v2 = t->getV2();
            v3 = t->getV3();
            vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
            triangle->GetPointIds()->SetNumberOfIds(3);
            triangle->GetPointIds()->SetId(0, static_cast<vtkIdType>(v1->getId()));
            triangle->GetPointIds()->SetId(1, static_cast<vtkIdType>(v2->getId()));
            triangle->GetPointIds()->SetId(2, static_cast<vtkIdType>(v3->getId()));
            meshTriangles->InsertNextCell(triangle);
            meshTColors->InsertNextTypedTuple(GRAY);
        }

        canvas = vtkSmartPointer<vtkPropAssembly>::New();


        vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        vtkMesh->SetPoints(meshPoints);
        vtkMesh->SetPolys(meshTriangles);
        vtkMesh->GetCellData()->SetScalars(meshTColors);
        mapper->SetInputData(vtkMesh);
        meshSurfaceActor = vtkSmartPointer<vtkActor>::New();
        meshSurfaceActor->SetMapper(mapper);
        meshSurfaceActor->GetProperty()->SetRepresentationToSurface();
        canvas->AddPart(meshSurfaceActor);

        /*if(drawAnnotations)
            for (unsigned int i = 0; i < annotations.size(); i++)
                dynamic_cast<DrawableAnnotation*>(annotations[i])->draw(canvas);*/


        vtkSmartPointer<vtkPolyData> wireframePolydata = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkPolyDataMapper> wireframeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        wireframePolydata->SetPoints(meshPoints);
        wireframePolydata->SetLines(meshEdges);
        wireframePolydata->GetCellData()->SetScalars(meshEColors);
        wireframeMapper->SetInputData(wireframePolydata);
        meshWireframeActor = vtkSmartPointer<vtkActor>::New();
        meshWireframeActor->SetMapper(wireframeMapper);
        meshWireframeActor->GetProperty()->SetRepresentationToWireframe();


        meshWireframeActor->GetProperty()->SetOpacity(1.0);
        meshWireframeActor->GetProperty()->SetLineWidth(3.0);

        canvas->AddPart(meshWireframeActor);


        vtkSmartPointer<vtkPolyData> pointPolydata = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkPolyDataMapper> pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        meshPointsActor = vtkSmartPointer<vtkActor>::New();
        pointPolydata->SetPoints(meshPoints);
        pointPolydata->SetPolys(meshTriangles);
        pointPolydata->GetPointData()->SetScalars(meshVColors);
        pointMapper->SetInputData(pointPolydata);
        meshPointsActor->SetMapper(pointMapper);
        meshPointsActor->GetProperty()->SetPointSize(5.0f);
        meshPointsActor->GetProperty()->SetRepresentationToPoints();


        canvas->Modified();

        vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
        vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

        renderer->SetBackground(255,255,255);
        renderer->AddActor(canvas);
        renderWindow->AddRenderer(renderer);
        renderWindow->SetSize(640, 480);
        renderWindow->SetWindowName("CityViewer");
        renderWindowInteractor->SetRenderWindow(renderWindow);
        renderWindowInteractor->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
        renderWindow->Render();
        renderWindowInteractor->Start();
    } else
        std::cerr << "Program terminated with return value " <<  retValue << std::endl;


    delete mesh;
    return 0;

}
