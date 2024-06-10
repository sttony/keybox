#include "CKBFileHeader.h"
#include "error_code.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace std;

// make gcc happy
const uint8_t CKBFileHeader::END_OF_HEADER = 0;
const uint8_t CKBFileHeader::KEEPASS_Comment = 1;
const uint8_t CKBFileHeader::KEEPASS_CipherID = 2;
const uint8_t CKBFileHeader::KEEPASS_CompressionFlags = 3;
const uint8_t CKBFileHeader::KEEPASS_MasterSeed = 4;
const uint8_t CKBFileHeader::KEEPASS_TransformSeed = 5;
const uint8_t CKBFileHeader::KEEPASS_TransformRounds = 6;
const uint8_t CKBFileHeader::KEEPASS_EncryptionIV = 7;
const uint8_t CKBFileHeader::KEEPASS_InnerRandomStreamKey = 8;
const uint8_t CKBFileHeader::KEEPASS_StreamStartBytes = 9;
const uint8_t CKBFileHeader::KEEPASS_InnerRandomStreamID = 10;
const uint8_t CKBFileHeader::KEEPASS_KdfParameters = 11;
const uint8_t CKBFileHeader::KEEPASS_PublicCustomData = 12;

const uint8_t CKBFileHeader::KEYBOX_PBKDF2_PARAM = 0x81;
const uint8_t CKBFileHeader::KEYBOX_HMAC_SIGNATURE = 0x82;

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

    cbRealSize += 1 + 2 + m_encryption_iv.size();
    cbRealSize += 1 + 2 + sizeof(m_key_derivative_parameters);
    cbRealSize += 1 + 2 + m_hmac_sha256_signature.size();
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
    memcpy(pBuffer + offset, m_encryption_iv.data(), m_encryption_iv.size());
    offset += m_encryption_iv.size();

    // write KEYBOX_HMAC_SIGNATURE
    if (write_field(pBuffer, cbBufferSize, offset, KEYBOX_HMAC_SIGNATURE)) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    field_size = m_hmac_sha256_signature.size();
    if (write_field(pBuffer, cbBufferSize, offset, field_size)) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    memcpy(pBuffer + offset, m_hmac_sha256_signature.data(), m_hmac_sha256_signature.size());
    offset += m_hmac_sha256_signature.size();

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

CKBFileHeader::CKBFileHeader() : m_encryption_iv(16), m_hmac_sha256_signature(32) {
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

    m_fields2handler[KEYBOX_HMAC_SIGNATURE] = [this](const unsigned char *p, uint32_t &offset, uint16_t cbSize) {
        m_hmac_sha256_signature.resize(cbSize);
        m_hmac_sha256_signature = std::vector<unsigned char>(p + offset, p + offset + cbSize);
        offset += cbSize;
        return 0u;
    };

    m_signature = 0x0BAD19840BAD1984;
    m_version = 1 << 16 | 0;

}

uint32_t CKBFileHeader::SetDerivativeParameters(const vector<unsigned char> &_salt, int num_round) {
    m_key_derivative_parameters.num_rounds = num_round;
    if (_salt.size() != 32) {
        return ERROR_INVALID_PARAMETER;
    }
    copy(_salt.begin(), _salt.end(), m_key_derivative_parameters.Salt);
    return 0;
}

uint32_t CKBFileHeader::CalculateHMAC(const vector<unsigned char> &master_key, const unsigned char *pPayloadBuff,
                                      size_t cbPayloadSize) {
    CCipherEngine cipherEngine;
    return cipherEngine.HMAC_SHA256(master_key, pPayloadBuff, cbPayloadSize, m_hmac_sha256_signature);
}
