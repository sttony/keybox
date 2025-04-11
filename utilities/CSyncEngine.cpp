#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "error_code.h"
#include "CAsymmetricKeyPair.h"
#include "CSyncEngine.h"
#include "CRequest.h"

using namespace std;

uint32_t CSyncEngine::Register(){
    // generate async pair
    CAsymmetricKeyPair asymmetricKeyPair;
    asymmetricKeyPair.ReGenerate();

    // Add pub key to head

    // Add prv key to encrypt area

    // send pubkey, email to url
    CMaskedBlob pubKey = asymmetricKeyPair.GetPublicKey( vector<unsigned char>(asymmetricKeyPair.GetPublicKeyLength()));
    CRequest request(m_sync_url+"/" + "register", CRequest::POST);
    boost::property_tree::ptree pay_load;
    pay_load.put("pubKey", pubKey.Show());
    pay_load.put("email", m_sync_email);
    std::ostringstream oss;
    boost::property_tree::write_json(oss, pay_load);
    request.SetPayload(oss.str());
    request.Send();

    if (request.GetResponseCode() == 200) {
        return 0;
    }
    return request.GetResponseCode() | ERROR_HTTP_ERROR_PREFIX;
}

uint32_t CSyncEngine::FinishRegister() {


    CRequest request(m_sync_url+"/" + "check_status", CRequest::POST);
    request.Send();

    if (request.GetResponseCode() == 200) {
        return 0;
    }
    return request.GetResponseCode() | ERROR_HTTP_ERROR_PREFIX;
}

void CSyncEngine::Unregister() {
}

uint32_t CSyncEngine::Sync(CKBFile* pKBFile) {
    // check if pubkey exists, if not exist,  try to register
    assert(pKBFile);
    CAsymmetricKeyPair asymmetricKeyPair;
    pKBFile->GetHeader();

    //
    // CRequest request(m_sync_url+"/" + "retrieve", CRequest::POST);
    // boost::property_tree::ptree pay_load;
    // pay_load.put("email", m_sync_email);
    // std::ostringstream oss;
    // boost::property_tree::write_json(oss, pay_load);
    // request.SetPayload(oss.str());
    // request.Send();




    return 0;
}


CSyncEngine::~CSyncEngine()= default;
