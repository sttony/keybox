//
// Created by tongl on 1/2/2024.
//

#include <QSplitter>
#include <QTextEdit>
#include <QListView>

#include <QToolButton>
#include <QLineEdit>
#include <QFileDialog>
#include "MainWindow.h"
#include "CKBModel.h"
#include "PrimaryPasswordDlg.h"
#include "EntryDlg.h"

using namespace std;

extern RandomGenerator g_RG;

MainWindow::MainWindow(){
    createToolbar();
    QSplitter* splitter = new QSplitter(this);
    QTextEdit* leftTextEdit = new QTextEdit();
    m_entry_table_view = new QTableView;
    //view->setModel(m_pModel.get());

    splitter->addWidget(leftTextEdit);
    splitter->addWidget(m_entry_table_view);
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
    auto newModel = new CKBModel;
    vector<unsigned char> randomv32;
    g_RG.GetNextBytes(32, randomv32);
    newModel->SetKeyDerivateParameters(randomv32);

    PrimaryPasswordDlg ppdlg(newModel->GetKeyDerivateParameters());
    // TODO: check return.
    ppdlg.exec();
    newModel->SetPrimaryKey(ppdlg.GetPassword());

    m_entry_table_view->setModel(newModel);
    delete m_pModel;
    m_pModel = newModel;
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_newFileAction);
    fileMenu->addAction(m_saveFileAction);

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

    m_saveFileAction = new QAction(tr("&Save"), this);
    m_saveFileAction->setShortcut(QKeySequence::Save);
    m_saveFileAction->setStatusTip(tr("Save the current file"));
    connect(m_newFileAction, &QAction::triggered, this, &MainWindow::saveFile);

    m_newEntryAction = new QAction(tr("&Add Entry"), this);
    connect(m_newEntryAction, &QAction::triggered, this, &MainWindow::newEntry);
}

void MainWindow::Lock() {

}

void MainWindow::newEntry() {
    EntryDlg ev;
    int ret = ev.exec();
    m_pModel->AddEntry(ev.GetPwdEntry());
}

void MainWindow::saveFile() {

}
