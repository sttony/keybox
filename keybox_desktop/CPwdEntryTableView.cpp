//
// Created by tongl on 3/22/2024.
//

#include "CPwdEntryTableView.h"
#include "CKBModel.h"
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QAction>
using namespace std;

void CPwdEntryTableView::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Copy)) {
        copySelectedEntryPassword();
    } else if (event->key() == Qt::Key_Delete) {
        if (!this->selectedIndexes().empty()) {
            auto selectedIdx = *(this->selectedIndexes().begin());
            emit deleteEntry(selectedIdx.row());
        }
    } else {
        QTableView::keyPressEvent(event);
    }
}

void CPwdEntryTableView::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    QAction *copyAction = menu.addAction("Copy Password");
    connect(copyAction, &QAction::triggered, this, &CPwdEntryTableView::copySelectedEntryPassword);

    QAction *deleteAction = menu.addAction("Delete Entry");
    connect(deleteAction, &QAction::triggered, this, [this]() {
        if (!this->selectedIndexes().empty()) {
            auto selectedIdx = *(this->selectedIndexes().begin());
            emit deleteEntry(selectedIdx.row());
        }
    });

    menu.exec(event->globalPos());
}

void CPwdEntryTableView::copySelectedEntryPassword() const {
    auto pModel = dynamic_cast<CKBModel*>(this->model());
    if( pModel == nullptr){
        return;
    }
    if( !this->selectedIndexes().empty()){
        auto selectedIdx = *(this->selectedIndexes().begin());

        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(pModel->GetEntry(selectedIdx.row()).GetPassword().c_str());
    }
}
