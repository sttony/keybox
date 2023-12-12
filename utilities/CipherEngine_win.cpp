#include "error_code.h"
#include "CipherEngine.h"
#include "win32_handler.h"
#include <iostream>
#include <functional>
#include <utility>
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

#include <winternl.h>
#include <ntstatus.h>
#include <bcrypt.h>
#include <winerror.h>

uint32_t CCipherEngine::SHA256(const char *pPlanText, size_t cbPlanTextSize, std::vector<char> &Output) {
    Win32Handler<BCRYPT_ALG_HANDLE> sha256AlgHandle(NULL, [](BCRYPT_ALG_HANDLE _h) {
        BCryptCloseAlgorithmProvider(_h, 0);
    });
    Win32Handler<BCRYPT_HASH_HANDLE> sha256HashHandle(NULL, [](BCRYPT_HASH_HANDLE _h) {
        BCryptDestroyHash(_h);
    });
    NTSTATUS status = BCryptOpenAlgorithmProvider(
            sha256AlgHandle.ptr(),
            BCRYPT_SHA256_ALGORITHM,
            NULL,
            0
    );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    DWORD HashLength = 0;
    DWORD ResultLength = 0;
    status = BCryptGetProperty(
            sha256AlgHandle,
            BCRYPT_HASH_LENGTH,
            (PBYTE) &HashLength,
            sizeof(HashLength),
            &ResultLength,
            0);

    if (!NT_SUCCESS(status)) {
        return status;
    }
    status = BCryptCreateHash(
            sha256AlgHandle,
            sha256HashHandle.ptr(),
            NULL,
            0,
            NULL,
            0,
            0);


    status = BCryptHashData(
            sha256HashHandle,
            (PBYTE) pPlanText,
            cbPlanTextSize,
            0);

    Output.resize(HashLength);

    status = BCryptFinishHash(
            sha256HashHandle,
            (PBYTE) &Output[0],
            HashLength,
            0);

    return status;
}

uint32_t
CCipherEngine::AES256EnDecrypt(const char *pInputBuff,
                               size_t cbInputBuff,
                               const std::vector<char>& vKey,
                               const std::vector<char>& vIV,
                               uint32_t chain_mode,
                               uint32_t padding_mode,
                               bool bEncrypt,
                               std::vector<char> &vOutputBuff) {
    if(vKey.size() !=32 || vIV.size() != 16){
        return ERROR_INVALID_PARAMETER;
    }
    Win32Handler<BCRYPT_ALG_HANDLE> aesAlgHandle(NULL, [](BCRYPT_ALG_HANDLE _h) {
        BCryptCloseAlgorithmProvider(_h, 0);
    });
    NTSTATUS status = BCryptOpenAlgorithmProvider(
            aesAlgHandle.ptr(),
            BCRYPT_AES_ALGORITHM,
            NULL,
            0
    );

    if (!NT_SUCCESS(status)) {
        return status;
    }
    Win32Handler<BCRYPT_KEY_HANDLE> aesKeyHandle(NULL, [](BCRYPT_KEY_HANDLE _h) {
        BCryptDestroyKey(_h);
    });

    status = BCryptGenerateSymmetricKey(
            aesAlgHandle,
            aesKeyHandle.ptr(),
            NULL,
            0,
            (PBYTE) &vKey[0],
            32,
            0);
    if (!NT_SUCCESS(status)) {
        if(status == STATUS_BUFFER_TOO_SMALL){
            return ERROR_BUFFER_TOO_SMALL;
        }
        else{
            return status;
        }
    }
    auto bcrypt_chaining_mode = BCRYPT_CHAIN_MODE_NA;
    size_t sz_bcrypt_chaining_mode = sizeof(BCRYPT_CHAIN_MODE_NA);
    switch (chain_mode) {
        case AES_CHAIN_MODE_CBC:
            bcrypt_chaining_mode = BCRYPT_CHAIN_MODE_CBC;
            sz_bcrypt_chaining_mode = sizeof(BCRYPT_CHAIN_MODE_CBC);
            break;
        case AES_CHAIN_MODE_CCM:
            bcrypt_chaining_mode = BCRYPT_CHAIN_MODE_CCM;
            sz_bcrypt_chaining_mode = sizeof(BCRYPT_CHAIN_MODE_CCM);
            break;
        case AES_CHAIN_MODE_CFB:
            bcrypt_chaining_mode = BCRYPT_CHAIN_MODE_CFB;
            sz_bcrypt_chaining_mode = sizeof(BCRYPT_CHAIN_MODE_CFB);
            break;
        case AES_CHAIN_MODE_ECB:
            bcrypt_chaining_mode = BCRYPT_CHAIN_MODE_ECB;
            sz_bcrypt_chaining_mode = sizeof(BCRYPT_CHAIN_MODE_ECB);
            break;
        case AES_CHAIN_MODE_GCM:
            bcrypt_chaining_mode = BCRYPT_CHAIN_MODE_GCM;
            sz_bcrypt_chaining_mode = sizeof(BCRYPT_CHAIN_MODE_GCM);
            break;
        default:
            return ERROR_INVALID_PARAMETER;
    }

    status = BCryptSetProperty(
            aesKeyHandle,
            BCRYPT_CHAINING_MODE,
            (PBYTE) bcrypt_chaining_mode,
            sz_bcrypt_chaining_mode,
            0);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    DWORD dwPadding  = BCRYPT_BLOCK_PADDING;
    if(padding_mode == AES_PADDING_MODE_NO){
        dwPadding = 0;
    }

    if(bEncrypt){
        DWORD dwCipherTextLength = 0;
        status = BCryptEncrypt(
                aesKeyHandle,
                (PBYTE)pInputBuff,
                cbInputBuff,
                NULL,
                (PBYTE)&vIV[0],
                16,
                NULL,
                0,
                &dwCipherTextLength,
                dwPadding);
        if (!NT_SUCCESS(status)) {
            return status;
        }
        vOutputBuff.resize(dwCipherTextLength);

        std::vector<char> tempIVBuff  = vIV;
        status = BCryptEncrypt(
                aesKeyHandle,
                (PBYTE)pInputBuff,
                cbInputBuff,
                NULL,
                reinterpret_cast<PBYTE>(&tempIVBuff[0]),
                16,
                reinterpret_cast<PBYTE>(&vOutputBuff[0]),
                vOutputBuff.size(),
                &dwCipherTextLength,
                dwPadding);
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }
    else{
        DWORD dwPlainTextLength = 0;
        status = BCryptDecrypt(
                aesKeyHandle,
                (PBYTE)pInputBuff,
                cbInputBuff,
                NULL,
                (PBYTE)&vIV[0],
                16,
                NULL,
                0,
                &dwPlainTextLength,
                dwPadding);
        if (!NT_SUCCESS(status)) {
            return status;
        }
        vOutputBuff.resize(dwPlainTextLength);

        std::vector<char> tempIVBuff = vIV;
        status = BCryptDecrypt(
                aesKeyHandle,
                (PBYTE)pInputBuff,
                cbInputBuff,
                NULL,
                reinterpret_cast<PBYTE>(&tempIVBuff[0]),
                16,
                reinterpret_cast<PBYTE>(&vOutputBuff[0]),
                vOutputBuff.size(),
                &dwPlainTextLength,
                dwPadding);
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }
    return 0;
}
