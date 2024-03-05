//
// Created by tongl on 3/1/2024.
//

#include "PrimaryPasswordDlg.h"
#include <memory>

using namespace std;
extern RandomGenerator g_RG;

PrimaryPasswordDlg::PrimaryPasswordDlg(QWidget *parent) {
    QVBoxLayout* rootLayout = new QVBoxLayout (this);

    m_pwdBox = new PasswordBox(nullptr, "password",  make_shared<CMaskedBlob>(m_pwd), g_RG, false, true);


    rootLayout->addWidget(m_pwdBox);
    QHBoxLayout* buttonLine = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel",  this);
    buttonLine->addWidget(okButton);
    buttonLine->addWidget(cancelButton);

    rootLayout->addLayout(buttonLine);

    QObject::connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

CMaskedBlob PrimaryPasswordDlg::GetPassword() {
    return m_pwd;
}
