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
            QAbstractTableModel(parent){}
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


    uint32_t LoadKB(const std::string& filepath);
private:
    CKBFile m_kbfile;
    std::vector<unsigned char> m_file_buff;

};


#endif //KEYBOX_CKBMODEL_H
