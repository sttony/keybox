//
// Created by tongl on 3/26/2024.
//

#ifndef KEYBOX_CPWDGENERATORDLG_H
#define KEYBOX_CPWDGENERATORDLG_H


#include <QDialog>
#include <QCheckBox>
#include <QSlider>
#include <QLineEdit>
#include <QSpinBox>
#include "CPasswordBox.h"
#include "utilities/CPasswordGenerator.h"

class CPwdGeneratorDlg : public QDialog{
Q_OBJECT
public:
    explicit CPwdGeneratorDlg(QWidget *parent = nullptr);

private slots:
    void onRegenerate();
    void onUpperCheck(int);
    void onLowerCheck(int);
    void onDigitsCheck(int);
    void onMinusCheck(int);
    void onAddCheck(int);
    void onShift1_8Check(int);
    void onBraceCheck(int);
    void onSpaceCheck(int);
    void onQuestionCheck(int);
    void onSlashCheck(int);
    void onGreaterLessCheck(int);
    void onLengthChange(const QString&);

private:
    void RefreshUI();

    CPasswordGenerator m_pwdGenerator;

    QCheckBox* m_chkboxUpper;
    QCheckBox* m_chkboxLower;
    QCheckBox* m_chkboxDigits;
    QCheckBox* m_chkboxMinus;
    QCheckBox* m_chkboxAdd;
    QCheckBox* m_chkboxShift1_8;
    QCheckBox* m_chkboxBrace;
    QCheckBox* m_chkboxSpace;
    QCheckBox* m_chkboxQuestion;
    QCheckBox* m_chkboxSlash;
    QCheckBox* m_chkboxGreaterLess;
    QSpinBox*  m_sliderLength;

    CPasswordBox* m_textPwd;
    QPushButton* m_buttonRegenerate;

    void RefreshPwd();
};


#endif //KEYBOX_CPWDGENERATORDLG_H
