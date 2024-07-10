#include <boost/test/unit_test.hpp>
#include "../utilities/CAsymmetricKeyPair.h"
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


BOOST_AUTO_TEST_SUITE(AsymmetricKeyTestSuit)

    BOOST_AUTO_TEST_CASE(GetPubkey)
    {
        TestRandomGenerator fakeRG;
        CAsymmetricKeyPair asymmetricKeyPair;
        asymmetricKeyPair.ReGenerate();
        auto pubKey = asymmetricKeyPair.GetPublicKey(fakeRG.GetNextBytes(asymmetricKeyPair.GetPublicKeyLength()));
        BOOST_CHECK_EQUAL(pubKey.size(), 133);  //secp521r1

    }

    BOOST_AUTO_TEST_CASE(GetPrivkey)
    {
        TestRandomGenerator fakeRG;
        CAsymmetricKeyPair asymmetricKeyPair;
        asymmetricKeyPair.ReGenerate();
        auto privKey = asymmetricKeyPair.GetPrivateKey(fakeRG.GetNextBytes(asymmetricKeyPair.GetPrivateKeyLength()));
        BOOST_CHECK_EQUAL(privKey.size(), 66);  //secp521r1

    }
BOOST_AUTO_TEST_SUITE_END()