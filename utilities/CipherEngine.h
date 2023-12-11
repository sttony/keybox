//
// Created by tongl on 12/11/2023.
//

#ifndef KEYBOX_CCIPHERENGINE_H
#define KEYBOX_CCIPHERENGINE_H

#include <cstdint>
#include <vector>

class CCipherEngine {
public:
    static const uint32_t AES_CHAIN_MODE_CBC = 1;
    static const uint32_t AES_CHAIN_MODE_ECB = 4;
    static const uint32_t AES_CHAIN_MODE_CCM = 2;
    static const uint32_t AES_CHAIN_MODE_CFB = 3;
    static const uint32_t AES_CHAIN_MODE_GCM = 5;

    uint32_t SHA256(const uint8_t* pPlanText,
                    size_t cbPlanTextSize,
                    std::vector<uint8_t>& Output);

    uint32_t AES256Encrypt(const uint8_t* pPlanText,
                           size_t cbPlanTextSize,
                           const uint8_t* pKey,
                           const uint8_t* pIV,
                           uint32_t chain_mode,
                           std::vector<uint8_t>& Output);
    uint32_t AES25Decrypt(const uint8_t* pCipherText,
                          size_t cbCipherText,
                          const uint8_t* pKey,
                          const uint8_t* pIV,
                          uint32_t chain_mode,
                          std::vector<uint8_t>& Output);
};


#endif //KEYBOX_CCIPHERENGINE_H
