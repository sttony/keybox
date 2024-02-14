//
// Created by tongl on 2/14/2024.
//
#include <boost/test/unit_test.hpp>
#include "../utilities/CMaskedPwd.h"
#include "../utilities/RandomGenerator.h"
#include <iomanip>
#include <iostream>

using namespace std;

class TestRandomGenerator : public IRandomGenerator {
public:
    uint32_t GetNextBytes(uint32_t num, std::vector<unsigned char>& output) override{
        output.resize(num);
        for(int i =0; i<num; ++i){
            output[i] = (i+1)%256;
        }
        return 0;
    }
};

BOOST_AUTO_TEST_SUITE(CMaskedPwdTestSuit)

BOOST_AUTO_TEST_CASE(TestShow){

    string plainword = "\x1\x2\x3\x4";
    TestRandomGenerator fakeRG;
    CMaskedPwd spwd(plainword, fakeRG);
    BOOST_CHECK_EQUAL(plainword, string(4, 0));
    BOOST_CHECK_EQUAL(spwd.Show(), "\x1\x2\x3\x4");
}


BOOST_AUTO_TEST_SUITE_END()