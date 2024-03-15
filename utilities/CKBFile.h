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
#include "CPwdEntry.h"

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
    static const uint8_t KEYBOX_HMAC_SIGNATURE = 0x82;

private:
    uint64_t m_signature = 0;
    uint32_t m_version = 0;
    std::unordered_map<uint8_t, std::function<uint32_t(const unsigned char *, uint32_t &, uint16_t)>> m_fields2handler;

    PBKDF2_256_PARAMETERS m_key_derivative_parameters = {};
    std::vector<unsigned char> m_encryption_iv = {};

public:
    CKBFileHeader();

    uint32_t Deserialize(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    PBKDF2_256_PARAMETERS &GetDerivativeParameters() {
        return m_key_derivative_parameters;
    }

    const std::vector<unsigned char> &GetIV() {
        return m_encryption_iv;
    }

    uint32_t SetDerivativeParameters(const std::vector<unsigned char> &_salt, int num_round = 60000);
};

class CKBFile {
public:

    uint32_t Deserialize(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t LoadHeader(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t LoadPayload(const unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t Serialize(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t Lock(unsigned char *pBuffer, uint32_t cbBufferSize, uint32_t &cbRealSize);

    uint32_t AddEntry(CPwdEntry _entry);
    CPwdEntry GetEntry(int idx);
    uint32_t SetEntry(CPwdEntry _entry, int idx);

    // TODO: for testing, expose them.
    const std::vector<CPwdEntry> &GetEntries() const;

    CKBFileHeader &GetHeader();

    void SetMasterKey(std::vector<unsigned char> key, IRandomGenerator &irg);

    void SetMasterKey(CMaskedBlob p);

    CPwdEntry QueryEntryByTitle(const std::string &_title);

    uint32_t SetDerivativeParameters(const std::vector<unsigned char> &_salt, int num_round = 60000);

    const PBKDF2_256_PARAMETERS &GetDerivativeParameters() {
        return m_header.GetDerivativeParameters();
    }

private:
    CKBFileHeader m_header;
    std::vector<CPwdEntry> m_entries;
    CMaskedBlob m_master_key;
};


#endif //KEYBOX_CKBFILE_H
