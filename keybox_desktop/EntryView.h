//
// Created by tongl on 2/15/2024.
//

#ifndef KEYBOX_ENTRYVIEW_H
#define KEYBOX_ENTRYVIEW_H


#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>

class EntryView : public QDialog {
Q_OBJECT
public:
    EntryView(QWidget *parent = nullptr);

private slots:

private:
    QLineEdit* m_title_box;
    QLineEdit* m_user_name_box;
    QLineEdit* m_url_box;
    QLineEdit* m_password_box;
    QPushButton* m_password_show_button;

    QTextEdit* m_note_box;
    QPushButton* m_note_show_button;

    QPushButton* m_ok_button;
    QPushButton* m_cancel_button;
};


#endif //KEYBOX_ENTRYVIEW_H
