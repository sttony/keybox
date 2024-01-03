//
// Created by tongl on 12/18/2023.
//

#include "RandomGenerator.h"
#include "CipherEngine.h"

#include <memory>

RandomGenerator::RandomGenerator(uint32_t _type) {
    m_type = _type;
}

uint32_t RandomGenerator::init(std::vector<unsigned char> vKey32, std::array<unsigned char, 8> vIV8) {
    std::vector<unsigned char> real_key ;
    CCipherEngine cipherEngine;
    cipherEngine.SHA256(&vKey32[0], vKey32.size(), real_key);
    std::array<unsigned char, 32> salsa20_key={};
    std::copy_n(real_key.begin(), 32, salsa20_key.begin());
    m_salsa20 = std::make_unique<Salsa20Cipher>(salsa20_key, vIV8);
    m_buffer = m_salsa20->nextBlock();
    m_buffer_used = 0;
    return 0;
}

uint32_t RandomGenerator::GetNextBytes(uint32_t num, std::vector<unsigned char> &output) {
    output.resize(num);
    uint32_t temp_num = num;


    while(temp_num>0) {
        if (m_buffer.size() - m_buffer_used >= temp_num) {
            memcpy(&output[num-temp_num], &m_buffer[m_buffer_used], temp_num);
            m_buffer_used += temp_num;
            temp_num=0;
            if( m_buffer_used == m_buffer.size()){
                m_buffer = m_salsa20->nextBlock();
                m_buffer_used = 0;
            }
        } else {
            memcpy(&output[num-temp_num], &m_buffer[m_buffer_used],m_buffer.size() - m_buffer_used);
            temp_num -= m_buffer.size() - m_buffer_used;
            m_buffer = m_salsa20->nextBlock();
            m_buffer_used = 0;

        }
    }
    return 0;
}
