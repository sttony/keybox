//
// Created by tongl on 1/2/2024.
//

#include <QSplitter>
#include <QTextEdit>
#include <QListView>
#include <QTableView>
#include "MainWindow.h"
#include "CKBModel.h"

MainWindow::MainWindow():
        model({"item1", "item2"}){
    QSplitter* splitter = new QSplitter(this);

    QTextEdit* leftTextEdit = new QTextEdit();
    QTableView* view = new QTableView;


    QStringList list;
    list << "Item 1" << "Item 2" << "Item 3";

    view->setModel(&model);



    splitter->addWidget(leftTextEdit);
    splitter->addWidget(view);
    QList<int> sizes;
    sizes << 200 << 800;
    splitter->setSizes(sizes);
    this->setCentralWidget(splitter);
    createActions();
    createMenus();
    resize(800, 600);
}


void MainWindow::newFile() {

}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);

}

void MainWindow::createActions() {
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
}
