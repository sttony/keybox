#include "error_code.h"
#include "CipherEngine.h"
#include <iostream>
#include <functional>
#include <utility>


uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, std::vector<unsigned char> &Output) {


    return 0;
}

uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, unsigned char *pOutput, size_t cbOutput) {
    return 0;
}

uint32_t
CCipherEngine::AES256EnDecrypt(const unsigned char *pInputBuff,
                               size_t cbInputBuff,
                               const std::vector<unsigned char>& vKey,
                               const std::vector<unsigned char>& vIV,
                               uint32_t chain_mode,
                               uint32_t padding_mode,
                               bool bEncrypt,
                               std::vector<unsigned char> &vOutputBuff) {

    return 0;
}

uint32_t CCipherEngine::KeepassDerivateKey(const std::string &sKey,
                                           const std::vector<unsigned char> &vTransformSeed,
                                           uint32_t uNumRounds,
                                           const std::vector<unsigned char> &vMasterSeed,
                                           std::vector<unsigned char> &output) {

    return 0;
}


