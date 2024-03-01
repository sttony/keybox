//
// Created by tongl on 1/2/2024.
//

#include <QSplitter>
#include <QTextEdit>
#include <QListView>
#include <QTableView>
#include <QToolButton>
#include "MainWindow.h"
#include "CKBModel.h"

MainWindow::MainWindow():
        model({"item1", "item2"}){
    // toolbar
    m_toolbar = this->addToolBar("toolbar");
    m_lockAction = new QAction(QIcon(":img/img/lock.svg"), "Lock", this);
    QToolButton* toolButton = new QToolButton(this);
//    toolButton->setText("Tool Button");
    toolButton->setDefaultAction(m_lockAction);
    connect(m_lockAction, &QAction::triggered, this, &MainWindow::Lock);
    m_toolbar->addWidget(toolButton);

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
    fileMenu->addAction(m_newFileAction);

}

void MainWindow::createActions() {
    m_newFileAction = new QAction(tr("&New"), this);
    m_newFileAction->setShortcuts(QKeySequence::New);
    m_newFileAction->setStatusTip(tr("Create a new file"));
    connect(m_newFileAction, &QAction::triggered, this, &MainWindow::newFile);
}

void MainWindow::Lock() {

}
