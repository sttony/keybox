//
// Created by tongl on 3/1/2024.
//

#include <fstream>
#include "CKBModel.h"
#include "../utilities/error_code.h"

int CKBModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_kbfile.GetEntries().size();
}

QVariant CKBModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole)
    {
        if(m_kbfile.GetEntries().size() >0) {
            const CPwdEntry &pwdEntry = m_kbfile.GetEntries().at(index.row());
            if (index.column() == 1) {
                return QString(pwdEntry.GetTitle().c_str());
            } else if (index.column() == 2) {
                return QString(pwdEntry.GetUserName().c_str());
            }
        }
    }
    return QVariant();
}

int CKBModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    // Title, Username,  Password (***), URL
    return 4;
}

uint32_t CKBModel::LoadKB(const std::string &filepath) {
    // load CKBFile from file at filepath
    std::ifstream fd(filepath, std::ios::binary | std::ios::ate);
    if (!fd){
        return ERROR_FILE_IO;
    }
    std::streamsize size = fd.tellg();
    fd.seekg(0, std::ios::beg);

    m_file_buff.resize(size);
    if (!fd.read((char*)m_file_buff.data(), size)) {
        return ERROR_FILE_IO;
    }
    uint32_t  cbRealSize=0;
    m_kbfile.Deserialize(m_file_buff.data(),m_file_buff.size(), cbRealSize);


    return 0;
}

void CKBModel::SetKeyDerivateParameters(const std::vector<unsigned char> &_salt, int num_round) {
    m_kbfile.SetDerivativeParameters(_salt, num_round);
}

const PBKDF2_256_PARAMETERS& CKBModel::GetKeyDerivateParameters() {
    return m_kbfile.GetDerivativeParameters();
}

void CKBModel::SetPrimaryKey(CMaskedBlob p) {
    m_kbfile.SetMasterKey(p);
}

void CKBModel::AddEntry(const CPwdEntry &pe) {
    beginResetModel();
    m_kbfile.AddEntry(pe);
    endResetModel();
}
