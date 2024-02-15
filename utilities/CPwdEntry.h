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

private:
    std::string m_username;
    std::string m_url;
    CMaskedPwd m_note;
    CMaskedPwd m_password;
    CMaskedPwd m_attachment;

};


#endif //KEYBOX_CPWDENTRY_H
