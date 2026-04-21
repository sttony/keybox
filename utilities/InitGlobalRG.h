//
// Created by tongl on 3/1/2024.
//

#ifndef KEYBOX_INITGLOBALRG_H
#define KEYBOX_INITGLOBALRG_H

#ifdef __cplusplus
#include "CRandomGenerator.h"

extern CRandomGenerator g_RG;

extern "C" {
#endif

void InitGlobalRG();

#ifdef __cplusplus
}
#endif

#endif //KEYBOX_INITGLOBALRG_H
