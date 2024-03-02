//
// Created by tongl on 12/11/2023.
//

#ifndef KEYBOX_ERROR_CODE_H
#define KEYBOX_ERROR_CODE_H
#include <cstdint>

const static uint32_t ERROR_INVALID_PARAMETER   =  0x00000001;
const static uint32_t ERROR_BUFFER_TOO_SMALL    =  0x00000002;
const static uint32_t ERROR_FILE_FORMAT         =  0x00000003;
const static uint32_t ERROR_UNEXPECT_RG_FAILURE =  0x00000004;
const static uint32_t ERROR_DUPLICATE_KEY       =  0x00000005;
const static uint32_t ERROR_MASTER_KEY_INVALID  =  0x00000006;
const static uint32_t ERROR_INVALID_BASE64      =  0x00000007;
const static uint32_t ERROR_INVALID_JSON        =  0x00000008;
const static uint32_t ERROR_FILE_IO             =  0x00000009;
#endif //KEYBOX_ERROR_CODE_H
