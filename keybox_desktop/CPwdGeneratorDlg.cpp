//
// Created by tongl on 3/26/2024.
//

#include <QVBoxLayout>

#include "CPwdGeneratorDlg.h"

CPwdGeneratorDlg::CPwdGeneratorDlg(QWidget *parent) {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);

    m_chkboxUpper = new QCheckBox("Upper case A-Z");
    rootLayout->addWidget(m_chkboxUpper);

    m_chkboxLower= new QCheckBox("Lowe case a-z");
    rootLayout->addWidget(m_chkboxLower);


    m_chkboxDigits = new QCheckBox("Digits 0-9");
    rootLayout->addWidget(m_chkboxDigits);

    m_chkboxMinus = new QCheckBox("Minus -");
    rootLayout->addWidget(m_chkboxMinus);

    m_chkboxAdd = new QCheckBox("Add +9");
    rootLayout->addWidget(m_chkboxAdd);

    m_chkboxShift1_8 = new QCheckBox("Shift1_8 !@#$%^&*");
    rootLayout->addWidget(m_chkboxShift1_8);

    m_chkboxBrace = new QCheckBox("Brace ()[]{}");
    rootLayout->addWidget(m_chkboxBrace);

    m_chkboxSpace = new QCheckBox("Space \" \"");
    rootLayout->addWidget(m_chkboxSpace);

    m_chkboxQuestion = new QCheckBox("Question ?");
    rootLayout->addWidget(m_chkboxQuestion);

    m_chkboxSlash = new QCheckBox("Slash /\\");
    rootLayout->addWidget(m_chkboxSlash);

    m_chkboxGreaterLess = new QCheckBox("Greater Less <>");
    rootLayout->addWidget(m_chkboxGreaterLess);

    this->setLayout(rootLayout);
}
