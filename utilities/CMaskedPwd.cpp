//
// Created by tongl on 2/14/2024.
//

#include "CMaskedPwd.h"
#include "Base64Coder.h"
using namespace std;

CMaskedPwd::~CMaskedPwd(){
    memset( &m_password[0], 0, m_password.size());
    memset( &m_onepad[0], 0, m_onepad.size());
}

void CMaskedPwd::Set(std::string &plainPassword, IRandomGenerator& randomGenerator) {
    randomGenerator.GetNextBytes(plainPassword.size(), m_onepad);
    m_password = vector<unsigned char>(plainPassword.size(), '*');
    for(int i=0; i< m_password.size(); ++i){
        m_password[i] = plainPassword[i] ^ m_onepad[i];
    }
    memset(&plainPassword[0], 0, plainPassword.size());
}

string CMaskedPwd::Show() {
    string result(m_password.size(), '*');
    for(int i=0; i< result.size(); ++i){
        result[i] = m_password[i] ^ m_onepad[i];
    }
    return result;
}

boost::property_tree::ptree CMaskedPwd::toJsonObj() {
    boost::property_tree::ptree root;
    Base64Coder base64Coder;
    string encoded_password;
    base64Coder.Encode(&m_password[0], m_password.size(), encoded_password);
    root.put("masked",  encoded_password);

    string encoded_onepad;
    base64Coder.Encode(&m_onepad[0], m_onepad.size(), encoded_onepad);
    root.put("onepad",  encoded_onepad);
    return root;
}
