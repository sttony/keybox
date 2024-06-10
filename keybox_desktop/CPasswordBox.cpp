//
// Created by tongl on 2/29/2024.
//

#include "CPasswordBox.h"
#include <iostream>
#include <utility>

using namespace std;

CPasswordBox::CPasswordBox(QWidget *parent,
                           const string &&_label,
                           IRandomGenerator &_rg,
                           bool multipleLine,
                           bool _doesShow)
        : QWidget(parent),
          m_label(_label),
          m_doesShow(_doesShow),
          m_randomGenerator(_rg) {
    init(multipleLine, _doesShow);
}

CPasswordBox::CPasswordBox(QWidget *parent,
                           const string &&_label,
                           IRandomGenerator &_rg,
                           const CMaskedBlob& _maskedBlob,
                           bool multipleLine,
                           bool _doesShow)
        : QWidget(parent),
          m_label(_label),
          m_doesShow(_doesShow),
          m_randomGenerator(_rg),
          m_masked_blob(_maskedBlob){
    init(multipleLine, _doesShow);
}

void CPasswordBox::onShowClickedOneline() {
    m_doesShow = !m_doesShow;
    if (m_doesShow) {
        m_showButton->setIcon(*m_showIcon);
        m_textInput_oneline->setEchoMode(QLineEdit::Normal);
    } else {
        m_showButton->setIcon(*m_hideIcon);
        m_textInput_oneline->setEchoMode(QLineEdit::Password);
    }
}

void CPasswordBox::onShowClickedMultipleline() {
    m_doesShow = !m_doesShow;
    if (m_doesShow) {
        m_showButton->setIcon(*m_showIcon);
        m_textInput_multipleline->setDisabled(false);
        m_textInput_multipleline->setText(m_masked_blob.Show().c_str());
    } else {
        m_showButton->setIcon(*m_hideIcon);
        m_textInput_multipleline->setDisabled(true);
        string text = m_textInput_multipleline->toPlainText().toStdString();
        m_masked_blob.Set(text, m_randomGenerator.GetNextBytes(text.size()));
        m_textInput_multipleline->setText(string(m_textInput_multipleline->toPlainText().size(), '*').c_str());
    }
}

void CPasswordBox::onTextChanged(const QString &text) {
    string temp = text.toStdString();
    m_masked_blob.Set(temp, m_randomGenerator.GetNextBytes(temp.size()));
}


void CPasswordBox::onMultipleTextChanged() {
    string temp = m_textInput_multipleline->toPlainText().toStdString();
    m_masked_blob.Set(temp, m_randomGenerator.GetNextBytes(temp.size()));
}


void CPasswordBox::onFocusOut() {
    string temp = m_textInput_oneline->text().toStdString();
    m_masked_blob.Set(temp, m_randomGenerator.GetNextBytes(temp.size()));
    if(m_doesShow){
        m_textInput_oneline->setEchoMode(QLineEdit::Normal);
    }
    else{
        m_textInput_oneline->setEchoMode(QLineEdit::Password);
    }
}

void CPasswordBox::init(bool multipleLine, bool _doesShow) {
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
        m_textInput_oneline->setText(m_masked_blob.Show().c_str());
        connect(m_textInput_oneline, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));
        connect(m_textInput_oneline, &QLineEdit::editingFinished, this, &CPasswordBox::onFocusOut);
    } else {
        m_textInput_multipleline = new QTextEdit;
        m_textInput_multipleline->setDisabled(!m_doesShow);
        m_textInput_multipleline->setStyleSheet("border: 2px solid black;");
        m_textInput_multipleline->setText(std::string(m_masked_blob.size(), '*').c_str());
        connect(m_textInput_multipleline, &QTextEdit::textChanged, this, &CPasswordBox::onMultipleTextChanged);
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
        connect(m_showButton, &QPushButton::clicked, this, &CPasswordBox::onShowClickedOneline);
    } else {
        connect(m_showButton, &QPushButton::clicked, this, &CPasswordBox::onShowClickedMultipleline);
    }
    //connect (m_textInput, &QTextEdit::textChanged, this, &CPasswordBox::onTextChanged);
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

void CPasswordBox::SetPassword(CMaskedBlob _blob) {
    m_masked_blob = _blob;
    if(m_textInput_multipleline){
        m_textInput_multipleline->setText(m_masked_blob.Show().c_str());
    }
    if(m_textInput_oneline){
        m_textInput_oneline->setText(m_masked_blob.Show().c_str());
    }
}
