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
    if (role == Qt::DisplayRole) {
        if (m_kbfile.GetEntries().size() > 0) {
            const CPwdEntry &pwdEntry = m_kbfile.GetEntries().at(index.row());
            if (index.column() == 0) {
                return QString(pwdEntry.GetTitle().c_str());
            } else if (index.column() == 1) {
                return QString(pwdEntry.GetUserName().c_str());
            } else if (index.column() == 2) {
                return QString("****");
            } else if (index.column() == 3) {
                return QString(pwdEntry.GetUrl().c_str());
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

uint32_t CKBModel::LoadKBHeader(const std::string &filepath) {
    // load CKBFile from file at filepath
    std::ifstream fd(filepath, std::ios::binary | std::ios::ate);
    if (!fd) {
        return ERROR_FILE_IO;
    }
    std::streamsize size = fd.tellg();
    fd.seekg(0, std::ios::beg);

    m_file_buff.resize(size);
    if (!fd.read((char *) m_file_buff.data(), size)) {
        return ERROR_FILE_IO;
    }
    return m_kbfile.LoadHeader(m_file_buff.data(), m_file_buff.size(), m_header_size);
}

void CKBModel::SetKeyDerivateParameters(const std::vector<unsigned char> &_salt, int num_round) {
    m_kbfile.SetDerivativeParameters(_salt, num_round);
}

const PBKDF2_256_PARAMETERS &CKBModel::GetKeyDerivateParameters() {
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

CKBModel::CKBModel(QObject *parent) : QAbstractTableModel(parent) {
}

QVariant CKBModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
                case 0:
                    return QString("Title");
                case 1:
                    return QString("UserName");
                case 2:
                    return QString("Password");
                case 3:
                    return QString("Url");
                default:
                    assert(false);
            }
        }
    }
    return QVariant();
}

const std::string &CKBModel::GetFilePath() const {
    return m_kbfile_fullpath;
}

uint32_t CKBModel::Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    return m_kbfile.Serialize(pBuffer, cbBufferSize, cbRealSize);
}

void CKBModel::SetFilePath(const std::string &filepath) {
    m_kbfile_fullpath = filepath;
}

uint32_t CKBModel::LoadPayload() {
    uint32_t cbRealSize = 0;
    return m_kbfile.LoadPayload(m_file_buff.data() + m_header_size, m_file_buff.size() - m_header_size, cbRealSize);
}

CPwdEntry CKBModel::GetEntry(int index) {
    return m_kbfile.GetEntry(index);
}

uint32_t CKBModel::SetEntry(const CPwdEntry &pe, int idx) {
    beginResetModel();
    uint32_t result = m_kbfile.SetEntry(pe, idx);
    endResetModel();
    return result;
}
