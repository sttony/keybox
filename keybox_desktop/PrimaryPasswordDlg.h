//
// Created by tongl on 3/1/2024.
//

#ifndef KEYBOX_PRIMARYPASSWORDDLG_H
#define KEYBOX_PRIMARYPASSWORDDLG_H
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include "PasswordBox.h"

class PrimaryPasswordDlg : public QDialog {
Q_OBJECT
public:
    explicit PrimaryPasswordDlg(QWidget *parent = nullptr);
    CMaskedBlob GetPassword();
private slots:

private:
    PasswordBox* m_pwdBox;
    CMaskedBlob m_pwd;
};


#endif //KEYBOX_PRIMARYPASSWORDDLG_H
