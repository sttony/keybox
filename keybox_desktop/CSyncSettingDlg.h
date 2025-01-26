//
// Created by sttony on 1/24/25.
//

#ifndef KEYBOX_CSYNCSETTINGDLG_H
#define KEYBOX_CSYNCSETTINGDLG_H
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "CKBModel.h"

class CSyncSettingDlg : public QDialog {
Q_OBJECT
public:
    explicit CSyncSettingDlg(CKBModel* pModel, QWidget *parent = nullptr);

private slots:
    void onSave();

private:
    CKBModel* m_kbModel = nullptr;

    QLineEdit * m_emailBox = nullptr;
    QLineEdit * m_syncUrlBox = nullptr;
    QPushButton* m_saveBtn = nullptr;
    QPushButton* m_cancelBtn = nullptr;

};


#endif //KEYBOX_CSYNCSETTINGDLG_H
