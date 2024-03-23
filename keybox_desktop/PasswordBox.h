//
// Created by tongl on 2/29/2024.
//

#ifndef KEYBOX_PASSWORDBOX_H
#define KEYBOX_PASSWORDBOX_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <memory>
#include "../utilities/CMaskedBlob.h"

class PasswordBox : public QWidget {
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

public:
    PasswordBox(QWidget *,
                const std::string &&,
                IRandomGenerator &,
                bool multipleLine,
                bool _doesShow);

    CMaskedBlob GetPassword() {
        return m_masked_blob;
    }

public slots:

    void onShowClickedOneline();

    void onShowClickedMultipleline();

    void onTextChanged(const QString &text);

    void onFocusOut();
};


#endif //KEYBOX_PASSWORDBOX_H
