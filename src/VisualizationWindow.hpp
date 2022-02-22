/* File: VisualizationWindow.hpp */

// Includes from the QT Library
#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFile>
#include <QLabel>
#include <QDebug>
#include <QFormLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QIntValidator>

/*!
 *  @brief Visualization Window.
 */
class VisualizationWindow : public QMainWindow 
{
    Q_OBJECT

private:
    QWidget* centralWidget;
    QGridLayout* gridLayout;
    VisualizationWidget* visualizationWidget;

    QGroupBox* meshGroupBox;
    QFormLayout* meshFormLayout;

    QComboBox* meshType;

    QValidator* validator;
    QValidator* dim_validator;

    QLineEdit* dimension_A;
    QLineEdit* dimension_B;
    QLineEdit* dimension_C;
    QLabel* dimension_A_label;
    QLabel* dimension_B_label;
    QLabel* dimension_C_label;

    QGroupBox* BCGroupBox;
    QFormLayout* BCFormLayout;

    QComboBox* boundaryCondition;
    QLineEdit* boundaryValue;

    QGroupBox* FEMGroupBox;
    QFormLayout* FEMFormLayout;

    QComboBox* refinement;
    QComboBox* shapeFunction;

    QPushButton* runButton;

    bool generatedGrid = false; // Grid Flag
    bool constantBC = false;

    std::vector<int> _dimensions2D = std::vector<int>(2, 0);
    std::vector<int> _dimensions3D = std::vector<int>(3, 0);
    std::vector<double> _dimensionsRad = std::vector<double>(2, 0.0);
    int _boundaryValue = 0;
    int _refinement = 0;
    int _shapeFunction = 0;
    QString _BC;

    Poisson<2>* poissonProblem2D;
    Poisson<3>* poissonProblem3D;
    Radial_Poisson* poissonProblemRad;

public:
    VisualizationWindow(QWidget* parent = nullptr) : QMainWindow(parent)
    {
        centralWidget = new QWidget(this);
        centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setCentralWidget(centralWidget);
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setColumnStretch(0, 5);
        gridLayout->setColumnStretch(1, 1);

        visualizationWidget = new VisualizationWidget();
        visualizationWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(visualizationWidget, 0, 0, 4, 1);

        meshGroupBox = new QGroupBox(tr("MESH PARAMETERS"));
        meshGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        meshFormLayout = new QFormLayout;

        meshType = new QComboBox();
        meshType->addItem("2D Square Grid");
        meshType->addItem("3D Square Grid");
        meshType->addItem("Radial Grid");

        dim_validator = new QIntValidator(1, 999);
        dimension_A = new QLineEdit();
        dimension_A->setValidator(dim_validator);
        dimension_B = new QLineEdit();
        dimension_B->setValidator(dim_validator);
        dimension_A_label = new QLabel(tr("Length in X = "));
        dimension_B_label = new QLabel(tr("Length in Y = "));

        meshFormLayout->addRow(new QLabel(tr("Mesh Type = ")), meshType);
        QObject::connect(meshType, SIGNAL(currentIndexChanged(const QString&)),
                         this, SLOT(switchedMeshType(const QString&)));

        meshFormLayout->addRow(dimension_A_label, dimension_A);
        meshFormLayout->addRow(dimension_B_label, dimension_B);
        meshGroupBox->setLayout(meshFormLayout);
        gridLayout->addWidget(meshGroupBox, 0, 1);

        boundaryCondition = new QComboBox();
        boundaryCondition->addItem("Euclidian Distance");
        boundaryCondition->addItem("Constant");
        QObject::connect(boundaryCondition, SIGNAL(currentIndexChanged(const QString&)),
                         this, SLOT(switchedBCType(const QString&)));
        validator = new QIntValidator(0, 999);

        BCGroupBox = new QGroupBox(tr("BOUNDARY PARAMETERS"));
        BCGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        BCFormLayout = new QFormLayout;
        BCFormLayout->addRow(new QLabel(tr("Boundary Condition = ")), boundaryCondition);
        BCGroupBox->setLayout(BCFormLayout);
        gridLayout->addWidget(BCGroupBox, 1, 1);

        refinement = new QComboBox();
        refinement->addItem("1");
        refinement->addItem("2");
        refinement->addItem("3");

        shapeFunction = new QComboBox();
        shapeFunction->addItem("1");
        shapeFunction->addItem("2");
        shapeFunction->addItem("3");

        FEMGroupBox = new QGroupBox(tr("FEM PARAMETERS"));
        FEMGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        FEMFormLayout = new QFormLayout;
        FEMFormLayout->addRow(new QLabel(tr("Refinement Level = ")), refinement);
        FEMFormLayout->addRow(new QLabel(tr("Shape Function Order = ")), shapeFunction);
        FEMGroupBox->setLayout(FEMFormLayout);
        gridLayout->addWidget(FEMGroupBox, 2, 1);

        runButton = new QPushButton(this);
        runButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        runButton->setText("Solve Poisson Problem");
        QObject::connect(runButton, SIGNAL(clicked()), this, SLOT(clickedRunButton()));
        gridLayout->addWidget(runButton, 3, 1);
    }

    ~VisualizationWindow() {}

public slots:
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
            dimension_C->setValidator(dim_validator);
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

    void switchedBCType(const QString& BCTypeString)
    {
        if (BCTypeString == "Constant")
        { 
            constantBC = true; 

            boundaryValue = new QLineEdit();
            boundaryValue->setValidator(validator);
            BCFormLayout->addRow(new QLabel(tr("Boundary Value = ")), boundaryValue);
        }
        else if (BCTypeString == "Euclidian Distance")
        { 
            constantBC = false; 

            BCFormLayout->removeRow(boundaryValue);
        }
    }

    void clickedRunButton()
    {
        int pos = 0;
        QString dimension_A_text = dimension_A->text();
        QString dimension_B_text = dimension_B->text();
        
        if (dim_validator->validate(dimension_A_text, pos) != QValidator::Acceptable ||
            dim_validator->validate(dimension_B_text, pos) != QValidator::Acceptable)
        {
            QMessageBox::information(this, "Error",
            "Please set all Parameters before solving the Poisson Problem!");
            return;
        }

        if (constantBC == true)
        {
            QString boundaryValue_text = boundaryValue->text();

            if (validator->validate(boundaryValue_text, pos) != QValidator::Acceptable)
            {
                QMessageBox::information(this, "Error",
                "Please set all Parameters before solving the Poisson Problem!");
                return;
            }
        }

        if (meshType->currentText() == "2D Square Grid")
        {
            /*
            if (_dimensions2D[0] == dimension_A->text().toInt() &&
                _dimensions2D[1] == dimension_B->text().toInt() &&
                _refinement == refinement->currentText().toInt() &&
                _shapeFunction == shapeFunction->currentText().toInt() &&
                _BC == boundaryCondition->currentText())
            {
                if (boundaryCondition->currentText() == "Euclidian Distance" ||
                    _boundaryValue == boundaryValue->text().toInt())
                {
                    QMessageBox::information(this, "Error",
                    "Poisson Problem already solved!");
                    return;
                }

                if (boundaryCondition->currentText() == "Constant")
                { _boundaryValue = boundaryValue->text().toInt(); }

                poissonProblem2D->run(_boundaryValue);
                visualizationWidget->openFile("solution-2d.vtk", "Same Grid reused.");
                return;
            }
            */

            _dimensions2D[0] = dimension_A->text().toInt();
            _dimensions2D[1] = dimension_B->text().toInt();
            _refinement = refinement->currentText().toInt();
            _shapeFunction = shapeFunction->currentText().toInt();
            _BC = boundaryCondition->currentText();

            if (boundaryCondition->currentText() == "Constant")
            { _boundaryValue = boundaryValue->text().toInt(); }

            poissonProblem2D = new Poisson<2>(_dimensions2D, _refinement, _shapeFunction, _boundaryValue, constantBC);
            poissonProblem2D->run();
            visualizationWidget->openFile("solution-2d.vtk", "New Grid generated.");
        }
        else if (meshType->currentText() == "3D Square Grid")
        {
            QString dimension_C_text = dimension_C->text();

            if (dim_validator->validate(dimension_C_text, pos) != QValidator::Acceptable)
            {
                QMessageBox::information(this, "Error",
                "Please set all Parameters before solving the Poisson Problem!");
                return;
            }

            /*
            if (_dimensions3D[0] == dimension_A->text().toInt() &&
                _dimensions3D[1] == dimension_B->text().toInt() &&
                _dimensions3D[2] == dimension_C->text().toInt() &&
                _refinement == refinement->currentText().toInt() &&
                _shapeFunction == shapeFunction->currentText().toInt() &&
                _BC == boundaryCondition->currentText())
            {
                if (boundaryCondition->currentText() == "Euclidian Distance" ||
                    _boundaryValue == boundaryValue->text().toInt())
                {
                    QMessageBox::information(this, "Error",
                    "Poisson Problem already solved!");
                    return;
                }

                if (boundaryCondition->currentText() == "Constant")
                { _boundaryValue = boundaryValue->text().toInt(); }

                poissonProblem3D->run(_boundaryValue);
                visualizationWidget->openFile("solution-3d.vtk", "Same Grid reused.");
                return;
            }
            */

            _dimensions3D[0] = dimension_A->text().toInt();
            _dimensions3D[1] = dimension_B->text().toInt();
            _dimensions3D[2] = dimension_C->text().toInt();
            _refinement = refinement->currentText().toInt();
            _shapeFunction = shapeFunction->currentText().toInt();
            _BC = boundaryCondition->currentText();

            if (boundaryCondition->currentText() == "Constant")
            { _boundaryValue = boundaryValue->text().toInt(); }

            poissonProblem3D = new Poisson<3>(_dimensions3D, _refinement, _shapeFunction, _boundaryValue, constantBC);
            poissonProblem3D->run();
            visualizationWidget->openFile("solution-3d.vtk", "New Grid generated.");
        }
        else if (meshType->currentText() == "Radial Grid")
        {
            if (boundaryCondition->currentText() == "Euclidian Distance")
            {
                QMessageBox::information(this, "Error",
                "Only constant boundary conditions are supported for the Radial Grid!");
                return;
            }

            /*
            if (_dimensions3D[0] == dimension_A->text().toInt() &&
                _dimensions3D[1] == dimension_B->text().toInt() &&
                _dimensions3D[2] == dimension_C->text().toInt() &&
                _refinement == refinement->currentText().toInt() &&
                _shapeFunction == shapeFunction->currentText().toInt() &&
                _BC == boundaryCondition->currentText())
            {
                if (_boundaryValue == boundaryValue->text().toInt())
                {
                    QMessageBox::information(this, "Error",
                    "Poisson Problem already solved!");
                    return;
                }

                if (boundaryCondition->currentText() == "Constant")
                { _boundaryValue = boundaryValue->text().toInt(); }

                poissonProblemRad->run(_boundaryValue);
                visualizationWidget->openFile("solution-2d.vtk", "Same Grid reused.");
                return;
            }
            */

            _dimensionsRad[0] = dimension_A->text().toDouble();
            _dimensionsRad[1] = dimension_B->text().toDouble();
            _refinement = refinement->currentText().toInt();
            _shapeFunction = shapeFunction->currentText().toInt();
            _BC = boundaryCondition->currentText();

            if (boundaryCondition->currentText() == "Constant")
            { _boundaryValue = boundaryValue->text().toInt(); }

            if (_dimensionsRad[0] >= _dimensionsRad[1])
            {
                QMessageBox::information(this, "Error",
                "Inner Radius has to be smaller than Outer Radius!");
                return;
            }

            poissonProblemRad = new Radial_Poisson(_dimensionsRad, _refinement, _shapeFunction, _boundaryValue);
            poissonProblemRad->run();
            visualizationWidget->openFile("solution-2d.vtk", "New Grid generated.");
        }
    }
}; 
