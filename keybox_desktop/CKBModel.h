//
// Created by tongl on 3/1/2024.
//

#ifndef KEYBOX_CKBMODEL_H
#define KEYBOX_CKBMODEL_H
#include <QAbstractTableModel>

#include "../utilities/CKBFile.h"

class CKBModel : public QAbstractTableModel{
Q_OBJECT

public:
    CKBModel(const QStringList& strings, QObject* parent = nullptr):
            QAbstractTableModel(parent), stringList(strings){}
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
private:
    CKBFile m_kbfile;
    QStringList stringList;
};


#endif //KEYBOX_CKBMODEL_H
