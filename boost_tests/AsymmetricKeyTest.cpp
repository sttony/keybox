#include <boost/test/unit_test.hpp>
#include <iomanip>
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

static std::string blobToHexString(const std::vector<unsigned char> &sha256_buff) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (auto c: sha256_buff) {
        // Convert each char to its hexadecimal representation
        ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
    }
    return ss.str();
}


BOOST_AUTO_TEST_SUITE(AsymmetricKeyTestSuit)

    BOOST_AUTO_TEST_CASE(GetPubkey) {
        TestRandomGenerator fakeRG;
        CAsymmetricKeyPair asymmetricKeyPair;
        asymmetricKeyPair.ReGenerate();
        auto pubKey = asymmetricKeyPair.GetPublicKey(fakeRG.GetNextBytes(asymmetricKeyPair.GetPublicKeyLength()));
        BOOST_CHECK_EQUAL(pubKey.size(), 800); //RSA4096
    }

    BOOST_AUTO_TEST_CASE(GetPrivkey) {
        TestRandomGenerator fakeRG;
        CAsymmetricKeyPair asymmetricKeyPair;
        asymmetricKeyPair.ReGenerate();
        auto privKey = asymmetricKeyPair.GetPrivateKey(fakeRG.GetNextBytes(asymmetricKeyPair.GetPrivateKeyLength()));
        BOOST_CHECK_EQUAL(privKey.size(), 3272); //RSA4096
    }

    BOOST_AUTO_TEST_CASE(Sign) {
        TestRandomGenerator fakeRG;
        CAsymmetricKeyPair asymmetricKeyPair;
        const char *prv_pem = R"(-----BEGIN PRIVATE KEY-----
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
llHGxoGi26UclGBkJqgFVN4lPiUp
-----END PRIVATE KEY-----)";
        vector<unsigned char> prvkey ( prv_pem, prv_pem+ strlen ( prv_pem ) );

        asymmetricKeyPair.LoadPrivateKey(std::move(prvkey));
        const char* toSign = "this is the data I want to sign";
        vector<unsigned char> data (toSign, toSign + strlen ( toSign ) );
        vector<unsigned char> signature;
        asymmetricKeyPair.Sign(data, signature);
        string hexStr = blobToHexString(signature);
        cout<<hexStr<<endl;
        uint32_t result = asymmetricKeyPair.Verify(data, signature);
        BOOST_CHECK_EQUAL(result, 0);

    }

BOOST_AUTO_TEST_SUITE_END()
