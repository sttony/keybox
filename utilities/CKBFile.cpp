//
// Created by tongl on 1/2/2024.
//

#include "CKBFile.h"
#include "error_code.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

using namespace std;


CPwdEntry CKBFile::QueryEntryByTitle(const string &_title) {
    for (const auto &entry: m_entries) {
        if (entry.GetTitle() == _title) {
            return entry;
        }
    }
    return {};
}

const vector<CPwdEntry> &CKBFile::GetEntries() const {
    return m_entries;
}

uint32_t CKBFile::Deserialize(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    if (m_master_key.size() == 0) {
        return ERROR_MASTER_KEY_INVALID;
    }
    m_header.Deserialize(pBuffer, cbBufferSize, cbRealSize);
    CCipherEngine cipherEngine;
    vector<unsigned char> decrypted_buff;
    uint32_t uResult = 0;
    uResult = cipherEngine.AES256EnDecrypt(
            pBuffer + cbRealSize,
            cbBufferSize - cbRealSize,
            m_master_key.ShowBin(),
            m_header.GetIV(),
            CCipherEngine::AES_CHAIN_MODE_CBC,
            CCipherEngine::AES_PADDING_MODE_PKCS7,
            false,
            decrypted_buff
    );
    if (uResult) {
        return uResult;
    }
    // load json from decrypted_buff
    decrypted_buff.push_back('\0'); // append a zero
    boost::property_tree::ptree entries_tree;
    std::istringstream iss((char *) (&decrypted_buff[0]));
    string temp = std::string(decrypted_buff.begin(), decrypted_buff.end());
    try {
        boost::property_tree::read_json(iss, entries_tree);
    }
    catch (exception &e) {
        return ERROR_INVALID_JSON;
    }

    // Clear existing entries
    m_entries.clear();
    m_groups.clear();

    for (const auto &kv: entries_tree.get_child("entries")) {
        CPwdEntry entry;
        entry.fromJsonObj(kv.second);
        m_entries.push_back(std::move(entry));
    }
    for (const auto &kv: entries_tree.get_child("groups")) {
        CPwdGroup group("");
        group.fromJsonObj(kv.second);
        m_groups.push_back(std::move(group));
    }
    return 0;
}

uint32_t CKBFile::Lock(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    uint32_t result = this->Serialize(pBuffer, cbBufferSize, cbRealSize);
    if (result != 0) {
        return result;
    }
    m_entries.clear();
    return 0;
}

CKBFileHeader &CKBFile::GetHeader() {
    return m_header;
}

uint32_t CKBFile::AddEntry(CPwdEntry _entry) {
    for (const auto &entry: m_entries) {
        if (entry.GetID() == _entry.GetID()) {
            return ERROR_DUPLICATE_KEY;
        }
    }
    m_entries.push_back(_entry);
    return 0;
}

void CKBFile::SetMasterKey(std::vector<unsigned char> key, std::vector<unsigned char>&& onepad) {
    m_master_key.Set(key, std::move(onepad));
}

uint32_t CKBFile::SetDerivativeParameters(const vector<unsigned char> &_salt, int num_round) {
    return m_header.SetDerivativeParameters(_salt, num_round);
}

void CKBFile::SetMasterKey(CMaskedBlob p) {
    m_master_key = p;
}

uint32_t CKBFile::Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    cbRealSize = 0;
    boost::property_tree::ptree pay_load;
    // pwd entries
    boost::property_tree::ptree entries;
    for (auto &entry: m_entries) {
        entries.push_back({"", entry.toJsonObj()});
    }
    pay_load.add_child("entries", entries);

    // pwd groups
    boost::property_tree::ptree groups;
    for (auto &group: m_groups) {
        groups.push_back({"", group.toJsonObj()});
    }
    pay_load.add_child("groups", groups);

    std::ostringstream oss;
    boost::property_tree::write_json(oss, pay_load);
    string pay_load_json = oss.str();

    uint32_t uResult = 0;
    if(pBuffer != nullptr) {
        uResult = m_header.CalculateHMAC(m_master_key.ShowBin(),
                                         reinterpret_cast<const unsigned char *>(pay_load_json.data()),
                                         pay_load_json.size());
        if (uResult) {
            return uResult;
        }
    }

    uResult = m_header.Serialize(pBuffer, cbBufferSize, cbRealSize);
    if (uResult) {
        if (pBuffer == nullptr && uResult == ERROR_BUFFER_TOO_SMALL) {
            // continue
        } else {
            return uResult;
        }
    }

    vector<unsigned char> encrypted_buff;
    CCipherEngine cipherEngine;
    uResult = cipherEngine.AES256EnDecrypt(
            (unsigned char *) &pay_load_json[0],
            pay_load_json.size(),
            m_master_key.ShowBin(),
            m_header.GetIV(),
            CCipherEngine::AES_CHAIN_MODE_CBC,
            CCipherEngine::AES_PADDING_MODE_PKCS7,
            true,
            encrypted_buff
    );
    if (uResult) {
        return uResult;
    }
    if (cbBufferSize < cbRealSize + encrypted_buff.size()) {
        if (pBuffer == nullptr) {
            cbRealSize += encrypted_buff.size();
            cipherEngine.CleanString(pay_load_json);
            return 0;
        } else {
            return ERROR_BUFFER_TOO_SMALL;
        }
    }
    memcpy(pBuffer + cbRealSize, &encrypted_buff[0], encrypted_buff.size());
    cbRealSize += encrypted_buff.size();
    cipherEngine.CleanString(pay_load_json);

    // clean
    return 0;
}

uint32_t CKBFile::LoadHeader(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    cbRealSize = 0;
    uint32_t uResult = 0;
    uResult = m_header.Deserialize(pBuffer, cbBufferSize, cbRealSize);

    if (uResult) {
        if (pBuffer == nullptr && uResult == ERROR_BUFFER_TOO_SMALL) {
            // continue
        } else {
            return uResult;
        }
    }
    m_groups = {g_RootGroup};
    return 0;
}

uint32_t CKBFile::LoadPayload(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    CCipherEngine cipherEngine;
    vector<unsigned char> decrypted_buff;
    uint32_t uResult = 0;
    uResult = cipherEngine.AES256EnDecrypt(
            pBuffer,
            cbBufferSize,
            m_master_key.ShowBin(),
            m_header.GetIV(),
            CCipherEngine::AES_CHAIN_MODE_CBC,
            CCipherEngine::AES_PADDING_MODE_PKCS7,
            false,
            decrypted_buff
    );
    if (uResult) {
        return uResult;
    }

    // load json from decrypted_buff
    decrypted_buff.push_back('\0'); // append a zero
    boost::property_tree::ptree entries_tree;
    std::istringstream iss((char *) (&decrypted_buff[0]));
    string temp = iss.str();

    // calculate HMAC with temp
    vector<unsigned char> vHmacSignature;
    cipherEngine.HMAC_SHA256(m_master_key.ShowBin(), reinterpret_cast<const unsigned char *>(temp.c_str()), temp.size(), vHmacSignature);
    if(vHmacSignature != m_header.GetHMACSignature()){
        return ERROR_MASTER_KEY_INVALID;
    }

    try {
        boost::property_tree::read_json(iss, entries_tree);
    }
    catch (exception &e) {
        return ERROR_INVALID_JSON;
    }

    // Clear existing entries
    m_entries.clear();
    m_groups.clear();

    for (const auto &kv: entries_tree.get_child("entries")) {
        CPwdEntry entry;
        entry.fromJsonObj(kv.second);
        m_entries.push_back(std::move(entry));
    }

    for (const auto &kv: entries_tree.get_child("groups")) {
        CPwdGroup group("");
        group.fromJsonObj(kv.second);
        m_groups.push_back(std::move(group));
    }
    return 0;
}

CPwdEntry CKBFile::GetEntry(int idx) {
    if(idx >= m_entries.size()){
        return {};
    }
    return m_entries[idx];
}

uint32_t CKBFile::SetEntry(const CPwdEntry& _entry) {
    for(auto& pe: m_entries){
        if( pe.GetID() == _entry.GetID()){
            pe = _entry;
            return 0;
        }
    }
    return ERROR_ENTRY_NOT_FOUND;
}

const std::vector<CPwdGroup> &CKBFile::GetGroups() {
    return m_groups;
}

uint32_t CKBFile::RemoveGroup(const std::string uuid_str) {
    auto it = find_if(m_groups.begin(), m_groups.end(),  [uuid_str] (const auto& g){
       return g.GetID() == uuid_str;
    });
    if( it == m_groups.end()){
        return ERROR_ENTRY_NOT_FOUND;
    }
    m_groups.erase(it);

    // update entry with _uuid to root;
    for(auto& entry : m_entries){
        if( boost::uuids::to_string(entry.GetGroup()) == uuid_str){
            entry.SetGroup(g_RootGroupId);
        }
    }

    return 0;
}

uint32_t CKBFile::UpdateGroup(const std::string &uuid_str, const std::string &name) {
    for (auto &group: m_groups) {
        if (group.GetID() == uuid_str) {
            group.SetName(name);
            return 0;
        }
    }

    try{
        CPwdGroup pwdG = CPwdGroup(name,  boost::uuids::string_generator()(uuid_str));
        m_groups.push_back(pwdG);
    }
    catch(runtime_error e){
        return ERROR_INVALID_PARAMETER;
    }


    return 0;
}


