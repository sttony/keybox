//
// Created by tongl on 12/12/2023.
//
#include "KeepassReader.h"
#include "CipherEngine.h"
#include "error_code.h"

uint32_t
CKdbxReader::TransformKey(const std::string &key,
                          const std::vector<unsigned char> &seed,
                          int uNumRounds,
                          std::vector<unsigned char>& output) {
    if(seed.size() != 32 ){
        return ERROR_INVALID_PARAMETER;
    }
    std::vector<unsigned char> sha256_key(32);
    output.resize(32);
    std::vector<unsigned char> sha256_key_2;
    std::vector<unsigned char> keypart1;
    std::vector<unsigned char> keypart2;
    CCipherEngine cipherEngine;
    cipherEngine.SHA256((unsigned char*)key.c_str(), key.size(), sha256_key);
    cipherEngine.SHA256(&sha256_key[0], 32, output);

    std::vector<unsigned char> tempIV(16);
    for(int i= 0; i< uNumRounds; ++i){
        cipherEngine.AES256EnDecrypt(
                &output[0],
                16,
                seed,
                tempIV,
                CCipherEngine::AES_CHAIN_MODE_ECB,
                CCipherEngine::AES_PADDING_MODE_NO,
                true,
                keypart1
                );
        cipherEngine.AES256EnDecrypt(
                &output[16],
                16,
                seed,
                tempIV,
                CCipherEngine::AES_CHAIN_MODE_ECB,
                CCipherEngine::AES_PADDING_MODE_NO,
                true,
                keypart2
        );
        memcpy_s(&output[0], 16, &keypart1[0], 16);
        memcpy_s(&output[16], 16, &keypart2[0], 16);
    }

    return 0;
}
