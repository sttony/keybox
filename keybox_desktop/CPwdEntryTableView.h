//
// Created by tongl on 3/22/2024.
//

#ifndef KEYBOX_CPWDENTRYTABLEVIEW_H
#define KEYBOX_CPWDENTRYTABLEVIEW_H


#include <QTableView>
#include <QKeyEvent>

class CPwdEntryTableView : public QTableView {
public:
    explicit CPwdEntryTableView(QWidget *parent = nullptr) : QTableView(parent) {}

protected:
    void keyPressEvent(QKeyEvent *event) override;
};


#endif //KEYBOX_CPWDENTRYTABLEVIEW_H
