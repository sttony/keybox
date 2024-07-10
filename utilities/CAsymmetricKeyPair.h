

#ifndef KEYBOX_CASYMMETRICKEYPAIR_H
#define KEYBOX_CASYMMETRICKEYPAIR_H


#include "CMaskedBlob.h"

class CAsymmetricKeyPair {
public:
    CMaskedBlob GetPublicKey();
    CMaskedBlob GetPrivateKey();

    boost::property_tree::ptree toJsonObj();
    uint32_t fromJsonObj(const boost::property_tree::ptree &);

};


#endif //KEYBOX_CASYMMETRICKEYPAIR_H
