/* File: VisualizationGUI.cpp */

// Include for the VisualizationWidget Class
#include "VisualizationWidget.hpp"

// Include for the VisualizationWindow Class
#include "VisualizationWindow.hpp"

// Includes from the QT Library
#include <QApplication>
#include <QSurfaceFormat>

/*!
 *  @brief Main function.
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
