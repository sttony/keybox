//
// Created by tongl on 1/2/2024.
//

#include <QSplitter>
#include <QTextEdit>
#include <QListView>

#include <QToolButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include "MainWindow.h"
#include "CKBModel.h"
#include "PrimaryPasswordDlg.h"
#include "EntryDlg.h"
#include "utilities/error_code.h"

using namespace std;

extern RandomGenerator g_RG;

MainWindow::MainWindow(){

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
    createToolbar();

}

void MainWindow::createToolbar() {// toolbar
    m_toolbar = addToolBar("toolbar");
    AddToolBarButton(m_newFileAction);
    AddToolBarButton(m_newEntryAction);
    AddToolBarButton(m_lockAction);


    QLineEdit* searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("Search...");
    m_toolbar->addWidget(searchBox);
}

void MainWindow::AddToolBarButton(QAction* action) {
    assert(action);
    QToolButton *toolButton = new QToolButton(this);
    toolButton->setDefaultAction(action);
    m_toolbar->addWidget(toolButton);
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_newFileAction);
    fileMenu->addAction(m_saveFileAction);
    fileMenu->addAction(m_openFileAction);

    entryMenu = menuBar()->addMenu(tr("&Entry"));
    entryMenu->addAction(m_newEntryAction);


    toolMenu = menuBar()->addMenu(tr("&Tools"));
    toolMenu->addAction(m_passwordGeneratorAction);

    fileMenu = menuBar()->addMenu(tr("&Help"));



}

void MainWindow::createActions() {
    m_newFileAction = new QAction(QIcon(":img/img/doc.badge.plus.svg"),tr("&New"), this);
    m_newFileAction->setShortcuts(QKeySequence::New);
    m_newFileAction->setStatusTip(tr("Create a new file"));
    connect(m_newFileAction, &QAction::triggered, this, &MainWindow::newFile);

    m_saveFileAction = new QAction(tr("&Save"), this);
    m_saveFileAction->setShortcut(QKeySequence::Save);
    m_saveFileAction->setStatusTip(tr("Save the current file"));
    connect(m_saveFileAction, &QAction::triggered, this, &MainWindow::saveFile);

    m_openFileAction = new QAction(tr("&Open"), this);
    m_openFileAction->setShortcut(QKeySequence::Open);
    m_openFileAction->setStatusTip(tr("Open a exiting kb file"));
    connect(m_openFileAction, &QAction::triggered, this, &MainWindow::openFile);

    m_newEntryAction = new QAction(QIcon(":img/img/person.badge.key.svg"), tr("&Add Entry"), this);
    connect(m_newEntryAction, &QAction::triggered, this, &MainWindow::newEntry);

    m_passwordGeneratorAction = new QAction(tr("Open Password &Generator"), this);
    connect(m_newEntryAction, &QAction::triggered, this, &MainWindow::openPasswordGenerator);

    m_lockAction = new QAction(QIcon(":img/img/lock.svg"), "Lock", this);
    connect(m_lockAction, &QAction::triggered, this, &MainWindow::Lock);
}

void MainWindow::Lock() {

}

void MainWindow::newEntry() {
    EntryDlg ev;
    if(ev.exec()) {
        m_pModel->AddEntry(ev.GetPwdEntry());
    }
}

void MainWindow::saveFile() {
    assert(m_pModel != nullptr);
    if( m_pModel->GetFilePath().empty()){
        QFileDialog dialog;
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setWindowTitle("Select a file");

        if (dialog.exec()) {
            // Get the selected file path(s)
            QStringList fileNames = dialog.selectedFiles();
            if(fileNames.empty()){
                return;
            }

            m_pModel->SetFilePath(fileNames.at(0).toStdString());
        }
    }
    if( m_pModel->GetFilePath().empty()){
        QMessageBox::information(nullptr, "Alert", "Please select a save path");
        return;
    }
    vector<unsigned char> buff;
    uint32_t cbRealsize = 0;
    m_pModel->Serialize(nullptr, 0, cbRealsize);
    buff.resize(cbRealsize);

    m_pModel->Serialize(buff.data(), buff.size(), cbRealsize);

    std::ofstream ofs(m_pModel->GetFilePath(), std::ios::binary); // Open file in binary mode

    if (!ofs.is_open()) {
        QMessageBox::information(nullptr, "Alert", "Failed to open file");
        return;
    }
    ofs.write(reinterpret_cast<const char*>(buff.data()), buff.size());

    // Check if writing was successful
    if (!ofs.good()) {
        QMessageBox::information(nullptr, "Alert", "Failed to write file");
        return;
    }

    ofs.close();

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

void MainWindow::openFile() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setWindowTitle("Select a file");

    if (dialog.exec()) {
        // Get the selected file path(s)
        QStringList fileNames = dialog.selectedFiles();
        if(fileNames.empty()){
            return;
        }
        auto newModel = new CKBModel;
        newModel->SetFilePath(fileNames.at(0).toStdString());
        newModel->LoadKBHeader(fileNames.at(0).toStdString());

        PrimaryPasswordDlg ppdlg(newModel->GetKeyDerivateParameters());
        ppdlg.exec();
        newModel->SetPrimaryKey(ppdlg.GetPassword());

        newModel->LoadPayload();



        m_entry_table_view->setModel(newModel);
        delete m_pModel;
        m_pModel = newModel;
        m_entry_table_view->repaint();
    }
}

void MainWindow::openPasswordGenerator() {

}
