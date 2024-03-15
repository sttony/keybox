//
// Created by tongl on 12/18/2023.
//

#include "Salsa20Cipher.h"
#include <cassert>


Salsa20Cipher::Salsa20Cipher(std::array<unsigned char, 32> vKey32, std::array<unsigned char, 8> vIV8) {
    m_s[0] = 0x61707865;
    m_s[5] = 0x3320646E;
    m_s[10] = 0x79622D32;
    m_s[15] = 0x6B206574;

    m_s[1] = *((uint32_t *) &vKey32[0]);
    m_s[2] = *((uint32_t *) &vKey32[4]);
    m_s[3] = *((uint32_t *) &vKey32[8]);
    m_s[4] = *((uint32_t *) &vKey32[12]);
    m_s[11] = *((uint32_t *) &vKey32[16]);
    m_s[12] = *((uint32_t *) &vKey32[20]);
    m_s[13] = *((uint32_t *) &vKey32[24]);
    m_s[14] = *((uint32_t *) &vKey32[28]);

    m_s[6] = *((uint32_t *) &vIV8[0]);
    m_s[7] = *((uint32_t *) &vIV8[4]);
    m_s[8] = 0; // Counter, low
    m_s[9] = 0; // Counter, high
}

Salsa20Cipher::~Salsa20Cipher() {
    memset(&m_s[0], 0, m_s.size() * sizeof(uint32_t));
    memset(&m_x[0], 0, m_s.size() * sizeof(uint32_t));
}

static uint32_t RotateLeft32(uint32_t _v, int bits) {
    assert(bits < 32);
    return _v << bits | _v >> (32 - bits);
}

std::array<unsigned char, 64> Salsa20Cipher::nextBlock() {
    assert(m_s.size() == m_x.size());
    memcpy(&m_x[0], &m_s[0], sizeof(uint32_t) * m_s.size());
    for (int i = 0; i < 10; ++i) {
        m_x[4] ^= RotateLeft32(m_x[0] + m_x[12], 7);
        m_x[8] ^= RotateLeft32(m_x[4] + m_x[0], 9);
        m_x[12] ^= RotateLeft32(m_x[8] + m_x[4], 13);
        m_x[0] ^= RotateLeft32(m_x[12] + m_x[8], 18);


        m_x[9] ^= RotateLeft32(m_x[5] + m_x[1], 7);
        m_x[13] ^= RotateLeft32(m_x[9] + m_x[5], 9);
        m_x[1] ^= RotateLeft32(m_x[13] + m_x[9], 13);
        m_x[5] ^= RotateLeft32(m_x[1] + m_x[13], 18);

        m_x[14] ^= RotateLeft32(m_x[10] + m_x[6], 7);
        m_x[2] ^= RotateLeft32(m_x[14] + m_x[10], 9);
        m_x[6] ^= RotateLeft32(m_x[2] + m_x[14], 13);
        m_x[10] ^= RotateLeft32(m_x[6] + m_x[2], 18);

        m_x[3] ^= RotateLeft32(m_x[15] + m_x[11], 7);
        m_x[7] ^= RotateLeft32(m_x[3] + m_x[15], 9);
        m_x[11] ^= RotateLeft32(m_x[7] + m_x[3], 13);
        m_x[15] ^= RotateLeft32(m_x[11] + m_x[7], 18);

        m_x[1] ^= RotateLeft32(m_x[0] + m_x[3], 7);
        m_x[2] ^= RotateLeft32(m_x[1] + m_x[0], 9);
        m_x[3] ^= RotateLeft32(m_x[2] + m_x[1], 13);
        m_x[0] ^= RotateLeft32(m_x[3] + m_x[2], 18);

        m_x[6] ^= RotateLeft32(m_x[5] + m_x[4], 7);
        m_x[7] ^= RotateLeft32(m_x[6] + m_x[5], 9);
        m_x[4] ^= RotateLeft32(m_x[7] + m_x[6], 13);
        m_x[5] ^= RotateLeft32(m_x[4] + m_x[7], 18);

        m_x[11] ^= RotateLeft32(m_x[10] + m_x[9], 7);
        m_x[8] ^= RotateLeft32(m_x[11] + m_x[10], 9);
        m_x[9] ^= RotateLeft32(m_x[8] + m_x[11], 13);
        m_x[10] ^= RotateLeft32(m_x[9] + m_x[8], 18);

        m_x[12] ^= RotateLeft32(m_x[15] + m_x[14], 7);
        m_x[13] ^= RotateLeft32(m_x[12] + m_x[15], 9);
        m_x[14] ^= RotateLeft32(m_x[13] + m_x[12], 13);
        m_x[15] ^= RotateLeft32(m_x[14] + m_x[13], 18);
    }

    for (int i = 0; i < 16; ++i) m_x[i] += m_s[i];


    std::array<unsigned char, 64> result{};
    for (int i = 0; i < 16; ++i) {
        int i4 = i << 2;
        uint32_t xi = m_x[i];

        result[i4] = (unsigned char) xi;
        result[i4 + 1] = (unsigned char) (xi >> 8);
        result[i4 + 2] = (unsigned char) (xi >> 16);
        result[i4 + 3] = (unsigned char) (xi >> 24);
    }
    ++m_s[8];
    if (m_s[8] == 0) ++m_s[9];
    return result;
}
