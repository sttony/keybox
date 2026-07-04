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

    boost::property_tree::ptree toJsonObj() const;

    uint32_t fromJsonObj(const boost::property_tree::ptree &);

    const boost::uuids::uuid &GetID() const {
        return m_uuid;
    }

    const std::string &GetTitle() const;

    void SetTitle(const std::string &_title);

    const std::string &GetUserName() const { return m_username; };

    void SetUserName(const std::string &_username) {
        m_username = _username;
        Touch();
    };

    const std::string &GetUrl() const { return m_url; };

    void SetUrl(const std::string &_url) {
        m_url = _url;
        Touch();
    };

    std::string GetPassword() {
        return m_password.Show();
    }

    uint32_t SetPassword(std::string &plain_pwd, std::vector<unsigned char>&& onepad) {
        uint32_t result = m_password.Set(plain_pwd, std::move(onepad));
        if (result == 0) Touch();
        return result;
    }

    void SetPassword(const CMaskedBlob& _maskedblob ) {
        m_password = _maskedblob;
        Touch();
    }

    const CMaskedBlob &GetPwd() const {
        return m_password;
    }

    std::string GetNote() {
        return m_note.Show();
    }

    uint32_t SetNote(std::string &plain_note, std::vector<unsigned char>&& onepad) {
        uint32_t result = m_note.Set(plain_note, std::move(onepad));
        if (result == 0) Touch();
        return result;
    }

    void SetNote(const CMaskedBlob& _maskedblob) {
        m_note = _maskedblob;
        Touch();
    }

    CMaskedBlob &GetN() {
        return m_note;
    }

    std::vector<unsigned char> GetAttachment() {
        return m_attachment.ShowBin();
    }

    uint32_t SetAttachment(std::vector<unsigned char> &blob, std::vector<unsigned char>&& onepad) {
        uint32_t result = m_attachment.Set(blob, std::move(onepad));
        if (result == 0) Touch();
        return result;
    }

    boost::uuids::uuid GetGroup() const;
    void SetGroup(boost::uuids::uuid uuid);

    long long GetNanoTimestamp() const;
    void SetNanoTimestamp(long long ts);

    const std::vector<std::pair<CMaskedBlob, long long>>& GetPasswordHistory() const;
    void SetPasswordHistory(const std::vector<std::pair<CMaskedBlob, long long>>& hist);
    void SetPasswordHistory(std::vector<std::pair<CMaskedBlob, long long>>&& hist);


private:
    static long long CurrentNanoTimestamp();
    void Touch();

    boost::uuids::uuid m_uuid;
    std::string m_title;
    std::string m_username;
    std::string m_url;
    CMaskedBlob m_note;
    CMaskedBlob m_password;
    CMaskedBlob m_attachment;
    boost::uuids::uuid m_group_uuid;
    long long m_nano_timestamp;
    std::vector<std::pair<CMaskedBlob, long long>> m_password_history;
};


#endif //KEYBOX_CPWDENTRY_H
