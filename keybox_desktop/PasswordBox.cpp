//
// Created by tongl on 2/29/2024.
//

#include "PasswordBox.h"
#include <iostream>
#include <utility>

using namespace std;

PasswordBox::PasswordBox(QWidget *parent,
                         const string &&_label,
                         IRandomGenerator &_rg,
                         bool multipleLine,
                         bool _doesShow)
        : QWidget(parent),
          m_label(_label),
          m_doesShow(_doesShow),
          m_randomGenerator(_rg) {
    m_hideIcon = new QIcon(":img/img/eye.slash.svg");
    m_showIcon = new QIcon(":img/img/eye.svg");

    QLabel *label = new QLabel(m_label.c_str());
    if (!multipleLine) {
        m_textInput_oneline = new QLineEdit;
        if(!m_doesShow){
            m_textInput_oneline->setEchoMode(QLineEdit::Password);
        }
        else{
            m_textInput_oneline->setEchoMode(QLineEdit::Normal);
        }
        connect(m_textInput_oneline, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));
        connect(m_textInput_oneline, &QLineEdit::editingFinished, this, &PasswordBox::onFocusOut);
    } else {
        m_textInput_multipleline = new QTextEdit;
        m_textInput_multipleline->setDisabled(!m_doesShow);
        m_textInput_multipleline->setStyleSheet("border: 2px solid black;");
    }
    m_showButton = new QPushButton();
    if (m_doesShow) {
        m_showButton->setIcon(*m_showIcon);
    } else {
        m_showButton->setIcon(*m_hideIcon);
    }
    m_showButton->setIconSize(QSize(24, 24));
    m_showButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_showButton->setFixedSize(m_showButton->iconSize());

    if (!multipleLine) {
        connect(m_showButton, &QPushButton::clicked, this, &PasswordBox::onShowClickedOneline);
    } else {
        connect(m_showButton, &QPushButton::clicked, this, &PasswordBox::onShowClickedMultipleline);
    }
    //connect (m_textInput, &QTextEdit::textChanged, this, &PasswordBox::onTextChanged);
    if (!multipleLine) {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(label);
        layout->setStretch(0, 1);
        QHBoxLayout *remain = new QHBoxLayout;
        remain->addWidget(m_textInput_oneline);
        remain->addWidget(m_showButton);
        layout->addLayout(remain);
        layout->setStretch(1, 4);
        setLayout(layout);
    } else {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        QHBoxLayout *titleLine = new QHBoxLayout;
        titleLine->addWidget(label);
        titleLine->addWidget(m_showButton);
        layout->addLayout(titleLine);
        layout->addWidget(m_textInput_multipleline);
        setLayout(layout);
        m_textInput_multipleline->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

}

void PasswordBox::onShowClickedOneline() {
    m_doesShow = !m_doesShow;
    if (m_doesShow) {
        m_showButton->setIcon(*m_showIcon);
        m_textInput_oneline->setEchoMode(QLineEdit::Normal);
    } else {
        m_showButton->setIcon(*m_hideIcon);
        m_textInput_oneline->setEchoMode(QLineEdit::Password);
    }
}

void PasswordBox::onShowClickedMultipleline() {
    m_doesShow = !m_doesShow;
    if (m_doesShow) {
        m_showButton->setIcon(*m_showIcon);
        m_textInput_multipleline->setDisabled(false);
        m_textInput_multipleline->setText(m_masked_blob.Show().c_str());
    } else {
        m_showButton->setIcon(*m_hideIcon);
        m_textInput_multipleline->setDisabled(true);
        string text = m_textInput_multipleline->toPlainText().toStdString();
        m_masked_blob.Set(text, m_randomGenerator);
        m_textInput_multipleline->setText(string(m_textInput_multipleline->toPlainText().size(), '*').c_str());
    }
}

void PasswordBox::onTextChanged(const QString &text) {
    string temp = text.toStdString();
    m_masked_blob.Set(temp, m_randomGenerator);
}

void PasswordBox::onFocusOut() {
    string temp = m_textInput_oneline->text().toStdString();
    m_masked_blob.Set(temp, m_randomGenerator);
    if(m_doesShow){
        m_textInput_oneline->setEchoMode(QLineEdit::Normal);
    }
    else{
        m_textInput_oneline->setEchoMode(QLineEdit::Password);
    }
}
