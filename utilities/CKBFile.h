//
// Created by tongl on 1/2/2024.
//

#ifndef KEYBOX_CKBFILE_H
#define KEYBOX_CKBFILE_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <functional>
#include "CipherEngine.h"

class CKBFileHeader {
public:
    // constants
    static const uint8_t END_OF_HEADER = 0;
    static const uint8_t KEEPASS_Comment = 1;
    static const uint8_t KEEPASS_CipherID = 2;
    static const uint8_t KEEPASS_CompressionFlags = 3;
    static const uint8_t KEEPASS_MasterSeed = 4;
    static const uint8_t KEEPASS_TransformSeed = 5;
    static const uint8_t KEEPASS_TransformRounds = 6;
    static const uint8_t KEEPASS_EncryptionIV = 7;
    static const uint8_t KEEPASS_InnerRandomStreamKey = 8;
    static const uint8_t KEEPASS_StreamStartBytes = 9;
    static const uint8_t KEEPASS_InnerRandomStreamID = 10;
    static const uint8_t KEEPASS_KdfParameters = 11;
    static const uint8_t KEEPASS_PublicCustomData = 12;

    static const uint8_t KEYBOX_PBKDF2_PARAM = 0x81;

private:
    uint64_t m_signature;
    uint32_t m_version;
    std::unordered_map<uint8_t, std::function<uint32_t(const unsigned char*, uint32_t&, uint16_t)>> m_fields2handler;

    PBKDF2_256_PARAMETERS m_key_derivative_parameters;

public:
    CKBFileHeader();
    uint32_t Deserialize(const unsigned char* pBuffer, uint32_t cbBufferSize);
    uint32_t Serialize(std::vector<unsigned char>& vOutput);
};

class CKBFile {

private:
    CKBFileHeader m_header;



};


#endif //KEYBOX_CKBFILE_H
