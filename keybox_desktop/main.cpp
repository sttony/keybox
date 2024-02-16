
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include "MainWindow.h"
#include "EntryView.h"

int main(int argc, char *argv[])
{
    // Create a Qt application
    QApplication app(argc, argv);

    EntryView ev;
    int ret = ev.exec();


    // Create a main window
//    MainWindow mainWindow;
//
//
//
//    // Show the main window
//    mainWindow.show();

    // Run the application event loop
    return 0;
    //return QApplication::exec();
}