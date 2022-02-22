/* File: VisualizationWidget.hpp */

// Include from the Poisson Solver Library
#include "../lib/poisson.hpp"

// Includes from the VTK Library
#include <vtkNew.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetReader.h>

#include <vtkActor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkCubeAxesActor.h>

#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkNamedColors.h>

#include <vtkStructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

/*!
 *  @brief Visualization Widget.
 */
class VisualizationWidget : public QVTKOpenGLNativeWidget 
{
    Q_OBJECT

private:
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkCamera> camera;

    vtkNew<vtkDataSetMapper> mapper;
    vtkNew<vtkActor> actor;
    vtkNew<vtkTextActor> textActor;
    vtkNew<vtkCubeAxesActor> cubeAxesActor;

    vtkNew<vtkLookupTable> lut;
    vtkNew<vtkScalarBarActor> scalarBar;
    vtkNew<vtkNamedColors> colors;

public:
    VisualizationWidget(QWidget* parent = nullptr) : QVTKOpenGLNativeWidget(parent)
    {
        setRenderWindow(window.Get());

        camera->SetViewUp(0, 1, 0);
        camera->SetPosition(0, 0, 10);
        camera->SetFocalPoint(0, 0, 0);

        renderer->SetActiveCamera(camera);
        renderer->SetBackground(colors->GetColor3d("Black").GetData());
        renderWindow()->AddRenderer(renderer);

        cubeAxesActor->SetUseTextActor3D(1);
        cubeAxesActor->GetTitleTextProperty(0)->SetFontSize(48);
        cubeAxesActor->DrawXGridlinesOn();
        cubeAxesActor->DrawYGridlinesOn();
        cubeAxesActor->DrawZGridlinesOn();
        cubeAxesActor->SetFlyModeToStaticEdges();

        textActor->GetTextProperty()->SetFontSize(24);
        textActor->GetTextProperty()->BoldOn();

        lut->Build();

        scalarBar->GetTitleTextProperty()->SetFontSize(20);
        scalarBar->GetLabelTextProperty()->SetFontSize(18);
        scalarBar->SetNumberOfLabels(7);
        scalarBar->UnconstrainedFontSizeOn();
    }

    ~VisualizationWidget() {}

    void visualizeDataSet(vtkSmartPointer<vtkDataSet> dataSet,
                          const char* description, 
                          const char* physicalQuantity)
    {
        renderer->RemoveAllViewProps();

        mapper->SetInputData(dataSet);
        mapper->SetScalarRange(dataSet->GetScalarRange());
        mapper->SetLookupTable(lut);
        actor->SetMapper(mapper);
        renderer->AddActor(actor);

        cubeAxesActor->SetBounds(dataSet->GetBounds());
        cubeAxesActor->SetCamera(renderer->GetActiveCamera());
        cubeAxesActor->SetGridLineLocation(cubeAxesActor->VTK_GRID_LINES_FURTHEST);
        renderer->AddActor(cubeAxesActor);

        textActor->SetInput(description);
        textActor->SetPosition(25, window->GetSize()[1] - 50);
        renderer->AddActor2D(textActor);

        scalarBar->SetLookupTable(lut);
        scalarBar->SetTitle(physicalQuantity);
        renderer->AddActor2D(scalarBar);

        renderer->ResetCamera(dataSet->GetBounds());
        renderWindow()->Render();
    }

    void openFile(const QString& fileName, const char* description)
    {
        vtkNew<vtkDataSetReader> reader;
        reader->SetFileName(fileName.toStdString().c_str());
        reader->Update();

        vtkSmartPointer<vtkDataSet> dataSet = reader->GetOutput();
        if (dataSet != nullptr) 
        { 
            this->visualizeDataSet(dataSet, 
                                   description,
                                   "Electrostatic Potential [V]"); 
        }
    }
};
