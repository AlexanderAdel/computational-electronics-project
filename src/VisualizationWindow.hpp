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

    QInputDialog* inputDialog;
    QLabel* label_1;
    QLabel* label_2;
    QLabel* label_3;
    QPushButton* prepareButton;
    QPushButton* runButton;

    QGroupBox* meshGroupBox;
    QFormLayout* meshFormLayout;

    QComboBox* meshType;
    QLineEdit* boundaryValue;

    QLineEdit* dimension_A;
    QLineEdit* dimension_B;
    QLabel* dimension_A_label;
    QLabel* dimension_B_label;

    QGroupBox* FEMGroupBox;
    QFormLayout* FEMFormLayout;

    QComboBox* refinement;
    QComboBox* shapeFunction;

    bool generatedGrid = false; // Grid Flag
    std::vector<int> _dimensions = std::vector<int>(2, 0);
    int _refinement = 0;
    int _shapeFunction = 0;

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
        gridLayout->addWidget(visualizationWidget, 0, 0, 6, 1);

        meshGroupBox = new QGroupBox(tr("MESH PROPERTIES"));
        meshGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        meshFormLayout = new QFormLayout;

        meshType = new QComboBox();
        meshType->addItem("Square Grid");
        meshType->addItem("Radial Grid");

        boundaryValue = new QLineEdit();

        dimension_A = new QLineEdit();
        dimension_B = new QLineEdit();
        dimension_A_label = new QLabel(tr("Length = "));
        dimension_B_label = new QLabel(tr("Width = "));

        meshFormLayout->addRow(new QLabel(tr("Mesh Type = ")), meshType);
        QObject::connect(meshType, SIGNAL(currentIndexChanged(const QString&)),
                         this, SLOT(switchedMeshType(const QString&)));

        meshFormLayout->addRow(new QLabel(tr("Boundary Value = ")), boundaryValue);
        meshFormLayout->addRow(dimension_A_label, dimension_A);
        meshFormLayout->addRow(dimension_B_label, dimension_B);
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
        FEMGroupBox->setLayout(FEMFormLayout);
        gridLayout->addWidget(FEMGroupBox, 1, 1);

        label_1 = new QLabel(this);
        label_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(label_1, 2, 1);
        label_2 = new QLabel(this);
        label_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(label_2, 3, 1);
        label_3 = new QLabel(this);
        label_3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(label_3, 4, 1);

        runButton = new QPushButton(this);
        runButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        runButton->setText("Solve Poisson Problem");
        QObject::connect(runButton, SIGNAL(clicked()), this, SLOT(clickedRunButton()));
        gridLayout->addWidget(runButton, 5, 1);
    }

    ~VisualizationWindow() {}

    void solvePoissonProblem(std::vector<int> _dimensions, int _refinement, int _shapeFunction)
    {
        Poisson poissonProblem(_dimensions, _refinement, _shapeFunction);
        poissonProblem.prepare();
        poissonProblem.run();
    }

public slots:
    void switchedMeshType(const QString& meshTypeString)
    {
        if (meshTypeString == "Square Grid")
        {
            dimension_A_label->setText("Length = ");
            dimension_B_label->setText("Width = ");
        }
        else if (meshTypeString == "Radial Grid")
        {
            dimension_A_label->setText("Inner Radius = ");
            dimension_B_label->setText("Outer Radius = ");
        }
    }

    void clickedRunButton()
    {
        if (meshType->currentText() == "Square Grid")
        {
            _dimensions[0] = dimension_A->text().toInt();
            _dimensions[1] = dimension_B->text().toInt();
            _refinement = refinement->currentText().toInt();
            _shapeFunction = shapeFunction->currentText().toInt();

            solvePoissonProblem(_dimensions, _refinement, _shapeFunction);
            visualizationWidget->openFile("solution.vtk");
        }
        else if (meshType->currentText() == "Radial Grid")
        {
            // TODO
        }
    }
}; 
