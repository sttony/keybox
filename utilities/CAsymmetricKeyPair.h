

#ifndef KEYBOX_CASYMMETRICKEYPAIR_H
#define KEYBOX_CASYMMETRICKEYPAIR_H

#include <vector>
#include <openssl/types.h>
#include "CMaskedBlob.h"

class CAsymmetricKeyPair {
public:
    CMaskedBlob GetPublicKey(std::vector<unsigned char>&& onepad) const;
    size_t GetPublicKeyLength() const;
    CMaskedBlob GetPrivateKey(std::vector<unsigned char>&& onepad);
    size_t GetPrivateKeyLength();

    uint32_t LoadPrivateKey(std::vector<unsigned char>&& privKey);

    uint32_t ReGenerate();

    boost::property_tree::ptree toJsonObj();
    uint32_t fromJsonObj(const boost::property_tree::ptree &);
    ~CAsymmetricKeyPair();

    uint32_t Sign( const std::vector<unsigned char>& data, std::vector<unsigned char>& signature);
    uint32_t Sign( const unsigned char* data, uint32_t data_size, std::vector<unsigned char>& signature);
    uint32_t Verify( const std::vector<unsigned char>& data, const std::vector<unsigned char>& signature);

    uint32_t Encrypt(const std::vector<unsigned char>& data, std::vector<unsigned char>& cipher);
    uint32_t Decrypt(const std::vector<unsigned char>& cipher, std::vector<unsigned char>& data);
private:
    EVP_PKEY* m_pkey = nullptr;
};


#endif //KEYBOX_CASYMMETRICKEYPAIR_H
