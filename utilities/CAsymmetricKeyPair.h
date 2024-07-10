

#ifndef KEYBOX_CASYMMETRICKEYPAIR_H
#define KEYBOX_CASYMMETRICKEYPAIR_H

#include <vector>
#include <openssl/types.h>
#include "CMaskedBlob.h"

class CAsymmetricKeyPair {
public:
    CMaskedBlob GetPublicKey(std::vector<unsigned char>&& onepad);
    size_t GetPublicKeyLength();
    CMaskedBlob GetPrivateKey(std::vector<unsigned char>&& onepad);
    size_t GetPrivateKeyLength();

    uint32_t ReGenerate();

    boost::property_tree::ptree toJsonObj();
    uint32_t fromJsonObj(const boost::property_tree::ptree &);
    ~CAsymmetricKeyPair();
private:
    EVP_PKEY* m_pkey = nullptr;
};


#endif //KEYBOX_CASYMMETRICKEYPAIR_H
