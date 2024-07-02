//
// Created by tongl on 3/1/2024.
//

#ifndef KEYBOX_CKBMODEL_H
#define KEYBOX_CKBMODEL_H

#include <QAbstractTableModel>

#include "../utilities/CKBFile.h"
#include "utilities/CPwdGroup.h"

class CKBModel : public QAbstractTableModel{
Q_OBJECT

public:
    explicit CKBModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void sort(int column, Qt::SortOrder order) override;

    void SetFilter(const QString& filterText);
    void SetFilter(const boost::uuids::uuid group);


    // two step load, because payload needs header to decrypt
    uint32_t LoadKBHeader(const std::string &filepath);

    uint32_t LoadPayload();

    void SetKeyDerivateParameters(const std::vector<unsigned char> &_salt, int num_round = 60000);

    const PBKDF2_256_PARAMETERS &GetKeyDerivateParameters();

    void SetPrimaryKey(CMaskedBlob p);

    void AddEntry(const CPwdEntry &pe);
    CPwdEntry GetEntry(int index);
    uint32_t SetEntry(const CPwdEntry &pe, int idx);

    uint32_t Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    const std::string &GetFilePath() const;

    void SetFilePath(const std::string &filepath);

    const std::vector<CPwdGroup>& GetGroups() const;
    uint32_t RemoveGroup(const std::string _uuid_str);
    uint32_t UpdateGroup(const std::string& uid, const std::string& name);

private:
    CKBFile m_kbfile;
    std::vector<unsigned char> m_file_buff;
    std::string m_kbfile_fullpath;
    uint32_t m_header_size;

    boost::uuids::uuid m_filter_group = g_RootGroupId;
    std::string m_filter_text;
    void setFilter();
    std::vector<CPwdEntry> m_filtered_entries;

};


#endif //KEYBOX_CKBMODEL_H
