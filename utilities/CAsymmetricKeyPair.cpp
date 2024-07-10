

#include "CAsymmetricKeyPair.h"

CMaskedBlob CAsymmetricKeyPair::GetPublicKey() {
    return CMaskedBlob();
}

CMaskedBlob CAsymmetricKeyPair::GetPrivateKey() {
    return CMaskedBlob();
}

boost::property_tree::ptree CAsymmetricKeyPair::toJsonObj() {
    return boost::property_tree::ptree();
}

uint32_t CAsymmetricKeyPair::fromJsonObj(const boost::property_tree::ptree &) {
    return 0;
}
