#include <iostream>
#include "utilities/CipherEngine.h"


int main() {
    CCipherEngine encryptEngine;
    std::vector<uint8_t>  sha256_buff;
    uint8_t input[32] ="aaa";
    encryptEngine.SHA256(input, 3, sha256_buff);
    return 0;
}
