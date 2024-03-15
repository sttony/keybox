//
// Created by tongl on 12/18/2023.
//

#ifndef KEYBOX_SALSA20CIPHER_H
#define KEYBOX_SALSA20CIPHER_H

#include <vector>
#include <array>

class Salsa20Cipher {
private:
    std::vector<uint32_t> m_s = std::vector<uint32_t>(16); // State
    std::vector<uint32_t> m_x = std::vector<uint32_t>(16); // Working


public:
    Salsa20Cipher(std::array<unsigned char, 32> vKey32, std::array<unsigned char, 8> vIV8);

    ~Salsa20Cipher();

    std::array<unsigned char, 64> nextBlock();
};


#endif //KEYBOX_SALSA20CIPHER_H
