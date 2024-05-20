//
// Created by tongl on 2/29/2024.
//

#ifndef KEYBOX_CPASSWORDBOX_H
#define KEYBOX_CPASSWORDBOX_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <memory>
#include "../utilities/CMaskedBlob.h"

class CPasswordBox : public QWidget {
Q_OBJECT
private:
    std::string m_label;
    QIcon *m_showIcon;
    QIcon *m_hideIcon;
    QLineEdit *m_textInput_oneline;
    QTextEdit *m_textInput_multipleline;
    QPushButton *m_showButton;

    bool m_doesShow;
    CMaskedBlob m_masked_blob;
    IRandomGenerator &m_randomGenerator;

    void init(bool multipleLine, bool _doesShow);

public:
    CPasswordBox(QWidget *,
                 const std::string &&,
                 IRandomGenerator &,
                 bool multipleLine,
                 bool _doesShow);
    CPasswordBox(QWidget *,
                 const std::string &&,
                 IRandomGenerator &,
                 const CMaskedBlob& _maskedBlob,
                 bool multipleLine,
                 bool _doesShow);

    CMaskedBlob GetPassword() {
        return m_masked_blob;
    }

    void SetPassword(CMaskedBlob _blob);

public slots:

    void onShowClickedOneline();

    void onShowClickedMultipleline();

    void onTextChanged(const QString &text);

    void onFocusOut();

    void onMultipleTextChanged();
};


#endif //KEYBOX_CPASSWORDBOX_H
