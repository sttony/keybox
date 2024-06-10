//
// Created by tongl on 12/18/2023.
//

#include "CRandomGenerator.h"
#include "CipherEngine.h"

#include <memory>
#include <algorithm>
#include <cstring>

using namespace std;

CRandomGenerator::CRandomGenerator(uint32_t _type) {
    m_type = _type;
}

uint32_t CRandomGenerator::init(std::vector<unsigned char> vKey32, std::array<unsigned char, 8> vIV8) {
    vector<unsigned char> real_key;
    CCipherEngine cipherEngine;
    cipherEngine.SHA256(&vKey32[0], vKey32.size(), real_key);
    array<unsigned char, 32> salsa20_key = {};
    copy_n(real_key.begin(), 32, salsa20_key.begin());
    m_salsa20 = make_unique<Salsa20Cipher>(salsa20_key, vIV8);
    m_buffer = m_salsa20->nextBlock();
    m_buffer_used = 0;
    return 0;
}

vector<unsigned char> CRandomGenerator::GetNextBytes(uint32_t num) {
    vector<unsigned char> result;
    this->GetNextBytes(num, result);
    return std::move(result);
}

uint32_t CRandomGenerator::GetNextBytes(uint32_t num, vector<unsigned char> &output) {
    output.resize(num);
    uint32_t temp_num = num;


    while (temp_num > 0) {
        if (m_buffer.size() - m_buffer_used >= temp_num) {
            memcpy(&output[num - temp_num], &m_buffer[m_buffer_used], temp_num);
            m_buffer_used += temp_num;
            temp_num = 0;
            if (m_buffer_used == m_buffer.size()) {
                m_buffer = m_salsa20->nextBlock();
                m_buffer_used = 0;
            }
        } else {
            memcpy(&output[num - temp_num], &m_buffer[m_buffer_used], m_buffer.size() - m_buffer_used);
            temp_num -= m_buffer.size() - m_buffer_used;
            m_buffer = m_salsa20->nextBlock();
            m_buffer_used = 0;

        }
    }
    return 0;
}

uint32_t CRandomGenerator::GetNextInt32() {
    vector<unsigned char> vOut;
    GetNextBytes(4, vOut);
    uint32_t result;
    memcpy(&result, vOut.data(), 4);
    return result;
}