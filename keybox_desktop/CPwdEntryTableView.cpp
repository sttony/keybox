//
// Created by tongl on 3/22/2024.
//

#include "CPwdEntryTableView.h"
#include "CKBModel.h"
#include <QApplication>
#include <QClipboard>
using namespace std;

void CPwdEntryTableView::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Copy)) {
        auto pModel = dynamic_cast<CKBModel*>(this->model());
        if( pModel == nullptr){
            return;
        }
        if( !this->selectedIndexes().empty()){
            auto selectedIdx = *(this->selectedIndexes().begin());

            QClipboard* clipboard = QApplication::clipboard();
            std::string temp = pModel->GetEntry(selectedIdx.row()).GetPassword().c_str();
            clipboard->setText(pModel->GetEntry(selectedIdx.row()).GetPassword().c_str());
        }

    }
    else {
        QTableView::keyPressEvent(event);
    }
}
