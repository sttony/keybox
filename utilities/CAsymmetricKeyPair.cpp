
#include <vector>


#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "CAsymmetricKeyPair.h"
#include "error_code.h"
#include "InitGlobalRG.h"
#include "Base64Coder.h"
#include "CipherEngine.h"

using namespace std;

size_t CAsymmetricKeyPair::GetPublicKeyLength() const {
    assert(m_pkey);
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, m_pkey);

    long len = BIO_get_mem_data(bio, nullptr);
    BIO_free(bio);
    return len;
}

CMaskedBlob CAsymmetricKeyPair::GetPublicKey(std::vector<unsigned char> &&onepad) const {
    assert(m_pkey);
    size_t len = GetPublicKeyLength();
    if (len != onepad.size()) {
        return {};
    }
    vector<unsigned char> buff(len);
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, m_pkey);
    char* data = nullptr;
    BIO_get_mem_data(bio, &data);

    CMaskedBlob result;
    result.Set(data, std::move(onepad));
    BIO_free(bio);
    return result;
}

size_t CAsymmetricKeyPair::GetPrivateKeyLength() {
    assert(m_pkey);
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(bio, m_pkey, NULL, NULL, 0, NULL, NULL);

    long len = BIO_get_mem_data(bio, nullptr);
    BIO_free(bio);
    return len;
}

CMaskedBlob CAsymmetricKeyPair::GetPrivateKey(std::vector<unsigned char> &&onepad) {
    assert(m_pkey);
    size_t len = GetPrivateKeyLength();
    if (len != onepad.size()) {
        return {};
    }
    vector<unsigned char> buff(len);
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(bio, m_pkey, NULL, NULL, 0, NULL, NULL);
    char* data = nullptr;
    BIO_get_mem_data(bio, &data);
    CMaskedBlob result;
    result.Set(data, std::move(onepad));
    BIO_free(bio);
    return result;
}

boost::property_tree::ptree CAsymmetricKeyPair::toJsonObj() {
    boost::property_tree::ptree root;
    CMaskedBlob private_key = this->GetPrivateKey( vector<unsigned char>( this->GetPrivateKeyLength(), 0xFF));
    root.add_child("async_pair", private_key.toJsonObj());
    return root;
}

uint32_t CAsymmetricKeyPair::fromJsonObj(const boost::property_tree::ptree & _root) {
    CMaskedBlob private_key;
    private_key.fromJsonObj(_root);
    this->LoadPrivateKey(private_key.ShowBin());
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

uint32_t CAsymmetricKeyPair::Sign(const std::vector<unsigned char> &data, std::vector<unsigned char> &signature) {
    /* Create the EVP_PKEY_CTX context for signing */
    EVP_PKEY_CTX * ctx = EVP_PKEY_CTX_new(m_pkey, nullptr);
    if (!ctx) {
        return ERR_get_error();
    }

    /* Initialize the signing context */
    if (EVP_PKEY_sign_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }

    vector<unsigned char> hash_data;
    CCipherEngine cipher_engine;
    cipher_engine.SHA256(data.data(), data.size(), hash_data);

    size_t signature_len = 0;
    /* Determine the required buffer length for the signature */
    if (EVP_PKEY_sign(ctx, nullptr, &signature_len, hash_data.data(), hash_data.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }
    signature.resize(signature_len);

    /* Perform the signing operation */
    if (EVP_PKEY_sign(ctx, signature.data(), &signature_len, hash_data.data(), hash_data.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }
    EVP_PKEY_CTX_free(ctx);
    return 0;
}

uint32_t CAsymmetricKeyPair::Verify(const std::vector<unsigned char> &data, const std::vector<unsigned char> &signature) {
    /* Create the EVP_PKEY_CTX context for signing */
    EVP_PKEY_CTX * ctx = EVP_PKEY_CTX_new(m_pkey, nullptr);
    if (!ctx) {
        return ERR_get_error();
    }

    /* Initialize the signing context */
    if (EVP_PKEY_verify_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }

    vector<unsigned char> hash_data;
    CCipherEngine cipher_engine;
    cipher_engine.SHA256(data.data(), data.size(), hash_data);

    size_t signature_len = 0;

    /* Perform the verifying operation */
    if (EVP_PKEY_verify(ctx, signature.data(), signature.size(), hash_data.data(), hash_data.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }
    EVP_PKEY_CTX_free(ctx);
    return 0;
}

uint32_t CAsymmetricKeyPair::LoadPrivateKey(vector<unsigned char> &&privKey) {
    BIO *bio = BIO_new_mem_buf(privKey.data(), privKey.size());
    if(bio == nullptr){
        return ERROR_INVALID_PEM;
    }

    if (m_pkey != nullptr) {
        EVP_PKEY_free(m_pkey);
    }

    m_pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);

    if(m_pkey == nullptr){
        BIO_free_all(bio);
        return ERROR_INVALID_PEM;
    }

    BIO_free_all(bio);
    return 0;
}
