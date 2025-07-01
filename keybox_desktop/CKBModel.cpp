//
// Created by tongl on 3/1/2024.
//

#include <fstream>
#include "CKBModel.h"
#include "../utilities/error_code.h"

using namespace std;

int CKBModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_filtered_entries.size();
}

QVariant CKBModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        if (!m_filtered_entries.empty()) {
            const CPwdEntry &pwdEntry = m_filtered_entries.at(index.row());
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

uint32_t CKBModel::LoadKBFileToBuff(const std::string &filepath) {
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
    return 0;
}

uint32_t CKBModel::LoadKBHeader(const std::string &filepath) {
    LoadKBFileToBuff(filepath);
    return m_kbfile.LoadHeader(m_file_buff.data(), m_file_buff.size(), m_header_size);
}

uint32_t CKBModel::LoadKBHeader() {
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
    m_filtered_entries.push_back(pe);
    m_kbfile.AddEntry(pe);
    emit layoutChanged();
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
    uint32_t uResult = m_kbfile.LoadPayload(m_file_buff.data() + m_header_size, m_file_buff.size() - m_header_size, cbRealSize);
    if(uResult){
        return uResult;
    }
    setFilter();
    return 0;
}

CPwdEntry CKBModel::GetEntry(int index) {
    return m_kbfile.GetEntry(index);
}

uint32_t CKBModel::SetEntry(const CPwdEntry &pe, int idx) {
    beginResetModel();
    uint32_t result = m_kbfile.SetEntry(pe);
    m_filtered_entries[idx] =pe;
    endResetModel();
    return result;
}

void CKBModel::sort(int column, Qt::SortOrder order) {
    switch(column){
        case 0:
            std::sort(m_filtered_entries.begin(), m_filtered_entries.end(), [order](const auto& _l, const auto& _r){
                return order == Qt::SortOrder::AscendingOrder ?
                    _l.GetTitle() < _r.GetTitle() :
                    _l.GetTitle() > _r.GetTitle();

            });
            break;
        case 1:
            std::sort(m_filtered_entries.begin(), m_filtered_entries.end(), [order](const auto& _l, const auto& _r){
                return order == Qt::SortOrder::AscendingOrder ?
                       _l.GetUserName() < _r.GetUserName() :
                       _l.GetUserName() > _r.GetUserName();

            });
            break;
        case 3:
            std::sort(m_filtered_entries.begin(), m_filtered_entries.end(), [order](const auto& _l, const auto& _r){
                return order == Qt::SortOrder::AscendingOrder ?
                       _l.GetUrl() < _r.GetUrl() :
                       _l.GetUrl() > _r.GetUrl();
            });
            break;
        default:
            break;
    }

    emit layoutChanged();
}

void CKBModel::setFilter() {
    m_filtered_entries.clear();
    for(const auto& pe: m_kbfile.GetEntries()){
        if( (pe.GetUserName().find( m_filter_text) != string::npos ||
                pe.GetUrl().find( m_filter_text) != string::npos ||
                pe.GetTitle().find( m_filter_text) != string::npos )
                && pe.GetGroup() == m_filter_group){
            m_filtered_entries.emplace_back(pe);
        }
    }
    emit layoutChanged();
}

void CKBModel::SetFilter(const QString& filterText){
    m_filter_text = filterText.toStdString();
    this->setFilter();
}

void CKBModel::SetFilter(const boost::uuids::uuid group){
    m_filter_group = group;
    this->setFilter();
}

const std::vector<CPwdGroup> &CKBModel::GetGroups() const {
    return m_kbfile.GetGroups();
}

uint32_t CKBModel::RemoveGroup(const std::string _uuid_str) {
    return m_kbfile.RemoveGroup(_uuid_str);
}

uint32_t CKBModel::UpdateGroup(const std::string &uid, const std::string &name) {
    return m_kbfile.UpdateGroup(uid, name);
}

const std::string CKBModel::GetEmail() {
    return m_kbfile.GetHeader().GetSyncEmail();
}
uint32_t CKBModel::SetEmail(const std::string email) {
    return m_kbfile.GetHeader().SetSyncEmail(email);
}
const std::string CKBModel::GetSyncUrl() {
    return m_kbfile.GetHeader().GetSyncUrl();
}
uint32_t CKBModel::SetSyncUrl(const std::string syncUrl) {
    return m_kbfile.GetHeader().SetSyncUrl(syncUrl);
}

void CKBModel::Lock() {
    uint32_t cbRealSize  =0;
    m_kbfile.Lock(nullptr, 0, cbRealSize);
    m_file_buff.resize(cbRealSize);
    m_kbfile.Lock(m_file_buff.data(), m_file_buff.size(), cbRealSize);
    emit layoutChanged();
}

uint32_t CKBModel::Register() {
    return m_kbfile.Register();
}

uint32_t CKBModel::RetrieveFromRemote() {
    return m_kbfile.RetrieveFromRemote();
}

uint32_t CKBModel::PushToRemote() {
    return m_kbfile.PushToRemote();
}
