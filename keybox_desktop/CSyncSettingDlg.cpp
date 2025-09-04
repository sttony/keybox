//
// Created by sttony on 1/24/25.
//

#include <QVBoxLayout>
#include "CSyncSettingDlg.h"

#include <QMessageBox>

#include "CNewClientDlg.h"
#include "CPrimaryPasswordDlg.h"
#include "utilities/CRequest.h"

using namespace std;
CSyncSettingDlg::CSyncSettingDlg(CKBModel* pModel, QWidget *parent) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    m_kbModel = pModel;

    QHBoxLayout * emailLine = new QHBoxLayout;
    m_emailBox  = new QLineEdit(m_kbModel? m_kbModel->GetEmail().c_str(): "");
    emailLine->addWidget(new QLabel("Email"));
    emailLine->addWidget(m_emailBox);
    rootLayout->addLayout(emailLine);


    QHBoxLayout * syncUrlLine = new QHBoxLayout;
    m_syncUrlBox= new QLineEdit(m_kbModel? m_kbModel->GetSyncUrl().c_str() : "");
    syncUrlLine->addWidget(new QLabel("Sync server"));
    syncUrlLine->addWidget(m_syncUrlBox);
    rootLayout->addLayout(syncUrlLine);


    QHBoxLayout *btnLine2 = new QHBoxLayout();
    m_registerBtn = new QPushButton("Register new email");
    m_cancelBtn = new QPushButton("Cancel");
    m_setNewClientBtn = new QPushButton("Set new client");
    btnLine2->addWidget(m_registerBtn);
    btnLine2->addWidget(m_setNewClientBtn);
    btnLine2->addWidget(m_cancelBtn);
    rootLayout->addLayout(btnLine2);
    setLayout(rootLayout);

    connect(m_registerBtn, &QPushButton::clicked, this, &CSyncSettingDlg::onSave);
    connect(m_setNewClientBtn, &QPushButton::clicked, this, &CSyncSettingDlg::onNewClient);
    QObject::connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void CSyncSettingDlg::onSave() {
    m_kbModel->SetSyncUrl(m_syncUrlBox->text().toStdString());
    m_kbModel->SetEmail(m_emailBox->text().toStdString());

    uint32_t result = m_kbModel->Register();
    if ( result == 0){
        QMessageBox::information(nullptr, "Alert", "Register success, please check your email to activate");
        QDialog::accept();
        emit saveSignal();
    }
    else {
        QMessageBox::information(nullptr, "Alert", "Register failed");
        QDialog::reject();
    }
}

void CSyncSettingDlg::onNewClient() {
    if ( m_kbModel == nullptr ) {
        m_kbModel = new CKBModel();
    }
    m_kbModel->SetSyncUrl(m_syncUrlBox->text().toStdString());
    m_kbModel->SetEmail(m_emailBox->text().toStdString());
    vector<unsigned char> encrypted_url;
    uint32_t result = m_kbModel->SetupNewClient(encrypted_url);
    if ( result != 0){
        QMessageBox::information(nullptr, "Alert", "Register failed");
        QDialog::reject();
        return;
    }
    CNewClientDlg dlg;
    if (!dlg.exec() ) {
        return;
    }
    vector<unsigned char> decrypted_buff;
    CCipherEngine cipherEngine;

    result = cipherEngine.AES256EnDecrypt(
        encrypted_url.data(),
        encrypted_url.size(),
        dlg.GetKey().ShowBin(),
        dlg.GetIV().ShowBin(),
        CCipherEngine::AES_CHAIN_MODE_CBC,
        CCipherEngine::AES_PADDING_MODE_PKCS7,
        false,
        decrypted_buff);

    if (result) {
        QMessageBox::information(nullptr, "Alert", "Decrypt url failed");
        QDialog::reject();
        return;
    }

    // download url
    CRequest request( string(reinterpret_cast<const char *>(decrypted_buff.data())));
    request.EnableFollowRedirect();
    request.Send();

    if (request.GetResponseCode() != 200) {
        QMessageBox::information(nullptr, "Alert", "Download url failed");
        QDialog::reject();
        return;
    }

    // load content into model
    m_kbModel->LoadBlobToBuff(request.GetResponsePayload());
    m_kbModel->LoadKBHeader();
    CPrimaryPasswordDlg ppdlg(m_kbModel->GetKeyDerivateParameters());
    if (ppdlg.exec() ) {
        m_kbModel->SetPrimaryKey(ppdlg.GetPassword());
        if (!m_kbModel->LoadPayload() ) {
        }
    }

    QDialog::accept();
}