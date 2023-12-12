//
// Created by tongl on 12/12/2023.
//
#include "KeepassReader.h"
#include "CipherEngine.h"
#include "error_code.h"

uint32_t
CKdbxReader::TransformKey(const std::string &key, const std::vector<char> &seed, std::vector<char> &iv, int around) {
    if(seed.size() != 32 ||  iv.size() != 16){
        return ERROR_INVALID_PARAMETER;
    }
    std::vector<char> sha256_key(32);
    CCipherEngine cipherEngine;
    cipherEngine.SHA256(key.c_str(), key.size(), sha256_key);



    return 0;
}
