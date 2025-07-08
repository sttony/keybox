
#ifndef CNEWCLIENTDLG_H
#define CNEWCLIENTDLG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include "CPasswordBox.h"

class CNewClientDlg : public QDialog {
Q_OBJECT
public:
    explicit CNewClientDlg(QWidget *parent = nullptr);
    CMaskedBlob GetPassword();

private slots:

    void onOK();

private:
    CPasswordBox *m_pwdBox;
    CMaskedBlob m_pwd;
};
#endif //CNEWCLIENTDLG_H
