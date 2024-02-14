//
// Created by tongl on 2/13/2024.
//
#include <boost/test/unit_test.hpp>
#include "../utilities/CKBFile.h"
#include "../utilities/error_code.h"
#include <iomanip>
#include <iostream>
using namespace std;

BOOST_AUTO_TEST_SUITE(CKBFileTestSuit)

    BOOST_AUTO_TEST_CASE(HeaderReadWrite)
    {
        CKBFileHeader ckbFileHeader;
        uint32_t cbSize;
        vector<unsigned char> buff;
        ckbFileHeader.GetDerivativeParameters().num_rounds = 1000;
        ckbFileHeader.GetDerivativeParameters().Salt[0] = 0x13;

        uint32_t hresult = ckbFileHeader.Serialize(nullptr, 0, cbSize);

        BOOST_CHECK_EQUAL(hresult, ERROR_BUFFER_TOO_SMALL);
        buff.resize(cbSize);

        hresult = ckbFileHeader.Serialize(&buff[0], buff.size(), cbSize);
        BOOST_CHECK_EQUAL(hresult, 0);

        CKBFileHeader newHeader ;
        newHeader.Deserialize(&buff[0], buff.size());

        BOOST_CHECK_EQUAL(newHeader.GetDerivativeParameters().num_rounds, 1000);
        BOOST_CHECK_EQUAL(newHeader.GetDerivativeParameters().Salt[0], 0x13);


    }
BOOST_AUTO_TEST_SUITE_END()