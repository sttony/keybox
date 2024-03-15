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
#include "CKBModel.h"

class MainWindow  : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    void newFile();
    void saveFile();
    void openFile();
    void newEntry();
    void Lock();
    void openPasswordGenerator();

private:
    void createActions();
    void createMenus();
    void createToolbar();

    QAction* m_newFileAction;
    QAction* m_saveFileAction;
    QAction* m_openFileAction;
    QAction* m_newEntryAction;
    QAction* m_lockAction;
    QAction* m_passwordGeneratorAction;

    QMenu *fileMenu;
    QMenu *entryMenu;
    QMenu *toolMenu;
    QToolBar* m_toolbar;

    QTableView* m_entry_table_view;

    CKBModel* m_pModel = nullptr;

    void AddToolBarButton(QAction* p);
};


#endif //KEYBOX_MAINWINDOW_H
