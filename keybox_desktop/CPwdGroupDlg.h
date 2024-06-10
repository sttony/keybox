

#ifndef KEYBOX_CPWDGROUPDLG_H
#define KEYBOX_CPWDGROUPDLG_H


#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QStyledItemDelegate>
#include "CKBModel.h"

class CPwdGroupDlg : public QDialog {
Q_OBJECT
public:
    explicit CPwdGroupDlg(CKBModel* pModel, QWidget *parent = nullptr);

private slots:
    void onAddGroup();
    void onDeleteGroup();
    void onSave();

private:
    QTableView* m_group_table_view = nullptr;
    QStandardItemModel* m_table_model = nullptr;
    CKBModel* m_kbModel = nullptr;
    QPushButton* m_addBtn = nullptr;
    QPushButton* m_deleteBtn = nullptr;
    QPushButton* m_saveBtn = nullptr;
    QPushButton* m_cancelBtn = nullptr;
};


#endif //KEYBOX_CPWDGROUPDLG_H
