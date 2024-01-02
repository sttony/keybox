
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    // Create a Qt application
    QApplication app(argc, argv);

    // Create a main window
    MainWindow mainWindow;



    // Show the main window
    mainWindow.show();

    // Run the application event loop
    return QApplication::exec();
}