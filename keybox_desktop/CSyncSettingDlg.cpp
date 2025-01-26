//
// Created by sttony on 1/24/25.
//

#include <QVBoxLayout>
#include "CSyncSettingDlg.h"



CSyncSettingDlg::CSyncSettingDlg(QWidget *parent) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);


    QHBoxLayout * emailLine = new QHBoxLayout;
    m_emailBox  = new QLineEdit();
    emailLine->addWidget(new QLabel("Email"));
    emailLine->addWidget(m_emailBox);
    rootLayout->addLayout(emailLine);


    QHBoxLayout * syncUrlLine = new QHBoxLayout;
    m_syncUrlBox= new QLineEdit();
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
    QObject::connect(m_saveBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void CSyncSettingDlg::onSave() {

}