/**
 *  \file VisualizationWidget.hpp
 *
 *  VisualizationWidget Class Header File
 */

#pragma once

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
#include <vtkBoxClipDataSet.h>

/**
 *  @brief Class for the visualization of the solution of the Poisson problem.
 *  
 *  The functionality of the widget is based on the VTK library.
 */
class VisualizationWidget : public QVTKOpenGLNativeWidget 
{
    Q_OBJECT

private:
    vtkNew<vtkGenericOpenGLRenderWindow> window; //!< Shows the finished visualization
    vtkNew<vtkRenderer> renderer;                //!< Renders the given actors
    vtkNew<vtkCamera> camera;                    //!< Defines the view point 

    vtkNew<vtkBoxClipDataSet> boxClip;           //!< Shows the inside of the 3D square grid
    vtkNew<vtkDataSetMapper> mapper;             //!< Connects the data set with the actor
    vtkNew<vtkActor> actor;                      //!< Contains the visualization data set
    vtkNew<vtkTextActor> textActor;              //!< Contains the description string
    vtkNew<vtkCubeAxesActor> cubeAxesActor;      //!< Contains the cartesian axes

    vtkNew<vtkLookupTable> lut;                  //!< Contains the scalar value range
    vtkNew<vtkScalarBarActor> scalarBar;         //!< Contains the color bar on the right
    vtkNew<vtkNamedColors> colors;               //!< Defines the used colors

public:
    /**
     *  @brief Function that sets up the vtkGenericOpenGLRenderWindow object.
     * 
     *  The render window is set up and the background is set to black. 
     *  The renderer object is then added to the window.
     */
    void setupWindow()
    {
        setRenderWindow(window.Get());
        renderer->SetBackground(colors->GetColor3d("Black").GetData());
        renderWindow()->AddRenderer(renderer);
    }

    /**
     *  @brief Function that sets up the VTKCamera object.
     * 
     *  The initial view point is defined from a top down perspective. The camera object is 
     *  then added to the renderer.
     */
    void setupCamera()
    {
        camera->SetViewUp(0, 1, 0);
        camera->SetPosition(0, 0, 10);
        camera->SetFocalPoint(0, 0, 0);
        renderer->SetActiveCamera(camera);
    }

    /**
     *  @brief Constructor for the VisualizationWidget class.
     * 
     *  @param parent Pointer object for the initialization of the base class.
     *  @return New VisualizationWidget class object.
     */
    VisualizationWidget(QWidget* parent = nullptr) : QVTKOpenGLNativeWidget(parent)
    {
        setupWindow();
        setupCamera();
    }

    /**
     * @brief Function that checks if the given dataSet has three dimensions.
     * 
     * @param zmax Largest value of the dataSet on the z-axis.
     * @return True if the dataSet is three dimensional, otherwise False. 
     */
    bool dataSetIsTreeDimensional(int zmax)
    {
        if (zmax != 0.0) { return true; }
        else             { return false; }
    }

    /**
     *  @brief Function that sets up the vtkBoxClipDataSet object. 
     * 
     *  @param dataSet Data set which should be visualized.
     * 
     *  The center and the corner point and the corresponding normal vectors are defined.
     *  With this geometric variables one quarter of the 3D square grid can be clipped,
     *  which allows to inspect the inside of the three dimensional data set.
     */
    void setupBoxClip(vtkSmartPointer<vtkDataSet> dataSet)
    {
        double* bounds = dataSet->GetBounds();
        double* center = dataSet->GetCenter();
        double minBoxPoint[3] = { center[0], center[1], center[2] };
        double maxBoxPoint[3] = { bounds[1], bounds[3], bounds[5] };

        const double minusx[] = {-1.0, 0.0, 0.0}; const double plusx[] = {1.0, 0.0, 0.0};
        const double minusy[] = {0.0, -1.0, 0.0}; const double plusy[] = {0.0, 1.0, 0.0};
        const double minusz[] = {0.0, 0.0, -1.0}; const double plusz[] = {0.0, 0.0, 1.0};

        boxClip->SetInputData(dataSet);
        boxClip->GenerateClippedOutputOn();
        boxClip->SetBoxClip(minusx, minBoxPoint, minusy, minBoxPoint, minusz, minBoxPoint, 
                            plusx, maxBoxPoint, plusy, maxBoxPoint, plusz, maxBoxPoint);

        mapper->SetInputConnection(boxClip->GetOutputPort(1));
    }

    /**
     *  @brief Function that sets up the vtkActor object. 
     * 
     *  @param dataSet Data set which should be visualized.
     * 
     *  If the given data set is three dimensional, a box clip is generated. If it is two
     *  dimensional, the whole data set is given as input data. The look up table for the
     *  color mapping is set up. The actor object is then added to the renderer.
     */
    void setupActor(vtkSmartPointer<vtkDataSet> dataSet)
    {
        int zmax = dataSet->GetBounds()[5];
        if (dataSetIsTreeDimensional(zmax)) { setupBoxClip(dataSet); }
        else                                { mapper->SetInputData(dataSet); }

        mapper->SetScalarRange(dataSet->GetScalarRange());
        mapper->SetLookupTable(lut);

        actor->SetMapper(mapper);
        renderer->AddActor(actor);
    }

    /**
     *  @brief Function that sets up the vtkCubeAxesActor object. 
     * 
     *  @param dataSet Data set which should be visualized.
     * 
     *  All properties of the cube axes are defined, such as the cartesian coordinate axes,
     *  the font size and the bounds of the gridlines. The cube axes actor object is then
     *  added to the renderer.
     */
    void setupCubeAxesActor(vtkSmartPointer<vtkDataSet> dataSet)
    {
        cubeAxesActor->SetUseTextActor3D(1);
        cubeAxesActor->GetTitleTextProperty(0)->SetFontSize(48);
        cubeAxesActor->DrawXGridlinesOn();
        cubeAxesActor->DrawYGridlinesOn();
        cubeAxesActor->DrawZGridlinesOn();
        cubeAxesActor->SetFlyModeToStaticEdges();

        cubeAxesActor->SetBounds(dataSet->GetBounds());
        cubeAxesActor->SetCamera(renderer->GetActiveCamera());
        cubeAxesActor->SetGridLineLocation(cubeAxesActor->VTK_GRID_LINES_FURTHEST);
        renderer->AddActor(cubeAxesActor);
    }

    /**
     *  @brief Function that sets up the vtkTextActor object. 
     * 
     *  @param description Information if the used grid is newly generated.
     * 
     *  All properties such as font size and placement are defined. The text actor object
     *  is then added to the renderer.
     */
    void setupTextActor(const char* description)
    {
        textActor->GetTextProperty()->SetFontSize(24);
        textActor->GetTextProperty()->BoldOn();

        textActor->SetInput(description);
        textActor->SetPosition(25, window->GetSize()[1] - 50);
        renderer->AddActor2D(textActor);
    }

    /**
     *  @brief Function that sets up the vtkTextActor object. 
     * 
     *  @param physicalQuantity The name of the phyical quantity that is calculated by the Poisson Solver.
     * 
     *  All properties of the color bar are defined. The look up table is connected to the
     *  color bar. The scalar bar object is then added to the renderer.
     */
    void setupScalarBar(const char* physicalQuantity)
    {
        scalarBar->GetTitleTextProperty()->SetFontSize(20);
        scalarBar->GetLabelTextProperty()->SetFontSize(18);
        scalarBar->SetNumberOfLabels(7);
        scalarBar->UnconstrainedFontSizeOn();

        lut->Build();
        scalarBar->SetLookupTable(lut);
        scalarBar->SetTitle(physicalQuantity);
        renderer->AddActor2D(scalarBar);
    }

    /**
     *  @brief Function that visualizes the given data set in the render window.
     * 
     *  @param dataSet Data set which should be visualized.
     *  @param description Information if the used grid is newly generated.
     *  @param physicalQuantity The name of the phyical quantity that is calculated by the Poisson Solver.
     * 
     *  First all remaining actors are removed from the render window. Then the data set,
     *  the cube axes, the text description and the color bar are initialized. At the end
     *  the camera is set to the bounds of the new data set and all new input is rendered.
     */
    void visualizeDataSet(vtkSmartPointer<vtkDataSet> dataSet,
                          const char* description, 
                          const char* physicalQuantity)
    {
        renderer->RemoveAllViewProps();

        setupActor(dataSet);
        setupCubeAxesActor(dataSet);
        setupTextActor(description);
        setupScalarBar(physicalQuantity);

        renderer->ResetCamera(dataSet->GetBounds());
        renderWindow()->Render();
    }

    /**
     *  @brief Function visualizes a data set given in a VTK file.
     * 
     *  @param fileName Name of the VTK file that contains the data set.
     *  @param description Information if the used grid is newly generated.
     * 
     *  The function opens a VTK file, reads the content and extracts the data set.
     *  This data set is then visualized in the render window.
     */
    void openFile(const QString& fileName, const char* description)
    {
        vtkNew<vtkDataSetReader> reader;
        reader->SetFileName(fileName.toStdString().c_str());
        reader->Update();

        vtkSmartPointer<vtkDataSet> dataSet = reader->GetOutput();
        visualizeDataSet(dataSet, description, "Physical Quantity"); 
    }
};
