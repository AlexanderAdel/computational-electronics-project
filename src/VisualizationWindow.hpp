/**
 *  \file VisualizationWindow.hpp
 *
 *  VisualizationWindow Class Header File
 */

// Includes from the QT Library
#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QPushButton>

/**
 *  @brief Class for the GUI window that contains the visualization widget.
 *  
 *  The functionality of this class is based on the open source Qt library.
 */
class VisualizationWindow : public QMainWindow 
{
    Q_OBJECT

private:
    QWidget* centralWidget;                   //!< Connects the grid layout to the window
    QGridLayout* gridLayout;                  //!< Organizes the individual widgets in the window
    VisualizationWidget* visualizationWidget; //!< Visualizes the given data set

    QGroupBox* meshGroupBox;                  //!< Groups the mesh parameters in a box
    QFormLayout* meshFormLayout;              //!< Organizes the mesh parameters in rows
    QComboBox* meshType;                      //!< Selects the type of the mesh
    QValidator* dimValidator;                 //!< Defines the valid values for the dimensions
    QLineEdit* dimension_A;                   //!< Sets the value for the first dimension parameter
    QLabel* dimension_A_label;                //!< Sets the label for the first dimension parameter
    QLineEdit* dimension_B;                   //!< Sets the value for the second dimension parameter
    QLabel* dimension_B_label;                //!< Sets the label for the second dimension parameter
    QLineEdit* dimension_C;                   //!< Sets the value for the third dimension parameter
    QLabel* dimension_C_label;                //!< Sets the label for the third dimension parameter

    QGroupBox* boundaryGroupBox;              //!< Groups the boundary parameters in a box
    QFormLayout* boundaryFormLayout;          //!< Organizes the boundary parameters in rows
    QComboBox* boundaryCondition;             //!< Selects the boundary condition on the mesh
    QValidator* validator;                    //!< Defines the valid values for the boundary values
    QLineEdit* boundaryValue;                 //!< Sets the value for the constant boundary condition

    QGroupBox* FEMGroupBox;                   //!< Groups the FEM parameters in a box
    QFormLayout* FEMFormLayout;               //!< Organizes the FEM parameters in rows
    QComboBox* refinement;                    //!< Selects the refinement level on the mesh
    QComboBox* shapeFunction;                 //!< Selects the shape function order on the mesh

    QPushButton* runButton;                   //!< Executes the Poisson Solver 

private:
    std::vector<int> _dimensions2D = std::vector<int>(2, 0);          //!< Saves the 2D square dimensions
    Poisson<2>* poissonProblem2D;                                     //!< Points to the 2D square Poisson object
    std::vector<int> _dimensions3D = std::vector<int>(3, 0);          //!< Saves the 3D square dimensions
    Poisson<3>* poissonProblem3D;                                     //!< Points to the 3D square Poisson object
    std::vector<double> _dimensionsRad = std::vector<double>(2, 0.0); //!< Saves the radial dimensions
    Radial_Poisson* poissonProblemRad;                                //!< Points to the radial Poisson object

    int _refinement = 0;             //!< Saves the refinement level on the mesh
    int _shapeFunction = 0;          //!< Saves the shape funtion order on the mesh
    int _boundaryValue = 0;          //!< Saves the value on the boundary on the mesh
    QString _boundaryCondition;      //!< Saves the boundary condition type on the mesh
    bool boundaryIsConstant = false; //!< Saves if the boundary condition is constant
    bool meshNotChanged = false;     //!< Saves if all parameters are the same as in the last calculation

public:
    /**
     *  @brief Function that sets up the QGridLayout object.
     * 
     *  First the central widget which connects the grid layout with the main window is 
     *  initialized. Then the grid layout dimensions are set which determine how much
     *  space is used for the visualization widget and how much for the parameter GUI.
     */
    void setupGridLayout()
    {
        centralWidget = new QWidget(this);
        centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setCentralWidget(centralWidget);

        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setColumnStretch(0, 5);
        gridLayout->setColumnStretch(1, 1);
    }

    /**
     *  @brief Function that sets up the visualizationWidget object.
     * 
     *  The visualization widget is initialized and added to the grid layout.
     */
    void setupVisualizationWidget()
    {
        visualizationWidget = new VisualizationWidget();
        visualizationWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(visualizationWidget, 0, 0, 4, 1);
    }

    /**
     *  @brief Function that sets up the meshGroupBox object.
     * 
     *  The form layout is filled with the meshType combo box which selects the type of
     *  the used mesh and the line edits for the dimension values. This form layout is then
     *  added to the meshGroupBox, which is then added to the grid layout of the window.
     */
    void setupMeshGroupBox()
    {
        meshFormLayout = new QFormLayout;

        meshType = new QComboBox();
        meshType->addItem("2D Square Grid");
        meshType->addItem("3D Square Grid");
        meshType->addItem("Radial Grid");
        meshFormLayout->addRow(new QLabel(tr("Mesh Type = ")), meshType);
        QObject::connect(meshType, SIGNAL(currentIndexChanged(const QString&)),
                         this, SLOT(switchedMeshType(const QString&)));

        dimValidator = new QIntValidator(1, 999);

        dimension_A = new QLineEdit();
        dimension_A->setValidator(dimValidator);
        dimension_A_label = new QLabel(tr("Length in X = "));
        meshFormLayout->addRow(dimension_A_label, dimension_A);

        dimension_B = new QLineEdit();
        dimension_B->setValidator(dimValidator);
        dimension_B_label = new QLabel(tr("Length in Y = "));
        meshFormLayout->addRow(dimension_B_label, dimension_B);

        meshGroupBox = new QGroupBox(tr("MESH PARAMETERS"));
        meshGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        meshGroupBox->setLayout(meshFormLayout);
        gridLayout->addWidget(meshGroupBox, 0, 1);
    }

    /**
     *  @brief Function that sets up the boundaryGroupBox object.
     * 
     *  The form layout is filled with the boundaryCondition combo box which selects the type 
     *  of the used boundary condition on the mesh. This form layout is then added to the 
     *  boundaryGroupBox, which is then added to the grid layout of the window.
     */
    void setupBoundaryGroupBox()
    {
        boundaryFormLayout = new QFormLayout;

        boundaryCondition = new QComboBox();
        boundaryCondition->addItem("Euclidian Distance");
        boundaryCondition->addItem("Constant");
        boundaryFormLayout->addRow(new QLabel(tr("Boundary Condition = ")), boundaryCondition);
        QObject::connect(boundaryCondition, SIGNAL(currentIndexChanged(const QString&)),
                         this, SLOT(switchedBoundaryType(const QString&)));

        validator = new QIntValidator(0, 999);
        
        boundaryGroupBox = new QGroupBox(tr("BOUNDARY PARAMETERS"));
        boundaryGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        boundaryGroupBox->setLayout(boundaryFormLayout);
        gridLayout->addWidget(boundaryGroupBox, 1, 1);
    }

    /**
     *  @brief Function that sets up the FEMGroupBox object.
     * 
     *  The form layout is filled with the refinement combo box which selects the level
     *  of refinement on the mesh and the shape function combo box which selects the order
     *  of the shape funtions on the mesh. This form layout is then added to the 
     *  FEMGroupBox, which is then added to the grid layout of the window.
     */
    void setupFEMGroupBox()
    {
        FEMFormLayout = new QFormLayout;

        refinement = new QComboBox();
        refinement->addItem("1");
        refinement->addItem("2");
        refinement->addItem("3");
        FEMFormLayout->addRow(new QLabel(tr("Refinement Level = ")), refinement);

        shapeFunction = new QComboBox();
        shapeFunction->addItem("1");
        shapeFunction->addItem("2");
        shapeFunction->addItem("3");
        FEMFormLayout->addRow(new QLabel(tr("Shape Function Order = ")), shapeFunction);

        FEMGroupBox = new QGroupBox(tr("FEM PARAMETERS"));
        FEMGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        FEMGroupBox->setLayout(FEMFormLayout);
        gridLayout->addWidget(FEMGroupBox, 2, 1);
    }

    /**
     *  @brief Function that sets up the run button for the execution of the Poisson solver.
     * 
     *  After the initialization of the push button the button is connected to the 
     *  clickedRunButton() function, which executes the solution of the Poisson equation.
     */
    void setupRunButton()
    {
        runButton = new QPushButton(this);
        runButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        runButton->setText("Solve Poisson Problem");
        QObject::connect(runButton, SIGNAL(clicked()), this, SLOT(clickedRunButton()));
        gridLayout->addWidget(runButton, 3, 1);
    }

    /**
     *  @brief Constructor for the VisualizationWindow class.
     * 
     *  @param parent Pointer object for the initialization of the base class.
     *  @return New VisualizationWindow class object.
     */
    VisualizationWindow(QWidget* parent = nullptr) : QMainWindow(parent)
    {
        setupGridLayout();
        setupVisualizationWidget();
        setupMeshGroupBox();
        setupBoundaryGroupBox();
        setupFEMGroupBox();
        setupRunButton();
    }

    /**
     *  @brief Function that checks if the input parameters are acceptable.
     * 
     *  @return True if at least one input parameter is not acceptable, otherwise False.
     * 
     *  All dimension values and the boundary value are checked against their specific
     *  validator. Also additional checks are performed for the radial mesh. If one of 
     *  the input parameters is not acceptable, an error message is generated.
     */
    bool inputParametersNotAcceptable()
    {
        int pos = 0;
        QString dimension_A_text = dimension_A->text();
        QString dimension_B_text = dimension_B->text();
        if (dimValidator->validate(dimension_A_text, pos) != QValidator::Acceptable ||
            dimValidator->validate(dimension_B_text, pos) != QValidator::Acceptable)
        { 
            QMessageBox::information(this, "Error",
            "Please set all Parameters before solving the Poisson Problem!");
            return true; 
        }

        if (meshType->currentText() == "3D Square Grid")
        {
            QString dimension_C_text = dimension_C->text();
            if (dimValidator->validate(dimension_C_text, pos) != QValidator::Acceptable)
            { 
                QMessageBox::information(this, "Error",
                "Please set all Parameters before solving the Poisson Problem!");
                return true; 
            }
        }

        if (meshType->currentText() == "Radial Grid")
        {
            if (boundaryCondition->currentText() == "Euclidian Distance")
            {
                QMessageBox::information(this, "Error",
                "Only constant boundary conditions are supported for the Radial Grid!");
                return true;
            }

            double innerRadius = dimension_A->text().toDouble();
            double outerRadius = dimension_B->text().toDouble();
            if (innerRadius >= outerRadius)
            {
                QMessageBox::information(this, "Error",
                "Inner Radius has to be smaller than Outer Radius!");
                return true;
            }
        }

        if (boundaryIsConstant == true)
        {
            QString boundaryValue_text = boundaryValue->text();
            if (validator->validate(boundaryValue_text, pos) != QValidator::Acceptable)
            { 
                QMessageBox::information(this, "Error",
                "Please set all Parameters before solving the Poisson Problem!");
                return true; 
            }
        }

        return false;
    }

    /**
     *  @brief Function that checks if the 2D square grid has changed since the last calculation.
     */
    bool square2DGridNotChanged()
    {
        return (_dimensions2D[0]   == dimension_A->text().toInt() &&
                _dimensions2D[1]   == dimension_B->text().toInt() &&
                _refinement        == refinement->currentText().toInt() &&
                _shapeFunction     == shapeFunction->currentText().toInt() &&
                _boundaryCondition == boundaryCondition->currentText());
    }

    /**
     *  @brief Function that checks if the 3D square grid has changed since the last calculation.
     */
    bool square3DGridNotChanged()
    {
        return (_dimensions3D[0]   == dimension_A->text().toInt() &&
                _dimensions3D[1]   == dimension_B->text().toInt() &&
                _dimensions3D[2]   == dimension_C->text().toInt() &&
                _refinement        == refinement->currentText().toInt() &&
                _shapeFunction     == shapeFunction->currentText().toInt() &&
                _boundaryCondition == boundaryCondition->currentText());
    }

    /**
     *  @brief Function that checks if the radial grid has changed since the last calculation.
     */
    bool radialGridNotChanged()
    {
        return (_dimensionsRad[0]  == dimension_A->text().toInt() &&
                _dimensionsRad[1]  == dimension_B->text().toInt() &&
                _refinement        == refinement->currentText().toInt() &&
                _shapeFunction     == shapeFunction->currentText().toInt() &&
                _boundaryCondition == boundaryCondition->currentText());
    }

    /**
     *  @brief Function that checks if the boundary value has changed since the last calculation.
     */
    bool boundaryValueNotChanged()
    {
        if (boundaryCondition->currentText() == "Euclidian Distance")
        {   
            QMessageBox::information(this, "Error",
            "Poisson Problem already solved!");
            return true; 
        }
            
        if (_boundaryValue == boundaryValue->text().toInt())
        {
            QMessageBox::information(this, "Error",
            "Poisson Problem already solved!");
            return true;
        }

        return false;
    }

    /**
     *  @brief Function that solves the Poisson equation on the 2D square grid.
     * 
     *  The function checks if the mesh can be reused or if the program has to generate
     *  a new mesh. If only the boundary value has changed, the same mesh is used again for
     *  the new calculation. If this is not the case, all input parameters are read into the 
     *  respective variables and a new instance of the Poisson class is initialized. This 
     *  class is then used to solve the Poisson equation on the 2D square grid. The solution 
     *  is visualized by the visualization widget.
     */
    void solve2DsquareGrid()
    {
        if (square2DGridNotChanged())
        {
            if (boundaryValueNotChanged()) { return; }
            _boundaryValue = boundaryValue->text().toInt();

            poissonProblem2D->run(_boundaryValue);
            visualizationWidget->openFile("solution-2d.vtk", "Same Grid reused.");
        }
        else
        {
            _dimensions2D[0] = dimension_A->text().toInt();
            _dimensions2D[1] = dimension_B->text().toInt();
            _refinement = refinement->currentText().toInt();
            _shapeFunction = shapeFunction->currentText().toInt();
            _boundaryCondition = boundaryCondition->currentText();

            if (_boundaryCondition == "Constant")
            { _boundaryValue = boundaryValue->text().toInt(); }

            poissonProblem2D = new Poisson<2>(_dimensions2D, _refinement, _shapeFunction, _boundaryValue, boundaryIsConstant);
            poissonProblem2D->run();
            visualizationWidget->openFile("solution-2d.vtk", "New Grid generated.");
        }
    }

    /**
     *  @brief Function that solves the Poisson equation on the 3D square grid.
     * 
     *  The function checks if the mesh can be reused or if the program has to generate
     *  a new mesh. If only the boundary value has changed, the same mesh is used again for
     *  the new calculation. If this is not the case, all input parameters are read into the 
     *  respective variables and a new instance of the Poisson class is initialized. This 
     *  class is then used to solve the Poisson equation on the 3D square grid. The solution 
     *  is visualized by the visualization widget.
     */
    void solve3DsquareGrid()
    {
        if (square3DGridNotChanged())
        {
            if (boundaryValueNotChanged()) { return; }
            _boundaryValue = boundaryValue->text().toInt();

            poissonProblem3D->run(_boundaryValue);
            visualizationWidget->openFile("solution-3d.vtk", "Same Grid reused.");
        }
        else
        {
            _dimensions3D[0] = dimension_A->text().toInt();
            _dimensions3D[1] = dimension_B->text().toInt();
            _dimensions3D[2] = dimension_C->text().toInt();
            _refinement = refinement->currentText().toInt();
            _shapeFunction = shapeFunction->currentText().toInt();
            _boundaryCondition = boundaryCondition->currentText();
            
            if (_boundaryCondition == "Constant")
            { _boundaryValue = boundaryValue->text().toInt(); }

            poissonProblem3D = new Poisson<3>(_dimensions3D, _refinement, _shapeFunction, _boundaryValue, boundaryIsConstant);
            poissonProblem3D->run();
            visualizationWidget->openFile("solution-3d.vtk", "New Grid generated.");
        }
    }

    /**
     *  @brief Function that solves the Poisson equation on the radial grid.
     * 
     *  The function checks if the mesh can be reused or if the program has to generate
     *  a new mesh. If only the boundary value has changed, the same mesh is used again for
     *  the new calculation. If this is not the case, all input parameters are read into the 
     *  respective variables and a new instance of the Poisson class is initialized. This 
     *  class is then used to solve the Poisson equation on the radial grid. The solution 
     *  is visualized by the visualization widget.
     */
    void solveRadialGrid()
    {
        if (radialGridNotChanged())
        {
            if (boundaryValueNotChanged()) { return; }
            _boundaryValue = boundaryValue->text().toInt();
        
            poissonProblemRad->run(_boundaryValue);
            visualizationWidget->openFile("solution-2d.vtk", "Same Grid reused.");
        }
        else
        {
            _dimensionsRad[0] = dimension_A->text().toDouble();
            _dimensionsRad[1] = dimension_B->text().toDouble();
            _refinement = refinement->currentText().toInt();
            _shapeFunction = shapeFunction->currentText().toInt();
            _boundaryCondition = boundaryCondition->currentText();
            
            if (_boundaryCondition == "Constant")
            { _boundaryValue = boundaryValue->text().toInt(); }

            poissonProblemRad = new Radial_Poisson(_dimensionsRad, _refinement, _shapeFunction, _boundaryValue);
            poissonProblemRad->run();
            visualizationWidget->openFile("solution-2d.vtk", "New Grid generated.");
        }
    }

public slots:
    /**
     *  @brief Function that changes the GUI depending on the selected mesh type.
     * 
     *  @param meshTypeString String that contains the selected mesh type.
     * 
     *  If a two dimensional mesh is selected, only two dimensions can be set to
     *  a specific value. If a three dimensional mesh is selected, all three 
     *  dimensions can be set to a specific value.
     */
    void switchedMeshType(const QString& meshTypeString)
    {
        if (meshTypeString == "2D Square Grid")
        {
            dimension_A_label->setText("Length in X = ");
            dimension_B_label->setText("Length in Y = ");

            meshFormLayout->removeRow(dimension_C);
        }
        else if (meshTypeString == "3D Square Grid")
        {
            dimension_A_label->setText("Length in X = ");
            dimension_B_label->setText("Length in Y = ");

            dimension_C = new QLineEdit();
            dimension_C->setValidator(dimValidator);
            dimension_C_label = new QLabel(tr("Length in Z = "));
            meshFormLayout->addRow(dimension_C_label, dimension_C);
        }
        else if (meshTypeString == "Radial Grid")
        {
            dimension_A_label->setText("Inner Radius = ");
            dimension_B_label->setText("Outer Radius = ");

            meshFormLayout->removeRow(dimension_C);
        }
    }

    /**
     *  @brief Function that changes the GUI depending on the selected boundary condition type.
     * 
     *  @param boundaryTypeString String that contains the selected boundary condition type.
     * 
     *  If a constant boundary condition is selected, the value of the solution on the 
     *  boundary can be set to a specific value. If the euclidian distance boundary condition
     *  is selected, no additional value can be set, since this is not necessary in this case.
     */
    void switchedBoundaryType(const QString& boundaryTypeString)
    {
        if (boundaryTypeString == "Euclidian Distance")
        { 
            boundaryIsConstant = false; 
            boundaryFormLayout->removeRow(boundaryValue);
        }
        else if (boundaryTypeString == "Constant")
        { 
            boundaryIsConstant = true; 
            boundaryValue = new QLineEdit();
            boundaryValue->setValidator(validator);
            boundaryFormLayout->addRow(new QLabel(tr("Boundary Value = ")), boundaryValue);
        }
    }

    /**
     *  @brief Function that executes the Poisson problem if the run button is clicked.
     * 
     *  If the run button is clicked, the function is checking if all input parameters 
     *  are valid. If this is the case, the Poisson equation is solved on the selected
     *  mesh with the help of the Poisson library. The solution is then visualized by
     *  the visualization widget.
     */
    void clickedRunButton()
    {
        if (inputParametersNotAcceptable()) { return; }

        if (meshType->currentText() == "2D Square Grid")
        {
            solve2DsquareGrid();
        }
        else if (meshType->currentText() == "3D Square Grid")
        {
            solve3DsquareGrid();
        }
        else if (meshType->currentText() == "Radial Grid")
        {
            solveRadialGrid();
        }
    }
}; 
