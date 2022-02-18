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
    QPushButton* button;
    QPushButton* poissonButton_prepare;
    QPushButton* poissonButton_run;

    Poisson poisson_problem; // Poisson class
    bool generatedGrid = false; // Grid Flag

    QGroupBox* meshGroupBox;
    QFormLayout* meshFormLayout;

    QComboBox* meshDimension;

    QGroupBox* FEMGroupBox;
    QFormLayout* FEMFormLayout;

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
        gridLayout->addWidget(visualizationWidget, 0, 0, 7, 1);

        meshGroupBox = new QGroupBox(tr("MESH PROPERTIES"));
        meshGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        meshFormLayout = new QFormLayout;

        meshDimension = new QComboBox();
        meshDimension->addItem("2D");
        meshDimension->addItem("3D");

        meshFormLayout->addRow(new QLabel(tr("Dimension = ")), meshDimension);
        meshFormLayout->addRow(new QLabel(tr("Mesh Type = ")), new QComboBox);
        meshFormLayout->addRow(new QLabel(tr("Mesh Size = ")), new QLineEdit);
        meshFormLayout->addRow(new QLabel(tr("Boundary Condition = ")), new QComboBox);
        meshGroupBox->setLayout(meshFormLayout);
        gridLayout->addWidget(meshGroupBox, 0, 1);

        FEMGroupBox = new QGroupBox(tr("FEM PROPERTIES"));
        FEMGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        FEMFormLayout = new QFormLayout;
        FEMFormLayout->addRow(new QLabel(tr("Shape Functions = ")), new QComboBox);
        FEMFormLayout->addRow(new QLabel(tr("Refinement Level = ")), new QComboBox);
        FEMFormLayout->addRow(new QLabel(tr("Physical Quantity = ")), new QComboBox);
        FEMGroupBox->setLayout(FEMFormLayout);
        gridLayout->addWidget(FEMGroupBox, 1, 1);

        labelInnerRadius = new QLabel(this);
        labelInnerRadius->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(labelInnerRadius, 2, 1);
        labelOuterRadius = new QLabel(this);
        labelOuterRadius->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gridLayout->addWidget(labelOuterRadius, 3, 1);

        button = new QPushButton(this);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        button->setText("Generate Radial Mesh");
        QObject::connect(button, SIGNAL(clicked()), this, SLOT(clickedSlot_A()));
        gridLayout->addWidget(button, 4, 1);

        poissonButton_prepare = new QPushButton(this);
        poissonButton_prepare->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        poissonButton_prepare->setText("Generate Poisson Mesh");
        QObject::connect(poissonButton_prepare, SIGNAL(clicked()), this, SLOT(clickedSlot_B()));
        gridLayout->addWidget(poissonButton_prepare, 5, 1);

        poissonButton_run = new QPushButton(this);
        poissonButton_run->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        poissonButton_run->setText("Solve Poisson Problem");
        QObject::connect(poissonButton_run, SIGNAL(clicked()), this, SLOT(clickedSlot_C()));
        gridLayout->addWidget(poissonButton_run, 6, 1);
    }

    ~VisualizationWindow() {}

public slots:
    void clickedSlot_A()
    {
        QDialog dialog(this);
        dialog.resize(350, 100);
        QFormLayout form(&dialog);

        form.addRow(new QLabel("Set the parameters of the mesh:"));

        QLineEdit *lineEditInnerRadius = new QLineEdit(&dialog);
        QString labelInnerRadius = QString("Inner Radius = ");
        form.addRow(labelInnerRadius, lineEditInnerRadius);

        QLineEdit *lineEditOuterRadius = new QLineEdit(&dialog);
        QString labelOuterRadius = QString("Inner Radius = ");
        form.addRow(labelOuterRadius, lineEditOuterRadius);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        if (dialog.exec() == QDialog::Accepted) 
        {
            double innerRadius = lineEditInnerRadius->text().toDouble();
            double outerRadius = lineEditOuterRadius->text().toDouble();

            if (innerRadius >= outerRadius)
            {
                QMessageBox::information(this, "Error",
                "Inner Radius has to be smaller than Outer Radius!");
                return;
            }

            this->labelInnerRadius->setText("Inner Radius = " + lineEditInnerRadius->text());
            this->labelOuterRadius->setText("Outer Radius = " + lineEditOuterRadius->text());

            Solver::generateRadialGrid(innerRadius, outerRadius);
            visualizationWidget->openFile("radialGrid.vtk");
        }
    }

    void clickedSlot_B()
    {
        if (generatedGrid == true)
        {
            QMessageBox::information(this, "Error",
            "Grid already generated!");
            return;
        }

        poisson_problem.prepare();
        visualizationWidget->openFile("grid.vtk");

        generatedGrid = true;

        this->labelInnerRadius->setText("Poisson Grid Generated");
        this->labelOuterRadius->setText("");
    }

    void clickedSlot_C()
    {
        if (generatedGrid == false)
        {
            QMessageBox::information(this, "Error",
            "Please generate a grid before solving the Poisson problem!");
            return;
        }

        poisson_problem.run();
        visualizationWidget->openFile("solution.vtk");

        if (meshDimension->currentText() == "2D")
        { 
            this->labelInnerRadius->setText("Poisson Grid Generated in 2D");
            this->labelOuterRadius->setText("Poisson Grid Generated in 2D"); 
        }
        else if (meshDimension->currentText() == "3D")
        { 
            this->labelInnerRadius->setText("Poisson Grid Generated in 3D");
            this->labelOuterRadius->setText("Poisson Grid Generated in 3D"); 
        }
    }
}; 
