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

using namespace std;

uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, vector<unsigned char> &Output) {
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
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = BCryptHashData(
            sha256HashHandle,
            (PBYTE) pInput,
            cbInput,
            0);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    Output.resize(HashLength);
    status = BCryptFinishHash(
            sha256HashHandle,
            (PBYTE) &Output[0],
            HashLength,
            0);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    return status;
}

uint32_t CCipherEngine::SHA256(const unsigned char *pInput, size_t cbInput, unsigned char *pOutput, size_t cbOutput) {

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

    if (cbOutput < HashLength) {
        return ERROR_BUFFER_TOO_SMALL;
    }
    status = BCryptCreateHash(
            sha256AlgHandle,
            sha256HashHandle.ptr(),
            NULL,
            0,
            NULL,
            0,
            0);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = BCryptHashData(
            sha256HashHandle,
            (PBYTE) pInput,
            cbInput,
            0);
    if (!NT_SUCCESS(status)) {
        return status;
    }


    status = BCryptFinishHash(
            sha256HashHandle,
            (PBYTE) pOutput,
            HashLength,
            0);
    return status;
}

uint32_t
CCipherEngine::AES256EnDecrypt(const unsigned char *pInputBuff,
                               size_t cbInputBuff,
                               const vector<unsigned char> &vKey,
                               const vector<unsigned char> &vIV,
                               uint32_t chain_mode,
                               uint32_t padding_mode,
                               bool bEncrypt,
                               vector<unsigned char> &vOutputBuff) {
    if (vKey.size() != 32 || vIV.size() != 16) {
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
        if (status == STATUS_BUFFER_TOO_SMALL) {
            return ERROR_BUFFER_TOO_SMALL;
        } else {
            return status;
        }
    }
    auto bcrypt_chaining_mode = BCRYPT_CHAIN_MODE_NA;
    size_t sz_bcrypt_chaining_mode = sizeof(BCRYPT_CHAIN_MODE_NA);
    vector<unsigned char> tempIVBuff = vIV;
    PBYTE pIV = (PBYTE) &tempIVBuff[0];
    DWORD cbIV = 16;
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
            pIV = NULL;
            cbIV = 0;
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

    DWORD dwPadding = BCRYPT_BLOCK_PADDING;
    if (padding_mode == AES_PADDING_MODE_NO) {
        dwPadding = 0;
    }

    if (bEncrypt) {
        DWORD dwCipherTextLength = 0;
        status = BCryptEncrypt(
                aesKeyHandle,
                (PBYTE) pInputBuff,
                cbInputBuff,
                NULL,
                pIV,
                cbIV,
                NULL,
                0,
                &dwCipherTextLength,
                dwPadding);
        if (!NT_SUCCESS(status)) {
            return status;
        }
        vOutputBuff.resize(dwCipherTextLength);


        status = BCryptEncrypt(
                aesKeyHandle,
                (PBYTE) pInputBuff,
                cbInputBuff,
                NULL,
                pIV,
                cbIV,
                reinterpret_cast<PBYTE>(&vOutputBuff[0]),
                vOutputBuff.size(),
                &dwCipherTextLength,
                dwPadding);
        if (!NT_SUCCESS(status)) {
            return status;
        }
    } else {
        DWORD dwPlainTextLength = 0;
        status = BCryptDecrypt(
                aesKeyHandle,
                (PBYTE) pInputBuff,
                cbInputBuff,
                NULL,
                pIV,
                cbIV,
                NULL,
                0,
                &dwPlainTextLength,
                dwPadding);
        if (!NT_SUCCESS(status)) {
            return status;
        }
        vOutputBuff.resize(dwPlainTextLength);

        vector<unsigned char> tempIVBuff = vIV;
        status = BCryptDecrypt(
                aesKeyHandle,
                (PBYTE) pInputBuff,
                cbInputBuff,
                NULL,
                pIV,
                cbIV,
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

uint32_t CCipherEngine::KeepassDerivativeKey(const string &sKey,
                                             const vector<unsigned char> &vTransformSeed,
                                             uint32_t uNumRounds,
                                             const vector<unsigned char> &vMasterSeed,
                                             vector<unsigned char> &output) {
    static_assert(sizeof(NTSTATUS) == 4);
    if (vTransformSeed.size() != 32 || vMasterSeed.size() != 32) {
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
            (PBYTE) &vTransformSeed[0],
            32,
            0);

    Win32Handler<BCRYPT_ALG_HANDLE> sha256AlgHandle(NULL, [](BCRYPT_ALG_HANDLE _h) {
        BCryptCloseAlgorithmProvider(_h, 0);
    });
    Win32Handler<BCRYPT_HASH_HANDLE> sha256HashHandle(NULL, [](BCRYPT_HASH_HANDLE _h) {
        BCryptDestroyHash(_h);
    });
    status = BCryptOpenAlgorithmProvider(
            sha256AlgHandle.ptr(),
            BCRYPT_SHA256_ALGORITHM,
            NULL,
            0
    );
    if (!NT_SUCCESS(status)) {
        return status;
    }

    CCipherEngine cipherEngine;
    vector<unsigned char> sha256_key(32);
    //  Sha256(password)
    status = cipherEngine.SHA256((unsigned char *) sKey.c_str(), sKey.size(), &sha256_key[0], 32);
    // Sha256(sha256(password))
    status = cipherEngine.SHA256(&sha256_key[0], 32, &sha256_key[0], 32);

    //
    vector<unsigned char> tempIV(16);
    status = BCryptSetProperty(
            aesKeyHandle,
            BCRYPT_CHAINING_MODE,
            (PBYTE) BCRYPT_CHAIN_MODE_ECB,
            sizeof(BCRYPT_CHAIN_MODE_ECB),
            0);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    DWORD dwCipherTextLength = 0;
    for (int i = 0; i < uNumRounds; ++i) {
        status = BCryptEncrypt(
                aesKeyHandle,
                (PBYTE) &sha256_key[0],
                16,
                NULL,
                NULL,
                0,
                (PBYTE) &sha256_key[0],
                16,
                &dwCipherTextLength,
                0);
        if (!NT_SUCCESS(status)) {
            return status;
        }
        status = BCryptEncrypt(
                aesKeyHandle,
                (PBYTE) &sha256_key[16],
                16,
                NULL,
                NULL,
                0,
                (PBYTE) &sha256_key[16],
                16,
                &dwCipherTextLength,
                0);
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    // Sha256 again
    status = cipherEngine.SHA256(&sha256_key[0], 32, &sha256_key[0], 32);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    vector<unsigned char> compositKey(64);
    memcpy_s(&compositKey[0], 32, &vMasterSeed[0], 32);
    memcpy_s(&compositKey[32], 32, &sha256_key[0], 32);

    //Sha256 again
    output.resize(32);
    status = cipherEngine.SHA256(&compositKey[0], compositKey.size(), &output[0], 32);
    return status;
}


void CCipherEngine::CleanString(string &str) {
    memset(&str[0], 0, str.size());
}

uint32_t CCipherEngine::PBKDF2DerivativeKey(const string &sKey, const PBKDF2_256_PARAMETERS &pbkdf2256Parameters,
                                            vector<unsigned char> &vOutput) {
    Win32Handler<BCRYPT_ALG_HANDLE> hAlgorithm(NULL, [](BCRYPT_ALG_HANDLE _h) {
        BCryptCloseAlgorithmProvider(_h, 0);
    });
    NTSTATUS status = BCryptOpenAlgorithmProvider(
            hAlgorithm.ptr(),
            BCRYPT_SHA256_ALGORITHM,
            NULL,
            BCRYPT_ALG_HANDLE_HMAC_FLAG
    );

    if (!NT_SUCCESS(status)) {
        return status;
    }
    vOutput.resize(32);
    status = BCryptDeriveKeyPBKDF2(
            hAlgorithm,
            (PUCHAR) sKey.c_str(),
            sKey.size(),
            (PUCHAR) pbkdf2256Parameters.Salt,
            32,
            pbkdf2256Parameters.num_rounds,
            &vOutput[0],
            32,
            0
    );

    if (!NT_SUCCESS(status)) {

        STATUS_INVALID_HANDLE;

        return status;
    }
    return 0;
}

