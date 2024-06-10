//
// Created by tongl on 2/13/2024.
//
#include <boost/test/unit_test.hpp>
#include "../utilities/CKBFile.h"
#include "../utilities/error_code.h"
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

BOOST_AUTO_TEST_SUITE_END()