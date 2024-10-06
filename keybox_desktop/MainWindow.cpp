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
#include "CPrimaryPasswordDlg.h"
#include "CPwdEntryDlg.h"
#include "utilities/error_code.h"
#include "CPwdGeneratorDlg.h"
#include "CSettings.h"
#include "CPwdGroupDlg.h"

using namespace std;

extern CRandomGenerator g_RG;
extern CSettings g_Settings;

MainWindow::MainWindow() {

    QSplitter *splitter = new QSplitter(this);

    m_entry_table_view = new CPwdEntryTableView;
    m_entry_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_entry_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_entry_table_view, &QTableView::doubleClicked, this, &MainWindow::onTableRowDoubleClicked);
    m_entry_table_view->setSortingEnabled(true);

    m_group_list_view = new CPwdGroupListView;
    m_group_model = new QStringListModel;
    m_group_list_view->setModel(m_group_model);
    connect(m_group_list_view, &QListView::clicked, this, &MainWindow::onClickGroup);

    splitter->addWidget(m_group_list_view);
    splitter->addWidget(m_entry_table_view);
    QList<int> sizes;
    sizes << 200 << 800;
    splitter->setSizes(sizes);
    this->setCentralWidget(splitter);
    CreateActions();
    CreateMenus();
    resize(800, 600);
    CreateToolbar();

    RefreshActionEnabled();
    string last_file_path = g_Settings.GetLastKeyboxFilepath();
    if (!last_file_path.empty()) {
        while(OpenFile(last_file_path));
    }
}

void MainWindow::CreateToolbar() {// toolbar
    m_toolbar = addToolBar("toolbar");
    AddToolBarButton(m_newFileAction);
    AddToolBarButton(m_newEntryAction);
    AddToolBarButton(m_saveFileAction);
    AddToolBarButton(m_newGroupAction);
    AddToolBarButton(m_lockAction);


    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("Search...");
    connect(m_searchBox, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChange);
    m_toolbar->addWidget(m_searchBox);
}

void MainWindow::AddToolBarButton(QAction *action) {
    assert(action);
    QToolButton *toolButton = new QToolButton(this);
    toolButton->setDefaultAction(action);
    m_toolbar->addWidget(toolButton);
}

void MainWindow::CreateMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_newFileAction);
    fileMenu->addAction(m_saveFileAction);
    fileMenu->addAction(m_openFileAction);

    entryMenu = menuBar()->addMenu(tr("&Entry"));
    entryMenu->addAction(m_newEntryAction);

    groupMenu = menuBar()->addMenu(tr("&Group"));
    groupMenu->addAction(m_newGroupAction);

    toolMenu = menuBar()->addMenu(tr("&Tools"));
    toolMenu->addAction(m_passwordGeneratorAction);

    fileMenu = menuBar()->addMenu(tr("&Help"));


}

void MainWindow::CreateActions() {
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

    m_newGroupAction = new QAction(QIcon(":img/img/folder.svg"), tr("Add Group"), this);
    connect(m_newGroupAction, &QAction::triggered, this, &MainWindow::newGroup);

    m_passwordGeneratorAction = new QAction(tr("Open Password &Generator"), this);
    connect(m_passwordGeneratorAction, &QAction::triggered, this, &MainWindow::openPasswordGenerator);

    m_lockAction = new QAction(QIcon(":img/img/lock.svg"), "lockFile", this);
    connect(m_lockAction, &QAction::triggered, this, &MainWindow::lockFile);

    this->RefreshActionEnabled();

}

void MainWindow::lockFile() {

}

void MainWindow::newEntry() {
    assert(m_pModel);
    CPwdEntryDlg ev( m_pModel->GetGroups());
    if (ev.exec()) {
        m_pModel->AddEntry(ev.GetPwdEntry());
    }
}

void MainWindow::newGroup() {
    assert(m_pModel);
    CPwdGroupDlg dlg(m_pModel);
    dlg.exec();
}

void MainWindow::onClickGroup(const QModelIndex& index) {
    assert(m_pModel);
    m_pModel->SetFilter(m_pModel->GetGroups().at(index.row()).GetUUID());
}

void MainWindow::saveFile() {
    assert(m_pModel != nullptr);
    if (m_pModel->GetFilePath().empty()) {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Save to",
                                                        QDir::home().filePath("keybox.kbx"),
                                                        "" );
        if(!fileName.isEmpty()){
            m_pModel->SetFilePath(fileName.toStdString());
            this->setWindowTitle(fileName);
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

    g_Settings.SetLastKeyboxFilepath(m_pModel->GetFilePath());
    g_Settings.Save();

}

void MainWindow::newFile() {
    auto newModel = new CKBModel;
    vector<unsigned char> randomv32;
    g_RG.GetNextBytes(32, randomv32);
    newModel->SetKeyDerivateParameters(randomv32);

    CPrimaryPasswordDlg ppdlg(newModel->GetKeyDerivateParameters());
    if (ppdlg.exec()) {
        newModel->SetPrimaryKey(ppdlg.GetPassword());
        m_entry_table_view->setModel(newModel);
        ResetGroup(newModel);
        delete m_pModel;
        m_pModel = newModel;
        this->RefreshActionEnabled();
    }
}

void MainWindow::ResetGroup(CKBModel *newModel) {
    QStringList groupList;
    for (const auto &group: newModel->GetGroups()) {
        groupList << group.GetName().c_str();

    }
    this->m_group_model->setStringList(groupList);
    this->m_group_list_view->repaint();
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
        auto file_path = fileNames.at(0).toStdString();
        while(OpenFile(file_path)){

        }

    }
}

int MainWindow::OpenFile(const std::string &file_path) {
    auto newModel = new CKBModel;
    newModel->SetFilePath(file_path);
    newModel->LoadKBHeader(file_path);
    CPrimaryPasswordDlg ppdlg(newModel->GetKeyDerivateParameters());
    ppdlg.setWindowTitle( file_path.c_str());
    ppdlg.setMinimumWidth(file_path.size() * 16);

    if( ppdlg.exec() ) {
        newModel->SetPrimaryKey(ppdlg.GetPassword());

        if (newModel->LoadPayload()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Open file failed");
            msgBox.setText("Seems like the password is not correct");
            QPushButton *customProceedButton = msgBox.addButton("Retry", QMessageBox::AcceptRole);
            QPushButton *customCancelButton = msgBox.addButton("Abort", QMessageBox::RejectRole);
            msgBox.exec();
            delete newModel;
            if (msgBox.clickedButton() == customProceedButton) {
                return 1;
            } else if (msgBox.clickedButton() == customCancelButton) {
                return 0;
            }
            else{
                return 0;
            }
        };
        m_entry_table_view->setModel(newModel);
        ResetGroup(newModel);
        delete m_pModel;
        m_pModel = newModel;
        m_entry_table_view->repaint();
        RefreshActionEnabled();
        setWindowTitle(file_path.c_str());
    }
    else{
        delete newModel;
    }
}

void MainWindow::openPasswordGenerator() {
    CPwdGeneratorDlg dlg;
    dlg.exec();
}

void MainWindow::RefreshActionEnabled() {
    if (m_saveFileAction)
        m_saveFileAction->setEnabled(m_pModel != nullptr);
    if (m_newEntryAction)
        m_newEntryAction->setEnabled(m_pModel != nullptr);
    if (m_lockAction)
        m_lockAction->setEnabled(m_pModel != nullptr);
    if (m_searchBox) {
        m_searchBox->setEnabled(m_pModel != nullptr);
    }
    if (m_newGroupAction) {
        m_newGroupAction->setEnabled(m_pModel != nullptr);
    }
}

void MainWindow::onTableRowDoubleClicked(const QModelIndex &index) {
    if (index.isValid()) {
        int row = index.row();
        CPwdEntryDlg entryDlg(m_pModel->GetGroups(), m_pModel->GetEntry(row));
        if (entryDlg.exec()) {
            m_pModel->SetEntry(entryDlg.GetPwdEntry(), row);
        }
    }
}

void MainWindow::onSearchTextChange(const QString &_filter) {
    m_pModel->SetFilter(_filter);
}
