//
// Created by tongl on 2/13/2024.
//

#ifndef KEYBOX_CPWDENTRY_H
#define KEYBOX_CPWDENTRY_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/uuid.hpp>
#include "CMaskedBlob.h"

class CPwdEntry {
public:
    CPwdEntry();

    CPwdEntry(boost::uuids::uuid _uuid);

    boost::property_tree::ptree toJsonObj();

    uint32_t fromJsonObj(const boost::property_tree::ptree &);

    const boost::uuids::uuid &GetID() const {
        return m_uuid;
    }

    const std::string &GetTitle() const;

    void SetTitle(const std::string &_title);

    const std::string &GetUserName() const { return m_username; };

    void SetUserName(const std::string &_username) {
        m_username = _username;
    };

    const std::string &GetUrl() const { return m_url; };

    void SetUrl(const std::string &_url) {
        m_url = _url;
    };

    std::string GetPassword() {
        return m_password.Show();
    }

    uint32_t SetPassword(std::string &plain_pwd, std::vector<unsigned char>&& onepad) {
        return m_password.Set(plain_pwd, std::move(onepad));
    }

    void SetPassword(const CMaskedBlob& _maskedblob ) {
        m_password = _maskedblob;
    }

    const CMaskedBlob &GetPwd() const {
        return m_password;
    }

    std::string GetNote() {
        return m_note.Show();
    }

    uint32_t SetNote(std::string &plain_note, std::vector<unsigned char>&& onepad) {
        return m_note.Set(plain_note, std::move(onepad));
    }

    void SetNote(const CMaskedBlob& _maskedblob) {
        m_note = _maskedblob;
    }

    CMaskedBlob &GetN() {
        return m_note;
    }

    std::vector<unsigned char> GetAttachment() {
        return m_attachment.ShowBin();
    }

    uint32_t SetAttachment(std::vector<unsigned char> &blob, std::vector<unsigned char>&& onepad) {
        return m_attachment.Set(blob, std::move(onepad));
    }

    boost::uuids::uuid GetGroup() const;
    void SetGroup(boost::uuids::uuid uuid);

private:
    boost::uuids::uuid m_uuid;
    std::string m_title;
    std::string m_username;
    std::string m_url;
    CMaskedBlob m_note;
    CMaskedBlob m_password;
    CMaskedBlob m_attachment;
    boost::uuids::uuid m_group_uuid;
    std::vector<CMaskedBlob> m_password_history;
};


#endif //KEYBOX_CPWDENTRY_H
