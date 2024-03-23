//
// Created by Tong, Li [C] on 3/21/24.
//
#include <unistd.h>
#include <sys/time.h>
#include "CipherEngine.h"
#include "InitGlobalRG.h"

using namespace std;
RandomGenerator g_RG(RandomGenerator::Salsa20);

void InitGlobalRG() {
    uint32_t current_pid = getpid();
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint32_t final = current_pid + tv.tv_usec;
    CCipherEngine cipherEngine;
    vector<unsigned char> buff32;
    cipherEngine.SHA256(reinterpret_cast<unsigned char *>(&final), sizeof(current_pid), buff32);
    static const std::array<unsigned char, 8> vIV = {0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a};
    g_RG.init(buff32, vIV);
}