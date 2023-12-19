
#include <boost/test/unit_test.hpp>
#include "../utilities/KeepassReader.h"
#include <iomanip>
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

BOOST_AUTO_TEST_SUITE(KdbxReaderTestSuit)

    BOOST_AUTO_TEST_CASE(SHA256)
    {
        CKdbxReader kdbxReader;
        std::vector<unsigned char> derived_key;
        std::string password = "aaa";
        std::vector<unsigned char> seed = {
            0x36, 0xd9, 0xa0, 0xb2, 0x03, 0xe6, 0xcf, 0xf6,
            0x44, 0x1d, 0xa0, 0x87, 0xce, 0xe5, 0xed, 0x3d,
            0x9b, 0x61, 0x02, 0x71, 0xca, 0x27, 0x08, 0x0d,
            0xeb, 0xaa, 0xb7, 0x4e, 0xe4, 0x04, 0x02, 0x3d
        };


//        kdbxReader.TransformKey(password,
//                                seed,
//                                600000,
//                                derived_key);

        // 47 bc e5 c7 4f 58 9f 48 67 db d5 7e 9c a9 f8 08 7d 8a 8e f6 58 26 17 2b 6c 29 80 9d 92 71 a7 cf
        string hexStr = blobToHexString(derived_key);
        cout<<blobToHexString(derived_key)<<endl;
        BOOST_CHECK_EQUAL(hexStr, "bdad2aef16eb2bdbf5c463a8e3ec0188386c6752194b5162f6cc4b98b3120841");
    }

BOOST_AUTO_TEST_SUITE_END()