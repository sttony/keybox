#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "error_code.h"
#include "CAsymmetricKeyPair.h"
#include "CSyncEngine.h"
#include "CRequest.h"

using namespace std;

uint32_t CSyncEngine::Register(){
    // read url from kb file
    string sync_url = m_pKbfile->GetHeader().GetSyncUrl();
    // read email from kb file
    string sync_email = m_pKbfile->GetHeader().GetSyncEmail();
    // generate async pair
    CAsymmetricKeyPair asymmetricKeyPair;
    asymmetricKeyPair.ReGenerate();

    // Add pub key to head

    // Add prv key to encrypt area

    // send pubkey, email to url
    CMaskedBlob pubKey = asymmetricKeyPair.GetPublicKey( vector<unsigned char>(asymmetricKeyPair.GetPublicKeyLength()));
    CRequest request(sync_url+"/" + "register", CRequest::POST);
    boost::property_tree::ptree pay_load;
    pay_load.put("pubKey", pubKey.Show());
    pay_load.put("email", sync_email);
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
    // read url from kb file
    string sync_url = m_pKbfile->GetHeader().GetSyncUrl();
    // read email from kb file
    string sync_email = m_pKbfile->GetHeader().GetSyncEmail();
    CRequest request(sync_url+"/" + "check_status", CRequest::POST);
    request.Send();

    if (request.GetResponseCode() == 200) {
        return 0;
    }
    return request.GetResponseCode() | ERROR_HTTP_ERROR_PREFIX;
}

void CSyncEngine::Unregister() {
}

void CSyncEngine::Sync() {


}


CSyncEngine::~CSyncEngine(){

}
