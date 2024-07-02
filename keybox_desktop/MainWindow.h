//
// Created by tongl on 1/2/2024.
//

#ifndef KEYBOX_MAINWINDOW_H
#define KEYBOX_MAINWINDOW_H

#include <memory>
#include <QtGlobal>
#include <QMainWindow>
#include <QActionGroup>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QTableView>
#include <QStringListModel>
#include "CKBModel.h"
#include "CPwdEntryTableView.h"
#include "CPwdGroupListView.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow();

private slots:

    void newFile();

    void saveFile();

    void openFile();

    void newEntry();

    void lockFile();

    void openPasswordGenerator();

    void onTableRowDoubleClicked(const QModelIndex &index);

    void onSearchTextChange(const QString &);

    void newGroup();

    void onClickGroup(const QModelIndex& index);

private:
    void CreateActions();

    void CreateMenus();

    void CreateToolbar();

    void ResetGroup(CKBModel *newModel);


    QAction *m_newFileAction = nullptr;
    QAction *m_saveFileAction = nullptr;
    QAction *m_openFileAction = nullptr;
    QAction *m_newEntryAction = nullptr;
    QAction *m_newGroupAction = nullptr;
    QAction *m_lockAction = nullptr;
    QAction *m_passwordGeneratorAction = nullptr;

    QMenu *fileMenu = nullptr;
    QMenu *entryMenu = nullptr;
    QMenu *groupMenu = nullptr;
    QMenu *toolMenu = nullptr;
    QToolBar *m_toolbar = nullptr;
    QLineEdit* m_searchBox = nullptr;

    CPwdEntryTableView *m_entry_table_view = nullptr ;
    CPwdGroupListView *m_group_list_view = nullptr;
    QStringListModel *m_group_model = nullptr;

    CKBModel *m_pModel = nullptr;

    void AddToolBarButton(QAction *p);

    void RefreshActionEnabled();

    void OpenFile(const std::string&);
};


#endif //KEYBOX_MAINWINDOW_H
