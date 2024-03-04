//
// Created by tongl on 3/1/2024.
//

#ifndef KEYBOX_INITGLOBALRG_H
#define KEYBOX_INITGLOBALRG_H
#include "RandomGenerator.h"

RandomGenerator g_RG(RandomGenerator::Salsa20);
void InitRG();


#endif //KEYBOX_INITGLOBALRG_H
