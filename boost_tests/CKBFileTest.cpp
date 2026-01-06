//
// Created by tongl on 2/13/2024.
//
#include <boost/test/unit_test.hpp>
#include "../utilities/CKBFile.h"
#include "../utilities/error_code.h"
#include "../utilities/CGZcompressor.h"
#include <boost/property_tree/json_parser.hpp>
#include <iomanip>
#include <iostream>

using namespace std;

class TestRandomGenerator : public IRandomGenerator {
public:
    uint32_t GetNextBytes(uint32_t num, std::vector<unsigned char> &output) override {
        output.resize(num);
        for (int i = 0; i < num; ++i) {
            output[i] = (i + 1) % 256;
        }
        return 0;
    }
    vector<unsigned char> GetNextBytes(uint32_t num) override {
        vector<unsigned char> result;
        this->GetNextBytes(num, result);
        return std::move(result);
    }

    uint32_t GetNextInt32() override {
        vector<unsigned char> vOut;
        GetNextBytes(4, vOut);
        uint32_t result;
        memcpy(&result, vOut.data(), 4);
        return result;
    }
};
TestRandomGenerator g_RG;

BOOST_AUTO_TEST_SUITE(CKBFileTestSuit)

    BOOST_AUTO_TEST_CASE(HeaderReadWrite)
    {
        CKBFileHeader ckbFileHeader;
        uint32_t cbSize;
        uint32_t cbRealSize;
        vector<unsigned char> buff;
        ckbFileHeader.GetDerivativeParameters().num_rounds = 1000;
        ckbFileHeader.GetDerivativeParameters().Salt[0] = 0x13;

        uint32_t hresult = ckbFileHeader.Serialize(nullptr, 0, cbSize);

        BOOST_CHECK_EQUAL(hresult, ERROR_BUFFER_TOO_SMALL);
        buff.resize(cbSize);

        hresult = ckbFileHeader.Serialize(&buff[0], buff.size(), cbSize);
        BOOST_CHECK_EQUAL(hresult, 0);

        CKBFileHeader newHeader;
        newHeader.Deserialize(&buff[0], buff.size(), cbRealSize);

        BOOST_CHECK_EQUAL(newHeader.GetDerivativeParameters().num_rounds, 1000);
        BOOST_CHECK_EQUAL(newHeader.GetDerivativeParameters().Salt[0], 0x13);
        BOOST_CHECK_EQUAL(cbRealSize, cbSize);
    }

    BOOST_AUTO_TEST_CASE(LockTest)
    {
        CKBFile kbFile;
        kbFile.GetHeader().GetDerivativeParameters().num_rounds = 12345;
        kbFile.GetHeader().GetDerivativeParameters().Salt[0] = 0x12;
        kbFile.GetHeader().GetDerivativeParameters().Salt[1] = 0x34;
        CPwdEntry pwdEntry;
        pwdEntry.SetTitle("Test");
        string plainpwd = "abc123";
        TestRandomGenerator fakerg;
        pwdEntry.SetPassword(plainpwd, fakerg.GetNextBytes(plainpwd.size()));

        CPwdEntry pwdEntry2;
        pwdEntry2.SetTitle("Test2");
        string plainpwd2 = "password";
        pwdEntry2.SetPassword(plainpwd2, fakerg.GetNextBytes(plainpwd2.size()));

        kbFile.AddEntry(pwdEntry);
        kbFile.AddEntry(pwdEntry2);
        vector<unsigned char> key(32, 0x01);
        kbFile.SetMasterKey(key, fakerg.GetNextBytes(key.size()));

        std::unique_ptr<CAsymmetricKeyPair> asymmetric_key_pair = std::make_unique<CAsymmetricKeyPair>();
        asymmetric_key_pair->ReGenerate();
        kbFile.SetAsymKey(std::move(asymmetric_key_pair));

        vector<unsigned char> buff(1024 * 1024);
        uint32_t realSize = 0;
        kbFile.Lock(&buff[0], buff.size(), realSize);

        CKBFile testKBFile;
        key = vector<unsigned char>(32, 0x01);
        testKBFile.SetMasterKey(key, fakerg.GetNextBytes(key.size()));
        uint32_t cbRealSize = 0;
        testKBFile.Deserialize(&buff[0], realSize, cbRealSize);
        auto entry1 = testKBFile.QueryEntryByTitle("Test");
        BOOST_CHECK_EQUAL(entry1.GetID() == pwdEntry.GetID(), true);

        auto entry2 = testKBFile.QueryEntryByTitle("Test2");
        BOOST_CHECK_EQUAL(entry2.GetID() == pwdEntry2.GetID(), true);
    }

    BOOST_AUTO_TEST_CASE(GZPayloadLoadTest)
    {
        CKBFile kbFile;
        CPwdEntry pwdEntry;
        pwdEntry.SetTitle("GZTest");
        string plainpwd = "compressed_password";
        TestRandomGenerator fakerg;
        pwdEntry.SetPassword(plainpwd, fakerg.GetNextBytes(plainpwd.size()));
        kbFile.AddEntry(pwdEntry);

        vector<unsigned char> key(32, 0x01);
        kbFile.SetMasterKey(key, fakerg.GetNextBytes(key.size()));

        // Manually create a gzipped and encrypted buffer
        // 1. Get JSON
        // Since we don't have a direct way to get JSON, we'll use a trick or just simulate what Serialize does but with GZIP.
        
        // Actually, let's just use the Serialize logic but intercept it or replicate it.
        // We want to test LoadPayload.
        
        // Mocking the behavior of a file that was saved with GZIP.
        // For now, let's see how we can construct it.
        
        // We'll use boost property tree to create the same JSON structure.
        boost::property_tree::ptree root;
        boost::property_tree::ptree entries;
        boost::property_tree::ptree entry_node = pwdEntry.toJsonObj();
        entries.push_back(std::make_pair("", entry_node));
        root.add_child("entries", entries);

        stringstream ss;
        boost::property_tree::write_json(ss, root);
        string json_data = ss.str();

        // 2. Compress
        CGZcompressor compressor;
        vector<unsigned char> compressed_data;
        BOOST_REQUIRE_EQUAL(compressor.compressData((const unsigned char*)json_data.c_str(), json_data.size(), compressed_data), 0);

        // 3. Encrypt
        CCipherEngine cipherEngine;
        vector<unsigned char> iv = fakerg.GetNextBytes(16);
        vector<unsigned char> encrypted_payload;
        BOOST_REQUIRE_EQUAL(cipherEngine.AES256EnDecrypt(
            compressed_data.data(),
            compressed_data.size(),
            key,
            iv,
            CCipherEngine::AES_CHAIN_MODE_CBC,
            CCipherEngine::AES_PADDING_MODE_PKCS7,
            true,
            encrypted_payload
        ), 0);

        // 4. Set up Header
        CKBFile testKBFile;
        testKBFile.SetMasterKey(key, fakerg.GetNextBytes(key.size()));
        testKBFile.GetHeader().SetCompressFlag(1); // GZIP
        testKBFile.GetHeader().SetIV(iv);
        
        // We need to set HMAC on the header. HMAC is on the UNCOMPRESSED JSON data in current implementation.
        testKBFile.GetHeader().CalculateHMAC(key, (const unsigned char*)json_data.c_str(), json_data.size());
        
        // 5. Test LoadPayload
        uint32_t cbRealSize = 0;
        BOOST_REQUIRE_EQUAL(testKBFile.LoadPayload(encrypted_payload.data(), encrypted_payload.size(), cbRealSize), 0);
        
        auto entry = testKBFile.QueryEntryByTitle("GZTest");
        BOOST_CHECK_EQUAL(entry.GetID() == pwdEntry.GetID(), true);
    }

    BOOST_AUTO_TEST_CASE(GZSerializeTest)
    {
        CKBFile kbFile;
        CPwdEntry pwdEntry;
        pwdEntry.SetTitle("GZSerializeTest");
        string plainpwd = "compressed_password_serialize";
        TestRandomGenerator fakerg;
        pwdEntry.SetPassword(plainpwd, fakerg.GetNextBytes(plainpwd.size()));
        kbFile.AddEntry(pwdEntry);

        vector<unsigned char> key(32, 0x01);
        kbFile.SetMasterKey(key, fakerg.GetNextBytes(key.size()));
        // 1. Serialize
        vector<unsigned char> buff(1024 * 1024);
        uint32_t realSize = 0;
        uint32_t uResult = kbFile.Serialize(&buff[0], buff.size(), realSize);
        BOOST_REQUIRE_EQUAL(uResult, 0);

        // 2. Deserialize (Round-trip)
        CKBFile testKBFile;
        testKBFile.SetMasterKey(key, fakerg.GetNextBytes(key.size()));
        uint32_t cbRealSize = 0;
        uResult = testKBFile.Deserialize(&buff[0], realSize, cbRealSize);
        BOOST_REQUIRE_EQUAL(uResult, 0);

        auto entry = testKBFile.QueryEntryByTitle("GZSerializeTest");
        BOOST_CHECK_EQUAL(entry.GetID() == pwdEntry.GetID(), true);
        BOOST_CHECK_EQUAL(testKBFile.GetHeader().GetCompressFlag(), 1);
    }

BOOST_AUTO_TEST_SUITE_END()