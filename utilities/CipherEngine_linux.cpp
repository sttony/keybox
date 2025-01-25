#include "error_code.h"
#include "CipherEngine.h"
#include <iostream>
#include <functional>
#include <utility>
#include <cstring>
#include <cmath>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>

using namespace std;

uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, std::vector<unsigned char> &Output) {
    Output.resize(32);

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();
    unsigned int lengthOfHash = 0;

    EVP_DigestInit_ex(mdctx, md, nullptr);
    EVP_DigestUpdate(mdctx, pInput, cbInput);
    EVP_DigestFinal_ex(mdctx, Output.data(), &lengthOfHash);
    EVP_MD_CTX_free(mdctx);

    return 0;
}

uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, unsigned char *pOutput, size_t cbOutput) {
    if (cbOutput < 32) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();
    unsigned int lengthOfHash = 0;

    EVP_DigestInit_ex(mdctx, md, nullptr);
    EVP_DigestUpdate(mdctx, pInput, cbInput);
    EVP_DigestFinal_ex(mdctx, pOutput, &lengthOfHash);
    EVP_MD_CTX_free(mdctx);
    return 0;
}

uint32_t
CCipherEngine::AES256EnDecrypt(const unsigned char *pInputBuff,
                               size_t cbInputBuff,
                               const vector<unsigned char> &vKey,
                               const vector<unsigned char> &vIV,
                               uint32_t chain_mode,
                               uint32_t padding_mode,
                               bool bEncrypt,
                               vector<unsigned char> &vOutputBuff) {
    if (vKey.size() != 32 || vIV.size() != 16) {
        return ERROR_INVALID_PARAMETER;
    }

    const EVP_CIPHER* hC = EVP_aes_256_cbc();
    switch(chain_mode){
        case AES_CHAIN_MODE_CBC:

            break;
        case AES_CHAIN_MODE_ECB:
            hC = EVP_aes_256_ecb();
            break;
        default:
            return ERROR_INVALID_PARAMETER;
    }
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();


    if (bEncrypt) {
        EVP_EncryptInit_ex(ctx, hC, nullptr, vKey.data(), vIV.data());
        if(padding_mode == AES_PADDING_MODE_NO){
            EVP_CIPHER_CTX_set_padding(ctx, false);
        }
        else if ( padding_mode == AES_PADDING_MODE_PKCS7){
            EVP_CIPHER_CTX_set_padding(ctx, true);
        }
        vOutputBuff.resize( (cbInputBuff /32 +1) *32);
        size_t cbRealOutPutSize = 0;
        EVP_EncryptUpdate(ctx,  vOutputBuff.data(), (int*)&cbRealOutPutSize, pInputBuff, (int)cbInputBuff);
        int len = 0;
        EVP_EncryptFinal_ex(ctx,  vOutputBuff.data() + cbRealOutPutSize, &len);
        vOutputBuff.resize(cbRealOutPutSize + len);
        EVP_CIPHER_CTX_free(ctx);
    }
    else{
        EVP_DecryptInit_ex(ctx, hC, nullptr, vKey.data(), vIV.data());
        if(padding_mode == AES_PADDING_MODE_NO){
            EVP_CIPHER_CTX_set_padding(ctx, false);
        }
        else if ( padding_mode == AES_PADDING_MODE_PKCS7){
            EVP_CIPHER_CTX_set_padding(ctx, true);
        }
        vOutputBuff.resize( ceil(cbInputBuff /32.0) *32);
        size_t cbRealOutPutSize = 0;
        EVP_DecryptUpdate(ctx, vOutputBuff.data(), (int*)&cbRealOutPutSize, pInputBuff, (int)cbInputBuff);
        int len = 0;
        EVP_DecryptFinal_ex(ctx,  vOutputBuff.data() + cbRealOutPutSize, &len);
        if ( padding_mode == AES_PADDING_MODE_NO){
            vOutputBuff.resize(cbRealOutPutSize + len);
        }
        EVP_CIPHER_CTX_free(ctx);
    }
    return 0;
}

uint32_t CCipherEngine::KeepassDerivativeKey(const string &sKey,
                                             const vector<unsigned char> &vTransformSeed,
                                             uint32_t uNumRounds,
                                             const vector<unsigned char> &vMasterSeed,
                                             vector<unsigned char> &output){
    uint32_t status  = 0;
    CCipherEngine cipherEngine;
    vector<unsigned char> sha256_key(32);
    //  Sha256(password)
    status = cipherEngine.SHA256((unsigned char *) sKey.c_str(), sKey.size(), &sha256_key[0], 32);
    // Sha256(sha256(password))
    status = cipherEngine.SHA256(&sha256_key[0], 32, &sha256_key[0], 32);

    vector<unsigned char> tempIV(16);
    size_t cbRealOutPutSize = 16;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_ecb(), nullptr, vTransformSeed.data(), nullptr);
    for (int i = 0; i < uNumRounds; ++i) {
        EVP_EncryptUpdate(ctx,  sha256_key.data(), (int*)&cbRealOutPutSize, sha256_key.data(), 16);
        EVP_EncryptUpdate(ctx,  sha256_key.data()+16, (int*)&cbRealOutPutSize, sha256_key.data()+16, 16);
    }

    // Sha256 again
    status = cipherEngine.SHA256(&sha256_key[0], 32, &sha256_key[0], 32);
    vector<unsigned char> compositKey(64);
    memcpy(&compositKey[0], &vMasterSeed[0], 32);
    memcpy(&compositKey[32], &sha256_key[0], 32);

    output.resize(32);
    status = cipherEngine.SHA256(&compositKey[0], compositKey.size(), &output[0], 32);
    return status;
}


void CCipherEngine::CleanString(string &str) {
    memset(&str[0], 0, str.size());
}

uint32_t CCipherEngine::PBKDF2DerivativeKey(const string &sKey, const PBKDF2_256_PARAMETERS &pbkdf2256Parameters,
                                            vector<unsigned char> &vOutput) {
    vOutput.resize(32);
    PKCS5_PBKDF2_HMAC(sKey.data(), sKey.size(), pbkdf2256Parameters.Salt, 32, pbkdf2256Parameters.num_rounds, EVP_sha256(), 32, vOutput.data());
    return 0;
}

uint32_t
CCipherEngine::HMAC_SHA256(const vector<unsigned char> &key, const unsigned char *pInputBuff, size_t cbInputBuff, vector<unsigned char> &Output) {
    unsigned char* digest = HMAC(EVP_sha256(), key.data(), key.size(), pInputBuff, cbInputBuff, NULL, NULL);
    Output.resize(32);
    memcpy(Output.data(), digest, 32);
    return 0;
}

uint32_t CCipherEngine::EC_Sign(const unsigned char *pInputBuff, size_t cbInputBuff, const vector<unsigned char> &vPrvKey,
                                vector<unsigned char> &vOutputBuff) {
    return 0;
}

uint32_t CCipherEngine::EC_Verify(const unsigned char *pInputBuff, size_t cbInputBuff, const vector<unsigned char> &vPubKey,
                                  const vector<unsigned char>& vDigitSignature) {
    return 0;
}
