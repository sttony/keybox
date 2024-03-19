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
    static const uint8_t END_OF_HEADER;
    static const uint8_t KEEPASS_Comment;
    static const uint8_t KEEPASS_CipherID;
    static const uint8_t KEEPASS_CompressionFlags;
    static const uint8_t KEEPASS_MasterSeed;
    static const uint8_t KEEPASS_TransformSeed;
    static const uint8_t KEEPASS_TransformRounds;
    static const uint8_t KEEPASS_EncryptionIV;
    static const uint8_t KEEPASS_InnerRandomStreamKey;
    static const uint8_t KEEPASS_StreamStartBytes;
    static const uint8_t KEEPASS_InnerRandomStreamID;
    static const uint8_t KEEPASS_KdfParameters;
    static const uint8_t KEEPASS_PublicCustomData;

    static const uint8_t KEYBOX_PBKDF2_PARAM;
    static const uint8_t KEYBOX_HMAC_SIGNATURE;

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
    uint32_t SetEntry(const CPwdEntry& _entry);

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
