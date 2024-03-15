#include "error_code.h"
#include "CipherEngine.h"
#include <iostream>
#include <functional>
#include <utility>
#include <CommonCrypto/CommonDigest.h>

using namespace std;

uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, std::vector<unsigned char> &Output) {
    CC_SHA256_CTX sha256;
    CC_SHA256_Init(&sha256);

    CC_SHA256_Update(&sha256, pInput, static_cast<CC_LONG>(cbInput));
    Output.resize(32);
    CC_SHA256_Final(&Output[0], &sha256);
    return 0;
}

uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, unsigned char *pOutput, size_t cbOutput) {
    if (cbOutput < 32) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    CC_SHA256_CTX sha256;
    CC_SHA256_Init(&sha256);

    CC_SHA256_Update(&sha256, pInput, static_cast<CC_LONG>(cbInput));
    CC_SHA256_Final(pOutput, &sha256);
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

uint32_t CCipherEngine::KeepassDerivateKey(const string &sKey,
                                           const vector<unsigned char> &vTransformSeed,
                                           uint32_t uNumRounds,
                                           const vector<unsigned char> &vMasterSeed,
                                           vector<unsigned char> &output) {

    return 0;
}

void CCipherEngine::CleanString(string &str) {
    memset(&str[0], 0, str.size());
}

uint32_t CCipherEngine::PBKDF2DerivativeKey(const string &sKey, const PBKDF2_256_PARAMETERS &pbkdf2256Parameters,
                                            vector<unsigned char> &vOutput) {
    return 0;
}


