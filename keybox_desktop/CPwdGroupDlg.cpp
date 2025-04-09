
#include <unordered_set>
#include <QVBoxLayout>
#include <QMessageBox>

#include "CPwdGroupDlg.h"

using namespace std;

CPwdGroupDlg::CPwdGroupDlg(CKBModel *pModel, QWidget *parent) {
    assert(pModel);
    resize(400, 600);
    m_kbModel = pModel;
    // name and uuid
    m_table_model = new QStandardItemModel(0, 2);
    for(const auto& group : m_kbModel->GetGroups()){
        QList<QStandardItem*> groupItem;
        groupItem.append(new QStandardItem(group.GetName().c_str()));
        auto gid = new QStandardItem(group.GetID().c_str());
        gid->setFlags(gid->flags() & ~Qt::ItemIsEditable);
        groupItem.append(gid);

        m_table_model->appendRow(groupItem);
    }
    m_table_model->setHeaderData(0, Qt::Horizontal, QObject::tr("name"));
    m_table_model->setHeaderData(1, Qt::Horizontal, QObject::tr("uuid"));
    m_group_table_view = new QTableView;
    m_group_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_group_table_view->setModel(m_table_model);

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->addWidget(m_group_table_view);

    QHBoxLayout *btnLine1 = new QHBoxLayout();
    m_addBtn = new QPushButton("Add a group");
    m_deleteBtn = new QPushButton("Delete group");
    connect(m_addBtn, &QPushButton::clicked, this, &CPwdGroupDlg::onAddGroup);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CPwdGroupDlg::onDeleteGroup);
    btnLine1->addWidget(m_addBtn);
    btnLine1->addWidget(m_deleteBtn);
    rootLayout->addLayout(btnLine1);

    QHBoxLayout *btnLine2 = new QHBoxLayout();
    m_saveBtn = new QPushButton("Save");
    m_cancelBtn = new QPushButton("Cancel");
    btnLine2->addWidget(m_saveBtn);
    btnLine2->addWidget(m_cancelBtn);
    rootLayout->addLayout(btnLine2);
    connect(m_saveBtn, &QPushButton::clicked, this, &CPwdGroupDlg::onSave);
    QObject::connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    this->setLayout(rootLayout);

}

void CPwdGroupDlg::onAddGroup() {
    CPwdGroup pwdGroup ("New group");
    QList<QStandardItem*> groupItem;
    groupItem.append(new QStandardItem(pwdGroup.GetName().c_str()));
    auto gid = new QStandardItem(pwdGroup.GetID().c_str());
    gid->setFlags(gid->flags() & ~Qt::ItemIsEditable);
    groupItem.append(gid);
    m_table_model->appendRow(groupItem);
}

void CPwdGroupDlg::onDeleteGroup() {
    auto selectedIdx = m_group_table_view->selectionModel()->selectedRows();
    if(!selectedIdx.isEmpty()){
        if(selectedIdx.first().row() == 0){
            QMessageBox::information(nullptr, "Alert", "Can't delete Root");
            return;
        }
        m_table_model->removeRow(selectedIdx.first().row());
    }
}

void CPwdGroupDlg::onSave() {
    int rowCount = m_table_model->rowCount();
    unordered_set<string> tobeDelete;
    for(const auto& group: m_kbModel->GetGroups()){
        tobeDelete.insert(group.GetID());
    }

    for (int i = 0; i < rowCount; ++i) {
        auto group_name = m_table_model->data(m_table_model->index(i, 0)).toString().toStdString();
        auto group_uuid = m_table_model->data(m_table_model->index(i, 1)).toString().toStdString();
        m_kbModel->UpdateGroup(group_uuid, group_name);
        tobeDelete.erase(group_uuid);
    }
    for(const auto& id : tobeDelete){
        m_kbModel->RemoveGroup(id);
    }

    QDialog::accept();
}
