#include <iostream>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include "MainWindow.h"
#include "EntryDlg.h"
#include "../utilities/CRandomGenerator.h"
#include "CPrimaryPasswordDlg.h"
#include "utilities/InitGlobalRG.h"

using namespace std;

int main(int argc, char *argv[]) {
    // Create a Qt application
    QApplication app(argc, argv);
    InitGlobalRG();
//    EntryDlg ev;
//    int ret = ev.exec();
//    CPrimaryPasswordDlg masterpasswordView;
//    if( masterpasswordView.exec() == QDialog::Accepted){
//        cout<<masterpasswordView.GetPassword().Show()<<endl;
//    }
//    else{
//        return 0;
//    }
    // Create a main window
    MainWindow mainWindow;

    // Show the main window
    mainWindow.show();

    // Run the application event loop
    //return 0;
    return QApplication::exec();
}