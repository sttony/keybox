//
// Created by tongl on 2/14/2024.
//

#ifndef KEYBOX_CMASKEDBLOB_H
#define KEYBOX_CMASKEDBLOB_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "CRandomGenerator.h"

class CMaskedBlob {
public:
    CMaskedBlob();

    size_t size() const { return m_password.size(); };

    std::string Show();

    std::vector<unsigned char> ShowBin();

    uint32_t Set(std::string& plainPassword, std::vector<unsigned char>&& onepad);

    uint32_t Set(std::string&& plainPassword, std::vector<unsigned char>&& onepad);

    uint32_t Set(std::vector<unsigned char> &plainPassword, std::vector<unsigned char>&& onepad);

    boost::property_tree::ptree toJsonObj();

    uint32_t fromJsonObj(const boost::property_tree::ptree &);

    CMaskedBlob(std::string& plainPassword, std::vector<unsigned char>&& onepad) {
        this->Set(plainPassword, std::move(onepad));
    }

    ~CMaskedBlob();

private:
    std::vector<unsigned char> m_password;
    std::vector<unsigned char> m_onepad;
};

#endif //KEYBOX_CMASKEDBLOB_H
