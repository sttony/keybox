//
// Created by tongl on 3/1/2024.
//

#include "PrimaryPasswordDlg.h"
#include <memory>

using namespace std;
extern RandomGenerator g_RG;

PrimaryPasswordDlg::PrimaryPasswordDlg(PBKDF2_256_PARAMETERS _pbkdf2, QWidget *parent) : m_pbkdf2_paras(_pbkdf2) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);

    m_pwdBox = new PasswordBox(nullptr, "password", make_shared<CMaskedBlob>(m_pwd), g_RG, false, true);


    rootLayout->addWidget(m_pwdBox);
    QHBoxLayout *buttonLine = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLine->addWidget(okButton);
    buttonLine->addWidget(cancelButton);

    rootLayout->addLayout(buttonLine);

    QObject::connect(okButton, &QPushButton::clicked, this, &PrimaryPasswordDlg::onOK);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

CMaskedBlob PrimaryPasswordDlg::GetPassword() {
    return m_pwd;
}

void PrimaryPasswordDlg::onOK() {
    CCipherEngine cipherEngine;
    vector<unsigned char> keybuff;
    cipherEngine.PBKDF2DerivativeKey(m_pwd.Show(), m_pbkdf2_paras, keybuff);
    m_pwd.Set(keybuff, g_RG);
    QDialog::accept();
}
