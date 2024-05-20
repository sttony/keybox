//
// Created by tongl on 3/1/2024.
//

#ifndef KEYBOX_CPRIMARYPASSWORDDLG_H
#define KEYBOX_CPRIMARYPASSWORDDLG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include "CPasswordBox.h"
#include "../utilities/CipherEngine.h"

class CPrimaryPasswordDlg : public QDialog {
Q_OBJECT
public:
    explicit CPrimaryPasswordDlg(PBKDF2_256_PARAMETERS _pkdf2_paras, QWidget *parent = nullptr);

    CMaskedBlob GetPassword();

private slots:

    void onOK();

private:
    CPasswordBox *m_pwdBox;
    CMaskedBlob m_pwd;
    PBKDF2_256_PARAMETERS m_pbkdf2_paras;
};


#endif //KEYBOX_CPRIMARYPASSWORDDLG_H
