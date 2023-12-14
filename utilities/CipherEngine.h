//
// Created by tongl on 12/11/2023.
//

#ifndef KEYBOX_CCIPHERENGINE_H
#define KEYBOX_CCIPHERENGINE_H

#include <cstdint>
#include <vector>
#include <string>

class CCipherEngine {
public:
    static const uint32_t AES_CHAIN_MODE_CBC = 1;
    static const uint32_t AES_CHAIN_MODE_ECB = 4;
    static const uint32_t AES_CHAIN_MODE_CCM = 2;
    static const uint32_t AES_CHAIN_MODE_CFB = 3;
    static const uint32_t AES_CHAIN_MODE_GCM = 5;

    static const uint32_t AES_PADDING_MODE_NO    = 1;
    static const uint32_t AES_PADDING_MODE_PKCS7 = 2;

    uint32_t SHA256(const unsigned char* pPlanText,
                    size_t cbPlanTextSize,
                    std::vector<unsigned char>& Output);

    uint32_t AES256EnDecrypt(const unsigned char* pInputBuff,
                           size_t cbInputBuff,
                           const std::vector<unsigned char>& vKey,
                           const std::vector<unsigned char>& vIV,
                           uint32_t chain_mode,
                           uint32_t padding_mode,
                           bool bEncrypt,
                           std::vector<unsigned char>& vOutputBuff);
    uint32_t  KeepassDerivateKey(const std::string &sKey,
                                 const std::vector<unsigned char> &vTransformSeed,
                                 uint32_t uNumRounds,
                                 const std::vector<unsigned char>& vMasterSeed,
                                 std::vector<unsigned char>& output);
};


#endif //KEYBOX_CCIPHERENGINE_H
