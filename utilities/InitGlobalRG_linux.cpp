//
// Created by sttony on 3/17/24.
//

#include "CipherEngine.h"
#include "InitGlobalRG.h"

using namespace std;
CRandomGenerator g_RG(CRandomGenerator::Salsa20);

void InitGlobalRG() {
    vector<unsigned char> buff32(32);
    static const std::array<unsigned char, 8> vIV = {0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a};
    g_RG.init(buff32, vIV);
}