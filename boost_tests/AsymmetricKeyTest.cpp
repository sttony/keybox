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
        BOOST_CHECK_EQUAL(pubKey.size(), 133); //secp521r1
    }

    BOOST_AUTO_TEST_CASE(GetPrivkey) {
        TestRandomGenerator fakeRG;
        CAsymmetricKeyPair asymmetricKeyPair;
        asymmetricKeyPair.ReGenerate();
        auto privKey = asymmetricKeyPair.GetPrivateKey(fakeRG.GetNextBytes(asymmetricKeyPair.GetPrivateKeyLength()));
        BOOST_CHECK_EQUAL(privKey.size(), 66); //secp521r1
    }

    BOOST_AUTO_TEST_CASE(Sign) {
        TestRandomGenerator fakeRG;
        CAsymmetricKeyPair asymmetricKeyPair;
        const char *prv_pem = R"(-----BEGIN EC PRIVATE KEY-----
MIHcAgEBBEIAUd3NsMELo92RanW5A/+jJVhoaAV1hIf2LvyO4pI68y3kt2tFG06d
Qc/AFOVUNLmvQaKHUa+ldTADt90BKY1heh+gBwYFK4EEACOhgYkDgYYABADh9fwj
QxqSW6FaRL0Yx4kWhKOiI8eE2RxbqrGKcBeh2csEJ6NvFUwEmiWF5gkJnqCNpbY/
fKk9x5trNKNDbLkRLwDqG6dUe+xm34+alqJ/nKtN5J8vr7qsi9PIjHqdVNTQhxwX
RQum7J2hhlcydAH6DYO2Wj3NHEBemYs1obfMmAK0dg==
-----END EC PRIVATE KEY-----)";
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
