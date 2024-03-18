//
// Created by tongl on 12/12/2023.
//
#include "KeepassReader.h"
#include "CipherEngine.h"
#include "error_code.h"

#include <cstring>

uint32_t CKdbxReader::LoadHeader(FILE *fp) {
    size_t io_error = fread(&m_sig1, 4, 1, fp);
    if (io_error != 1) {
        return ERROR_FILE_FORMAT;
    }
    io_error = fread(&m_sig2, 4, 1, fp);
    if (io_error != 1) {
        return ERROR_FILE_FORMAT;
    }
    io_error = fread(&m_nVer, 4, 1, fp);
    if (io_error != 1) {
        return ERROR_FILE_FORMAT;
    }
    bool isNextAvailable = true;
    uint32_t error_no = 0;
    while (isNextAvailable) {
        error_no = ReadNextField(fp, isNextAvailable);
        if (error_no) {
            return error_no;
        }
    }
    cbTotalHeaderSize = ftell(fp);
    return 0;
}

uint32_t CKdbxReader::ReadNextField(FILE *fp, bool &isNextAvailable) {
    this->fields.emplace_back();
    size_t io_error = fread(&this->fields.back().btFieldID, 1, 1, fp);
    if (io_error != 1) {
        return ERROR_FILE_FORMAT;
    }
    io_error = fread(&this->fields.back().cbSize, 2, 1, fp);
    if (io_error != 1) {
        return ERROR_FILE_FORMAT;
    }
    this->fields.back().data.resize(this->fields.back().cbSize);
    io_error = fread(&this->fields.back().data[0], 1, this->fields.back().cbSize, fp);
    if (io_error != this->fields.back().cbSize) {
        return ERROR_FILE_FORMAT;
    }
    if (this->fields.back().btFieldID == KBDBXHeaderField::EndOfHeader) {
        isNextAvailable = false;
    } else {
        isNextAvailable = true;
    }
    return 0;
}

std::vector<unsigned char> CKdbxReader::getTransformSeed() {
    for (const auto &field: fields) {
        if (field.btFieldID == KBDBXHeaderField::TransformSeed) {
            return field.data;
        }
    }
    return {};
}

uint32_t CKdbxReader::getTransformRounds() {
    uint32_t result = 0;
    for (const auto &field: fields) {
        if (field.btFieldID == KBDBXHeaderField::TransformRounds) {
            memcpy(&result, &field.data[0], 4);
        }
    }
    return result;
}

std::vector<unsigned char> CKdbxReader::getEncryptionIV() {
    for (const auto &field: fields) {
        if (field.btFieldID == KBDBXHeaderField::EncryptionIV) {
            return field.data;
        }
    }
    return {};
}

std::vector<unsigned char> CKdbxReader::getMasterSeed() {
    for (const auto &field: fields) {
        if (field.btFieldID == KBDBXHeaderField::MasterSeed) {
            return field.data;
        }
    }
    return {};
}

std::vector<unsigned char> CKdbxReader::getInnerRandomStreamKey() {
    for (const auto &field: fields) {
        if (field.btFieldID == KBDBXHeaderField::InnerRandomStreamKey) {
            return field.data;
        }
    }
    return {};
}

