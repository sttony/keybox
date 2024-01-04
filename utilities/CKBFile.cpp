//
// Created by tongl on 1/2/2024.
//

#include "CKBFile.h"
#include "error_code.h"

template <typename T>
uint32_t read_field(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t& offset,  T& output){
    if( offset + sizeof(T) < cbBufferSize){
        return ERROR_BUFFER_TOO_SMALL;
    }
    output = *((T*)(pBuffer + offset));
    offset += sizeof(T);
    return 0;
}

uint32_t CKBFileHeader::Deserialize(const unsigned char *pBuffer, uint32_t cbBufferSize) {
    uint32_t offset = 0;
    if(read_field(pBuffer, cbBufferSize, offset, m_signature)){
        return ERROR_BUFFER_TOO_SMALL;
    }
    if(read_field(pBuffer, cbBufferSize, offset, m_version)){
        return ERROR_BUFFER_TOO_SMALL;
    }

    while(true){
        uint8_t fieldId = -1;
        if(read_field(pBuffer, cbBufferSize, offset, fieldId)){
            return ERROR_BUFFER_TOO_SMALL;
        }
        uint16_t cbSize = -1;
        if(read_field(pBuffer, cbBufferSize, offset, cbSize)){
            return ERROR_BUFFER_TOO_SMALL;
        }
        auto handler_it = m_fields2handler.find(fieldId);
        if( handler_it!= m_fields2handler.end()){
            handler_it->second(pBuffer, offset, cbSize);
        }
        if(fieldId == END_OF_HEADER){
            break;
        }
    }
    return 0;
}

uint32_t CKBFileHeader::Serialize(std::vector<unsigned char> &vOutput) {
    return 0;
}

CKBFileHeader::CKBFileHeader() {
    m_fields2handler[KEYBOX_PBKDF2_PARAM] = [this](const unsigned char* p, uint32_t& offset, uint16_t cbSize) {
        if(read_field(p, offset + cbSize, offset, this->m_key_derivative_parameters)){
            return ERROR_BUFFER_TOO_SMALL;
        }
        return 0u;
    };
}
