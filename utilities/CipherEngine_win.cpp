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

uint32_t CCipherEngine::SHA256(const uint8_t *pPlanText, size_t cbPlanTextSize, std::vector<uint8_t> &Output) {
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