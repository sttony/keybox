//
// Created by tongl on 2/15/2024.
//

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include "EntryView.h"

#include "PasswordBox.h"
using namespace std;
RandomGenerator g_rg(RandomGenerator::Salsa20);

QHBoxLayout* EntryView::createInputLine(const QString& label, QLineEdit* inputWidget) {
    QHBoxLayout* inputLine = new QHBoxLayout;
    inputLine->addWidget(new QLabel(label));
    inputLine->addWidget(inputWidget);
    inputLine->setStretch(0, 1);
    inputLine->setStretch(1, 4);
    return inputLine;
}


EntryView::EntryView(QWidget *parent) {
    QVBoxLayout* rootLayout = new QVBoxLayout (this);

    m_title_box = new QLineEdit;
    m_url_box = new QLineEdit;
    m_user_name_box = new QLineEdit;
    m_password_box = new QLineEdit;
    m_note_box = new QTextEdit;

    m_ok_button = new QPushButton("Save");
    m_cancel_button = new QPushButton("Cancel");
    m_password_show_button = new QPushButton("P");
    m_note_show_button = new QPushButton("N");



    std::array<unsigned char, 8> vIV = {0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a};
    std::vector<unsigned char> vKey = {0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a,
                                          0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a,
                                          0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a,
                                          0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a};
    g_rg.init(vKey, vIV);


    rootLayout->addLayout(createInputLine("Title", m_title_box));
    rootLayout->addLayout(createInputLine("Url", m_url_box));
    rootLayout->addLayout(createInputLine("Username", m_user_name_box));

    PasswordBox* pwd_box = new PasswordBox(nullptr, "Password", make_shared<CMaskedBlob>(b),
                                               g_rg, false, false);
    rootLayout->addWidget(pwd_box);
    PasswordBox* note_box = new PasswordBox(nullptr, "Note", make_shared<CMaskedBlob>(n), g_rg, true, false);
    rootLayout->addWidget(note_box);

//    gridLayout->addWidget(m_ok_button, 15, 0);
//    gridLayout->addWidget(m_cancel_button, 15, 1);
    this->setLayout(rootLayout);
}
