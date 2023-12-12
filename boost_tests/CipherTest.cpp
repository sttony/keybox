#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <iomanip>
#include "../utilities/CipherEngine.h"
#include <iostream>
using namespace std;

std::string blobToHexString(const std::vector<uint8_t> &sha256_buff) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for(auto c : sha256_buff) {
        // Convert each char to its hexadecimal representation
        ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
    }
    return ss.str();
}

BOOST_AUTO_TEST_SUITE(CipherTestSuit)

    BOOST_AUTO_TEST_CASE(SHA256)
    {
        CCipherEngine encryptEngine;
        std::vector<uint8_t>  sha256_buff;
        uint8_t input[32] ="aaa";
        encryptEngine.SHA256(input, 3, sha256_buff);

        // 47 bc e5 c7 4f 58 9f 48 67 db d5 7e 9c a9 f8 08 7d 8a 8e f6 58 26 17 2b 6c 29 80 9d 92 71 a7 cf
        string hexStr = blobToHexString(sha256_buff);
        BOOST_CHECK_EQUAL(hexStr, "9834876dcfb05cb167a5c24953eba58c4ac89b1adf57f28f2f9d09af107ee8f0");
    }
BOOST_AUTO_TEST_SUITE_END()

