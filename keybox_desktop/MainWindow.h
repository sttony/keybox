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
#include "CKBModel.h"

class MainWindow  : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    void newFile();
    void Lock();

private:
    void createActions();
    void createMenus();
    void createToolbar();

    QAction* m_newFileAction;
    QAction* m_lockAction;

    QMenu *fileMenu;
    QToolBar* m_toolbar;

private:
    std::unique_ptr<CKBModel> m_pModel;
};


#endif //KEYBOX_MAINWINDOW_H
