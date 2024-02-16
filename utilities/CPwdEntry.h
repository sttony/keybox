//
// Created by tongl on 2/13/2024.
//

#ifndef KEYBOX_CPWDENTRY_H
#define KEYBOX_CPWDENTRY_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "CMaskedBlob.h"

class CPwdEntry{
public:
    boost::property_tree::ptree toJsonObj();
    const std::string& GetTitle() { return m_title;};
    void SetTitle(const std::string& _title) {
        m_title = _title;
    };

    const std::string& GetUserName() { return m_username;};
    void SetUserName(const std::string& _username) {
        m_username = _username;
    };

    const std::string& GetUrl() { return m_url;};
    void SetUrl(const std::string& _url){
      m_url = _url;
    };

    std::string GetPassword(){
        return m_password.Show();
    }
    uint32_t SetPassword(std::string& plain_pwd, IRandomGenerator& iRandomGenerator){
        return m_password.Set(plain_pwd, iRandomGenerator);
    }

    std::string GetNote(){
        return m_note.Show();
    }
    uint32_t SetNote(std::string& plain_note, IRandomGenerator& iRandomGenerator){
        return m_note.Set(plain_note, iRandomGenerator);
    }

    std::vector<unsigned char> GetAttachment(){
        return m_attachment.ShowBin();
    }
    uint32_t SetAttachment(std::vector<unsigned char>& blob, IRandomGenerator& iRandomGenerator){
        return m_attachment.Set(blob, iRandomGenerator);
    }

private:
    std::string  m_title;
    std::string m_username;
    std::string m_url;
    CMaskedBlob m_note;
    CMaskedBlob m_password;
    CMaskedBlob m_attachment;
    std::vector<CMaskedBlob> m_password_history;
};


#endif //KEYBOX_CPWDENTRY_H
