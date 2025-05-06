
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

    BUF_MEM* buf;
    BIO_get_mem_ptr(bio, &buf);
    const size_t len = buf->length;
    BIO_free(bio);
    return len;
}

CMaskedBlob CAsymmetricKeyPair::GetPublicKey(std::vector<unsigned char> &&onepad) const {
    assert(m_pkey);
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, m_pkey);

    BUF_MEM* buf;
    BIO_get_mem_ptr(bio, &buf);
    const size_t len = buf->length;

    if (len != onepad.size()) {
        BIO_free(bio);
        return {};
    }
    vector<unsigned char> buff(len);

    CMaskedBlob result;
    result.Set(string(buf->data, len), std::move(onepad));
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
    result.Set(string(data, len), std::move(onepad));
    BIO_free(bio);
    return result;
}

boost::property_tree::ptree CAsymmetricKeyPair::toJsonObj() {
    boost::property_tree::ptree root;
    CMaskedBlob private_key = this->GetPrivateKey( g_RG.GetNextBytes(this->GetPrivateKeyLength()));
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
        m_pkey = nullptr;
    }
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 4096);
    EVP_PKEY_keygen(ctx, &m_pkey);
    if (m_pkey == nullptr) {
        EVP_PKEY_CTX_free(ctx);
        return ERROR_UNEXPECT_OPENSSL_FAILURE;
    }
    EVP_PKEY_CTX_free(ctx);
    return 0;
}

CAsymmetricKeyPair::~CAsymmetricKeyPair() {
    if (m_pkey != nullptr) {
        EVP_PKEY_free(m_pkey);
        m_pkey = nullptr;
    }
}

uint32_t CAsymmetricKeyPair::Sign(const std::vector<unsigned char> &data, std::vector<unsigned char> &signature) {
    return Sign(data.data(), data.size(), signature);
}

uint32_t CAsymmetricKeyPair::Sign(const unsigned char *data, uint32_t data_size,
    std::vector<unsigned char> &signature) {
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
    cipher_engine.SHA256(data, data_size, hash_data);

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

uint32_t CAsymmetricKeyPair::Encrypt(const std::vector<unsigned char> &data, std::vector<unsigned char> &cipher) {
    assert(m_pkey); // Ensure the key pair is valid

    // Create a context for encryption
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(m_pkey, nullptr);
    if (!ctx) {
        return ERR_get_error(); // Return an error if context creation fails
    }

    // Initialize the encryption operation
    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }

    // Determine the buffer length needed for encryption
    size_t cipher_len = 0;
    if (EVP_PKEY_encrypt(ctx, nullptr, &cipher_len, data.data(), data.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }

    // Resize the output vector to hold the encrypted data
    cipher.resize(cipher_len);

    // Perform the encryption
    if (EVP_PKEY_encrypt(ctx, cipher.data(), &cipher_len, data.data(), data.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }

    EVP_PKEY_CTX_free(ctx); // Free the encryption context
    return 0; // Return success

}

uint32_t CAsymmetricKeyPair::Decrypt(const std::vector<unsigned char> &cipher, std::vector<unsigned char> &data) {
    assert(m_pkey); // Ensure the private key is valid

    // Create a context for decryption
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(m_pkey, nullptr);
    if (!ctx) {
        return ERR_get_error();
    }

    // Initialize the decryption operation
    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }

    // Determine the buffer size required for the plaintext
    size_t data_len = 0;
    if (EVP_PKEY_decrypt(ctx, nullptr, &data_len, cipher.data(), cipher.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }

    // Resize the output vector to hold the decrypted data
    data.resize(data_len);

    // Perform the decryption
    if (EVP_PKEY_decrypt(ctx, data.data(), &data_len, cipher.data(), cipher.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return ERR_get_error();
    }

    EVP_PKEY_CTX_free(ctx); // Free resources
    return 0; // Return success

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
