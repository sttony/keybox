//
// Created by tongl on 1/2/2024.
//

#include "CKBFile.h"
#include "error_code.h"

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


uint32_t CKBFileHeader::Deserialize(const unsigned char *pBuffer, uint32_t cbBufferSize) {
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
    return 0;
}

uint32_t CKBFileHeader::Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize) {
    cbRealSize = 0;
    cbRealSize += sizeof(m_signature);
    cbRealSize += sizeof(m_version);

    cbRealSize += 1 + 2 + m_encryption_iv.size() * sizeof(uint8_t);
    cbRealSize += 1 + 2 + sizeof(m_key_derivative_parameters);
    cbRealSize += 1;

    if (cbRealSize > cbBufferSize) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    uint32_t offset = 0;
    if(write_field(pBuffer, cbBufferSize, offset, m_signature)){
        return ERROR_BUFFER_TOO_SMALL;
    }

    if(write_field(pBuffer, cbBufferSize, offset, m_version)){
        return ERROR_BUFFER_TOO_SMALL;
    }

    // write KEYBOX_PBKDF2_PARAM
    if(write_field(pBuffer, cbBufferSize, offset, KEYBOX_PBKDF2_PARAM)){
        return ERROR_BUFFER_TOO_SMALL;
    }
    uint16_t field_size = sizeof(m_key_derivative_parameters);
    if(write_field(pBuffer, cbBufferSize, offset, field_size)){
        return ERROR_BUFFER_TOO_SMALL;
    }
    if(write_field(pBuffer, cbBufferSize, offset, m_key_derivative_parameters)){
        return ERROR_BUFFER_TOO_SMALL;
    }

    // write KEEPASS_EncryptionIV
    if(write_field(pBuffer, cbBufferSize, offset, KEEPASS_EncryptionIV)){
        return ERROR_BUFFER_TOO_SMALL;
    }
    field_size = m_encryption_iv.size();
    if(write_field(pBuffer, cbBufferSize, offset, field_size)){
        return ERROR_BUFFER_TOO_SMALL;
    }
    memcpy(pBuffer+offset, &m_encryption_iv[0], m_encryption_iv.size());


    // write END_OF_HEADER
    if(write_field(pBuffer, cbBufferSize, offset, END_OF_HEADER)){
        return ERROR_BUFFER_TOO_SMALL;
    }
    return 0;
}

CKBFileHeader::CKBFileHeader(): m_encryption_iv(32) {
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
