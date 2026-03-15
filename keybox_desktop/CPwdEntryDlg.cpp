//
// Created by tongl on 2/15/2024.
//

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QTemporaryFile>
#include <QFileInfo>
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

void CPwdEntryDlg::onAttach() {
    QString fileName = QFileDialog::getOpenFileName(this, "Attach File");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            std::vector<unsigned char> blob(data.begin(), data.end());
            m_PwdEntry.SetAttachment(blob, g_RG.GetNextBytes(blob.size()));
            m_attachment_status_label->setText("Attached: " + QFileInfo(fileName).fileName());
            m_open_attachment_button->setEnabled(true);
            m_save_attachment_button->setEnabled(true);
        }
    }
}

void CPwdEntryDlg::onOpenAttachment() {
    std::vector<unsigned char> blob = m_PwdEntry.GetAttachment();
    if (blob.empty()) return;

    QTemporaryFile* tempFile = new QTemporaryFile(this);
    // Since we don't have the original filename, we just use a generic one or maybe we could have stored it.
    // For now, let's use "attachment"
    tempFile->setFileTemplate(QDir::tempPath() + "/keybox_attach_XXXXXX");
    if (tempFile->open()) {
        tempFile->write(reinterpret_cast<const char*>(blob.data()), blob.size());
        tempFile->close();
        tempFile->setAutoRemove(true);
        QDesktopServices::openUrl(QUrl::fromLocalFile(tempFile->fileName()));
    }
}

void CPwdEntryDlg::onSaveAttachment() {
    std::vector<unsigned char> blob = m_PwdEntry.GetAttachment();
    if (blob.empty()) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save Attachment As");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reinterpret_cast<const char*>(blob.data()), blob.size());
            file.close();
        }
    }
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

    QHBoxLayout *attachmentLine = new QHBoxLayout;
    m_attachment_status_label = new QLabel;
    if (m_PwdEntry.GetAttachment().empty()) {
        m_attachment_status_label->setText("No attachment");
    } else {
        m_attachment_status_label->setText("Has attachment");
    }
    m_attach_button = new QPushButton("Attach");
    m_open_attachment_button = new QPushButton("Open");
    m_save_attachment_button = new QPushButton("Save As");

    m_open_attachment_button->setEnabled(!m_PwdEntry.GetAttachment().empty());
    m_save_attachment_button->setEnabled(!m_PwdEntry.GetAttachment().empty());

    attachmentLine->addWidget(m_attachment_status_label);
    attachmentLine->addWidget(m_attach_button);
    attachmentLine->addWidget(m_open_attachment_button);
    attachmentLine->addWidget(m_save_attachment_button);
    rootLayout->addLayout(attachmentLine);

    QHBoxLayout *buttonLine = new QHBoxLayout;
    buttonLine->addWidget(m_ok_button);
    buttonLine->addWidget(m_cancel_button);
    rootLayout->addLayout(buttonLine);

    QObject::connect(m_ok_button, &QPushButton::clicked, this, &CPwdEntryDlg::onOK);
    QObject::connect(m_cancel_button, &QPushButton::clicked, this, &QDialog::reject);
    QObject::connect(m_attach_button, &QPushButton::clicked, this, &CPwdEntryDlg::onAttach);
    QObject::connect(m_open_attachment_button, &QPushButton::clicked, this, &CPwdEntryDlg::onOpenAttachment);
    QObject::connect(m_save_attachment_button, &QPushButton::clicked, this, &CPwdEntryDlg::onSaveAttachment);
    this->setLayout(rootLayout);
}
