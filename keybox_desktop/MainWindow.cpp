//
// Created by tongl on 1/2/2024.
//

#include "MainWindow.h"

MainWindow::MainWindow() {
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
