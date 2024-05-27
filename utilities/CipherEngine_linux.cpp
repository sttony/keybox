#include "error_code.h"
#include "CipherEngine.h"
#include <iostream>
#include <functional>
#include <utility>
#include <cstring>
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

    return 0;
}

uint32_t CCipherEngine::KeepassDerivativeKey(const string &sKey,
                                             const vector<unsigned char> &vTransformSeed,
                                             uint32_t uNumRounds,
                                             const vector<unsigned char> &vMasterSeed,
                                             vector<unsigned char> &output){
    return 0;
}


void CCipherEngine::CleanString(string &str) {
    memset(&str[0], 0, str.size());
}

uint32_t CCipherEngine::PBKDF2DerivativeKey(const string &sKey, const PBKDF2_256_PARAMETERS &pbkdf2256Parameters,
                                            vector<unsigned char> &vOutput) {
    return 0;
}

uint32_t
CCipherEngine::HMAC_SHA256(const vector<unsigned char> &key, const unsigned char *pInputBuff, size_t cbInputBuff, vector<unsigned char> &Output) {
    unsigned char* digest = HMAC(EVP_sha256(), key.data(), key.size(), pInputBuff, cbInputBuff, NULL, NULL);
    Output.resize(32);
    memcpy(Output.data(), digest, 32);
    return 0;
}


