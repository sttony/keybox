

#ifndef KEYBOX_CKBFILEHEADER_H
#define KEYBOX_CKBFILEHEADER_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <functional>
#include "CipherEngine.h"

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
    static const uint8_t KEYBOX_SYNC_URL;

private:
    uint64_t m_signature = 0;
    uint32_t m_version = 0;
    std::unordered_map<uint8_t, std::function<uint32_t(const unsigned char *, uint32_t &, uint16_t)>> m_fields2handler;

    PBKDF2_256_PARAMETERS m_key_derivative_parameters = {};
    std::vector<unsigned char> m_encryption_iv = {};
    std::vector<unsigned char> m_hmac_sha256_signature = {};

    std::string m_sync_url;

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

    const std::vector<unsigned char>& GetHMACSignature(){
        return m_hmac_sha256_signature;
    }

    uint32_t SetDerivativeParameters(const std::vector<unsigned char> &_salt, int num_round = 60000);

    uint32_t CalculateHMAC(const std::vector<unsigned char>& master_key, const unsigned char* pPayloadBuff, size_t cbPayloadSize);
};


#endif //KEYBOX_CKBFILEHEADER_H
