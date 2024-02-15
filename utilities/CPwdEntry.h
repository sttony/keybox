//
// Created by tongl on 2/13/2024.
//

#ifndef KEYBOX_CPWDENTRY_H
#define KEYBOX_CPWDENTRY_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "CMaskedPwd.h"

class CPwdEntry{
public:
    boost::property_tree::ptree toJsonObj();
    const std::string& GetUserName() { return m_username;};
    void SetUserName(const std::string& _username) {
        m_username = _username;
    };

    const std::string& GetUrl() { return m_url;};
    void SetUrl(const std::string& _url){
      m_url = _url;
    };



private:
    std::string m_username;
    std::string m_url;
    CMaskedPwd m_note;
    CMaskedPwd m_password;
    CMaskedPwd m_attachment;
    std::vector<CMaskedPwd> m_password_history;
};


#endif //KEYBOX_CPWDENTRY_H
