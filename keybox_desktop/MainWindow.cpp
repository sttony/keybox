//
// Created by tongl on 1/2/2024.
//

#include <QSplitter>
#include <QTextEdit>
#include <QListView>
#include <QTableView>
#include <QToolButton>
#include <QLineEdit>
#include <QFileDialog>
#include "MainWindow.h"
#include "CKBModel.h"
#include "PrimaryPasswordDlg.h"

using namespace std;

extern RandomGenerator g_RG;

MainWindow::MainWindow(){
    createToolbar();
    QSplitter* splitter = new QSplitter(this);
    QTextEdit* leftTextEdit = new QTextEdit();
    QTableView* view = new QTableView;
    QStringList list;
    list << "Item 1" << "Item 2" << "Item 3";
    //view->setModel(&model);

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

void MainWindow::createToolbar() {// toolbar
    m_toolbar = addToolBar("toolbar");
    m_lockAction = new QAction(QIcon(":img/img/lock.svg"), "Lock", this);
    QToolButton* toolButton = new QToolButton(this);
    toolButton->setDefaultAction(m_lockAction);
    connect(m_lockAction, &QAction::triggered, this, &MainWindow::Lock);
    m_toolbar->addWidget(toolButton);
    QLineEdit* searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("Search...");
    m_toolbar->addWidget(searchBox);
}


void MainWindow::newFile() {
    m_pModel = make_unique<CKBModel>(nullptr);

    PBKDF2_256_PARAMETERS pbkdf2256Parameters;
    vector<unsigned char> randomv32;
    g_RG.GetNextBytes(32, randomv32);


    PrimaryPasswordDlg ppdlg(pbkdf2256Parameters);
    ppdlg.exec();
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_newFileAction);

    fileMenu = menuBar()->addMenu(tr("&Entry"));
    fileMenu->addAction(m_newEntryAction);


    fileMenu = menuBar()->addMenu(tr("&Tools"));

    fileMenu = menuBar()->addMenu(tr("&Help"));



}

void MainWindow::createActions() {
    m_newFileAction = new QAction(tr("&New"), this);
    m_newFileAction->setShortcuts(QKeySequence::New);
    m_newFileAction->setStatusTip(tr("Create a new file"));
    connect(m_newFileAction, &QAction::triggered, this, &MainWindow::newFile);

    m_newEntryAction = new QAction(tr("&Add Entry"), this);
}

void MainWindow::Lock() {

}
