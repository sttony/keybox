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
    CKBModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;


    uint32_t LoadKB(const std::string& filepath);

    void SetKeyDerivateParameters(const std::vector<unsigned char>& _salt, int num_round = 60000);
    const PBKDF2_256_PARAMETERS& GetKeyDerivateParameters();

    void SetPrimaryKey(CMaskedBlob p);
    void AddEntry(const CPwdEntry& pe);

    uint32_t Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    const std::string& GetFilePath() const;
    void SetFilePath(const std::string& filepath);
private:
    CKBFile m_kbfile;
    std::vector<unsigned char> m_file_buff;
    std::string m_kbfile_fullpath;

};


#endif //KEYBOX_CKBMODEL_H
