//
// Created by tongl on 3/26/2024.
//

#include <QVBoxLayout>

#include "CPwdGeneratorDlg.h"
extern CRandomGenerator g_RG;

using namespace std;

CPwdGeneratorDlg::CPwdGeneratorDlg(QWidget *parent): m_pwdGenerator(g_RG) {
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


    m_sliderLength = new QSpinBox();
    m_sliderLength->setMinimum(1);
    m_sliderLength->setMaximum(100);
    m_sliderLength->setValue(8);
    rootLayout->addWidget(m_sliderLength);

    m_textPwd = new CPasswordBox(nullptr, "Password", g_RG, false, false);

    rootLayout->addWidget(m_textPwd);

    m_buttonRegenerate = new QPushButton("Re-generate");
    rootLayout->addWidget(m_buttonRegenerate);

    this->setLayout(rootLayout);

    QObject::connect(m_buttonRegenerate, &QPushButton::clicked, this, &CPwdGeneratorDlg::onRegenerate);

    CMaskedBlob _blob;
    _blob.Set(m_pwdGenerator.GeneratePassword(), g_RG);
    m_textPwd->SetPassword(_blob);

}

void CPwdGeneratorDlg::onRegenerate() {
    this->RefreshUI();
    CMaskedBlob _blob;
    _blob.Set(m_pwdGenerator.GeneratePassword(), g_RG);
    m_textPwd->SetPassword(_blob);
}

void CPwdGeneratorDlg::RefreshUI() {
    if(m_pwdGenerator.GetSlash()){
        m_chkboxSlash->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetQuestion()){
        m_chkboxQuestion->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetSpace()){
        m_chkboxSpace->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetBrace()){
        m_chkboxBrace->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetMinus()){
        m_chkboxMinus->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetGreaterLess()){
        m_chkboxGreaterLess->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetShift1_8()){
        m_chkboxShift1_8->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetDigit()){
        m_chkboxDigits->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetAdd()){
        m_chkboxAdd->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetLower()){
        m_chkboxLower->setCheckState(Qt::CheckState::Checked);
    }
    if(m_pwdGenerator.GetUpper()){
        m_chkboxUpper->setCheckState(Qt::CheckState::Checked);
    }
    m_sliderLength->setValue(m_pwdGenerator.GetLength());

}