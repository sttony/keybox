//
// Created by tongl on 1/2/2024.
//

#include "CKBFile.h"
#include "error_code.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

template<typename T>
uint32_t read_field(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &offset, T &output) {
    if (offset + sizeof(T) > cbBufferSize) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    output = *((T *) (pBuffer + offset));
    offset += sizeof(T);
    return 0;
}

template<typename T>
uint32_t write_field(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &offset, const T &field) {
    if (offset + sizeof(T) > cbBufferSize) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    memcpy(pBuffer + offset, &field, sizeof(field));
    offset += sizeof(T);
    return 0;
}


uint32_t CKBFileHeader::Deserialize(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    uint32_t offset = 0;
    if (read_field(pBuffer, cbBufferSize, offset, m_signature)) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    if (read_field(pBuffer, cbBufferSize, offset, m_version)) {
        return ERROR_BUFFER_TOO_SMALL;
    }

    while (true) {
        uint8_t fieldId = -1;
        if (read_field(pBuffer, cbBufferSize, offset, fieldId)) {
            return ERROR_BUFFER_TOO_SMALL;
        }
        uint16_t cbSize = -1;
        if (read_field(pBuffer, cbBufferSize, offset, cbSize)) {
            return ERROR_BUFFER_TOO_SMALL;
        }
        auto handler_it = m_fields2handler.find(fieldId);
        if (handler_it != m_fields2handler.end()) {
            handler_it->second(pBuffer, offset, cbSize);
        }
        if (fieldId == END_OF_HEADER) {
            break;
        }
    }
    cbRealSize = offset;
    return 0;
}

uint32_t CKBFileHeader::Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    cbRealSize = 0;
    cbRealSize += sizeof(m_signature);
    cbRealSize += sizeof(m_version);

    cbRealSize += 1 + 2 + m_encryption_iv.size() * sizeof(uint8_t);
    cbRealSize += 1 + 2 + sizeof(m_key_derivative_parameters);
    cbRealSize += 1 + 2;

    if (cbRealSize > cbBufferSize) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    uint32_t offset = 0;
    if (write_field(pBuffer, cbBufferSize, offset, m_signature)) {
        return ERROR_BUFFER_TOO_SMALL;
    }

    if (write_field(pBuffer, cbBufferSize, offset, m_version)) {
        return ERROR_BUFFER_TOO_SMALL;
    }

    // write KEYBOX_PBKDF2_PARAM
    if (write_field(pBuffer, cbBufferSize, offset, KEYBOX_PBKDF2_PARAM)) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    uint16_t field_size = sizeof(m_key_derivative_parameters);
    if (write_field(pBuffer, cbBufferSize, offset, field_size)) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    if (write_field(pBuffer, cbBufferSize, offset, m_key_derivative_parameters)) {
        return ERROR_BUFFER_TOO_SMALL;
    }

    // write KEEPASS_EncryptionIV
    if (write_field(pBuffer, cbBufferSize, offset, KEEPASS_EncryptionIV)) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    field_size = m_encryption_iv.size();
    if (write_field(pBuffer, cbBufferSize, offset, field_size)) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    memcpy(pBuffer + offset, &m_encryption_iv[0], m_encryption_iv.size());


    // write END_OF_HEADER
    if (write_field(pBuffer, cbBufferSize, offset, END_OF_HEADER)) {
        field_size = 0;
        if (write_field(pBuffer, cbBufferSize, offset, field_size)) {
            return ERROR_BUFFER_TOO_SMALL;
        }
        return ERROR_BUFFER_TOO_SMALL;
    }
    return 0;
}

CKBFileHeader::CKBFileHeader() : m_encryption_iv(16) {
    m_fields2handler[KEYBOX_PBKDF2_PARAM] = [this](const unsigned char *p, uint32_t &offset, uint16_t cbSize) {
        if (read_field(p, offset + cbSize, offset, this->m_key_derivative_parameters)) {
            return ERROR_BUFFER_TOO_SMALL;
        }
        return 0u;
    };

    m_fields2handler[KEEPASS_EncryptionIV] = [this](const unsigned char *p, uint32_t &offset, uint16_t cbSize) {
        m_encryption_iv.resize(cbSize);
        m_encryption_iv = std::vector<unsigned char>(p + offset, p + offset + cbSize);
        offset += cbSize;
        return 0u;
    };

    m_signature = 0x0BAD19840BAD1984;
    m_version = 1 << 16 | 0;

}

uint32_t CKBFileHeader::SetDerivativeParameters(const vector<unsigned char> &_salt, int num_round) {
    m_key_derivative_parameters.num_rounds = num_round;
    if( _salt.size() != 32){
        return ERROR_INVALID_PARAMETER;
    }
    copy(_salt.begin(), _salt.end(), m_key_derivative_parameters.Salt);
    return 0;
}

CPwdEntry CKBFile::QueryEntryByTitle(const string &_title) {
    for (const auto &entry: m_entries) {
        if (entry.GetTitle() == _title) {
            return entry;
        }
    }
    return {};
}

const vector<CPwdEntry> & CKBFile::GetEntries() const {
    return m_entries;
}

uint32_t CKBFile::Deserialize(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    if(m_master_key.size()==0){
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
    std::istringstream iss( (char*)(&decrypted_buff[0]));
    string temp = std::string(decrypted_buff.begin(), decrypted_buff.end());
    try {
        boost::property_tree::read_json(iss, entries_tree);
    }
    catch(exception &e){
        return ERROR_INVALID_JSON;
    }

    // Clear existing entries
    m_entries.clear();

    for (const auto& kv : entries_tree.get_child("entries")) {
        CPwdEntry entry;
        entry.fromJsonObj(kv.second);
        m_entries.push_back(std::move(entry));
    }
    return 0;
}

uint32_t CKBFile::Lock(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    uint32_t result = this->Serialize(pBuffer, cbBufferSize, cbRealSize);
    if(result != 0){
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

void CKBFile::SetMasterKey(std::vector<unsigned char> key, IRandomGenerator &irg) {
    m_master_key.Set(key, irg);
}

uint32_t CKBFile::SetDerivativeParameters(const vector<unsigned char> &_salt, int num_round) {
    return m_header.SetDerivativeParameters(_salt, num_round);
}

void CKBFile::SetMasterKey(CMaskedBlob p) {
    m_master_key = p;
}

uint32_t CKBFile::Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    cbRealSize = 0;
    uint32_t uResult = 0;
    uResult = m_header.Serialize(pBuffer, cbBufferSize, cbRealSize);

    if (uResult) {
        if( pBuffer== nullptr && uResult == ERROR_BUFFER_TOO_SMALL){
            // continue
        }
        else {
            return uResult;
        }
    }

    boost::property_tree::ptree pay_load;
    boost::property_tree::ptree  entries;
    for (auto & entry : m_entries) {
        entries.push_back({"", entry.toJsonObj()});
    }
    pay_load.add_child("entries", entries);

    std::ostringstream oss;
    boost::property_tree::write_json(oss, pay_load);
    string pay_load_json = oss.str();

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
        if(pBuffer == nullptr){
            cbRealSize += encrypted_buff.size();
            cipherEngine.CleanString(pay_load_json);
            return 0;
        }
        else{
            return ERROR_BUFFER_TOO_SMALL;
        }
    }
    memcpy(pBuffer + cbRealSize, &encrypted_buff[0], encrypted_buff.size());
    cbRealSize += encrypted_buff.size();
    cipherEngine.CleanString(pay_load_json);

    // clean
    return 0;
}
