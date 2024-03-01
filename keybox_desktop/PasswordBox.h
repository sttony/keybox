//
// Created by tongl on 2/29/2024.
//

#ifndef KEYBOX_PASSWORDBOX_H
#define KEYBOX_PASSWORDBOX_H
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

class PasswordBox : public QWidget {
    Q_OBJECT
private:
    std::string m_label;
    QIcon* m_showIcon;
    QIcon* m_hideIcon;
    QLineEdit* m_textInput;
    QPushButton* m_showButton;

    bool m_doesShow;

public:
    PasswordBox(QWidget*, const std::string&&);

public slots:
    void onShowClicked();
};


#endif //KEYBOX_PASSWORDBOX_H
