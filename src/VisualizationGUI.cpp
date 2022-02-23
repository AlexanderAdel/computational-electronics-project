/**
 *  \file VisualizationGUI.cpp
 *
 *  VisualizationGUI Execution File
 */

// Include for the VisualizationWidget Class
#include "VisualizationWidget.hpp"

// Include for the VisualizationWindow Class
#include "VisualizationWindow.hpp"

// Includes from the QT Library
#include <QApplication>
#include <QSurfaceFormat>

/**
 *  @brief Main function that executes the program.
 * 
 *  @param argc Argument counter. Not needed in this program.
 *  @param argv Argument vector. Not needed in this program.
 *  @return int Error code from the QApplication class.
 * 
 *  First the application is initialized. The window for the visualization is generated,
 *  the window title is defined and the window size is set to maximum. Then the 
 *  application is executed.
 */
int main(int argc, char** argv)
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication application(argc, argv);
    VisualizationWindow visualizationWindow;

    visualizationWindow.setWindowTitle(QString::fromUtf8("Computational Electronics Project"));
    visualizationWindow.setWindowState(Qt::WindowMaximized);
    visualizationWindow.show();

    return application.exec();
}
