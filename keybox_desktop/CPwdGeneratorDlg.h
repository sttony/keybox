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

class CPwdGeneratorDlg : public QDialog{
Q_OBJECT
public:
    explicit CPwdGeneratorDlg(QWidget *parent = nullptr);
private:

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

    QLineEdit* m_textPwd;
};


#endif //KEYBOX_CPWDGENERATORDLG_H
