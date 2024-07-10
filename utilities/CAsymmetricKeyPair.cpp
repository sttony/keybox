
#include <vector>


#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include "CAsymmetricKeyPair.h"
#include "error_code.h"
#include "InitGlobalRG.h"

using namespace std;

size_t CAsymmetricKeyPair::GetPublicKeyLength() {
    assert(m_pkey);
    size_t len = 0;
    if (EVP_PKEY_get_octet_string_param(m_pkey, OSSL_PKEY_PARAM_PUB_KEY, nullptr, 0, &len) != 1) {
        return 0;
    }
    return len;
}

CMaskedBlob CAsymmetricKeyPair::GetPublicKey(std::vector<unsigned char> &&onepad) {
    assert(m_pkey);
    size_t len = GetPublicKeyLength();
    if (len != onepad.size()) {
        return {};
    }
    vector<unsigned char> buff(len);
    if (EVP_PKEY_get_octet_string_param(m_pkey, OSSL_PKEY_PARAM_PUB_KEY, buff.data(), buff.size(), &len) != 1) {
        return {};
    };

    CMaskedBlob result;
    result.Set(buff, std::move(onepad));
    return result;
}

size_t CAsymmetricKeyPair::GetPrivateKeyLength() {
    assert(m_pkey);
    size_t len = 0;
    BIGNUM *pri_key = nullptr;
    if (EVP_PKEY_get_bn_param(m_pkey, OSSL_PKEY_PARAM_PRIV_KEY, &pri_key) != 1) {
        return 0;
    }

    size_t result = BN_num_bytes(pri_key);
    BN_clear_free(pri_key);
    return result;
}

CMaskedBlob CAsymmetricKeyPair::GetPrivateKey(std::vector<unsigned char> &&onepad) {
    assert(m_pkey);
    size_t len = GetPrivateKeyLength();
    if (len != onepad.size()) {
        return {};
    }
    vector<unsigned char> buff(len);
    BIGNUM *pri_key = nullptr;
    if (EVP_PKEY_get_bn_param(m_pkey, OSSL_PKEY_PARAM_PRIV_KEY, &pri_key) != 1) {
        return {};
    }
    if (!BN_bn2bin(pri_key, buff.data())) {
        return {};
    }
    CMaskedBlob result;
    result.Set(buff, std::move(onepad));
    return result;
}

boost::property_tree::ptree CAsymmetricKeyPair::toJsonObj() {
    return boost::property_tree::ptree();
}

uint32_t CAsymmetricKeyPair::fromJsonObj(const boost::property_tree::ptree &) {
    return 0;
}

uint32_t CAsymmetricKeyPair::ReGenerate() {
    if (m_pkey != nullptr) {
        EVP_PKEY_free(m_pkey);
    }
    m_pkey = EVP_EC_gen("secp521r1");
    if (m_pkey == nullptr) {
        return ERROR_UNEXPECT_OPENSSL_FAILURE;
    }
    return 0;
}

CAsymmetricKeyPair::~CAsymmetricKeyPair() {
    if (m_pkey != nullptr) {
        EVP_PKEY_free(m_pkey);
    }
}