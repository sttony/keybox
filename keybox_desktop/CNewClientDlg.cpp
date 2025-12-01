
#include "CNewClientDlg.h"

#include <QMessageBox>
#include <algorithm>
#include <cctype>

#include "utilities/Base64Coder.h"

extern CRandomGenerator g_RG;

using namespace std;

CNewClientDlg::CNewClientDlg(QWidget *parent) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);

    // Allow multi-line input so users can paste UUID-like grouped codes across lines
    // Set doesShow=true so the field is editable without toggling visibility first
    m_pwdBox = new CPasswordBox(nullptr, "New client code", g_RG, true, true);

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
    // Normalize input: remove whitespace and dashes so users can enter UUID-style chunks
    std::string raw = m_pwdBox->GetPassword().Show();
    raw.erase(std::remove_if(raw.begin(), raw.end(), [](unsigned char ch){
        return std::isspace(ch) || ch=='-';
    }), raw.end());
    vector<unsigned char> code = hex_to_bytes(raw);
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

