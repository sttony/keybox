//
// Created by tongl on 1/2/2024.
//

#ifndef KEYBOX_MAINWINDOW_H
#define KEYBOX_MAINWINDOW_H

#include <QtGlobal>
#include <QMainWindow>
#include <QActionGroup>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QTreeWidgetItem>
#include "CKBModel.h"

class MainWindow  : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    void newFile();

private:
    void createActions();
    void createMenus();

    QAction *newAct;

private:
    QMenu *fileMenu;
    CKBModel model;
};


#endif //KEYBOX_MAINWINDOW_H
