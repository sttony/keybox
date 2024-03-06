//
// Created by tongl on 3/1/2024.
//

#ifndef KEYBOX_PRIMARYPASSWORDDLG_H
#define KEYBOX_PRIMARYPASSWORDDLG_H
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include "PasswordBox.h"
#include "../utilities/CipherEngine.h"

class PrimaryPasswordDlg : public QDialog {
Q_OBJECT
public:
    explicit PrimaryPasswordDlg(PBKDF2_256_PARAMETERS _pkdf2_paras, QWidget *parent = nullptr);
    CMaskedBlob GetPassword();
private slots:
    void onOK();
private:
    PasswordBox* m_pwdBox;
    CMaskedBlob m_pwd;
    PBKDF2_256_PARAMETERS m_pbkdf2_paras;
};


#endif //KEYBOX_PRIMARYPASSWORDDLG_H
