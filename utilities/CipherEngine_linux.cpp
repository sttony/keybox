#include "error_code.h"
#include "CipherEngine.h"
#include <iostream>
#include <functional>
#include <utility>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/sha.h>

using namespace std;

uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, std::vector<unsigned char> &Output) {

    return 0;
}

uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, unsigned char *pOutput, size_t cbOutput) {

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

