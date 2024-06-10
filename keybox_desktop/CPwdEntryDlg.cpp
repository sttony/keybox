//
// Created by tongl on 2/15/2024.
//

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include "CPwdEntryDlg.h"
#include "../utilities/InitGlobalRG.h"

#include "CPasswordBox.h"

using namespace std;


QHBoxLayout *CPwdEntryDlg::createInputLine(const QString &label, QLineEdit *inputWidget) {
    QHBoxLayout *inputLine = new QHBoxLayout;
    inputLine->addWidget(new QLabel(label));
    inputLine->addWidget(inputWidget);
    inputLine->setStretch(0, 1);
    inputLine->setStretch(1, 4);
    return inputLine;
}


CPwdEntryDlg::CPwdEntryDlg(const std::vector<CPwdGroup>& groups, QWidget *parent) {
    init(groups, parent);
}

void CPwdEntryDlg::onOK() {
    m_PwdEntry.SetTitle(m_title_box->text().toStdString());
    m_PwdEntry.SetUrl(m_url_box->text().toStdString());
    m_PwdEntry.SetUserName(m_user_name_box->text().toStdString());
    m_PwdEntry.SetPassword(m_pwd_box->GetPassword());
    m_PwdEntry.SetNote(m_note_box->GetPassword());
    m_PwdEntry.SetGroup(m_groups.at(m_group_box->currentIndex()).GetUUID());

    QDialog::accept();
}

const CPwdEntry &CPwdEntryDlg::GetPwdEntry() {
    return m_PwdEntry;
}

CPwdEntryDlg::CPwdEntryDlg(const std::vector<CPwdGroup>& groups, const CPwdEntry& pwdEntry, QWidget *parent) {
    m_PwdEntry = pwdEntry;
    init(groups, parent);
}

void CPwdEntryDlg::init(const std::vector<CPwdGroup>& groups, QWidget *parent) {
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

    m_pwd_box = new CPasswordBox(nullptr, "Password", g_RG, m_PwdEntry.GetPwd(), false, false);

    rootLayout->addWidget(m_pwd_box);
    m_note_box = new CPasswordBox(nullptr, "Note", g_RG, m_PwdEntry.GetN(), true, false);
    rootLayout->addWidget(m_note_box);

    m_group_box = new QComboBox;
    m_groups = groups;
    int group_idx = 0;
    for(int i = 0; i< m_groups.size(); ++i){
        auto group = m_groups.at(i);
        m_group_box->addItem( group.GetName().c_str());
        if( group.GetUUID() == m_PwdEntry.GetGroup()){
            group_idx = i;
        }
    }
    m_group_box->setCurrentIndex(group_idx);
    rootLayout->addWidget(m_group_box);

    QHBoxLayout *buttonLine = new QHBoxLayout;
    buttonLine->addWidget(m_ok_button);
    buttonLine->addWidget(m_cancel_button);
    rootLayout->addLayout(buttonLine);

    QObject::connect(m_ok_button, &QPushButton::clicked, this, &CPwdEntryDlg::onOK);
    QObject::connect(m_cancel_button, &QPushButton::clicked, this, &QDialog::reject);
    this->setLayout(rootLayout);
}
