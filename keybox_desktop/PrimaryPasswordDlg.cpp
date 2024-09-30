//
// Created by tongl on 3/1/2024.
//

#include "CPrimaryPasswordDlg.h"
#include <memory>

using namespace std;
extern CRandomGenerator g_RG;

CPrimaryPasswordDlg::CPrimaryPasswordDlg(PBKDF2_256_PARAMETERS _pbkdf2, QWidget *parent) : m_pbkdf2_paras(_pbkdf2) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);

    m_pwdBox = new CPasswordBox(nullptr, "password", g_RG, false, false);

    rootLayout->addWidget(m_pwdBox);
    QHBoxLayout *buttonLine = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    okButton->setDefault(true);
    buttonLine->addWidget(okButton);
    buttonLine->addWidget(cancelButton);

    rootLayout->addLayout(buttonLine);

    this->setWindowFlag(Qt::WindowTitleHint, true);
    QObject::connect(okButton, &QPushButton::clicked, this, &CPrimaryPasswordDlg::onOK);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

CMaskedBlob CPrimaryPasswordDlg::GetPassword() {
    return m_pwd;
}

void CPrimaryPasswordDlg::onOK() {
    m_pwd = m_pwdBox->GetPassword();
    CCipherEngine cipherEngine;
    vector<unsigned char> keybuff;
    cipherEngine.PBKDF2DerivativeKey(m_pwd.Show(), m_pbkdf2_paras, keybuff);
    m_pwd.Set(keybuff, g_RG.GetNextBytes(keybuff.size()));
    QDialog::accept();
}
