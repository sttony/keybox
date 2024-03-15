#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <iomanip>
#include "../utilities/CipherEngine.h"
#include <iostream>

using namespace std;

static std::string blobToHexString(const std::vector<unsigned char> &sha256_buff) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (auto c: sha256_buff) {
        // Convert each char to its hexadecimal representation
        ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
    }
    return ss.str();
}

BOOST_AUTO_TEST_SUITE(CipherTestSuit)

    BOOST_AUTO_TEST_CASE(SHA256)
    {
        CCipherEngine encryptEngine;
        std::vector<unsigned char> sha256_buff;
        unsigned char input[32] = "aaa";
        encryptEngine.SHA256(input, 3, sha256_buff);

        // 47 bc e5 c7 4f 58 9f 48 67 db d5 7e 9c a9 f8 08 7d 8a 8e f6 58 26 17 2b 6c 29 80 9d 92 71 a7 cf
        string hexStr = blobToHexString(sha256_buff);
        BOOST_CHECK_EQUAL(hexStr, "9834876dcfb05cb167a5c24953eba58c4ac89b1adf57f28f2f9d09af107ee8f0");
    }

    BOOST_AUTO_TEST_CASE(AES256_Padding)
    {
        CCipherEngine encryptEngine;
        vector<unsigned char> key(32);
        vector<unsigned char> iv(16);
        iv[0] = '\1';

        const char plaintext[16] = {0};
        std::vector<unsigned char> encrypted_buff;
        encryptEngine.AES256EnDecrypt((unsigned char *) plaintext,
                                      16,
                                      key,
                                      iv,
                                      CCipherEngine::AES_CHAIN_MODE_CBC,
                                      CCipherEngine::AES_PADDING_MODE_PKCS7,
                                      true,
                                      encrypted_buff);


        string hexStr = blobToHexString(encrypted_buff);
        BOOST_CHECK_EQUAL(hexStr, "5275f3d86b4fb8684593133ebfa53cd3a32ee726721a38c529df5a9b0d3af07f");

        std::vector<unsigned char> decrypted_buff;
        encryptEngine.AES256EnDecrypt(&encrypted_buff[0],
                                      encrypted_buff.size(),
                                      key,
                                      iv,
                                      CCipherEngine::AES_CHAIN_MODE_CBC,
                                      CCipherEngine::AES_PADDING_MODE_PKCS7,
                                      false,
                                      decrypted_buff);
        hexStr = blobToHexString(decrypted_buff);
        BOOST_CHECK_EQUAL(hexStr, "0000000000000000000000000000000000000000000000000000000000000000");


    }

    BOOST_AUTO_TEST_CASE(AES256_No_Padding)
    {
        CCipherEngine encryptEngine;
        vector<unsigned char> key(32);
        vector<unsigned char> iv(16);
        iv[0] = '\1';

        const unsigned char plaintext[16] = {0};
        std::vector<unsigned char> encrypted_buff;
        encryptEngine.AES256EnDecrypt((unsigned char *) plaintext,
                                      16,
                                      key,
                                      iv,
                                      CCipherEngine::AES_CHAIN_MODE_CBC,
                                      CCipherEngine::AES_PADDING_MODE_NO,
                                      true,
                                      encrypted_buff);


        string hexStr = blobToHexString(encrypted_buff);
        BOOST_CHECK_EQUAL(hexStr, "5275f3d86b4fb8684593133ebfa53cd3");

        std::vector<unsigned char> decrypted_buff;
        encryptEngine.AES256EnDecrypt(&encrypted_buff[0],
                                      encrypted_buff.size(),
                                      key,
                                      iv,
                                      CCipherEngine::AES_CHAIN_MODE_CBC,
                                      CCipherEngine::AES_PADDING_MODE_NO,
                                      false,
                                      decrypted_buff);
        hexStr = blobToHexString(decrypted_buff);
        BOOST_CHECK_EQUAL(hexStr, "00000000000000000000000000000000");
    }

    BOOST_AUTO_TEST_CASE(Kdbx_key_Derivation)
    {
        std::vector<unsigned char> derived_key;
        CCipherEngine cipherEngine;

        std::vector<unsigned char> derivation_seed = {
                0x30, 0xe2, 0xe6, 0xf4, 0x20, 0x14, 0x2f, 0x8e,
                0x71, 0xb7, 0xb5, 0xd3, 0x9d, 0x04, 0xeb, 0xa1,
                0x29, 0xfc, 0x5e, 0x43, 0x9c, 0x80, 0x59, 0x9d,
                0x97, 0x22, 0xbc, 0xc7, 0x57, 0x3b, 0x93, 0xc6
        };

        std::vector<unsigned char> masterSeed = {
                0x89, 0xd7, 0x2e, 0x7b, 0x56, 0x2f, 0xef, 0xcc,
                0xfa, 0x9e, 0x21, 0xd7, 0x6a, 0x87, 0x84, 0x8b,
                0x4f, 0x10, 0x7b, 0x6e, 0xf2, 0x9e, 0x1b, 0x36,
                0x89, 0x06, 0x7f, 0x21, 0x9e, 0x33, 0xd2, 0xaa
        };
        uint32_t derivation_round = 600000;
        cipherEngine.KeepassDerivativeKey(
                "aaa",
                derivation_seed,
                derivation_round,
                masterSeed,
                derived_key
        );

        auto hexStr = blobToHexString(derived_key);
        BOOST_CHECK_EQUAL(hexStr, "f375027f3ca786d16c72bcb8c0d8a3754089983b465a4670c2669df4bcf4f1ba");
    }

    BOOST_AUTO_TEST_CASE(PDKFD2_key_Derivation)
    {
        std::vector<unsigned char> derived_key;
        CCipherEngine cipherEngine;
        PBKDF2_256_PARAMETERS pdkdf2_parameters = {
                {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
                 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                },
                60000
        };

        cipherEngine.PBKDF2DerivativeKey("hello_world!", pdkdf2_parameters, derived_key);
        auto hexStr = blobToHexString(derived_key);
        BOOST_CHECK_EQUAL(hexStr, "e382fed3606bd2758c6ccb45ab4853be81423c4ee04ae1e2ca8b8071778ea460");
    }

BOOST_AUTO_TEST_SUITE_END()

