//
// Created by tongl on 12/18/2023.
//

#ifndef KEYBOX_RANDOMGENERATOR_H
#define KEYBOX_RANDOMGENERATOR_H

#include <array>
#include <memory>
#include "Salsa20Cipher.h"

class RandomGenerator {
private:
    uint32_t m_type = 0;
    std::unique_ptr<Salsa20Cipher> m_salsa20;

    std::array<unsigned char, 64> m_buffer{};
    uint32_t m_buffer_used = 0;
public:
    const static uint32_t Salsa20 = 0x01;
    explicit RandomGenerator(uint32_t _type);
    uint32_t init(std::vector<unsigned char> vKey32, std::array<unsigned char, 8> vIV8);

    uint32_t GetNextBytes(uint32_t num, std::vector<unsigned char>& output);
};


#endif //KEYBOX_RANDOMGENERATOR_H
