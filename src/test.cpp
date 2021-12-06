/* File: test.cpp */

// Include for Solver Library
#include "../lib/solver.hpp"

#include <QVTKOpenGLNativeWidget.h>
#include <QWidget>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>

#include <QMainWindow>
#include <QFile>
#include <QLabel>
#include <QDebug>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <vtkDataSetReader.h>

#include <QApplication>
#include <QSurfaceFormat>

class VisualizationWidget : public QVTKOpenGLNativeWidget 
{
    Q_OBJECT

private:
    vtkSmartPointer<vtkRenderer> renderer;

public:
    VisualizationWidget(QWidget* parent = nullptr) : QVTKOpenGLNativeWidget(parent)
    {
        vtkNew<vtkGenericOpenGLRenderWindow> window;
        setRenderWindow(window.Get());

        vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
        camera->SetViewUp(0, 1, 0);
        camera->SetPosition(0, 0, 10);
        camera->SetFocalPoint(0, 0, 0);

        renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->SetActiveCamera(camera);
        renderer->SetBackground(0.5, 0.5, 0.5);
        renderWindow()->AddRenderer(renderer);
    }

    ~VisualizationWidget() {}

    void openFile(const QString& fileName)
    {
        this->removeDataSet();

        vtkSmartPointer<vtkDataSetReader> reader = vtkSmartPointer<vtkDataSetReader>::New();
        reader->SetFileName(fileName.toStdString().c_str());

        reader->Update();

        vtkSmartPointer<vtkDataSet> dataSet = reader->GetOutput();
        if (dataSet != nullptr) { this->addDataSet(reader->GetOutput()); }
    }

    void addDataSet(vtkSmartPointer<vtkDataSet> dataSet)
    {
        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

        vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
        mapper->SetInputData(dataSet);
        actor->SetMapper(mapper);

        renderer->AddActor(actor);
        renderer->ResetCamera(dataSet->GetBounds());

        renderWindow()->Render();
    }

    void removeDataSet()
    {
        vtkActor* actor = renderer->GetActors()->GetLastActor();
        if (actor != nullptr) 
        { renderer->RemoveActor(actor); }

        renderWindow()->Render();
    }

public slots:
    void zoomToExtent()
    {
        vtkSmartPointer<vtkActor> actor = renderer->GetActors()->GetLastActor();
        if (actor != nullptr) 
        { renderer->ResetCamera(actor->GetBounds()); }

        renderWindow()->Render();
    }
};

class MainWindow : public QMainWindow 
{
    Q_OBJECT

private:
    VisualizationWidget* visualizationWidget;
    QInputDialog* inputDialog;
    QLabel* labelInnerRadius;
    QLabel* labelOuterRadius;
    QPushButton* button;
    QWidget* centralWidget;
    QVBoxLayout* layout;

public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent)
    {
        visualizationWidget = new VisualizationWidget();
        inputDialog = new QInputDialog(this);
        labelInnerRadius = new QLabel(this);
        labelOuterRadius = new QLabel(this);
        button = new QPushButton(this);
        centralWidget = new QWidget(this);
        layout = new QVBoxLayout(centralWidget);
    }

    ~MainWindow() {}

    void Execute()
    {
        visualizationWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        layout->addWidget(visualizationWidget);

        layout->addWidget(labelInnerRadius);
        layout->addWidget(labelOuterRadius);

        button->setText("Generate Radial Mesh");
        QObject::connect(button, SIGNAL(clicked()), this, SLOT(clickedSlot()));
        layout->addWidget(button);

        centralWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        setCentralWidget(centralWidget);
    }

public slots:
    void clickedSlot()
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
}; 

int main(int argc, char* argv[])
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication application(argc, argv);
    MainWindow mainWindow;

    mainWindow.setWindowTitle(QString::fromUtf8("Computational Electronics Project"));
    mainWindow.resize(600, 600);
    mainWindow.Execute();
    mainWindow.show();

    return application.exec();
}

// Include for Q_OBJECT
#include <test.moc>
