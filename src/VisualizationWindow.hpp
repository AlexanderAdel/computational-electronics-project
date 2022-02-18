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

class VisualizationWindow : public QMainWindow 
{
    Q_OBJECT

private:
    QWidget* centralWidget;
    QGridLayout* gridLayout;
    VisualizationWidget* visualizationWidget;

    QInputDialog* inputDialog;
    QLabel* labelInnerRadius;
    QLabel* labelOuterRadius;
    QPushButton* prepareButton;
    QPushButton* runButton;

    QGroupBox* meshGroupBox;
    QFormLayout* meshFormLayout;

    QLineEdit* dimension_X;
    QLineEdit* dimension_Y;
    QLineEdit* dimension_Z;

    QGroupBox* FEMGroupBox;
    QFormLayout* FEMFormLayout;

    QComboBox* refinement;
    QComboBox* shapeFunction;

    bool generatedGrid = false; // Grid Flag
    std::vector<int> _dimensions = std::vector<int>(3, 0);
    int _refinement = 0;
    int _shapeFunction = 0;

public:
    VisualizationWindow(QWidget* parent = nullptr) : QMainWindow(parent)
    {
        centralWidget = new QWidget(this);
        centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setCentralWidget(centralWidget);
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setColumnStretch(0, 4);
        gridLayout->setColumnStretch(1, 1);

        visualizationWidget = new VisualizationWidget();
        visualizationWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(visualizationWidget, 0, 0, 5, 1);

        meshGroupBox = new QGroupBox(tr("MESH PROPERTIES"));
        meshGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        meshFormLayout = new QFormLayout;

        dimension_X = new QLineEdit();
        dimension_Y = new QLineEdit();
        dimension_Z = new QLineEdit();

        meshFormLayout->addRow(new QLabel(tr("Dimension_X = ")), dimension_X);
        meshFormLayout->addRow(new QLabel(tr("Dimension_Y = ")), dimension_Y);
        meshFormLayout->addRow(new QLabel(tr("Dimension_Z = ")), dimension_Z);
        //meshFormLayout->addRow(new QLabel(tr("Mesh Type = ")), new QComboBox);
        //meshFormLayout->addRow(new QLabel(tr("Boundary Condition = ")), new QComboBox);
        meshGroupBox->setLayout(meshFormLayout);
        gridLayout->addWidget(meshGroupBox, 0, 1);

        refinement = new QComboBox();
        refinement->addItem("1");
        refinement->addItem("2");
        refinement->addItem("3");

        shapeFunction = new QComboBox();
        shapeFunction->addItem("1");
        shapeFunction->addItem("2");
        shapeFunction->addItem("3");

        FEMGroupBox = new QGroupBox(tr("FEM PROPERTIES"));
        FEMGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        FEMFormLayout = new QFormLayout;
        FEMFormLayout->addRow(new QLabel(tr("Refinement Level = ")), refinement);
        FEMFormLayout->addRow(new QLabel(tr("Shape Function Order = ")), shapeFunction);
        //FEMFormLayout->addRow(new QLabel(tr("Physical Quantity = ")), new QComboBox);
        FEMGroupBox->setLayout(FEMFormLayout);
        gridLayout->addWidget(FEMGroupBox, 1, 1);

        labelInnerRadius = new QLabel(this);
        labelInnerRadius->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(labelInnerRadius, 2, 1);
        labelOuterRadius = new QLabel(this);
        labelOuterRadius->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(labelOuterRadius, 3, 1);

        runButton = new QPushButton(this);
        runButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        runButton->setText("Solve Poisson Problem");
        QObject::connect(runButton, SIGNAL(clicked()), this, SLOT(clickedRunButton()));
        gridLayout->addWidget(runButton, 4, 1);
    }

    ~VisualizationWindow() {}

    void solvePoissonProblem(std::vector<int> _dimensions, int _refinement, int _shapeFunction)
    {
        Poisson poissonProblem(_dimensions, _refinement, _shapeFunction);
        poissonProblem.prepare();
        poissonProblem.run();
    }

public slots:
    void clickedRunButton()
    {
        _dimensions[0] = dimension_X->text().toInt();
        _dimensions[1] = dimension_Y->text().toInt();
        _dimensions[2] = dimension_Z->text().toInt();
        _refinement = refinement->currentText().toInt();
        _shapeFunction = shapeFunction->currentText().toInt();

        solvePoissonProblem(_dimensions, _refinement, _shapeFunction);
        visualizationWidget->openFile("solution.vtk");


        //if (meshDimension->currentText() == "2D")
        //{ 
        //    this->labelInnerRadius->setText("Poisson Grid Generated in 2D");
        //    this->labelOuterRadius->setText("Poisson Grid Generated in 2D"); 
        //}
        //else if (meshDimension->currentText() == "3D")
        //{ 
        //    this->labelInnerRadius->setText("Poisson Grid Generated in 3D");
        //    this->labelOuterRadius->setText("Poisson Grid Generated in 3D"); 
        //}
    }
}; 
