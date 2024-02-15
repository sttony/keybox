//
// Created by tongl on 2/14/2024.
//

#ifndef KEYBOX_CMASKEDPWD_H
#define KEYBOX_CMASKEDPWD_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "RandomGenerator.h"

class CMaskedPwd {
public:
    size_t size() { return m_password.size(); };
    std::string Show();
    void Set(std::string& plainPassword, IRandomGenerator& randomGenerator);
    boost::property_tree::ptree toJsonObj();

    CMaskedPwd(std::string& plainPassword, IRandomGenerator& randomGenerator){
        Set(plainPassword, randomGenerator);
    }

    ~CMaskedPwd();

private:
    std::vector<unsigned char> m_password;
    std::vector<unsigned char> m_onepad;
};

#endif //KEYBOX_CMASKEDPWD_H
