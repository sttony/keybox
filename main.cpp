#include <iostream>
#include "utilities/CipherEngine.h"


int main() {
    CCipherEngine encryptEngine;
    std::vector<char>  sha256_buff;
    char input[32] ="aaa";
    encryptEngine.SHA256(input, 3, sha256_buff);
    return 0;
}
