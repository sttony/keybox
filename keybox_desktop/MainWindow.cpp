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

MainWindow::MainWindow() {

    QSplitter *splitter = new QSplitter(this);
    QTextEdit *leftTextEdit = new QTextEdit();
    m_entry_table_view = new QTableView;
    m_entry_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_entry_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_entry_table_view, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onTableRowDoubleClicked(const QModelIndex&)));
    m_entry_table_view->setSortingEnabled(true);

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
    AddToolBarButton(m_saveFileAction);
    AddToolBarButton(m_lockAction);


    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("Search...");
    connect(m_searchBox, SIGNAL(textChanged(const QString &)), this, SLOT(onSearchTextChange(const QString &)));
    m_toolbar->addWidget(m_searchBox);
}

void MainWindow::AddToolBarButton(QAction *action) {
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
    m_newFileAction = new QAction(QIcon(":img/img/doc.badge.plus.svg"), tr("&New"), this);
    m_newFileAction->setShortcuts(QKeySequence::New);
    m_newFileAction->setStatusTip(tr("Create a new file"));
    connect(m_newFileAction, &QAction::triggered, this, &MainWindow::newFile);

    m_saveFileAction = new QAction(QIcon(":img/img/square.and.arrow.down.on.square.svg"), tr("&Save"), this);
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

    this->RefreshActionEnabled();

}

void MainWindow::Lock() {

}

void MainWindow::newEntry() {
    EntryDlg ev;
    if (ev.exec()) {
        m_pModel->AddEntry(ev.GetPwdEntry());
    }
}

void MainWindow::saveFile() {
    assert(m_pModel != nullptr);
    if (m_pModel->GetFilePath().empty()) {
        QFileDialog dialog;
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setWindowTitle("Select a file");

        if (dialog.exec()) {
            // Get the selected file path(s)
            QStringList fileNames = dialog.selectedFiles();
            if (fileNames.empty()) {
                return;
            }

            m_pModel->SetFilePath(fileNames.at(0).toStdString());
        }
    }
    if (m_pModel->GetFilePath().empty()) {
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
    ofs.write(reinterpret_cast<const char *>(buff.data()), buff.size());

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
    this->RefreshActionEnabled();
}

void MainWindow::openFile() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setWindowTitle("Select a file");

    if (dialog.exec()) {
        // Get the selected file path(s)
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.empty()) {
            return;
        }
        auto newModel = new CKBModel;
        newModel->SetFilePath(fileNames.at(0).toStdString());
        this->setWindowTitle(fileNames.at(0));
        newModel->LoadKBHeader(fileNames.at(0).toStdString());

        PrimaryPasswordDlg ppdlg(newModel->GetKeyDerivateParameters());
        ppdlg.exec();
        newModel->SetPrimaryKey(ppdlg.GetPassword());

        if( newModel->LoadPayload()){
            QMessageBox::information(nullptr, "Alert", "Seems like the password is not correct");
            delete newModel;
            return;
        };

        m_entry_table_view->setModel(newModel);
        delete m_pModel;
        m_pModel = newModel;
        m_entry_table_view->repaint();
        this->RefreshActionEnabled();
    }
}

void MainWindow::openPasswordGenerator() {

}

void MainWindow::RefreshActionEnabled() {
    if (m_saveFileAction)
        m_saveFileAction->setEnabled(m_pModel != nullptr);
    if (m_newEntryAction)
        m_newEntryAction->setEnabled(m_pModel != nullptr);
    if (m_lockAction)
        m_lockAction->setEnabled(m_pModel != nullptr);
}

void MainWindow::onTableRowDoubleClicked(const QModelIndex &index) {
    if (index.isValid()) {
        int row = index.row();
        EntryDlg entryDlg(m_pModel->GetEntry(row));
        if(entryDlg.exec()){
            m_pModel->SetEntry(entryDlg.GetPwdEntry(), row);
        }
    }
}

void MainWindow::onSearchTextChange(const QString& _filter) {
    m_pModel->SetFilter(_filter);
}
