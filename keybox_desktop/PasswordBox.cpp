//
// Created by tongl on 2/29/2024.
//

#include "PasswordBox.h"
#include <iostream>
using namespace std;

PasswordBox::PasswordBox(QWidget *parent, const string&& _label)
        : QWidget(parent), m_label(_label), m_doesShow(false){
    m_hideIcon = new QIcon(":img/img/eye.slash.svg");
    m_showIcon = new QIcon(":img/img/eye.svg");

    QHBoxLayout *layout = new QHBoxLayout(this);
    QLabel *label = new QLabel(m_label.c_str());
    m_textInput = new QLineEdit;

    m_showButton = new QPushButton();
    m_showButton->setIcon(*m_hideIcon);

    connect(m_showButton, &QPushButton::clicked, this, &PasswordBox::onShowClicked);

    layout->addWidget(label);
    layout->addWidget(m_textInput);
    layout->addWidget(m_showButton);
}

void PasswordBox::onShowClicked() {
    m_doesShow = !m_doesShow;
    if(m_doesShow){
        m_showButton->setIcon(*m_showIcon);
    }
    else{
        m_showButton->setIcon(*m_hideIcon);
    }
}
