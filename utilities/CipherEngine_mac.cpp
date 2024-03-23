#include "error_code.h"
#include "CipherEngine.h"
#include <iostream>
#include <functional>
#include <utility>
#include <CommonCrypto/CommonDigest.h>
#include <CommonCrypto/CommonCrypto.h>
#include <CommonCrypto/CommonHMAC.h>

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
                               size_t cbInputBuffSize,
                               const vector<unsigned char> &vKey,
                               const vector<unsigned char> &vIV,
                               uint32_t chain_mode,
                               uint32_t padding_mode,
                               bool bEncrypt,
                               vector<unsigned char> &vOutputBuff) {
    if (vKey.size() != 32 || vIV.size() != 16) {
        return ERROR_INVALID_PARAMETER;
    }
    CCCryptorStatus status;
    CCOperation op = kCCEncrypt;
    if( !bEncrypt) op = kCCDecrypt;

    CCOptions options  = 0;
    switch(chain_mode){
        case AES_CHAIN_MODE_CBC:

            break;
        case AES_CHAIN_MODE_ECB:
            options |= kCCOptionECBMode;
            break;
        default:
            return ERROR_INVALID_PARAMETER;
    }



    if(padding_mode == AES_PADDING_MODE_NO){
        //
    }
    else if ( padding_mode == AES_PADDING_MODE_PKCS7){
        options |= kCCOptionPKCS7Padding;
    }


    size_t cbRealOutPutSize = 0;
    status = CCCrypt(op, kCCAlgorithmAES, options,
                     vKey.data(), vKey.size(),
                     vIV.data(),
                     pInputBuff, cbInputBuffSize,
                     nullptr, 0,
                     &cbRealOutPutSize
                     );
    if(status != kCCBufferTooSmall){
        return status;
    }
    vOutputBuff.resize(cbRealOutPutSize);
    status = CCCrypt(op, kCCAlgorithmAES, options,
                     vKey.data(), vKey.size(),
                     vIV.data(),
                     pInputBuff, cbInputBuffSize,
                     vOutputBuff.data(), vOutputBuff.size(),
                     &cbRealOutPutSize
    );
    return status;
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
    CCCryptorStatus status;
    vOutput.resize(32);
    status = CCKeyDerivationPBKDF(kCCPBKDF2,
                                  sKey.data(), sKey.size(), pbkdf2256Parameters.Salt, 32, kCCPRFHmacAlgSHA256,
                                  pbkdf2256Parameters.num_rounds, vOutput.data(), vOutput.size());
    return status;
}

uint32_t
CCipherEngine::HMAC_SHA256(const vector<unsigned char> &key, const unsigned char *pInputBuff, size_t cbInputBuff, vector<unsigned char> &Output) {
    CCHmacContext  ctx;
    CCHmacInit(&ctx, kCCHmacAlgSHA256, key.data(), key.size());
    CCHmacUpdate(&ctx, pInputBuff, cbInputBuff);
    Output.resize(32);
    CCHmacFinal(&ctx, Output.data());
    return 0;
}
