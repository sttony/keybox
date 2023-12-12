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

    static const uint32_t AES_PADDING_MODE_NO    = 1;
    static const uint32_t AES_PADDING_MODE_PKCS7 = 2;

    uint32_t SHA256(const uint8_t* pPlanText,
                    size_t cbPlanTextSize,
                    std::vector<uint8_t>& Output);

    uint32_t AES256EnDecrypt(const uint8_t* pInputBuff,
                           size_t cbInputBuff,
                           const uint8_t* pKey,
                           const uint8_t* pIV,
                           uint32_t chain_mode,
                           uint32_t padding_mode,
                           bool bEncrypt,
                           std::vector<uint8_t>& vOutputBuff);

};


#endif //KEYBOX_CCIPHERENGINE_H
