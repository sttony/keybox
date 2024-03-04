//
// Created by tongl on 3/1/2024.
//
#include <Windows.h>
#include "CipherEngine.h"
#include "InitGlobalRG.h"

using namespace std;
extern RandomGenerator g_RG;

void InitGlobalRG(){
    DWORD current_pid = GetProcessId(GetCurrentProcess());
    POINT mouse_position ={};
    GetCursorPos(&mouse_position);
    DWORD mouse_pos = mouse_position.x  |  mouse_position.y<<16;
    DWORD current_tick = GetTickCount();

    DWORD final =  current_pid + mouse_pos + current_tick;

    CCipherEngine cipherEngine;
    vector<unsigned char> buff32;
    cipherEngine.SHA256( reinterpret_cast<unsigned char*>(&final), sizeof(current_pid), buff32);
    static const std::array<unsigned char, 8> vIV = {0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a};
    g_RG.init(buff32, vIV);
}