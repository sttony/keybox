//
// Created by tongl on 2/15/2024.
//

#ifndef KEYBOX_CPWDENTRYDLG_H
#define KEYBOX_CPWDENTRYDLG_H


#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include "CPasswordBox.h"
#include "utilities/CPwdEntry.h"
#include "utilities/CPwdGroup.h"

class CPwdEntryDlg : public QDialog {
Q_OBJECT
public:
    explicit CPwdEntryDlg(const std::vector<CPwdGroup>& groups, QWidget *parent = nullptr);
    explicit CPwdEntryDlg(const std::vector<CPwdGroup>& groups, const CPwdEntry& pwdEntry, QWidget *parent = nullptr);

    const CPwdEntry &GetPwdEntry();

private slots:

    void onOK();

private:
    QLineEdit *m_title_box;
    QLineEdit *m_user_name_box;
    QLineEdit *m_url_box;

    QPushButton *m_ok_button;
    QPushButton *m_cancel_button;
    CPasswordBox *m_pwd_box;
    CPasswordBox *m_note_box;

    QComboBox* m_group_box;

    CPwdEntry m_PwdEntry;
    std::vector<CPwdGroup> m_groups;
    QHBoxLayout *createInputLine(const QString &label, QLineEdit *inputWidget);
    void init(const std::vector<CPwdGroup>& groups, QWidget *parent = nullptr);
};


#endif //KEYBOX_CPWDENTRYDLG_H
