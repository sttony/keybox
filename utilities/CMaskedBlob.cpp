//
// Created by tongl on 2/14/2024.
//

#include "CMaskedBlob.h"
#include "Base64Coder.h"
#include "error_code.h"
using namespace std;

CMaskedBlob::~CMaskedBlob(){
    if( m_password.size()>0)
        memset( &m_password[0], 0, m_password.size());
    if(m_onepad.size()>0)
        memset( &m_onepad[0], 0, m_onepad.size());
}

uint32_t CMaskedBlob::Set(std::string &plainPassword, IRandomGenerator& randomGenerator) {
    if(randomGenerator.GetNextBytes(plainPassword.size(), m_onepad)){
        return ERROR_UNEXPECT_RG_FAILURE;
    };
    m_password = vector<unsigned char>(plainPassword.size(), '*');
    for(int i=0; i< m_password.size(); ++i){
        m_password[i] = plainPassword[i] ^ m_onepad[i];
    }
    memset(&plainPassword[0], 0, plainPassword.size());
    return 0;
}

uint32_t CMaskedBlob::Set(vector<unsigned char> &plainPassword, IRandomGenerator& randomGenerator) {
    if(randomGenerator.GetNextBytes(plainPassword.size(), m_onepad)){
        return ERROR_UNEXPECT_RG_FAILURE;
    };
    m_password = vector<unsigned char>(plainPassword.size(), '*');
    for(int i=0; i< m_password.size(); ++i){
        m_password[i] = plainPassword[i] ^ m_onepad[i];
    }
    memset(&plainPassword[0], 0, plainPassword.size());
    return 0;
}

string CMaskedBlob::Show() {
    string result(m_password.size(), '*');
    for(size_t i=0; i< result.size(); ++i){
        result[i] = m_password[i] ^ m_onepad[i];
    }
    return result;
}

boost::property_tree::ptree CMaskedBlob::toJsonObj() {
    boost::property_tree::ptree root;
    Base64Coder base64Coder;
    string encoded_password;
    if( m_password.size()>0) {
        base64Coder.Encode(&m_password[0], m_password.size(), encoded_password);
        root.put("masked", encoded_password);

        string encoded_onepad;
        base64Coder.Encode(&m_onepad[0], m_onepad.size(), encoded_onepad);
        root.put("onepad", encoded_onepad);
    }
    return root;
}

std::vector<unsigned char> CMaskedBlob::ShowBin() {
    vector<unsigned char> result(m_password.size(), '*');
    for(size_t i=0; i< result.size(); ++i){
        result[i] = m_password[i] ^ m_onepad[i];
    }
    return result;
}

CMaskedBlob::CMaskedBlob() {
    m_password = {};
    m_onepad ={};
}

uint32_t CMaskedBlob::fromJsonObj(const boost::property_tree::ptree &jsonObj) {
    Base64Coder base64Coder;
    // Get password and onepad from json and decode them
    std::string encoded_password = jsonObj.get<std::string>("masked", "");
    std::string encoded_onepad = jsonObj.get<std::string>("onepad", "");

    if (!base64Coder.Decode(encoded_password, m_password) ||
        !base64Coder.Decode(encoded_onepad, m_onepad)) {
        return ERROR_INVALID_BASE64; // Some predefined error code.
    }
    return 0;
}
