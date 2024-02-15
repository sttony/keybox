//
// Created by tongl on 12/19/2023.
//
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../utilities/Base64Coder.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(Base64TestSuit)
    BOOST_AUTO_TEST_CASE(decode)
    {
        Base64Coder base64Coder;
        vector<unsigned char> output1;
        base64Coder.Decode("aGVsbG8gd29ybGQh", output1);
        BOOST_CHECK_EQUAL(string((char*)&output1[0], output1.size()), "hello world!");


        vector<unsigned char> output2;
        base64Coder.Decode("AQIDBAUA", output2);
        BOOST_CHECK_EQUAL(output2.size(), 6);
        BOOST_CHECK_EQUAL(output2[0], 0x01);
        BOOST_CHECK_EQUAL(output2[1], 0x02);
        BOOST_CHECK_EQUAL(output2[2], 0x03);
        BOOST_CHECK_EQUAL(output2[3], 0x04);
        BOOST_CHECK_EQUAL(output2[4], 0x05);
        BOOST_CHECK_EQUAL(output2[5], 0x00);
    }
    BOOST_AUTO_TEST_CASE(encode)
    {
        Base64Coder base64Coder;
        string output ;
        base64Coder.Encode((unsigned char*)"hello world!", sizeof("hello world!")-1, output);
        BOOST_CHECK_EQUAL(output, "aGVsbG8gd29ybGQh");

        vector<unsigned char> input1 = {0x01, 0x02, 0x03, 0x04, 0x05, 0x00};
        output="";
        base64Coder.Encode(&input1[0], 6, output);
        BOOST_CHECK_EQUAL(output, "AQIDBAUA");
    }


BOOST_AUTO_TEST_SUITE_END()