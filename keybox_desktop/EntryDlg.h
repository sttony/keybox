//
// Created by tongl on 2/15/2024.
//

#ifndef KEYBOX_ENTRYDLG_H
#define KEYBOX_ENTRYDLG_H


#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include "PasswordBox.h"
#include "utilities/CPwdEntry.h"

class EntryDlg : public QDialog {
Q_OBJECT
public:
    EntryDlg(QWidget *parent = nullptr);
    const CPwdEntry& GetPwdEntry();

private slots:
    void onOK();
private:
    QLineEdit* m_title_box;
    QLineEdit* m_user_name_box;
    QLineEdit* m_url_box;

    QPushButton* m_ok_button;
    QPushButton* m_cancel_button;
    PasswordBox* m_pwd_box;
    PasswordBox* m_note_box;
    CPwdEntry m_PwdEntry;


    QHBoxLayout*  createInputLine(const QString& label, QLineEdit* inputWidget);
};


#endif //KEYBOX_ENTRYDLG_H
