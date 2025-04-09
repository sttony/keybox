//
// Created by sttony on 1/24/25.
//

#include <QVBoxLayout>
#include "CSyncSettingDlg.h"



CSyncSettingDlg::CSyncSettingDlg(CKBModel* pModel, QWidget *parent) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    m_kbModel = pModel;

    QHBoxLayout * emailLine = new QHBoxLayout;
    m_emailBox  = new QLineEdit(m_kbModel->GetEmail().c_str());
    emailLine->addWidget(new QLabel("Email"));
    emailLine->addWidget(m_emailBox);
    rootLayout->addLayout(emailLine);


    QHBoxLayout * syncUrlLine = new QHBoxLayout;
    m_syncUrlBox= new QLineEdit(m_kbModel->GetSyncUrl().c_str());
    syncUrlLine->addWidget(new QLabel("Sync server"));
    syncUrlLine->addWidget(m_syncUrlBox);
    rootLayout->addLayout(syncUrlLine);


    QHBoxLayout *btnLine2 = new QHBoxLayout();
    m_saveBtn = new QPushButton("Save");
    m_cancelBtn = new QPushButton("Cancel");
    btnLine2->addWidget(m_saveBtn);
    btnLine2->addWidget(m_cancelBtn);
    rootLayout->addLayout(btnLine2);
    setLayout(rootLayout);

    connect(m_saveBtn, &QPushButton::clicked, this, &CSyncSettingDlg::onSave);
    QObject::connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void CSyncSettingDlg::onSave() {
    m_kbModel->SetSyncUrl(m_syncUrlBox->text().toStdString());
    m_kbModel->SetEmail(m_emailBox->text().toStdString());
}