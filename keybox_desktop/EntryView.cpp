//
// Created by tongl on 2/15/2024.
//

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include "EntryView.h"

EntryView::EntryView(QWidget *parent) {
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(10);

    m_title_box = new QLineEdit;
    m_url_box = new QLineEdit;
    m_user_name_box = new QLineEdit;
    m_password_box = new QLineEdit;
    m_note_box = new QTextEdit;

    m_ok_button = new QPushButton("Save");
    m_cancel_button = new QPushButton("Cancel");
    m_password_show_button = new QPushButton("P");
    m_note_show_button = new QPushButton("N");


    gridLayout->addWidget(new QLabel("Title"), 0, 0);
    gridLayout->addWidget(m_title_box, 0, 1, 1, 19);

    gridLayout->addWidget(new QLabel("Url"), 1, 0);
    gridLayout->addWidget(m_url_box, 1, 1, 1, 19);

    gridLayout->addWidget(new QLabel("UserName"), 2, 0);
    gridLayout->addWidget(m_user_name_box, 2, 1, 1, 19);

    gridLayout->addWidget(new QLabel("Password"), 3, 0);
    gridLayout->addWidget(m_password_box, 3, 1, 1, 17);
    gridLayout->addWidget(m_password_show_button, 3, 18);

    gridLayout->addWidget(new QLabel("Note"), 4, 0);
    gridLayout->addWidget(m_note_show_button, 4, 18);
    gridLayout->addWidget(m_note_box, 5, 0, 9, 19);

    gridLayout->addWidget(m_ok_button, 15, 0);
    gridLayout->addWidget(m_cancel_button, 15, 1);


    this->setLayout(gridLayout);

}
