//
// Created by tongl on 3/1/2024.
//

#include "CKBModel.h"

int CKBModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 4;
}

QVariant CKBModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole)
    {
        return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() + 1);
    }
    return QVariant();
}

int CKBModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    // Title, Username,  Password (***), URL
    return 4;
}
