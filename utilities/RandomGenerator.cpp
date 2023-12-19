//
// Created by tongl on 12/18/2023.
//

#include "RandomGenerator.h"

#include <memory>

RandomGenerator::RandomGenerator(uint32_t _type) {
    m_type = _type;
}

uint32_t RandomGenerator::init(std::array<unsigned char, 32> vKey32, std::array<unsigned char, 8> vIV8) {
    m_salsa20 = std::make_unique<Salsa20Cipher>(vKey32, vIV8);
    return 0;
}

uint32_t RandomGenerator::GetNextBytes(uint32_t num, std::vector<unsigned char> &output) {
    output.resize(num);
    uint32_t temp_num = num;

    while(temp_num>0) {
        if (m_buffer.size() - m_buffer_used >= temp_num) {
            memcpy_s(&output[0], num, &m_buffer[m_buffer_used], num);
        } else {
            memcpy_s(&output[0], m_buffer.size() - m_buffer_used, &m_buffer[m_buffer_used],
                     m_buffer.size() - m_buffer_used);
            temp_num -= m_buffer.size() - m_buffer_used;
            m_buffer = m_salsa20->nextBlock();
            m_buffer_used = 0;
        }
    }
    return 0;
}
