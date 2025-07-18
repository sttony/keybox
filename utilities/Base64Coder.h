//
// Created by tongl on 12/19/2023.
//

#ifndef KEYBOX_BASE64CODER_H
#define KEYBOX_BASE64CODER_H

#include <vector>
#include <string>
#include <cstdint>

class Base64Coder {
public:
    uint32_t Encode(const unsigned char *pInput, uint32_t cbInput, std::string &vOutput);

    uint32_t Decode(const std::string &sInput, std::vector<unsigned char> &vOutput);
};
std::vector<unsigned char> hex_to_bytes(const std::string& hex);

#endif //KEYBOX_BASE64CODER_H
