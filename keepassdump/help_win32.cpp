//
// Created by tongl on 12/29/2023.
//
#include "help.h"
#include <iostream>
#include <windows.h>

using namespace std;
std::string read_password_from_console()
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
    string s;
    getline(cin, s);
    SetConsoleMode(hStdin, mode & (ENABLE_ECHO_INPUT));
    return s;
}