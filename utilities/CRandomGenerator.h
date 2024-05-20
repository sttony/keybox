//
// Created by tongl on 12/18/2023.
//

#ifndef KEYBOX_CRANDOMGENERATOR_H
#define KEYBOX_CRANDOMGENERATOR_H

#include <array>
#include <memory>
#include "Salsa20Cipher.h"

struct IRandomGenerator {
    virtual uint32_t GetNextBytes(uint32_t num, std::vector<unsigned char> &output) = 0;
    virtual uint32_t GetNextInt32() = 0;
};

class CRandomGenerator : public IRandomGenerator {
private:
    uint32_t m_type = 0;
    std::unique_ptr<Salsa20Cipher> m_salsa20;

    std::array<unsigned char, 64> m_buffer{};
    uint32_t m_buffer_used = 0;
public:
    const static uint32_t Salsa20 = 0x01;

    explicit CRandomGenerator(uint32_t _type);

    uint32_t init(std::vector<unsigned char> vKey32, std::array<unsigned char, 8> vIV8);

    uint32_t GetNextBytes(uint32_t num, std::vector<unsigned char> &output) override;

    uint32_t GetNextInt32() override;
};


#endif //KEYBOX_CRANDOMGENERATOR_H
