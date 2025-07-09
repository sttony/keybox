
#include "CNewClientDlg.h"

#include <QMessageBox>

#include "utilities/Base64Coder.h"

extern CRandomGenerator g_RG;

using namespace std;

CNewClientDlg::CNewClientDlg(QWidget *parent) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);

    m_pwdBox = new CPasswordBox(nullptr, "New client code", g_RG, false, false);

    rootLayout->addWidget(m_pwdBox);
    QHBoxLayout *buttonLine = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    okButton->setDefault(true);
    buttonLine->addWidget(okButton);
    buttonLine->addWidget(cancelButton);

    rootLayout->addLayout(buttonLine);

    this->setWindowFlag(Qt::WindowTitleHint, true);
    QObject::connect(okButton, &QPushButton::clicked, this, &CNewClientDlg::onOK);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}


CMaskedBlob CNewClientDlg::GetKey() {
    return m_pwd;
}

CMaskedBlob CNewClientDlg::GetIV() {
    return m_iv;
}

void CNewClientDlg::onOK() {
    vector<unsigned char> code = hex_to_bytes(m_pwdBox->GetPassword().Show());
    if ( code.size() != 32 + 16 ) {
        QMessageBox::information(nullptr, "Alert", "Code is not 96 chars");
        QDialog::reject();
        return;
    }
    vector<unsigned char> pwd (code.begin(), code.begin() + 32);
    vector<unsigned char> iv (code.begin() + 32, code.end());
    m_pwd.Set(pwd, vector<unsigned char>(pwd.size()));
    m_iv.Set(iv, vector<unsigned char>(iv.size()));
    QDialog::accept();
}

