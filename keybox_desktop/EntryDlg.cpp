//
// Created by tongl on 2/15/2024.
//

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include "EntryDlg.h"
#include "../utilities/InitGlobalRG.h"

#include "PasswordBox.h"

using namespace std;


QHBoxLayout *EntryDlg::createInputLine(const QString &label, QLineEdit *inputWidget) {
    QHBoxLayout *inputLine = new QHBoxLayout;
    inputLine->addWidget(new QLabel(label));
    inputLine->addWidget(inputWidget);
    inputLine->setStretch(0, 1);
    inputLine->setStretch(1, 4);
    return inputLine;
}


EntryDlg::EntryDlg(QWidget *parent) {
    init(parent);
}

void EntryDlg::onOK() {
    m_PwdEntry.SetTitle(m_title_box->text().toStdString());
    m_PwdEntry.SetUrl(m_url_box->text().toStdString());
    m_PwdEntry.SetUserName(m_user_name_box->text().toStdString());

    QDialog::accept();
}

const CPwdEntry &EntryDlg::GetPwdEntry() {
    return m_PwdEntry;
}

EntryDlg::EntryDlg(const CPwdEntry& pwdEntry, QWidget *parent) {
    m_PwdEntry = pwdEntry;
    init(parent);
}

void EntryDlg::init(QWidget *parent) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);

    m_title_box = new QLineEdit;
    m_title_box->setText(m_PwdEntry.GetTitle().c_str());
    m_url_box = new QLineEdit;
    m_url_box->setText(m_PwdEntry.GetUrl().c_str());
    m_user_name_box = new QLineEdit;
    m_user_name_box->setText(m_PwdEntry.GetUserName().c_str());

    m_ok_button = new QPushButton("Save");
    m_cancel_button = new QPushButton("Cancel");


    rootLayout->addLayout(createInputLine("Title", m_title_box));
    rootLayout->addLayout(createInputLine("Url", m_url_box));
    rootLayout->addLayout(createInputLine("Username", m_user_name_box));

    m_pwd_box = new PasswordBox(nullptr, "Password", make_shared<CMaskedBlob>(m_PwdEntry.GetPwd()),
                                g_RG, false, false);
    rootLayout->addWidget(m_pwd_box);
    m_note_box = new PasswordBox(nullptr, "Note", make_shared<CMaskedBlob>(m_PwdEntry.GetN()),
                                 g_RG, true, false);
    rootLayout->addWidget(m_note_box);


    QHBoxLayout *buttonLine = new QHBoxLayout;
    buttonLine->addWidget(m_ok_button);
    buttonLine->addWidget(m_cancel_button);
    rootLayout->addLayout(buttonLine);

    QObject::connect(m_ok_button, &QPushButton::clicked, this, &EntryDlg::onOK);
    QObject::connect(m_cancel_button, &QPushButton::clicked, this, &QDialog::reject);
    this->setLayout(rootLayout);
}
